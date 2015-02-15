/*
  Copyright (c) 2013 - Raw Material Software Ltd, Rory Walsh

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

#ifndef __FILTER_COMP_HEADER__
#define __FILTER_COMP_HEADER__

#include "FilterGraph.h"
#include "../Editor/CodeWindow.h"
#include "../CabbageLookAndFeel.h"
#include "PluginWrapperProcessor.h"

class GraphEditorPanel;


#define INTERNAL 1
#define CABBAGE 2
#define THIRDPARTY 3

//======================================================================
// Filter Component, GUI component that represents a processing node
//======================================================================
class FilterComponent    : public Component, 
						   public ActionListener,
						   public Timer
{
public:
    FilterComponent (FilterGraph& graph_, const uint32 filterID_);
	~FilterComponent();

    FilterGraph& graph;
    const uint32 filterID;
    int numInputs, numOutputs;
	void mouseDown (const MouseEvent& e);
	void mouseDrag (const MouseEvent& e);
	void mouseUp (const MouseEvent& e);
	bool hitTest (int x, int y);
	void actionListenerCallback (const String &message);
	void paint (Graphics& g);
	void getPinPos (const int index, const bool isInput, float& x, float& y);
	void update();
	void drawLevelMeter (Graphics& g, float x, float y, int width, int height, float level);
	void drawMuteIcon(Graphics& g, Rectangle<float> rect, bool state);
	void drawBypassIcon(Graphics& g, Rectangle<float> rect, bool isActive);
	void timerCallback();
	void enableEditMode(bool enable);

private:
	ScopedPointer<CodeWindow> codeWindow;
	int pluginType;
    int pinSize;
    Colour filterColour;
    bool filterIsPartofSelectedGroup;
    Point<int> originalPos;
	float rmsLeft, rmsRight;
	void resized();
    Font font;
    int numIns, numOuts;
    DropShadowEffect shadow;
	bool isMuted, isBypassed;
	Rectangle<float> muteButton;
	Rectangle<float> bypassButton;

    GraphEditorPanel* getGraphPanel() const noexcept
    {
        return findParentComponentOfClass<GraphEditorPanel>();
    }


    FilterComponent (const FilterComponent&);
    FilterComponent& operator= (const FilterComponent&);
};

//======================================================================
// GUI component that represents a pin connection
//======================================================================
class PinComponent   : public Component,
    public SettableTooltipClient
{
public:
    PinComponent (FilterGraph& graph_,
                  const uint32 filterID_, const int index_, const bool isInput_);

    void paint (Graphics& g);
    void mouseDown (const MouseEvent& e);

    void mouseDrag (const MouseEvent& e);
	void mouseUp (const MouseEvent& e);

    const uint32 filterID;
    const int index;
    const bool isInput;

private:
    FilterGraph& graph;

    GraphEditorPanel* getGraphPanel() const noexcept
    {
        return findParentComponentOfClass<GraphEditorPanel>();
    }
	
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PinComponent)
};

//======================================================================
// patch lead component
//======================================================================
class ConnectorComponent   : public Component,
    public SettableTooltipClient
{
public:
    ConnectorComponent (FilterGraph& graph_);
    void setInput (const uint32 sourceFilterID_, const int sourceFilterChannel_);
    void setOutput (const uint32 destFilterID_, const int destFilterChannel_);
    void dragStart (int x, int y);
    void dragEnd (int x, int y);
    void update();
    void resizeToFit();
    void getPoints (float& x1, float& y1, float& x2, float& y2) const;
    void paint (Graphics& g);
    bool hitTest (int x, int y);
    void mouseDown (const MouseEvent&);
    void mouseDrag (const MouseEvent& e);
    void mouseUp (const MouseEvent& e);
    void resized();
    uint32 sourceFilterID, destFilterID;
    int sourceFilterChannel, destFilterChannel;

private:
    FilterGraph& graph;
    float lastInputX, lastInputY, lastOutputX, lastOutputY;
    Path linePath, hitPath;
    bool dragging;

    GraphEditorPanel* getGraphPanel() const noexcept
    {
        return findParentComponentOfClass<GraphEditorPanel>();
    }
	
    void getDistancesFromEnds (int x, int y, double& distanceFromStart, double& distanceFromEnd) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ConnectorComponent)
};
#endif