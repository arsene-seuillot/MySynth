#pragma once

#include <JuceHeader.h>
#include "ExternalProcess.h"

//==============================================================================
// MySynthAudioProcessor
class MySynthAudioProcessor  : public juce::AudioProcessor, juce::AudioProcessorValueTreeState::Listener
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
    
    // DÉCLARATION DU VALUE-TREE
    juce::AudioProcessorValueTreeState treeState;
    
    
    float getParameterValue(const juce::String& parameterID) const;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MySynthAudioProcessor)
    
    float phase = 0.0f;
    float frequency = 440.0f;
    float currentSampleRate = 44100.0;
    bool isNotePlaying = false;  // Variable pour suivre si une note est active
    
    // On ajoute un objet ExternalProcess du fichier externe
    ExternalProcess Process;
    
    // On déclare l'objet synthétiseur
    juce::Synthesiser synth;
    
    
    // On déclare les variables du synthé
    float val = 0.0f;
    
    // On crée un audio-tree pour lier des valeurs à des variables
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    void parameterChanged(const juce::String &parameterID, float newValue) override;
};

//==============================================================================
// SineWaveVoice
class SineWaveVoice : public juce::SynthesiserVoice
{
public:
    SineWaveVoice()
        : tailoff(false),
          tailoffFactor(0.9999f),
          tailinFactor(0.0f),  // Ajoute la valeur d'initialisation de tailinFactor
          currentAngle(0.0),
          angleDelta(0.0),
          level(0.0),
          frequency(440.0f)
    {}

    bool canPlaySound(juce::SynthesiserSound* sound) override
    {
        return true;
    }

    void startNote(int midiNoteNumber, float velocity,
                   juce::SynthesiserSound*, int currentPitchWheelPosition) override
    {
        auto sampleRate = getSampleRate();
        if (sampleRate <= 0.0) return;  // Vérifie que le sample rate est valide

        currentAngle = 0.0;
        volume = velocity;
        // On fait commencer à preque 0 pour le tail-in, > 0.001 pour éviter les conflits avec le tailoff
        level = 0.002;
        tailoff = false;
        frequency = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
        angleDelta = (juce::MathConstants<double>::twoPi * frequency) / sampleRate;
    }

    void stopNote(float velocity, bool allowTailOff) override
    {
        if (allowTailOff)
        {
            if (!tailoff)
                tailoff = true;
        }
        else
        {
            level = 0.0;
            clearCurrentNote();
        }
        tailoff = true;
    }

    void setTailOff(float newTailOff)
    {
        tailoffFactor = newTailOff;
    }
    void setTailIn(float newTailIn)
    {
        tailinFactor = newTailIn;
    }
    
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override
    {
        if (angleDelta > 0.0)
        {
            for (int sample = 0; sample < numSamples; ++sample)
            {
                float sampleValue = std::sin(currentAngle) * level;
                for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
                    outputBuffer.addSample(channel, startSample, sampleValue);

                currentAngle += angleDelta;
                if (currentAngle > juce::MathConstants<double>::twoPi)
                    currentAngle -= juce::MathConstants<double>::twoPi;
                
                // Appliquer une montée progressive du volume (tail-in)
                if (!tailoff && level < volume && level > 0.001)
                {
                    {
                        level += (volume - level) *(1-tailinFactor) * 0.001f; // Montée fluide
                    }
                    if (tailinFactor == 0.0f)
                    {
                        level = volume;
                    }
                    if (level > volume)
                        level = volume; // Évite de dépasser la valeur cible
                }
                
                // Appliquer la décroissance (tail-off)
                if (tailoff)
                {
                    level *= (1-(0.7*tailoffFactor*tailoffFactor)*0.0005);
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
    // Déclaration des variables
    bool tailoff;
    float tailoffFactor;  // Facteur de décroissance
    float tailinFactor;   // Facteur de croissance
    double currentAngle;
    double angleDelta;
    double level;  // volume effectif joué
    double frequency;
    double volume; // On le définit pour fixer le volume souhaité
};

//==============================================================================
// SineWaveSound
class SineWaveSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote(int /*midiNoteNumber*/) override { return true; }
    bool appliesToChannel(int /*midiChannel*/) override { return true; }
};
