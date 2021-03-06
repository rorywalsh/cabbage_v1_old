/*
  Copyright (C) 2010 Rory Walsh, Damien Rennick

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

#ifndef _CabbageWidgets_H__
#define _CabbageWidgets_H__

#include "../JuceLibraryCode/JuceHeader.h"
#include "CabbageLookAndFeel.h"
#include "CabbageUtils.h"
#include "CabbageGUIClass.h"
#include "CabbageTable.h"
#include "Table.h"
#include "XYPad.h"
#include "Soundfiler.h"

class CabbagePluginAudioProcessorEditor;

class InfoWindow   : public DocumentWindow
{
    ScopedPointer<WebBrowserComponent> htmlInfo;

public:
    //==============================================================================
    InfoWindow(LookAndFeel* look, String file)  : DocumentWindow (String("Information"),
                Colours::black,
                DocumentWindow::allButtons)
    {
        setSize(600, 400);
        this->setTitleBarHeight(20);
        htmlInfo = new WebBrowserComponent(false);
        htmlInfo->setBounds(0, 0, 600, 400);
        setContentOwned(htmlInfo, true);
        htmlInfo->goToURL(file);
    }

    ~InfoWindow() {};

    void closeButtonPressed()
    {
        setVisible(false);
    };

};

//==============================================================================
// custom button component with optional surrounding groupbox
//==============================================================================
class CabbageButton : public Component
{
    int offX, offY, offWidth, offHeight, pivotx, pivoty, latched, svgDebug;
    String buttonType;
    String name, caption, tooltipText, buttonText, colour, fontcolour, oncolour, onfontcolour;
    float rotate;
    File svgFileButtonOn, svgFileButtonOff, svgPath;
public:
    ScopedPointer<GroupComponent> groupbox;
    ScopedPointer<TextButton> button;
    //---- constructor -----

    CabbageButton(CabbageGUIType &cAttr) :
        name(cAttr.getStringProp(CabbageIDs::name)),
        caption(cAttr.getStringProp(CabbageIDs::caption)),
        buttonText(cAttr.getStringProp(CabbageIDs::text)),
        colour(cAttr.getStringProp(CabbageIDs::colour)),
        fontcolour(cAttr.getStringProp(CabbageIDs::fontcolour)),
        oncolour(cAttr.getStringProp(CabbageIDs::oncolour)),
        onfontcolour(cAttr.getStringProp(CabbageIDs::onfontcolour)),
        rotate(cAttr.getNumProp(CabbageIDs::rotate)),
        pivotx(cAttr.getNumProp(CabbageIDs::pivotx)),
        pivoty(cAttr.getNumProp(CabbageIDs::pivoty)),
        tooltipText(String::empty),
        svgDebug(cAttr.getNumProp(CabbageIDs::svgdebug))
    {
        setName(name);
        offX=offY=offWidth=offHeight=0;
        groupbox = new GroupComponent(String("groupbox_")+name);
        button = new TextButton(name);
        button->getProperties().set("svgpath",  cAttr.getStringProp(CabbageIDs::svgpath));


        svgFileButtonOn = File(cUtils::returnFullPathForFile(cAttr.getStringProp(CabbageIDs::svgbuttonon),
                               cAttr.getStringProp(CabbageIDs::parentdir)));
        svgFileButtonOff = File(cUtils::returnFullPathForFile(cAttr.getStringProp(CabbageIDs::svgbuttonoff),
                                cAttr.getStringProp(CabbageIDs::parentdir)));

        svgPath = File(cAttr.getStringProp(CabbageIDs::svgpath));

        setSVGs(cAttr);
        //cUtils::debug(button->getProperties().getWithDefault("svgbuttonwidth", 100).toString());

        addAndMakeVisible(groupbox);
        addAndMakeVisible(button);
        groupbox->setVisible(false);
        groupbox->getProperties().set("groupLine", var(1));
        button->setButtonText(buttonText);

        if(cAttr.getNumProp(CabbageIDs::radiogroup)!=0)
            button->setRadioGroupId(cAttr.getNumProp(CabbageIDs::radiogroup));

        if(caption.length()>0)
        {
            offX=10;
            offY=35;
            offWidth=-20;
            offHeight=-45;
            groupbox->setVisible(true);
            groupbox->setText(caption);
        }

        if(cAttr.getStringProp(CabbageIDs::popuptext).isNotEmpty())
        {
            tooltipText = cAttr.getStringProp(CabbageIDs::popuptext);
            button->setTooltip(tooltipText);
        }

        setAlpha(cAttr.getNumProp(CabbageIDs::alpha));

        button->setColour(TextButton::textColourOffId, Colour::fromString(fontcolour));
        button->setColour(TextButton::buttonColourId, Colour::fromString(colour));
        button->setColour(TextButton::textColourOnId, Colour::fromString(onfontcolour));
        button->setColour(TextButton::buttonOnColourId, Colour::fromString(oncolour));
        button->setButtonText(cAttr.getStringArrayPropValue(CabbageIDs::text, cAttr.getNumProp(CabbageIDs::value)));
        button->setToggleState((bool)cAttr.getNumProp(CabbageIDs::value), dontSendNotification);
    }
    //---------------------------------------------
    ~CabbageButton()
    {

    }

    void setSVGs(CabbageGUIType &cAttr)
    {
        cUtils::setSVGProperties(*button, svgFileButtonOn, svgPath, "buttonon");
        cUtils::setSVGProperties(*button, svgFileButtonOff, svgPath, "buttonoff");
    }

    //update controls
    void update(CabbageGUIType m_cAttr)
    {
        const MessageManagerLock mmLock;

        button->setColour(TextButton::textColourOffId, Colour::fromString(m_cAttr.getStringProp(CabbageIDs::fontcolour)));
        button->setColour(TextButton::textColourOnId, Colour::fromString(m_cAttr.getStringProp(CabbageIDs::onfontcolour)));

        button->setColour(TextButton::buttonColourId, Colour::fromString(m_cAttr.getStringProp(CabbageIDs::colour)));
        button->setColour(TextButton::buttonOnColourId, Colour::fromString(m_cAttr.getStringProp(CabbageIDs::oncolour)));

        cUtils::debug("OnColour:"+m_cAttr.getStringProp(CabbageIDs::oncolour));
        cUtils::debug("OffColour:"+m_cAttr.getStringProp(CabbageIDs::colour));
        cUtils::debug(m_cAttr.getNumProp(CabbageIDs::value));

        setAlpha(m_cAttr.getNumProp(CabbageIDs::alpha));
        if(rotate!=m_cAttr.getNumProp(CabbageIDs::rotate))
        {
            rotate = m_cAttr.getNumProp(CabbageIDs::rotate);
            setTransform(AffineTransform::rotation(rotate, getX()+m_cAttr.getNumProp(CabbageIDs::pivotx), getY()+m_cAttr.getNumProp(CabbageIDs::pivoty)));
        }
        if(!m_cAttr.getNumProp(CabbageIDs::visible))
        {
            setVisible(false);
            setEnabled(false);
        }
        else
        {
            setEnabled(true);
            setVisible(true);
        }
        if(!m_cAttr.getNumProp(CabbageIDs::active))
        {
            setEnabled(false);
        }
        else
        {
            setEnabled(true);
        }
        if(tooltipText!=m_cAttr.getStringProp(CabbageIDs::popuptext))
        {
            tooltipText = m_cAttr.getStringProp(CabbageIDs::popuptext);
            button->setTooltip(tooltipText);
        }
        if(m_cAttr.getNumProp(CabbageIDs::svgdebug)!=svgDebug)
        {
            setSVGs(m_cAttr);
            svgDebug = m_cAttr.getNumProp(CabbageIDs::svgdebug);
        }
        setBounds(m_cAttr.getBounds());
        repaint();
    }

    //---------------------------------------------
    void resized()
    {
        groupbox->setBounds(0, 0, getWidth(), getHeight());
        button->setBounds(offX, offY, getWidth()+offWidth, getHeight()+offHeight);
        if(rotate!=0)
            setTransform(AffineTransform::rotation(rotate, getX()+pivotx, pivoty+getY()));
        this->setWantsKeyboardFocus(false);
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageButton);
};

//==============================================================================
// custom slider component with optional surrounding groupbox
//==============================================================================
class CabbageRangeSlider : public Slider
{

public:
    CabbageRangeSlider(String name):
        Slider(name),
        thumbWidth(10)
    {

    }

    ~CabbageRangeSlider() {};

private:
    void mouseDown(const MouseEvent& event)
    {
        isDraggingRange=false;
        if(getSliderStyle()==Slider::TwoValueHorizontal)
        {
            topThumbDownPos = valueToProportionOfLength(getMinValue())*getWidth();
            bottomThumbDownPos = valueToProportionOfLength(getMaxValue())*getWidth();
        }
        else
        {
            rightThumbDownPos = valueToProportionOfLength(getMinValue())*getHeight();
            leftThumbDownPos = valueToProportionOfLength(getMaxValue())*getHeight();
        }
    }


    void mouseMove(const MouseEvent& event)
    {
        const float sliderMin = getMinValue();
        const float sliderMax = getMaxValue();
        if(getSliderStyle()==Slider::TwoValueHorizontal)
        {
            const float currentMouseX = event.getPosition().getX();
            const float bottomThumbPosition = valueToProportionOfLength(sliderMin)*getWidth();
            const float topThumbPosition = valueToProportionOfLength(sliderMax)*getWidth();
            const int minPosition = valueToProportionOfLength(getMinimum())*getWidth();
            const int maxPosition = valueToProportionOfLength(getMaximum())*getWidth();
            if(currentMouseX>bottomThumbPosition+thumbWidth && currentMouseX<topThumbPosition-thumbWidth)
            {
                if(topThumbPosition>minPosition && bottomThumbPosition<maxPosition)
                    setMouseCursor(MouseCursor::DraggingHandCursor);
            }
            else
                setMouseCursor(MouseCursor::NormalCursor);
        }
        else
        {
            const int minPosition = valueToProportionOfLength(getMinimum())*getHeight();
            const int maxPosition = valueToProportionOfLength(getMaximum())*getHeight();
            const float currentMouseY = event.getPosition().getY();
            const float leftThumbPosition = getHeight()-valueToProportionOfLength(sliderMin)*getHeight();
            const float rightThumbPosition = getHeight()-valueToProportionOfLength(sliderMax)*getHeight();
            if(currentMouseY<leftThumbPosition-thumbWidth && currentMouseY>rightThumbPosition+thumbWidth)
            {
                if(rightThumbPosition>minPosition && leftThumbPosition<maxPosition)
                    setMouseCursor(MouseCursor::DraggingHandCursor);
            }
            else
                setMouseCursor(MouseCursor::NormalCursor);
        }
    }

    void mouseDrag(const MouseEvent& event)
    {
        const float sliderMin = getMinValue();
        const float sliderMax = getMaxValue();
        if(getSliderStyle()==Slider::TwoValueHorizontal)
        {
            const float currentMouseX = event.getPosition().getX();
            const float bottomThumbPosition = valueToProportionOfLength(sliderMin)*getWidth();
            const float topThumbPosition = valueToProportionOfLength(sliderMax)*getWidth();
            const int distanceFromStart = event.getDistanceFromDragStartX();

            if(currentMouseX>bottomThumbPosition+thumbWidth && currentMouseX<topThumbPosition-thumbWidth)
            {
                setMinValue(proportionOfLengthToValue(topThumbDownPos+distanceFromStart)/getWidth());
                setMaxValue(proportionOfLengthToValue(bottomThumbDownPos+distanceFromStart)/getWidth());
                setMouseCursor(MouseCursor::DraggingHandCursor);
                isDraggingRange = true;
            }

            if(isDraggingRange==false)
            {
                setMouseCursor(MouseCursor::NormalCursor);
                if(event.getPosition().getY()>getHeight()/2.f)
                {
                    if(currentMouseX>topThumbPosition-thumbWidth && currentMouseX<topThumbPosition+thumbWidth)
                        setMaxValue(proportionOfLengthToValue(currentMouseX/getWidth()));
                }
                else
                {
                    if(currentMouseX>bottomThumbPosition-thumbWidth && currentMouseX<bottomThumbPosition+thumbWidth)
                        setMinValue(proportionOfLengthToValue(currentMouseX/getWidth()));
                }
            }

        }
        else//vertical slider
        {
            const int minPosition = valueToProportionOfLength(getMinimum())*getHeight();
            const int maxPosition = valueToProportionOfLength(getMaximum())*getHeight();
            const float currentMouseY = event.getPosition().getY();
            const float leftThumbPosition = getHeight()-valueToProportionOfLength(sliderMin)*getHeight();
            const float rightThumbPosition = getHeight()-valueToProportionOfLength(sliderMax)*getHeight();
            const int distanceFromStart = event.getDistanceFromDragStartY();

            if(currentMouseY<leftThumbPosition-thumbWidth && currentMouseY>rightThumbPosition+thumbWidth)
            {
                setMinValue(proportionOfLengthToValue((rightThumbDownPos+(1.f-distanceFromStart)))/getHeight());
                setMaxValue(proportionOfLengthToValue((leftThumbDownPos+(1.f-distanceFromStart)))/getHeight());
                setMouseCursor(MouseCursor::DraggingHandCursor);
                isDraggingRange = true;
            }

            if(isDraggingRange==false)
            {
                setMouseCursor(MouseCursor::NormalCursor);
                if(event.getPosition().getX()>getWidth()/2.f)
                {
                    if(currentMouseY>rightThumbPosition-thumbWidth && currentMouseY<rightThumbPosition+thumbWidth)
                        setMaxValue(proportionOfLengthToValue(1.f-(currentMouseY/getHeight())));
                }
                else
                {
                    if(currentMouseY>leftThumbPosition-thumbWidth && currentMouseY<leftThumbPosition+thumbWidth)
                        setMinValue(proportionOfLengthToValue(1.f-(currentMouseY/getHeight())));
                }
            }
        }
    }

    int topThumbDownPos, bottomThumbDownPos, rightThumbDownPos, leftThumbDownPos;
    const int thumbWidth;
    bool isDraggingRange;
};
//==============================================================================
// custom slider components
//==============================================================================
class CabbageSlider : public Component,
    public ChangeBroadcaster
{
    int offX, offY, offWidth, offHeight, plantX, plantY, pivotx, pivoty, svgDebug;
    String sliderType, compName, cl;
    int resizeCount;
    String tracker;
    File svgFileSlider, svgFileSliderBg, svgPath;
    String name, text, caption, kind, colour, fontColour, textColour, trackerFill, outlineColour, channel, channel2;
    int textBox, decPlaces;
    double min, max, value;
    ScopedPointer<Label> textLabel;
    float incr, skew, trackerThickness, rotate, velocity;
    ScopedPointer<CabbageLookAndFeel> lookAndFeel;
public:

    ScopedPointer<GroupComponent> groupbox;
    ScopedPointer<Slider> slider;
    bool shouldDisplayPopup;
    String tooltipText;
    //---- constructor -----
    CabbageSlider(CabbageGUIType &cAttr) : plantX(-99), plantY(-99),
        name(cAttr.getStringProp(CabbageIDs::name)),
        caption(cAttr.getStringProp(CabbageIDs::caption)),
        colour(cAttr.getStringProp(CabbageIDs::colour)),
        fontColour(cAttr.getStringProp(CabbageIDs::fontcolour)),
        textColour(cAttr.getStringProp(CabbageIDs::textcolour)),
        outlineColour(cAttr.getStringProp(CabbageIDs::outlinecolour)),
        sliderType(cAttr.getStringProp(CabbageIDs::kind)),
        compName(cAttr.getStringProp(CabbageIDs::caption)),
        cl(cAttr.getStringProp(CabbageIDs::colour)),
        tracker(cAttr.getStringProp(CabbageIDs::trackercolour)),
        decPlaces(cAttr.getNumProp(CabbageIDs::decimalplaces)),
        textBox(cAttr.getNumProp(CabbageIDs::textbox)),
        trackerThickness(cAttr.getNumProp(CabbageIDs::trackerthickness)),
        text(cAttr.getStringProp(CabbageIDs::text)),
        channel(cAttr.getStringProp(CabbageIDs::channel)),
        lookAndFeel(new CabbageLookAndFeel()),
        shouldDisplayPopup(false),
        value(cAttr.getNumProp(CabbageIDs::value)),
        textLabel(new Label()),
        rotate(cAttr.getNumProp(CabbageIDs::rotate)),
        pivotx(cAttr.getNumProp(CabbageIDs::pivotx)),
        pivoty(cAttr.getNumProp(CabbageIDs::pivoty)),
        svgDebug(0)
    {
        setName(name);

        offX=offY=offWidth=offHeight=0;
        groupbox = new GroupComponent(String("groupbox_")+name);

        if(sliderType=="vertical2" || sliderType=="horizontal2")
            slider = new CabbageRangeSlider(channel);
        else
            slider = new Slider(text);

        if(textBox<1)
        {
            slider->setTextBoxStyle (Slider::NoTextBox, true, 0, 0);
            shouldDisplayPopup=true;
        }

        if(sliderType=="horizontal2" || sliderType=="vertical2")
            channel2 = cAttr.getStringArrayPropValue(CabbageIDs::channel, 1);

        svgFileSlider = File(cUtils::returnFullPathForFile(cAttr.getStringProp(CabbageIDs::svgslider),
                             cAttr.getStringProp(CabbageIDs::parentdir)));
        svgFileSliderBg = File(cUtils::returnFullPathForFile(cAttr.getStringProp(CabbageIDs::svgsliderbg),
                               cAttr.getStringProp(CabbageIDs::parentdir)));
        svgPath = File(cAttr.getStringProp(CabbageIDs::svgpath));

        setSVGs(cAttr);

        slider->toFront(true);

        velocity = cAttr.getNumProp(CabbageIDs::velocity);

        slider->addMouseListener(this, false);
        textLabel->setColour(Label::textColourId, Colour::fromString(textColour));
        textLabel->setColour(Label::outlineColourId, Colours::transparentBlack);
        //slider->setPopupDisplayEnabled (true, 0);


        slider->setColour(Slider::textBoxHighlightColourId, Colours::lime.withAlpha(.2f));
        //slider->setColour(Slider::rotarySliderFillColourId, Colours::red);
        slider->setColour(Slider::thumbColourId, Colour::fromString(cl));
        //slider->setColour(Slider::textBoxTextColourId, Colour::fromString(fontColour));
        slider->setColour(Label::textColourId, Colour::fromString(fontColour));
        slider->setColour(Label::backgroundColourId, cUtils::getBackgroundSkin());
        slider->setColour(TextEditor::textColourId, Colour::fromString(fontColour));
        slider->setColour(Slider::textBoxTextColourId, Colour::fromString(fontColour));
        slider->setColour(Slider::textBoxBackgroundColourId, Colours::black);
        slider->setColour(Slider::textBoxHighlightColourId, Colours::white);
        slider->setColour(Slider::trackColourId, Colour::fromString(tracker));

        slider->setColour(Label::outlineColourId, cUtils::getBackgroundSkin());
        slider->setColour(Slider::Slider::rotarySliderOutlineColourId, Colour::fromString(outlineColour));


        // slider->setLookAndFeel(lookAndFeel);
        textLabel->setColour(Label::outlineColourId, Colours::transparentBlack);
        addAndMakeVisible(slider);
        addAndMakeVisible(groupbox);
        addAndMakeVisible(textLabel);
        textLabel->setVisible(false);
        groupbox->setVisible(false);
        groupbox->getProperties().set("groupLine", var(1));
        groupbox->setColour(GroupComponent::textColourId, Colour::fromString(fontColour));
        groupbox->setColour(TextButton::buttonColourId, cUtils::getComponentSkin());
        slider->getProperties().set("decimalPlaces", decPlaces);
        slider->getProperties().set("trackerthickness", trackerThickness);
        slider->getProperties().set("gradient", cAttr.getNumProp(CabbageIDs::gradient));


        setAlpha(cAttr.getNumProp(CabbageIDs::alpha));

        this->setWantsKeyboardFocus(false);
        resizeCount = 0;


        if(cAttr.getNumProp(CabbageIDs::min)==cAttr.getNumProp(CabbageIDs::max)
                || cAttr.getNumProp(CabbageIDs::min)>cAttr.getNumProp(CabbageIDs::max))
        {
            //cUtils::showMessage("Your min value is the same or greater than your max value.\nCabbage will now reduce your min value so that it falls into range", &getLookAndFeel());
            cAttr.setNumProp(CabbageIDs::min, cAttr.getNumProp(CabbageIDs::max)-.001);
        }

        min = (cAttr.getNumProp(CabbageIDs::minvalue)==-9999 ? cAttr.getNumProp(CabbageIDs::min) : cAttr.getNumProp(CabbageIDs::minvalue));
        max = (cAttr.getNumProp(CabbageIDs::maxvalue)==-9999 ? cAttr.getNumProp(CabbageIDs::max) : cAttr.getNumProp(CabbageIDs::maxvalue));

//        if(sliderType=="vertical" || sliderType=="horizontal" || sliderType=="rotary")
//            slider->setValue(cAttr.getNumProp(CabbageIDs::value));
//        else
//            setupMinMaxValue();

        //Logger::writeToLog(String(cAttr.getNumProp(CabbageIDs::value)));

        if(velocity > 0)
        {
            slider->setVelocityModeParameters(velocity, 1, 0.0, true);
            slider->setVelocityBasedMode(true);
        }
        else
            slider->setVelocityBasedMode(false);

        incr = cAttr.getNumProp(CabbageIDs::sliderincr);
        skew = cAttr.getNumProp(CabbageIDs::sliderskew);
        slider->setSkewFactor(cAttr.getNumProp(CabbageIDs::sliderskew));
        slider->setRange(cAttr.getNumProp(CabbageIDs::min), cAttr.getNumProp(CabbageIDs::max), cAttr.getNumProp(CabbageIDs::sliderincr));

        if(cAttr.getStringProp(CabbageIDs::popuptext).isNotEmpty())
            tooltipText = cAttr.getStringProp(CabbageIDs::popuptext);

        slider->setDoubleClickReturnValue(true, cAttr.getNumProp(CabbageIDs::value));

    }//--- end of constructor ----

    //---------------------------------------------
    ~CabbageSlider()
    {
    }

    void setSliderValue(float val)
    {
        value = val;
    }

    void setSVGs(CabbageGUIType &cAttr)
    {
        if (sliderType.contains("rotary"))
        {
            cUtils::setSVGProperties(*slider, svgFileSliderBg, svgPath, "rsliderbg");
            cUtils::setSVGProperties(*slider, svgFileSlider, svgPath, "rslider");
        }
        else if(sliderType.contains("horizontal"))
        {
            cUtils::setSVGProperties(*slider, svgFileSliderBg, svgPath, "hsliderbg");
            cUtils::setSVGProperties(*slider, svgFileSlider, svgPath, "hslider");
        }
        else if(sliderType.contains("vertical"))
        {
            cUtils::setSVGProperties(*slider, svgFileSliderBg, svgPath, "vsliderbg");
            cUtils::setSVGProperties(*slider, svgFileSlider, svgPath, "vslider");
        }
    }

    void setupMinMaxValue()
    {
        slider->setMinAndMaxValues(min, max);
    }

    bool shouldDisplayPopupValue()
    {
        return shouldDisplayPopup;
    }

    String getSliderType()
    {
        return sliderType;
    }

    void mouseDrag(const MouseEvent& event)
    {
        if(shouldDisplayPopup)
            sendChangeMessage();
    }


    void mouseMove (const MouseEvent &event)
    {
        if(shouldDisplayPopup)
            sendChangeMessage();
    }

    void mouseEnter (const MouseEvent &event)
    {
        if(shouldDisplayPopup)
            sendChangeMessage();
    }

    String getChannel()
    {
        return channel;
    }

    void setLabelText(String label)
    {
        textLabel->setText(label, dontSendNotification);
    }

    //update controls
    void update(CabbageGUIType m_cAttr)
    {
        const MessageManagerLock mmLock;
        slider->setColour(Slider::rotarySliderFillColourId, Colour::fromString(m_cAttr.getStringProp(CabbageIDs::colour)));
        slider->setColour(Slider::thumbColourId, Colour::fromString(m_cAttr.getStringProp(CabbageIDs::colour)));
        slider->setColour(Slider::textBoxTextColourId, Colour::fromString(m_cAttr.getStringProp(CabbageIDs::fontcolour)));
        slider->setColour(Slider::trackColourId, Colour::fromString(m_cAttr.getStringProp(CabbageIDs::trackercolour)));
        setBounds(m_cAttr.getBounds());
        slider->setName(m_cAttr.getStringProp(CabbageIDs::text));
        slider->setSkewFactor(m_cAttr.getNumProp(CabbageIDs::sliderskew));
        textLabel->setColour(Label::textColourId, Colour::fromString(m_cAttr.getStringProp(CabbageIDs::textcolour)));
        if(!m_cAttr.getNumProp(CabbageIDs::visible))
        {
            setVisible(false);
            setEnabled(false);
        }
        else
        {
            setVisible(true);
            setEnabled(true);
        }
        if(!m_cAttr.getNumProp(CabbageIDs::active))
        {
            setEnabled(false);
        }
        else
        {
            setEnabled(true);
        }
        if(rotate!=m_cAttr.getNumProp(CabbageIDs::rotate))
        {
            rotate = m_cAttr.getNumProp(CabbageIDs::rotate);
            setTransform(AffineTransform::rotation(rotate, getX()+m_cAttr.getNumProp(CabbageIDs::pivotx), getY()+m_cAttr.getNumProp(CabbageIDs::pivoty)));
        }
        if(tooltipText!=m_cAttr.getStringProp(CabbageIDs::popuptext))
        {
            tooltipText = m_cAttr.getStringProp(CabbageIDs::popuptext);
        }

        if(m_cAttr.getNumProp(CabbageIDs::svgdebug)!=svgDebug)
        {
            setSVGs(m_cAttr);
            svgDebug = m_cAttr.getNumProp(CabbageIDs::svgdebug);
        }
        setAlpha(m_cAttr.getNumProp(CabbageIDs::alpha));
        repaint();
    }

    //---------------------------------------------
    void resized()
    {

        //if rotary
        if (sliderType.contains("rotary"))
        {
            slider->setColour(Slider::rotarySliderFillColourId, Colour::fromString(cl));
            getProperties().set("type", var("rslider"));
            slider->setSliderStyle(Slider::RotaryVerticalDrag);
            slider->setValue((value));
            if(textBox>0)
                slider->setTextBoxStyle(Slider::TextBoxBelow, false, 40, 15);

            slider->setRotaryParameters(float_Pi * 1.2f, float_Pi * 2.8f, false);
            //if using group caption
            if (compName.length() > 0)
            {
                groupbox->setBounds(0, 0, getWidth(), getHeight());
                int textHeight = 20;
                int availableHeight = getHeight()-textHeight;
                offY = textHeight + 5;
                int sliderHeight = availableHeight - 10;
                int sliderWidth = sliderHeight;
                offX = (getWidth()/2) - (sliderWidth/2);
                slider->setBounds(offX, offY, sliderWidth, sliderHeight);
                groupbox->setText (compName);
                groupbox->setVisible(true);
                slider->toFront(true);
            }
            else
            {
                if(text.isNotEmpty())
                {
                    if(textBox>0)
                    {
                        textLabel->setBounds(0, 0, getWidth(), 20);
                        textLabel->setText(text, dontSendNotification);
                        textLabel->setJustificationType(Justification::centred);
                        textLabel->setVisible(true);
                        slider->setBounds(0, 20, getWidth(), getHeight()-20);
                    }
                    else
                    {
                        textLabel->setBounds(0, getHeight()-20, getWidth(), 20);
                        textLabel->setText(text, dontSendNotification);
                        textLabel->setJustificationType(Justification::centred);
                        textLabel->setVisible(true);
                        slider->setBounds(0, 0, getWidth(), getHeight()-15);
                    }
                }
                else
                    slider->setBounds(0, 0, getWidth(), getHeight());

            }
        }
        //else if vertical
        else if (sliderType.contains("vertical"))
        {

            if(sliderType=="vertical3")
            {
                slider->setSliderStyle(Slider::ThreeValueVertical);
                setupMinMaxValue();
                slider->setValue(value);
            }
            else if(sliderType=="vertical2")
            {
                slider->setSliderStyle(Slider::TwoValueVertical);
                setupMinMaxValue();
            }
            else
            {
                slider->setSliderStyle(Slider::LinearVertical);
                slider->setValue(value);
            }



            if(textBox>0)
                slider->setTextBoxStyle(Slider::TextBoxBelow, false, 40, 15);

            if(cl.length() > 0)
                slider->setColour(Slider::thumbColourId, Colour::fromString(cl));
            if (compName.length() > 0)
            {
                groupbox->setBounds(0, 0, getWidth(), getHeight());
                int textHeight = 20;
                int availableHeight = getHeight()-textHeight;
                offY = textHeight + 5;
                int sliderHeight = availableHeight - 10;
                int sliderWidth = getWidth() - 20;
                offX = (getWidth()/2) - (sliderWidth/2);
                slider->setBounds(offX, offY, sliderWidth, sliderHeight);
                groupbox->setVisible(true);
                groupbox->setText(compName);
                slider->toFront(true);
            }
            else
            {
                if(text.isNotEmpty())
                {
                    textLabel->setBounds(0, getHeight()-20, getWidth(), 20);
                    textLabel->setJustificationType(Justification::centred);
                    textLabel->setText(text, dontSendNotification);
                    textLabel->setVisible(true);
                    slider->setBounds(0, 0, getWidth(), getHeight()-20);
                }
                else
                    slider->setBounds(0, 0, getWidth(), getHeight());

            }
        }
        //else if horizontal
        else
        {
            if(sliderType=="horizontal3")
            {
                slider->setSliderStyle(Slider::ThreeValueHorizontal);
                setupMinMaxValue();
                slider->setValue(value);
            }
            else if(sliderType=="horizontal2")
            {
                slider->setSliderStyle(Slider::TwoValueHorizontal);
                setupMinMaxValue();
            }
            else
            {
                slider->setSliderStyle(Slider::LinearHorizontal);
                slider->setValue(value);
            }



            if(textBox>0)
                slider->setTextBoxStyle(Slider::TextBoxRight, false, 40, 15);
            if(cl.length() > 0)
                slider->setColour(Slider::thumbColourId, Colour::fromString(cl));
            if (compName.length() > 0)
            {
                groupbox->setBounds(0, 0, getWidth(), getHeight());
                int textHeight = 20;
                int availableHeight = getHeight()-textHeight;
                offY = textHeight + 5;
                int sliderHeight = availableHeight - 10;
                int sliderWidth = getWidth() - 20;
                offX = (getWidth()/2) - (sliderWidth/2);
                slider->setBounds(offX, offY, sliderWidth, sliderHeight);
                groupbox->setText(compName);
                groupbox->setVisible(true);
                slider->toFront(true);
            }
            else
            {
                if(text.isNotEmpty())
                {
                    float width = textLabel->getFont().getStringWidthFloat(text)+10.f;
                    textLabel->setBounds(0, 0, width, getHeight());
                    textLabel->setText(text, dontSendNotification);
                    textLabel->setVisible(true);
                    slider->setBounds(width-1, 0, getWidth()-width, getHeight());
                }
                else
                    slider->setBounds(0, 0, getWidth(), getHeight());
            }
        }

        //We only store the original dimensions the first time resized() is called.
        //Otherwise we would be passing incorrect values to the slider l+f methods...
        if (resizeCount == 0)
        {
            slider->getProperties().set(String("origHeight"), getHeight());
            slider->getProperties().set(String("origWidth"), getWidth());
            slider->getProperties().set(String("origX"), getX());
            slider->getProperties().set(String("origY"), getY());
        }
        resizeCount++;
        this->setWantsKeyboardFocus(false);

        if(rotate!=0)
            setTransform(AffineTransform::rotation(rotate, getX()+pivotx, pivoty+getY()));

    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageSlider);
};

//==============================================================================
// custom checkbox component with optional surrounding groupbox
//==============================================================================
class CabbageCheckbox : public Component
{
    int offX, offY, offWidth, offHeight, pivotx, pivoty, corners;
    float rotate;

public:
    ScopedPointer<GroupComponent> groupbox;
    ScopedPointer<ToggleButton> button;
    bool isRect;
    String name, caption, tooltipText, buttonText, colour, fontcolour, oncolour;
    //---- constructor -----
    CabbageCheckbox(CabbageGUIType &cAttr) :
        name(cAttr.getStringProp(CabbageIDs::name)),
        caption(cAttr.getStringProp(CabbageIDs::caption)),
        buttonText(cAttr.getStringProp(CabbageIDs::text)),
        colour(cAttr.getStringProp(CabbageIDs::colour)),
        fontcolour(cAttr.getStringProp(CabbageIDs::fontcolour)),
        oncolour(cAttr.getStringProp(CabbageIDs::oncolour)),
        isRect(cAttr.getStringProp(CabbageIDs::shape).equalsIgnoreCase("square")),
        rotate(cAttr.getNumProp(CabbageIDs::rotate)),
        pivotx(cAttr.getNumProp(CabbageIDs::pivotx)),
        pivoty(cAttr.getNumProp(CabbageIDs::pivoty)),
        tooltipText(String::empty),
        corners(cAttr.getNumProp(CabbageIDs::corners))

    {
        setName(name);
        offX=offY=offWidth=offHeight=0;
        groupbox = new GroupComponent(String("groupbox_")+name);
        button = new ToggleButton(name);
        button->getProperties().set("svgpath", cAttr.getStringProp(CabbageIDs::svgpath));
        addAndMakeVisible(groupbox);
        addAndMakeVisible(button);
        groupbox->setVisible(false);
        groupbox->getProperties().set("groupLine", var(1));
        groupbox->setColour(GroupComponent::textColourId, Colour::fromString(fontcolour));
        groupbox->setColour(TextButton::buttonColourId, cUtils::getComponentSkin());

        button->setButtonText(buttonText);



        button->getProperties().set("cornersize", corners);

        if(caption.length()>0)
        {
            offX=10;
            offY=20;
            offWidth=-20;
            offHeight=-30;
            groupbox->setVisible(true);
            groupbox->setText(caption);
        }

        if(cAttr.getStringProp(CabbageIDs::popuptext).isNotEmpty())
        {
            tooltipText = cAttr.getStringProp(CabbageIDs::popuptext);
            button->setTooltip(tooltipText);
        }

        button->getProperties().set("isRect", isRect);

        if(cAttr.getNumProp(CabbageIDs::radiogroup)!=0)
            button->setRadioGroupId(cAttr.getNumProp(CabbageIDs::radiogroup));

        button->setColour(ToggleButton::textColourId, Colour::fromString(fontcolour));
        button->setColour(TextButton::buttonOnColourId, Colour::fromString(oncolour));
        button->setColour(TextButton::buttonColourId, Colour::fromString(colour));
        button->setButtonText(buttonText);
        setAlpha(cAttr.getNumProp(CabbageIDs::alpha));

        //set initial value if given
        if(cAttr.getNumProp(CabbageIDs::value)==1)
            button->setToggleState(true, dontSendNotification);
        else
            button->setToggleState(false, dontSendNotification);

        this->setWantsKeyboardFocus(false);
    }
    //---------------------------------------------
    ~CabbageCheckbox()
    {

    }

    //update controls
    void update(CabbageGUIType m_cAttr)
    {
        const MessageManagerLock mmLock;
        button->setColour(ToggleButton::textColourId, Colour::fromString(m_cAttr.getStringProp(CabbageIDs::fontcolour)));
        button->setColour(TextButton::buttonColourId, Colour::fromString(m_cAttr.getStringProp(CabbageIDs::colour)));
        setBounds(m_cAttr.getBounds());
        button->getProperties().set("isRect", m_cAttr.getStringProp(CabbageIDs::shape).equalsIgnoreCase("square"));
        button->setButtonText(m_cAttr.getStringProp(CabbageIDs::text));
        setAlpha(m_cAttr.getNumProp(CabbageIDs::alpha));
        if(rotate!=m_cAttr.getNumProp(CabbageIDs::rotate))
        {
            rotate = m_cAttr.getNumProp(CabbageIDs::rotate);
            setTransform(AffineTransform::rotation(rotate, getX()+m_cAttr.getNumProp(CabbageIDs::pivotx), getY()+m_cAttr.getNumProp(CabbageIDs::pivoty)));
        }
        if(!m_cAttr.getNumProp(CabbageIDs::visible))
        {
            setVisible(false);
            setEnabled(false);
        }
        else
        {
            setVisible(true);
            setEnabled(true);
        }
        if(!m_cAttr.getNumProp(CabbageIDs::active))
        {
            setEnabled(false);
        }
        else
        {
            setEnabled(true);
        }
        if(tooltipText!=m_cAttr.getStringProp(CabbageIDs::popuptext))
        {
            tooltipText = m_cAttr.getStringProp(CabbageIDs::popuptext);
            button->setTooltip(tooltipText);
        }
        repaint();
    }

    //---------------------------------------------
    void resized()
    {
        groupbox->setBounds(0, 0, getWidth(), getHeight());
        button->setBounds(offX, offY, getWidth()+offWidth, getHeight()+offHeight);
        if(rotate!=0)
            setTransform(AffineTransform::rotation(rotate, getX()+pivotx, pivoty+getY()));
        this->setWantsKeyboardFocus(false);
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageCheckbox);
};


//==============================================================================
// custom checkbox component with optional surrounding groupbox
//==============================================================================
class CabbagePopupMenu : public Component
{
    //ScopedPointer<LookAndFeel> lookFeel;
    int offX, offY, offWidth, offHeight, pivotx, pivoty;
    String name, caption, text, colour, fontcolour;
    StringArray items;
public:
    //---- constructor -----
    CabbagePopupMenu(CabbageGUIType &cAttr):
        name(cAttr.getStringProp(CabbageIDs::name)),
        caption(cAttr.getStringProp(CabbageIDs::caption)),
        colour(cAttr.getStringProp(CabbageIDs::colour)),
        fontcolour(cAttr.getStringProp(CabbageIDs::fontcolour))
    {
        //populate combo with files
        Array<File> dirFiles;
        if(cAttr.getStringProp("fileType").length()<1)
            for(int i=0; i<cAttr.getStringArrayProp("text").size(); i++)
            {
                String item  = cAttr.getStringArrayPropValue("text", i);
                items.add(item);
            }
        else
        {
            //appProperties->getUserSettings()->getValue("CsoundPluginDirectory");
            File pluginDir;

            const String filetype = cAttr.getStringProp("filetype");

            pluginDir.findChildFiles(dirFiles, 2, false, filetype);

            for (int i = 0; i < dirFiles.size(); ++i)
            {
                //m.addItem (i + menuSize, cabbageFiles[i].getFileNameWithoutExtension());
                //String test  = String(i+1)+": "+dirFiles[i].getFileName();
                String filename;
                if(filetype.contains("snaps"))
                    filename = dirFiles[i].getFileNameWithoutExtension();
                else
                    filename = dirFiles[i].getFileName();
                items.add(filename);
            }
        }

    }
    //---------------------------------------------
    ~CabbagePopupMenu()
    {

    }

    void addItemsToPopup(PopupMenu &m)
    {
        for(int i=0; i<items.size(); i++)
            m.addItem(i+1, items[i]);
    }

    //update controls
    void update(CabbageGUIType m_cAttr)
    {

    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbagePopupMenu);
};

//==============================================================================
// custom checkbox component with optional surrounding groupbox
//==============================================================================
class CabbageComboBox : public Component
{
    //ScopedPointer<LookAndFeel> lookFeel;
    int offX, offY, offWidth, offHeight, pivotx, pivoty;
    String name, tooltipText, caption, text, colour, fontcolour;
    float rotate;
public:
    ScopedPointer<GroupComponent> groupbox;
    ScopedPointer<ComboBox> combo;
    //---- constructor -----
    CabbageComboBox(CabbageGUIType &cAttr):
        name(cAttr.getStringProp(CabbageIDs::name)),
        caption(cAttr.getStringProp(CabbageIDs::caption)),
        colour(cAttr.getStringProp(CabbageIDs::colour)),
        fontcolour(cAttr.getStringProp(CabbageIDs::fontcolour)),
        rotate(cAttr.getNumProp(CabbageIDs::rotate)),
        pivotx(cAttr.getNumProp(CabbageIDs::pivotx)),
        pivoty(cAttr.getNumProp(CabbageIDs::pivoty)),
        tooltipText(String::empty)
    {
        setName(name);
        offX=offY=offWidth=offHeight=0;
        var fileNames;
        groupbox = new GroupComponent(String("groupbox_")+name);
        combo = new ComboBox(name);

        addAndMakeVisible(groupbox);
        addAndMakeVisible(combo);
        groupbox->setVisible(false);
        groupbox->getProperties().set("groupLine", var(1));
        combo->setColour(ComboBox::textColourId, Colour::fromString(fontcolour));
        combo->setColour(ComboBox::backgroundColourId, Colour::fromString(colour));
        combo->lookAndFeelChanged();
        combo->getProperties().set("svgpath", cAttr.getStringProp(CabbageIDs::svgpath));

        groupbox->setColour(GroupComponent::textColourId, Colour::fromString(fontcolour));
        groupbox->setColour(TextButton::buttonColourId, cUtils::getComponentSkin());

        if(caption.length()>0)
        {
            offX=10;
            offY=35;
            offWidth=-20;
            offHeight=-45;
            groupbox->setVisible(true);
            groupbox->setText(caption);
        }

        if(cAttr.getStringProp(CabbageIDs::popuptext).isNotEmpty())
        {
            tooltipText = cAttr.getStringProp(CabbageIDs::popuptext);
            combo->setTooltip(tooltipText);
        }

        combo->setEditableText (false);
        combo->setJustificationType (Justification::centredLeft);
        combo->setTextWhenNothingSelected(text);
        this->setWantsKeyboardFocus(false);
        setAlpha(cAttr.getNumProp(CabbageIDs::alpha));
        //populate combo with files
        Array<File> dirFiles;


        if(cAttr.getStringProp(CabbageIDs::file).isNotEmpty())
        {
            combo->clear(dontSendNotification);
            String file = File(cAttr.getStringProp(CabbageIDs::file)).loadFileAsString();
            StringArray lines = StringArray::fromLines(file);
            for (int i = 0; i < lines.size(); ++i)
            {
                combo->addItem(lines[i], i+1);
            }
        }

        else if(cAttr.getStringProp(CabbageIDs::filetype).length()<1)
        {
            combo->clear(dontSendNotification);
            for(int i=0; i<cAttr.getStringArrayProp("text").size(); i++)
            {
                String item  = cAttr.getStringArrayPropValue("text", i);
                combo->addItem(item, i+1);
            }
        }
        else
        {
            //appProperties->getUserSettings()->getValue("CsoundPluginDirectory");
            combo->clear(dontSendNotification);
            //cUtils::showMessage(cAttr.getStringProp(CabbageIDs::workingdir));
            File pluginDir(cAttr.getStringProp(CabbageIDs::workingdir));

            const String filetype = cAttr.getStringProp("filetype");

            pluginDir.findChildFiles(dirFiles, 2, false, filetype);

            for (int i = 0; i < dirFiles.size(); ++i)
            {
                //m.addItem (i + menuSize, cabbageFiles[i].getFileNameWithoutExtension());
                //String test  = String(i+1)+": "+dirFiles[i].getFileName();
                String filename;
                if(filetype.contains("snaps"))
                    filename = dirFiles[i].getFileNameWithoutExtension();
                else
                    filename = dirFiles[i].getFileName();
                combo->addItem(filename, i+1);
                fileNames.append(filename);
                //cAttr.setStringArrayPropValue(CabbageIDs::text, i, filename);
            }
        }
        //cAttr.setStringArrayProp(CabbageIDs::text, fileNames);
        combo->setSelectedItemIndex(cAttr.getNumProp(CabbageIDs::value)-1);

    }
    //---------------------------------------------
    ~CabbageComboBox()
    {

    }

    //update controls
    void update(CabbageGUIType m_cAttr)
    {
        const MessageManagerLock mmLock;
        combo->getProperties().set("colour", m_cAttr.getStringProp(CabbageIDs::colour));
        combo->getProperties().set("fontcolour", m_cAttr.getStringProp(CabbageIDs::fontcolour));
        setBounds(m_cAttr.getBounds());
        setAlpha(m_cAttr.getNumProp(CabbageIDs::alpha));
        if(rotate!=m_cAttr.getNumProp(CabbageIDs::rotate))
        {
            rotate = m_cAttr.getNumProp(CabbageIDs::rotate);
            setTransform(AffineTransform::rotation(rotate, getX()+m_cAttr.getNumProp(CabbageIDs::pivotx), getY()+m_cAttr.getNumProp(CabbageIDs::pivoty)));
        }
        if(!m_cAttr.getNumProp(CabbageIDs::visible))
        {
            setVisible(false);
            setEnabled(false);
        }
        else
        {
            setVisible(true);
            setEnabled(true);
        }
        if(!m_cAttr.getNumProp(CabbageIDs::active))
        {
            setEnabled(false);
        }
        else
        {
            setEnabled(true);
        }
        repaint();
    }


    //---------------------------------------------
    void resized()
    {
        groupbox->setBounds(0, 0, getWidth(), getHeight());
        combo->setBounds(offX, offY, getWidth()+offWidth, getHeight()+offHeight);
        if(rotate!=0)
            setTransform(AffineTransform::rotation(rotate, getX()+pivotx, pivoty+getY()));
        this->setWantsKeyboardFocus(false);
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageComboBox);
};
//==============================================================================
// encoder component
class CabbageEncoder : public Component, public ChangeBroadcaster, public Label::Listener
{
    String name, channel, outlinecolour, colour, trackercolour, text, textcolour, tooltipText;
    float rotate, sliderincr, value, sliderIncr, startingValue, sliderPos, currentValue;
    Image img;
    int top, left, width, yAxis, textBox, height, line, pivotx, pivoty, resize, corners, progress;
    Point<float> scale;
    AffineTransform transform;
    bool isMouseOver, shouldDisplayPopup;
    Rectangle<float> slider;
    Label textLabel, valueLabel;
    CabbagePluginAudioProcessorEditor* owner;

public:
    CabbageEncoder(CabbageGUIType &cAttr, CabbagePluginAudioProcessorEditor* _owner);
    void labelTextChanged (Label *label);
    void mouseDown(const MouseEvent &e);
    void mouseEnter(const MouseEvent &e);
    void mouseDrag(const MouseEvent& e);
    void mouseExit(const MouseEvent &e);
    void paint(Graphics &g);
    void showPopup();
    void resized();
    void update(CabbageGUIType m_cAttr);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageEncoder);
};
//==============================================================================
class CabbageImage : public Component,
    public ChangeBroadcaster, public TooltipClient
{
    String name, outline, colour, shape, file;
    float rotate;
    Image img;
    int top, left, width, height, line, pivotx, pivoty, resize, corners;
    String currentDirectory, tooltipText;
    Point<float> scale;
    AffineTransform transform;

public:
    CabbageImage(CabbageGUIType &cAttr):
        name(cAttr.getStringProp(CabbageIDs::name)),
        file(cAttr.getStringProp(CabbageIDs::file)),
        outline(cAttr.getStringProp(CabbageIDs::outlinecolour)),
        colour(cAttr.getStringProp(CabbageIDs::colour)),
        shape(cAttr.getStringProp("shape")),
        line(cAttr.getNumProp(CabbageIDs::outlinethickness)),
        transform(AffineTransform::identity),
        rotate(cAttr.getNumProp(CabbageIDs::rotate)),
        corners(cAttr.getNumProp(CabbageIDs::corners)),
        pivotx(cAttr.getNumProp(CabbageIDs::pivotx)),
        pivoty(cAttr.getNumProp(CabbageIDs::pivoty)),
        tooltipText(String::empty),
        leftButton(false),
        counter(0)

    {
        setName(name);

        if(file.containsIgnoreCase(".svg"))
            img = cUtils::drawFromSVG(File(file).loadFileAsString(), cUtils::getSVGWidth(File(file).loadFileAsString()), cUtils::getSVGHeight(File(file).loadFileAsString()), AffineTransform::identity);
        else
            img = ImageCache::getFromFile (File (file));
        this->setWantsKeyboardFocus(false);

        //if widget is a plant intercept mouse events
        if(cAttr.getStringProp(CabbageIDs::plant).isNotEmpty())
            this->setInterceptsMouseClicks(true, true);

        setAlpha(cAttr.getNumProp(CabbageIDs::alpha));

        if(cAttr.getStringProp(CabbageIDs::popuptext).isNotEmpty())
            tooltipText = cAttr.getStringProp(CabbageIDs::popuptext);

        resize = cAttr.getNumProp(CabbageIDs::resize);
        scale = Point<float>(cAttr.getNumProp(CabbageIDs::scalex), cAttr.getNumProp(CabbageIDs::scaley));
    }
    ~CabbageImage()
    {
    }

    String getTooltip()
    {
        return tooltipText;
    }

    void setBaseDirectory(String dir)
    {
        currentDirectory = dir;
    }

    void mouseDown(const MouseEvent& event)
    {
        if(!event.mods.isPopupMenu())
        {
            counter = (counter==0 ? 1 : 0);
            sendChangeMessage();
        }


    }

    void rescale(float x, float y)
    {
        for( int i=0; i<getNumChildComponents(); i++)
        {
            const Rectangle<int> bounds = getChildComponent(i)->getBounds();
            getChildComponent(i)->setBounds(bounds.getX()*x,
                                            bounds.getY()*y,
                                            bounds.getWidth()*x,
                                            bounds.getHeight()*y);
        }

        setSize(this->getWidth()*x, this->getHeight()*y);
        getParentComponent()->resized();
    }

    //update control
    void update(CabbageGUIType m_cAttr)
    {
        setBounds(m_cAttr.getBounds());
        colour = m_cAttr.getStringProp(CabbageIDs::colour);
        outline = m_cAttr.getStringProp(CabbageIDs::outlinecolour);
        shape = m_cAttr.getStringProp(CabbageIDs::shape);
        line = m_cAttr.getNumProp(CabbageIDs::outlinethickness);
        setAlpha(m_cAttr.getNumProp(CabbageIDs::alpha));

        if(m_cAttr.getStringProp(CabbageIDs::file)!=file)
        {
            if(!File(m_cAttr.getStringProp(CabbageIDs::file)).existsAsFile())
                file = cUtils::returnFullPathForFile(m_cAttr.getStringProp(CabbageIDs::file), currentDirectory);
            else
                file = m_cAttr.getStringProp(CabbageIDs::file);

            img = ImageCache::getFromFile (File (file));
            repaint(this->getBounds());
        }

        if(scale!=Point<float>(m_cAttr.getNumProp(CabbageIDs::scalex), m_cAttr.getNumProp(CabbageIDs::scaley)))
        {
            scale=Point<float>(m_cAttr.getNumProp(CabbageIDs::scalex), m_cAttr.getNumProp(CabbageIDs::scaley));
            rescale(scale.getX(), scale.getY());
        }

        if(rotate!=m_cAttr.getNumProp(CabbageIDs::rotate))
        {
            rotate = m_cAttr.getNumProp(CabbageIDs::rotate);
            transform = AffineTransform::rotation(rotate, getX()+m_cAttr.getNumProp(CabbageIDs::pivotx), getY()+m_cAttr.getNumProp(CabbageIDs::pivoty));
            setTransform(transform);
        }


        if(!m_cAttr.getNumProp(CabbageIDs::visible))
        {
            setVisible(false);
            setEnabled(false);
        }
        else
        {
            setEnabled(true);
            setVisible(true);
        }
        if(!m_cAttr.getNumProp(CabbageIDs::active))
        {
            setEnabled(false);
        }
        else
        {
            setEnabled(true);
        }

        if(tooltipText!=m_cAttr.getStringProp(CabbageIDs::popuptext))
        {
            tooltipText = m_cAttr.getStringProp(CabbageIDs::popuptext);
        }

        repaint();
    }

    void paint (Graphics& g)
    {
        //Logger::writeToLog("in paint routine");
        if(img.isValid())
        {
            g.drawImage(img, 0, 0, width, height, 0, 0, img.getWidth(), img.getHeight());
        }
        else
        {
            if(shape=="rounded")
            {
                g.fillAll(Colours::transparentBlack);
                g.setColour(Colour::fromString(outline));
                g.drawRoundedRectangle(0,0, width, height, corners, line);
                g.setColour(Colour::fromString(colour));
                g.fillRoundedRectangle(line,line, width-(line*2), height-(line*2), corners);
            }
            if(shape=="ellipse")
            {
                g.setColour(Colour::fromString(outline));
                g.drawEllipse(0,0, width, height, line);
                g.setColour(Colour::fromString(colour));
                g.fillEllipse(line,line, width-(line*2), height-(line*2));
            }
            if(shape=="sharp")
            {
                g.fillAll(Colours::transparentBlack);
                g.setColour(Colour::fromString(outline));
                g.drawRect(0,0, width, height, line);
                g.setColour(Colour::fromString(colour));
                g.fillRect(line,line, width-(line*2), height-(line*2));
            }
        }

    }

    void resized()
    {
        top = getY();
        left = getX();
        width = getWidth();
        height = getHeight();
        this->setWantsKeyboardFocus(false);
        if(rotate!=0)
            setTransform(AffineTransform::rotation(rotate, getX()+pivotx, pivoty+getY()));

    }

    bool leftButton;
    int counter;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageImage);
};

//==============================================================================
// custom groupbox component, this can act as a plant for other components
//==============================================================================
class CabbageGroupbox : public GroupComponent,
    public ChangeBroadcaster, public TooltipClient
{
    int offX, offY, offWidth, offHeight, pivotx, pivoty, left, top, corners, svgDebug;
    String name, caption, text, colour, fontcolour, tooltipText;
    float rotate;
    int line;
    Point<float> scale;
    File svgPath, svgFile;
public:
    //---- constructor -----
    CabbageGroupbox(CabbageGUIType &cAttr):
        name(cAttr.getStringProp(CabbageIDs::name)),
        caption(cAttr.getStringProp(CabbageIDs::caption)),
        text(cAttr.getStringProp(CabbageIDs::text)),
        colour(cAttr.getStringProp(CabbageIDs::colour)),
        left(cAttr.getNumProp(CabbageIDs::left)),
        top(cAttr.getNumProp(CabbageIDs::top)),
        fontcolour(cAttr.getStringProp(CabbageIDs::fontcolour)),
        GroupComponent(cAttr.getStringProp(CabbageIDs::name)),
        line(cAttr.getNumProp(CabbageIDs::linethickness)),
        rotate(cAttr.getNumProp(CabbageIDs::rotate)),
        pivotx(cAttr.getNumProp(CabbageIDs::pivotx)),
        pivoty(cAttr.getNumProp(CabbageIDs::pivoty)),
        tooltipText(String::empty),
        corners(cAttr.getNumProp(CabbageIDs::corners)),
        svgDebug(cAttr.getNumProp(CabbageIDs::svgdebug))
    {
        toBack();
        offX=offY=offWidth=offHeight=0;
        setColour(TextButton::buttonColourId, Colour::fromString(colour));
        setName(cAttr.getStringProp(CabbageIDs::name));
        setColour(GroupComponent::textColourId, Colour::fromString(fontcolour));

        svgFile = File(cUtils::returnFullPathForFile(cAttr.getStringProp(CabbageIDs::svggroupbox),
                       cAttr.getStringProp(CabbageIDs::parentdir)));

        svgPath = File(cAttr.getStringProp(CabbageIDs::svgpath));
        cUtils::setSVGProperties(*this, svgFile, svgPath, "groupbox");

        setAlpha(cAttr.getNumProp(CabbageIDs::alpha));
        this->setText(text);
        this->setWantsKeyboardFocus(false);
        if(line==0)
            this->getProperties().set("groupLine", var(0));
        else
            this->getProperties().set("groupLine", line);
        //repaint();

        //if widget is a plant intercept mouse events
        if(cAttr.getStringProp(CabbageIDs::plant).isNotEmpty())
            this->setInterceptsMouseClicks(true, true);
        this->toFront(true);

        this->getProperties().set("cornersize", corners);
        cUtils::debug(corners);

        if(rotate!=0)
            setTransform(AffineTransform::rotation(rotate, left+pivotx, pivoty+top));

        if(cAttr.getStringProp(CabbageIDs::popuptext).isNotEmpty())
            tooltipText = cAttr.getStringProp(CabbageIDs::popuptext);
        scale = Point<float>(cAttr.getNumProp(CabbageIDs::scalex), cAttr.getNumProp(CabbageIDs::scaley));
    }
    //---------------------------------------------
    ~CabbageGroupbox()
    {

    }

    String getTooltip()
    {
        return tooltipText;
    }

    void mouseDown(const MouseEvent& event)
    {
        //Logger::writeToLog("mouse down in groupbox");
        if(event.mods.isPopupMenu())
            sendChangeMessage();
    }

    //rescale plant
    void rescale(float x, float y)
    {
        for( int i=0; i<getNumChildComponents(); i++)
        {
            Rectangle<int> bounds = getChildComponent(i)->getBounds();
            getChildComponent(i)->setBounds(bounds.getX()*x,
                                            bounds.getY()*y,
                                            bounds.getWidth()*x,
                                            bounds.getHeight()*y);
        }

        setSize(this->getWidth()*x, this->getHeight()*y);
        getParentComponent()->resized();
    }

    //update control
    void update(CabbageGUIType m_cAttr)
    {

        if(scale!=Point<float>(m_cAttr.getNumProp(CabbageIDs::scalex), m_cAttr.getNumProp(CabbageIDs::scaley)))
        {
            scale=Point<float>(m_cAttr.getNumProp(CabbageIDs::scalex), m_cAttr.getNumProp(CabbageIDs::scaley));
            rescale(scale.getX(), scale.getY());
        }

        if(!m_cAttr.getNumProp(CabbageIDs::visible))
        {
            setVisible(false);
            setEnabled(false);
        }
        else
        {
            setEnabled(true);
            setVisible(true);
        }
        if(!m_cAttr.getNumProp(CabbageIDs::active))
        {
            setEnabled(false);
        }
        else
        {
            setEnabled(true);
        }
        if(isEnabled())
        {
            setColour(TextButton::buttonColourId, Colour::fromString(m_cAttr.getStringProp(CabbageIDs::colour)));
            setColour(GroupComponent::textColourId, Colour::fromString(m_cAttr.getStringProp(CabbageIDs::fontcolour)));
            setBounds(m_cAttr.getBounds());
            setText(m_cAttr.getStringProp(CabbageIDs::text));
            setAlpha(m_cAttr.getNumProp(CabbageIDs::alpha));

            if(rotate!=m_cAttr.getNumProp(CabbageIDs::rotate))
            {
                rotate = m_cAttr.getNumProp(CabbageIDs::rotate);
                setTransform(AffineTransform::rotation(rotate, getX()+m_cAttr.getNumProp(CabbageIDs::pivotx), getY()+m_cAttr.getNumProp(CabbageIDs::pivoty)));
            }

            repaint();

        }

        if(tooltipText!=m_cAttr.getStringProp(CabbageIDs::popuptext))
        {
            tooltipText = m_cAttr.getStringProp(CabbageIDs::popuptext);
        }

        if(m_cAttr.getNumProp(CabbageIDs::svgdebug)!=svgDebug)
        {
            cUtils::setSVGProperties(*this, svgFile, svgPath, "groupbox");
            svgDebug = m_cAttr.getNumProp(CabbageIDs::svgdebug);
        }
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageGroupbox);
};

//==============================================================================
// custom directorylist
//==============================================================================
//class CabbageDirectoryList	:	public Component
//{
//public:
//    ScopedPointer<DirectoryContentsComponent> directoryList;
//    String name, channel, workingDir, fileType;
////---- constructor -----
//public:
//    CabbageDirectoryList (String _name, String _channel, String _workingDir, String _fileType):
//        name(_name),
//        channel(_channel),
//        workingDir(_workingDir),
//        fileType(_fileType)
//    {
//        setName(name);
//        directoryList = new DirectoryContentsComponent(workingDir, fileType);
//        addAndMakeVisible(directoryList);
//    }
//
//    ~CabbageDirectoryList() {}
//
//    void resized()
//    {
//        directoryList->setBounds(0, 0, getWidth(), getHeight());
//    }
//
//    void paint(Graphics& g) {}
//    const StringArray getListContents()
//    {
//        StringArray test = directoryList->getFunctionTables();
//        //Logger::writeToLog(test.joinIntoString("\n"));
//
//        return directoryList->getFunctionTables();
//    }
//
//private:
//    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageDirectoryList);
//};

//==============================================================================
// custom CabbageLine
//==============================================================================
class CabbageLine	:	public Component
{
    Colour col;
public:
    CabbageLine (bool isHorizontal, String colour)
        : isHorizontal(isHorizontal)
    {
        col = Colour::fromString(colour);
    }

    ~CabbageLine()
    {
    }

    void resized()
    {
        if (isHorizontal == true)
            this->setBounds(getX(), getY(), getWidth(), getHeight());
        else
            this->setBounds(getX(), getY(), getWidth(), getHeight());

        this->setAlpha(0.7);

    }

    void paint(Graphics& g)
    {
        g.setColour (col);
        g.fillRoundedRectangle(0, 0, getWidth(), getHeight(), 1);

        g.setColour (cUtils::getBackgroundSkin());
        g.fillRoundedRectangle (0, 0, getWidth()-1, getHeight()-1, 1);
    }


private:
    bool isHorizontal;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageLine);
};

//==============================================================================
// custom table widget
//==============================================================================
class CabbageGenTable	:	public Component,
    public ChangeBroadcaster,
    public ChangeListener, public TooltipClient
{
    String colour;
    String fontcolour;
    String file, tooltipText;
    var ampranges;
    float zoom, rotate;
    float startpos, endpos;
    double sampleRate;
    int pivotx, pivoty;
    double scrubberPos;
    Array<float> ampRanges;
//---- constructor -----
public:
    CabbageGenTable (CabbageGUIType &cAttr) : colour(cAttr.getStringProp(CabbageIDs::colour)),
        fontcolour(cAttr.getStringProp(CabbageIDs::fontcolour)),
        file(cAttr.getStringProp(CabbageIDs::file)),
        zoom(cAttr.getNumProp(CabbageIDs::zoom)),
        scrubberPos(cAttr.getNumProp(CabbageIDs::scrubberposition)),
        rotate(cAttr.getNumProp(CabbageIDs::rotate)),
        pivotx(cAttr.getNumProp(CabbageIDs::pivotx)),
        pivoty(cAttr.getNumProp(CabbageIDs::pivoty)),
        startpos(-1),
        endpos(-1),
        tooltipText(String::empty)
    {
        setName(cAttr.getStringProp(CabbageIDs::name));
        table = new TableManager();
        addAndMakeVisible(table);
        //table->addChangeListener(this);
        sampleRate = 44100;
        setAlpha(cAttr.getNumProp(CabbageIDs::alpha));

        if(File(file).existsAsFile())
            setFile(file);

        if(cAttr.getStringProp(CabbageIDs::popuptext).isNotEmpty())
            tooltipText = cAttr.getStringProp(CabbageIDs::popuptext);
    }

    ~CabbageGenTable()
    {
    }

    String getTooltip()
    {
        return tooltipText;
    }

    void resized()
    {
        table->setBounds(0, 0, getWidth(), getHeight());
        if(rotate!=0)
            setTransform(AffineTransform::rotation(rotate, getX()+pivotx, getY()+pivoty));
    }


    //update control
    void update(CabbageGUIType m_cAttr)
    {
        //setBounds(m_cAttr.getBounds());
        //Logger::writeToLog("ScrubberPos:"+String(m_cAttr.getNumProp(CabbageIDs::scrubberposition)));
        if(scrubberPos!=m_cAttr.getNumProp(CabbageIDs::scrubberposition))
        {
            //var scrubberPos = m_cAttr.getVarArrayProp(CabbageIDs::scrubberposition);
            if(m_cAttr.getVarArrayProp(CabbageIDs::scrubberposition).size()>1)
            {
                scrubberPos = m_cAttr.getVarArrayProp(CabbageIDs::scrubberposition)[0];
                int tableNumber = m_cAttr.getVarArrayProp(CabbageIDs::scrubberposition)[1];
                table->setScrubberPos(scrubberPos, tableNumber);
            }
        }

        if(!m_cAttr.getNumProp(CabbageIDs::visible))
        {
            setEnabled(false);
            setVisible(false);
        }
        else
        {
            setEnabled(true);
            setVisible(true);
        }
        if(!m_cAttr.getNumProp(CabbageIDs::active))
        {
            table->toggleEditMode(false);
        }
        else
        {
            table->toggleEditMode(true);
        }

        if(ampRanges!=m_cAttr.getFloatArrayProp("amprange"))
        {
            ampRanges = m_cAttr.getFloatArrayProp("amprange");
            table->setAmpRanges(ampRanges);
            if(ampRanges.size()>2)
                table->enableEditMode(StringArray(""), ampRanges[2]);
        }

        if(m_cAttr.getNumProp(CabbageIDs::startpos)!=startpos ||  m_cAttr.getNumProp(CabbageIDs::endpos)!=endpos)
        {
            table->setRange(m_cAttr.getNumProp(CabbageIDs::startpos), m_cAttr.getNumProp(CabbageIDs::endpos));
            endpos = m_cAttr.getNumProp(CabbageIDs::endpos);
            startpos = m_cAttr.getNumProp(CabbageIDs::startpos);
        }

        if(tooltipText!=m_cAttr.getStringProp(CabbageIDs::popuptext))
        {
            tooltipText = m_cAttr.getStringProp(CabbageIDs::popuptext);
        }

        if(zoom!=m_cAttr.getNumProp(CabbageIDs::zoom))
        {
            zoom = m_cAttr.getNumProp(CabbageIDs::zoom);
            //table->setZoomFactor(zoom);
        }

    }

    void setFile(String newFile)
    {
        // table->setFile(File(newFile));
    }

    int setWaveform(AudioSampleBuffer buffer, int ftnumber)
    {
        table->setWaveform(buffer, ftnumber);
    }

    int getPosition()
    {
        //return table->getCurrentPlayPosInSamples();
    }

    int getLoopLength()
    {
        //return table->getLoopLengthInSamples();
    }


    ScopedPointer<TableManager> table;
private:
    double scrubberPosition;

    void changeListenerCallback(ChangeBroadcaster *source)
    {
        //send change message when users interact with waveform

        sendChangeMessage();
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageGenTable);
};


//==============================================================================
// custom soundfiler
//==============================================================================
class CabbageSoundfiler	:	public Component,
    public ChangeBroadcaster,
    public ChangeListener
{
    String colour;
    String fontcolour;
    String file;
    float zoom, rotate;
    int pivotx, pivoty;
    double sampleRate;
    float scrubberPos;
//---- constructor -----
public:
    CabbageSoundfiler (CabbageGUIType &cAttr) : colour(cAttr.getStringProp(CabbageIDs::colour)),
        fontcolour(cAttr.getStringProp(CabbageIDs::fontcolour)),
        file(cAttr.getStringProp(CabbageIDs::file)),
        zoom(cAttr.getNumProp(CabbageIDs::zoom)),
        scrubberPos(cAttr.getNumProp(CabbageIDs::scrubberposition)),
        rotate(cAttr.getNumProp(CabbageIDs::rotate)),
        pivotx(cAttr.getNumProp(CabbageIDs::pivotx)),
        pivoty(cAttr.getNumProp(CabbageIDs::pivoty))
    {
        setName(cAttr.getStringProp(CabbageIDs::name));
        soundFiler = new Soundfiler(44100, Colour::fromString(colour), Colour::fromString(fontcolour));
        addAndMakeVisible(soundFiler);
        soundFiler->addChangeListener(this);
        sampleRate = 44100;
        soundFiler->setZoomFactor(cAttr.getNumProp(CabbageIDs::zoom));
        setAlpha(cAttr.getNumProp(CabbageIDs::alpha));

        //if no channels are set remove the selectable range feature

        if(cAttr.getNumProp(CabbageIDs::scrubberposition)<0)
            soundFiler->shouldShowScrubber(false);

        if(cAttr.getStringArrayProp(CabbageIDs::channel).size()==0)
            soundFiler->setIsRangeSelectable(false);

    }

    ~CabbageSoundfiler()
    {
    }

    void resized()
    {
        soundFiler->setBounds(0, 0, getWidth(), getHeight());
        if(rotate!=0)
            setTransform(AffineTransform::rotation(rotate, getX()+pivotx, getY()+pivoty));
    }


    //update control
    void update(CabbageGUIType m_cAttr)
    {
        setBounds(m_cAttr.getBounds());
        if(scrubberPos!=m_cAttr.getNumProp(CabbageIDs::scrubberposition))
        {
            scrubberPos = m_cAttr.getNumProp(CabbageIDs::scrubberposition);
            soundFiler->setScrubberPos(scrubberPos);
        }

        if(!m_cAttr.getNumProp(CabbageIDs::visible))
        {
            setVisible(false);
            setEnabled(false);
        }
        else
        {
            setEnabled(true);
            setVisible(true);
        }
        if(!m_cAttr.getNumProp(CabbageIDs::active))
        {
            setEnabled(false);
        }
        else
        {
            setEnabled(true);
        }
        if(zoom!=m_cAttr.getNumProp(CabbageIDs::zoom))
        {
            zoom = m_cAttr.getNumProp(CabbageIDs::zoom);
            soundFiler->setZoomFactor(zoom);
        }


    }

    void setFile(String newFile)
    {
        soundFiler->setFile(File(newFile));
    }

    int setWaveform(AudioSampleBuffer buffer, int channels)
    {
        soundFiler->setWaveform(buffer, channels);
    }


    int getPosition()
    {
        return soundFiler->getCurrentPlayPosInSamples();
    }

    int getLoopLength()
    {
        return soundFiler->getLoopLengthInSamples();
    }


    ScopedPointer<Soundfiler> soundFiler;
private:
    float scrubberPosition;

    void changeListenerCallback(ChangeBroadcaster *source)
    {
        //send change message when users interact with waveform

        sendChangeMessage();
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageSoundfiler);
};


//==============================================================================
// custom CabbageKeyboard
//==============================================================================
class CabbageKeyboard	:	public Component
{
    float rotate;
    int pivotx, pivoty;
public:
    CabbageKeyboard (CabbageGUIType &cAttr, MidiKeyboardState &state)
        : rotate(cAttr.getNumProp(CabbageIDs::rotate)),
          pivotx(cAttr.getNumProp(CabbageIDs::pivotx)),
          pivoty(cAttr.getNumProp(CabbageIDs::pivoty))
    {
        addAndMakeVisible(keyboard = new MidiKeyboardComponent(state, MidiKeyboardComponent::horizontalKeyboard));
        if(!cAttr.getNumProp(CabbageIDs::visible))
        {
            setVisible(false);
            //Logger::writeToLog("visivle");
        }
        else
        {
            setVisible(true);
            //Logger::writeToLog("visivle");
        }

        setAlpha(cAttr.getNumProp(CabbageIDs::alpha));
        keyboard->setLowestVisibleKey(cAttr.getNumProp(CabbageIDs::value));

        keyboard->setOctaveForMiddleC(cAttr.getNumProp(CabbageIDs::middlec));

#ifdef AndroidBuild
        keyboard->setKeyWidth(30);
#endif
        keyboard->setScrollButtonsVisible(true);

    }

    ~CabbageKeyboard()
    {
    }

    void resized()
    {
        keyboard->setBounds(getLocalBounds());
        if(rotate!=0)
            setTransform(AffineTransform::rotation(rotate, getX()+pivotx, getY()+pivoty));
    }

    //update control
    void update(CabbageGUIType m_cAttr)
    {
        setBounds(m_cAttr.getBounds());
        setAlpha(m_cAttr.getNumProp(CabbageIDs::alpha));
        if(rotate!=m_cAttr.getNumProp(CabbageIDs::rotate))
        {
            rotate = m_cAttr.getNumProp(CabbageIDs::rotate);
            setTransform(AffineTransform::rotation(rotate, getX()+m_cAttr.getNumProp(CabbageIDs::pivotx), getY()+m_cAttr.getNumProp(CabbageIDs::pivoty)));
        }
        if(!m_cAttr.getNumProp(CabbageIDs::visible))
        {
            setVisible(false);
            setEnabled(false);
        }
        else
        {
            setEnabled(true);
            setVisible(true);
        }
        if(!m_cAttr.getNumProp(CabbageIDs::active))
        {
            setEnabled(false);
        }
        else
        {
            setEnabled(true);
        }

        repaint();
    }


private:
    String text, colour, fontcolour, align;
    ScopedPointer<MidiKeyboardComponent> keyboard;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageKeyboard);
};

//==============================================================================
// custom CabbageXYController
//==============================================================================
class CabbageXYController : public Component
{
    //ScopedPointer<LookAndFeel> lookFeel;
    int offX, offY, offWidth, offHeight;
public:
    ScopedPointer<GroupComponent> groupbox;
    ScopedPointer<XYPad> xypad;
    int XYAutoIndex;
    //---- constructor -----
    CabbageXYController(XYPadAutomation* xyAuto,
                        String name,
                        String text,
                        String caption,
                        int minX, int maxX, int minY, int maxY, int index, int dec,
                        String colour,
                        String fontcolour,
                        String textcolour,
                        float xValue,
                        float yValue)
    {
        setName(name);
        XYAutoIndex = index;
        offX=offY=offWidth=offHeight=0;
        caption="";
        groupbox = new GroupComponent(String("groupbox_")+name);
        groupbox->setWantsKeyboardFocus(false);
        xypad = new XYPad(xyAuto, text, minX, maxX, minY, maxY, dec, Colour::fromString(colour), Colour::fromString(fontcolour), Colour::fromString(textcolour),xValue, yValue);
        xypad->setWantsKeyboardFocus(false);
        addAndMakeVisible(xypad);
        addAndMakeVisible(groupbox);

        groupbox->setVisible(false);
        //outline colour ID
        groupbox->setColour(GroupComponent::textColourId, Colour::fromString(fontcolour));
        groupbox->setColour(TextButton::buttonColourId, cUtils::getComponentSkin());

        if(caption.length()>0)
        {
            offX=10;
            offY=15;
            offWidth=-20;
            offHeight=-25;
            groupbox->setVisible(true);
            groupbox->setText(caption);
        }
        this->setWantsKeyboardFocus(false);
    }
    //---------------------------------------------
    ~CabbageXYController()
    {

    }


    //update control
    void update(CabbageGUIType m_cAttr)
    {
        setBounds(m_cAttr.getBounds());
        setAlpha(m_cAttr.getNumProp(CabbageIDs::alpha));

        if(!m_cAttr.getNumProp(CabbageIDs::visible))
        {
            setVisible(false);
            setEnabled(false);
        }
        else
        {
            setEnabled(true);
            setVisible(true);
        }
        if(!m_cAttr.getNumProp(CabbageIDs::active))
        {
            setEnabled(false);
        }
        else
        {
            setEnabled(true);
        }
        //repaint();
    }

    //---------------------------------------------
    void resized()
    {
        groupbox->setBounds(0, 0, getWidth(), getHeight());
        xypad->setBounds(offX, offY, getWidth()+offWidth, getHeight()+offHeight);
        this->setWantsKeyboardFocus(false);
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageXYController);
};

//===============================================================================
//Cabbage text editor; for sending string to Csound
//===============================================================================
class CabbageTextEditor : public Component,
    public TextEditor::Listener,
    public KeyListener,
    public ChangeBroadcaster
{
    ScopedPointer<GroupComponent> groupbox;
    ScopedPointer<LookAndFeel_V1> lookAndFeel;
    String text, tooltipText, name, caption, type, currentText;
    StringArray strings;
    Colour colour, fontcolour;
    int offX, offY, offWidth, offHeight, stringIndex, pivotx, pivoty;
    float rotate;
public:
    String channel;

    class customTextEditor : public TextEditor
    {
    public:
        customTextEditor(CabbageTextEditor* _owner):TextEditor(""), owner(_owner) {}
        ~customTextEditor() {}

        void addPopupMenuItems (PopupMenu &menuToAddTo, const MouseEvent *mouseClickEvent)
        {
            menuToAddTo.addItem(1, "Cut");
            menuToAddTo.addItem(2, "Copy");
            menuToAddTo.addItem(3, "Paste");
            menuToAddTo.addItem(4, "Select All");
            menuToAddTo.addSeparator();
            menuToAddTo.addItem(5, "Send text");
        }

        void performPopupMenuAction (int menuItemID)
        {
            if(menuItemID==1)
                cutToClipboard();
            else if(menuItemID==2)
                copyToClipboard();
            else if(menuItemID==3)
                pasteFromClipboard();
            else if(menuItemID==4)
                selectAll();
            else if(menuItemID==5)
                owner->sendTextMessage();
        }

    private:
        CabbageTextEditor* owner;
    };

    ScopedPointer<customTextEditor> editor;
    //---- constructor -----
    CabbageTextEditor(CabbageGUIType &cAttr) :
        name(cAttr.getStringProp(CabbageIDs::name)),
        caption(cAttr.getStringProp(CabbageIDs::caption)),
        text(cAttr.getStringProp(CabbageIDs::text)),
        type(cAttr.getStringProp(CabbageIDs::type)),
        editor(new customTextEditor(this)),
        groupbox(new GroupComponent(String("groupbox_"))),
        rotate(cAttr.getNumProp(CabbageIDs::rotate)),
        pivotx(cAttr.getNumProp(CabbageIDs::pivotx)),
        pivoty(cAttr.getNumProp(CabbageIDs::pivoty)),
        lookAndFeel(new LookAndFeel_V1()),
        fontcolour(Colour::fromString(cAttr.getStringProp(CabbageIDs::fontcolour))),
        colour(Colour::fromString(cAttr.getStringProp(CabbageIDs::colour))),
        channel(cAttr.getStringProp(CabbageIDs::channel)),
        offX(0),
        offY(0),
        offWidth(0),
        offHeight(0),
        stringIndex(0),
        tooltipText(String::empty)
    {
        editor->setLookAndFeel(lookAndFeel);
        addAndMakeVisible(editor);
        editor->setMultiLine(false);
        editor->setScrollbarsShown(true);
        editor->setReadOnly(false);
        //background colour ID
        editor->setColour(0x1000200, colour);
        //text colour ID
        editor->setColour(0x1000201, fontcolour);
        editor->setColour(Label::outlineColourId, Colours::white);
        editor->addListener(this);
        editor->addKeyListener(this);
        editor->setText(text, false);
        if(cAttr.getStringProp(CabbageIDs::popuptext).isNotEmpty())
        {
            editor->setTooltip(cAttr.getStringProp(CabbageIDs::popuptext));
            tooltipText = cAttr.getStringProp(CabbageIDs::popuptext);
        }
        //groupbox->setColour(GroupComponent::ColourIds::outlineColourId, Colours::red);
        this->setWantsKeyboardFocus(false);
        setAlpha(cAttr.getNumProp(CabbageIDs::alpha));
    }

    //---------------------------------------------
    ~CabbageTextEditor() {}

    //update control
    void update(CabbageGUIType m_cAttr)
    {
        editor->setColour(0x1000200, Colour::fromString(m_cAttr.getStringProp(CabbageIDs::colour)));
        editor->setColour(0x1000201, Colour::fromString(m_cAttr.getStringProp(CabbageIDs::fontcolour)));
        setBounds(m_cAttr.getBounds());
        if(!m_cAttr.getNumProp(CabbageIDs::visible))
        {
            setVisible(false);
            setEnabled(false);
        }
        else
        {
            setEnabled(true);
            setVisible(true);
        }
        if(!m_cAttr.getNumProp(CabbageIDs::active))
        {
            setEnabled(false);
        }
        else
        {
            setEnabled(true);
        }
        if(text!=m_cAttr.getStringProp(CabbageIDs::text))
        {
            editor->setText(m_cAttr.getStringProp(CabbageIDs::text));
            text = m_cAttr.getStringProp(CabbageIDs::text);
        }
        if(rotate!=m_cAttr.getNumProp(CabbageIDs::rotate))
        {
            rotate = m_cAttr.getNumProp(CabbageIDs::rotate);
            setTransform(AffineTransform::rotation(rotate, getX()+m_cAttr.getNumProp(CabbageIDs::pivotx), getY()+m_cAttr.getNumProp(CabbageIDs::pivoty)));
        }
        if(tooltipText!=m_cAttr.getStringProp(CabbageIDs::popuptext))
        {
            tooltipText = m_cAttr.getStringProp(CabbageIDs::popuptext);
            editor->setTooltip(tooltipText);
        }
        setAlpha(m_cAttr.getNumProp(CabbageIDs::alpha));
        repaint();
    }

    void paint(Graphics &g)
    {
        g.fillAll(colour);
    }

    //---------------------------------------------
    void resized()
    {
        editor->setBounds(0, 0, getWidth(), getHeight());
        if(rotate!=0)
            setTransform(AffineTransform::rotation(rotate, getX()+pivotx, getY()+pivoty));
        this->setWantsKeyboardFocus(false);
    }

    void textEditorReturnKeyPressed (TextEditor&)
    {
        sendTextMessage();
    }

    void sendTextMessage()
    {
        //cUtils::showMessage(editor->getText());
        strings.add(editor->getText());
        currentText = editor->getText();
        strings.removeDuplicates(false);
        stringIndex = strings.size()-1;
        //editor->setText("", false);
        sendChangeMessage();
    }

    String getCurrentText()
    {
        return currentText;
    }

    bool keyPressed(const juce::KeyPress &key,Component *)
    {
        //Logger::writeToLog(String(key.getKeyCode()));
        if (key.getTextDescription().contains("cursor up"))
        {
            editor->setText(strings[jmax(0, stringIndex--)]);
            if(stringIndex<1)
                stringIndex=0;
        }
        else if (key.getTextDescription().contains("cursor down"))
        {
            editor->setText(strings[jmin(strings.size()-1, stringIndex++)]);
            if(stringIndex>strings.size()-1)
                stringIndex=strings.size()-1;
        }
        return false;
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageTextEditor);
};

//==============================================================================
// custom Csound message console
//==============================================================================
class CabbageTextbox : public Component
{
    ScopedPointer<GroupComponent> groupbox;
    ScopedPointer<LookAndFeel_V1> lookAndFeel;
    String text, name, tooltipText, caption, type;
    Colour colour, fontcolour;
    int offX, offY, offWidth, offHeight, pivotx, pivoty;
    float rotate;
public:
    ScopedPointer<TextEditor> editor;
    //---- constructor -----
    CabbageTextbox(CabbageGUIType &cAttr) :
        name(cAttr.getStringProp(CabbageIDs::name)),
        caption(cAttr.getStringProp(CabbageIDs::caption)),
        text(cAttr.getStringProp(CabbageIDs::text)),
        type(cAttr.getStringProp(CabbageIDs::type)),
        editor(new TextEditor(String("editor_"))),
        rotate(cAttr.getNumProp(CabbageIDs::rotate)),
        pivotx(cAttr.getNumProp(CabbageIDs::pivotx)),
        pivoty(cAttr.getNumProp(CabbageIDs::pivoty)),
        groupbox(new GroupComponent(String("groupbox_"))),
        lookAndFeel(new LookAndFeel_V1()),
        fontcolour(Colour::fromString(cAttr.getStringProp(CabbageIDs::fontcolour))),
        colour(Colour::fromString(cAttr.getStringProp(CabbageIDs::colour))),
        offX(0),
        offY(0),
        offWidth(0),
        offHeight(0),
        tooltipText(String::empty)
    {
        editor->setLookAndFeel(lookAndFeel);
        addAndMakeVisible(editor);
        if(cAttr.getNumProp(CabbageIDs::wrap)==0)
            editor->setMultiLine(true, false);
        else
            editor->setMultiLine(true);

        editor->setScrollbarsShown(true);
        editor->setReturnKeyStartsNewLine(true);
        editor->setReadOnly(true);

        if(cAttr.getStringProp(CabbageIDs::popuptext).isNotEmpty())
        {
            editor->setTooltip(cAttr.getStringProp(CabbageIDs::popuptext));
            tooltipText = cAttr.getStringProp(CabbageIDs::popuptext);
        }

        //background colour ID
        editor->setColour(0x1000200, colour);
        //text colour ID
        editor->setColour(0x1000201, fontcolour);
        setAlpha(cAttr.getNumProp(CabbageIDs::alpha));
        if(type==CabbageIDs::textbox.toString())
        {
            editor->setColour(Label::outlineColourId, Colours::white);
            setFile(cAttr.getStringProp(CabbageIDs::file));
        }

        if(caption.length()>0)
        {
            offX=10;
            offY=15;
            offWidth=-20;
            offHeight=-25;
            groupbox->setVisible(true);
            groupbox->setText(caption);
        }

        //groupbox->setColour(GroupComponent::ColourIds::outlineColourId, Colours::red);
        this->setWantsKeyboardFocus(false);
    }

    //---------------------------------------------
    ~CabbageTextbox() {}

    void setFile(String filename)
    {
        if(File(filename).exists())
            editor->setText(File(filename).loadFileAsString(), false);
        else
            editor->setText("Could not open file: "+String(filename));
    }

    //update control
    void update(CabbageGUIType m_cAttr)
    {
        editor->setColour(0x1000200, Colour::fromString(m_cAttr.getStringProp(CabbageIDs::colour)));
        editor->setColour(0x1000201, Colour::fromString(m_cAttr.getStringProp(CabbageIDs::fontcolour)));
        setBounds(m_cAttr.getBounds());
        if(type==CabbageIDs::textbox.toString())
            setFile(m_cAttr.getStringProp(CabbageIDs::file));
        if(!m_cAttr.getNumProp(CabbageIDs::visible))
        {
            setVisible(false);
            setEnabled(false);
        }
        else
        {
            setEnabled(true);
            setVisible(true);
        }
        if(!m_cAttr.getNumProp(CabbageIDs::active))
        {
            setEnabled(false);
        }
        else
        {
            setEnabled(true);
        }
        setAlpha(m_cAttr.getNumProp(CabbageIDs::alpha));
        if(rotate!=m_cAttr.getNumProp(CabbageIDs::rotate))
        {
            rotate = m_cAttr.getNumProp(CabbageIDs::rotate);
            setTransform(AffineTransform::rotation(rotate, getX()+m_cAttr.getNumProp(CabbageIDs::pivotx), getY()+m_cAttr.getNumProp(CabbageIDs::pivoty)));
        }
        if(tooltipText!=m_cAttr.getStringProp(CabbageIDs::popuptext))
        {
            tooltipText = m_cAttr.getStringProp(CabbageIDs::popuptext);
            editor->setTooltip(tooltipText);
        }
        repaint();
    }

    void paint(Graphics &g)
    {
        //----- For drawing the border
        if(type==CabbageIDs::csoundoutput)
        {
            g.setColour(cUtils::getComponentSkin());
            g.fillRoundedRectangle (0, -3, getWidth(), getHeight(), 5.f);
            //g.setColour(Colours::black);
            //g.fillRoundedRectangle(5, getHeight()-35, getWidth()-10, 20, 5.f);

            //----- For drawing the title
            g.setColour (Colours::whitesmoke);
            g.setOpacity (0.8);
            g.setFont (15);
            Justification just(1);
            g.drawText(text, 10, -5, getWidth()-20, 35, just, false);
        }
        else
        {
            g.fillAll(colour);
        }
    }

    //---------------------------------------------
    void resized()
    {
        groupbox->setBounds(0, 0, getWidth(), getHeight());
        if(type==CabbageIDs::textbox.toString())
            editor->setBounds(0, 0, getWidth(), getHeight());
        else
            editor->setBounds(offX+5, offY+20, (getWidth()+offWidth-10), (getHeight()+offHeight)-30);
        this->setWantsKeyboardFocus(false);
        if(rotate!=0)
            setTransform(AffineTransform::rotation(rotate, getX()+pivotx, getY()+pivoty));
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageTextbox);
};


//==============================================================================
// custom CabbageTable, deprecated....
//==============================================================================
class CabbageTable : public Component
{
    //ScopedPointer<LookAndFeel> lookFeel;
    int offX, offY, offWidth, offHeight, tableSize;
    StringArray tableColours, channels;
    Array<int> tableSizes, tableNumbers, drawingModes, resizingModes;
    Array< Point<float> > minMax;
    float alpha;
public:
    bool readOnly, stackMode;
    ScopedPointer<GroupComponent> groupbox;
    ScopedPointer<CabbageTableManager> table;
    ScopedPointer<TextButton> button;
    ScopedPointer<ChangeListener> listener;
    //---- constructor -----
    CabbageTable(String name, String text, String caption, StringArray chans,
                 Array<int> tblNumbers,
                 Array<int> tblSize,
                 Array<int> drawingModes,
                 Array<int> resizingModes,
                 Array<float> ampRanges,
                 StringArray colours,
                 bool readOnly,
                 bool stackMode,
                 ChangeListener* listen)
        : tableSizes(tblSize),
          tableColours(colours),
          channels(chans),
          readOnly(readOnly),
          listener(listen),
          tableNumbers(tblNumbers),
          drawingModes(drawingModes),
          resizingModes(resizingModes),
          stackMode(stackMode)
    {
        setName(name);

        for(int i=0; i<ampRanges.size(); i+=2)
        {
            minMax.add(Point<float>(ampRanges[i], ampRanges[i+1]));
        }


        offX=offY=offWidth=offHeight=0;

        groupbox = new GroupComponent(String("groupbox_")+name);
        groupbox->setWantsKeyboardFocus(false);
        tableSize = tableSizes[0];
        table = new CabbageTableManager(tableSize);

        // set up drawing modes for each table
        // 0: default mode, draws everything normally
        // 1: drawHoriztonalSegments
        // 2: make fixed point envelope
        // 3: fixed points and horizontal
        // 4: fixed points and horizontal and toggle max/min, editable from the onst
        // ....varous fill variations...

        for(int i=drawingModes.size(); i<tableSizes.size(); i++)
            drawingModes.add(0);

        addAndMakeVisible(table);
        addAndMakeVisible(groupbox);

        groupbox->setVisible(false);
        //outline colour ID
        groupbox->setColour(GroupComponent::textColourId, Colours::white);
        groupbox->setColour(TextButton::buttonColourId, cUtils::getComponentSkin());

        if(caption.length()>0)
        {
            offX=10;
            offY=15;
            offWidth=-20;
            offHeight=-25;
            groupbox->setVisible(true);
            groupbox->setText(caption);
        }
        this->setWantsKeyboardFocus(false);

    }


    void setDrawingModeBooleans(bool &fixedEnv, bool &drawHorizontal, bool &toggleMaxMin, bool &drawOriginal, bool &drawFill, int mode)
    {
        if(mode==1)
        {
            //horizontal draw mode, no fill
            drawHorizontal = true;
            fixedEnv = false;
            toggleMaxMin= false;
            drawOriginal = true;
            drawFill = false;
        }
        else if(mode==2)
        {
            //normal but fixed points
            drawHorizontal = false;
            fixedEnv = true;
            toggleMaxMin= false;
            drawOriginal = true;
            drawFill = false;
        }
        else if(mode==3)
        {
            //horizontal but with fixed points
            drawHorizontal = true;
            fixedEnv = true;
            toggleMaxMin= false;
            drawOriginal = true;
            drawFill = false;
        }
        else if(mode==4)
        {
            //horizontal with fill
            drawHorizontal = true;
            fixedEnv = true;
            toggleMaxMin= false;
            drawOriginal = true;
            drawFill = true;
        }
        else if(mode==5)
        {
            //on/off horizontal with fill
            drawHorizontal = true;
            fixedEnv = true;
            toggleMaxMin= true;
            drawOriginal = false;
            drawFill = true;
        }
        else
        {
            //normal drawing mode
            drawHorizontal = false;
            fixedEnv = false;
            toggleMaxMin= false;
            drawOriginal = true;
            drawFill = false;
        }
    }

    //---------------------------------------------
    ~CabbageTable()
    {
        listener.release();
    }

    void paint(Graphics& g)
    {
        //Logger::writeToLog("test");
    }

    // add tables to widgets
    void addTables()
    {
        bool fixed, horizontal, toggleMaxMin, 	drawOriginal, drawFill;
        if(table->getNumberOfTables()<tableNumbers.size())
            if(tableNumbers.size()>1)
                for(int i=0; i<tableNumbers.size(); i++)
                {
                    String name = "table"+String(tableNumbers[i]);
                    setDrawingModeBooleans(fixed, horizontal, toggleMaxMin, drawOriginal, drawFill, drawingModes[i]);
                    table->addTable(name, channels[i], tableNumbers[i], tableSizes[i], fixed,
                                    horizontal, drawOriginal, toggleMaxMin, drawFill,
                                    resizingModes[i], minMax[0], Colours::findColourForName(tableColours[i],
                                            Colours::white), readOnly, stackMode, tableNumbers.size(), listener);
                }
            else
            {
                setDrawingModeBooleans(fixed, horizontal, toggleMaxMin, drawOriginal, drawFill, drawingModes[0]);
                String name = "table"+String(tableNumbers[0]);
                //Logger::writeToLog(name);
                table->addTable("table0", channels[0], tableNumbers[0], tableSizes[0], fixed,
                                horizontal, drawOriginal, toggleMaxMin, drawFill,
                                resizingModes[0], minMax[0], Colours::findColourForName(tableColours[0],
                                        Colours::white), readOnly, stackMode, tableNumbers.size(), listener);
            }

    }
    //---------------------------------------------
    void fillTable(int ID, Array<float, CriticalSection> yValues)
    {
        if(yValues.size()>1)
            table->fillTable(ID, yValues);
        //table->repaint();
    }

    //---------------------------------------------
    void setTableEvtCode(int ID, StringArray fdata)
    {
        //table->setEvtCode(ID, fdata);
    }

    void setScrubberPosition(int ID, float position)
    {
        //if(position>0)
        //table->setScrubberPosition(ID, position);
    }
    //---------------------------------------------
    void resized()
    {

        groupbox->setBounds(0, 0, getWidth(), getHeight());
        table->setBounds(offX, offY, getWidth()+offWidth, getHeight()+offHeight);
        //check that we have drawn enough tables


        this->setWantsKeyboardFocus(false);

    }

    //update control
    void update(CabbageGUIType m_cAttr)
    {
        setBounds(m_cAttr.getBounds());
        if(!m_cAttr.getNumProp(CabbageIDs::visible))
        {
            setVisible(false);
            setEnabled(false);
        }
        else
        {
            setEnabled(true);
            setVisible(true);
        }
        if(!m_cAttr.getNumProp(CabbageIDs::active))
        {
            setEnabled(false);
        }
        else
        {
            setEnabled(true);
        }
        setAlpha(m_cAttr.getNumProp(CabbageIDs::alpha));
        repaint();
    }


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageTable);
};

//==============================================================================
// custom NumToggleButton
//==============================================================================
class CabbageNumberBox :  public Component
{
    String name;
    Colour colour, outlinecolour, fontcolour, textcolour;
    ScopedPointer<GroupComponent> groupbox;
    ScopedPointer<Label> label;
    int offX, offY, offWidth, offHeight, width, height, value, lastValue, decPlaces, textbox, pivotx, pivoty;
    String text, caption;
    float min, max, skew, incr;
    bool buttonState, mouseButtonState;
    float rotate;
public:
    ScopedPointer<Slider> slider;
    CabbageNumberBox(CabbageGUIType &cAttr) :
        colour(Colour::fromString(cAttr.getStringProp(CabbageIDs::colour))),
        outlinecolour(Colour::fromString(cAttr.getStringProp(CabbageIDs::outlinecolour))),
        fontcolour(Colour::fromString(cAttr.getStringProp(CabbageIDs::fontcolour))),
        textcolour(Colour::fromString(cAttr.getStringProp(CabbageIDs::textcolour))),
        text(cAttr.getStringProp(CabbageIDs::text)),
        textbox(cAttr.getNumProp(CabbageIDs::textbox)),
        decPlaces(cAttr.getNumProp(CabbageIDs::decimalplaces)),
        caption(cAttr.getStringProp(CabbageIDs::caption)),
        name(cAttr.getStringProp(CabbageIDs::name)),
        rotate(cAttr.getNumProp(CabbageIDs::rotate)),
        pivotx(cAttr.getNumProp(CabbageIDs::pivotx)),
        pivoty(cAttr.getNumProp(CabbageIDs::pivoty))

    {
        setName(name);
        offX=offY=offWidth=offHeight=0;
        groupbox = new GroupComponent(String("groupbox_")+name);
        slider = new Slider(text);
        //slider->setName(text);
        slider->toFront(true);
        label = new Label();
        label->setText(text, dontSendNotification);
        label->setJustificationType(Justification::centred);
        label->setColour(Label::textColourId, textcolour);

        addAndMakeVisible(label);
        addAndMakeVisible(slider);
        addAndMakeVisible(groupbox);
        groupbox->setVisible(false);
        groupbox->getProperties().set("groupLine", var(1));
        groupbox->setColour(GroupComponent::textColourId, fontcolour);
        groupbox->setColour(TextButton::buttonColourId, cUtils::getComponentSkin());

        slider->setSliderStyle(Slider::LinearBarVertical);
        slider->setColour(Slider::trackColourId, colour);
        slider->setColour(Slider::thumbColourId, colour);
        slider->setColour(Slider::textBoxHighlightColourId, slider->findColour(Slider::textBoxBackgroundColourId));
        slider->setColour(Slider::textBoxTextColourId, fontcolour);
        slider->setColour(Slider::textBoxBackgroundColourId, colour);


        //slider->setVelocityBasedMode(cAttr.getNumProp(CabbageIDs::velocity)==1 ? true : false);


        slider->setVelocityModeParameters(80);
        slider->getProperties().set("decimalPlaces", decPlaces);
        setAlpha(cAttr.getNumProp(CabbageIDs::alpha));

        if(caption.length()>0)
        {
            offX=10;
            offY=35;
            offWidth=-20;
            offHeight=-45;
            groupbox->setVisible(true);
            groupbox->setText(caption);
        }

        min = cAttr.getNumProp(CabbageIDs::min);
        max = cAttr.getNumProp(CabbageIDs::max);
        incr = cAttr.getNumProp(CabbageIDs::sliderincr);
        skew = cAttr.getNumProp(CabbageIDs::sliderskew);
        slider->setSkewFactor(cAttr.getNumProp(CabbageIDs::sliderskew));
        slider->setRange(cAttr.getNumProp("min"), cAttr.getNumProp("max"), cAttr.getNumProp(CabbageIDs::sliderincr));
        slider->setValue(cAttr.getNumProp(CabbageIDs::value));
    }

    ~CabbageNumberBox() {}

    void resized()
    {
        groupbox->setBounds(0, 0, getWidth(), getHeight());
        if(text.isNotEmpty())
        {
            label->setBounds(offX, offY, getWidth()+offWidth, ((getHeight()+offHeight)/2)-2);
            slider->setBounds(offX, offY+(getHeight()/2), getWidth()+offWidth, getHeight()+offHeight-(getHeight()/2));
        }
        else
            slider->setBounds(offX, offY, getWidth()+offWidth, getHeight()+offHeight);
        this->setWantsKeyboardFocus(false);
        if(rotate!=0)
            setTransform(AffineTransform::rotation(rotate, getX()+pivotx, getY()+pivoty));
    }

    //update controls
    void update(CabbageGUIType m_cAttr)
    {
        const MessageManagerLock mmLock;
        slider->setColour(Slider::rotarySliderFillColourId, Colour::fromString(m_cAttr.getStringProp(CabbageIDs::colour)));
        slider->setColour(Slider::thumbColourId, Colour::fromString(m_cAttr.getStringProp(CabbageIDs::colour)));
        slider->setColour(Slider::textBoxTextColourId, Colour::fromString(m_cAttr.getStringProp(CabbageIDs::fontcolour)));
        slider->setColour(Slider::trackColourId, Colour::fromString(m_cAttr.getStringProp(CabbageIDs::trackercolour)));
        setBounds(m_cAttr.getBounds());
        setAlpha(m_cAttr.getNumProp(CabbageIDs::alpha));
        if(rotate!=m_cAttr.getNumProp(CabbageIDs::rotate))
        {
            rotate = m_cAttr.getNumProp(CabbageIDs::rotate);
            setTransform(AffineTransform::rotation(rotate, getX()+m_cAttr.getNumProp(CabbageIDs::pivotx), getY()+m_cAttr.getNumProp(CabbageIDs::pivoty)));
        }
        slider->setName(m_cAttr.getStringProp(CabbageIDs::text));
        if(!m_cAttr.getNumProp(CabbageIDs::visible))
        {
            setVisible(false);
            setEnabled(false);
        }
        else
        {
            setEnabled(true);
            setVisible(true);
        }
        if(!m_cAttr.getNumProp(CabbageIDs::active))
        {
            setEnabled(false);
        }
        else
        {
            setEnabled(true);
        }
        repaint();
    }

};

//============================================================
// all new custom widgets class declrations should be made here
//============================================================


//============================================================
//example CabbageStepper class
//============================================================
class CabbageStepper	:	public Component, public Timer
{
    String name, channel;
    float numberOfSteps;
    int stepBPM, currentStep, isRunning;
    Colour colour;
    Array<int> stepStates;
    CabbagePluginAudioProcessorEditor* owner;
    bool isMouseDown;

public:
    CabbageStepper (CabbageGUIType &cAttr, CabbagePluginAudioProcessorEditor* _owner);
    void timerCallback();
    ~CabbageStepper()
    {
        stopTimer();
        owner = nullptr;
    }
    void resized() {}
    void mouseDown(const MouseEvent& event);
    void mouseUp(const MouseEvent& event);
    void mouseEnter(const MouseEvent& event);
    void paint(Graphics& g);
    void update(CabbageGUIType cAttr);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageStepper);
};

//==============================================================================
// custom CabbageLabel
//==============================================================================
class CabbageLabel	:	public Component
{
    float rotate, corners;
    int pivotx, pivoty, fontstyle;
    String channel;
    CabbagePluginAudioProcessorEditor* owner;
    int counter;
public:
    CabbageLabel (CabbageGUIType &cAttr, CabbagePluginAudioProcessorEditor* _owner);
    ~CabbageLabel();
    void resized();
    void paint(Graphics& g);
    void mouseDown(const MouseEvent& e);
    void setText(String _text);
    //update control
    void update(CabbageGUIType m_cAttr);

private:
    String text, colour, fontcolour, align;
    Justification textAlign;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageLabel);
};
//============================================================
//example CabbageListbox class
//============================================================
class CabbageListbox	:	public Component, ListBoxModel
{
    String name, channel;
    float rotate;
    Colour bgColour;
    String align, colour, highlightcolour, fontcolour, channelType;
    Justification justify;
    CabbagePluginAudioProcessorEditor* owner;
    StringArray items;

public:
    CabbageListbox (CabbageGUIType &cAttr, CabbagePluginAudioProcessorEditor* _owner);
    ~CabbageListbox();
    void paint (Graphics& g) override;
    void resized() override;
    int getNumRows() override;
    void listBoxItemDoubleClicked(int row, const MouseEvent &e);
    void paintListBoxItem (int rowNumber, Graphics& g,
                           int width, int height, bool rowIsSelected) override;
    void selectedRowsChanged (int /*lastRowselected*/) override;
    void update(CabbageGUIType cAttr);
    ListBox listBox;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageListbox);
};
//============================================================
// homegrown version of JUCE's two value, slider with dragable range
//============================================================
class RangeSlider	:	public Component
{


