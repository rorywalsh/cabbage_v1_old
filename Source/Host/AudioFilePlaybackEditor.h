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
    WaveformDisplay(AudioFormatManager& formatManager, BufferingAudioSource *source, int sr, Colour col);
    ~WaveformDisplay();
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

    bool isInterestedInDragSource (const SourceDetails &dragSourceDetails) override
    {
        return true;
    }

	
	void itemDropped (const DragAndDropTarget::SourceDetails& dragSourceDetails);

	DrawableButton playButton;
	DrawableButton stopButton;
	DrawableButton openButton;
	DrawableButton zoomInButton;
	DrawableButton zoomOutButton;
	CabbageLookAndFeelBasic basicLook;
    Colour tableColour;
    double zoom;
};


#endif  // __PLUGINEDITOR_H_266C4137__
