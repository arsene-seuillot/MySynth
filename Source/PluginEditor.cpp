/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MySynthAudioProcessorEditor::MySynthAudioProcessorEditor (MySynthAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    
    // Slider du TailOff
    addAndMakeVisible(fader_tailoff);
    fader_tailoff.setBounds(50, 50, 50, 200);
    fader_tailoff.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    fader_tailoff.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 32);
    fader_tailoff.setRange(0.0,1, 0.9999);
    fader_tailoff.setColour(juce::Slider::ColourIds::trackColourId, juce::Colours::whitesmoke);
    fader_tailoff.setDoubleClickReturnValue(true, 0.0);
    // On attache le fader au paramètre "gain" de treeState.
    faderTailoffAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, "tail-off", fader_tailoff);
    
    // Slider du TailIn
    addAndMakeVisible(fader_tailin);
    fader_tailin.setBounds(120, 50, 50, 200);
    fader_tailin.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    fader_tailin.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 32);
    fader_tailin.setRange(0.0,1, 0.9999);
    fader_tailin.setColour(juce::Slider::ColourIds::trackColourId, juce::Colours::whitesmoke);
    fader_tailin.setDoubleClickReturnValue(true, 0.0);
    // On attache le fader au paramètre "gain" de treeState.
    faderTailinAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, "tail-in", fader_tailin);
    
    // Implémentation du Label
    // Configuration du label
    infoLabel.setJustificationType(juce::Justification::centred);
    infoLabel.setText("Frequency : 0 Hz", juce::dontSendNotification);
    addAndMakeVisible(infoLabel);
    startTimer(100); // Rafraîchissement toutes les 100ms
    
    // Taille de la fenêtre
    setSize (400, 300);
}

MySynthAudioProcessorEditor::~MySynthAudioProcessorEditor()
{
}

//==============================================================================
void MySynthAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (15.0f));
    g.drawFittedText ("Hell World...", getLocalBounds(), juce::Justification::centred, 1);
}

void MySynthAudioProcessorEditor::resized()
{
    infoLabel.setBounds(0, 0, getWidth() - 100, 40);
}

void MySynthAudioProcessorEditor::timerCallback()
{
    infoLabel.setText("Ceci est un test", juce::dontSendNotification);
}
