
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
    gainPot.setTextBoxStyle(Slider::TextBoxBelow, false, 80, 20);

    addAndMakeVisible(gainLabel);
    gainLabel.attachToComponent(&gainPot, false);
    gainLabel.setText("Gain",dontSendNotification);

    // tone
    addAndMakeVisible(&tonePot);
    tonePot.setSliderStyle(Slider::RotaryVerticalDrag);
    tonePot.setTextBoxStyle(Slider::TextBoxBelow, false, 80, 20);

    addAndMakeVisible(toneLabel);
    toneLabel.attachToComponent(&tonePot, false);
    toneLabel.setText("Tone", dontSendNotification);
    
    // volume
    addAndMakeVisible(&volumePot); 
    volumePot.setSliderStyle(Slider::RotaryVerticalDrag);
    volumePot.setTextBoxStyle(Slider::TextBoxBelow, false, 80, 20);
    
    addAndMakeVisible(volumeLabel);
    volumeLabel.attachToComponent(&volumePot, false);
    volumeLabel.setText("Volume", dontSendNotification);
    
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
    typeMenu.addItem("Rectifier", 6);
    typeMenu.addItem("Octave Rectifier", 7);

    // checkbox oversampling
    addAndMakeVisible(checkOversampling);
    checkOversampling.setButtonText("Oversampling (x4)");

    // attachments Editor-Processor (used instead of listeners)
    gainAttach = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.parameterState, "gain", gainPot);
    volumeAttach = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.parameterState, "volume", volumePot);
    toneAttach = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.parameterState, "tone", tonePot);
    oversamplingAttach = std::make_unique<AudioProcessorValueTreeState::ButtonAttachment>(processor.parameterState, "oversampling", checkOversampling);
    typeMenuAttach = std::make_unique<AudioProcessorValueTreeState::ComboBoxAttachment>(processor.parameterState, "type", typeMenu);
    
    // style/colours of the plugin
    getLookAndFeel().setColour(Slider::thumbColourId,Colours::red);
    

}

//distructor
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
    g.setFont(14.0f);
}

//resized method
void DistortionAudioProcessorEditor::resized()
{

    const int l = 100; // width and height of pots
    const int spacing = 20;
    const int h_margin = (getWidth()-3*l)/2; // horizontal margin from parent window (for 3 pots width)
    const int v_margin = (getHeight() - l) / 2; // vertical margin from parent window (for 1 pot + menu height)
    const int menu_width = 140; // width of the menu
    

    // x position, y position, width, height (measured from top left)
    
    // oversampling checkbox
    checkOversampling.setBounds((2.08)*getWidth() / 3 - menu_width / 2, v_margin + l + spacing / 2, menu_width, 20);

    // menu of functions
    typeMenu.setBounds(getWidth()/3-menu_width/2 ,v_margin+l+spacing/2, menu_width, 20);

    // potentiometers
    gainPot.setBounds(h_margin, v_margin-spacing/2,l,l);
    
    tonePot.setBounds(h_margin+l, v_margin-spacing/2,l,l);
    
    volumePot.setBounds(h_margin+2*l, v_margin-spacing/2,l,l);

}