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
     int margin = 20;
 int buttonHeight = 40;
 int labelHeight = static_cast<int>(4 * (metadataLabel.getFont().getHeight() + 10));
 int buttonSpace = 10;
 int rowButtons = 5;
 int numButtons = 7; 
 int buttonWidth = 80;
 

int y = 10 + labelHeight;
int x = margin;

juce::Component* buttons[] = {&loadButton,&restartButton,&stopButton,&playButton,&loopButton,&muteButton,&unmuteButton};


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
metadataLabel.setBounds(margin, 10, getWidth() - 2 * margin, labelHeight);
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
    else if (button == &muteButton)
    {
    playerAudio.mute();
    }
    else if (button == &unmuteButton)
    {
    playerAudio.unmute();
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

}




void PlayerGUI::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
    g.setColour(juce::Colours::white);
    g.drawText("Audio Player", getLocalBounds(), juce::Justification::centredTop);
}


