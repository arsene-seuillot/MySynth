/*
  ==============================================================================

    ExternalProcess.cpp
    Created: 21 Mar 2025 9:19:30pm
    Author:  Arsène SEUILLOT

  ==============================================================================
*/

#include "ExternalProcess.h"

ExternalProcess::ExternalProcess()
{
    audioDeviceManager.initialise(1, 0, nullptr, true); // 1 entrée (micro), 0 sortie
    audioDeviceManager.addAudioCallback(this);
}

ExternalProcess::~ExternalProcess()
{
    audioDeviceManager.removeAudioCallback(this);
}

void ExternalProcess::start()
{
    audioDeviceManager.restartLastAudioDevice();
}

void ExternalProcess::stop()
{
    audioDeviceManager.closeAudioDevice();
}

void ExternalProcess::audioDeviceAboutToStart(juce::AudioIODevice* device)
{
    const int numSamples = device->getDefaultBufferSize();
    buffer.setSize(1, numSamples); // 1 canal, N samples
}

void ExternalProcess::audioDeviceStopped()
{
    buffer.clear();
}

void ExternalProcess::audioDeviceIOCallback(const float** inputChannelData, int numInputChannels,
                                            float** outputChannelData, int numOutputChannels,
                                            int numSamples)
{
    if (numInputChannels > 0 && inputChannelData[0] != nullptr)
    {
        std::lock_guard<std::mutex> lock(bufferMutex);
        buffer.copyFrom(0, 0, inputChannelData[0], numSamples); // Stocke l'audio du micro
    }

    // Efface la sortie pour éviter du bruit
    for (int i = 0; i < numOutputChannels; ++i)
        juce::FloatVectorOperations::clear(outputChannelData[i], numSamples);
}

juce::AudioBuffer<float>& ExternalProcess::getBuffer()
{
    return buffer;
}

