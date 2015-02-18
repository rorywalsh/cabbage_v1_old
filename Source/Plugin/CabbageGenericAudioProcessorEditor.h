/*
  Copyright (C) 2012 Rory Walsh

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

  This file is based on the JUCE library feil of the same name.
  JUCE - "Jules' Utility Class Extensions"
  Copyright 2004-9 by Raw Material Software Ltd.
*/


#ifndef __CABBAGE_GENERICAUDIOPROCESSOREDITOR_JUCEHEADER__
#define __CABBAGE_GENERICAUDIOPROCESSOREDITOR_JUCEHEADER__

//#include "FilterGraph.h"
#include "../CabbageLookAndFeel.h"
#include "../Host/PluginWrapperProcessor.h"

class ProcessorParameterPropertyComp   : public PropertyComponent,
                                         private AudioProcessorListener,
                                         private Timer,
										 public ChangeBroadcaster,
										 public ActionListener
{
public:
	String changeMessage;

    ProcessorParameterPropertyComp (const String& name, AudioProcessor& p, int paramIndex, int Id)
        : PropertyComponent (name),
          owner (p),
		  nodeId(Id),
          index (paramIndex),
          paramHasChanged (false),
          slider (p, paramIndex),
		  midiLearnEnabled(false),
		  isBeingAutomated(false),
		  changeMessage(""),
		  lookAndFeelBasic(new CabbageLookAndFeelBasic())
    {
		setLookAndFeel(lookAndFeelBasic);
        startTimer (100);
        addAndMakeVisible (slider);
		setPreferredHeight(20);
		
		if(PluginWrapper* wrapper = dynamic_cast<PluginWrapper*>(&p))
		{
			wrapper->vstInstance->addListener (this);
		}
		
        owner.addListener (this);
		slider.lookAndFeelChanged();
		slider.addActionListener(this);
    }

    ~ProcessorParameterPropertyComp()
    {
		if(PluginWrapper* wrapper = dynamic_cast<PluginWrapper*>(&owner))
		{
			wrapper->vstInstance->removeListener (this);
		}
		else
			owner.removeListener (this);
    }

	void mouseDown(const MouseEvent& event)
	{
		if(event.mods.isPopupMenu())
		{
			PopupMenu m;
			if(!isBeingAutomated)
				m.addItem(1, "Add automation");
			else
				m.addItem(2, "Remove automation");
			
			if(int item = m.show())
			{
				if(item==1)
				{
					isBeingAutomated=true;
					changeMessage="add automation";
					sendChangeMessage();
				}
				else
				{
					isBeingAutomated=false;
					changeMessage="remove automation";
					sendChangeMessage();					
				}
			}			
		}
	}

	int32 getNodeId(){	return nodeId;	}
	int getParamIndex(){	return index;	}

    void refresh() override
    {
        paramHasChanged = false;

        if (slider.getThumbBeingDragged() < 0)
            slider.setValue (owner.getParameter (index), dontSendNotification);

        slider.updateText();
    }

    void paint(Graphics &g)
    {
        g.fillAll(cUtils::getComponentSkin().darker(.4f)); 
		
		if(midiLearnEnabled == true)
		{
			g.setColour(Colours::yellow);
			g.drawRect(getLocalBounds().withLeft(5).withWidth(slider.getPosition().getX()-15.f), 1.f);
		}		
		
        String text = getName();
        g.setColour(Colours::whitesmoke);

        Font font (cUtils::getTitleFont());
        g.setFont (font);

        g.drawFittedText(text, 5, 5,
                         slider.getPosition().getX(), font.getHeight()-2, Justification::centred, 1);

    }
	
    void audioProcessorChanged (AudioProcessor*) override  {}

    void audioProcessorParameterChanged (AudioProcessor*, int parameterIndex, float) override
    {
        if (parameterIndex == index)
		{
            paramHasChanged = true;
			sendChangeMessage();
		}
    }

		
	void actionListenerCallback(const String &message)
	{
		//sendChangeMessage();
	}
	
    void timerCallback() override
    {
        if (paramHasChanged)
        {
            refresh();
            startTimerHz (50);
        }
        else
        {
            startTimer (jmin (1000 / 4, getTimerInterval() + 10));
        }
    }

private:
    //==============================================================================
    class ParamSlider  : public Slider, public ActionBroadcaster
    {
    public:
        ParamSlider (AudioProcessor& p, int paramIndex)  : owner (p), index (paramIndex)
        {
            const int steps = owner.getParameterNumSteps (index);

            if (steps > 1 && steps < 0x7fffffff)
                setRange (0.0, 1.0, 1.0 / (steps - 1.0));
            else
                setRange (0.0, 1.0);

            setSliderStyle (Slider::LinearHorizontal);
            setTextBoxIsEditable (false);
            setScrollWheelEnabled (true);
        }

        void valueChanged() override
        {
            const float newVal = (float) getValue();

            if (owner.getParameter (index) != newVal)
            {
                owner.setParameterNotifyingHost (index, newVal);
                updateText();
            }
        }

		void mouseEnter(const MouseEvent& e)
		{
            sendActionMessage("midiPopup");
		}

        String getTextFromValue (double /*value*/) override
        {
            return owner.getParameterText (index) + " " + owner.getParameterLabel (index).trimEnd();
        }

    private:
        //==============================================================================
        AudioProcessor& owner;
        const int index;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParamSlider)
    };

    AudioProcessor& owner;
    const int index;
	int32 nodeId;

    bool volatile paramHasChanged;
	bool isBeingAutomated;
	bool midiLearnEnabled;
	Colour midiLearnColour;
    ParamSlider slider;
	ScopedPointer<CabbageLookAndFeelBasic> lookAndFeelBasic;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ProcessorParameterPropertyComp)
};


//==============================================================================

class CabbageGenericAudioProcessorEditor      : public AudioProcessorEditor
{
public:
	CabbageGenericAudioProcessorEditor(AudioProcessor* const p, bool midiLearn)
    : AudioProcessorEditor (p), midiLearnMode(midiLearn)
	{
		jassert (p != nullptr);
		setOpaque (true);

		addAndMakeVisible (panel);
		Array <PropertyComponent*> params;

		const int numParams = p->getNumParameters();
		int totalHeight = 0;

		for (int i = 0; i < numParams; ++i)
		{
			String name (p->getParameterName (i));
			if (name.trim().isEmpty())
				name = "Unnamed";

			ProcessorParameterPropertyComp* const pc = new ProcessorParameterPropertyComp (name, *p, i, -1);
			params.add (pc);
			totalHeight += pc->getPreferredHeight();
		}

		panel.addProperties (params);

		setSize (400, jlimit (25, 400, totalHeight));
	}

	~CabbageGenericAudioProcessorEditor()
	{
	}

	void paint (Graphics& g)
	{
		g.fillAll (cUtils::getDarkerBackgroundSkin());
	}

	void resized()
	{
		panel.setBounds (getLocalBounds());
	}

private:
	PropertyPanel panel;
	bool midiLearnMode;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageGenericAudioProcessorEditor)
};



#endif

