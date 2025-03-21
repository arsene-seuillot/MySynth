/*
  ==============================================================================

    ExternalProcess.h
    Created: 21 Mar 2025 9:19:42pm
    Author:  Arsène SEUILLOT

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class ExternalProcess : public juce::AudioIODeviceCallback
{
public:
    ExternalProcess();
    ~ExternalProcess() override;

    void start();
    void stop();

    // Callbacks audio
    void audioDeviceAboutToStart(juce::AudioIODevice* device) override;
    void audioDeviceStopped() override;
    void audioDeviceIOCallback(const float** inputChannelData, int numInputChannels,
                               float** outputChannelData, int numOutputChannels,
                               int numSamples);

    juce::AudioBuffer<float>& getBuffer();

private:
    juce::AudioDeviceManager audioDeviceManager;
    juce::AudioBuffer<float> buffer;
    std::mutex bufferMutex;
};

