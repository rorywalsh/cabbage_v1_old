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
#include "BreakpointEnvelope.h"


class AutomationEditor;

class AutomationDisplay : public Component,
						public Timer,
						public ChangeListener,
						private ScrollBar::Listener
{
public:
    AutomationDisplay(AutomationEditor* editor);
    ~AutomationDisplay();
	
	
    void setScrubberPos(double pos);
    void changeListenerCallback (ChangeBroadcaster*);
    void setZoomFactor (double amount);
    void setRange (Range<double> newRange);
    void paint (Graphics& g);
    void timerCallback();
    void mouseDown (const MouseEvent& e);
    void mouseDrag (const MouseEvent& e);
    void resetPlaybackPosition();
    void resized() override;
	void addNewAutomationEnvelope(Colour envColour, int tableNumber);
	void bringEnvelopeToFront(int index);

private:
	OwnedArray<BreakpointEnvelope> automationEnvelopes;
	Range<double> visibleRange;
	float totalLength;
	ScrollBar scrollbar;
    double startTime, endTime;
	Rectangle<int> localBounds;
	float sampleRate;
	double currentPlayPosition;
	Colour tableColour;
    DrawableRectangle currentPositionMarker;
	AutomationEditor* owner;
    float timeToX (const double time) const;
    double xToTime (const float x) const;
    void scrollBarMoved (ScrollBar* scrollBarThatHasMoved, double newRangeStart) override;
};

//==============================================================================
/**
*/
class AutomationEditor  : public AudioProcessorEditor,
						  public Button::Listener,
						  public ComboBox::Listener,
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
	void changeListenerCallback(ChangeBroadcaster* source);
	void addTable(Colour tableColour, int tableNumber);
	void updateComboBoxItems();
	void updateScrubberPosition(double position);
    void timerCallback();
	void comboBoxChanged(ComboBox* combo);
	
    AutomationProcessor* getFilter() const
    {
        return static_cast <AutomationProcessor*> (getAudioProcessor());
    }
	
	
private:
	AutomationDisplay automationDisplay;
    AutomationProcessor& processor;
	
    int tableNumber;

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
	TextButton button;
	Array <float, CriticalSection> tableValues;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AutomationEditor)
};


#endif  // PLUGINEDITOR_H_INCLUDED
