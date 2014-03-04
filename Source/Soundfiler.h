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

#ifndef SOUNDFILEWAVEFORM_H
#define SOUNDFILEWAVEFORM_H

#include "../JuceLibraryCode/JuceHeader.h" 
#include "CabbageUtils.h"
#include "CabbageLookAndFeel.h"


//=================================================================
// display a sound file as a waveform..
//=================================================================
class WaveformDisplay : public Component,
						public ChangeBroadcaster,
						private ScrollBar::Listener,
						public ChangeListener
{
public:
	WaveformDisplay(int sr, Colour col);	
	~WaveformDisplay();	
	
	double getCurrentPlayPos(){
		return currentPlayPosition;
	}

	int getCurrentPlayPosInSamples(){
		return currentPlayPosition*sampleRate;
	}
	
	int getLoopEndInSamples(){
		return loopEnd*sampleRate;
	}	
	
	void setScrubberPos(double pos){
		scrubberPosition = pos/sampleRate;
		repaint();
	}
	
    float timeToX (const double time) const
    {
        return getWidth() * (float) ((time - startTime) / (endTime - startTime));
    }

    double xToTime (const float x) const
    {
        return (x / getWidth()) * (endTime - startTime) + startTime;
    }	
	
	void setZoomFactor (double amount);
	void setFile (const File& file, bool firstTime);
	
private:
	Range<double> visibleRange;
	ScopedPointer<ScrollBar> scrollbar;
	void setRange(Range<double> newRange);
	void resized();	    
    void paint (Graphics& g);
    void mouseDown (const MouseEvent& e);
	void mouseUp(const MouseEvent& e);
    void mouseDrag(const MouseEvent& e);
	bool reDraw;
	double scrubberPosition;
	void scrollBarMoved (ScrollBar* scrollBarThatHasMoved, double newRangeStart);
	void changeListenerCallback(ChangeBroadcaster *source);

	AudioFormatManager formatManager;
	float sampleRate;
	float regionWidth;
	Image waveformImage;
    AudioThumbnailCache thumbnailCache;
    ScopedPointer<AudioThumbnail> thumbnail;
	Colour colour;
	int mouseDownX, mouseUpX;
    double startTime, endTime;
	Rectangle<int> localBounds;
	double loopEnd;
	double currentPlayPosition;
	bool drawWaveform;

    DrawableRectangle currentPositionMarker;
	
};


//=================================================================
// holds a wavefor display components. needed because of viewport..
//=================================================================
class Soundfiler :  public Component,
					public ChangeBroadcaster,
					public ChangeListener
{
	ScopedPointer<WaveformDisplay> waveformDisplay;
	//ScopedPointer<Viewport> viewport;
	int position;
	int endPosition;
	
public:
	Soundfiler(String fileName, int sr, Colour colour, Colour fontcolour, int zoom);
	~Soundfiler(){
	};
	
    // This is just a standard Juce paint method...
    void paint (Graphics& g);
	void changeListenerCallback(ChangeBroadcaster *source);
	void resized();
	void buttonClicked(Button *button);
	void setScrubberPosition(int pos);
	
	int getPosition(){
		return position;
	}
	
	int getEndPosition(){
		return endPosition;
	}
	
	void setFile(String filename){
		waveformDisplay->setFile(File(filename), false);
	}
	
	void setZoom(float zoom){
	waveformDisplay->setZoomFactor(zoom);
	}
	
    float timeToX (const double time) const
    {
        return waveformDisplay->timeToX(time);
    }
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Soundfiler);
	
};

#endif // SOUNDFILEWAVEFORM_H
