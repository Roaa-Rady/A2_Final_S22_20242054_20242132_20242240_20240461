/*
  ==============================================================================

    PlayerGUI.cpp
    Created: 17 Oct 2025 2:21:10pm
    Author:  RTX

  ==============================================================================
*/

#include "PlayerGUI.h"




PlayerGUI::MarkersListBoxModel::MarkersListBoxModel(juce::Array<TrackMarker>& m, PlayerGUI& p) : markersArray(m), parent(p) {}

int PlayerGUI::MarkersListBoxModel::getNumRows() { return markersArray.size(); }

void PlayerGUI::MarkersListBoxModel::paintListBoxItem(int row, juce::Graphics& g, int width, int height, bool rowIsSelected)
{
    if (row < 0 || row >= markersArray.size()) return;

    if (rowIsSelected)
        g.fillAll(juce::Colours::darkblue.withAlpha(0.5f));

    g.setColour(juce::Colours::white);
    g.setFont(14.0f);

    const TrackMarker& marker = markersArray[row];
    juce::String text = marker.name + " (" + marker.getFormattedPosition() + ")";
    g.drawText(text, 4, 0, width - 8, height, juce::Justification::centredLeft, true);
}

void PlayerGUI::MarkersListBoxModel::listBoxItemDoubleClicked(int row, const juce::MouseEvent& e)
{
    if (row >= 0 && row < markersArray.size())
    {
        parent.playerAudio.setPosition(markersArray[row].position);
        parent.playerAudio.play();
    }
}

PlayerGUI::PlayerGUI(const juce::String& name)
    : slotName(name) {  
    markersListBoxModel = std::make_unique<MarkersListBoxModel>(markers, *this);

    markersListBox.setModel(markersListBoxModel.get());
    addAndMakeVisible(markersListBox);

    auto makeIconButton = [&](std::unique_ptr<juce::DrawableButton>& btn,
        const juce::String& name,
        const juce::String& fileName)
        {
            btn = std::make_unique<juce::DrawableButton>(name, juce::DrawableButton::ImageFitted);

            juce::File exeFile = juce::File::getSpecialLocation(juce::File::currentApplicationFile);
            juce::File cwd = juce::File::getCurrentWorkingDirectory();

            juce::File tryPaths[] = {
                exeFile.getParentDirectory().getChildFile(fileName),
                exeFile.getParentDirectory().getChildFile("Assets").getChildFile(fileName),
                cwd.getChildFile("Assets").getChildFile(fileName),
                cwd.getChildFile(fileName)
            };

            juce::File iconFile;
            for (auto& f : tryPaths)
            {
                if (f.existsAsFile())
                {
                    iconFile = f;
                    break;
                }
            }

            if (iconFile.existsAsFile())
            {
                DBG("🔍 Loading SVG: " + iconFile.getFullPathName());

                if (auto xml = juce::parseXML(iconFile))
                {
                    if (auto svg = juce::Drawable::createFromSVG(*xml))
                    {
                        // transfer ownership of the Drawable into the button so it remains valid
                        btn->setImages(svg.release());
                        DBG("✅ Loaded SVG: " + fileName);
                    }
                    else
                    {
                        DBG("❌ SVG parse failed: " + fileName);
                    }
                }
                else
                {
                    DBG("❌ Failed to parse XML from: " + fileName);
                }
            }
            else
            {
                DBG("❌ Could not find file: " + fileName);
            }

            // ensure clicks are delivered to PlayerGUI::buttonClicked
            btn->addListener(this);
            addAndMakeVisible(btn.get());
        };




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
    panSlider.setRange(0.0, 1.0);
    panSlider.setValue(0.5);
    panSlider.setTextValueSuffix(" Pan");
    panSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    panSlider.setColour(juce::Slider::thumbColourId, juce::Colours::orange);
    panSlider.addListener(this);
    addAndMakeVisible(panSlider);
    panLabel.setText("Pan", juce::dontSendNotification);
    panLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(panLabel);
    addAndMakeVisible(timeLabel);
    addAndMakeVisible(metadataLabel);
    addAndMakeVisible(markerNameInput);

    timeLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    timeLabel.setJustificationType(juce::Justification::centred);
    metadataLabel.setText("No file loaded", juce::dontSendNotification);
    metadataLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    metadataLabel.setJustificationType(juce::Justification::centred);
    startTimer(10);
    loadLastSession();
    // playlist box
    addAndMakeVisible(playlistBox);
    playlistBox.setModel(this);
    playlistBox.setMultipleSelectionEnabled(false);
    playlistBox.setRowHeight(24);
    playlistBox.setVisible(playlistVisible);

    makeIconButton(playButton, "Play", "play.svg");
    makeIconButton(stopButton, "Stop", "stop.svg");
    makeIconButton(restartButton, "Restart", "restart.svg");
    makeIconButton(forwardButton, "Forward", "forward.svg");
    makeIconButton(backwardButton, "Backward", "backward.svg");
    makeIconButton(muteButton, "Mute", "mute.svg");
    makeIconButton(unmuteButton, "Unmute", "unmute.svg");
    makeIconButton(loopButton, "Loop", "loop.svg");
    makeIconButton(setAButton, "SetA", "setA.svg");
    makeIconButton(setBButton, "SetB", "setB.svg");
    makeIconButton(loopABButton, "LoopAB", "loopAB.svg");
    makeIconButton(prevTrackButton, "Prev", "prev.svg");
    makeIconButton(nextTrackButton, "Next", "next.svg");
    makeIconButton(playlistToggleButton, "Playlist", "playlist.svg");
    makeIconButton(goToEnd, "End", "end.svg");
    makeIconButton(loadButton, "Load", "load.svg");
    makeIconButton(addMarkerButton, "AddMarker", "addmarker.svg");
    makeIconButton(clearMarkersButton, "DeleteMarker", "deletemark.svg");

}

