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
    public juce::Slider::Listener,
    public juce::ListBoxModel,
    public juce::Timer 
{
public:
    PlayerGUI();
    ~PlayerGUI() override;
    PlayerAudio playerAudio; 
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
    void releaseResources();

    void paint(juce::Graphics& g) override;
    void resized() override;

    // ListBoxModel overrides (playlist)
    int getNumRows() override;
    void paintListBoxItem(int row, juce::Graphics& g, int width, int height, bool rowIsSelected) override;
    void listBoxItemDoubleClicked(int row, const juce::MouseEvent& e) override;
    void selectedRowsChanged(int lastRowSelected) override;

private:
    
    juce::TextButton loadButton{ "Load Files" };
    juce::TextButton restartButton{ "Restart" };
    juce::TextButton stopButton{ "Stop" };
    juce::TextButton playButton{ "Play" };
    juce::TextButton goToEnd{ "End" };
    juce::TextButton loopButton{ "Loop" };
    juce::TextButton muteButton{ "Mute" };
    juce::TextButton unmuteButton{ "Unmute" };
    juce::TextButton nextTrackButton{ "Next" };
    juce::TextButton prevTrackButton{ "Prev" };
    juce::Slider speedSlider; 
    juce::Label speedLabel; 
    juce::Label metadataLabel;
    juce::Slider volumeSlider;
    juce::Slider progressSlider;
    juce::Label timeLabel;
    juce::TextButton forwardButton{ "10s >>" };
    juce::TextButton backwardButton{ "<< 10s" };
    juce::TextButton setAButton{ "Set A" };
    juce::TextButton setBButton{ "Set B" };
    juce::TextButton loopABButton{ "Loop A-B" };
    std::unique_ptr<juce::FileChooser> fileChooser;

    juce::TextButton playlistToggleButton{ "Playlist" };
    bool playlistVisible = true;
    juce::ListBox playlistBox;
    juce::Array<juce::File> playlist;
    int currentIndex = -1;

    bool loopOn = false;
    bool isLoopingAB = false;
    // Event handlers 
    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;
    void playFileAtIndex(int index);
    void timerCallback() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI)
};

