/*
  ==============================================================================

    Split component for Juce

    File: SplitComponent.cpp
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

#include <cassert>
#include "SplitComponent.h"


SplitComponent::SplitComponent(bool isVertical)
    : m_pTopLeftComponent(NULL)
    , m_pBottomRightComponent(NULL)
    , m_isVertical(isVertical)
    , m_splitBarGravity(0.0)
    , m_fitToParent(true)
{
    CommonConstruction();
}

SplitComponent::SplitComponent(Component& topLeftComponent, Component& bottomRightComponent, bool isVertical)
    : m_pTopLeftComponent(&topLeftComponent)
    , m_pBottomRightComponent(&bottomRightComponent)
    , m_isVertical(isVertical)
    , m_splitBarGravity(0.0)
    , m_fitToParent(false)
{
    CommonConstruction();
}

void SplitComponent::CommonConstruction()
{
    // Set the bounds of this split component based on the parent's size (if there is a parent).
    parentSizeChanged();

    m_pSplitBar = new SplitBar(*this, m_isVertical);

    // set default position and width of the SplitBar
    if (m_isVertical)
    {
        m_pSplitBar->setBounds(getWidth() / 2, 0, 8, getHeight());
        m_lastSplitbarMovePosition = m_pSplitBar->getX();
        m_lastSplitbarMoveExtent = getWidth();
    }
    else
    {
        m_pSplitBar->setBounds(0, getHeight() / 2, getWidth(), 8);
        m_lastSplitbarMovePosition = m_pSplitBar->getY();
        m_lastSplitbarMoveExtent = getHeight();
    }

    if (m_pTopLeftComponent)
    {
        addAndMakeVisible(m_pTopLeftComponent, 0);
    }
    if (m_pBottomRightComponent)
    {
        addAndMakeVisible(m_pBottomRightComponent, 1);
    }
    addAndMakeVisible(m_pSplitBar, -1);	// make sure the split bar is in front of other components.
}


SplitComponent::~SplitComponent()
{
    removeAllChildren();
}

void SplitComponent::SetTopLeftComponent(Component &component)
{
    if (m_pTopLeftComponent)
    {
        removeChildComponent(m_pTopLeftComponent);
    }
    m_pTopLeftComponent = &component;
    addAndMakeVisible(m_pTopLeftComponent, 0);
    Layout();
}

void SplitComponent::SetBottomRightComponent(Component &component)
{
    if (m_pBottomRightComponent)
    {
        removeChildComponent(m_pBottomRightComponent);
    }
    m_pBottomRightComponent = &component;
    addAndMakeVisible(m_pBottomRightComponent, 1);
    Layout();
}


void SplitComponent::Layout()
{
    Rectangle<int> topLeftComponentRect;
    Rectangle<int> bottomRightComponentRect;

    topLeftComponentRect.setTop(0);
    topLeftComponentRect.setLeft(0);

    if (m_isVertical)
    {
        int splitBarPosX = m_pSplitBar->getX();
        int splitBarWidth = m_pSplitBar->getWidth();

        topLeftComponentRect.setWidth(splitBarPosX);
        topLeftComponentRect.setHeight(getHeight());

        bottomRightComponentRect.setPosition(splitBarPosX + splitBarWidth, 0);
        bottomRightComponentRect.setWidth(getWidth() - (splitBarPosX + splitBarWidth));
        bottomRightComponentRect.setHeight(getHeight());
    }
    else
    {
        int splitBarPosY = m_pSplitBar->getY();
        int splitBarHeight = m_pSplitBar->getHeight();

        topLeftComponentRect.setWidth(getWidth());
        topLeftComponentRect.setHeight(splitBarPosY);

        bottomRightComponentRect.setPosition(0, splitBarPosY + splitBarHeight);
        bottomRightComponentRect.setWidth(getWidth());
        bottomRightComponentRect.setHeight(getHeight() - (splitBarPosY + splitBarHeight));
    }

    if (m_pTopLeftComponent)
    {
        m_pTopLeftComponent->setBounds(topLeftComponentRect);
    }
    if (m_pBottomRightComponent)
    {
        m_pBottomRightComponent->setBounds(bottomRightComponentRect);
    }

}

void SplitComponent::SetSplitBarPosition(int newPosition)
{
    if (m_isVertical)
    {
        newPosition = jmax(0, jmin(newPosition, getWidth() - m_pSplitBar->getWidth()));	// limit

        m_pSplitBar->setBounds(newPosition, 0, m_pSplitBar->getWidth(), m_pSplitBar->getHeight());
        m_lastSplitbarMoveExtent = getWidth();
    }
    else
    {
        newPosition = jmax(0, jmin(newPosition, getHeight() - m_pSplitBar->getHeight()));	// limit

        m_pSplitBar->setBounds(0, newPosition, m_pSplitBar->getWidth(), m_pSplitBar->getHeight());
        m_lastSplitbarMoveExtent = getHeight();
    }
    m_lastSplitbarMovePosition = newPosition;

    Layout();
}


int SplitComponent::GetSplitBarPosition() const
{
    return m_isVertical ? m_pSplitBar->getX() : m_pSplitBar->getY();
}

void SplitComponent::SetSplitBarThickness(int newThickness)
{
    // The position of the split bar may now be invalid as a result of the new thickness, so adjust.
    int splitBarPosition;

    if (m_isVertical)
    {
        newThickness = jmax(0, jmin(newThickness, getWidth() - 2));

        splitBarPosition = jmin(m_pSplitBar->getX(), getWidth() - newThickness);	// limit split bar position
        m_pSplitBar->setBounds(splitBarPosition, 0, newThickness, m_pSplitBar->getHeight());
    }
    else
    {
        newThickness = jmax(0, jmin(newThickness, getHeight() - 2));

        splitBarPosition = jmin(m_pSplitBar->getY(), getHeight() - newThickness);
        m_pSplitBar->setBounds(0, splitBarPosition, m_pSplitBar->getWidth(), newThickness);
    }

    SetSplitBarPosition(splitBarPosition);
}

int SplitComponent::GetSplitBarThickness() const
{
    return m_isVertical ? m_pSplitBar->getWidth() : m_pSplitBar->getHeight();
}

void SplitComponent::SetSplitBarGravity(double gravity)
{
    assert((gravity >= 0.0) && (gravity <= 1.0));

    // limit
    m_splitBarGravity = (gravity < 0.0) ? 0.0 : (gravity > 1.0) ? 1.0 : gravity;
}

double SplitComponent::GetSplitBarGravity() const
{
    return m_splitBarGravity;;
}

void SplitComponent::SetVerticalSplit(bool isVertical)
{
    if (m_isVertical != isVertical)
    {
        // Change the orientation of the split bar.
        // The split bar position might now be invalid (outside the SplitComponent bounds). So move it to be valid.

        int splitbarThickness;
        int splitBarPosition;

        if (m_isVertical)	// change from vertical to horizontal
        {
            splitbarThickness = m_pSplitBar->getWidth();
            splitBarPosition = m_pSplitBar->getX();

            // limit split bar position
            splitBarPosition = jmin(splitBarPosition, getHeight() - splitbarThickness);

            m_pSplitBar->setBounds(0, splitBarPosition, getWidth(), splitbarThickness);
        }
        else	// change from horizontal to vertical
        {
            splitbarThickness = m_pSplitBar->getHeight();
            splitBarPosition = m_pSplitBar->getY();

            // limit split bar position
            splitBarPosition = jmin(splitBarPosition, getWidth() - splitbarThickness);

            m_pSplitBar->setBounds(splitBarPosition, 0, splitbarThickness, getHeight());
        }

        m_pSplitBar->SetVertical(isVertical);
        m_isVertical = isVertical;
        SetSplitBarPosition(splitBarPosition);
    }
}

bool SplitComponent::IsVerticalSplit() const
{
    return m_isVertical;
}

void SplitComponent::SetFitToParent(bool fitToParent)
{
    if (m_fitToParent != fitToParent)
    {
        m_fitToParent = fitToParent;
        parentSizeChanged();
    }
}

bool SplitComponent::GetFitToParent() const
{
    return m_fitToParent;
}

void SplitComponent::resized()
{
    if (m_isVertical)
    {
        // Account for gravity
        int widthDiff = getWidth() - m_lastSplitbarMoveExtent;		// Get the difference in SplitComponent width since the split bar was moved manually.
        widthDiff = int(widthDiff * m_splitBarGravity);				// Get a percentage of that difference based on the gravity setting.
        int newXPosition = widthDiff + m_lastSplitbarMovePosition;	// Add the difference to getthe new desired right position. (which may get limited below)

        // Keep the split bar inside the visible area
        int maxSplitPosition = getWidth() - m_pSplitBar->getWidth();
        newXPosition = juce::jmax(0, juce::jmin(maxSplitPosition, newXPosition));

        // Essentially, adjust the height of the split bar to be the height of the SplitComponent.
        m_pSplitBar->setBounds(newXPosition, 0, m_pSplitBar->getWidth(), getHeight());
    }
    else
    {
        // Account for gravity
        int heightDiff = getHeight() - m_lastSplitbarMoveExtent;		// Get the difference in SplitComponent width since the split bar was moved manually.
        heightDiff = int(heightDiff * m_splitBarGravity);				// Get a percentage of that difference based on the gravity setting.
        int newYPosition = heightDiff + m_lastSplitbarMovePosition;		// Add the difference to getthe new desired right position. (which may get limited below)

        // Keep the split bar inside the visible area
        newYPosition = juce::jlimit(0, getHeight() - m_pSplitBar->getHeight(), newYPosition);

        // Essentially, adjust the width of the split bar to be the width of the SplitComponent.
        m_pSplitBar->setBounds(0, newYPosition, getWidth(), m_pSplitBar->getHeight());
    }

    Layout();
}

/**
	Resize the SplitComponent to fills the area of it's parent if fit-to-parent is set.
*/
void SplitComponent::parentSizeChanged()
{
    Component* pParentComponent = getParentComponent();

    if (pParentComponent && m_fitToParent)
    {
        // Occupy the entire space of the parent component.

        // Use Rectangle::setPosition(0,0) and NOT Rectangle::setLeft(0) Rectangle::setTop(0).
        // The later two would change the width and height of the rectangle so that the right and
        // bottom edges stay in the same place. setPosition does not have that side effect.
        Rectangle<int> bounds = pParentComponent->getBounds();
        bounds.setPosition(0, 0);
        setBounds(bounds);	// This will cause SplitComponent::resized() to be called.
    }
}