PlayerGUI::~PlayerGUI() {
    saveLastSession();
}

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
            double newPos = progressSlider.getValue() * length;
            playerAudio.setPosition(newPos);
        }
    }
    else if (slider == &speedSlider)
    {
        playerAudio.setPlaybackRate((float)slider->getValue());
    }
    else if (slider == &panSlider)
    {
        playerAudio.setPan((float)slider->getValue());
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

    // حساب مساحة التحكم
    int controlsX = margin;
    int controlsY = margin;
    int playlistWidth = getWidth() / 3;
    int minPlaylistWidth = 220;
    if (playlistWidth < minPlaylistWidth) playlistWidth = minPlaylistWidth;
    if (playlistWidth > getWidth() - 200) playlistWidth = getWidth() - 200;

    int controlsW = getWidth() - playlistWidth - 3 * margin;

    // playlist box — قصير شوي
    int playlistX = getWidth() - margin - playlistWidth;
    int playlistY = margin;
    int playlistH = 150; // قلصناه
    playlistBox.setBounds(playlistX, playlistY, playlistWidth, playlistH);
    playlistBox.setVisible(playlistVisible);

    // metadata label
    metadataLabel.setBounds(controlsX, controlsY, controlsW, labelHeight);

    // ترتيب الأزرار بعد الميتاداتا — بدون تغيير
    int buttonsStartY = controlsY + labelHeight + 10;
    int cols = controlsW / (buttonWidth + buttonSpace);
    if (cols < 1) cols = 1;

    juce::Component* buttons[] = {
        loadButton.get(), restartButton.get(), stopButton.get(), playButton.get(),
        loopButton.get(), muteButton.get(), unmuteButton.get(), forwardButton.get(),
        backwardButton.get(), setAButton.get(), setBButton.get(), loopABButton.get(),
        prevTrackButton.get(), nextTrackButton.get(), playlistToggleButton.get(), goToEnd.get()
    };
    int numButtonsToLayout = sizeof(buttons) / sizeof(buttons[0]);

    for (int i = 0; i < numButtonsToLayout; i++)
    {
        int row = i / cols;
        int col = i % cols;
        int bx = controlsX + col * (buttonWidth + buttonSpace);
        int by = buttonsStartY + row * (buttonHeight + 10);
        buttons[i]->setBounds(bx, by, buttonWidth, buttonHeight);
    }

    int totalRows = (numButtonsToLayout + cols - 1) / cols;
    int afterButtonsY = buttonsStartY + totalRows * (buttonHeight + 10) + 10;

    // ---------------- Markers section تحت Playlist ----------------
    int markerAreaY = playlistY + playlistH + 10;
    int markerButtonsHeight = buttonHeight;

    markerNameInput.setBounds(playlistX, markerAreaY, playlistWidth - buttonWidth * 2 - 10, markerButtonsHeight);
    addMarkerButton->setBounds(playlistX + playlistWidth - buttonWidth * 2 - 5, markerAreaY, buttonWidth, markerButtonsHeight);
    clearMarkersButton->setBounds(playlistX + playlistWidth - buttonWidth, markerAreaY, buttonWidth, markerButtonsHeight);

    int markersListY = markerAreaY + markerButtonsHeight + 5;
    int markersListHeight = getHeight() - markersListY - margin;
    markersListBox.setBounds(playlistX, markersListY, playlistWidth, markersListHeight);

    // الـ Sliders — بدون تغيير
    volumeSlider.setBounds(controlsX, afterButtonsY, controlsW, 30);
    progressSlider.setBounds(controlsX, afterButtonsY + 40, controlsW, 20);
    speedLabel.setBounds(controlsX, afterButtonsY + 70, controlsW, 20);
    speedSlider.setBounds(controlsX, afterButtonsY + 85, controlsW, 25);
    panLabel.setBounds(controlsX, afterButtonsY + 110, controlsW, 20);
    panSlider.setBounds(controlsX, afterButtonsY + 120, controlsW, 25);
    timeLabel.setBounds(controlsX, afterButtonsY + 50, controlsW, 25);

}



