#include "MainComponent.h"

MainComponent::MainComponent()
{
    player1 = std::make_unique<PlayerGUI>();
    addAndMakeVisible(player1.get());
    mixer.addInputSource(&player1->playerAudio, false);
    player2 = std::make_unique<PlayerGUI>();
    mixer.addInputSource(&player2->playerAudio, false);
     addAndMakeVisible(player2.get());

    setSize(500,400);
    setAudioChannels(0, 2);
}

MainComponent::~MainComponent()
{
    shutdownAudio();
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    mixer.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    mixer.getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources()
{
    mixer.releaseResources();
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
}

void MainComponent::resized()
{
    player1->setBounds(0, 0, getWidth(), getHeight() / 2);
    player2->setBounds(0, getHeight() / 2, getWidth(), getHeight() / 2);
}


