
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

    int selectedOversampling = 0;
    

    // update tone parameter
    void updateFilter();
    
    

private:

    // declare variables used in processor.cpp as private members of the processor class
    float lastSampleRate; //may be useless
    float signal;
    float volume;
    float tone;
    float gain;
    int function;

    //valve parameters
    float q = -0.7;
    float dist = 8;

    // distortion
    float distortionEffect(float input, int parameter);

    // tone (duplicates for both channels)

    dsp::ProcessorDuplicator<dsp::IIR::Filter <float>, dsp::IIR::Coefficients <float>> toneFilter;

    // oversampling
    int oversamplingFactor = 2;
    ScopedPointer<dsp::Oversampling<float>> oversam;


    //==============================================================================
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DistortionAudioProcessor)

};