void PlayerGUI::buttonClicked(juce::Button* button)
{
    if (button == loadButton.get())
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
    else if (button == restartButton.get())
    {
        playerAudio.setPosition(0.0);
        playerAudio.play();
        playButton->setColour(juce::TextButton::buttonColourId, juce::Colours::green);
        stopButton->setColour(juce::TextButton::buttonColourId, juce::Colours::grey);
    }
    else if (button == stopButton.get())
    {
        playerAudio.stop();
        stopButton->setColour(juce::TextButton::buttonColourId, juce::Colours::red);
        playButton->setColour(juce::TextButton::buttonColourId, juce::Colours::grey);

    }
    else if (button == playButton.get())
    {
        playerAudio.play();
        playButton->setColour(juce::TextButton::buttonColourId, juce::Colours::green);
        stopButton->setColour(juce::TextButton::buttonColourId, juce::Colours::grey);
    }
    else if (button == muteButton.get())
    {
        playerAudio.mute();
        muteButton->setColour(juce::TextButton::buttonColourId, juce::Colours::red);
        unmuteButton->setColour(juce::TextButton::buttonColourId, juce::Colours::grey);
    }
    else if (button == unmuteButton.get())
    {
        playerAudio.unmute();
        unmuteButton->setColour(juce::TextButton::buttonColourId, juce::Colours::green);
        muteButton->setColour(juce::TextButton::buttonColourId, juce::Colours::grey);
    }
    else if (button == forwardButton.get())
    {
        playerAudio.skipForward(10.0);
    }
    else if (button == backwardButton.get())
    {
        playerAudio.skipBackward(10.0);
    }
    else if (button == loopButton.get())
    {
        loopOn = !loopOn;
        if (loopOn)
            loopButton->setColour(juce::TextButton::buttonColourId, juce::Colours::green);
        else
            loopButton->setColour(juce::TextButton::buttonColourId, juce::Colours::grey);
        playerAudio.setLooping(loopOn);
    }
    else if (button == setAButton.get())
    {
        playerAudio.setLoopPointA(playerAudio.getPosition());
    }
    else if (button == setBButton.get())
    {
        playerAudio.setLoopPointB(playerAudio.getPosition());
    }
    else if (button == loopABButton.get())
    {
        playerAudio.enableSegmentLoop(!playerAudio.getSegmentLooping());
        isLoopingAB = !isLoopingAB;
        if (isLoopingAB)
            loopABButton->setColour(juce::TextButton::buttonColourId, juce::Colours::mediumorchid);
        else
            loopABButton->setColour(juce::TextButton::buttonColourId, juce::Colours::grey);
        playerAudio.setLooping(isLoopingAB);
    }

    else if (button == nextTrackButton.get())
    {
        if (playlist.size() > 0)
        {
            currentIndex = (currentIndex + 1 + playlist.size()) % playlist.size();
            playlistBox.selectRow(currentIndex);
            playFileAtIndex(currentIndex);
        }
    }
    else if (button == prevTrackButton.get())
    {
        if (playlist.size() > 0)
        {
            currentIndex = (currentIndex - 1 + playlist.size()) % playlist.size();
            playlistBox.selectRow(currentIndex);
            playFileAtIndex(currentIndex);
        }
    }
    else if (button == goToEnd.get())
    {
        playerAudio.goToEnd();
    }
    else if (button == playlistToggleButton.get())
    {
        // toggle playlist visibility
        playlistVisible = !playlistVisible;
        playlistBox.setVisible(playlistVisible);

        // optional: change button appearance
        if (playlistVisible)
            playlistToggleButton->setColour(juce::TextButton::buttonColourId, juce::Colours::green);
        else
            playlistToggleButton->setColour(juce::TextButton::buttonColourId, juce::Colours::grey);

    }
    // force layout update so left controls can expand/contract if desired 

    else if (button == addMarkerButton.get())
    {
        if (playerAudio.getLength() > 0.0)
        {
            double currentPos = playerAudio.getPosition();
            juce::String defaultName = "Marker " + juce::String(markers.size() + 1);
            juce::String markerText = markerNameInput.getText().isEmpty() ? defaultName : markerNameInput.getText();

            markers.add({ markerText, currentPos });
            markersListBox.updateContent();
            markersListBox.selectRow(markers.size() - 1);
            markerNameInput.clear();
            saveLastSession();
        }
        else
        {
            metadataLabel.setText("Load an audio file first to add a marker.", juce::dontSendNotification);
        }
    }

    else if (button == clearMarkersButton.get())
    {
        markers.clear();
        markersListBox.updateContent();
        saveLastSession();
    }
    resized();

}


