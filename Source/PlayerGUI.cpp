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
    for (auto* btn : { &loadButton, &restartButton, &stopButton, &playButton ,&loopButton,&muteButton,&unmuteButton, &forwardButton , &backwardButton,&setAButton,&setBButton,&loopABButton })
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
 int buttonHeight = 30;
 int labelHeight = static_cast<int>(4 * (metadataLabel.getFont().getHeight() + 10));
 int buttonSpace = 10;
 int rowButtons = 7;
 int numButtons = 12; 
 int buttonWidth = 70;
 

int y = 10 + labelHeight;
int x = margin;

juce::Component* buttons[] = {&loadButton,&restartButton,&stopButton,&playButton,&loopButton,&muteButton,&unmuteButton,&forwardButton , &backwardButton ,&setAButton,&setBButton,&loopABButton};


for (int i = 0; i < numButtons; i++)
{
    int row = i / rowButtons;   
    int col = i % rowButtons;   
    int x = margin + col * (buttonWidth + buttonSpace);
    int newY = y + row * (buttonHeight + 10);

    buttons[i]->setBounds(x, newY, buttonWidth, buttonHeight);
}

int totalRows = (numButtons + rowButtons - 1) / rowButtons;

int nextY = y + totalRows * (buttonHeight + 20);
volumeSlider.setBounds(margin, nextY, getWidth() - 2 * margin, 30);
progressSlider.setBounds(margin, nextY + 40, getWidth() - 2 * margin, 20);
speedSlider.setBounds(margin, nextY + 60, getWidth() - 2 * margin, 20);
speedLabel.setBounds(margin, nextY +70, getWidth() - 2 * margin, 20);
timeLabel.setBounds(margin, nextY +50, getWidth() - 2 * margin, 20);
metadataLabel.setBounds(margin, 10, getWidth() - 2 * margin, labelHeight);
int speedLabelY = nextY + 70;  
int speedSliderY = speedLabelY + 30;
speedLabel.setBounds(margin, speedLabelY, getWidth() - 2 * margin, 35);  
speedSlider.setBounds(margin, speedSliderY, getWidth() - 2 * margin, 45); 
 
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
            juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
            [this](const juce::FileChooser& fc)
            {
                auto file = fc.getResult();
                if (file.existsAsFile())
                {
                    playerAudio.LoadFile(file);

                    juce::AudioFormatManager formatManager;
                    formatManager.registerBasicFormats();
                    std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(file));

                    if (reader != nullptr)
                    {
                        
                        double duration = reader->lengthInSamples / reader->sampleRate;

                        juce::String info = "File: " + file.getFileName() + "\n";
                        info += "Sample Rate: " + juce::String(reader->sampleRate) + " Hz\n";
                        info += "Channels: " + juce::String(reader->numChannels) + "\n";
                        info += "Duration: " + juce::String(duration, 2) + " sec";

                        metadataLabel.setText(info, juce::dontSendNotification);
                    }
                    else
                    {
                        metadataLabel.setText("Loaded: " + file.getFileName(), juce::dontSendNotification);
                    }
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


void PlayerGUI::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
    g.setColour(juce::Colours::white);
    g.drawText("Audio Player", getLocalBounds(), juce::Justification::centredTop);
}


