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
#include "CabbageTable.h"
#include "Table.h"
#include "XYPad.h"
#include "Soundfiler.h"
#include "DirectoryContentsComponent.h"


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
    int offX, offY, offWidth, offHeight;
    String buttonType;
    String name, caption, buttonText, colour, fontcolour, oncolour, onfontcolour;
public:
    ScopedPointer<GroupComponent> groupbox;
    ScopedPointer<Button> button;
    //---- constructor -----

    CabbageButton(CabbageGUIClass &cAttr) :
        name(cAttr.getStringProp(CabbageIDs::name)),
        caption(cAttr.getStringProp(CabbageIDs::caption)),
        buttonText(cAttr.getStringProp(CabbageIDs::text)),
        colour(cAttr.getStringProp(CabbageIDs::colour)),
        fontcolour(cAttr.getStringProp(CabbageIDs::fontcolour)),
        oncolour(cAttr.getStringProp(CabbageIDs::oncolour)),
        onfontcolour(cAttr.getStringProp(CabbageIDs::onfontcolour))
    {
        setName(name);
        offX=offY=offWidth=offHeight=0;
        groupbox = new GroupComponent(String("groupbox_")+name);
        button = new TextButton(name);
		button->getProperties().set("svgpath", cAttr.getStringProp(CabbageIDs::svgpath));
        addAndMakeVisible(groupbox);
        addAndMakeVisible(button);
        groupbox->setVisible(false);
        groupbox->getProperties().set("groupLine", var(1));
        Logger::writeToLog(buttonText);
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

    //update controls
    void update(CabbageGUIClass m_cAttr)
    {
        const MessageManagerLock mmLock;
        button->setColour(TextButton::textColourOnId, Colour::fromString(m_cAttr.getStringProp(CabbageIDs::fontcolour)));
        button->setColour(TextButton::buttonColourId, Colour::fromString(m_cAttr.getStringProp(CabbageIDs::colour)));
        if(!m_cAttr.getNumProp(CabbageIDs::visible))
            setVisible(false);
        else
            setVisible(true);
        setBounds(m_cAttr.getBounds());
        repaint();
    }

    //---------------------------------------------
    void resized()
    {
        groupbox->setBounds(0, 0, getWidth(), getHeight());
        button->setBounds(offX, offY, getWidth()+offWidth, getHeight()+offHeight);
        this->setWantsKeyboardFocus(false);
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageButton);
};

