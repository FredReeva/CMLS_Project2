
#pragma once

#include <JuceHeader.h>

// declare classes used in PluginProcessor.cpp

class DistortionAudioProcessor  : public AudioProcessor
{
    // declare methods/variables used in DistortionAudioProcessor class
    
public:
    
    DistortionAudioProcessor(); //constructor
    ~DistortionAudioProcessor(); //deconstructor
    
    //==============================================================================
    //various public methods...
    
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
public:

    // declare variables linked to the potentiometers (public because they need to be seen in PluginEditor class)
    float gainValue;
    float toneValue;
    float volumeValue;
    int typeValue;
    //valve parameters
    

private:

    // declare variables used in processor.cpp as private members of the processor class
    float in;
    float out;
    float volume;
    float tone;
    float gain;
    int type;
    float q = -0.2;
    float dist = 2;

    

    //==============================================================================
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DistortionAudioProcessor)

};
