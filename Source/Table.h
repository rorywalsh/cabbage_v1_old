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

#define FIXED_WIDTH 15
#define MAX_TABLE_SIZE 16384

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
    var tableConfigList;
    int largestTable;
    double scrubberPosition;
    double scrubberFreq;
    bool shouldShowTableButtons;
    bool shouldShowZoomButtons;
    int mainFooterHeight;
	Colour gridColour;
	int tableIndex;

public:
    TableManager();
    ~TableManager() {};
    void paint (Graphics& g)
    {
        g.fillAll(Colours::transparentBlack);
    };
	
	
	void setGridColour(Colour col);
	void setOutlineThickness(float thickness);
	void setBackgroundColour(Colour col);
	void repaintAllTables();
    void resized();
    void setZoomFactor(double zoom);
    void setDrawMode(String mode);
    void bringButtonsToFront();
    void setAmpRanges(Array<float> ampRange);
    void timerCallback();
    void setRange(double start, double end);
    ScopedPointer<DrawableRectangle> currentPositionMarker;
    double getLengthInSamples();
    void setScrubberPos(double pos, int tableNum);
    void scroll(double newRangeStart);
    void addTable(int sr, const String col, int gen, Array<float> ampRange, int ftnumber, ChangeListener* listener);
    void setWaveform(AudioSampleBuffer buffer, int ftNumber);
    void scrollBarMoved (ScrollBar* scrollBarThatHasMoved, double newRangeStart);
    void setWaveform(Array<float, CriticalSection> buffer, int ftNumber, bool updateRange = true);
    void enableEditMode(StringArray pFields, int ftnumber);
    ScopedPointer<RoundButton> zoomIn, zoomOut;
    OwnedArray<RoundButton> tableButtons;
    OwnedArray<GenTable> tables;
    void showZoomButtons(bool show);
    void showTableButtons(bool show);
    void changeListenerCallback(ChangeBroadcaster *source);
    void bringTableToFront(int ftNumber);
    void configTableSizes(var tableConfig);
    GenTable* getLargestTable();
    GenTable* getTableFromFtNumber(int ftnumber);
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
		return getWidth() * (double) ((time - visibleRange.getStart()) / (visibleRange.getLength()));
    }

    double xToTime (const float x) const
    {
        return (x / getWidth()) * (visibleRange.getLength()) + visibleRange.getStart();
    }

    void setSampleRange(double pos, double end);
    void setZoomFactor (double amount);
    void setFile (const File& file);
    void mouseWheelMove (const MouseEvent&, const MouseWheelDetails& wheel);
    void setWaveform(AudioSampleBuffer buffer);
    void enableEditMode(StringArray pFields);
    Point<int> tableTopAndHeight;
    void setWaveform(Array<float, CriticalSection> buffer, bool updateRange = true);
    void createImage(String filename);
    void addTable(int sr, const String col, int gen, Array<float> ampRange);
    static float ampToPixel(int height, Range<float> minMax, float sampleVal);
    static float pixelToAmp(int height, Range<float> minMax, float sampleVal);
    Array<double> getPfields();
    String changeMessage;
	Colour gridColour;
    int tableNumber, tableSize, genRoutine, realGenRoutine;
    void setRange(Range<double> newRange, bool isScrolling = false);
    Range<double> globalRange;
    bool isTableOnTop;
    ScopedPointer<ScrollBar> scrollbar;
    void resized();
    Range<double> visibleRange;
	bool drawGrid;
    int scrollbarReduction;
    void showScrollbar(bool show);
    int mainFooterHeight, paintFooterHeight;
	
    HandleViewer* getHandleViewer()
    {
        return handleViewer;
    }
    double quantiseSpace;
    void setAmpRanges(Array<float> ampRange);
    void setXPosition(double pos);
    bool drawAsVUMeter;
	
	HandleComponent* getCurrentHandle()
	{
		if(currentHandle)
			return currentHandle;
		else
		{
			assert(false);
			return nullptr;			
		}

	}

	String getCoordinates()
	{
		return coordinates;
	}
	
	double getNewRangeStart()
	{
		return newRangeStart;
	}
	
	void shouldDrawGrid(bool val)
	{
		drawGrid = val;
	}
	
	int displayAsGrid()
	{
		return qsteps;
	}
	
	void setTraceThickness(float thickness)
	{
	traceThickness = thickness;	
	}
	
	void setBackgroundColour(Colour col)
	{
		backgroundColour = col;
	}
	
