#pragma once
#include <JuceHeader.h>

class PlayerAudio : public juce::AudioSource
{
public:
    PlayerAudio();
    ~PlayerAudio() override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    bool LoadFile(const juce::File& file);
    void play();
    void stop();
    void Restart();
    void setGain(float gain);
    void setPosition(double pos);
    double getPosition() const;
    double getLength() const;
    void setLooping(bool shouldLoop);
    bool isLooping() const;
    void skipForward(double seconds);
    void skipBackward(double seconds);
    void mute();
    void unmute();
    void setLoopPointA(double position);
    void setLoopPointB(double position);
    void enableSegmentLoop(bool shouldLoop);
    bool getSegmentLooping() const { return isSegmentLooping; }
    void checkSegmentLoop();
private:
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;
    bool islooping = false;
    float previousGain = 0.5f;
    bool isMuted = false;
    double pointA = -1.0;
    double pointB = -1.0;
    bool isSegmentLooping = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerAudio)
};