//==============================================================================
// custom slider component with optional surrounding groupbox
//==============================================================================
class CabbageSlider : public Component,
    public ChangeBroadcaster
{
    int offX, offY, offWidth, offHeight, plantX, plantY;
    String sliderType, compName, cl;
    int resizeCount;
    String tracker;


    //subclass slider here to expose mouse listener method
    class cSlider : public Slider
    {
    public:
        cSlider(String text, CabbageSlider* _slider):Slider(text),
            slider(_slider) {}
        ~cSlider() {};

    private:
        void mouseMove(const MouseEvent& event)
        {
            slider->sendChangeMessage();
        }


        CabbageSlider* slider;
    };


    String name, text, caption, kind, colour, fontColour, trackerFill;
    int textBox, decPlaces;
	double min, max;
    float incr, skew;
public:

    ScopedPointer<GroupComponent> groupbox;
    ScopedPointer<Slider> slider;
    //---- constructor -----
    CabbageSlider(CabbageGUIClass &cAttr) : plantX(-99), plantY(-99),
        name(cAttr.getStringProp(CabbageIDs::name)),
        caption(cAttr.getStringProp(CabbageIDs::caption)),
        colour(cAttr.getStringProp(CabbageIDs::colour)),
        fontColour(cAttr.getStringProp(CabbageIDs::fontcolour)),
        sliderType(cAttr.getStringProp(CabbageIDs::kind)),
        compName(cAttr.getStringProp(CabbageIDs::caption)),
        cl(cAttr.getStringProp(CabbageIDs::colour)),
        tracker(cAttr.getStringProp(CabbageIDs::trackercolour)),
        decPlaces(cAttr.getNumProp(CabbageIDs::decimalplaces)),
        textBox(cAttr.getNumProp(CabbageIDs::textbox)),
        text(cAttr.getStringProp(CabbageIDs::text))
    {
        setName(name);
        offX=offY=offWidth=offHeight=0;
        groupbox = new GroupComponent(String("groupbox_")+name);
        slider = new Slider(text);
		slider->getProperties().set("svgpath", cAttr.getStringProp(CabbageIDs::svgpath));
        slider->toFront(true);
        addAndMakeVisible(slider);
        addAndMakeVisible(groupbox);
        groupbox->setVisible(false);
        groupbox->getProperties().set("groupLine", var(1));
        groupbox->setColour(GroupComponent::textColourId, Colour::fromString(fontColour));
        groupbox->setColour(TextButton::buttonColourId, CabbageUtils::getComponentSkin());

        slider->setColour(Slider::rotarySliderFillColourId, Colour::fromString(cl));
        slider->setColour(Slider::thumbColourId, Colour::fromString(cl));
        slider->setColour(Slider::textBoxTextColourId, Colour::fromString(fontColour));
        slider->setColour(Slider::trackColourId, Colour::fromString(tracker));

        slider->getProperties().set("decimalPlaces", decPlaces);


        if(textBox<1)
            slider->setTextBoxStyle (Slider::NoTextBox, true, 0, 0);
        this->setWantsKeyboardFocus(false);
        resizeCount = 0;

        min = cAttr.getNumProp(CabbageIDs::min);
        max = cAttr.getNumProp(CabbageIDs::max);
		if(min==max || min>max){
			CabbageUtils::showMessage("Your min value is the same or greater than your max value.\nCabbage will now reduce your min value so that it falls into range", &getLookAndFeel());
			min = max-.1;
		}
        incr = cAttr.getNumProp(CabbageIDs::sliderincr);
        skew = cAttr.getNumProp(CabbageIDs::sliderskew);
        slider->setSkewFactor(cAttr.getNumProp(CabbageIDs::sliderskew));
        slider->setRange(min, max, cAttr.getNumProp(CabbageIDs::sliderincr));
        slider->setValue(cAttr.getNumProp(CabbageIDs::value));
    }//--- end of constructor ----

    //---------------------------------------------
    ~CabbageSlider()
    {
    }

    //update controls
    void update(CabbageGUIClass m_cAttr)
    {
        const MessageManagerLock mmLock;
        slider->setColour(Slider::rotarySliderFillColourId, Colour::fromString(m_cAttr.getStringProp(CabbageIDs::colour)));
        slider->setColour(Slider::thumbColourId, Colour::fromString(m_cAttr.getStringProp(CabbageIDs::colour)));
        slider->setColour(Slider::textBoxTextColourId, Colour::fromString(m_cAttr.getStringProp(CabbageIDs::fontcolour)));
        slider->setColour(Slider::trackColourId, Colour::fromString(m_cAttr.getStringProp(CabbageIDs::trackercolour)));
        setBounds(m_cAttr.getBounds());
        slider->setName(m_cAttr.getStringProp(CabbageIDs::text));
        slider->setSkewFactor(m_cAttr.getNumProp(CabbageIDs::sliderskew));
        if(!m_cAttr.getNumProp(CabbageIDs::visible))
            setVisible(false);
        else
            setVisible(true);
        repaint();
    }

    //---------------------------------------------
    void resized()
    {
        //if rotary
        if (sliderType.contains("rotary"))
        {
            slider->setColour(Slider::rotarySliderFillColourId, Colour::fromString(cl));
            slider->setSliderStyle(Slider::Rotary);
            getProperties().set("type", var("rslider"));
            slider->setSliderStyle(Slider::RotaryVerticalDrag);
			
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
            //else if no group caption then the slider takes the whole area available
            else
                slider->setBounds(0, 0, getWidth(), getHeight());
        }
        //else if vertical
        else if (sliderType.contains("vertical"))
        {
            slider->setSliderStyle(Slider::LinearVertical);
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
                slider->setBounds(0, 0, getWidth(), getHeight());
        }
        //else if horizontal
        else
        {
            slider->setSliderStyle(Slider::LinearHorizontal);
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
                slider->setBounds(0, 0, getWidth(), getHeight());
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
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageSlider);
};

//==============================================================================
// custom checkbox component with optional surrounding groupbox
//==============================================================================
class CabbageCheckbox : public Component
{
    int offX, offY, offWidth, offHeight;

public:
    ScopedPointer<GroupComponent> groupbox;
    ScopedPointer<ToggleButton> button;
    bool isRect;
    String name, caption, buttonText, colour, fontcolour, oncolour;
    //---- constructor -----
    CabbageCheckbox(CabbageGUIClass &cAttr) :
        name(cAttr.getStringProp(CabbageIDs::name)),
        caption(cAttr.getStringProp(CabbageIDs::caption)),
        buttonText(cAttr.getStringProp(CabbageIDs::text)),
        colour(cAttr.getStringProp(CabbageIDs::colour)),
        fontcolour(cAttr.getStringProp(CabbageIDs::fontcolour)),
        oncolour(cAttr.getStringProp(CabbageIDs::oncolour)),
        isRect(cAttr.getStringProp(CabbageIDs::shape).equalsIgnoreCase("square"))
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
        groupbox->setColour(TextButton::buttonColourId, CabbageUtils::getComponentSkin());

        button->setButtonText(buttonText);
        if(caption.length()>0)
        {
            offX=10;
            offY=20;
            offWidth=-20;
            offHeight=-30;
            groupbox->setVisible(true);
            groupbox->setText(caption);
        }

        button->getProperties().set("isRect", isRect);
		
		if(cAttr.getNumProp(CabbageIDs::radiogroup)!=0)
			button->setRadioGroupId(cAttr.getNumProp(CabbageIDs::radiogroup));
			
        button->setColour(ToggleButton::textColourId, Colour::fromString(fontcolour));
        button->setColour(TextButton::buttonOnColourId, Colour::fromString(oncolour));
        button->setColour(TextButton::buttonColourId, Colour::fromString(colour));
        button->setButtonText(buttonText);

        //set initial value if given
        if(cAttr.getNumProp(CabbageIDs::value)==1)
            button->setToggleState(true, sendNotification);
        else
            button->setToggleState(false, sendNotification);

        this->setWantsKeyboardFocus(false);
    }
    //---------------------------------------------
    ~CabbageCheckbox()
    {

    }

    //update controls
    void update(CabbageGUIClass m_cAttr)
    {
        const MessageManagerLock mmLock;
        button->setColour(ToggleButton::textColourId, Colour::fromString(m_cAttr.getStringProp(CabbageIDs::fontcolour)));
        button->setColour(TextButton::buttonColourId, Colour::fromString(m_cAttr.getStringProp(CabbageIDs::colour)));
        setBounds(m_cAttr.getBounds());
        button->getProperties().set("isRect", m_cAttr.getStringProp(CabbageIDs::shape).equalsIgnoreCase("square"));
        button->setButtonText(m_cAttr.getStringProp(CabbageIDs::text));
        if(!m_cAttr.getNumProp(CabbageIDs::visible))
            setVisible(false);
        else
            setVisible(true);
        repaint();
    }

    //---------------------------------------------
    void resized()
    {
        groupbox->setBounds(0, 0, getWidth(), getHeight());
        button->setBounds(offX, offY, getWidth()+offWidth, getHeight()+offHeight);
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
    int offX, offY, offWidth, offHeight;
    String name, caption, text, colour, fontcolour;
    StringArray items;
public:
    //---- constructor -----
    CabbagePopupMenu(CabbageGUIClass &cAttr):
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
    void update(CabbageGUIClass m_cAttr)
    {
        //const MessageManagerLock mmLock;
        //combo->getProperties().set("colour", m_cAttr.getStringProp(CabbageIDs::colour));
        //combo->getProperties().set("fontcolour", m_cAttr.getStringProp(CabbageIDs::fontcolour));
        //setBounds(m_cAttr.getBounds());
        //if(!m_cAttr.getNumProp(CabbageIDs::visible))
        //	setVisible(false);
        //else
        //	setVisible(true);
        //repaint();
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbagePopupMenu);
};

//==============================================================================
// custom checkbox component with optional surrounding groupbox
//==============================================================================
class CabbageComboBox : public Component
{
    //ScopedPointer<LookAndFeel> lookFeel;
    int offX, offY, offWidth, offHeight;
    String name, caption, text, colour, fontcolour;
public:
    ScopedPointer<GroupComponent> groupbox;
    ScopedPointer<ComboBox> combo;
    //---- constructor -----
    CabbageComboBox(CabbageGUIClass &cAttr):
        name(cAttr.getStringProp(CabbageIDs::name)),
        caption(cAttr.getStringProp(CabbageIDs::caption)),
        colour(cAttr.getStringProp(CabbageIDs::colour)),
        fontcolour(cAttr.getStringProp(CabbageIDs::fontcolour))
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
        groupbox->setColour(TextButton::buttonColourId, CabbageUtils::getComponentSkin());

        if(caption.length()>0)
        {
            offX=10;
            offY=35;
            offWidth=-20;
            offHeight=-45;
            groupbox->setVisible(true);
            groupbox->setText(caption);
        }


        combo->setEditableText (false);
        combo->setJustificationType (Justification::centredLeft);
        combo->setTextWhenNothingSelected(text);
        this->setWantsKeyboardFocus(false);

        //populate combo with files
        Array<File> dirFiles;

        if(cAttr.getStringProp(CabbageIDs::filetype).length()<1)
            for(int i=0; i<cAttr.getStringArrayProp("text").size(); i++)
            {
                String item  = cAttr.getStringArrayPropValue("text", i);
                combo->addItem(item, i+1);
            }
        else
        {
            //appProperties->getUserSettings()->getValue("CsoundPluginDirectory");
			combo->clear(dontSendNotification);
            //CabbageUtils::showMessage(cAttr.getStringProp(CabbageIDs::workingdir));
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
				cAttr.setStringArrayPropValue(CabbageIDs::text, i, filename);
            }
        }
		cAttr.setStringArrayProp(CabbageIDs::text, fileNames);
		//CabbageUtils::showMessage(cAttr.getStringArrayPropValue(CabbageIDs::text, 1));
        combo->setSelectedItemIndex(cAttr.getNumProp(CabbageIDs::value)-1);
		
    }
    //---------------------------------------------
    ~CabbageComboBox()
    {

    }

    //update controls
    void update(CabbageGUIClass m_cAttr)
    {
        const MessageManagerLock mmLock;
        combo->getProperties().set("colour", m_cAttr.getStringProp(CabbageIDs::colour));
        combo->getProperties().set("fontcolour", m_cAttr.getStringProp(CabbageIDs::fontcolour));
        setBounds(m_cAttr.getBounds());
        if(!m_cAttr.getNumProp(CabbageIDs::visible))
            setVisible(false);
        else
            setVisible(true);
        repaint();
    }


    //---------------------------------------------
    void resized()
    {
        groupbox->setBounds(0, 0, getWidth(), getHeight());
        combo->setBounds(offX, offY, getWidth()+offWidth, getHeight()+offHeight);
        this->setWantsKeyboardFocus(false);
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageComboBox);
};
//==============================================================================
// custom image component
//==============================================================================
class CabbageImage : public Component,
    public ChangeBroadcaster
{
    String name, outline, colour, shape, file;
    Image img;
    int top, left, width, height, line;
    File picFile;

public:
    CabbageImage(CabbageGUIClass &cAttr):
        name(cAttr.getStringProp(CabbageIDs::name)),
        file(cAttr.getStringProp(CabbageIDs::file)),
        outline(cAttr.getStringProp(CabbageIDs::outlinecolour)),
        colour(cAttr.getStringProp(CabbageIDs::colour)),
        shape(cAttr.getStringProp("shape")),
        line(cAttr.getNumProp(CabbageIDs::line))
    {
        setName(name);
        img = ImageCache::getFromFile (File (file));
        this->setWantsKeyboardFocus(false);
        //if widget is a plant intercept mouse events
        if(cAttr.getStringProp(CabbageIDs::plant).isNotEmpty())
            this->setInterceptsMouseClicks(true, true);
        repaint();
    }
    ~CabbageImage()
    {
    }

    void mouseDown(const MouseEvent& event)
    {
        if(event.mods.isPopupMenu())
            sendChangeMessage();
    }

    //update control
    void update(CabbageGUIClass m_cAttr)
    {
        colour = m_cAttr.getStringProp(CabbageIDs::colour);
        outline = m_cAttr.getStringProp(CabbageIDs::outlinecolour);
        shape = m_cAttr.getStringProp(CabbageIDs::shape);
        line = m_cAttr.getNumProp(CabbageIDs::line);
        setBounds(m_cAttr.getBounds());
        if(!m_cAttr.getNumProp(CabbageIDs::visible))
            setVisible(false);
        else
            setVisible(true);
        repaint();
    }

    void paint (Graphics& g)
    {
        //Logger::writeToLog("in paint routine");
        if(File(file).existsAsFile())
        {
            //Logger::writeToLog("drawing file");
            g.drawImage(img, 0, 0, width, height, 0, 0, img.getWidth(), img.getHeight());
        }
        else
        {
            if(shape=="rounded")
            {
                g.fillAll(Colours::transparentBlack);
                g.setColour(Colour::fromString(outline));
                g.drawRoundedRectangle(0,0, width, height, width*.02, line);
                g.setColour(Colour::fromString(colour));
                g.fillRoundedRectangle(line,line, width-(line*2), height-(line*2), width*.02);
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
    }
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageImage);
};

//==============================================================================
// custom groupbox component, this can act as a plant for other components
//==============================================================================
class CabbageGroupbox : public GroupComponent,
    public ChangeBroadcaster
{
    int offX, offY, offWidth, offHeight;
    String name, caption, text, colour, fontcolour;
    int line;
public:
    //---- constructor -----
    CabbageGroupbox(CabbageGUIClass &cAttr):
        name(cAttr.getStringProp(CabbageIDs::name)),
        caption(cAttr.getStringProp(CabbageIDs::caption)),
        text(cAttr.getStringProp(CabbageIDs::text)),
        colour(cAttr.getStringProp(CabbageIDs::colour)),
        fontcolour(cAttr.getStringProp(CabbageIDs::fontcolour)),
        GroupComponent(cAttr.getStringProp(CabbageIDs::name)),
        line(cAttr.getNumProp(CabbageIDs::line))
    {
        toBack();
        offX=offY=offWidth=offHeight=0;
        setColour(TextButton::buttonColourId, Colour::fromString(colour));
        setName(cAttr.getStringProp(CabbageIDs::name));
        setColour(GroupComponent::textColourId, Colour::fromString(fontcolour));
		getProperties().set("svgpath", cAttr.getStringProp(CabbageIDs::svgpath));

        this->setText(text);
        this->setWantsKeyboardFocus(false);
        if(line==0)
            this->getProperties().set("groupLine", var(0));
        else
            this->getProperties().set("groupLine", var(1));
        this->repaint();

        //if widget is a plant intercept mouse events
        if(cAttr.getStringProp(CabbageIDs::plant).isNotEmpty())
            this->setInterceptsMouseClicks(true, true);
        this->toFront(true);
    }
    //---------------------------------------------
    ~CabbageGroupbox()
    {

    }

    void mouseDown(const MouseEvent& event)
    {
        Logger::writeToLog("mouse down in groupbox");
        if(event.mods.isPopupMenu())
            sendChangeMessage();
    }

    //update control
    void update(CabbageGUIClass m_cAttr)
    {
        setColour(TextButton::buttonColourId, Colour::fromString(m_cAttr.getStringProp(CabbageIDs::colour)));
        setColour(GroupComponent::textColourId, Colour::fromString(m_cAttr.getStringProp(CabbageIDs::fontcolour)));
        setBounds(m_cAttr.getBounds());
        setText(m_cAttr.getStringProp(CabbageIDs::text));
        if(!m_cAttr.getNumProp(CabbageIDs::visible))
            setVisible(false);
        else
            setVisible(true);
        repaint();
    }

    //---------------------------------------------
    //void resized()
    //{
    //this->setBounds(0, 18, getWidth(), getHeight());
    //Logger::writeToLog("resizing plant");
    //this->setWantsKeyboardFocus(false);
    //}


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageGroupbox);
};

