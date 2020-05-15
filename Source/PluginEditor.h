
#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"



// declare all the classes that will be used in PluginEditor.cpp

class DistortionAudioProcessorEditor  : public AudioProcessorEditor 
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
    
    DistortionAudioProcessor& processor; // reference to DistortionAudioProcessor (?)
    
    // declare variables (potentiometers,menus,checkbox) + labels
    
    Slider gainPot;
    Label gainLabel;
    
    Slider tonePot;
    Label toneLabel;
      
    Slider volumePot;
    Label volumeLabel;
   
    Slider typeParam;
    Label typeLabel;

    ComboBox typeMenu;

    ToggleButton checkOversampling;


public:
    // attachments editor-processor. They MUST be put AFTER the declaration of the thing they attach, 
    // otherwise crash when closing (due to bottom-up distruction)
    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> gainAttach; 
    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> volumeAttach;
    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> toneAttach;
    std::unique_ptr <AudioProcessorValueTreeState::ButtonAttachment> oversamplingAttach;
    std::unique_ptr <AudioProcessorValueTreeState::ComboBoxAttachment> typeMenuAttach;

    //==============================================================================
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DistortionAudioProcessorEditor)
};
