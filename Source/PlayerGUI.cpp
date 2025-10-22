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
    for (auto* btn : { &loadButton, &restartButton, &stopButton, &playButton , &forwardButton , &backwardButton })
    {
        addAndMakeVisible(btn); 
        btn->addListener(this);
    }

    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);
    addAndMakeVisible(metadataLabel);
    metadataLabel.setText("No file loaded", juce::dontSendNotification);
    metadataLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    metadataLabel.setJustificationType(juce::Justification::centred);
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
}

void PlayerGUI::resized()
{
    int labelHeight = 4 * (metadataLabel.getFont().getHeight() + 10);
    int margin = 20;
    int buttonHeight = 40;
    int buttonWidth = (getWidth() - margin * 2 - 30) / 4; 
    int y = 10 + labelHeight;

    metadataLabel.setBounds(margin, 10, getWidth() - 2 * margin, labelHeight);

    loadButton.setBounds(margin, y, buttonWidth, buttonHeight);
    restartButton.setBounds(margin + buttonWidth + 10, y, buttonWidth, buttonHeight);
    stopButton.setBounds(margin + 2 * (buttonWidth + 10), y, buttonWidth, buttonHeight);
    playButton.setBounds(margin + 3 * (buttonWidth + 10), y, buttonWidth, buttonHeight);

    int fbY = y + buttonHeight + 15;
    backwardButton.setBounds(margin + 110, fbY, buttonWidth, buttonHeight);
    forwardButton.setBounds(margin + buttonWidth + 120, fbY, buttonWidth, buttonHeight);

    volumeSlider.setBounds(margin, fbY + buttonHeight + 20, getWidth() - 2 * margin, 30);
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
    }
    else if (button == &stopButton)
    {
        playerAudio.stop();
        
    }
    else if (button == &playButton)
    {
        playerAudio.play();
    }
    else if (button == &forwardButton)
    {
        playerAudio.skipForward(10.0); 
    }
    else if (button == &backwardButton)
    {
        playerAudio.skipBackward(10.0); 
    }
}



void PlayerGUI::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
    g.setColour(juce::Colours::white);
    g.drawText("Audio Player", getLocalBounds(), juce::Justification::centredTop);
}