//==============================================================================
// custom directorylist
//==============================================================================
class CabbageDirectoryList	:	public Component
{
public:
    ScopedPointer<DirectoryContentsComponent> directoryList;
    String name, channel, workingDir, fileType;
//---- constructor -----
public:
    CabbageDirectoryList (String _name, String _channel, String _workingDir, String _fileType):
        name(_name),
        channel(_channel),
        workingDir(_workingDir),
        fileType(_fileType)
    {
        setName(name);
        directoryList = new DirectoryContentsComponent(workingDir, fileType);
        addAndMakeVisible(directoryList);
    }

    ~CabbageDirectoryList() {}

    void resized()
    {
        directoryList->setBounds(0, 0, getWidth(), getHeight());
    }

    void paint(Graphics& g) {}
    const StringArray getListContents()
    {
        StringArray test = directoryList->getFunctionTables();
        //Logger::writeToLog(test.joinIntoString("\n"));

        return directoryList->getFunctionTables();
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageDirectoryList);
};

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

        g.setColour (CabbageUtils::getBackgroundSkin());
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
    public ChangeListener
{
    String colour;
    String fontcolour;
    String file;
	var ampranges;
    float zoom;
	float startpos, endpos;
    double sampleRate;
    double scrubberPos;
	Array<float> ampRanges;
//---- constructor -----
public:
    CabbageGenTable (CabbageGUIClass &cAttr) : colour(cAttr.getStringProp(CabbageIDs::colour)),
        fontcolour(cAttr.getStringProp(CabbageIDs::fontcolour)),
        file(cAttr.getStringProp(CabbageIDs::file)),
        zoom(cAttr.getNumProp(CabbageIDs::zoom)),
        scrubberPos(cAttr.getNumProp(CabbageIDs::scrubberposition)),
		startpos(-1),
		endpos(-1)
    {
        setName(cAttr.getStringProp(CabbageIDs::name));
        table = new TableManager();
        addAndMakeVisible(table);
        //table->addChangeListener(this);
        sampleRate = 44100;
        if(File(file).existsAsFile())
            setFile(file);
    }

    ~CabbageGenTable()
    {
    }

    void resized()
    {
        table->setBounds(0, 0, getWidth(), getHeight());
    }


    //update control
    void update(CabbageGUIClass m_cAttr)
    {
        //setBounds(m_cAttr.getBounds());
		//Logger::writeToLog("ScrubberPos:"+String(m_cAttr.getNumProp(CabbageIDs::scrubberposition)));
        if(scrubberPos!=m_cAttr.getNumProp(CabbageIDs::scrubberposition))
        {
            //var scrubberPos = m_cAttr.getVarArrayProp(CabbageIDs::scrubberposition);
			scrubberPos = m_cAttr.getVarArrayProp(CabbageIDs::scrubberposition)[0];
			int tableNumber = m_cAttr.getVarArrayProp(CabbageIDs::scrubberposition)[1];		
			table->setScrubberPos(scrubberPos, tableNumber);
        }

        if(!m_cAttr.getNumProp(CabbageIDs::visible))
            setVisible(false);
        else
            setVisible(true);

		if(ampRanges!=m_cAttr.getFloatArrayProp("amprange"))
		{
			ampRanges = m_cAttr.getFloatArrayProp("amprange");
			table->setAmpRanges(ampRanges);
			if(ampRanges.size()>2)
				table->enableEditMode(StringArray(""), ampRanges[2]);
		}

		if(m_cAttr.getNumProp(CabbageIDs::startpos)!=startpos &&  m_cAttr.getNumProp(CabbageIDs::endpos)!=endpos)
		{
		table->setRange(m_cAttr.getNumProp(CabbageIDs::startpos), m_cAttr.getNumProp(CabbageIDs::endpos));	
		endpos = m_cAttr.getNumProp(CabbageIDs::endpos);
		startpos = m_cAttr.getNumProp(CabbageIDs::startpos);
		}

        if(zoom!=m_cAttr.getNumProp(CabbageIDs::zoom))
        {
            zoom = m_cAttr.getNumProp(CabbageIDs::zoom);
            //table->setZoomFactor(zoom);
        }


    }

    void setFile(String newFile)
    {
        //table->setFile(File(newFile));
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
    float zoom;
    double sampleRate;
    float scrubberPos;
//---- constructor -----
public:
    CabbageSoundfiler (CabbageGUIClass &cAttr) : colour(cAttr.getStringProp(CabbageIDs::colour)),
        fontcolour(cAttr.getStringProp(CabbageIDs::fontcolour)),
        file(cAttr.getStringProp(CabbageIDs::file)),
        zoom(cAttr.getNumProp(CabbageIDs::zoom)),
        scrubberPos(cAttr.getNumProp(CabbageIDs::scrubberposition))
    {
        setName(cAttr.getStringProp(CabbageIDs::name));
        soundFiler = new Soundfiler(44100, Colour::fromString(colour), Colour::fromString(fontcolour));
        addAndMakeVisible(soundFiler);
        soundFiler->addChangeListener(this);
        sampleRate = 44100;
        soundFiler->setZoomFactor(cAttr.getNumProp(CabbageIDs::zoom));
        if(File(file).existsAsFile())
            setFile(file);
    }

    ~CabbageSoundfiler()
    {
    }

    void resized()
    {
        soundFiler->setBounds(0, 0, getWidth(), getHeight());
    }


    //update control
    void update(CabbageGUIClass m_cAttr)
    {
        setBounds(m_cAttr.getBounds());
        if(scrubberPos!=m_cAttr.getNumProp(CabbageIDs::scrubberposition))
        {
            scrubberPos = m_cAttr.getNumProp(CabbageIDs::scrubberposition);
            soundFiler->setScrubberPos(scrubberPos);
        }

        if(!m_cAttr.getNumProp(CabbageIDs::visible))
            setVisible(false);
        else
            setVisible(true);

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
// custom CabbageLabel
//==============================================================================
class CabbageLabel	:	public Component
{

public:
    CabbageLabel (CabbageGUIClass &cAttr)
        : text(cAttr.getStringProp(CabbageIDs::text)),
          colour(cAttr.getStringProp(CabbageIDs::colour)),
          fontcolour(cAttr.getStringProp(CabbageIDs::fontcolour)),
          align(cAttr.getStringProp(CabbageIDs::align)),
          textAlign(Justification::centred)
    {
        if(!cAttr.getNumProp(CabbageIDs::visible))
        {
            setVisible(false);
            Logger::writeToLog("visivle");
        }
        else
        {
            setVisible(true);
            Logger::writeToLog("visivle");
        }

        setText(cAttr.getStringProp(CabbageIDs::text));

        if(align=="centre")
            textAlign = Justification::centred;
        else if(align=="left")
            textAlign = Justification::left;
        else
            textAlign = Justification::right;

    }

    ~CabbageLabel()
    {
    }

    void resized()
    {

    }

    void paint(Graphics& g)
    {
        g.setColour(Colour::fromString(colour));
        g.fillRoundedRectangle(getLocalBounds().toFloat(), 3.f);
        g.setColour(Colour::fromString(fontcolour));
        g.setFont(CabbageUtils::getComponentFont());
        g.setFont(getHeight());
        g.drawFittedText(text, 0, 0, getWidth(), getHeight(), textAlign, 1, 1);
    }

    void setText(String _text)
    {
        text = _text;
        repaint();
    }


    //update control
    void update(CabbageGUIClass m_cAttr)
    {
        colour = m_cAttr.getStringProp(CabbageIDs::colour);
        fontcolour = m_cAttr.getStringProp(CabbageIDs::fontcolour);
        setBounds(m_cAttr.getBounds());
        if(!m_cAttr.getNumProp(CabbageIDs::visible))
            setVisible(false);
        else
            setVisible(true);
        setText(m_cAttr.getStringProp(CabbageIDs::text));
        repaint();
    }


private:
    String text, colour, fontcolour, align;
    Justification textAlign;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageLabel);
};

//==============================================================================
// custom multitab
//==============================================================================
class CabbageMultiTab	:	public Component
{
public:
    ScopedPointer<TabbedComponent> tabComp;
    String name, fontcolour, bgcolour;
//---- constructor -----
public:
    CabbageMultiTab (String _name,  String _fontcolour, String _bgcolour):
        name(_name),
        fontcolour(_fontcolour),
        bgcolour(_bgcolour)
    {
        setName(name);
        tabComp = new TabbedComponent(TabbedButtonBar::TabsAtTop);
        tabComp->setOutline(0);
        addAndMakeVisible(tabComp);
    }

    ~CabbageMultiTab() {}

    void resized()
    {
        tabComp->setBounds(0, 0, getWidth(), getHeight());
    }

    void paint(Graphics& g) {}


private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageMultiTab);
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
                        float xValue,
                        float yValue)
    {
        setName(name);
        XYAutoIndex = index;
        offX=offY=offWidth=offHeight=0;
        caption="";
        groupbox = new GroupComponent(String("groupbox_")+name);
        groupbox->setWantsKeyboardFocus(false);
        xypad = new XYPad(xyAuto, text, minX, maxX, minY, maxY, dec, Colour::fromString(colour), Colour::fromString(fontcolour), xValue, yValue);
        xypad->setWantsKeyboardFocus(false);
        addAndMakeVisible(xypad);
        addAndMakeVisible(groupbox);

        groupbox->setVisible(false);
        //outline colour ID
        groupbox->setColour(GroupComponent::textColourId, Colour::fromString(fontcolour));
        groupbox->setColour(TextButton::buttonColourId, CabbageUtils::getComponentSkin());

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
    void update(CabbageGUIClass m_cAttr)
    {
        setBounds(m_cAttr.getBounds());
        if(!m_cAttr.getNumProp(CabbageIDs::visible))
            setVisible(false);
        else
            setVisible(true);
        repaint();
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
    String text, name, caption, type, currentText;
	StringArray strings;
    Colour colour, fontcolour;
    int offX, offY, offWidth, offHeight, stringIndex;
public:
	String channel;
    ScopedPointer<TextEditor> editor;
    //---- constructor -----
    CabbageTextEditor(CabbageGUIClass &cAttr) :
        name(cAttr.getStringProp(CabbageIDs::name)),
        caption(cAttr.getStringProp(CabbageIDs::caption)),
        text(cAttr.getStringProp(CabbageIDs::text)),
        type(cAttr.getStringProp(CabbageIDs::type)),
        editor(new TextEditor(String("editor_"))),
        groupbox(new GroupComponent(String("groupbox_"))),
        lookAndFeel(new LookAndFeel_V1()),
        fontcolour(Colour::fromString(cAttr.getStringProp(CabbageIDs::fontcolour))),
        colour(Colour::fromString(cAttr.getStringProp(CabbageIDs::colour))),
		channel(cAttr.getStringProp(CabbageIDs::channel)),
        offX(0),
        offY(0),
        offWidth(0),
        offHeight(0),
		stringIndex(0)
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
        //groupbox->setColour(GroupComponent::ColourIds::outlineColourId, Colours::red);
        this->setWantsKeyboardFocus(false);
    }

    //---------------------------------------------
    ~CabbageTextEditor() {}

    //update control
    void update(CabbageGUIClass m_cAttr)
    {
        editor->setColour(0x1000200, Colour::fromString(m_cAttr.getStringProp(CabbageIDs::colour)));
        editor->setColour(0x1000201, Colour::fromString(m_cAttr.getStringProp(CabbageIDs::fontcolour)));
        setBounds(m_cAttr.getBounds());
        if(!m_cAttr.getNumProp(CabbageIDs::visible))
            setVisible(false);
        else
            setVisible(true);
			
		if(text!=m_cAttr.getStringProp(CabbageIDs::text))	
		{
			editor->setText(m_cAttr.getStringProp(CabbageIDs::text));
			text = m_cAttr.getStringProp(CabbageIDs::text);			
		}
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
        this->setWantsKeyboardFocus(false);
    }

	void textEditorReturnKeyPressed (TextEditor&) 
	{
		//CabbageUtils::showMessage(editor->getText());
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
    String text, name, caption, type;
    Colour colour, fontcolour;
    int offX, offY, offWidth, offHeight;
public:
    ScopedPointer<TextEditor> editor;
    //---- constructor -----
    CabbageTextbox(CabbageGUIClass &cAttr) :
        name(cAttr.getStringProp(CabbageIDs::name)),
        caption(cAttr.getStringProp(CabbageIDs::caption)),
        text(cAttr.getStringProp(CabbageIDs::text)),
        type(cAttr.getStringProp(CabbageIDs::type)),
        editor(new TextEditor(String("editor_"))),
        groupbox(new GroupComponent(String("groupbox_"))),
        lookAndFeel(new LookAndFeel_V1()),
        fontcolour(Colour::fromString(cAttr.getStringProp(CabbageIDs::fontcolour))),
        colour(Colour::fromString(cAttr.getStringProp(CabbageIDs::colour))),
        offX(0),
        offY(0),
        offWidth(0),
        offHeight(0)
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
        //background colour ID
        editor->setColour(0x1000200, colour);
        //text colour ID
        editor->setColour(0x1000201, fontcolour);

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
    void update(CabbageGUIClass m_cAttr)
    {
        editor->setColour(0x1000200, Colour::fromString(m_cAttr.getStringProp(CabbageIDs::colour)));
        editor->setColour(0x1000201, Colour::fromString(m_cAttr.getStringProp(CabbageIDs::fontcolour)));
        setBounds(m_cAttr.getBounds());
        if(type==CabbageIDs::textbox.toString())
            setFile(m_cAttr.getStringProp(CabbageIDs::file));
        if(!m_cAttr.getNumProp(CabbageIDs::visible))
            setVisible(false);
        else
            setVisible(true);
        repaint();
    }

    void paint(Graphics &g)
    {
        //----- For drawing the border
        if(type==CabbageIDs::csoundoutput)
        {
            g.setColour(CabbageUtils::getComponentSkin());
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
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageTextbox);
};


//==============================================================================
// custom CabbageTable, uses the Table class
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
        groupbox->setColour(TextButton::buttonColourId, CabbageUtils::getComponentSkin());

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
        table->repaint();
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
    void update(CabbageGUIClass m_cAttr)
    {
        setBounds(m_cAttr.getBounds());
        if(!m_cAttr.getNumProp(CabbageIDs::visible))
            setVisible(false);
        else
            setVisible(true);
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
    Colour colour, outlinecolour, fontcolour;
    ScopedPointer<GroupComponent> groupbox;
    ScopedPointer<Label> label;
    int offX, offY, offWidth, offHeight, width, height, value, lastValue, decPlaces, textbox;
    String text, caption;
    float min, max, skew, incr;
    bool buttonState, mouseButtonState;
public:
    ScopedPointer<Slider> slider;
    CabbageNumberBox(CabbageGUIClass &cAttr) :
        colour(Colour::fromString(cAttr.getStringProp(CabbageIDs::colour))),
        outlinecolour(Colour::fromString(cAttr.getStringProp(CabbageIDs::outlinecolour))),
        fontcolour(Colour::fromString(cAttr.getStringProp(CabbageIDs::fontcolour))),
        text(cAttr.getStringProp(CabbageIDs::text)),
        textbox(cAttr.getNumProp(CabbageIDs::textbox)),
        decPlaces(cAttr.getNumProp(CabbageIDs::decimalplaces)),
        caption(cAttr.getStringProp(CabbageIDs::caption)),
		name(cAttr.getStringProp(CabbageIDs::name))

    {
        setName(name);
        offX=offY=offWidth=offHeight=0;
        groupbox = new GroupComponent(String("groupbox_")+name);
        slider = new Slider(text);
        //slider->setName(text);
        slider->toFront(true);
        label = new Label();
        label->setText(text, dontSendNotification);
        label->setColour(Label::textColourId, fontcolour);

        addAndMakeVisible(label);
        addAndMakeVisible(slider);
        addAndMakeVisible(groupbox);
        groupbox->setVisible(false);
        groupbox->getProperties().set("groupLine", var(1));
        groupbox->setColour(GroupComponent::textColourId, fontcolour);
        groupbox->setColour(TextButton::buttonColourId, CabbageUtils::getComponentSkin());

        slider->setSliderStyle(Slider::LinearBarVertical);
        slider->setColour(Slider::trackColourId, colour);
        slider->setColour(Slider::thumbColourId, colour);
        slider->setColour(Slider::textBoxHighlightColourId, slider->findColour(Slider::textBoxBackgroundColourId));
        slider->setColour(Slider::textBoxTextColourId, fontcolour.contrasting());
        slider->setVelocityBasedMode(true);
        slider->setVelocityModeParameters(80);
        slider->getProperties().set("decimalPlaces", decPlaces);


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
    }

    //update controls
    void update(CabbageGUIClass m_cAttr)
    {
        const MessageManagerLock mmLock;
        slider->setColour(Slider::rotarySliderFillColourId, Colour::fromString(m_cAttr.getStringProp(CabbageIDs::colour)));
        slider->setColour(Slider::thumbColourId, Colour::fromString(m_cAttr.getStringProp(CabbageIDs::colour)));
        slider->setColour(Slider::textBoxTextColourId, Colour::fromString(m_cAttr.getStringProp(CabbageIDs::fontcolour)));
        slider->setColour(Slider::trackColourId, Colour::fromString(m_cAttr.getStringProp(CabbageIDs::trackercolour)));
        setBounds(m_cAttr.getBounds());
        slider->setName(m_cAttr.getStringProp(CabbageIDs::text));
        slider->setSkewFactor(m_cAttr.getNumProp(CabbageIDs::sliderskew));
        if(!m_cAttr.getNumProp(CabbageIDs::visible))
            setVisible(false);
        else
            setVisible(true);
        repaint();
    }

};

//==============================================================================
// custom CabbageTransportControl
//==============================================================================
/*
 * this will need a fast forward, skip to start, play/pause and
 */
class CabbageTransportControl	:	public Component
{
    ScopedPointer<ImageButton> playButton;
    ScopedPointer<ImageButton> skipToStartButton;
    ScopedPointer<ImageButton> skipToEndButton;

public:
    CabbageTransportControl(int width, int height)
    {
        playButton = new ImageButton("Play button");
        addAndMakeVisible(playButton);
        skipToStartButton = new ImageButton("Skip to start button");
        addAndMakeVisible(skipToStartButton);
        skipToEndButton = new ImageButton("Skip to end button");
        addAndMakeVisible(skipToEndButton);

        //playButton->setToggleState(false, true);
        playButton->setClickingTogglesState(true);
        //playButton->setState(Button::buttonDown);
        /*
        	playButton->setImages(false, true, true,
        		CabbageUtils::drawSoundfilerButton("play_normal"), 1.0f, Colours::transparentBlack,
        		CabbageUtils::drawSoundfilerButton("play_hover"), 1.0f, Colours::transparentBlack,
        		CabbageUtils::drawSoundfilerButton("play_down"), 1.0f, Colours::transparentBlack);

        	skipToStartButton->setImages(false, true, true,
        		CabbageUtils::drawSoundfilerButton("skip_start_normal"), 1.0f, Colours::transparentBlack,
        		CabbageUtils::drawSoundfilerButton("skip_start_hover"), 1.0f, Colours::transparentBlack,
        		CabbageUtils::drawSoundfilerButton("skip_start_down"), 1.0f, Colours::transparentBlack);

        	skipToEndButton->setImages(false, true, true,
        		CabbageUtils::drawSoundfilerButton("skip_end_normal"), 1.0f, Colours::transparentBlack,
        		CabbageUtils::drawSoundfilerButton("skip_end_hover"), 1.0f, Colours::transparentBlack,
        		CabbageUtils::drawSoundfilerButton("skip_end_down"), 1.0f, Colours::transparentBlack);
        		 */
    }

    ~CabbageTransportControl() {}

    void resized()
    {
        int numOfButtons = 3;
        float buttonWidth = getWidth() / (numOfButtons+1); //jmin(getHeight()*0.8f, (float)getWidth()/(numOfButtons+1));
        float buttonGap = buttonWidth / (numOfButtons+1); //(getWidth()-(buttonWidth*numOfButtons)) / numOfButtons;
        float buttonHeight = getHeight() - (buttonGap*2);

        skipToStartButton->setBounds(buttonGap, buttonGap, buttonWidth, buttonHeight);
        playButton->setBounds(buttonGap*2 + buttonWidth, buttonGap, buttonWidth, buttonHeight);
        skipToEndButton->setBounds(buttonGap*3 + buttonWidth*2, buttonGap, buttonWidth, buttonHeight);
    }

    void paint(Graphics& g)
    {
        //g.setColour(CabbageUtils::getDarkerBackgroundSkin());
        g.setColour(Colours::lightgrey);
        g.fillAll();
    }
};

#endif
