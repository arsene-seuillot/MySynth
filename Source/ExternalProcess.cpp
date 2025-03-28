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
    
    if (process_buffer.getNumSamples() != inputBuffer.getNumSamples())
        process_buffer.setSize(inputBuffer.getNumChannels(), inputBuffer.getNumSamples(), false, false, true);

    process_buffer.makeCopyOf(inputBuffer);
}

juce::AudioBuffer<float>& ExternalProcess::getBuffer()
{
    return process_buffer;
}

float ExternalProcess::getRMSLevel()
{
    std::lock_guard<std::mutex> lock(bufferMutex);

    if (process_buffer.getNumSamples() == 0)
        return 0.0f;

    float totalRMS = 0.0f;

    for (int channel = 0; channel < process_buffer.getNumChannels(); ++channel)
        totalRMS += process_buffer.getRMSLevel(channel, 0, process_buffer.getNumSamples());

    return totalRMS / static_cast<float>(process_buffer.getNumChannels()); // Moyenne sur tous les canaux
    //return 10.0f;
}

DetectedNote ExternalProcess::analyzeAudio()
{
    std::lock_guard<std::mutex> lock(bufferMutex);

    DetectedNote note;
    
    if (process_buffer.getNumSamples() == 0)
        return note;

    // Détection de l'intensité (RMS)
    float rms = getRMSLevel();
    note.velocity = juce::jmap(rms, 0.0f, 0.1f, 0.0f, 1.0f); // Normalisation

    // Détection de la fréquence dominante
    note.frequency = estimateFrequency(process_buffer);

    // Active la note si l'intensité dépasse un seuil
    note.isActive = note.velocity > 0.01f;

    return note;
}

float ExternalProcess::estimateFrequency(const juce::AudioBuffer<float>& buffer)
{
    // Implémente une analyse FFT ici (placeholder)
    return 440.0f; // Exemple : toujours 440Hz pour tester
}

bool ExternalProcess::isSoundPlayed()
{
    juce::AudioBuffer<float>& micBuffer = getBuffer(); // Récupère le buffer du micro
    
    float rmsLevel = 0.0f; // Stocke le niveau RMS total
    int totalSamples = 0;

    for (int channel = 0; channel < micBuffer.getNumChannels(); ++channel)
    {
        rmsLevel += micBuffer.getRMSLevel(channel, 0, micBuffer.getNumSamples());
        totalSamples += micBuffer.getNumSamples();
    }

    rmsLevel /= micBuffer.getNumChannels(); // Moyenne des canaux

    float threshold = 0.02; // Seuil à ajuster en fonction du bruit ambiant

    return rmsLevel > threshold;
}

