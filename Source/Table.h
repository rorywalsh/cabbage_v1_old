/*
  Copyright (C) 20139 Rory Walsh

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

#ifndef GENTABLE_H
#define GENTABLE_H

#include "../JuceLibraryCode/JuceHeader.h" 
#include "CabbageUtils.h"
#include "CabbageLookAndFeel.h"

class ZoomButton;
class GenThumbnail;
//=================================================================
// display a sound file as a waveform..
//=================================================================
class GenTable : public Component,
						public ChangeBroadcaster,
						private ScrollBar::Listener,
						public ChangeListener
{
public:
	GenTable();	
	~GenTable();	
	
	double getCurrentPlayPos(){
		return currentPlayPosition;
	}

	int getCurrentPlayPosInSamples(){
		return currentPlayPosition*sampleRate;
	}
	
	int getLoopLengthInSamples(){
		return loopLength*sampleRate;
	}	
	
	void setScrubberPos(double pos);
	
    float timeToX (const double time) const
    {
        return getWidth() * (float) ((time - visibleRange.getStart()) / (visibleRange.getLength()));
    }

    double xToTime (const float x) const
    {
        return (x / getWidth()) * (visibleRange.getLength()) + visibleRange.getStart();
    }

	
	void setZoomFactor (double amount);
	void setFile (const File& file);
	void mouseWheelMove (const MouseEvent&, const MouseWheelDetails& wheel);
	void setWaveform(AudioSampleBuffer buffer, int channels);
	void createImage(String filename);
	void addTable(int sr, String colour, StringArray fstatement);
	
private:
	Image img;
	//Graphics& graphics;
	int imgCount;
	Range<double> visibleRange;
	double zoom;
	ScopedPointer<DrawableRectangle> currentPositionMarker;
	ScopedPointer<ScrollBar> scrollbar;
	void setRange(Range<double> newRange);
	void resized();	    
    void paint (Graphics& g);
    void mouseDown (const MouseEvent& e);
	void mouseUp(const MouseEvent& e);
	void mouseEnter(const MouseEvent& e);
    void mouseDrag(const MouseEvent& e);
	void mouseExit(const MouseEvent& e);
	bool reDraw;
	double scrubberPosition;
	void scrollBarMoved (ScrollBar* scrollBarThatHasMoved, double newRangeStart);
	void changeListenerCallback(ChangeBroadcaster *source);
	ScopedPointer<ZoomButton> zoomIn, zoomOut;

	AudioFormatManager formatManager;
	double sampleRate;
	float regionWidth;
	Image waveformImage;
    AudioThumbnailCache thumbnailCache;
    ScopedPointer<AudioThumbnail> thumbnail;
	Colour colour, fontcolour;
	int mouseDownX, mouseUpX;
	Rectangle<int> localBounds;
	double loopLength;
	double loopStart;
	double currentPlayPosition;
	bool drawWaveform;
	int gen;
};

//==============================================================================
// GenThumbnail class
//==============================================================================
class GenThumbnail : public Component
{
	
public:
	GenThumbnail(int gen, Colour colour);
	~GenThumbnail();
	void paint(Graphics &g);
};
					
//==============================================================================
// zooming button
//==============================================================================
class ZoomButton : public Component,
					public ChangeBroadcaster
					
{
public:
	ZoomButton(String type):Component()
	{
	setName(type);	
	}
	~ZoomButton(){}
	
	void mouseDown(const MouseEvent& e){
		sendChangeMessage();
	}
	
	void paint(Graphics& g){
		g.fillAll(Colours::transparentBlack);
		g.setColour(Colours::white.withAlpha(.8f));
		g.fillEllipse(0, 0, getWidth(), getHeight());
		g.setColour(Colours::black);
		g.fillRoundedRectangle(getWidth()*.18, getHeight()*.4f, getWidth()*.65, getHeight()*.25, 2);
		if(getName()=="zoomIn")
			g.fillRoundedRectangle(getWidth()*.38f, getHeight()*.20, getWidth()*.25, getHeight()*.65, 2);
	}
	
};
#endif // SOUNDFILEWAVEFORM_H