private:
	void drawBackgroundGrid();
	Image backgroundImage;
	float traceThickness;
	bool paintCachedImage;
	String coordinates;
	double newRangeStart;
	HandleComponent* currentHandle;
    bool shouldScroll;
    int normalised;
    int imgCount;
    Colour fillColour;
	Colour backgroundColour;
    float currentWidth;
    double zoom;
    bool showScroll;
    double qsteps;
    double numPixelsPerIndex;
    ColourGradient gradient;
    StringArray pFields;
    ScopedPointer<DrawableRectangle> currentPositionMarker;
    juce::Rectangle<int> thumbArea;
    juce::Rectangle<int> handleViewerRect;
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
    juce::Rectangle<int> localBounds;
    double loopLength;
    double loopStart;
    double currentPlayPosition;
    bool drawWaveform;

	const Image drawGridImage(bool redraw, double width=0.0, double height=0.0, double offset=0.0);

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
class HandleViewer : public Component
{
		
public:
    HandleViewer();
    ~HandleViewer();
    ScopedPointer<TextButton> button1;
    ScopedPointer<TextButton> button2;
    void mouseDown(const MouseEvent& e);
    void mouseDrag(const MouseEvent& e);
    void positionHandle(const MouseEvent& e);
    void repaint(Graphics &g);
    void resized();
    void addHandle(double x, double y, double width, double height, Colour colour);
    void insertHandle(double x, double y, Colour colour);
    double getSnapYPosition(const double y);
	double getSnapXPosition(const double x);
    HandleComponent* getPreviousHandle(HandleComponent* thisHandle);
    HandleComponent* getNextHandle(HandleComponent* thisHandle);
    int getHandleIndex(HandleComponent* thisHandle);
    void removeHandle (HandleComponent* thisHandle);
    OwnedArray<HandleComponent, CriticalSection> handles;
    void fixEdgePoints(int gen);
    void showHandles(bool show);
    int handleIndex;
	bool showingGrid;
    double tableSize;
    Range<float> minMax;
    Colour colour;
    int gen;
	
	bool isShowingGrid()
	{
		return showingGrid;
	}
	
	void setShowingGrid(bool val)
	{
		showingGrid = val;
	}
    bool shouldShowHandles;

    GenTable* getParentTable()
    {
        return findParentComponentOfClass <GenTable>();
    };

};

//==============================================================================
// Handle class
//==============================================================================
class HandleComponent : public Component,
    public ChangeBroadcaster
{
public:
    HandleComponent(double xPos, double yPos, int index, bool fixed, int gen, Colour colour);
    ~HandleComponent();


	void setPosition(double x, double y, bool circularEnv);

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
    void setColour(Colour icolour);
    void setRelativePosition(Point<double> point);

    HandleViewer* getParentHandleViewer()
    {
        return findParentComponentOfClass <HandleViewer>();
    };
	
    GenTable* getParentGenTable()
    {
        return findParentComponentOfClass <GenTable>();
    };	

    TableManager* getParentTableManager()
    {
        return findParentComponentOfClass <TableManager>();
    };

	int getUniqueID()
	{
		return uniqueID;
	}

	void setUniqueID(int id)
	{
		uniqueID = id;
	}

	double getDoubleY()
	{
		return highResY;
	}

	double setDoubleY(double y)
	{
		highResY = y;
	}
	
    HandleComponent* getPreviousHandle();
    HandleComponent* getNextHandle();
    String changeMessage;
    String mouseStatus;
    double xPosRelative, yPosRelative;
    bool status;

private:
    Colour colour;
    bool fixed;
	double highResY;
	int uniqueID;

    ComponentDragger dragger;
    int lastX, lastY;
    int offsetX, offsetY;
    int genRoutine;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HandleComponent);
};

//==============================================================================
// round button
//==============================================================================
class RoundButton : public Component,
    public ChangeBroadcaster

{
    String type;
	String visible;
    Colour colour;
    int mode;
public:
    RoundButton(String _type, Colour _colour):Component(), visible("bacground")
    {
        setName(_type);
        type = _type;
        colour = _colour;
        mode = 0;
    }
    ~RoundButton() {}

    void mouseDown(const MouseEvent& e)
    {
        //Logger::writeToLog("Mouse down on round button:"+String(type));
        sendChangeMessage();
        mode = (mode==1 ? 0 : mode+1);
    }

    void paint(Graphics& g)
    {
        //Logger::writeToLog(type);
        if(type.contains("zoom"))
        {
            g.fillAll(Colours::transparentBlack);
			g.setColour(cUtils::getBackgroundSkin());
			g.fillEllipse(0, 0, getWidth(), getHeight());
            g.setColour(Colours::white.withAlpha(.8f));
            g.fillEllipse(1, 1, getWidth()-2, getHeight()-2);
            g.setColour(Colours::black);
            g.fillRoundedRectangle(getWidth()*.18, getHeight()*.4f, getWidth()*.65, getHeight()*.25, 2);
            if(getName()=="zoomIn")
                g.fillRoundedRectangle(getWidth()*.38f, getHeight()*.20, getWidth()*.25, getHeight()*.65, 2);
        }
        else
        {
            g.fillAll(Colours::transparentBlack);
			g.setColour(cUtils::getBackgroundSkin());
			g.fillEllipse(0, 0, getWidth(), getHeight());
            g.setColour((visible=="foreground" ? colour : colour.withAlpha(.3f)));
            g.fillEllipse(1, 1, getWidth()-2, getHeight()-2);
            g.setColour(colour.contrasting());
            g.drawFittedText(type, 0, 0, getWidth(), getHeight(), Justification::centred, 1);
			if(visible=="off")
			{
				g.setColour(colour.contrasting());
				g.drawLine(0, 0, getWidth(), getHeight(), 2);
				g.drawLine(0, getHeight(), getWidth(), 0, 2);	
			}
        }
    }

	void setVisibilityStatus(String status)
	{
		visible = status;
	}

    int getMode()
    {
        return mode;
    }
	
    int setMode(int mod)
    {
        mode=mod;
    }
};

#endif // SOUNDFILEWAVEFORM_H
