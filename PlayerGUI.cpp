/*
  ==============================================================================

    PlayerGUI.cpp
    Created: 17 Oct 2025 2:21:10pm
    Author:  RTX

  ==============================================================================
*/

#include "PlayerGUI.h"

PlayerGUI::PlayerGUI()
{
    for (auto* btn : { &loadButton, &restartButton, &stopButton, &playButton ,&loopButton,&muteButton,&unmuteButton, &forwardButton , &backwardButton,&setAButton,&setBButton,&loopABButton, &prevTrackButton, &nextTrackButton, &playlistToggleButton ,&goToEnd })
    {
        addAndMakeVisible(btn); 
        btn->addListener(this);
    }

    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);
    speedSlider.setRange(0.5, 2.0, 0.01); 
    speedSlider.setValue(1.0); 
    speedSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20); 
    speedSlider.setTextValueSuffix("x"); 
    speedSlider.addListener(this); 
    addAndMakeVisible(speedSlider);
    speedLabel.setText("Speed", juce::dontSendNotification); 
    speedLabel.setJustificationType(juce::Justification::centred); 
    addAndMakeVisible(speedLabel); 
    progressSlider.setRange(0.0, 1.0);  
    progressSlider.setSliderStyle(juce::Slider::LinearHorizontal); 
    progressSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0); 
    addAndMakeVisible(progressSlider);
    progressSlider.addListener(this); 
    addAndMakeVisible(timeLabel);
    addAndMakeVisible(metadataLabel);
    addAndMakeVisible(setAButton);
    addAndMakeVisible(setBButton);
    addAndMakeVisible(loopABButton);

    timeLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    timeLabel.setJustificationType(juce::Justification::centred);
    metadataLabel.setText("No file loaded", juce::dontSendNotification);
    metadataLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    metadataLabel.setJustificationType(juce::Justification::centred);
    startTimer(10);

    // playlist box
    addAndMakeVisible(playlistBox);
    playlistBox.setModel(this);
    playlistBox.setMultipleSelectionEnabled(false);
    playlistBox.setRowHeight(24);
    playlistBox.setVisible(playlistVisible);
}

PlayerGUI::~PlayerGUI() {}

void PlayerGUI::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    playerAudio.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerGUI::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    playerAudio.getNextAudioBlock(bufferToFill);
  
}

void PlayerGUI::releaseResources()
{
    playerAudio.releaseResources();
}

void PlayerGUI::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider)
        playerAudio.setGain((float)slider->getValue());
  else if (slider == &progressSlider)
{

    double length = playerAudio.getLength();
    if (length > 0.0)
    {
        double newPos =progressSlider.getValue() * length;
        playerAudio.setPosition(newPos);
    }
}
  else if (slider == &speedSlider) 
{
    playerAudio.setPlaybackRate((float)slider->getValue());
}
}

void PlayerGUI::resized()
{
  int margin = 20;
  int buttonHeight = 40;
  int buttonSpace = 10;
  int buttonWidth = 100;
  int numButtons = 16;
  int labelHeight = static_cast<int>(4 * (metadataLabel.getFont().getHeight() + 10));

  
  int minPlaylistWidth = 220;
  int playlistWidth = getWidth() / 3;
  if (playlistWidth < minPlaylistWidth) playlistWidth = minPlaylistWidth;
  if (playlistWidth > getWidth() - 200) playlistWidth = getWidth() - 200;

  int playlistX = getWidth() - margin - playlistWidth;
  int playlistY = margin;
  int playlistH = getHeight() - 2 * margin;

  playlistBox.setBounds(playlistX, playlistY, playlistWidth, playlistH);
  playlistBox.setVisible(playlistVisible);

  int controlsX = margin;
  int controlsY = margin;
  int controlsW = getWidth() - playlistWidth - 3 * margin;

  // عنوان البيانات (metadata)
  metadataLabel.setBounds(controlsX, controlsY, controlsW, labelHeight);

  // بداية ترتيب الأزرار بعد الميتاداتا
  int buttonsStartY = controlsY + labelHeight + 10;

  // نحسب عدد الأعمدة اللي ممكن تدخل في المساحة
  int cols = controlsW / (buttonWidth + buttonSpace);
  if (cols < 1) cols = 1;
  int rowButtons = cols;

  // ترتيب كل الأزرار
  juce::Component* buttons[] = {
      &loadButton, &restartButton, &stopButton, &playButton,
      &loopButton, &muteButton, &unmuteButton, &forwardButton,
      &backwardButton, &setAButton, &setBButton, &loopABButton
      ,& prevTrackButton,& nextTrackButton,& playlistToggleButton, &goToEnd
  };

  for (int i = 0; i < numButtons; i++)
  {
    int row = i / rowButtons;
    int col = i % rowButtons;
    int bx = controlsX + col * (buttonWidth + buttonSpace);
    int by = buttonsStartY + row * (buttonHeight + 10);
    buttons[i]->setBounds(bx, by, buttonWidth, buttonHeight);
  }

  // بعد الزرائر
  int totalRows = (numButtons + rowButtons - 1) / rowButtons;
  int afterButtonsY = buttonsStartY + totalRows * (buttonHeight + 10) + 10;

  // --- الـ Sliders ---
  volumeSlider.setBounds(controlsX, afterButtonsY, controlsW, 30);
  progressSlider.setBounds(controlsX, afterButtonsY + 40, controlsW, 20);
  speedLabel.setBounds(controlsX, afterButtonsY + 70, controlsW, 20);
  speedSlider.setBounds(controlsX, afterButtonsY + 100, controlsW, 25);
  timeLabel.setBounds(controlsX, afterButtonsY + 130, controlsW, 25);
}


