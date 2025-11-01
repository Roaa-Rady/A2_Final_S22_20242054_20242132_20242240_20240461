/*
  ==============================================================================

    PlayerGUI.h
    Created: 17 Oct 2025 2:21:10pm
    Author:  RTX

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "PlayerAudio.h"

class PlayerGUI : public juce::Component,
    public juce::Button::Listener,
    public juce::Slider::Listener
{
public:
    PlayerGUI();
    ~PlayerGUI() override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
    void releaseResources();

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    PlayerAudio playerAudio; 
    juce::TextButton loadButton{ "Load Files" };
    juce::TextButton restartButton{ "Restart" };
    juce::TextButton stopButton{ "Stop" };
    juce::TextButton playButton{ "Play" };
    juce::TextButton loopButton{ "Loop" };
    juce::TextButton muteButton{ "Mute" };
    juce::TextButton unmuteButton{ "Unmute" };
    juce::Label metadataLabel;
    juce::Slider volumeSlider;
    juce::TextButton forwardButton{ "10s >>" };
    juce::TextButton backwardButton{ "<< 10s" };
    juce::TextButton setAButton{ "Set A" };
    juce::TextButton setBButton{ "Set B" };
    juce::TextButton loopABButton{ "Loop A-B" };
    std::unique_ptr<juce::FileChooser> fileChooser;
    bool loopOn = false;
    bool isLoopingAB = false;
    // Event handlers 
    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI)
};

