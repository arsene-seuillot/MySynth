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
    
    addAndMakeVisible(fader);
    fader.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    fader.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 32);
    fader.setRange(0.0,1, 0.9999);
    fader.setColour(juce::Slider::ColourIds::trackColourId, juce::Colours::whitesmoke);
    fader.setDoubleClickReturnValue(true, 0.0);
    
    // On attache le fader au paramètre "gain" de treeState.
    faderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, "tail-off", fader);
    
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
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void MySynthAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