void PlayerGUI::buttonClicked(juce::Button* button)
{
    if (button == &loadButton)
    {
        fileChooser = std::make_unique<juce::FileChooser>(
            "Select an audio file...",
            juce::File{},
            "*.wav;*.mp3");

        fileChooser->launchAsync(
          juce::FileBrowserComponent::openMode |
          juce::FileBrowserComponent::canSelectFiles |
          juce::FileBrowserComponent::canSelectMultipleItems,
            [this](const juce::FileChooser& fc)
            {
            auto results = fc.getResults();
            if (results.size() > 0)
            {
              for (const auto& f : results)
              {
                if (f.existsAsFile())
                  playlist.add(f);
              }

              playlistBox.updateContent();

              // if nothing is playing, select and play the first newly added file
              if (currentIndex == -1 && playlist.size() > 0)
              {
                currentIndex = 0;
                playlistBox.selectRow(currentIndex);
                playFileAtIndex(currentIndex);
              }

              // update metadata label simply with count
              metadataLabel.setText(juce::String(playlist.size()) + " file(s) in playlist", juce::dontSendNotification);
            }
            });

    }
    else if (button == &restartButton)
    {
        playerAudio.setPosition(0.0);
        playerAudio.play();
        playButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green); 
        stopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::grey);
    }
    else if (button == &stopButton)
    {
        playerAudio.stop();
        stopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red); 
        playButton.setColour(juce::TextButton::buttonColourId, juce::Colours::grey);
        
    }
    else if (button == &playButton)
    {
        playerAudio.play();
        playButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green); 
        stopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::grey);
    }
    else if (button == &muteButton)
    {
        playerAudio.mute();
        muteButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red); 
        unmuteButton.setColour(juce::TextButton::buttonColourId, juce::Colours::grey);
    }
    else if (button == &unmuteButton)
    {
        playerAudio.unmute();
        unmuteButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green); 
        muteButton.setColour(juce::TextButton::buttonColourId, juce::Colours::grey);
    }
    else if (button == &forwardButton)
    {
        playerAudio.skipForward(10.0); 
    }
    else if (button == &backwardButton)
    {
        playerAudio.skipBackward(10.0); 
    }
      else if (button == &loopButton)
    {
        loopOn = !loopOn;
        if (loopOn)
            loopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
        else
            loopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::grey);
        playerAudio.setLooping(loopOn);
    }
 else if (button == &setAButton)
 {
     playerAudio.setLoopPointA(playerAudio.getPosition());
 }
 else if (button == &setBButton)
  {
     playerAudio.setLoopPointB(playerAudio.getPosition());
  }
 else if (button == &loopABButton)
  {
     playerAudio.enableSegmentLoop(!playerAudio.getSegmentLooping());
     isLoopingAB = !isLoopingAB;
  }

 else if (button == &nextTrackButton)
    {
      if (playlist.size() > 0)
      {
        currentIndex = (currentIndex + 1 + playlist.size()) % playlist.size();
        playlistBox.selectRow(currentIndex);
        playFileAtIndex(currentIndex);
      }
    }
 else if (button == &prevTrackButton)
    {
      if (playlist.size() > 0)
      {
        currentIndex = (currentIndex - 1 + playlist.size()) % playlist.size();
        playlistBox.selectRow(currentIndex);
        playFileAtIndex(currentIndex);
      }
    }
 else if (button == &goToEnd)
 {
   playerAudio.goToEnd();
}
 else if (button == &playlistToggleButton)
    {
      // toggle playlist visibility
      playlistVisible = !playlistVisible;
      playlistBox.setVisible(playlistVisible);

      // optional: change button appearance
      if (playlistVisible)
        playlistToggleButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
      else
        playlistToggleButton.setColour(juce::TextButton::buttonColourId, juce::Colours::grey);

      // force layout update so left controls can expand/contract if desired
      resized();
    }
}


