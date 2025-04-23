/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/
#include "FFTDisplay.h"
#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MySynthAudioProcessor::MySynthAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     //#if ! JucePlugin_IsMidiEffect
                      //#if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      //#endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     //#endif
                       ), treeState(*this, nullptr, "PARAMETERS", createParameterLayout())
#endif
{
    // On connecte le process externe
    Process.start();
    
    // Paramètres globaux du synthé
    synth.clearVoices();
    
    // On ajoute 8 voix possibles pour le synthé
    for (int i=0; i<8; i++) synth.addVoice(new SineWaveVoice());
    synth.clearSounds();
    synth.addSound(new SineWaveSound);
    
    // Listener pour les variables du synthé
    treeState.addParameterListener("gain", this);
    treeState.addParameterListener("tail-off", this);
    treeState.addParameterListener("tail-in", this);
    treeState.addParameterListener("debug", this);
    
}

MySynthAudioProcessor::~MySynthAudioProcessor()
{
    Process.stop();
    treeState.removeParameterListener("gain", this);
    treeState.removeParameterListener("tail-off", this);
    treeState.removeParameterListener("tail-in", this);
    treeState.removeParameterListener("debug", this);
    
}

// ON IMPLÉMENTE LA FONCTION QUI CRÉÉE LA LISTE DES PARAMÈTERES DE L'AUDIO-TREE
juce::AudioProcessorValueTreeState::ParameterLayout MySynthAudioProcessor::createParameterLayout() {
    
    std::vector <std::unique_ptr<juce::RangedAudioParameter>> params;
    
    // Ici on crée les différents paramètres du ParameterLayout
    auto pGain = std::make_unique<juce::AudioParameterFloat>("gain", "Gain", -24.0, 24.0, 10.0);
    auto pTailOff = std::make_unique<juce::AudioParameterFloat>("tail-off", "Tail Off", 0.0f, 1.0f, 0.5f);
    auto pTailIn = std::make_unique<juce::AudioParameterFloat>("tail-in", "Tail In", 0.0f, 1.0f, 0.5f); // Décroissance du volume
    auto pFrequency = std::make_unique<juce::AudioParameterFloat>("frequency", "Frequency", 0.1f, 23000.0f, 10000.5f);
    auto pDebug = std::make_unique<juce::AudioParameterFloat>("debug", "Debug", -100000.0f, 100000.0f, 10000.5f);
        
    params.push_back(std::move(pTailOff));
    params.push_back(std::move(pTailIn));
    params.push_back(std::move(pGain));
    params.push_back(std::move(pFrequency));
    params.push_back(std::move(pDebug));
    
    return {params.begin(), params.end()};
}

// MÉTHODE POUR NE METTRE À JOUR LES VALEURS QUE QUAND ELLES CHANGENT
void MySynthAudioProcessor::parameterChanged(const juce::String &parameterID, float newValue) {
    
    // Quand une nouvelle valeur est détectée dans le listener, newValue prend cette valeur
    // et on sait quel paramètre est concerné avec parameterID.

    if (parameterID == "gain") {
        val = juce::Decibels::decibelsToGain(newValue);
    }
    else if (parameterID == "tail-off") {
    // Met à jour le facteur de décroissance dans toutes les voix
        for (int i = 0; i < synth.getNumVoices(); ++i) {
            auto* voice = synth.getVoice(i);
            if (auto* sineVoice = dynamic_cast<SineWaveVoice*>(voice)) {
                sineVoice->setTailOff(newValue);  // Met à jour le facteur tail-off
            }
        }
    }
    else if (parameterID == "tail-in") {
    // Met à jour le facteur de croissance dans toutes les voix
        for (int i = 0; i < synth.getNumVoices(); ++i) {
            auto* voice = synth.getVoice(i);
            if (auto* sineVoice = dynamic_cast<SineWaveVoice*>(voice)) {
                sineVoice->setTailIn(newValue);  // Met à jour le facteur tail-in
            }
        }
    }
}

// Récupère la valeur du paramètre "tail-off"
float MySynthAudioProcessor::getParameterValue(const juce::String& parameterID) const
{
    return *treeState.getRawParameterValue(parameterID); // Retourne la valeur de "tail-off"
}

//==============================================================================
ExternalProcess& MySynthAudioProcessor::getExternalProcess()
{
    return Process;
}

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
    synth.setCurrentPlaybackSampleRate(sampleRate); // IMPORTANT: Définir le sample rate du synthétiseur
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

void MySynthAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    // Vérifier si l'entrée audio est valide
    if (getTotalNumInputChannels() > 0) {}
    Process.processAudio(buffer); // Stocke l'entrée audio
    float intensity = Process.getRMSLevel();
    // On stocke ici le message de debug
    *treeState.getRawParameterValue("debug") = intensity;
    
    float SoundPlayed = Process.isSoundPlayed();
    *treeState.getRawParameterValue("debug") = SoundPlayed;
    
    juce::ScopedNoDenormals noDenormals;
        
    // Partie ou on fait des dingueries !! il faut revoir toutes les nouvelles implémentations
    
    
    /*
    
    // Extraire les informations sonores à partir du buffer d'entrée
    DetectedNote detectedNote = Process.analyzeAudio();

    // Parcourir les voix de synthé et les mettre à jour avec les nouvelles infos
    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<SineWaveVoice*>(synth.getVoice(i)))
        {
            voice->playDetectedNote(detectedNote);
        }
    }

    buffer.clear(); // Efface le buffer avant de générer les nouvelles ondes
    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
     */
    
}




//==============================================================================
bool MySynthAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* MySynthAudioProcessor::createEditor()
{
    //return new juce::GenericAudioProcessorEditor(*this);
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