void PlayerGUI::timerCallback()
{
    topPlayerCurrentFile = playlist.size() > 0 && currentIndex >= 0 ? playlist[currentIndex] : juce::File();
    topPlayerPosition = playerAudio.getPosition();


    if (!progressSlider.isMouseButtonDown()) {
        progressSlider.setValue(playerAudio.getPosition() / playerAudio.getLength(), juce::dontSendNotification);
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
    // خفف لون الخلفية علشان الأيقونات تبان
    g.fillAll(juce::Colour::fromRGB(50, 50, 50));  // رمادي غامق ناعم مش أسود تمامًا
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(18.0f, juce::Font::bold));
    g.drawText("Audio Player", getLocalBounds().removeFromTop(30), juce::Justification::centred);
}


void PlayerGUI::saveLastSession()
{
    juce::File sessionFile = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory)
        .getChildFile("audio_player_session_" + slotName + ".txt");

    juce::String data;

    data << topPlayerCurrentFile.getFullPathName() << "\n"
        << topPlayerPosition << "\n";

    data << "---MARKERS---\n";
    for (const auto& marker : markers)
    {
        data << marker.toString() << "\n";
    }
    data << "---END_MARKERS---\n";

    data << "---PLAYLIST---\n";
    for (int i = 0; i < playlist.size(); ++i)
    {
        data << playlist[i].getFullPathName() << "," << playlistPositions[i] << "\n";
    }
    data << "---END_PLAYLIST---\n";

    sessionFile.replaceWithText(data);
}

