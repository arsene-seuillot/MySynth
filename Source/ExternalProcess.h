/*
  ==============================================================================

    ExternalProcess.h
    Created: 21 Mar 2025 9:19:42pm
    Author:  Arsène SEUILLOT

  ==============================================================================
*/


#pragma once

#include <JuceHeader.h>

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

private:
    juce::AudioBuffer<float> buffer;
    std::mutex bufferMutex;
};