    String name, tooltipText;
    StringArray channels;
    CabbagePluginAudioProcessorEditor* owner;
    bool shouldDisplayPopup, isVertical;
    String colour, text;
    Colour trackerColour;
    double min, trackerThickness, initVal, minVal, maxVal, max, incr, skew;
    float thumbIncr;
    int thumbWidth, thumbHeight, rangeDistance, currentThumb, index;
    float width, height;


public:
    Rectangle<int> sliderThumbBounds;

    class SliderThumb : public Component
    {
        bool isVertical;
        Point<int> currentPos;
        RangeSlider* owner;
        Colour colour;

    public:
        SliderThumb(RangeSlider* _owner,String kind);
        ~SliderThumb()
        {
            owner=nullptr;
        }
        void resized()
        {}
        void paint(Graphics& g);
        void setColour(Colour _colour)
        {
            colour = _colour;
        };
    };


    RangeSlider(CabbageGUIType &cAttr, CabbagePluginAudioProcessorEditor* _owner);
    ~RangeSlider()
    {
        owner = nullptr;
    }

    enum SliderType
    {
        left,
        right,
        top,
        bottom
    };


    void setIndex(int _index)
    {
        index = _index;
    }

    void setValue(float val1, float val2);
    void resized();
    void mouseDown(const MouseEvent& event);
    void mouseUp(const MouseEvent& event);
    void mouseEnter(const MouseEvent& event);
    void mouseDrag(const MouseEvent& event);
    void mouseMove(const MouseEvent& event);
    void horizontalDrag(const MouseEvent& event);
    void verticalDrag(const MouseEvent& event);
    void paint(Graphics& g);
    void update(CabbageGUIType cAttr);
    int getSliderPosition(SliderType type);
    void showPopup();
    double getSkewedValue(double proportion);
    int getSkewedPosition(double proportion);
    void sendValuesToCsound(double val1, double val2, int thumb);

