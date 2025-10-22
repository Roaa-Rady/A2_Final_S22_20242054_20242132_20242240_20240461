#include "MainComponent.h"

MainComponent::MainComponent()
{
    player1 = std::make_unique<PlayerGUI>();
    addAndMakeVisible(player1.get());

    setSize(500,400);
    setAudioChannels(0, 2);
}

MainComponent::~MainComponent()
{
    shutdownAudio();
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    player1->prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    player1->getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources()
{
    player1->releaseResources();
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
}

void MainComponent::resized()
{
    player1->setBounds(20, 20, getWidth() - 40, 300);
}


