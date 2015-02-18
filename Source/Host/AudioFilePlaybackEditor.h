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

class WaveformDisplay : public Component,
						public Timer,
						public ChangeListener,
						private ScrollBar::Listener
{
public:
	WaveformDisplay(AudioFormatManager& formatManager, BufferingAudioSource *source, int sr):
	thumbnailCache (5), 
	thumbnail (512, formatManager, thumbnailCache), 
	source(source), 
	sampleRate(sr),
	scrollbar (false),
	currentPlayPosition(0)
	{	
		tableColour = Colour(Random::getSystemRandom().nextInt(255),
							 Random::getSystemRandom().nextInt(255),
							 Random::getSystemRandom().nextInt(255));
							 
		
		thumbnail.addChangeListener(this);
		currentPositionMarker.setFill (Colours::lime);
		addAndMakeVisible(currentPositionMarker);		
        addAndMakeVisible (scrollbar);
        scrollbar.setRangeLimits (visibleRange);
        scrollbar.setAutoHide (false);
        scrollbar.addListener (this);		
	}
	
	~WaveformDisplay()
	{	
		stopTimer();
	}
	

	void setScrubberPos(double pos)
	{
        currentPositionMarker.setVisible (true);
        //pos = (pos/(thumbnail.getTotalLength()*sampleRate))*thumbnail.getTotalLength();
        currentPositionMarker.setRectangle (Rectangle<float> (timeToX (pos) - 0.75f, 0,
                                             1.5f, (float) (getHeight() - scrollbar.getHeight())));
	}	
	
	void changeListenerCallback (ChangeBroadcaster*)
	{
		repaint();
	}
	
    void setFile (const File& file)
    {
        AudioFormatManager format;
        format.registerBasicFormats();
        AudioFormatReader* reader = format.createReaderFor(file);

        if(reader)
        {
            AudioSampleBuffer buffer(reader->numChannels, reader->lengthInSamples);
            buffer.clear();
            buffer.setSize(reader->numChannels, reader->lengthInSamples);
            reader->read(&buffer,0, buffer.getNumSamples(), 0, true, true);
			setWaveform(buffer, reader->numChannels);
        }

        delete reader;
    }

	void setWaveform(AudioSampleBuffer buffer, int channels)
	{
		thumbnail.clear();
		thumbnail.reset(channels, 44100, buffer.getNumSamples());
		thumbnail.addBlock(0, buffer, 0, buffer.getNumSamples());
		const Range<double> newRange (0.0, thumbnail.getTotalLength());
		scrollbar.setRangeLimits (newRange);
		setRange (newRange);
	}

    void setZoomFactor (double amount)
    {
        if (thumbnail.getTotalLength() > 0)
        {
            const double newScale = jmax (0.001, thumbnail.getTotalLength() * (1.0 - jlimit (0.0, 0.99, amount)));
            const double timeAtCentre = xToTime (getWidth() / 2.0f);
            setRange (Range<double> (timeAtCentre - newScale * 0.5, timeAtCentre + newScale * 0.5));
        }
    }

    void setRange (Range<double> newRange)
    {
        visibleRange = newRange;
        scrollbar.setCurrentRange (visibleRange);
        repaint();
    }
	
    void paint (Graphics& g)
    {
        g.fillAll (Colour(20, 20, 20));
        g.setColour (tableColour);

        if (thumbnail.getTotalLength() > 0)
        {
            thumbnail.drawChannels (g, getLocalBounds().reduced(.6f),
                                    visibleRange.getStart(), visibleRange.getEnd(), 2.0f);
        }
        else
        {
            g.setFont (14.0f);
            g.drawFittedText ("(No audio file selected)", getLocalBounds(), Justification::centred, 2);
        }

    }

    void timerCallback()
    {
		if(thumbnail.getTotalLength()>0)
		{
			currentPlayPosition = source->getNextReadPosition()/sampleRate;
			setScrubberPos(currentPlayPosition);			
		}
    }

    void mouseDown (const MouseEvent& e)
    {
		if(thumbnail.getTotalLength()>0)
		{
			source->setNextReadPosition (jmax (0.0, xToTime ((float) e.x)*sampleRate));
			currentPlayPosition = jmax (0.0, xToTime ((float) e.x));
			setScrubberPos(currentPlayPosition);		
		}
    }

    void mouseDrag (const MouseEvent& e)
    {
		if(thumbnail.getTotalLength()>0)
		{
			source->setNextReadPosition (jmax (0.0, xToTime ((float) e.x)*sampleRate));
			currentPlayPosition = jmax (0.0, xToTime ((float) e.x));
			setScrubberPos(currentPlayPosition);
		}
    }
	
	void resetPlaybackPosition()
	{
		currentPlayPosition=0;
	}

	BufferingAudioSource* source;
	
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

    float timeToX (const double time) const
    {
        return getWidth() * (float) ((time - visibleRange.getStart()) / (visibleRange.getLength()));
    }

    double xToTime (const float x) const
    {
        return (x / getWidth()) * (visibleRange.getLength()) + visibleRange.getStart();
    }
	
    void scrollBarMoved (ScrollBar* scrollBarThatHasMoved, double newRangeStart) override
    {
        if (scrollBarThatHasMoved == &scrollbar)
            setRange (visibleRange.movedToStartAt (newRangeStart));
    }	
};
//==============================================================================
/**
*/
class AudioFilePlaybackEditor  : public AudioProcessorEditor,
								 public Button::Listener,
								 public DragAndDropTarget
{
public:
    AudioFilePlaybackEditor (AudioFilePlaybackProcessor* ownerFilter);
    ~AudioFilePlaybackEditor();

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

    bool isInterestedInDragSource (const SourceDetails &dragSourceDetails) override
    {
        return true;
    }

	
	void itemDropped (const DragAndDropTarget::SourceDetails& dragSourceDetails);

	DrawableButton playButton;
	DrawableButton stopButton;
	DrawableButton openButton;
	ScopedPointer<Viewport> viewport;
	CabbageLookAndFeelBasic basicLook;
};


#endif  // __PLUGINEDITOR_H_266C4137__