void PlayerGUI::loadLastSession()
{
    juce::File sessionFile = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory)
        .getChildFile("audio_player_session_" + slotName + ".txt");

    if (sessionFile.existsAsFile())
    {
        juce::StringArray allLines;
        allLines.addLines(sessionFile.loadFileAsString());

        int currentLineIndex = 0;

        if (allLines.size() > currentLineIndex + 1)
        {
            topPlayerCurrentFile = juce::File(allLines[currentLineIndex++]);
            topPlayerPosition = allLines[currentLineIndex++].getDoubleValue();

            if (topPlayerCurrentFile.existsAsFile())
            {
                playerAudio.LoadFile(topPlayerCurrentFile);
                playerAudio.setPosition(topPlayerPosition);
                int existingIndex = playlist.indexOf(topPlayerCurrentFile);

                if (existingIndex == -1)
                {
                    playlist.add(topPlayerCurrentFile);
                    playlistPositions.add(topPlayerPosition);
                    currentIndex = playlist.size() - 1;
                }
                else
                {
                    playlistPositions.set(existingIndex, topPlayerPosition);
                    currentIndex = existingIndex;
                }


                playlistBox.updateContent();
                playlistBox.selectRow(currentIndex);
                metadataLabel.setText("Restored: " + topPlayerCurrentFile.getFileName() + " @ " + juce::String(topPlayerPosition, 2) + "s", juce::dontSendNotification);
                playerAudio.play();
            }
        }

        bool foundMarkersSection = false;
        while (currentLineIndex < allLines.size())
        {
            if (allLines[currentLineIndex] == "---MARKERS---")
            {
                foundMarkersSection = true;
                currentLineIndex++;
                break;
            }
            currentLineIndex++;
        }

        if (foundMarkersSection)
        {
            while (currentLineIndex < allLines.size() && allLines[currentLineIndex] != "---END_MARKERS---")
            {
                juce::String line = allLines[currentLineIndex++];
                if (!line.isEmpty())
                {
                    markers.add(TrackMarker::fromString(line));
                }
            }
            markersListBox.updateContent();
        }

        bool foundPlaylistSection = false;
        while (currentLineIndex < allLines.size())
        {
            if (allLines[currentLineIndex] == "---PLAYLIST---")
            {
                foundPlaylistSection = true;
                currentLineIndex++;
                break;
            }
            currentLineIndex++;
        }

        if (foundPlaylistSection)
        {
            while (currentLineIndex < allLines.size() && allLines[currentLineIndex] != "---END_PLAYLIST---")
            {
                juce::String line = allLines[currentLineIndex++];
                if (!line.isEmpty())
                {
                    juce::StringArray parts;
                    parts.addTokens(line, ",", "");
                    if (parts.size() == 2)
                    {
                        juce::File playlistFile(parts[0]);
                        double playlistFilePosition = parts[1].getDoubleValue();
                        if (playlistFile.existsAsFile())
                        {
                            int existingIndex = playlist.indexOf(playlistFile);
                            if (existingIndex == -1)
                            {
                                playlist.add(playlistFile);
                                playlistPositions.add(playlistFilePosition);
                            }
                        }
                    }
                }
            }
            playlistBox.updateContent();
            if (currentIndex == -1 && playlist.size() > 0)
            {
                currentIndex = 0;
                playlistBox.selectRow(currentIndex);
            }
        }
    }
}



