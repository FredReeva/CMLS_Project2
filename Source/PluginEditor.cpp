

#include "PluginProcessor.h"
#include "PluginEditor.h"


//constructor of GUI
DistortionAudioProcessorEditor::DistortionAudioProcessorEditor(DistortionAudioProcessor& p) : AudioProcessorEditor (&p), processor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (500, 230);
    
    // add controls to the constructor, defining parameters and making it visible

    
    // gain
    addAndMakeVisible(&gainPot);
    gainPot.setSliderStyle(Slider::RotaryVerticalDrag);
    gainPot.setRange(0.0 , 100.0, 1.0);
    gainPot.setValue(0.0);
    gainPot.setTextBoxStyle(Slider::TextBoxBelow, false, 80, 20);

    addAndMakeVisible(gainLabel);
    gainLabel.attachToComponent(&gainPot, false);
    gainLabel.setText("Gain",dontSendNotification);


    // tone
    addAndMakeVisible(&tonePot);
    tonePot.setSliderStyle(Slider::RotaryVerticalDrag);
    tonePot.setRange(0.0 , 100.0, 1.0);
    tonePot.setValue(65.0);
    tonePot.setTextBoxStyle(Slider::TextBoxBelow, false, 80, 20);

    addAndMakeVisible(toneLabel);
    toneLabel.attachToComponent(&tonePot, false);
    toneLabel.setText("Tone", dontSendNotification);
    
    
    // volume
    addAndMakeVisible(&volumePot); 
    volumePot.setSliderStyle(Slider::RotaryVerticalDrag);
    volumePot.setRange(0.0 , 100.0, 1.0);
    volumePot.setValue(0.1);
    volumePot.setTextBoxStyle(Slider::TextBoxBelow, false, 80, 20);
    
    addAndMakeVisible(volumeLabel);
    volumeLabel.attachToComponent(&volumePot, false);
    volumeLabel.setText("Volume", dontSendNotification);
    

    // dist type
    addAndMakeVisible(typeParam);
    typeParam.setSliderStyle(Slider::LinearVertical);
    typeParam.setRange(1, 2, 1);
    typeParam.setValue(1);
    typeParam.setTextBoxStyle(Slider::TextBoxBelow, false, 80, 20);

    addAndMakeVisible(typeLabel);
    typeLabel.attachToComponent(&typeParam, false);
    typeLabel.setText("Type", dontSendNotification);





    getLookAndFeel().setColour(Slider::thumbColourId,Colours::red);
    
    
    // add listener to the potentiometers (to change value)
    gainPot.addListener(this);
    tonePot.addListener(this);
    volumePot.addListener(this);
    typeParam.addListener(this);
    
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
    g.setFont (14.0f);

    //g.drawText ("Distortion", 75, 10, 50, 20, Justification::centred, true);
    //g.drawText("Tone", 175, 80, 50, 20, Justification::centred, true);
    //g.drawText("Volume", 275, 80, 50, 20, Justification::centred, true);
}

//resized method
void DistortionAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    const int margin = 50; //margin from parent window
    const int l = 100; //width and height

    // x position, y position, width, height (measured wrt top left)
    
    gainPot.setBounds(margin, margin,l,l);
    
    tonePot.setBounds(margin +l, margin,l,l);
    
    volumePot.setBounds(margin +2*l, margin,l,l);
    
    typeParam.setBounds(margin +3*l, margin, l, l);
  

}

// method that will be called when the pot value is changed (and will actually change the variable)
// this method links the editor with the processor!
void DistortionAudioProcessorEditor::sliderValueChanged(Slider* slider)
{
    processor.gainValue = gainPot.getValue();
    
    processor.toneValue = tonePot.getValue();

    processor.volumeValue = volumePot.getValue();

    processor.typeValue = typeParam.getValue();

}


