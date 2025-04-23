/*
  ==============================================================================

    ExternalProcess.cpp
    Created: 21 Mar 2025 9:19:30pm
    Author:  Arsène SEUILLOT

  ==============================================================================
*/

#include "ExternalProcess.h"

ExternalProcess::ExternalProcess() : fft(11)
{
    // Initialisation des buffers
    fftData.resize(2048);      // Réserve de la mémoire pour les résultats FFT
    windowedBuffer.resize(2048); // Réserve de la mémoire pour le buffer avec fenêtre (par exemple, fenêtre Hamming)
}
ExternalProcess::~ExternalProcess() {}

void ExternalProcess::start() {}

void ExternalProcess::stop() {}

void ExternalProcess::processAudio(const juce::AudioBuffer<float>& inputBuffer)
{
    std::lock_guard<std::mutex> lock(bufferMutex);
    
    if (process_buffer.getNumSamples() != inputBuffer.getNumSamples())
        process_buffer.setSize(inputBuffer.getNumChannels(), inputBuffer.getNumSamples(), false, false, true);

    process_buffer.makeCopyOf(inputBuffer);
    
    // Vérifie si la taille du buffer est suffisamment grande
    if (inputBuffer.getNumSamples() < 2048)
    return;
    
    // Appliquer une fenêtre Hamming
    for (int i = 0; i < 2048; ++i)
    {
        windowedBuffer[i] = inputBuffer.getReadPointer(0)[i] *
                            (0.54f - 0.46f * cosf(2.0f * juce::MathConstants<float>::pi * i / 2048.0f));
    }
    
    // Copie le signal fenêtré dans fftData
    std::copy(windowedBuffer.begin(), windowedBuffer.end(), fftData.begin());

    // Calculer la FFT
    fft.performFrequencyOnlyForwardTransform(fftData.data());

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
    //std::lock_guard<std::mutex> lock(bufferMutex); // Protéger l'accès au buffer

    float rmsLevel = getRMSLevel(); // Utilise directement la méthode existante
    float threshold = 0.01f; // Ajuste selon le bruit ambiant

    return rmsLevel > threshold;
}

std::vector<float> ExternalProcess::getFFTData()
{
    std::lock_guard<std::mutex> lock(bufferMutex);

    // On renvoie uniquement la moitié basse du spectre (car la FFT est symétrique)
    return std::vector<float>(fftData.begin(), fftData.begin() + fftData.size() / 2);
}

