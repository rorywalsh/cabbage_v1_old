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


#ifndef __PLUGIN_GENERICAUDIOPROCESSOREDITOR_JUCEHEADER__
#define __PLUGIN_GENERICAUDIOPROCESSOREDITOR_JUCEHEADER__

//#include "FilterGraph.h"
#include "../CabbageLookAndFeel.h"
#include "../Host/PluginWrapperProcessor.h"

class PluginProcessorParameterPropertyComp   : public PropertyComponent,
                                         private AudioProcessorListener,
                                         private Timer,
										 public ActionBroadcaster
{
public:
    PluginProcessorParameterPropertyComp (const String& name, PluginWrapper& p, int paramIndex)
        : PropertyComponent (name),
          owner (p),
          index (paramIndex),
          paramHasChanged (false),
          slider (p, paramIndex),
		  lookAndFeelBasic(new CabbageLookAndFeelBasic()),
		  active(false)
    {
		setLookAndFeel(lookAndFeelBasic);
        startTimer (100);
        addAndMakeVisible (slider);
		setPreferredHeight(20);
        owner.vstInstance->addListener (this);
		slider.setLookAndFeel(lookAndFeelBasic);
		slider.lookAndFeelChanged();
    }

    ~PluginProcessorParameterPropertyComp()
    {
        owner.vstInstance->removeListener (this);
    }

    void refresh() override
    {
        paramHasChanged = false;

        if (slider.getThumbBeingDragged() < 0)
            slider.setValue (owner.getParameter (index), dontSendNotification);

        slider.updateText();
    }

	void setActive(bool val)
	{
		active = val;
		repaint();
	}

    void paint(Graphics &g)
    {
        g.fillAll(cUtils::getComponentSkin().darker(.4f)); //background
		
		if(active == true)
		{
			g.setColour(Colours::yellow);
			g.drawRect(getLocalBounds().withLeft(5).withWidth(slider.getPosition().getX()-15.f), 1.f);
		}
		
        String text = getName();
        g.setColour(Colours::whitesmoke);

        Font font (cUtils::getTitleFont());
        //font.setFallbackFontName (String("Verdana")); //in case the user doesn't have the first font installed
        g.setFont (font);

        g.drawFittedText(text, 5, 5,
                         slider.getPosition().getX(), font.getHeight()-2, Justification::centred, 1);

        //	g.drawFittedText(text, 5, 5,
        //	font.getStringWidth(text), font.getHeight()-2, Justification::centredTop, 1);
    }
	
    void audioProcessorChanged (AudioProcessor*) override  {}

    void audioProcessorParameterChanged (AudioProcessor*, int parameterIndex, float) override
    {
        if (parameterIndex == index)
            paramHasChanged = true;
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
    class ParamSlider  : public Slider
    {
    public:
        ParamSlider (PluginWrapper& p, int paramIndex)  : owner (p), index (paramIndex)
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

                if(PluginProcessorParameterPropertyComp* propComp = findParentComponentOfClass <PluginProcessorParameterPropertyComp>())
				{
					propComp->sendActionMessage(String(index));
				}
				
				updateText();
            }
        }

        String getTextFromValue (double /*value*/) override
        {
            return owner.getParameterText (index) + " " + owner.getParameterLabel (index).trimEnd();
        }

    private:
        //==============================================================================
        PluginWrapper& owner;
        const int index;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParamSlider)
    };

    PluginWrapper& owner;
    const int index;
    bool volatile paramHasChanged;
    ParamSlider slider;
	bool active;
	Colour midiLearnColour;
	ScopedPointer<CabbageLookAndFeelBasic> lookAndFeelBasic;
	
	

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginProcessorParameterPropertyComp)
};


//==============================================================================

class PluginGenericAudioProcessorEditor : 	public AudioProcessorEditor,
											public ActionListener,
											public ActionBroadcaster
{
public:
	PluginGenericAudioProcessorEditor(PluginWrapper* const p, bool midiLearn)
    : AudioProcessorEditor (p), midiLearnMode(midiLearn)
	{
		jassert (p != nullptr);
		setOpaque (true);

		addAndMakeVisible (panel);
		
		const int numParams = p->getNumParameters();
		int totalHeight = 0;

		for (int i = 0; i < numParams; ++i)
		{
			String name (p->getParameterName (i));
			if (name.trim().isEmpty())
				name = "Unnamed";

			PluginProcessorParameterPropertyComp* const pc = new PluginProcessorParameterPropertyComp (name, *p, i);
			
			if(midiLearn)
				pc->addActionListener(this);
				
			params.add (pc);
			totalHeight += pc->getPreferredHeight();
		}

		
		panel.addProperties(params);

		setSize (400, jlimit (25, 400, totalHeight));
	}

	~PluginGenericAudioProcessorEditor()
	{
		sendActionMessage("shutting down generic interface");
	}

	void paint (Graphics& g)
	{
		g.fillAll (cUtils::getDarkerBackgroundSkin());
	}

	void resized()
	{
		panel.setBounds (getLocalBounds().withHeight(getHeight()));
	}

	void actionListenerCallback (const String &message)
	{
		int index = message.getIntValue();
		for(int i=0;i<params.size();i++)
			((PluginProcessorParameterPropertyComp*)params.getReference(i))->setActive(index==i ? true : false);

	}
private:
	PropertyPanel panel;
	Array <PropertyComponent*> params;
	bool midiLearnMode;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginGenericAudioProcessorEditor)
};



#endif

