/*
  ==============================================================================

    Split component for Juce

    File: SplitComponent.h
    Created: 15 Mar 2014

	--------------------------------------------------------------------------------
	Copyright 2014 Jim Hewes

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.

  ==============================================================================
*/

#ifndef SPLITCOMPONENT_H_INCLUDED
#define SPLITCOMPONENT_H_INCLUDED

#include "JuceHeader.h"



/**
	SplitComponent is a simple "splitter window" in which two child components are divided by a split bar that can be moved.\n
	\n
	SplitComponent is useful when you want to divide an area into two halves and let the user move a middle split bar
	to resize the halves. You can split the area either vertically or horizontally. You can also set a "gravity" amount for
	the middle split bar so that when you resize the window the split bar will move proportionally to grow or shrink the
	child components.\n
	\n
	By default, a SplitComponent fills the entire area of it's parent. To limit its size within the parent, call
	FitToParent(false) and then set the bounds as needed.
	\n
	SplitComponents can be nested so that you can create more complex divisions, but since a SplitComponent fills the entire
	area of it's parent, it should be put inside another component first.
	\n
	To use the SplitComponent, you supply two child components to be the left and right sides (or top and bottom for a
	horzontal split). Here's an example:

	@code{.cpp}

	MyLeftComponent myLeftComponent;
	MyRightComponent myRightComponent;
	SplitComponent* pSplitComponent = new SplitComponent(myLeftComponent, myRightComponent);

	pSplitComponent->SetSplitBarPosition(getWidth() / 2);	// Set the split bar to halfway so the area is divided equally.
															// The split bar thickness is 8 pixels by default, but you can change it.

	addAndMakeVisible(pSplitComponent);		// Add the SplitComponent to this component is, (Maybe the MainComponent)

	@endcode

	If it's more convenient, you can construct an empty SplitComponent first, and then add the left and right child components later.\n
	\n
	Note that when the SplitComponent is resized or its split bar is moved, it resizes the child components.
	So if you have a child component that wants to resize itself according to its parent's size (for example, it
	fills the area of its parent) you need to first enclose it in another component. This can be as simple as
	just a Component object. For example, say that  MyLeftComponent above wants to fill the area of its parent,
	you could implement it like this:

	@code{.cpp}

	MyLeftComponent myLeftComponent;
	MyRightComponent myRightComponent;

	Component leftPanel;
	leftPanel.addAndMakeVisible(myLeftComponent);

	SplitComponent* pSplitComponent = new SplitComponent(leftPanel, myRightComponent);

	...etc.
	@endcode

	This would be the case when nesting a SplitComponent that is set to fit to the area of it's parent.\n
	\n
	The split bar conforms to whatever look and feel you've set since it calls drawStretchableLayoutResizerBar() to paint itself.

*/
class SplitComponent : public Component
{
public:
    SplitComponent(bool isVertical = true);
    SplitComponent(Component& topLeftComponent, Component& bottomRightComponent, bool isVertical = true);
    virtual ~SplitComponent();

    /**
    	Sets (or replaces) the top component if the SplitComponent is horizontal, or the left component if the SplitComponent is vertical.

    	The SplitComponent does not own the top/left component. It is not deleted when the SplitComponent is deleted.
    	The caller must delete it.
    	*/
    void SetTopLeftComponent(Component &component);


    /**
    	Sets (or replaces) the bottom component if the SplitComponent is horizontal, or the right component if the SplitComponent is vertical.

    	The SplitComponent does not own the bottom/right component. It is not deleted when the SplitComponent is deleted.
    	The caller must delete it.
    */
    void SetBottomRightComponent(Component &component);


    /**
    	Sets the splitbar position vertically in terms of pixels if this is a horizontal SplitComponent.\n
    	Sets the splitbar position horizontally in terms of pixels if this is a vertical SplitComponent.\n

    	The position to which the split bar is actually set will be adjusted so it is inside the SplitComponent.

    	@param[in] newPositionInPixels	The new top/left position of the split bar. For a vertical split, the
    									position will be limited to the width of the SplitComponent minus the
    									thickness of the split bar. For a horizontal split, the position will
    									be limited to the width of the SplitComponent minus the	thickness of
    									the split bar.
    */
    void SetSplitBarPosition(int newPositionInPixels);


    /**
    	Gets the splitbar vertical position in terms of pixels if this is a horizontal SplitComponent.
    	Sets the splitbar horizontal position in terms of pixels if this is a vertical SplitComponent.

    	@returns	The current split bar position.
    */
    int GetSplitBarPosition() const;


