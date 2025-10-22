#include "PlayerAudio.h"

PlayerAudio::PlayerAudio()
{
    formatManager.registerBasicFormats();
}

PlayerAudio::~PlayerAudio()
{
    releaseResources();
}

void PlayerAudio::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerAudio::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    transportSource.getNextAudioBlock(bufferToFill);
}

void PlayerAudio::releaseResources()
{
    transportSource.releaseResources();
}

bool PlayerAudio::LoadFile(const juce::File& file)
{
    if (file.existsAsFile())
    {
        if (auto* reader = formatManager.createReaderFor(file))
        {
            transportSource.stop();
            transportSource.setSource(nullptr);
            readerSource.reset();

            readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);

            transportSource.setSource(readerSource.get(), 0, nullptr, reader->sampleRate);
            return true;
        }
    }
    return false;
}

void PlayerAudio::play() { transportSource.start(); }

void PlayerAudio::stop()
{
    transportSource.stop();
    
}

void PlayerAudio::Restart() {
    transportSource.setPosition(0.0);
    transportSource.start();
}

void PlayerAudio::skipForward(double seconds)
{
    auto newPos = transportSource.getCurrentPosition() + seconds;
    transportSource.setPosition(std::min(newPos, transportSource.getLengthInSeconds()));
}

void PlayerAudio::skipBackward(double seconds)
{
    auto newPos = transportSource.getCurrentPosition() - seconds;
    transportSource.setPosition(std::max(0.0, newPos));
}
void PlayerAudio::setGain(float gain)
{
    transportSource.setGain(gain);
}

void PlayerAudio::setPosition(double pos)
{
    transportSource.setPosition(pos);
}

double PlayerAudio::getPosition() const
{
    return transportSource.getCurrentPosition();
}

double PlayerAudio::getLength() const
{
    return transportSource.getLengthInSeconds();
}
void PlayerAudio::mute()
{
    if (!isMuted)
    {
        previousGain = transportSource.getGain();
        transportSource.setGain(0.0f);
        isMuted = true;
    }
    
  
}
void PlayerAudio::unmute()
{
    if (isMuted)
    {
        transportSource.setGain(previousGain);
        isMuted = false;
    }
    
   
}

