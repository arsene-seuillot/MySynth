/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class MySynthAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Timer
{
public:
    MySynthAudioProcessorEditor (MySynthAudioProcessor&);
    ~MySynthAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override; // fonction executée à chaque clic du timer

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MySynthAudioProcessor& audioProcessor;
    
    // Initialisation du fader et de son attache à une variable
    juce::Slider fader_tailoff;
    juce::Slider fader_tailin;
    juce::Slider fader_gain;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> faderTailoffAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> faderTailinAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> faderGainfAttachment;
    
    juce::Label infoLabel; // Étiquette sur le synthé pour afficher des infos dynamiquement

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MySynthAudioProcessorEditor)
};
