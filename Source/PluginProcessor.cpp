
#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "math.h"

DistortionAudioProcessor::DistortionAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", AudioChannelSet::stereo(), true)
#endif
    ), // initialization list (toneFilter, parameterState)
    toneFilter(dsp::IIR::Coefficients<float>::makeLowPass(44100, 20000.f, 0.1)),
    parameterState(*this, nullptr, "PARAMETER", { createParameterLayout() })

#endif
{
    // oversampling constructor
    oversam = new dsp::Oversampling<float>(2, oversamplingFactor, dsp::Oversampling<float>::FilterType::filterHalfBandFIREquiripple, true);
    // value tree where parameters are stored
    parameterState.state = ValueTree("savedParams");
}

//distructor
DistortionAudioProcessor::~DistortionAudioProcessor()
{
}

// this method contains all the parameters linked to the editor and their values
AudioProcessorValueTreeState::ParameterLayout DistortionAudioProcessor::createParameterLayout()
{
    //generate a vector 
    std::vector <std::unique_ptr<RangedAudioParameter>> params;

    //parameters and their values (id, Name, startValue, endValue, defaultValue)
    auto gainParam = std::make_unique<AudioParameterFloat>("gain", "Gain", 0.f, 100.f, 1.f);
    auto volumeParam = std::make_unique<AudioParameterFloat>("volume", "Volume", 0.f, 1.f, 0.01f);
    auto toneParam = std::make_unique<AudioParameterFloat>("tone", "Tone", 250.f, 3000.f, 1500.f);
    auto typeParam = std::make_unique<AudioParameterInt>("type","Type",1,7,1);
    auto oversamplingCheckParam = std::make_unique<AudioParameterBool>("oversampling", "Oversampling", false);

    //"writes" the vector
    params.push_back(std::move(gainParam));
    params.push_back(std::move(toneParam));
    params.push_back(std::move(volumeParam));
    params.push_back(std::move(typeParam));
    params.push_back(std::move(oversamplingCheckParam));

    // return the vector to be put in the initialization list memeber "parameterState"
    return { params.begin(), params.end() };
}

//==============================================================================

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

    // initialize oversampling
    oversam->initProcessing(static_cast<size_t> (samplesPerBlock));

    // initialize dsp iir filter (tone)
    dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    toneFilter.prepare(spec);
    toneFilter.reset();

}

void DistortionAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

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
    volume = parameterState.getRawParameterValue("volume"); 
    gain = parameterState.getRawParameterValue("gain");
    checkOS = parameterState.getRawParameterValue("oversampling");
    type = parameterState.getRawParameterValue("type");

    //function = typeValue;

    dsp::AudioBlock<float> inputBlock(buffer);

    //tone: process and update value (not working properly)

    toneFilter.process(dsp::ProcessContextReplacing<float>(inputBlock));
    updateFilter();

    if (*checkOS == 0.f)
    {
        for (int channel = 0; channel < inputBlock.getNumChannels(); ++channel) //scan the channels
        {
            auto* channelSignal = inputBlock.getChannelPointer(channel); // get a ponter to 

            for (int sample = 0; sample < inputBlock.getNumSamples(); ++sample)
            {
                channelSignal[sample] = (*volume) * distortionEffect(channelSignal[sample] * (*gain), *type);
            }

        }
    }
    else if (*checkOS == 1.f)
    {
        dsp::AudioBlock<float> oversampledBlock;

        oversampledBlock = oversam->processSamplesUp(inputBlock); // Upsample...

        for (int channel = 0; channel < oversampledBlock.getNumChannels(); ++channel) //scan the channels
        {
            auto* channelSignal = oversampledBlock.getChannelPointer(channel); // get a ponter to 

            for (int sample = 0; sample < oversampledBlock.getNumSamples(); ++sample)
            {
                channelSignal[sample] = (*volume) * distortionEffect(channelSignal[sample] * (*gain), *type);
            }
        }

        oversam->processSamplesDown(inputBlock); // Downsample... (filter included)
    }

    
     
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

    // get parameters when opening a saved project in daw
    std::unique_ptr <XmlElement> xml (parameterState.state.createXml());
    copyXmlToBinary(*xml, destData);
}

void DistortionAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.

    // store parameters when closing daw and saving project
    std::unique_ptr <XmlElement> savedParameters(getXmlFromBinary(data, sizeInBytes));

    if (savedParameters != nullptr)
    {
        if (savedParameters->hasTagName(parameterState.state.getType()))
        {
            parameterState.state = ValueTree::fromXml(*savedParameters);
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DistortionAudioProcessor();
}

// process signal (apply distortion)
float DistortionAudioProcessor::distortionEffect(float in, int function) // implement various distortions 
{
    float out;
    // implement various distortions: 
    switch (function) {

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
            out = (in > 0) ? 1.f : -1.f;
        }
        break;

    case(5): // valve simulation
        if (in >= 0) { out = (2.f / float_Pi) * atan(in); }
        else {
            if (in == q) {
                out = (1.f / dist) + (q / (1.f - exp(q * dist)));
            }
            else {
                out = ((in - q) / (1.f - exp(-dist * (in - q)))) + (q / (1.f - exp(dist * q)));
            }
        }
        break;

    case(6): // rectifier
        if (in >= 0) { out = (2.f / float_Pi) * atan(in); }
        else {
            out = 0.f;
        }
        break;

    case(7): // octave rectifier
        if (in >= 0) { out = (2.f / float_Pi) * atan(in); }
        else {
            out = - (2.f / float_Pi) * atan(in);
        }
        break;
    }
    return out;
}

// method created for updating tone control
void DistortionAudioProcessor::updateFilter()
{
    auto tone = parameterState.getRawParameterValue("tone"); //change here the behaviour of the pot (lin/log...)   tone = 80+0.5*pow(toneValue,2.25);

    *toneFilter.state = *dsp::IIR::Coefficients<float>::makeLowPass(lastSampleRate, (*tone)); // (samplerate, frequency, Q, multiplier)
}