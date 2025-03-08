/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MySynthAudioProcessor::MySynthAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

MySynthAudioProcessor::~MySynthAudioProcessor()
{
}

//==============================================================================
const juce::String MySynthAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MySynthAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool MySynthAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool MySynthAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double MySynthAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MySynthAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int MySynthAudioProcessor::getCurrentProgram()
{
    return 0;
}

void MySynthAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String MySynthAudioProcessor::getProgramName (int index)
{
    return {};
}

void MySynthAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void MySynthAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
}

void MySynthAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MySynthAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void MySynthAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    buffer.clear(); // Effacer le buffer pour éviter des artefacts sonores
    
    // Analyse des messages MIDI
        for (const auto metadata : midiMessages)
        {
            auto message = metadata.getMessage();
            
            // Gestion des messages NoteOn
            if (message.isNoteOn())
            {
                frequency = juce::MidiMessage::getMidiNoteInHertz(message.getNoteNumber());
                isNotePlaying = true;  // La note commence à jouer
                phase = 0.0f;  // Réinitialisation de la phase pour éviter des clics
            }
            // Gestion des messages NoteOff
            else if (message.isNoteOff())
            {
                isNotePlaying = false;  // La note est relâchée
            }
        }
    

    auto* leftChannel = buffer.getWritePointer(0);
    auto* rightChannel = buffer.getWritePointer(1);
    
    for (int i = 0; i < buffer.getNumSamples(); ++i)
    {        
        if (isNotePlaying) {
            float sample = std::sin(phase); // Joue uniquement si une note est active
            phase += 2.0f * juce::MathConstants<float>::pi * frequency / currentSampleRate;
            
            if (phase > 2.0f * juce::MathConstants<float>::pi)
                phase -= 2.0f * juce::MathConstants<float>::pi;
            
            leftChannel[i] = sample;
            rightChannel[i] = sample;
        }
    }
}



//==============================================================================
bool MySynthAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* MySynthAudioProcessor::createEditor()
{
    return new MySynthAudioProcessorEditor (*this);
}

//==============================================================================
void MySynthAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void MySynthAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MySynthAudioProcessor();
}