void PlayerGUI::timerCallback()
{
    if (!progressSlider.isMouseButtonDown()){
        progressSlider.setValue(playerAudio.getPosition() / playerAudio.getLength(),juce::dontSendNotification);
    }
      auto toTime = [](double s)
        {
            int m = (int)(s / 60);
            int sec = (int)(fmod(s, 60.0));
            return juce::String::formatted("%02d:%02d", m, sec);
        };

    timeLabel.setText(toTime(playerAudio.getPosition()) + " / " + toTime(playerAudio.getLength()), juce::dontSendNotification);
}


void PlayerGUI::playFileAtIndex(int index)
{
  if (index < 0 || index >= playlist.size())
    return;

  const juce::File& file = playlist[index];

  if (playerAudio.LoadFile(file))
  {
    currentIndex = index;
    playlistBox.selectRow(currentIndex);
    playerAudio.play();

    // show metadata basic info (filename + length if available)
    juce::AudioFormatManager fmt;
    fmt.registerBasicFormats();
    std::unique_ptr<juce::AudioFormatReader> reader(fmt.createReaderFor(file));
    if (reader != nullptr)
    {
      double duration = reader->lengthInSamples / reader->sampleRate;
      juce::String info = "File: " + file.getFileName() + "\n";
      info += "Sample Rate: " + juce::String(reader->sampleRate) + " Hz\n";
      info += "Channels: " + juce::String(reader->numChannels) + "\n";
      int totalSeconds = static_cast<int>(duration);
      int minutes = totalSeconds / 60;
      int seconds = totalSeconds % 60;

      juce::String durationStr = juce::String::formatted("%d:%02d", minutes, seconds);
      info += "Duration: " + durationStr + " (mm:ss)";
      if (reader->metadataValues.size() > 0)
      {
        juce::String artist, title, album;

        for (auto key : reader->metadataValues.getAllKeys())
        {
          if (key.compareIgnoreCase("artist") == 0 || key.compareIgnoreCase("author") == 0)
            artist = reader->metadataValues[key];
          else if (key.compareIgnoreCase("title") == 0)
            title = reader->metadataValues[key];
          else if (key.compareIgnoreCase("album") == 0)
            album = reader->metadataValues[key];
        }

        info << "\n--- Metadata ---\n";
        if (title.isNotEmpty())  info << "Title: " << title << "\n";
        if (artist.isNotEmpty()) info << "Artist: " << artist << "\n";
        if (album.isNotEmpty())  info << "Album: " << album << "\n";
      }
      else
      {
        info << "\n(No another metadata found)";
      }

      metadataLabel.setText(info, juce::dontSendNotification);
    }
    else
    {
      metadataLabel.setText("Loaded: " + file.getFileName(), juce::dontSendNotification);
    }
  }
  else
  {
    metadataLabel.setText("Failed to load: " + file.getFileName(), juce::dontSendNotification);
  }
}

int PlayerGUI::getNumRows()
{
  return playlist.size();
}

void PlayerGUI::paintListBoxItem(int row, juce::Graphics& g, int width, int height, bool rowIsSelected)
{
  if (row < 0 || row >= playlist.size())
    return;

  if (rowIsSelected)
    g.fillAll(juce::Colours::darkslategrey);

  g.setColour(juce::Colours::white);
  g.setFont(14.0f);

  const juce::String fileName = playlist[row].getFileName();
  g.drawText(fileName, 4, 0, width - 8, height, juce::Justification::centredLeft);
}

void PlayerGUI::listBoxItemDoubleClicked(int row, const juce::MouseEvent& e)
{
  if (row >= 0 && row < playlist.size())
    playFileAtIndex(row);
}

void PlayerGUI::selectedRowsChanged(int lastRowSelected)
{
  // optional: update metadata preview when selection changes
  if (lastRowSelected >= 0 && lastRowSelected < playlist.size())
  {
    const juce::File& file = playlist[lastRowSelected];
    metadataLabel.setText("Selected: " + file.getFileName(), juce::dontSendNotification);
  }
}

void PlayerGUI::paint(juce::Graphics& g)
{
  g.fillAll(juce::Colours::darkgrey);
  g.setColour(juce::Colours::white);
  g.drawText("Audio Player", getLocalBounds(), juce::Justification::centredTop);
}


