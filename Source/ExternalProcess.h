/*
  ==============================================================================

    ExternalProcess.h
    Created: 21 Mar 2025 9:19:42pm
    Author:  Arsène SEUILLOT

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <mutex>

// Structure représentant une note détectée
struct DetectedNote {
    float frequency = 0.0f;  // Fréquence détectée
    float velocity = 0.0f;   // Intensité détectée
    bool isActive = false;   // Note active ou non
};

class ExternalProcess
{
public:
    ExternalProcess();
    ~ExternalProcess();

    void start();
    void stop();

    // Processus principal : copie l'audio du buffer reçu du DAW
    void processAudio(const juce::AudioBuffer<float>& inputBuffer);

    // Récupérer le buffer audio stocké
    juce::AudioBuffer<float>& getBuffer();
    
    // Retourne l'intensité sonore du buffer
    float getRMSLevel();

    // Analyse l'audio et retourne une note détectée
    DetectedNote analyzeAudio();
    float estimateFrequency(const juce::AudioBuffer<float>& buffer);

private:
    juce::AudioBuffer<float> buffer;
    std::mutex bufferMutex;
};
