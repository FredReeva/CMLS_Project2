

#include "PluginProcessor.h"
#include "PluginEditor.h"


//constructor of GUI
DistortionAudioProcessorEditor::DistortionAudioProcessorEditor(DistortionAudioProcessor& p) : AudioProcessorEditor (&p), processor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(400, 200); //minimum (300,200)
    
    // add controls to the constructor, defining parameters and making it visible

    
    // gain
    addAndMakeVisible(&gainPot);
    gainPot.setSliderStyle(Slider::RotaryVerticalDrag);
    gainPot.setRange(1.0 , 100.0, 1.0);
    gainPot.setValue(1.0); 
    gainPot.setTextBoxStyle(Slider::TextBoxBelow, false, 80, 20);

    addAndMakeVisible(gainLabel);
    gainLabel.attachToComponent(&gainPot, false);
    gainLabel.setText("Gain",dontSendNotification);


    // tone
    addAndMakeVisible(&tonePot);
    tonePot.setSliderStyle(Slider::RotaryVerticalDrag);
    tonePot.setRange(1.0 , 100.0, 1.0);
    tonePot.setValue(50.0);
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
    

    // add listener to the potentiometers (to change value)
    gainPot.addListener(this);
    tonePot.addListener(this);
    volumePot.addListener(this);
    
    // dist type menu
    addAndMakeVisible(typeMenu);
    typeMenu.addSectionHeading("Symmetric");
    typeMenu.addItem("Soft Clipping #1", 1);
    typeMenu.addItem("Soft Clipping #2", 2);
    typeMenu.addItem("Hard Clipping #1", 3);
    typeMenu.addItem("Hard Clipping #2", 4);
    typeMenu.addSeparator();
    typeMenu.addSectionHeading("Asymmetric");
    typeMenu.addItem("Valve Simulation", 5);
    typeMenu.onChange = [this] { typeMenuChanged(&typeMenu); };
    typeMenu.setSelectedId(1);

    // checkbox oversampling
    addAndMakeVisible(checkOversampling);
    checkOversampling.onClick = [this] { updateToggleState(&checkOversampling); };
    checkOversampling.setButtonText("Oversampling (x4)");
    
    getLookAndFeel().setColour(Slider::thumbColourId,Colours::red);
    

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

    const int l = 100; //width and height
    const int spacing = 20;
    const int h_margin = (getWidth()-3*l)/2; //horizontal margin from parent window (for 3 pots width)
    const int v_margin = (getHeight() - l) / 2; //vertical margin from parent window (for 1 pot+ menu height)
    const int menu_width = 140; //depends on the content
    

    // x position, y position, width, height (measured wrt top left)
    checkOversampling.setBounds((2.08)*getWidth() / 3 - menu_width / 2, v_margin + l + spacing / 2, menu_width, 20);

    typeMenu.setBounds(getWidth()/3-menu_width/2 ,v_margin+l+spacing/2, menu_width, 20);

    gainPot.setBounds(h_margin, v_margin-spacing/2,l,l);
    
    tonePot.setBounds(h_margin+l, v_margin-spacing/2,l,l);
    
    volumePot.setBounds(h_margin+2*l, v_margin-spacing/2,l,l);

  

}

// method that will be called when the pot value is changed (and will actually change the variable)
// this method links the editor with the processor!
void DistortionAudioProcessorEditor::sliderValueChanged(Slider* slider)
{
    processor.gainValue = gainPot.getValue();
    
    processor.toneValue = tonePot.getValue();

    processor.volumeValue = volumePot.getValue();


}


void DistortionAudioProcessorEditor::typeMenuChanged(ComboBox* menu)
{
    //processor.typeValue = typeMenu.getSelectedId();
    processor.typeValue = menu->getSelectedId();
}

void DistortionAudioProcessorEditor::updateToggleState(Button* button)
{
    /*auto state = button->getToggleState();
    processor.selectedOversampling = state ? true: false;*/

    processor.selectedOversampling = button->getToggleState();
}