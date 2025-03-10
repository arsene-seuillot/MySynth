#pragma once

#include <JuceHeader.h>

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

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MySynthAudioProcessor)
    
    float phase = 0.0f;
    float frequency = 440.0f;
    float currentSampleRate = 44100.0;
    bool isNotePlaying = false;  // Variable pour suivre si une note est active
    
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
    SineWaveVoice() : tailoff(false), tailoffFactor(0.9999f), currentAngle(0.0), angleDelta(0.0), level(0.0), frequency(440.0f) {}

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
        level = velocity;
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
    }

    void setTailOff(float newTailOff)
    {
        tailoffFactor = newTailOff;
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
                
                // Appliquer la décroissance (tail-off)
                if (tailoff)
                {
                    level *= (1-(0.5*tailoffFactor*tailoffFactor)*0.0005);
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
    double currentAngle;
    double angleDelta;
    double level;
    double frequency;
    bool tailoff;
    float tailoffFactor;  // Facteur de décroissance
};

//==============================================================================
// SineWaveSound
class SineWaveSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote(int /*midiNoteNumber*/) override { return true; }
    bool appliesToChannel(int /*midiChannel*/) override { return true; }
};
