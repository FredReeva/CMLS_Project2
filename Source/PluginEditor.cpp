

#include "PluginProcessor.h"
#include "PluginEditor.h"


//constructor of GUI
DistortionAudioProcessorEditor::DistortionAudioProcessorEditor(DistortionAudioProcessor& p) : AudioProcessorEditor (&p), processor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (500, 230);
    
    // add controls to the constructor, defining parameters and making it visible
    
    // dist type
    smoothParam.setSliderStyle(Slider::LinearVertical);
    smoothParam.setRange(1.0, 2.0, 1.0);
    smoothParam.setTextBoxStyle(Slider::TextBoxBelow, false, 80, 20);
    //gainPot.setPopupDisplayEnabled(true, false, this);
    smoothParam.setTextValueSuffix(" Type");
    smoothParam.setValue(1.0);

    addAndMakeVisible(&smoothParam);

    
    // gain
    gainPot.setSliderStyle(Slider::RotaryVerticalDrag);
    gainPot.setRange(0.0 , 100.0, 1.0);
    gainPot.setTextBoxStyle(Slider::TextBoxBelow, false, 80, 20);
    //gainPot.setPopupDisplayEnabled(true, false, this);
    gainPot.setTextValueSuffix(" Gain");
    gainPot.setValue(0.0);
    
    addAndMakeVisible(&gainPot);

    
    // tone
    tonePot.setSliderStyle(Slider::RotaryVerticalDrag);
    tonePot.setRange(0.0 , 100.0, 1.0);
    tonePot.setTextBoxStyle(Slider::TextBoxBelow, false, 80, 20);
    //tonePot.setPopupDisplayEnabled(true, false, this);
    tonePot.setTextValueSuffix(" Tone");
    tonePot.setValue(65.0);
    
    addAndMakeVisible(&tonePot);
    
    
    // volume
    volumePot.setSliderStyle(Slider::RotaryVerticalDrag);
    volumePot.setRange(0.0 , 100.0, 1.0);
    volumePot.setTextBoxStyle(Slider::TextBoxBelow, false, 80, 20);
    //volumePot.setPopupDisplayEnabled(true, false, this);
    volumePot.setTextValueSuffix(" Volume");
    volumePot.setValue(0.1);
    
    addAndMakeVisible(&volumePot);
    
    
    // add listener to the potentiometers
    smoothParam.addListener(this);
    gainPot.addListener(this);
    tonePot.addListener(this);
    volumePot.addListener(this);
    
}

//deconstructor
DistortionAudioProcessorEditor::~DistortionAudioProcessorEditor()
{
}

//==============================================================================

//paint method
void DistortionAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    g.setColour (Colours::white);
    g.setFont (10.0f);

    g.drawText ("Gain", 75, 80, 50, 20, Justification::centred, true);
    g.drawText("Tone", 175, 80, 50, 20, Justification::centred, true);
    g.drawText("Volume", 275, 80, 50, 20, Justification::centred, true);
}

//resized method
void DistortionAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    smoothParam.setBounds(350, 50, 100, 100);
    
    gainPot.setBounds(50,50,100,100);
    
    tonePot.setBounds(150,50,100,100);
    
    volumePot.setBounds(250,50,100,100);

}

// method that will be called when the pot value is changed (and will actually change the variable)
// this method links the editor with the processor!
void DistortionAudioProcessorEditor::sliderValueChanged(Slider* slider)
{
    processor.gainValue = gainPot.getValue();
    
    processor.toneValue = tonePot.getValue();

    processor.volumeValue = volumePot.getValue();

    processor.smoothValue = smoothParam.getValue();

}
