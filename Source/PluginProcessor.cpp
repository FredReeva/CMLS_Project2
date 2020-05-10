
#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "math.h"

//constructor of plugin
DistortionAudioProcessor::DistortionAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       ), toneFilter(dsp::IIR::Coefficients<float>::makeLowPass(44100, 20000.f, 0.1) ), halfFilter(dsp::IIR::Coefficients<float>::makeLowPass(44100, 18000.f))
#endif
{
    
}

//distructor
DistortionAudioProcessor::~DistortionAudioProcessor()
{
}

//==============================================================================
// various things don't know what they do

const String DistortionAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DistortionAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool DistortionAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool DistortionAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double DistortionAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DistortionAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int DistortionAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DistortionAudioProcessor::setCurrentProgram (int index)
{
}

const String DistortionAudioProcessor::getProgramName (int index)
{
    return {};
}

void DistortionAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================

// prepare to play method
void DistortionAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    
    // initialize things before the process block method

    lastSampleRate = sampleRate;
    signal = 0.f;
    toneValue = 1;

    K = 8; //oversampling factor

    // implement dsp iir filter
    dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();

    toneFilter.prepare(spec);
    toneFilter.reset();

    //
    dsp::ProcessSpec specOver;
    specOver.sampleRate = sampleRate * K; //*K
    specOver.maximumBlockSize = samplesPerBlock*K;
    specOver.numChannels = getTotalNumOutputChannels();

    halfFilter.prepare(specOver);
    halfFilter.reset();

    oversampledBuffer.setSize( 1 , samplesPerBlock * K);
    oversampledBuffer.clear();


}

// release resources method
void DistortionAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

// don't know (?)
#ifndef JucePlugin_PreferredChannelConfigurations
bool DistortionAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
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

// method created for updating tone control
void DistortionAudioProcessor::updateFilter()
{
    tone = toneValue; //change here the behaviour of the pot (lin/log...)   tone = 80+0.5*pow(toneValue,2.25);
    *toneFilter.state = *dsp::IIR::Coefficients<float>::makeLowPass(lastSampleRate, tone, 1.f);
}

// processor method: write here the actual code for the plugin
void DistortionAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
        
    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    
    //--------------------------------------------------------------------

    

    int numSamples = buffer.getNumSamples(); // how many samples we have in our input buffer? determined by daw
    

    float* channelOutDataL = buffer.getWritePointer(0); //pointer output left channel
    float* channelOutDataR = buffer.getWritePointer(1); //pointer output right channel
    const float* channelInData = buffer.getReadPointer(0); //pointer input channel



    // oversampling
    //const int K = 4; // N+1 number of 0s inserted in oversampling
    //oversampledBuffer.setSize(1, buffer.getNumSamples * K);

    int numOversamples = oversampledBuffer.getNumSamples();


    for (int i = 0; i < numOversamples ; ++i) {  //samplewise scanning for distortion

        //create a copy of variables (do we need it?)
        volume = volumeValue * 0.01f; // __Value are public variables
        gain = gainValue * 0.5f;
        function = typeValue;
        int j = i / K ; //index of audio sample: one in K
        
        if (i % K == 0) // every K samples...
        {
            //... perform non-linear process
            // gain stage multiplication
            signal = channelInData[j] * gain;

            // apply non linear distortion effect
            signal = distortionEffect(signal, function);

        } else { //... otherwise zero stuff
            signal = 0.f;
        }
        // save the result in the oversampled buffer
        oversampledBuffer.setSample(0,i,signal);
    }
        // apply filter to oversampled buffer



    //dsp::AudioBlock<float> overBlock(oversampledBuffer); //reads buffer to be used in dsp module
    //halfFilter.state = *dsp::IIR::Coefficients<float>::makeLowPass(getSampleRate(), 18000.f, 1.f);
    //halfFilter.process(dsp::ProcessContextReplacing<float>(overBlock));

    dsp::AudioBlock<float> block(oversampledBuffer); //reads buffer to be used in dsp module
    //tone: process and update value
    toneFilter.process(dsp::ProcessContextReplacing<float>(block));
    updateFilter();

    for (int i = 0; i < numOversamples; ++i) {  //samplewise scanning for distortion

        int j = i / K; //index of audio sample: one in K
        //downsampling
        if (i % K == 0) {
            // apply master volume 
            channelOutDataL[j] = (K-0.5f) * volume * oversampledBuffer.getSample(0, i);
            channelOutDataR[j] = channelOutDataL[j];
        }

    }

    oversampledBuffer.clear();

    //--------------------------------------------------------------------

    
    
    
}

//==============================================================================

bool DistortionAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* DistortionAudioProcessor::createEditor()
{
    return new DistortionAudioProcessorEditor (*this);
}

//==============================================================================
void DistortionAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void DistortionAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DistortionAudioProcessor();
}

float DistortionAudioProcessor::distortionEffect(float in, int type) // implement various distortions 
{
    float out;
    // implement various distortions: 
    switch (type) {

    case(1): // soft 1 (arctan)
        out = (2.f / float_Pi) * atan(in);
        break;

    case(2): // soft 2 (fract)
        out = in / (1.f + abs(in));
        break;

    case(3): // hard 1 (exponential)
        if (in >= 0.f) {
            out = (1.f - exp(-abs(in)));
        }
        else {
            out = -(1.f - exp(-abs(in)));
        }
        break;

    case(4): // hard 2 (clip)
        if ((abs(in)) <= 1.f) {
            out = in;
        }
        else {
            out = copysign(1.f, in);
        }
        break;

    case(5): // valve simulation (don't know about this...) try to change parameters
        in = 0.5 * in;
        if ( in > 0) { out = (2.f / float_Pi) * atan(in); }
        else if (in == q) {
            out = (1 / dist) + (q / (1.f - exp(q * dist)));
        }
        else {
            out = ((in - q) / (1.f - exp(-dist * (in - q)))) + (q / (1.f - exp(dist * q)));
        }
        break;
    }

    return out;
}

