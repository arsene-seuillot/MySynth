/*
  ==============================================================================

    ExternalProcess.cpp
    Created: 21 Mar 2025 9:19:30pm
    Author:  Arsène SEUILLOT

  ==============================================================================
*/

#include "ExternalProcess.h"

ExternalProcess::ExternalProcess() {}
ExternalProcess::~ExternalProcess() {}

void ExternalProcess::start() {}

void ExternalProcess::stop() {}

void ExternalProcess::processAudio(const juce::AudioBuffer<float>& inputBuffer)
{
    std::lock_guard<std::mutex> lock(bufferMutex);
    
    if (buffer.getNumSamples() != inputBuffer.getNumSamples())
        buffer.setSize(inputBuffer.getNumChannels(), inputBuffer.getNumSamples(), false, false, true);

    buffer.makeCopyOf(inputBuffer);
}

juce::AudioBuffer<float>& ExternalProcess::getBuffer()
{
    return buffer;
}

float ExternalProcess::getRMSLevel()
{
    std::lock_guard<std::mutex> lock(bufferMutex);

    if (buffer.getNumSamples() == 0)
        return 0.0f;

    float totalRMS = 0.0f;

    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        totalRMS += buffer.getRMSLevel(channel, 0, buffer.getNumSamples());

    return totalRMS / static_cast<float>(buffer.getNumChannels()); // Moyenne sur tous les canaux
    //return 10.0f;
}

DetectedNote ExternalProcess::analyzeAudio()
{
    std::lock_guard<std::mutex> lock(bufferMutex);

    DetectedNote note;
    
    if (buffer.getNumSamples() == 0)
        return note;

    // Détection de l'intensité (RMS)
    float rms = getRMSLevel();
    note.velocity = juce::jmap(rms, 0.0f, 0.1f, 0.0f, 1.0f); // Normalisation

    // Détection de la fréquence dominante
    note.frequency = estimateFrequency(buffer);

    // Active la note si l'intensité dépasse un seuil
    note.isActive = note.velocity > 0.01f;

    return note;
}

float ExternalProcess::estimateFrequency(const juce::AudioBuffer<float>& buffer)
    {
        // Implémente une analyse FFT ici (placeholder)
        return 440.0f; // Exemple : toujours 440Hz pour tester
    }
