/*
  Copyright (C) 2007 Rory Walsh

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
#include "MixerStrip.h"
#include "../CabbageUtils.h"
#include "GraphEditorPanel.h"

InternalMixerStrip::InternalMixerStrip(String name, int numChannels): 
mixerName(name), 
numberOfChannels(numChannels),
currentGainLevel(1.f),
range(0, 1, 0.01, .5)
{
	for(int i=0;i<numberOfChannels;i++)
		channelRMS.add(0.f);
	
	addAndMakeVisible(currentGainMarker = new DrawableRectangle());
	currentGainMarker->setFill(Colours::white);
	currentGainMarker->setVisible (true);
}
	
InternalMixerStrip::~InternalMixerStrip()
{
}
	
void InternalMixerStrip::changeListenerCallback (ChangeBroadcaster* source)
{
	if(mixerName=="Inputs")
	{
		const Array<float> inputChannel = ((GraphAudioProcessorPlayer*)source)->getInputChannelRMS();
		for(int i=0;i<inputChannel.size();i++)
			channelRMS.getReference(i) =(float) exp (log ( inputChannel[i]*2.f)/3.0);
	}
	else if(mixerName=="Outputs")
	{
		const Array<float> outputChannel = ((GraphAudioProcessorPlayer*)source)->getOutputChannelRMS();
		for(int i=0;i<outputChannel.size();i++)
			channelRMS.getReference(i) =(float) exp (log (outputChannel[i]*2.f)/3.0);		
	}
	repaint();
	//channelRMS.getReference(i) = rmsValues[i].getFloatValue();
}

void InternalMixerStrip::resized()
{
	currentGainMarker->setRectangle(Rectangle<float> (getWidth()-10, 3, 5, getHeight()-3));
}

void InternalMixerStrip::mouseDown(const MouseEvent &e)
{
	int xPos = jlimit(45.0, getWidth()-10.0, e.getPosition().getX()-2.5);
	xPos = cUtils::roundToMultiple(xPos, (getWidth()-45.0)/17.0);
	currentGainLevel = (xPos-45.f)/(getWidth()-55.0);
	currentGainMarker->setRectangle(Rectangle<float> (xPos, 3, 5, getHeight()-3));	
	//send message so that we can adjust the level appropriately
	sendChangeMessage();
}

void InternalMixerStrip::mouseDrag(const MouseEvent &e)
{
	int xPos = jlimit(45.0, getWidth()-10.0, e.getPosition().getX()-2.5);
	xPos = cUtils::roundToMultiple(xPos, (getWidth()-45.0)/17.0);
	currentGainLevel = (xPos-45.f)/(getWidth()-55.0);
	currentGainMarker->setRectangle(Rectangle<float> (xPos, 3, 5, getHeight()-3));
	//send message so that we can adjust the level appropriately
	sendChangeMessage();
}

void InternalMixerStrip::paint(Graphics& g)
{
	g.fillAll(cUtils::getDarkerBackgroundSkin());
	g.setColour(Colours::whitesmoke);
	g.drawFittedText(mixerName,
					  0, 0, 40, getHeight(),
					  Justification::centred, 2);
	
	for(int i=0;i<numberOfChannels;i++)
	{
		drawLevelMeter (g, 45, 2+i*(getHeight()/numberOfChannels), getWidth()-55, getHeight()/numberOfChannels, channelRMS[i]);		
	}
	
}

void InternalMixerStrip::drawLevelMeter (Graphics& g, float x, float y, int width, int height, float level)
{
	//g.fillAll(cUtils::getDarkerBackgroundSkin());
    const int totalBlocks = 16;
    const float numBlocks = (totalBlocks * level);
    const float w = (width) / (float) totalBlocks;

    for (float i = 0; i < totalBlocks; ++i)
    {
        if (i >= numBlocks)
            g.setColour (Colours::lightblue.withAlpha (0.1f));
        else
            g.setColour (i < totalBlocks - 1 ? 
						Colours::cornflowerblue.interpolatedWith(Colours::lime, (i/numBlocks)) : 
						Colours::red);

        g.fillRoundedRectangle (x + i * w + w * 0.1f, 
								y+1, 
								w-1.f, 
								height-2.f,
								2.f);
    }
}

