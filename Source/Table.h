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

class RoundButton;
class HandleViewer;
class HandleComponent;
class GenTable;

class TableManager : public Component,
					 private ScrollBar::Listener,
					 public ChangeListener
{
	double zoom;
	int currentTableIndex;
public:	
	TableManager();
	~TableManager(){};
	void paint (Graphics& g){
		g.fillAll(Colours::transparentBlack);
	};
	void resized();
	void bringButtonsToFront();
	void addTable(int sr, const String col, int gen, Array<float> ampRange, int ftnumber, ChangeListener* listener);
    void setWaveform(AudioSampleBuffer buffer, int ftNumber);
	void scrollBarMoved (ScrollBar* scrollBarThatHasMoved, double newRangeStart);
    void setWaveform(Array<float, CriticalSection> buffer, int ftNumber, bool updateRange = true);
	void enableEditMode(StringArray pFields, int ftnumber);
    ScopedPointer<RoundButton> zoomIn, zoomOut;
	OwnedArray<RoundButton> tableButtons;
	OwnedArray<GenTable> tables;
	void changeListenerCallback(ChangeBroadcaster *source);
	void bringTableToFront(int ftNumber);
};

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

    double getCurrentPlayPos()
    {
        return currentPlayPosition;
    }

    int getCurrentPlayPosInSamples()
    {
        return currentPlayPosition*sampleRate;
    }

    int getLoopLengthInSamples()
    {
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
    void setWaveform(AudioSampleBuffer buffer);
    void enableEditMode(StringArray pFields);
    void setWaveform(Array<float, CriticalSection> buffer, bool updateRange = true);
    void createImage(String filename);
    void addTable(int sr, const String col, int gen, Array<float> ampRange);
    static float ampToPixel(int height, Range<float> minMax, float sampleVal);
    static float pixelToAmp(int height, Range<float> minMax, float sampleVal);
    Array<float> getPfields();
    String changeMessage;
    int tableNumber, tableSize, genRoutine;
	void setRange(Range<double> newRange, bool isScrolling = false);
	Range<double> globalRange;
	bool isTableOnTop;
	ScopedPointer<ScrollBar> scrollbar;
	void resized();
	Range<double> visibleRange;
	int scrollbarReduction;
	
	HandleViewer* getHandleViewer(){ return handleViewer;}
	
private:
    Image img;
    int normalised;
    int imgCount;
	Colour fillColour;
    float currentWidth;
    double zoom;
    ScopedPointer<DrawableRectangle> currentPositionMarker;
    
    
    
    Rectangle<int> handleViewerRect;
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
    ScopedPointer<HandleViewer> handleViewer;

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

    Array<float, CriticalSection> waveformBuffer;
    double visibleLength, visibleStart, visibleEnd, maxAmp;
    Range<float> minMax;

    Range<float> findMinMax(Array<float, CriticalSection> buffer)
    {
        float min=buffer[0],max=buffer[0];
        for(int i=0; i<buffer.size(); i++)
        {
            if(buffer[i]>max)
                max=buffer[i];
            if(buffer[i]<min)
                min=buffer[i];
        }
        return Range<float>(min, max);
    }

};

//==============================================================================
// HandleViewer class, holds breakpoint handles
//==============================================================================
class HandleViewer : public Component,
    public ActionListener
{
    void actionListenerCallback(const String &message);
    ScopedPointer<Label> label;
public:
    HandleViewer();
    ~HandleViewer();
    ScopedPointer<TextButton> button1;
    ScopedPointer<TextButton> button2;
    void mouseDown(const MouseEvent& e);
    void repaint(Graphics &g);
    void resized();
    void addHandle(double x, double y);
	void insertHandle(double x, double y);
    HandleComponent* getPreviousHandle(HandleComponent* thisHandle);
    HandleComponent* getNextHandle(HandleComponent* thisHandle);
    int getHandleIndex(HandleComponent* thisHandle);
    void removeHandle (HandleComponent* thisHandle);
    OwnedArray<HandleComponent, CriticalSection> handles;
    void fixEdgePoints();
	void showHandles(bool show);
    int handleIndex;
    float tableSize;
	Range<float> minMax;
	Colour colour;
	int gen;

};

//==============================================================================
// Handle class
//==============================================================================
class HandleComponent : public Component,
    public ChangeBroadcaster,
    public ActionBroadcaster
{
public:
    HandleComponent(double xPos, double yPos, int index, bool fixed);
    ~HandleComponent();

    HandleViewer* getParentComponent();
    void paint (Graphics& g);
    void removeThisHandle();
    void mouseEnter (const MouseEvent& e);
    void mouseDown (const MouseEvent& e);
    void mouseDrag (const MouseEvent& e);
    void mouseUp (const MouseEvent& e);
	void mouseExit (const MouseEvent& e);
    int index;
    int height, width;
    int x,y;

    HandleComponent* getPreviousHandle();
    HandleComponent* getNextHandle();
    double xPosRelative, yPosRelative;
    String changeMessage;
    String mouseStatus;

private:
    Colour colour;
    bool fixed;
    ComponentDragger dragger;
    int lastX, lastY;
    int offsetX, offsetY;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HandleComponent);
};

//==============================================================================
// round button
//==============================================================================
class RoundButton : public Component,
    public ChangeBroadcaster

{
String type;
Colour colour;
public:
    RoundButton(String _type, Colour _colour):Component()
    {
        setName(_type);
		type = _type;
		colour = _colour;
    }
    ~RoundButton() {}

    void mouseDown(const MouseEvent& e)
    {
		Logger::writeToLog("Mouse down on round button:"+String(type));
        sendChangeMessage();
    }

    void paint(Graphics& g)
    {
		Logger::writeToLog(type);
        if(type.contains("zoom"))
		{
			g.fillAll(Colours::transparentBlack);
			g.setColour(Colours::white.withAlpha(.8f));
			g.fillEllipse(0, 0, getWidth(), getHeight());
			g.setColour(Colours::black);
			g.fillRoundedRectangle(getWidth()*.18, getHeight()*.4f, getWidth()*.65, getHeight()*.25, 2);
			if(getName()=="zoomIn")
				g.fillRoundedRectangle(getWidth()*.38f, getHeight()*.20, getWidth()*.25, getHeight()*.65, 2);
		}
		else{
			g.fillAll(Colours::transparentBlack);
			g.setColour(colour);
			g.fillEllipse(0, 0, getWidth(), getHeight());
			g.setColour(colour.contrasting());
			g.drawFittedText(type, 0, 0, getWidth(), getHeight(), Justification::centred, 1); 
		}
    }

};

#endif // SOUNDFILEWAVEFORM_H
