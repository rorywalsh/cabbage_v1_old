/*
  Copyright (c) 2015 - Rory Walsh

  Cabbage is free software; you can redistribute it
  and/or modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  Cabbage is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with Csound; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
  02111-1307 USA
*/

#ifndef AUTOMATIONPLUGINEDITOR_H_INCLUDED
#define AUTOMATIONPLUGINEDITOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "AutomationProcessor.h"
#include "../CabbageCustomWidgets.h"
#include "../Table.h"


//==============================================================================
/**
*/
class AutomationEditor  : public AudioProcessorEditor,
						  public Button::Listener,
						  public ChangeListener,
                          public Timer
{
public:
    AutomationEditor (AutomationProcessor&);
    ~AutomationEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
	void buttonClicked(Button*);
	
    AutomationProcessor* getFilter() const
    {
        return static_cast <AutomationProcessor*> (getAudioProcessor());
    }
	
	void changeListenerCallback(ChangeBroadcaster* source);
	void updatefTableData(GenTable* table);
	void addTable(int tableNumber, int genRoutine);	
	void updateComboBoxItems();
	void updateScrubberPosition(double position);
    
    void timerCallback();
	
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AutomationProcessor& processor;
    int tableNumber;
    TableManager tableManager;
	ComboBox autoCombo;
	CabbageLookAndFeelBasic basicLook;
	Array<float> ampRange;
	var tableNumbers;
	float zoom;

	DrawableButton stopButton;
	DrawableButton zoomInButton;
	DrawableButton loopButton;	
	DrawableButton zoomOutButton;
	DrawableButton linkToTransport; 
	DrawableButton playButton;
	
	
	BubbleMessageComponent popupBubble;
	TextButton button;
	Array <float, CriticalSection> tableValues;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AutomationEditor)
};


#endif  // PLUGINEDITOR_H_INCLUDED
