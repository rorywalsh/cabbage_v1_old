/*
  Copyright (c) 2014 - Rory Walsh

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

#ifndef __PLUGINEDITOR_H_266C4137__
#define __PLUGINEDITOR_H_266C4137__

#include "../JuceLibraryCode/JuceHeader.h"
#include "AudioFilePlaybackProcessor.h"
#include "../CabbageLookAndFeel.h"

class AudioFilePlaybackEditor;
class Handle;

class WaveformDisplay : public Component,
						public Timer,
						public ChangeListener,
						private ScrollBar::Listener
{
public:
    WaveformDisplay(AudioFormatManager& formatManager, BufferingAudioSource *source, int sr, Colour col);
    ~WaveformDisplay();
	
	class GainEnvelope : public Component
	{			
	public:
		GainEnvelope();
		
		~GainEnvelope()
		{
			handles.clear();
		}
		
		void resized();		
		void paint (Graphics& g);
		void mouseDown(const MouseEvent& e);
		Handle* getPreviousHandle(Handle* thisHandle);
		int getHandleIndex(Handle* thisHandle);
		void removeHandle(Handle* handle);
		Handle* getLastHandle();
		Handle* getFirstHandle();
		Handle* getNextHandle(Handle* thisHandle);
		void createGainEnvStartEndPoint();
		void addHandle(Point<double> pos, bool resize=true);
		
		AudioFilePlaybackEditor* getEditor()
		{
			return this->findParentComponentOfClass<AudioFilePlaybackEditor>();
		}
		
	private:
		OwnedArray<Handle> handles;
		
	};
	
	GainEnvelope gainEnvelope;
	void showGainEnvelope(bool show);
    void setScrubberPos(double pos);
    void changeListenerCallback (ChangeBroadcaster*);
    void setFile (const File& file);
    void setWaveform(AudioSampleBuffer buffer, int channels);
    void setZoomFactor (double amount);
    void setRange (Range<double> newRange);
    void paint (Graphics& g);
    void timerCallback();
    void mouseDown (const MouseEvent& e);
    void mouseDrag (const MouseEvent& e);
    void resetPlaybackPosition();
    void resized() override;

	BufferingAudioSource* source;

	AudioFilePlaybackEditor* getEditor()
	{
		return this->findParentComponentOfClass<AudioFilePlaybackEditor>();
	}	
	
private:
	Range<double> visibleRange;
    //Slider& zoomSlider;
	ScrollBar scrollbar;
    AudioThumbnailCache thumbnailCache;
	
    AudioThumbnail thumbnail;
    double startTime, endTime;
	Rectangle<int> localBounds;
	float sampleRate;
	double currentPlayPosition;
	Colour tableColour;
    DrawableRectangle currentPositionMarker;
    float timeToX (const double time) const;
    double xToTime (const float x) const;
    void scrollBarMoved (ScrollBar* scrollBarThatHasMoved, double newRangeStart) override;
};

//handle class
class Handle : public Component
{
public:
	Handle(WaveformDisplay::GainEnvelope* env):
	owner(env),
	compY(0),
	compX(0)
	{
	setSize(8, 8);	
	}
	
	void paint (Graphics& g)
	{
		g.fillAll(Colours::transparentBlack);
		g.setColour(Colours::white);
		g.fillEllipse(0, 0, 8, 8);
	}

	void setRelativePosition(Point<double> pos, double width, double height)
	{
		//convert position so that it's scaled between 0 and 1
		xPosRelative = jlimit(0.0, 1.0, pos.getX()/width);
		yPosRelative = jlimit(0.0, 1.0, pos.getY()/height);
	} 
	
	Point<double> getRelativePosition()
	{
		return Point<double>(xPosRelative, yPosRelative);
	} 

	void mouseDown(const MouseEvent &e)
	{
		if(e.mods.isPopupMenu() && (this!=owner->getFirstHandle() || this!=owner->getLastHandle()))
		{
			PopupMenu pop;
			pop.addItem(99, "Delete");
			
			if(pop.show()==99)
			{
				owner->removeHandle(this);
			}
		}
		
		compX = getX();
		compY = getY();
		setMouseCursor (MouseCursor::DraggingHandCursor);
		dragger.startDraggingComponent (this, e);				
		constrainer.setMinimumWidth(200);
	}

	void mouseEnter(const MouseEvent &e)
	{
		constrainer.setMinimumOnscreenAmounts(owner->getHeight(), 
											  owner->getWidth(), 
											  owner->getHeight(), 
											  1);
		setMouseCursor (MouseCursor::DraggingHandCursor);		
	}

	void mouseDrag(const MouseEvent &e)
	{
		
		//when a handle is dragged, we update its position and send a message
		//to Cabbage to update the Csound function table(CabbagePluginEditor.cpp)
		const Handle* previousHandle = owner->getPreviousHandle(this);
		const Handle* nextHandle = owner->getNextHandle(this);

		setMouseCursor (MouseCursor::DraggingHandCursor);

		double xPos = compX+e.getDistanceFromDragStartX();
		double yPos = jlimit(-4.0, owner->getHeight()-4.0, compY+e.getDistanceFromDragStartY());
		
		
		if(this==owner->getFirstHandle() || this==owner->getLastHandle())
		{			
			setRelativePosition(Point<double>((this==owner->getLastHandle() ? owner->getWidth() : 0), yPos), owner->getWidth(), owner->getHeight());
			dragger.dragComponent (this, e.withNewPosition(Point<float>(compX, e.getPosition().getY())), &constrainer);
		}
		else
		{
			if((xPos>(previousHandle->getX()-4)) && (xPos<nextHandle->getX()))
			{
				cUtils::debug("Owner height", owner->getHeight());
				cUtils::debug("yPos", yPos);
				setRelativePosition(Point<double>(xPos, yPos), owner->getWidth(), owner->getHeight());
				dragger.dragComponent (this, e, &constrainer);
			}
		}
		
		
		owner->repaint();
	}
	
	void mouseUp(const MouseEvent &e)
	{
		owner->resized();
		owner->repaint();
	}
	
	double xPosRelative, yPosRelative, compX, compY;
	ComponentDragger dragger;
	int uid;
	ComponentBoundsConstrainer constrainer;
	WaveformDisplay::GainEnvelope* owner;

};
//==============================================================================
/**
*/
class AudioFilePlaybackEditor  : public AudioProcessorEditor,
								 public Button::Listener,
								 public DragAndDropTarget,
								 public Slider::Listener
{
public:
    AudioFilePlaybackEditor (AudioFilePlaybackProcessor* ownerFilter);
    ~AudioFilePlaybackEditor();
	const DrawablePath* createZoomInButtonPath(int buttonSize);

    //CabbagePluginAudioProcessor* filter;
    AudioFilePlaybackProcessor* getFilter() const
	{
			return static_cast <AudioFilePlaybackProcessor*> (getAudioProcessor());
	}

    //==============================================================================
    // This is just a standard Juce paint method...
    void paint (Graphics& g);
	void resized();
	void buttonClicked(Button *button);
	ScopedPointer<WaveformDisplay> waveformDisplay;
	void setCurrentPosition(String pos)
	{
		currentPositionLabel.setText(pos, dontSendNotification);
	}

    bool isInterestedInDragSource (const SourceDetails &dragSourceDetails) override
    {
        return true;
    }

	void sliderValueChanged (Slider* sliderThatWasMoved);
	void itemDropped (const DragAndDropTarget::SourceDetails& dragSourceDetails);


private:
	DrawableButton playButton;
	Label fileNameLabel;
	Label beatOffsetLabel;
	Label currentPositionLabel;
	DrawableButton stopButton;
	DrawableButton openButton;
	DrawableButton zoomInButton;
	DrawableButton gainEnvelopeButton;
	DrawableButton loopButton;	
	DrawableButton zoomOutButton;
	
	CabbageLookAndFeelBasic basicLook;
	Slider beatOffset;
	DrawableButton linkToTransport; 
    Colour tableColour;
    double zoom;
};


#endif  // __PLUGINEDITOR_H_266C4137__