void SplitComponent::parentHierarchyChanged()
{
    // If the parent changed, we no longer know if the split bar position is within the parent's bounds.
    // So treat it as if the parent size changed. This also takes care of the case where a SplitComponent is added
    // to a parent for the first time and the split position was not initialized.
    parentSizeChanged();
}



//////////////////////////////////////////////////////////////////////////////////////////////////
//
//		Splitbar
//
//
SplitComponent::SplitBar::SplitBar(SplitComponent& splitComponent, bool isVertical)
    : m_isVertical(isVertical)
    , m_splitComponent(splitComponent)
{
    setRepaintsOnMouseActivity(true);
    setMouseCursor(isVertical ? MouseCursor::LeftRightResizeCursor : MouseCursor::UpDownResizeCursor);
}


void SplitComponent::SplitBar::paint(Graphics& g)
{

    getLookAndFeel().drawStretchableLayoutResizerBar(g,
            getWidth(), getHeight(),
            m_isVertical,
            isMouseOver(),
            isMouseButtonDown());
}

void SplitComponent::SplitBar::mouseDown(const MouseEvent&)
{
    const Point<int> pos = getPosition();
    m_mouseDownPos = m_isVertical ? pos.getX() : pos.getY();
}

void SplitComponent::SplitBar::mouseDrag(const MouseEvent& e)
{
    int desiredPos = m_mouseDownPos + (m_isVertical ? e.getDistanceFromDragStartX() : e.getDistanceFromDragStartY());


    // Adjust position to keep the split bar inside the SplitComponent bounds.
    desiredPos = jmax(desiredPos, 0);
    if (m_isVertical)
    {
        desiredPos = jmin(desiredPos, m_splitComponent.getWidth() - getWidth());
    }
    else
    {
        desiredPos = jmin(desiredPos, m_splitComponent.getHeight() - getHeight());
    }

    if (desiredPos != m_mouseDownPos)
    {
        m_splitComponent.SetSplitBarPosition(desiredPos);
    }
}

void SplitComponent::SplitBar::SetVertical(bool isVertical)
{
    if (m_isVertical != isVertical)
    {
        m_isVertical = isVertical;
        setMouseCursor(m_isVertical ? MouseCursor::LeftRightResizeCursor : MouseCursor::UpDownResizeCursor);
    }
}

bool SplitComponent::SplitBar::IsVertical()
{
    return m_isVertical;
}
