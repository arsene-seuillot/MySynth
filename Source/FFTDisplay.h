/*
  ==============================================================================

    FFTDisplay.h
    Created: 23 Apr 2025 3:58:48pm
    Author:  Arsène SEUILLOT

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class FFTDisplay : public juce::Component
{
public:
    FFTDisplay();
    ~FFTDisplay() override;

    // Met à jour les données FFT à afficher
    void setFFTData(const std::vector<float>& newData);

    // Dessiner la FFT dans la fenêtre
    void paint(juce::Graphics& g) override;

private:
    std::vector<float> fftData; // Contient les magnitudes FFT

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FFTDisplay)
};