    Rectangle<int> getThumbOuterBounds(int type);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RangeSlider);

private:
    SliderThumb sliderThumb;
};
//=============================================================================
//container class for rangeslider widget
class CabbageRangeSlider2	:	public Component
{

    String name, text, textColour;
    RangeSlider slider;
    Label textLabel;
    bool isVertical;
    float initVal1, initVal2, min, max;

public:

    CabbageRangeSlider2 (CabbageGUIType &cAttr, CabbagePluginAudioProcessorEditor* _owner);

    ~CabbageRangeSlider2()
    {

    }

    void resized();

    RangeSlider& getSlider()
    {
        return slider;
    }

    void update(CabbageGUIType &cAttr) {}

};

//=============================================================================
// FFT Display Widget
//=============================================================================
class CabbageFFTDisplay	:	public Component,
    private ScrollBar::Listener,
    public ChangeListener
{
    String name, displayType;
    CabbagePluginAudioProcessorEditor* owner;
    RoundButton zoomInButton, zoomOutButton;
    Array<float, CriticalSection> points;
    int tableNumber, freq, shouldDrawSonogram, leftPos, scrollbarHeight,
        minFFTBin, maxFFTBin, size, zoomLevel, scopeWidth;
    Colour fontColour, colour, backgroundColour, outlineColour;
    ScrollBar scrollbar;
    bool isScrollbarShowing;
    float rotate;


    class FrequencyRangeDisplayComponent : public Component
    {
        int maxFreq, minFreq, resolution;
        Colour fontColour, backgroundColour;

    public:
        FrequencyRangeDisplayComponent(Colour fColour, Colour bgColour) :
            Component(),
            fontColour(fColour),
            backgroundColour(bgColour),
            minFreq(0),
            maxFreq(22050),
            resolution(10)
        {}

        ~FrequencyRangeDisplayComponent() {}

        void resized()
        {
            if(getWidth()<400)
                resolution = resolution/2.f;
        }

        void setResolution(int res)
        {
            resolution = res;
            if(getWidth()<400)
                resolution = resolution/3.f;

        }

        void paint(Graphics &g)
        {
            g.fillAll(backgroundColour);
            g.setColour(fontColour);
            for(int i=0; i<resolution; i++)
            {
                const int width = getWidth()/resolution;
                int freq = jmap(i, 0, resolution, minFreq, maxFreq);

                String freqStr = String(freq);
                if(freqStr.length()>4)
                    freqStr = freqStr.substring(0, 2) + "." + freqStr.substring(2, 3)+"kHz";
                else if(freqStr.length()>3)
                    freqStr = freqStr.substring(0, 1) + "." + freqStr.substring(1, 2)+"kHz";
                else
                    freqStr = freqStr+"Hz";

                g.drawFittedText(String(freqStr), i*width, 4, 35, 7, Justification::left, 1);
            }
        }

        void setMinMax(int min, int max)
        {
            minFreq = min;
            maxFreq = max;
        }
    };


public:

    CabbageFFTDisplay (CabbageGUIType &cAttr, CabbagePluginAudioProcessorEditor* _owner);
    ~CabbageFFTDisplay()
    {}

    void setBins(int min, int max);
    void scrollBarMoved (ScrollBar* scrollBarThatHasMoved, double newRangeStart);
    void changeListenerCallback(ChangeBroadcaster *source);
    void drawSonogram();
    void drawSpectroscope(Graphics& g);
    void paint(Graphics& g);
    void setPoints(Array<float, CriticalSection> _points);
    void resized();
    void update(CabbageGUIType m_cAttr);
    void mouseMove(const MouseEvent &e);
    void showPopup(String text);
    void showScrollbar(bool show);
    void zoomOut(int factor=1);
    void zoomIn(int factor=1);
    Image spectrogramImage, spectroscopeImage;
    FrequencyRangeDisplayComponent freqRangeDisplay;
    Range<int> freqRange;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageFFTDisplay);
};
#endif
