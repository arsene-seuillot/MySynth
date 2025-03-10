/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>



//==============================================================================
/**
*/
class MySynthAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    MySynthAudioProcessor();
    ~MySynthAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MySynthAudioProcessor)
    
    float phase = 0.0f;
    float frequency = 440.0f;
    float currentSampleRate = 44100.0;
    bool isNotePlaying = false; // Variable pour suivre si une note est active
    
    juce::Synthesiser synth;
};


class SineWaveVoice : public juce::SynthesiserVoice
{
    
public:
    SineWaveVoice() {}

    bool canPlaySound(juce::SynthesiserSound* sound) override
    {
        return true;
    }

    void startNote(int midiNoteNumber, float velocity,
                   juce::SynthesiserSound*, int currentPitchWheelPosition) override
    {
        auto sampleRate = getSampleRate();
        if (sampleRate <= 0.0) return; // Vérifie que le sample rate est valide

        currentAngle = 0.0;
        level = velocity;
        frequency = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
        angleDelta = (juce::MathConstants<double>::twoPi * frequency) / sampleRate;
    }


    void stopNote(float velocity, bool allowTailOff) override
    {
        if (allowTailOff)
        {
            if (tailoff == false) tailoff = true;
        }
        else
        {
            level = 0.0;
            clearCurrentNote();
        }
    }
    
    // C'est cette fonction qui est appelée en boucle pour générer le son
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override
    {
        if (angleDelta > 0.0)
        {
            // On boucle sur tous les samples
            for (int sample = 0; sample < numSamples; ++sample)
            {
                float sampleValue = std::sin(currentAngle) * level;
                for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
                    outputBuffer.addSample(channel, startSample, sampleValue);

                currentAngle += angleDelta;
                if (currentAngle > juce::MathConstants<double>::twoPi)
                                currentAngle -= juce::MathConstants<double>::twoPi;
                
                // On fait diminuer le volume de la note jusqu'à la supprimer
                if (tailoff == true)
                {
                    level *= 0.9999;
                    
                    if (level < 0.001)
                    {
                        clearCurrentNote();
                        level = 0.0;
                        tailoff = false;
                        break;
                    }
                }
                ++startSample;
            }
        }
    }

    void pitchWheelMoved(int newPitchWheelValue) override {}
    void controllerMoved(int controllerNumber, int newControllerValue) override {}

private:
    double currentAngle = 0.0;
    double angleDelta = 0.0;
    double level = 0.0;
    double frequency = 440.0;
    bool tailoff;
};



class SineWaveSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote(int /*midiNoteNumber*/) override { return true; }
    bool appliesToChannel(int /*midiChannel*/) override { return true; }
};

        