    /**
    	Enables or disables the SplitComponent to always entirely fill it parent's client area.

    	@param[in]	fitToParent		If true, the SplitComponent will always entirely fill it parent's client area. (The default)
    								If false, the not fill it parent's client area, in which case you must set the bounds
    								of the SplitContainer to be what you want.
    	@see GetFitToParent

    */
    void SetFitToParent(bool fitToParent);

    /**
    	Retrieves the current state of the fit-to-parent setting.

    	@retval		true	If the SplitComponent is set to entirely fill it parent's client area.
    	@retval		false	If the SplitComponent is notset to entirely fill it parent's client area.

    	@see SetFitToParent
    */
    bool GetFitToParent() const;


    /**
    	Sets the splitbar width in terms of pixels if this is a vertical SplitComponent.\n
    	Sets the splitbar height in terms of pixels if this is a horizontal SplitComponent.\n
    	The splitbar thickness can be set to 0 pixels in which case it's not visible and the
    	user cannot move it. But its position can still be set programatically.

    	@param[in] newThicknessInPixels		The new width/height of the split bar.
    	@see GetSplitBarThickness
    	*/
    void SetSplitBarThickness(int newThicknessInPixels);

    /**
    	Gets the splitbar width in terms of pixels if this is a vertical SplitComponent.\n
    	Gets the splitbar height in terms of pixels if this is a horizontal SplitComponent.\n

    	@returns The current split bar thickness.
    	@see SetSplitBarThickness
    */
    int GetSplitBarThickness() const;

    /**
    	Sets the amount by which each component grows or shrinks when the windows is resized.

    	- 0.0 means only the bottom/right component is resized. (The default)
    	- 0.5 means each component is resized equally.
    	- 1.0 means only the top/left component is resized.

    	@param[in] gravity	The gravity of the split bar. A value between 0.0 and 1.0.
    	@see GetSplitBarGravity
    */
    void SetSplitBarGravity(double gravity);

    /**
    	Gets the amount by which each component grows or shrinks when he windows is resized.

    	@returns		The current gravity of the split bar. A value between 0.0 and 1.0.
    	@see SetSplitBarGravity
    */
    double GetSplitBarGravity() const;

    /**
    	Sets the split direction of the SplitComponent.

    	@param[in] isVertical	If true, the split direction will be set (changed) to be vertical.
    							If false, the split direction will be set (changed) to be horizontal.
    	@see IstVerticalSplit
    */
    void SetVerticalSplit(bool isVertical);

    /**
    	Gets the split direction of the SplitComponent.

    	@retval true	The split direction is currently vertical.
    	@retval false	The split direction is currently horizontal.
    	@see SetVerticalSplit
    */
    bool IsVerticalSplit() const;


    /** Called when this component's immediate parent has been resized.

    @see resized, parentHierarchyChanged
    */
    void parentSizeChanged() override;

    void resized() override;

    /**
    	This is defined for when the SplitContainer is added to a parent. It will resize itself to that parent.
    */
    void parentHierarchyChanged() override;

private:
    class SplitBar : public Component
    {
    public:
        SplitBar(SplitComponent& splitComponent, bool isVertical);
        void SetVertical(bool isVertical);
        bool IsVertical();
        void paint(Graphics&) override;
        void mouseDown(const MouseEvent&) override;
        void mouseDrag(const MouseEvent&) override;

    private:
        bool			m_isVertical;
        SplitComponent& m_splitComponent;
        int				m_mouseDownPos;
    };

    void CommonConstruction();

    /**
    	Lays out the two components on either side of the split bar by setting their bounds according to where the split bar is.

    	Layout assumes that the split bar bounds and position have already been set.
    */
    void Layout();

    bool					m_isVertical;
    bool					m_fitToParent;
    ScopedPointer<SplitBar>	m_pSplitBar;
    Component*				m_pTopLeftComponent;			///< A pointer to the top component if the splitter is horizontal, or the left component if the splitter is vertical.
    Component*				m_pBottomRightComponent;		///< A pointer to the bottom component if the splitter is horizontal, or the right component if the splitter is vertical.
    double					m_splitBarGravity;

    // The following two values are used with gravity. The purpose of keeping these two is to avoid having the
    // splitbar walk left or right due to accumulated roundoff error when the SplitComponent is resized many times.
    int						m_lastSplitbarMovePosition;		///< The position of the split bar the last time it was moved manually or programmatically. (Not as a result of a window resize)
    int						m_lastSplitbarMoveExtent;		///< The width/height of the SplitComponent the last time the split bar was moved manually or programmatically. (Not as a result of a window resize)
};


#endif  // SPLITCOMPONENT_H_INCLUDED
