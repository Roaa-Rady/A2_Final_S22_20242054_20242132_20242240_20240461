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
struct TrackMarker
{
    double position = 0.0;
    juce::String name;

    TrackMarker() = default;
    TrackMarker(const juce::String& n, double pos) : name(n), position(pos) {}

    juce::String getFormattedPosition() const
    {
        int totalSeconds = static_cast<int>(position);
        int hours = totalSeconds / 3600;
        int minutes = (totalSeconds % 3600) / 60;
        int seconds = totalSeconds % 60;

        if (hours > 0)
            return juce::String::formatted("%02d:%02d:%02d", hours, minutes, seconds);
        else
            return juce::String::formatted("%02d:%02d", minutes, seconds);
    }

    juce::String toString() const
    {
        return name + "," + juce::String(position);
    }

    static TrackMarker fromString(const juce::String& s)
    {
        juce::StringArray parts;
        parts.addTokens(s, ",", "");
        if (parts.size() == 2)
            return { parts[0], parts[1].getDoubleValue() };
        return { "Invalid", 0.0 };
    }
};


class PlayerGUI : public juce::Component,
    public juce::Button::Listener,
    public juce::Slider::Listener,
    public juce::ListBoxModel,
    public juce::Timer

{
public:
    ~PlayerGUI() override;
    PlayerAudio playerAudio;

    PlayerGUI(const juce::String& name = "default");
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
    void releaseResources();

    void paint(juce::Graphics& g) override;
    void resized() override;
    juce::File topPlayerCurrentFile;
    double topPlayerPosition = 0.0;
    juce::Array<double> playlistPositions;

    // ListBoxModel overrides (playlist)
    int getNumRows() override;
    void paintListBoxItem(int row, juce::Graphics& g, int width, int height, bool rowIsSelected) override;
    void listBoxItemDoubleClicked(int row, const juce::MouseEvent& e) override;
    void selectedRowsChanged(int lastRowSelected) override;
    void saveLastSession();
    void loadLastSession();
private:
    juce::String slotName;

    std::unique_ptr<juce::DrawableButton> playButton, stopButton, restartButton,
        muteButton, unmuteButton, nextTrackButton, prevTrackButton,
        forwardButton, backwardButton, loopButton, setAButton,
        setBButton, loopABButton, playlistToggleButton, goToEnd, loadButton;

    juce::Slider speedSlider;
    juce::Label speedLabel;
    juce::Label metadataLabel;
    juce::Slider volumeSlider;
    juce::Slider progressSlider;
    juce::Slider panSlider;
    juce::Label panLabel;
    juce::Label timeLabel;
    std::unique_ptr<juce::FileChooser> fileChooser;
    juce::Array<TrackMarker> markers;
    std::unique_ptr<juce::DrawableButton> addMarkerButton;
    std::unique_ptr<juce::DrawableButton> clearMarkersButton;

    juce::ListBox markersListBox;
    juce::TextEditor markerNameInput;

    class MarkersListBoxModel : public juce::ListBoxModel
    {
    public:
        MarkersListBoxModel(juce::Array<TrackMarker>& m, PlayerGUI& p);

        int getNumRows() override;
        void paintListBoxItem(int row, juce::Graphics& g, int width, int height, bool rowIsSelected) override;
        void listBoxItemDoubleClicked(int row, const juce::MouseEvent& e) override;
       
    private:
        juce::Array<TrackMarker>& markersArray;
        PlayerGUI& parent;
    };

    std::unique_ptr<MarkersListBoxModel> markersListBoxModel;

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




