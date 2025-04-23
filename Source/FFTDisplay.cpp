/*
  ==============================================================================

    FFTDisplay.cpp
    Created: 23 Apr 2025 3:58:34pm
    Author:  Arsène SEUILLOT

  ==============================================================================
*/

#include "FFTDisplay.h"

FFTDisplay::FFTDisplay()
{
    setSize(400, 200); // Définir la taille de la boîte où l'on veut dessiner
}

FFTDisplay::~FFTDisplay() {}

void FFTDisplay::setFFTData(const std::vector<float>& newData)
{
    fftData = newData;
    repaint(); // Redessiner à chaque fois que les données changent
}

void FFTDisplay::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black); // Fond noir

    if (fftData.empty())
        return;

    // Dessiner les lignes de la FFT
    g.setColour(juce::Colours::green);

    float width = static_cast<float>(getWidth());
    float height = static_cast<float>(getHeight());
    float maxMagnitude = 0.0f;

    // Chercher la magnitude maximale pour la normalisation
    for (float value : fftData)
    {
        if (value > maxMagnitude)
            maxMagnitude = value;
    }

    // Dessiner la FFT (graphique en ligne)
    for (size_t i = 0; i < fftData.size(); ++i)
    {
        float magnitude = fftData[i];
        float x = width * (static_cast<float>(i) / fftData.size());
        float y = height - (magnitude / maxMagnitude) * height; // Normaliser la magnitude pour l'affichage

        if (i > 0)
        {
            float prevX = width * (static_cast<float>(i - 1) / fftData.size());
            float prevY = height - (fftData[i - 1] / maxMagnitude) * height;
            g.drawLine(prevX, prevY, x, y);
        }
    }
}

