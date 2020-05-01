
#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"



// declare all the classes that will be used in PluginEditor.cpp

class DistortionAudioProcessorEditor  : public AudioProcessorEditor, 
                                        private Slider::Listener
{
    // declare all the methods that will be used in PluginEditor.cpp
    
public:

    DistortionAudioProcessorEditor (DistortionAudioProcessor& p); //constructor 
    ~DistortionAudioProcessorEditor(); //distructor

    //==============================================================================
    //other public methods...
    
    void paint (Graphics&) override;
    void resized() override;
    

private:

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    
    void sliderValueChanged(Slider* slider) override; // value changes of sliders
    
    DistortionAudioProcessor& processor; // reference to DistortionAudioProcessor (?)
    
    // declare variables (potentiometers,...)

    
    Slider gainPot;
    Label gainLabel;
    
    
    Slider tonePot;
    Label toneLabel;
    
        
    Slider volumePot;
    Label volumeLabel;
    

    Slider typeParam;
    Label typeLabel;



    
    

    //==============================================================================
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DistortionAudioProcessorEditor)
};
