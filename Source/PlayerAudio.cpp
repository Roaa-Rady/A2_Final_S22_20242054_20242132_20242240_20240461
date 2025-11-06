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
    checkSegmentLoop();
    // Apply panning (left/right balance)
    if (bufferToFill.buffer->getNumChannels() > 1)
    {
        float leftGain = 1.0f - static_cast<float>(pan);
        float rightGain = static_cast<float>(pan);

        bufferToFill.buffer->applyGain(0, bufferToFill.startSample, bufferToFill.numSamples, leftGain);
        bufferToFill.buffer->applyGain(1, bufferToFill.startSample, bufferToFill.numSamples, rightGain);
    }

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
    float currentGain = transportSource.getGain();
    const int fadeSteps = 20;
    const float decrement = currentGain / fadeSteps;

    for (int i = 0; i < fadeSteps; ++i)
    {
        transportSource.setGain(currentGain - decrement * i);
        juce::Thread::sleep(150);
    }

    transportSource.stop();
    transportSource.setGain(currentGain);

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
void PlayerAudio::goToEnd()
{
    transportSource.setPosition(transportSource.getLengthInSeconds());
}
void PlayerAudio::unmute()
{
    if (isMuted)
    {
        transportSource.setGain(previousGain);
        isMuted = false;
    }
}

void PlayerAudio::setPlaybackRate(float rate)
{
    if (rate <= 0.0f || rate > 3.0f) return;

    if (!readerSource) return;

    auto* reader = readerSource->getAudioFormatReader();
    if (!reader) return;


    bool wasPlaying = transportSource.isPlaying();
    double currentPos = transportSource.getCurrentPosition();


    transportSource.stop();
    transportSource.setSource(nullptr);

    double newSampleRate = reader->sampleRate * rate;
    transportSource.setSource(readerSource.get(), 0, nullptr, newSampleRate);


    if (wasPlaying)
    {
        transportSource.setPosition(currentPos);
        transportSource.start();
    }

    playbackRate = rate;
}

float PlayerAudio::getPlaybackRate() const
{
    return playbackRate;
}


void PlayerAudio::setLooping(bool shouldLoop)
{
    islooping = shouldLoop;
    if (readerSource != nullptr)
    {
        readerSource->setLooping(shouldLoop);
    }
}

bool PlayerAudio::isLooping() const
{
    return islooping;
}

void PlayerAudio::setLoopPointA(double position)
{
    pointA = position;
}

void PlayerAudio::setLoopPointB(double position)
{
    pointB = position;
}

void PlayerAudio::enableSegmentLoop(bool shouldLoop)
{
    isSegmentLooping = shouldLoop;
}

void PlayerAudio::checkSegmentLoop()
{
    if (isSegmentLooping && pointA >= 0 && pointB > pointA)
    {
        double pos = transportSource.getCurrentPosition();
        if (pos >= pointB)
        {
            transportSource.setPosition(pointA);
        }
    }
}

bool PlayerAudio::isPlaying() const
{
    return transportSource.isPlaying();
}





