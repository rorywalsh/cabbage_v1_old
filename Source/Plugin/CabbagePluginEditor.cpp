/*
  Copyright (C) 2007 Rory Walsh

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

#include "CabbagePluginProcessor.h"
#include "CabbagePluginEditor.h"
#include  "../CabbageCustomWidgets.h"


#ifdef Cabbage_Build_Standalone
#include "../ComponentLayoutEditor.h"
#include "../CabbageMainPanel.h"
#endif


Array <Colour> swatchColours;

//==============================================================================
CabbagePluginAudioProcessorEditor::CabbagePluginAudioProcessorEditor (CabbagePluginAudioProcessor* ownerFilter)
    : AudioProcessorEditor (ownerFilter),
      inValue(0),
      authorText(""),
      keyIsPressed(false),
      currentLineNumber(0),
#ifdef Cabbage_Build_Standalone
      propsWindow(new CabbagePropertiesDialog("Properties")),
#endif
      xyPadIndex(0),
      tableBuffer(2, 44100),
	  showScrollbars(true)
{
    //setup swatches for colour selector.
    swatchColours.set(0, Colour(0xFF000000));
    swatchColours.set(1, Colour(0xFFFFFFFF));
    swatchColours.set(2, Colour(0xFFFF0000));
    swatchColours.set(3, Colour(0xFF00FF00));
    swatchColours.set(4, Colour(0xFF0000FF));
    swatchColours.set(5, Colour(0xFFFFFF00));
    swatchColours.set(6, Colour(0xFFFF00FF));
    swatchColours.set(7, Colour(0xFF00FFFF));
    swatchColours.set(8, Colour(0x80000000));
    swatchColours.set(9, Colour(0x80FFFFFF));
    swatchColours.set(10, Colour(0x80FF0000));
    swatchColours.set(11, Colour(0x8000FF00));
    swatchColours.set(12, Colour(0x800000FF));
    swatchColours.set(13, Colour(0x80FFFF00));
    swatchColours.set(14, Colour(0x80FF00FF));
    swatchColours.set(15, Colour(0x8000FFFF));
    setWantsKeyboardFocus(false);
    //set custom skin yo use
    lookAndFeel = new CabbageLookAndFeel();
    basicLookAndFeel = new CabbageLookAndFeelBasic();

#ifdef Cabbage_Build_Standalone
    propsWindow->setAlwaysOnTop(true);
    propsWindow->setVisible(false);
    propsWindow->centreWithSize(5,5);
    propsWindow->addActionListener(this);
    propsWindow->setLookAndFeel(basicLookAndFeel);
    propsWindow->setTitleBarHeight(20);
#endif


    viewport = new Viewport("mainViewport");
    viewportComponent = new CabbageViewportComponent();
    viewport->addMouseListener(this, true);

    Component::setLookAndFeel(lookAndFeel);
    //oldSchoolLook = new OldSchoolLookAndFeel();
#ifdef Cabbage_Build_Standalone
    //determine whether instrument should be opened in GUI mode or not
    componentPanel = new CabbageMainPanel();
    componentPanel->setLookAndFeel(lookAndFeel);
    componentPanel->setBounds(0, 0, getWidth(), getHeight());
    layoutEditor = new ComponentLayoutEditor();
    layoutEditor->setLookAndFeel(lookAndFeel);
    layoutEditor->addChangeListener(this);
    layoutEditor->setBounds(0, 0, getWidth(), getHeight());



    viewportComponent->addAndMakeVisible(layoutEditor);
    viewportComponent->addAndMakeVisible(componentPanel);

    layoutEditor->setTargetComponent(componentPanel);
    layoutEditor->setInterceptsMouseClicks(true, true);
    resizeLimits.setSizeLimits (150, 150, 3800, 3800);
    resizer = new CabbageCornerResizer(this, this, &resizeLimits);

#else
    componentPanel = new Component();
    componentPanel->setTopLeftPosition(0, 0);
    viewportComponent->addAndMakeVisible(componentPanel);
#endif




    componentPanel->addKeyListener(this);
    componentPanel->setInterceptsMouseClicks(false, true);



#ifndef Cabbage_No_Csound
    if(getFilter()->getCsound())
        zero_dbfs = getFilter()->getCsound()->Get0dBFS();
#endif

    setSize(400, 400);
    componentPanel->setSize(400, 400);
#ifdef Cabbage_Build_Stanalone
    layoutEditor->setSize(400, 400);
    layoutEditor->updateFrames();
#endif


    for(int i=0; i<getFilter()->getGUILayoutCtrlsSize(); i++)
        InsertGUIControls(getFilter()->getGUILayoutCtrls(i));
    for(int i=0; i<getFilter()->getGUICtrlsSize(); i++)
        InsertGUIControls(getFilter()->getGUICtrls(i));


    //this will prevent editors from creating xyAutos if they have already been crated.
    getFilter()->setHaveXYAutoBeenCreated(true);


#ifdef Cabbage_Build_Standalone
    componentPanel->addActionListener(this);
    if(!ownerFilter->isGuiEnabled())
    {
        layoutEditor->addAndMakeVisible(resizer);
        layoutEditor->setEnabled(false);
        layoutEditor->toFront(false);
        layoutEditor->updateFrames();
#ifdef Cabbage_Build_Standalone
        //only want to grab keyboard focus on standalone mode as DAW handle their own keystrokes
        componentPanel->setWantsKeyboardFocus(true);
        componentPanel->toFront(true);
        componentPanel->grabKeyboardFocus();
#endif
    }
    else
    {
        layoutEditor->setEnabled(true);
        layoutEditor->toFront(true);
        layoutEditor->updateFrames();
    }
#endif

#ifdef Cabbage_Build_Standalone
    //only want to grab keyboard focus on standalone mode as DAW handle their own keystrokes
    componentPanel->setWantsKeyboardFocus(true);
    componentPanel->toFront(true);
    componentPanel->grabKeyboardFocus();
#endif

    //we update our tables when our editor first opens by sending -1's to each table channel. These are aded to our message queue so
    //the data will only be sent to Csund when it's safe
    for(int index=0; index<getFilter()->getGUILayoutCtrlsSize(); index++)
    {
        if(getFilter()->getGUILayoutCtrls(index).getStringProp(CabbageIDs::type)=="table")
        {
            for(int y=0; y<getFilter()->getGUILayoutCtrls(index).getStringArrayProp("channels").size(); y++)
                getFilter()->messageQueue.addOutgoingChannelMessageToQueue(getFilter()->getGUILayoutCtrls(index).getStringArrayPropValue(CabbageIDs::channel, y).toUTF8(),  -1.f, getFilter()->getGUILayoutCtrls(index).getStringProp(CabbageIDs::type));
        }
    }

    //addAndMakeVisible(viewportComponent);
    //viewport->setScrollBarsShown(true, true);
    addAndMakeVisible(viewport);
    viewport->setViewedComponent(viewportComponent);
    getFilter()->addChangeListener(this);
    resized();
    //startTimer(20);
}


//============================================================================
//desctrutor
//============================================================================
CabbagePluginAudioProcessorEditor::~CabbagePluginAudioProcessorEditor()
{
    comps.clear(true);
    subPatches.clear(true);
    layoutComps.clear(true);
    getFilter()->removeChangeListener(this);
    removeAllChangeListeners();
    //getFilter()->editorBeingDeleted(this);
    if(presetFileText.length()>1)
        SnapShotFile.replaceWithText(presetFileText);
		
	Logger::writeToLog("======EDITOR DECONSTRCUTOR=========");	
}

//==============================================================================
void CabbagePluginAudioProcessorEditor::resized()
{
    //this->setSize(this->getWidth(), this->getHeight());
    viewport->setBounds(0, 0, this->getWidth(), this->getHeight());

    componentPanel->setTopLeftPosition(0, 0);

#ifdef Cabbage_Build_Standalone
    layoutEditor->setTopLeftPosition(0, 0);
    if(componentPanel->getWidth()<this->getWidth()+18 && componentPanel->getHeight()<this->getHeight()+18)
        viewport->setScrollBarsShown(false, false);
    else
		if(showScrollbars)
        viewport->setScrollBarsShown(true, true);
		else
			viewport->setScrollBarsShown(false, false);
#endif
}

//==============================================================================
void CabbagePluginAudioProcessorEditor::InsertGUIControls(CabbageGUIClass cAttr)
{
    if(cAttr.getStringProp(CabbageIDs::type)==String("form"))
    {
        SetupWindow(cAttr);   //set main application
    }
    else if(cAttr.getStringProp(CabbageIDs::type)==String("groupbox"))
    {
        InsertGroupBox(cAttr);
    }
    else if(cAttr.getStringProp(CabbageIDs::type)==String("image"))
    {
        InsertImage(cAttr);
    }
    else if(cAttr.getStringProp(CabbageIDs::type)==String("keyboard"))
    {
        InsertMIDIKeyboard(cAttr);
    }
    else if(cAttr.getStringProp(CabbageIDs::type)==String("label"))
    {
        InsertLabel(cAttr);
    }
    else if(cAttr.getStringProp(CabbageIDs::type)==String("popupmenu"))
    {
        InsertPopupMenu(cAttr);
    }
    else if(cAttr.getStringProp(CabbageIDs::type)==String("csoundoutput"))
    {
        InsertCsoundOutput(cAttr);
    }
    else if(cAttr.getStringProp(CabbageIDs::type)==String("snapshot"))
    {
        InsertSnapshot(cAttr);
    }
    else if(cAttr.getStringProp(CabbageIDs::type)==String("gentable"))
    {
        InsertGenTable(cAttr);
    }
    else if(cAttr.getStringProp(CabbageIDs::type)==String("infobutton"))
    {
        InsertInfoButton(cAttr);
    }
    else if(cAttr.getStringProp(CabbageIDs::type)==String("filebutton"))
    {
        InsertFileButton(cAttr);
    }
    else if(cAttr.getStringProp(CabbageIDs::type)==String("recordbutton"))
    {
        InsertRecordButton(cAttr);
    }
    else if(cAttr.getStringProp(CabbageIDs::type)==String("textbox"))
    {
        InsertTextbox(cAttr);
    }
    else if(cAttr.getStringProp(CabbageIDs::type)==String("transport"))
    {
        InsertTransport(cAttr);
    }
    else if(cAttr.getStringProp(CabbageIDs::type)==String("soundfiler"))
    {
        InsertSoundfiler(cAttr);
    }
    else if(cAttr.getStringProp(CabbageIDs::type)==String("numberbox"))
    {
        InsertNumberBox(cAttr);
    }
    else if(cAttr.getStringProp(CabbageIDs::type)==String("directorylist"))
    {
        InsertDirectoryList(cAttr);
    }
    else if(cAttr.getStringProp(CabbageIDs::type)==String("multitab"))
    {
        InsertMultiTab(cAttr);
    }
    else if(cAttr.getStringProp(CabbageIDs::type)==String(CabbageIDs::line))
    {
        InsertLineSeparator(cAttr);
    }
    else if(cAttr.getStringProp(CabbageIDs::type)==String("table"))
    {
        InsertTable(cAttr);
    }
    else if(cAttr.getStringProp(CabbageIDs::type)==String("hslider")
            ||cAttr.getStringProp(CabbageIDs::type)==String("vslider")
            ||cAttr.getStringProp(CabbageIDs::type)==String("rslider"))
    {
        InsertSlider(cAttr);       //insert slider
    }
    else if(cAttr.getStringProp(CabbageIDs::type)==String("button"))
    {
        InsertButton(cAttr);       //insert button
    }
    else if(cAttr.getStringProp(CabbageIDs::type)==String("checkbox"))
    {
        InsertCheckBox(cAttr);       //insert checkbox
    }
    else if(cAttr.getStringProp(CabbageIDs::type)==String("combobox"))
    {
        InsertComboBox(cAttr);       //insert combobox
    }
    else if(cAttr.getStringProp(CabbageIDs::type)==String("xypad"))
    {
        InsertXYPad(cAttr);       //insert xypad
    }
    else if(cAttr.getStringProp(CabbageIDs::type)==String("texteditor"))
    {
        InsertTextEditor(cAttr);       //insert xypad
    }
}


//===========================================================================
//WHEN IN GUI EDITOR MODE THIS CALLBACK WILL NOTIFIY THE HOST OF EVENTS
//IT CAN ALSO BE CALLED BY OTHER GUI WIDGETS WHEN THEIR STATE CHNANGES
//===========================================================================
void CabbagePluginAudioProcessorEditor::changeListenerCallback(ChangeBroadcaster *source)
{
//update our GUI controls when the filter says so! This is triggered by the
//CabbagePluginAudioProcessor::updateCabbageControls()
    CabbagePluginAudioProcessor* filter = dynamic_cast<CabbagePluginAudioProcessor*>(source);
    if(filter)
        updateGUIControls();

#ifdef Cabbage_Build_Standalone
    ComponentLayoutEditor* le = dynamic_cast<ComponentLayoutEditor*>(source);
    if(le)
    {
        if(le->currentEvent=="triggerPopupMenu")
            showInsertControlsMenu(le->currentMouseCoors.getX(), le->currentMouseCoors.getY());
        else if(le->currentEvent=="mouseDownChildAlias")
        {
        }
        else if(le->currentEvent=="mouseUpChildAlias")
            //following method updates the sizes and positions of components when a mouseUp takes place
            updateSizesAndPositionsOfComponents();

        else if(le->currentEvent.contains("addPlantToRepo"))
        {
            String name = le->currentEvent.substring(15);
            addToRepository(name);
        }

        else if(le->currentEvent=="deleteComponents")
            //following method deletes selected componments and plants
            deleteComponents();

        else if(le->currentEvent=="convertToPlant")
            convertIntoPlant();

        else if(le->currentEvent=="breakUpPlant")
            breakUpPlant();

        else if(le->currentEvent=="duplicateComponents")
            duplicateComponents();

        le->currentEvent = "";
    }
    else
    {
        Table* table = dynamic_cast<Table*>(source);
        if(table)
        {
            if(table->changeMessage=="overwriteFunctionTable")
                insertScoreStatementText(table, true);
            else if(table->changeMessage=="writeNewFunctionTable")
                insertScoreStatementText(table, false);
            else if(table->changeMessage=="updateFunctionDisplay")
                createfTableData(table, false);
            else// "updateFunctionTable"
                createfTableData(table, true);
            return;
        }

        GenTable* gentable = dynamic_cast<GenTable*>(source);
        if(gentable)
        {
            if(gentable->changeMessage == "updateFunctionTable")
                updatefTableData(gentable);
        }

        CabbageTextEditor* textEditor = dynamic_cast<CabbageTextEditor*>(source);
        if(textEditor)
        {
           getFilter()->messageQueue.addOutgoingChannelMessageToQueue(textEditor->channel, textEditor->getCurrentText(), "string");
		}

        CabbageSoundfiler* soundfiler = dynamic_cast<CabbageSoundfiler*>(source);
        if(soundfiler)
        {
            String channel;
            float val;
            int index = soundfiler->getProperties().getWithDefault(CabbageIDs::index, -99);
            channel = getFilter()->getGUILayoutCtrls(index).getStringArrayProp(CabbageIDs::channel)[0];
            val = soundfiler->getPosition();
            getFilter()->messageQueue.addOutgoingChannelMessageToQueue(channel, val, "");
            //there snhould always be two channels, but check just in case..
            if(getFilter()->getGUILayoutCtrls(index).getStringArrayProp(CabbageIDs::channel).size()>1)
            {
                channel = getFilter()->getGUILayoutCtrls(index).getStringArrayProp(CabbageIDs::channel)[1];
                val = soundfiler->getLoopLength();
                getFilter()->messageQueue.addOutgoingChannelMessageToQueue(channel, val, "");
            }
        }

        CabbageGroupbox* groupbox = dynamic_cast<CabbageGroupbox*>(source);
        if(groupbox)
        {
            for(int i=0; i<popupMenus.size(); i++)
            {
                if(getFilter()->getGUILayoutCtrls(popupMenus[i]).getStringProp("reltoplant").isNotEmpty())
                    if(getFilter()->getGUILayoutCtrls(popupMenus[i]).getBounds().contains(groupbox->getMouseXYRelative()))
                    {
                        int index = popupMenus[i];
                        layoutComps[popupMenus[i]]->setLookAndFeel(lookAndFeel);
                        PopupMenu m;
                        m.setLookAndFeel(lookAndFeel);
                        static_cast<CabbagePopupMenu*>(layoutComps[index])->addItemsToPopup(m);
                        int result;
#if !defined(AndroidBuild)
						result = m.show();
#endif
                        if(result>0)
                            getFilter()->messageQueue.addOutgoingChannelMessageToQueue(
                                getFilter()->getGUILayoutCtrls(index).getStringProp(CabbageIDs::channel),
                                result,
                                "popup");
                    }

            }
        }


    }
#endif
}

//==================================================================
//inserts score statments(f-statements for now) into Csound text file
//==================================================================
void CabbagePluginAudioProcessorEditor::insertScoreStatementText(Table* table, bool overwrite)
{
#ifndef Cabbage_No_Csound
    StringArray csdArray;
    StringArray brokenPlant;
    csdArray.addLines(getFilter()->getCsoundInputFileText());
    if(overwrite==true)
    {
        for(int i=0; i<csdArray.size(); i++)
            if(csdArray[i].substring(0, 10).contains("f"))
            {
                //check for comment
                if(!csdArray[i].substring(0, 3).contains(";"))
                {
                    String statement = csdArray[i].removeCharacters("f");
                    StringArray pfields;
                    pfields.addTokens(statement, " ", "");
                    for(int y=0; y<pfields.size(); y++)
                        if(pfields[0].getFloatValue()==table->tableNumber &&
                                pfields[2].getFloatValue()==table->tableSize)
                        {
                            //Logger::writeToLog(csdArray[i]);
                            csdArray.set(i, table->currentfStatement);
                            getFilter()->updateCsoundFile(csdArray.joinIntoString("\n"));
                            getFilter()->getCsound()->Message("!!Cabbage has overwritten score f-statement!!");
                            getFilter()->highlightLine(csdArray[i]);
                            i=csdArray.size()+100;
                            y=pfields.size()+100;
                        }
                }
            }
    }
    else
    {
        for(int i=0; i<csdArray.size(); i++)
            if(csdArray[i].contains("<CsScore>"))
            {
                csdArray.insert(i+1, table->currentfStatement);
                getFilter()->updateCsoundFile(csdArray.joinIntoString("\n"));
                getFilter()->highlightLine(csdArray[i+1]);
                getFilter()->getCsound()->Message("!!Cabbage has inserted new score f-statement!!");
                i=csdArray.size()+100;
            }
    }


    table->changeMessage="";
    table->currentfStatement = "";
    getFilter()->sendActionMessage("Score Updated");

#endif
}
//==================================================================
//create function table data from our breakpoint envelopes
//==================================================================
void CabbagePluginAudioProcessorEditor::createfTableData(Table* table, bool )
{
    Array< PointData > points;
    for(int i=0; i<table->handles.size(); i++)
        points.add(PointData(table->handles[i]->getPosition(),
                             table->handles[i]->getProperties().getWithDefault("curveType", 0)));

    int xPos= 0;
    int prevXPos=0;
    int prevYPos = 0;
    signed int curve=0;
    String fStatement = "f"+String(table->tableNumber)+" 0 "+String(table->tableSize)+" -16 ";
    String pFields = "";
    //Logger::writeToLog(fStatement);
    float xAxisRescaleFactor = (float)table->tableSize/(float)table->getWidth();
    Logger::writeToLog("end XPos:"+String(table->getWidth()*xAxisRescaleFactor));
    for(int i=0; i<points.size()-1; i++)
    {
        int handleYPos1 = points.getReference(i).point.getY();
        int handleYPos2 = points.getReference(i+1).point.getY();
        //expon curves
        if(points.getReference(i+1).curveType==1)
            if(handleYPos2>handleYPos1)
                curve=-3;
            else
                curve=3;
        if(points.getReference(i+1).curveType==2)
            if(handleYPos2<handleYPos1)
                curve=-3;
            else
                curve=3;
        else
            curve = 0;


        if(handleYPos1<table->getHeight()/2) handleYPos1 = handleYPos1-1;
        else if(handleYPos1>table->getHeight()/2) handleYPos1 = handleYPos1+1;

        float yAmp = table->convertPixelToAmp(handleYPos1);
        if(i+1==points.size()-1)
            xPos = (table->getWidth()*xAxisRescaleFactor)-prevXPos;
        else
            xPos = (points.getReference(i+1).point.getX()*xAxisRescaleFactor-prevXPos);
        pFields = pFields+String(float(yAmp))+" "+String(xPos)+" "+String(curve)+" ";
        //Logger::writeToLog("y:"+String(yAmp));
        //Logger::writeToLog("x:"+String(xPos));
        //Logger::writeToLog("Type:"+String(yAmp));
        prevXPos += xPos;
    }

    int handleYPos;
    if(isPositiveAndBelow(points.size()-1, points.size()))
        handleYPos = points.getReference(points.size()-1).point.getY();
    else
        handleYPos = 0;
    float yAmp = table->convertPixelToAmp(handleYPos);

    pFields = pFields + String(yAmp);
    fStatement = fStatement+pFields;
    //Logger::writeToLog(fStatement);
    table->currentfStatement = fStatement;
    getFilter()->messageQueue.addOutgoingTableUpdateMessageToQueue(fStatement, table->tableNumber);

}

void CabbagePluginAudioProcessorEditor::updatefTableData(GenTable* table)
{
#ifndef Cabbage_No_Csound	
    Array<double> pFields = table->getPfields();
    if( table->genRoutine==5 || table->genRoutine==7 || table->genRoutine==2)
    {
        FUNC *ftpp;
        EVTBLK  evt;
        memset(&evt, 0, sizeof(EVTBLK));
        evt.pcnt = 5+pFields.size();
        evt.opcod = 'f';
        evt.p[0]=0;

        //setting table number to 0.
        evt.p[1]=0;
        evt.p[2]=0;
        evt.p[3]=table->tableSize;
        evt.p[4]=table->realGenRoutine;
		if(table->genRoutine==5)
		{
			for(int i=0; i<pFields.size()-1; i++)
				evt.p[5+i]= jmax(0.00001, pFields[i+1]);
		}
		else if(table->genRoutine==7)
		{
			for(int i=0; i<pFields.size()-1; i++)
				evt.p[5+i]= pFields[i+1];			
		}
		else
		{
			for(int i=0; i<pFields.size(); i++)
				evt.p[5+i] = pFields[i];
		}


        StringArray fStatement;
        for(int i=0; i<evt.pcnt; i++)
            fStatement.add(String(evt.p[i]));

        //now set table number and set score char to f
        fStatement.set(1, String(table->tableNumber));
        fStatement.set(0, "f");

		//Logger::writeToLog(fStatement.joinIntoString(" "));
        getFilter()->getCsound()->GetCsound()->hfgens(getFilter()->getCsound()->GetCsound(), &ftpp, &evt, 1);
        Array<float, CriticalSection> points;

        points = Array<float, CriticalSection>(ftpp->ftable, ftpp->flen);
        table->setWaveform(points, false);
		//table->enableEditMode(fStatement);
        
        getFilter()->messageQueue.addOutgoingTableUpdateMessageToQueue(fStatement.joinIntoString(" "), table->tableNumber);
    }
#endif
}

//=======================================================
//adds a plant to the plant repository
//=======================================================
void CabbagePluginAudioProcessorEditor::addToRepository(String entryName)
{
#ifdef Cabbage_Build_Standalone
    StringArray csdArray;
    StringArray plantText;
    csdArray.addLines(getFilter()->getCsoundInputFileText());
    populateLineNumberArray(csdArray);
    if(csdArray[lineNumbers[0]].contains(" plant("))
    {
        for(int i=0; i>-1; i++)
            if(!csdArray[lineNumbers[0]+i].contains("}"))
                plantText.add(csdArray[lineNumbers[0]+i]);
            else
            {
                plantText.add(csdArray[lineNumbers[0]+i]);
                i=-100;
            }
    }
    String plantDir = appProperties->getUserSettings()->getValue("PlantFileDir", "");
    String fileName = plantDir+"/"+entryName+".plant";
    File file(fileName);
    file.replaceWithText(plantText.joinIntoString("\n"));
#endif
}


//=======================================================
//takes a bunch of selected components and groups them together into a plant.
//it firsts records the lines which the components appear on and saves them into
//a new string. Then it deletes the selected components and inserts the new plant
//text into the csd file. We have to make sure that we update the coordinates
//of the child components relative to the top/left position of the image/groupbox
//
//the method beneath it does the opposite and breaks up a plant into it's different
//components
//=======================================================
void CabbagePluginAudioProcessorEditor::convertIntoPlant()
{
#ifdef Cabbage_Build_Standalone
    StringArray csdArray;
    StringArray boundsForSelectComps;
    StringArray newPlantText;

    csdArray.addLines(getFilter()->getCsoundInputFileText());
    ComponentLayoutEditor* le = layoutEditor;

    populateLineNumberArray(csdArray);

    for(int i=0; i<lineNumbers.size(); i++)
    {
        csdArray.set(lineNumbers[i], replaceIdentifier(csdArray[lineNumbers[i]], "bounds", boundsForSelectComps[i]));
        currentLineNumber = lineNumbers[0];
    }

    for(int i=0; i<lineNumbers.size(); i++)
        newPlantText.add(csdArray[lineNumbers[i]]);

    if(newPlantText.joinIntoString("\n").contains(" plant("))
        showMessage("Illegal Operation:\nThis group of objects already contains a plant", &this->getLookAndFeel());
    else
    {
        //Logger::writeToLog(newPlantText.joinIntoString("\n"));
        deleteComponents();

        //find area of image needed to hold plant
        juce::Rectangle<int> bounds(9999, 9999, -100, -100);
        for(int i=0; i<newPlantText.size(); i++)
        {
            //get left most point
            CabbageGUIClass cAttr(newPlantText[i], -99);
            if(cAttr.getBounds().getX()<bounds.getX())
                bounds.setX(cAttr.getBounds().getX());
        }

        for(int i=0; i<newPlantText.size(); i++)
        {
            //get right most point
            CabbageGUIClass cAttr(newPlantText[i], -99);
            if(cAttr.getBounds().getY()<bounds.getY())
                bounds.setY(cAttr.getBounds().getY());
        }

        for(int i=0; i<newPlantText.size(); i++)
        {
            //get width
            CabbageGUIClass cAttr(newPlantText[i], -99);
            if(cAttr.getBounds().getWidth()+cAttr.getBounds().getX()>bounds.getX()+bounds.getWidth())
                bounds.setWidth(cAttr.getBounds().getWidth()+cAttr.getBounds().getX()-bounds.getX());
        }

        for(int i=0; i<newPlantText.size(); i++)
        {
            //get height
            CabbageGUIClass cAttr(newPlantText[i], -99);
            if(cAttr.getBounds().getHeight()+cAttr.getBounds().getY()>bounds.getY()+bounds.getHeight())
                bounds.setHeight(cAttr.getBounds().getHeight()+cAttr.getBounds().getY()-bounds.getY());
        }
        //Logger::writeToLog(getBoundsString(bounds));
        //now that we have our image bounds, we need to change the child coordinates relative
        //to the bounds top/left position

        for(int i=0; i<newPlantText.size(); i++)
        {
            CabbageGUIClass cAttr(newPlantText[i], -99);
            juce::Rectangle<int> newBounds(cAttr.getNumProp(CabbageIDs::left)-bounds.getX(), cAttr.getNumProp(CabbageIDs::top)-bounds.getY(), cAttr.getNumProp(CabbageIDs::width), cAttr.getNumProp(CabbageIDs::height));
            newPlantText.getReference(i) = replaceIdentifier(newPlantText[i], "bounds", getBoundsString(newBounds));
        }

        for(int i=0; i<newPlantText.size(); i++)
            if(newPlantText[i].contains("image ") || newPlantText[i].contains("groupbox "))
                newPlantText.move(i, 0);



        newPlantText.insert(0, "image "+getBoundsString(bounds)+", colour(255, 255, 255, 0), plant(\"newPlant\"){");
        newPlantText.add("}");
        insertComponentsFromCabbageText(newPlantText, false);

    }
#endif
}

//===================================================================
//break plant up into different components
//===================================================================
void CabbagePluginAudioProcessorEditor::breakUpPlant()
{
#ifdef Cabbage_Build_Standalone
    StringArray csdArray;
    StringArray brokenPlant;
    csdArray.addLines(getFilter()->getCsoundInputFileText());
    ComponentLayoutEditor* le = layoutEditor;

    populateLineNumberArray(csdArray);

    Point<int> plantPos;
    for(int i=0; i<lineNumbers.size(); i++)
    {
        if(csdArray[lineNumbers[i]].contains(" plant("))
        {
            CabbageGUIClass cAttr(csdArray[lineNumbers[i]], -99);
            plantPos.setX(cAttr.getBounds().getX());
            plantPos.setY(cAttr.getBounds().getY());
            //get individual components
            for(int y=1; y>-1; y++)
            {
                brokenPlant.add(csdArray[lineNumbers[i]+y]);
                if(csdArray[lineNumbers[i]+y].contains("}"))
                {
                    y=-100;
                    i=lineNumbers.size()+1;
                }
            }
        }
    }
    brokenPlant.remove(brokenPlant.size()-1);
    //Logger::writeToLog(brokenPlant.joinIntoString("\n"));

    for(int i=0; i<brokenPlant.size(); i++)
    {
        //reposition all controls relative to the plant position
        CabbageGUIClass cAttr(brokenPlant[i], -99);
        juce::Rectangle <int> bounds(cAttr.getNumProp(CabbageIDs::left)+plantPos.getX(),
                               cAttr.getNumProp(CabbageIDs::top)+plantPos.getY(),
                               cAttr.getNumProp(CabbageIDs::width),
                               cAttr.getNumProp(CabbageIDs::height));
        brokenPlant.getReference(i) = replaceIdentifier(brokenPlant[i], "bounds", getBoundsString(bounds));
    }
    //Logger::writeToLog(brokenPlant.joinIntoString("\n"));
    deleteComponents();
    this->insertComponentsFromCabbageText(brokenPlant, false);
#endif
}

//===========================================================
//populate lineNumbers array with the number of lines selected
//===========================================================
void CabbagePluginAudioProcessorEditor::populateLineNumberArray(StringArray csdArray)
{
#ifdef Cabbage_Build_Standalone
    lineNumbers.clear();
    for(int i=0; i<csdArray.size(); i++)
    {
        CabbageGUIClass CAttr(csdArray[i], i-99);
        if(csdArray[i].contains("</Cabbage>"))
            i=csdArray.size();

        int numSelected = layoutEditor->selectedCompsOrigCoordinates.size();

        for(int y=0; y<numSelected; y++)
        {
            if(layoutEditor->selectedCompsOrigCoordinates[y]==CAttr.getComponentBounds())
            {
                lineNumbers.add(i);
            }
        }
    }
#endif
}

//========================================================
//deletes any selected Components and removes text decs from csd file
//care is taken here to first delete the instances of each GUI component
//from the component vector, and then to update the abstract vector held by our
//main filter. When components are deleted the csd file must also be updated..
//========================================================
void CabbagePluginAudioProcessorEditor::deleteComponents()
{
#ifdef Cabbage_Build_Standalone
    StringArray csdArray;
    StringArray boundsForSelectComps;
    csdArray.addLines(getFilter()->getCsoundInputFileText());
    ComponentLayoutEditor* le = layoutEditor;

    populateLineNumberArray(csdArray);

    currentLineNumber = lineNumbers[0];


    //deselect all the items before deleting them..
    le->getLassoSelection().deselectAll();

    //now that the line numbers have been located, remove GUI comps from relevant vectors
    for(int i=0; i<lineNumbers.size(); i++)
    {
        CabbageGUIClass cAttr(csdArray[lineNumbers[i]], -99);
        if(cAttr.getStringProp(CabbageIDs::basetype).contains("interactive"))
        {
            //first remove component from interactive comps vector..
            for(int y=0; y<comps.size(); y++)
                if(cAttr.getBounds()==comps[y]->getBounds())
                    comps.remove(y);

            //then remove abstract instance of GUI components
            for(int y=0; y<getFilter()->getGUICtrlsSize(); y++)
                if(cAttr.getBounds()==getFilter()->getGUICtrls(y).getBounds())
                    getFilter()->removeGUIComponent(y, "interactive");

        }
        else
        {
            //now remove component from layout comps vector..
            for(int y=0; y<layoutComps.size(); y++)
                if(cAttr.getBounds()==layoutComps[y]->getBounds())
                    layoutComps.remove(y);

            //then remove abstract instance of GUI components
            for(int y=0; y<getFilter()->getGUILayoutCtrlsSize(); y++)
                if(cAttr.getBounds()==getFilter()->getGUILayoutCtrls(y).getBounds())
                    getFilter()->removeGUIComponent(y, "layout");

        }
    }

    //now remove and update lines from csd file in editor
    StringArray plantDefs;
    if(lineNumbers.size()>1)
        for(int i=0, y=0; i<lineNumbers.size(); i++)
        {
            //if more than one comp is selected remove each line in sequence
            if(!csdArray[lineNumbers[i]-y].contains("plant("))
            {

                csdArray.remove(lineNumbers[i]-y);
                y++;
            }
            else
                plantDefs.add(csdArray[lineNumbers[i]-y]);
        }
    else
    {
        if(!csdArray[currentLineNumber].contains("plant("))
            csdArray.remove(currentLineNumber);
        else
            plantDefs.add(csdArray[currentLineNumber]);
    }

    //if there are plants, delete them
    for(int i=0; i<plantDefs.size(); i++)
    {
        for(int y=0; y<csdArray.size(); y++)
        {
            if(csdArray[y]==plantDefs[i])
            {
                while(!csdArray[y].contains("}"))
                {
                    csdArray.remove(y);
                }
                //remove the final closing bracket
                csdArray.remove(y);
            }

        }
    }

    plantDefs.clear();
    getFilter()->updateCsoundFile(csdArray.joinIntoString("\n"));

    getFilter()->createGUI(csdArray.joinIntoString("\n"), true);


    //getFilter()->sendActionMessage("GUI Updated, controls deleted");
    //update frames to reflect changes made to GUI
    this->updateLayoutEditorFrames();
    updateLayoutEditorFrames();
    //close props windows after deleting object
    propsWindow->setVisible(false);
#endif
}

//========================================================
//duplicates and selected Components
//========================================================
void CabbagePluginAudioProcessorEditor::duplicateComponents()
{
#ifdef Cabbage_Build_Standalone
    StringArray csdArray;
    StringArray boundsForSelectComps;
    lineNumbers.clear();
    csdArray.addLines(getFilter()->getCsoundInputFileText());
    ComponentLayoutEditor* le = layoutEditor;

    StringArray duplicatedControls;
    StringArray duplicatedPlants;
    plantLineNumbers.clear();
    int linesToSkip = 0;
    //following code will duplicate selected components and offset their positions
    for(int i=0; i<csdArray.size(); i++)
    {
        CabbageGUIClass CAttr(csdArray[i], i-99);
        if(csdArray[i].contains("</Cabbage>"))
            i=csdArray.size();

        int numSelected = le->selectedCompsOrigCoordinates.size();
        if(CAttr.getBounds().getWidth()>0)
            for(int y=0; y<numSelected; y++)
            {
                //Logger::writeToLog("Selected:"+getBoundsString(le->selectedCompsOrigCoordinates[y]));
                //Logger::writeToLog("From code:"+getBoundsString(CAttr.getBounds()));

                if(le->selectedCompsOrigCoordinates[y]==CAttr.getBounds())
                {
                    //if we dealing with a plant, let if be known!
                    if(csdArray[i].contains("plant("))
                    {
                        for(int u=0; u>-1; u++)
                            if(csdArray[i+u].contains("}"))
                            {
                                plantLineNumbers.add(i+u);
                                u=-100;
                            }
                            else
                            {
                                plantLineNumbers.add(i+u);
                                linesToSkip++;
                            }
                        i=i+linesToSkip;
                        linesToSkip = 0;
                    }
                    else
                        lineNumbers.add(i);
                }
            }
    }

    le->getLassoSelection().deselectAll();

    for(int i=0; i<lineNumbers.size(); i++)
    {
        duplicatedControls.add(csdArray[lineNumbers[i]]);
    }
    //insert new objects based on the duplicated ones from above
    if(duplicatedControls.size())
        insertComponentsFromCabbageText(duplicatedControls, true);
    duplicatedControls.clear();

    //now set it up for plants too...
    for(int i=0; i<plantLineNumbers.size(); i++)
    {
        duplicatedPlants.add(csdArray[plantLineNumbers[i]]);
    }
    //insert new plants based on the duplicated ones from above
    if(duplicatedPlants.size())
        insertComponentsFromCabbageText(duplicatedPlants, true);
    duplicatedPlants.clear();

    //clear array containing bounds for duplicated controls
    layoutEditor->boundsForDuplicatedCtrls.clear();
    getFilter()->sendActionMessage("GUI Updated, controls added, resized");
#endif
}

//========================================================
//updates the sizes and positions of any currently selected componenets
//in our csd file
//========================================================
void CabbagePluginAudioProcessorEditor::updateSizesAndPositionsOfComponents(int newLine)
{
    newLine = 0;
#ifdef Cabbage_Build_Standalone
    StringArray csdArray;
    StringArray boundsForSelectComps;
    lineNumbers.clear();
    csdArray.addLines(getFilter()->getCsoundInputFileText());
    ComponentLayoutEditor* le = layoutEditor;

    lineNumbers.clear();
    int numSelected = le->selectedLineNumbers.size();
    for(int y=0; y<numSelected; y++)
    {
        //if(CAttr.getStringProp(CabbageIDs::type)=="groupbox")
        //Logger::writeToLog("SelectedCompBounds:"+getBoundsString(le->selectedCompsOrigCoordinates[y]));
        lineNumbers.add(le->selectedLineNumbers[y]);
        boundsForSelectComps.add(getBoundsString(le->selectedCompsNewCoordinates[y]));
    }

    //Logger::writeToLog(String(lineNumbers.size()));
    for(int i=0; i<lineNumbers.size(); i++)
    {
        csdArray.set(lineNumbers[i], replaceIdentifier(csdArray[lineNumbers[i]], "bounds", boundsForSelectComps[i]));
        currentLineNumber = lineNumbers[0];
    }

    //resize all elements that are contained in a plant, but only
    //if it is a non-popup plant
    String tempPlantText, temp;
    int endLine;
    if((csdArray[currentLineNumber].contains("plant(\"")) && (!csdArray[currentLineNumber].contains("popup(1)")))
    {
        tempPlantText=csdArray[currentLineNumber]+"\n";
        for(int y=1, off=0; y<componentPanel->childBounds.size()+1; y++)
        {
            //stops things from getting messed up if there are line
            if((csdArray[currentLineNumber+y+off].length()<2) || csdArray[currentLineNumber+y+off].indexOf(";")==0)
            {
                off++;
                y--;
            }
            temp = replaceIdentifier(csdArray[currentLineNumber+y+off], "bounds", getBoundsString(componentPanel->childBounds[y-1]));
            csdArray.set(currentLineNumber+y+off, temp);
            //add last curly brace
            endLine = currentLineNumber+y+off;
            tempPlantText = tempPlantText+temp+"\n";
            // csdArray.set(currentLineNumber+y, tempArray[y-1]);//.replace("bounds()", componentPanel->getCurrentChildBounds(y-1), true));
        }
        tempPlantText = tempPlantText+csdArray[endLine+1]+"\n";
        //Logger::writeToLog(tempPlantText);
    }

    CabbageGUIClass cAttr(csdArray[currentLineNumber], -99);
    propsWindow->updateProps(cAttr);
    propsWindow->setVisible(true);
    propsWindow->toFront(true);
    getFilter()->updateCsoundFile(csdArray.joinIntoString("\n"));
    getFilter()->highlightLine(csdArray[currentLineNumber+newLine]);
    //getFilter()->setGuiEnabled(true);
    //getFilter()->setCurrentLineText(csdArray[currentLineNumber+newLine]);
    //getFilter()->sendActionMessage("GUI Updated, controls added, resized");
    lineNumbers.clear();
#endif
}

//==============================================================================
// these functions update filter info with the current state of a mouse
//=============================================================================
void CabbagePluginAudioProcessorEditor::mouseMove(const MouseEvent& event)
{
	//look after mouse message from main plant windows and main window
	int x = event.eventComponent->getTopLevelComponent()->getMouseXYRelative().x;
	int y = event.eventComponent->getTopLevelComponent()->getMouseXYRelative().y;
	
	if(dynamic_cast<CabbagePlantWindow*>(event.eventComponent->getTopLevelComponent()))
	{
		getFilter()->messageQueue.addOutgoingChannelMessageToQueue(CabbageIDs::mousex, x, "float");
		getFilter()->messageQueue.addOutgoingChannelMessageToQueue(CabbageIDs::mousey, jmax(0, y-18), "float");
	}
	else 
	{
		getFilter()->messageQueue.addOutgoingChannelMessageToQueue(CabbageIDs::mousex, x, "float");
		getFilter()->messageQueue.addOutgoingChannelMessageToQueue(CabbageIDs::mousey, y-28, "float");	
	}
}

static void popupMenuCallback(int result, CabbagePluginAudioProcessorEditor* editor)
{
	if(result>0)
		editor->getFilter()->messageQueue.addOutgoingChannelMessageToQueue(
			editor->getFilter()->getGUILayoutCtrls(editor->currentPopupIndex).getStringProp(CabbageIDs::channel),
			result,	"popup");
}

void CabbagePluginAudioProcessorEditor::mouseDown(const MouseEvent& event)
{
    if(event.mods.isPopupMenu())
    {
        for(int i=0; i<popupMenus.size(); i++)
        {
            if(getFilter()->getGUILayoutCtrls(popupMenus[i]).getBounds().contains(event.getEventRelativeTo(this).getPosition())
                    && getFilter()->getGUILayoutCtrls(popupMenus[i]).getStringProp("reltoplant").isEmpty())
            {
                currentPopupIndex = popupMenus[i];
                layoutComps[popupMenus[i]]->setLookAndFeel(lookAndFeel);
                PopupMenu m;
				
                m.setLookAndFeel(lookAndFeel);
                static_cast<CabbagePopupMenu*>(layoutComps[currentPopupIndex])->addItemsToPopup(m);
                m.showMenuAsync(PopupMenu::Options(), ModalCallbackFunction::forComponent(popupMenuCallback, this));
            }

        }
    } 
    if(event.mods.isLeftButtonDown())
        getFilter()->messageQueue.addOutgoingChannelMessageToQueue(CabbageIDs::mousedownleft, 1, "");
    else if(event.mods.isRightButtonDown())
        getFilter()->messageQueue.addOutgoingChannelMessageToQueue(CabbageIDs::mousedownright, 1, "");
    else if(event.mods.isMiddleButtonDown())
        getFilter()->messageQueue.addOutgoingChannelMessageToQueue(CabbageIDs::mousedownlmiddle, 1, "");

}

void CabbagePluginAudioProcessorEditor::mouseUp(const MouseEvent& event)
{
    if(event.mods.isLeftButtonDown())
        getFilter()->messageQueue.addOutgoingChannelMessageToQueue(CabbageIDs::mousedownleft, 0, "");
    else if(event.mods.isRightButtonDown())
        getFilter()->messageQueue.addOutgoingChannelMessageToQueue(CabbageIDs::mousedownright, 0, "");
    else if(event.mods.isMiddleButtonDown())
        getFilter()->messageQueue.addOutgoingChannelMessageToQueue(CabbageIDs::mousedownlmiddle, 0, "");
}
//==============================================================================
// this function will display a context menu on right mouse click. The menu
// is populated by all a list of all GUI controls and GUI abstractions stored in the CabbagePlant folder.
// Users can create their own GUI abstraction at any time, save them to this folder, and
// insert them to their instrument whenever they like
//==============================================================================
void CabbagePluginAudioProcessorEditor::showInsertControlsMenu(int x, int y)
{
#ifdef Cabbage_Build_Standalone
    ScopedPointer<XmlElement> xml;
    xml = new XmlElement("PLANTS");
    PopupMenu m;
    Array<File> plantFiles;
    m.setLookAndFeel(lookAndFeel);
    if(getFilter()->isGuiEnabled())
    {
        PopupMenu subm;
        subm.setLookAndFeel(&this->getLookAndFeel());
        subm.addItem(1, "button");
        subm.addItem(2, "rslider");
        subm.addItem(3, "vslider");
        subm.addItem(4, "hslider");
        subm.addItem(5, "combobox");
        subm.addItem(6, "checkbox");
        subm.addItem(7, "groupbox");
        subm.addItem(8, "image");
        subm.addItem(9, "keyboard");
        subm.addItem(10, "xypad");
        subm.addItem(11, "label");
        subm.addItem(16, "filebutton");
        subm.addItem(17, "numberbox");
		subm.addItem(18, "texteditor");
		subm.addItem(19, "textbox");
//subm.addItem(13, "soundfiler");
        subm.addItem(14, "table");
        subm.addItem(15, "Csound message console");
        m.addSubMenu(String("Indigenous"), subm);
        subm.clear();

        String plantDir = appProperties->getUserSettings()->getValue("PlantFileDir", "");
        addFilesToPopupMenu(subm, plantFiles, plantDir, "*.plant", 100);
        m.addSubMenu(String("Homegrown"), subm);
    }

    int channelOffset = getFilter()->getGUICtrlsSize();

    int choice;
#if !defined(AndroidBuild)
	choice = m.show();
#endif
    if(choice==1)
        insertComponentsFromCabbageText(StringArray(String("button bounds(")+String(x)+(", ")+String(y)+String(", 60, 25), channel(\"but1\"), text(\"on\", \"off\")")), false);
    else if(choice==2)
        insertComponentsFromCabbageText(StringArray(String("rslider bounds(")+String(x)+(", ")+String(y)+String(", 50, 50), channel(\"rslider\"), range(0, 100, 0)")), false);
    else if(choice==3)
        insertComponentsFromCabbageText(StringArray(String("vslider bounds(")+String(x)+(", ")+String(y)+String(", 30, 200), channel(\"vslider\"), range(0, 100, 0), colour(\"white\")")), false);
    else if(choice==4)
        insertComponentsFromCabbageText(StringArray(String("hslider bounds(")+String(x)+(", ")+String(y)+String(", 200, 30), channel(\"hslider\"), range(0, 100, 0), colour(\"white\")")), false);
    else if(choice==5)
        insertComponentsFromCabbageText(StringArray(String("combobox bounds(")+String(x)+(", ")+String(y)+String(", 100, 30), channel(\"combobox\"), items(\"Item 1\", \"Item 2\", \"Item 3\")")), false);
    else if(choice==6)
        insertComponentsFromCabbageText(StringArray(String("checkbox bounds(")+String(x)+(", ")+String(y)+String(", 80, 20), channel(\"checkbox\"), text(\"checkbox\")")), false);
    else if(choice==7)
        insertComponentsFromCabbageText(StringArray(String("groupbox bounds(")+String(x)+(", ")+String(y)+String(", 200, 150), text(\"groupbox\"), colour(\"black\"), caption(\"groupbBox\")")), false);
    else if(choice==8)
        insertComponentsFromCabbageText(StringArray(String("image bounds(")+String(x)+(", ")+String(y)+String(", 200, 150), colour(\"white\"), line(0)")), false);
    else if(choice==9)
        insertComponentsFromCabbageText(StringArray(String("keyboard bounds(")+String(x)+(", ")+String(y)+String(", 150, 60)")), false);
    else if(choice==10)
        insertComponentsFromCabbageText(StringArray(String("xypad bounds(")+String(x)+(", ")+String(y)+String(", 200, 200), channel(\"xchan")+String(channelOffset)+("\", \"ychan")+String(channelOffset)+("\"), rangex(0, 100, 0), rangey(0, 100, 0)")), false);
    else if(choice==11)
        insertComponentsFromCabbageText(StringArray(String("label bounds(")+String(x)+(", ")+String(y)+String(", 50, 15), text(\"Label\"), fontcolour(\"white\")")), false);
    else if(choice==12)
        insertComponentsFromCabbageText(StringArray(String("infobutton bounds(")+String(x)+(", ")+String(y)+String(", 60, 25), text(\"Info\"), file(\"info.html\")")), false);
    else if(choice==16)
        insertComponentsFromCabbageText(StringArray(String("filebutton bounds(")+String(x)+(", ")+String(y)+String(", 60, 25), text(\"File\")")), false);
    else if(choice==13)
        insertComponentsFromCabbageText(StringArray(String("soundfiler bounds(")+String(x)+(", ")+String(y)+String(", 360, 160)")), false);
    else if(choice==14)
        insertComponentsFromCabbageText(StringArray(String("table bounds(")+String(x)+(", ")+String(y)+String(", 260, 160)")), false);
    else if(choice==15)
        insertComponentsFromCabbageText(StringArray(String("csoundoutput bounds(")+String(x)+(", ")+String(y)+String(", 360, 200)")), false);
    else if(choice==17)
        insertComponentsFromCabbageText(StringArray(String("numberbox bounds(")+String(x)+(", ")+String(y)+String(", 40, 20), channel(\"numberbox\"), range(0, 100, 0), colour(\"white\")")), false);
    else if(choice==18)
        insertComponentsFromCabbageText(StringArray(String("texteditor bounds(")+String(x)+(", ")+String(y)+String(", 40, 20), channel(\"texteditor\"), colour(\"white\")")), false);
    else if(choice==19)
        insertComponentsFromCabbageText(StringArray(String("textbox bounds(")+String(x)+(", ")+String(y)+String(", 140, 80), colour(\"white\")")), false);


    else if(choice>=100)
    {
        //showMessage(xml->getAttributeValue(choice-100));
        //update X and Y positions from plants
        String customPlantControl = plantFiles[choice-100].loadFileAsString();
        StringArray userPlant;
        userPlant.addLines(customPlantControl);
        CabbageGUIClass cAttr(userPlant[0], -99);
        juce::Rectangle <int> bounds(x, y, cAttr.getBounds().getWidth(), cAttr.getBounds().getHeight());

        userPlant.getReference(0) = replaceIdentifier(userPlant[0], "bounds", getBoundsString(bounds));
        insertComponentsFromCabbageText(userPlant, false);

    }

//add new lines to text..
//updateSizesAndPositionsOfComponents(1);
#endif
}

void CabbagePluginAudioProcessorEditor::setEditMode(bool on)
{
#ifdef Cabbage_Build_Standalone
    if(on)
    {
        Logger::writeToLog(CabbageUtils::getBoundsString(layoutEditor->getBounds()));
        getFilter()->setGuiEnabled(true);;
        componentPanel->toBack();
        layoutEditor->setEnabled(true);
        layoutEditor->updateFrames();
        layoutEditor->toFront(true);
    }
    else
    {
        layoutEditor->setEnabled(false);
        componentPanel->toFront(true);
        componentPanel->setInterceptsMouseClicks(false, true);
        getFilter()->setGuiEnabled(false);
    }
    resized();
#endif
}

//==============================================================================
// this function will postion component
//==============================================================================
void CabbagePluginAudioProcessorEditor::setPositionOfComponent(float left, float top, float width, float height, Component* comp, String reltoplant)
{
    String type;
    if(comp->getName().contains("rslider"))
        type = "rslider";

    if(width+left>componentPanel->getWidth() && reltoplant.isEmpty())
    {
        componentPanel->setBounds(0, 0, width+left, componentPanel->getHeight());
        viewportComponent->setBounds(0, 0, width+left, componentPanel->getHeight());
#ifdef Cabbage_Build_Stanalone
        layoutEditor->setBounds(componentPanel->getBounds());
#endif
    }
    else
    {
        viewportComponent->setBounds(0, 0, componentPanel->getWidth(), componentPanel->getHeight());
#ifdef Cabbage_Build_Stanalone
        layoutEditor->setBounds(0, 0, componentPanel->getWidth(), componentPanel->getHeight());
#endif
    }

    if(top+height>componentPanel->getHeight() && reltoplant.isEmpty())
    {
        componentPanel->setBounds(0, 0, componentPanel->getWidth(), top+height);
        viewportComponent->setBounds(0, 0, componentPanel->getWidth(), top+height);
#ifdef Cabbage_Build_Stanalone
        layoutEditor->setBounds(0, 0, componentPanel->getWidth(), componentPanel->getHeight());
#endif
    }
    else
    {
        viewportComponent->setBounds(0, 0, componentPanel->getWidth(), componentPanel->getHeight());
#ifdef Cabbage_Build_Stanalone
        layoutEditor->setBounds(0, 0, componentPanel->getWidth(), componentPanel->getHeight());
#endif
    }

    componentPanel->setTopLeftPosition(0, 0);
#ifdef Cabbage_Build_Stanalone
    layoutEditor->setTopLeftPosition(0, 0);
#endif

    if(layoutComps.size()>0)
    {
        for(int y=0; y<layoutComps.size(); y++)
            if(reltoplant.isNotEmpty())
            {
                if(layoutComps[y]->getProperties().getWithDefault(String("plant"), -99).toString().equalsIgnoreCase(reltoplant))
                {
                    positionComponentWithinPlant(type, left, top, width, height, layoutComps[y], comp);
                }
            }
            else
            {
                comp->setBounds(left, top, width, height);
                componentPanel->addAndMakeVisible(comp);
            }
    }
    else
    {
        comp->setBounds(left, top, width, height);
        componentPanel->addAndMakeVisible(comp);
    }

}

void CabbagePluginAudioProcessorEditor::positionComponentWithinPlant(String type,
        float left,
        float top,
        float width,
        float height,
        Component *layout,
        Component *control)
{
//if dimensions are < 1 then the user is using the decimal proportional of positioning
    if(width<=1 && height<=1)
    {
        width = (width>1 ? .5 : width*layout->getWidth());
        height = (height>1 ? .5 : height*layout->getHeight());
        top = (top*layout->getHeight());
        left = (left*layout->getWidth());
    }
//rotary sliders should have the same width and height...
    if(type.equalsIgnoreCase("rslider"))
        if(width<height) height = width;
        else if(height<width) width = height;

    if(layout->getName().containsIgnoreCase("groupbox")||layout->getName().containsIgnoreCase("image"))
    {
        control->setBounds(left, top, width, height);
        if(layout->getName().containsIgnoreCase("groupbox"))
            control->toBack();
        layout->addAndMakeVisible(control);
		//showMessage(String(layoutComps[idx]->getNumChildComponents()));
		control->addMouseListener(this, false);
    }

}



//==============================================================================
// the following methods dynamically update the GUI editor based on
// an StringArray of cabbage text
//==============================================================================
void CabbagePluginAudioProcessorEditor::insertComponentsFromCabbageText(StringArray text, bool useOffset)
{
#ifdef Cabbage_Build_Standalone
//offset the new object by a random value each time so they don't overlap exactly
    int offset;
    if(useOffset)
        offset = 30+Random().nextInt(40);
    else
        offset = 0;
    StringArray csdArray;
    int currentLine, numberPlantsToBeDuplicated=0;
    csdArray.addLines(getFilter()->getCsoundInputFileText());
//plants and normal components get passed to this routine individually.
//the text array will either be made up of plants, or not. Check for plants first..
    for(int i=0; i<text.size(); i++)
        if(text.joinIntoString("\n").contains("plant("))
        {
            if(text[i].contains(" plant("))
            {
                //we need to dynamically rename any plants placed onto the form
                CabbageGUIClass cAttr(text[i], i-99);
                //if duplicated plants, give them each a unique ID
                String plantName = "GUIabst_"+String(getFilter()->getGUILayoutCtrlsSize()+numberPlantsToBeDuplicated);
                numberPlantsToBeDuplicated++;
                text.getReference(i) = replaceIdentifier(text[i], "plant", "plant(\""+plantName+"\")");
                juce::Rectangle<int> bounds(cAttr.getNumProp(CabbageIDs::left)+offset, cAttr.getNumProp(CabbageIDs::top)+offset, cAttr.getNumProp(CabbageIDs::width), cAttr.getNumProp(CabbageIDs::height));
                text.getReference(i) = replaceIdentifier(text[i], "bounds", getBoundsString(bounds));
                layoutEditor->boundsForDuplicatedCtrls.add(bounds);
            }
        }
        else
        {
            CabbageGUIClass cAttr(text[i], i-99);
            juce::Rectangle<int> bounds(cAttr.getNumProp(CabbageIDs::left)+offset, cAttr.getNumProp(CabbageIDs::top)+offset, cAttr.getNumProp(CabbageIDs::width), cAttr.getNumProp(CabbageIDs::height));
            text.getReference(i) = replaceIdentifier(text[i], "bounds", getBoundsString(bounds));
            layoutEditor->boundsForDuplicatedCtrls.add(bounds);
        }

    String currentText;
    for(int i=0; i<csdArray.size(); i++)
        //always insert text on the last line of GUI code...
        if(csdArray[i].containsIgnoreCase("</Cabbage>"))
        {
            csdArray.insert(i, text.joinIntoString("\n"));
            currentLineNumber = i;
            currentText = csdArray[i];
            i=csdArray.size();
        }

    //create GUI for new components....
    //getFilter()->removeXYAutomaters();
    getFilter()->setHaveXYAutoBeenCreated(false);
    //showMessage(text.joinIntoString("\n"));

    if(text.size()==1)
        layoutEditor->selectedFilters.deselectAll();
    getFilter()->updateCsoundFile(csdArray.joinIntoString("\n"));
    getFilter()->highlightLine(currentText);
    //currentLineNumber = getFilter()->getCurrentLine();
    getFilter()->createGUI(text.joinIntoString("\n"), false);
    CabbageGUIClass cAttr(currentText, -99);
    propsWindow->updateProps(cAttr);
    getFilter()->sendActionMessage("GUI Updated, controls added, resized");


    updateLayoutEditorFrames();
    layoutEditor->selectDuplicatedComponents(layoutEditor->boundsForDuplicatedCtrls);

#endif
}

//==============================================================================
void CabbagePluginAudioProcessorEditor::paint (Graphics& g)
{

    for(int i=0; i<getFilter()->getGUILayoutCtrlsSize(); i++)
    {
        if(getFilter()->getGUILayoutCtrls(i).getStringProp(CabbageIDs::type).equalsIgnoreCase("keyboard"))
        {
#ifdef Cabbage_Build_Standalone
            if(keyIsPressed)
                if(isMouseOver(true))
                {
                    //this lets controls keep focus even when you are playing the keyboard
                    //layoutComps[i]->setWantsKeyboardFocus(true);
                    //layoutComps[i]->grabKeyboardFocus();
                    //layoutComps[i]->toFront(true);
                }
#endif
        }
    }
#ifdef Cabbage_Build_Standalone
    if(getFilter()->compiledOk())
    {
        g.setColour (Colours::black);
        //g.setColour (CabbageUtils::getBackgroundSkin());
        g.fillAll();

        Image logo = ImageCache::getFromMemory (BinaryData::logo_cabbage_Black_png, BinaryData::logo_cabbage_Black_pngSize);
        g.drawImage(logo, 10, 10, getWidth(), getHeight()-60, 0, 0, logo.getWidth(), logo.getHeight());

    }
    else
    {
        g.setColour(formColour);
        g.fillAll();

        g.setColour (CabbageUtils::getTitleFontColour().withAlpha(.3f));
        Image logo = ImageCache::getFromMemory (BinaryData::cabbageLogoHBlueText_png, BinaryData::cabbageLogoHBlueText_pngSize);
        g.drawImage (logo, getWidth() - 100, getHeight()-35, logo.getWidth()*0.55, logo.getHeight()*0.55,
                     0, 0, logo.getWidth(), logo.getHeight(), true);
        g.setColour(fontColour);
        g.drawFittedText(authorText, 10, getHeight()-35, getWidth()*.65, logo.getHeight(), 1, 1);
        //g.drawLine(10, getHeight()-27, getWidth()-10, getHeight()-27, 0.2);
    }

#else
    g.setColour(formColour);
    g.fillAll();
    g.setColour (CabbageUtils::getTitleFontColour());
#ifndef Cabbage_Plugin_Host
    Image logo = ImageCache::getFromMemory (BinaryData::cabbageLogoHBlueText_png, BinaryData::cabbageLogoHBlueText_pngSize);
    g.drawImage (logo, getWidth() - 100, getHeight()-35, logo.getWidth()*0.55, logo.getHeight()*0.55,
                 0, 0, logo.getWidth(), logo.getHeight(), true);
    g.setColour(fontColour);
    g.drawFittedText(authorText, 10, getHeight()-35, getWidth()*.65, logo.getHeight(), 1, 1);
#endif
#endif
}

//=======================================================================================
//      non-interactive components
//=======================================================================================
//+++++++++++++++++++++++++++++++++++++++++++
//                                      groupbox
//+++++++++++++++++++++++++++++++++++++++++++
void CabbagePluginAudioProcessorEditor::InsertGroupBox(CabbageGUIClass &cAttr)
{
    layoutComps.add(new CabbageGroupbox(cAttr));

    int idx = layoutComps.size()-1;
    float left = cAttr.getNumProp(CabbageIDs::left);
    float top = cAttr.getNumProp(CabbageIDs::top);
    float width = cAttr.getNumProp(CabbageIDs::width);
    float height = cAttr.getNumProp(CabbageIDs::height);
    int relY=0,relX=0;
    if(layoutComps.size()>0)
    {
        for(int y=0; y<layoutComps.size(); y++)
            if(cAttr.getStringProp("reltoplant").length()>0)
            {
                if(layoutComps[y]->getProperties().getWithDefault(String("plant"), -99).toString().equalsIgnoreCase(cAttr.getStringProp("reltoplant")))
                {
                    positionComponentWithinPlant("", left, top, width, height, layoutComps[idx], layoutComps[idx]);
                }
            }
            else
            {
                if(cAttr.getNumProp("popup")==0)
                {
                    //Logger::writeToLog(layoutComps[idx]->getName());
                    setPositionOfComponent(left, top, width, height, layoutComps[idx], cAttr.getStringProp("reltoplant"));
                }
            }

        //if dealing with a popup plant
        if(cAttr.getNumProp("popup")==1)
        {
            layoutComps[idx]->centreWithSize(width, height);
            layoutComps[idx]->setLookAndFeel(lookAndFeel);
            subPatches.add(new CabbagePlantWindow(getFilter()->getGUILayoutCtrls(idx).getStringProp(CabbageIDs::plant), Colours::black));
            int patchIndex = subPatches.size()-1;
            subPatches[patchIndex]->setAlwaysOnTop(true);
            subPatches[patchIndex]->setTitleBarHeight(18);
            layoutComps[idx]->getProperties().set("popupPlantIndex", patchIndex);

			//if plant is to stay within the bounds of the main window...
            if(cAttr.getNumProp(CabbageIDs::child)==1)
            {
                subPatches[patchIndex]->setSize(layoutComps[idx]->getWidth(), layoutComps[idx]->getHeight()+18);
                int x = getScreenPosition().getX()+getWidth()/2-(layoutComps[idx]->getWidth()/2);
                int y = getScreenPosition().getY()+getHeight()/2-(layoutComps[idx]->getHeight()/2);
                subPatches[patchIndex]->setTopLeftPosition(x, y);
                subPatches[patchIndex]->setVisible(false);
                subPatches[patchIndex]->setMinimised(true);
                componentPanel->addChildComponent(subPatches[patchIndex]);
                componentPanel->getChildComponent(componentPanel->getNumChildComponents()-1)->toBack();
            }
            else
                subPatches[patchIndex]->centreWithSize(layoutComps[idx]->getWidth(), layoutComps[idx]->getHeight()+18);

            subPatches[patchIndex]->setContentNonOwned(layoutComps[idx], true);
			//layoutComps[idx]->addMouseListener(subPatches[patchIndex], false);
            subPatches[patchIndex]->setMinimised(true);
            subPatches[patchIndex]->setVisible(false);
            subPatches[patchIndex]->setAlwaysOnTop(false);
            subPatches[patchIndex]->addMouseListener(this, true);
        }

    }

    //if control is not part of a plant, add mouse listener
    if(cAttr.getStringProp("plant").isEmpty())
        layoutComps[idx]->addMouseListener(this, true);

    dynamic_cast<CabbageGroupbox*>(layoutComps[idx])->addChangeListener(this);

    //set visiblilty
    layoutComps[idx]->setVisible((cAttr.getNumProp(CabbageIDs::visible)==1 ? true : false));
    cAttr.setStringProp(CabbageIDs::type, "groupbox");
    layoutComps[idx]->getProperties().set(String("plant"), var(cAttr.getStringProp("plant")));
    layoutComps[idx]->getProperties().set(String("groupLine"), cAttr.getNumProp(CabbageIDs::line));
    layoutComps[idx]->getProperties().set(CabbageIDs::lineNumber, cAttr.getNumProp(CabbageIDs::lineNumber));
    layoutComps[idx]->getProperties().set(CabbageIDs::index, idx);

}

//+++++++++++++++++++++++++++++++++++++++++++
//                                      image
//+++++++++++++++++++++++++++++++++++++++++++
void CabbagePluginAudioProcessorEditor::InsertImage(CabbageGUIClass &cAttr)
{
    if((!File::isAbsolutePath(cAttr.getStringProp(CabbageIDs::file))&&(cAttr.getStringProp(CabbageIDs::file).isNotEmpty())))
    {
        String pic = returnFullPathForFile(cAttr.getStringProp(CabbageIDs::file), getFilter()->getCsoundInputFile().getParentDirectory().getFullPathName());
        cAttr.setStringProp(CabbageIDs::file, pic);
    }

    layoutComps.add(new CabbageImage(cAttr));
    int idx = layoutComps.size()-1;
    float left = cAttr.getNumProp(CabbageIDs::left);
    float top = cAttr.getNumProp(CabbageIDs::top);
    float width = cAttr.getNumProp(CabbageIDs::width);
    float height = cAttr.getNumProp(CabbageIDs::height);

    int relY=0,relX=0;
    if(layoutComps.size()>0)
    {
        for(int y=0; y<layoutComps.size(); y++)
            if(cAttr.getStringProp("reltoplant").length()>0)
            {
                if(layoutComps[y]->getProperties().getWithDefault(String("plant"), -99).toString().equalsIgnoreCase(cAttr.getStringProp("reltoplant")))
                {
                    positionComponentWithinPlant("", left, top, width, height, layoutComps[y], layoutComps[idx]);
                }
            }
            else
            {
                if(cAttr.getNumProp("popup")==0)
                {
                    //Logger::writeToLog(layoutComps[idx]->getName());
                    setPositionOfComponent(left, top, width, height, layoutComps[idx], cAttr.getStringProp("reltoplant"));
                }
            }

        if(cAttr.getNumProp("popup")==1)
        {
            layoutComps[idx]->setBounds(0, 18, width, height);
            componentPanel->addAndMakeVisible(plantButton[plantButton.size()-1]);
            layoutComps[idx]->setLookAndFeel(lookAndFeel);
            subPatches.add(new CabbagePlantWindow(getFilter()->getGUILayoutCtrls(idx).getStringProp(CabbageIDs::plant), Colours::black));
            subPatches[subPatches.size()-1]->setAlwaysOnTop(true);

            subPatches[subPatches.size()-1]->setContentNonOwned(layoutComps[idx], true);
            subPatches[subPatches.size()-1]->setTitleBarHeight(18);
            layoutComps[idx]->getProperties().set("popupPlantIndex", subPatches.size()-1);
			//if plant is to stay within the bounds of the main window...
            if(cAttr.getNumProp(CabbageIDs::child)==1)
            {
                subPatches[subPatches.size()-1]->setSize(layoutComps[idx]->getWidth(), layoutComps[idx]->getHeight()+18);
                int x = getScreenPosition().getX()+getWidth()/2-(layoutComps[idx]->getWidth()/2);
                int y = getScreenPosition().getY()+getHeight()/2-(layoutComps[idx]->getHeight()/2);
                subPatches[subPatches.size()-1]->setTopLeftPosition(x, y);
                subPatches[subPatches.size()-1]->setVisible(false);
                componentPanel->addChildComponent(subPatches[subPatches.size()-1]);
            }
            else
                subPatches[subPatches.size()-1]->centreWithSize(layoutComps[idx]->getWidth(), layoutComps[idx]->getHeight()+18);
        }

    }

    //set visiblilty
    layoutComps[idx]->setVisible((cAttr.getNumProp(CabbageIDs::visible)==1 ? true : false));

    //if control is not part of a plant, add mouse listener
    if(cAttr.getStringProp("plant").isEmpty())
        layoutComps[idx]->addMouseListener(this, true);
    dynamic_cast<CabbageImage*>(layoutComps[idx])->addChangeListener(this);
    layoutComps[idx]->getProperties().set(String("plant"), var(cAttr.getStringProp("plant")));
    layoutComps[idx]->getProperties().set(CabbageIDs::lineNumber, cAttr.getNumProp(CabbageIDs::lineNumber));
    layoutComps[idx]->getProperties().set(CabbageIDs::index, idx);
}

//+++++++++++++++++++++++++++++++++++++++++++
//                                      line separator
//+++++++++++++++++++++++++++++++++++++++++++
void CabbagePluginAudioProcessorEditor::InsertLineSeparator(CabbageGUIClass &cAttr)
{
    layoutComps.add(new CabbageLine(true, cAttr.getStringProp(CabbageIDs::colour)));
    int idx = layoutComps.size()-1;

    float left = cAttr.getNumProp(CabbageIDs::left);
    float top = cAttr.getNumProp(CabbageIDs::top);
    float width = cAttr.getNumProp(CabbageIDs::width);
    float height = cAttr.getNumProp(CabbageIDs::height);
    setPositionOfComponent(left, top, width, height, layoutComps[idx], cAttr.getStringProp("reltoplant"));
    layoutComps[idx]->getProperties().set(String("plant"), var(cAttr.getStringProp("plant")));
    layoutComps[idx]->getProperties().set(CabbageIDs::lineNumber, cAttr.getNumProp(CabbageIDs::lineNumber));
    layoutComps[idx]->getProperties().set(CabbageIDs::index, idx);
    //set visiblilty
    layoutComps[idx]->setVisible((cAttr.getNumProp(CabbageIDs::visible)==1 ? true : false));
}

//+++++++++++++++++++++++++++++++++++++++++++
//                                      transport control
//+++++++++++++++++++++++++++++++++++++++++++
void CabbagePluginAudioProcessorEditor::InsertTransport(CabbageGUIClass &cAttr)
{
    jassert(1);
}

//+++++++++++++++++++++++++++++++++++++++++++
//                                      label
//+++++++++++++++++++++++++++++++++++++++++++
void CabbagePluginAudioProcessorEditor::InsertLabel(CabbageGUIClass &cAttr)
{
    layoutComps.add(new CabbageLabel(cAttr));
    int idx = layoutComps.size()-1;

    float left = cAttr.getNumProp(CabbageIDs::left);
    float top = cAttr.getNumProp(CabbageIDs::top);
    float width = cAttr.getNumProp(CabbageIDs::width);
    float height = cAttr.getNumProp(CabbageIDs::height);
    setPositionOfComponent(left, top, width, height, layoutComps[idx], cAttr.getStringProp("reltoplant"));
    //if control is not part of a plant, add mouse listener
    if(cAttr.getStringProp("plant").isEmpty())
        layoutComps[idx]->addMouseListener(this, true);
    cAttr.setStringProp(CabbageIDs::type, "label");
    layoutComps[idx]->getProperties().set(CabbageIDs::index, idx);
    layoutComps[idx]->getProperties().set(CabbageIDs::lineNumber, cAttr.getNumProp(CabbageIDs::lineNumber));
    //set visiblilty
    layoutComps[idx]->setVisible((cAttr.getNumProp(CabbageIDs::visible)==1 ? true : false));
}

//+++++++++++++++++++++++++++++++++++++++++++
//                                      window
//+++++++++++++++++++++++++++++++++++++++++++
void CabbagePluginAudioProcessorEditor::SetupWindow(CabbageGUIClass &cAttr)
{
    setName(cAttr.getStringProp(CabbageIDs::caption));
    getFilter()->setPluginName(cAttr.getStringProp(CabbageIDs::caption));
    int left = cAttr.getNumProp(CabbageIDs::left);
    int top = cAttr.getNumProp(CabbageIDs::top);
    int width = cAttr.getNumProp(CabbageIDs::width);
    int height = cAttr.getNumProp(CabbageIDs::height);

	showScrollbars = (bool)cAttr.getNumProp(CabbageIDs::scrollbars);
    if(cAttr.getStringProp(CabbageIDs::colour).isNotEmpty())
        formColour = Colour::fromString(cAttr.getStringProp(CabbageIDs::colour));
    else
        formColour = CabbageUtils::getBackgroundSkin();

    if(cAttr.getStringProp(CabbageIDs::fontcolour).isNotEmpty())
        fontColour = Colour::fromString(cAttr.getStringProp(CabbageIDs::fontcolour));
    else
        fontColour = CabbageUtils::getComponentFontColour();
    authorText = cAttr.getStringProp("author");

#ifdef Cabbage_Build_Standalone
    viewportComponent->setBounds(0, 0, this->getWidth(), this->getHeight());
    layoutEditor->setBounds(left, top, width, height);
    formPic = getFilter()->getCsoundInputFile().getParentDirectory().getFullPathName();
#else
    File thisFile(File::getSpecialLocation(File::currentApplicationFile));
    formPic = thisFile.getParentDirectory().getFullPathName();
#endif

    setSize(width, height);
    componentPanel->setBounds(left, top, width, height);

#ifdef LINUX
    formPic.append(String("/")+String(cAttr.getStringProp(CabbageIDs::file)), 1024);
#else
    formPic.append(String("\\")+String(cAttr.getStringProp(CabbageIDs::file)), 1024);
#endif

    if(cAttr.getStringProp(CabbageIDs::file).length()<2)
        formPic = "";

    this->resized();
    //add a dummy control to our layoutComps vector so that our filters abstract layout vector
    //is the same size as our editors one.
    layoutComps.add(new Component());
    //set visiblilty
    layoutComps[layoutComps.size()-1]->setVisible((cAttr.getNumProp(CabbageIDs::visible)==1 ? true : false));
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//      Csound output widget.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CabbagePluginAudioProcessorEditor::InsertCsoundOutput(CabbageGUIClass &cAttr)
{
    layoutComps.add(new CabbageTextbox(cAttr));
    int idx = layoutComps.size()-1;
    float left = cAttr.getNumProp(CabbageIDs::left);
    float top = cAttr.getNumProp(CabbageIDs::top);
    float width = cAttr.getNumProp(CabbageIDs::width);
    float height = cAttr.getNumProp(CabbageIDs::height);
    setPositionOfComponent(left, top, width, height, layoutComps[idx], cAttr.getStringProp("reltoplant"));
    layoutComps[idx]->setName("csoundoutput");
    layoutComps[idx]->getProperties().set(String("plant"), var(cAttr.getStringProp("plant")));
    //if control is not part of a plant, add mouse listener
    if(cAttr.getStringProp("plant").isEmpty())
        layoutComps[idx]->addMouseListener(this, true);
    layoutComps[idx]->getProperties().set(CabbageIDs::lineNumber, cAttr.getNumProp(CabbageIDs::lineNumber));
    layoutComps[idx]->getProperties().set(CabbageIDs::index, idx);
    //set visiblilty
    layoutComps[idx]->setVisible((cAttr.getNumProp(CabbageIDs::visible)==1 ? true : false));
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//      Textbox widget.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CabbagePluginAudioProcessorEditor::InsertTextbox(CabbageGUIClass &cAttr)
{
    if(!File::isAbsolutePath(cAttr.getStringProp(CabbageIDs::file)))
    {
        String pic = returnFullPathForFile(cAttr.getStringProp(CabbageIDs::file), getFilter()->getCsoundInputFile().getParentDirectory().getFullPathName());
        cAttr.setStringProp(CabbageIDs::file, pic);
    }

    layoutComps.add(new CabbageTextbox(cAttr));
    int idx = layoutComps.size()-1;
    float left = cAttr.getNumProp(CabbageIDs::left);
    float top = cAttr.getNumProp(CabbageIDs::top);
    float width = cAttr.getNumProp(CabbageIDs::width);
    float height = cAttr.getNumProp(CabbageIDs::height);
    setPositionOfComponent(left, top, width, height, layoutComps[idx], cAttr.getStringProp("reltoplant"));
    layoutComps[idx]->setName("csoundoutput");
    layoutComps[idx]->getProperties().set(String("plant"), var(cAttr.getStringProp("plant")));
    //if control is not part of a plant, add mouse listener
    if(cAttr.getStringProp("plant").isEmpty())
        layoutComps[idx]->addMouseListener(this, true);
    dynamic_cast<CabbageTextbox*>(layoutComps[idx])->editor->setLookAndFeel(lookAndFeel);
    layoutComps[idx]->getProperties().set(CabbageIDs::lineNumber, cAttr.getNumProp(CabbageIDs::lineNumber));
    layoutComps[idx]->getProperties().set(CabbageIDs::index, idx);
    //set visiblilty
    layoutComps[idx]->setVisible((cAttr.getNumProp(CabbageIDs::visible)==1 ? true : false));
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//      TextEditor widget.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CabbagePluginAudioProcessorEditor::InsertTextEditor(CabbageGUIClass &cAttr)
{
    layoutComps.add(new CabbageTextEditor(cAttr));
    int idx = layoutComps.size()-1;
    float left = cAttr.getNumProp(CabbageIDs::left);
    float top = cAttr.getNumProp(CabbageIDs::top);
    float width = cAttr.getNumProp(CabbageIDs::width);
    float height = cAttr.getNumProp(CabbageIDs::height);
    setPositionOfComponent(left, top, width, height, layoutComps[idx], cAttr.getStringProp("reltoplant"));
    layoutComps[idx]->setName("csoundoutput");
    layoutComps[idx]->getProperties().set(String("plant"), var(cAttr.getStringProp("plant")));
    //if control is not part of a plant, add mouse listener
    if(cAttr.getStringProp("plant").isEmpty())
        layoutComps[idx]->addMouseListener(this, true);
    dynamic_cast<CabbageTextEditor*>(layoutComps[idx])->editor->setLookAndFeel(lookAndFeel);
    layoutComps[idx]->getProperties().set(CabbageIDs::lineNumber, cAttr.getNumProp(CabbageIDs::lineNumber));
    layoutComps[idx]->getProperties().set(CabbageIDs::index, idx);
    //set visiblilty
    layoutComps[idx]->setVisible((cAttr.getNumProp(CabbageIDs::visible)==1 ? true : false));
	//add change listener so we can inform Csound when the contents of the editor changes
	dynamic_cast<CabbageTextEditor*>(layoutComps[idx])->addChangeListener(this);
	//dynamic_cast<CabbageTextEditor*>(layoutComps[idx])->sendChangeMessage();
	
	//Logger::writeToLog(cAttr.getStringProp(CabbageIDs::channel))
	if(cAttr.getStringProp(CabbageIDs::text).isNotEmpty() && cAttr.getStringProp(CabbageIDs::channel).isNotEmpty())
		getFilter()->messageQueue.addOutgoingChannelMessageToQueue(cAttr.getStringProp(CabbageIDs::channel), 
																   cAttr.getStringProp(CabbageIDs::text), "string");
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//      Info button.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CabbagePluginAudioProcessorEditor::InsertInfoButton(CabbageGUIClass &cAttr)
{
    layoutComps.add(new CabbageButton(cAttr));
    int idx = layoutComps.size()-1;

    float left = cAttr.getNumProp(CabbageIDs::left);
    float top = cAttr.getNumProp(CabbageIDs::top);
    float width = cAttr.getNumProp(CabbageIDs::width);
    float height = cAttr.getNumProp(CabbageIDs::height);

    setPositionOfComponent(left, top, width, height, layoutComps[idx], cAttr.getStringProp("reltoplant"));
    ((CabbageButton*)layoutComps[idx])->button->setName("infobutton");
    ((CabbageButton*)layoutComps[idx])->button->getProperties().set(String("filename"), cAttr.getStringProp(CabbageIDs::file));
    layoutComps[idx]->getProperties().set(String("plant"), var(cAttr.getStringProp("plant")));
    ((CabbageButton*)layoutComps[idx])->button->addListener(this);
    ((CabbageButton*)layoutComps[idx])->button->setButtonText(cAttr.getStringProp(CabbageIDs::text));
    ((CabbageButton*)layoutComps[idx])->button->getProperties().set(String("index"), idx);

    //set visiblilty
    layoutComps[idx]->setVisible((cAttr.getNumProp(CabbageIDs::visible)==1 ? true : false));

    //if control is not part of a plant, add mouse listener
    if(cAttr.getStringProp("plant").isEmpty())
        layoutComps[idx]->addMouseListener(this, true);
    layoutComps[idx]->getProperties().set(CabbageIDs::lineNumber, cAttr.getNumProp(CabbageIDs::lineNumber));
    layoutComps[idx]->getProperties().set(CabbageIDs::index, idx);
}

//+++++++++++++++++++++++++++++++++++++++++++
//                       insert file button
//+++++++++++++++++++++++++++++++++++++++++++
void CabbagePluginAudioProcessorEditor::InsertFileButton(CabbageGUIClass &cAttr)
{
    layoutComps.add(new CabbageButton(cAttr));
    int idx = layoutComps.size()-1;

    float left = cAttr.getNumProp(CabbageIDs::left);
    float top = cAttr.getNumProp(CabbageIDs::top);
    float width = cAttr.getNumProp(CabbageIDs::width);
    float height = cAttr.getNumProp(CabbageIDs::height);
    setPositionOfComponent(left, top, width, height, layoutComps[idx], cAttr.getStringProp("reltoplant"));
    ((CabbageButton*)layoutComps[idx])->button->addListener(this);
    //((CabbageButton*)layoutComps[idx])->button->setName("button");
    if(cAttr.getStringArrayProp(CabbageIDs::text).size()>0)
        ((CabbageButton*)layoutComps[idx])->button->setButtonText(cAttr.getStringArrayPropValue("text", cAttr.getNumProp(CabbageIDs::value)));
#ifdef Cabbage_Build_Standalone
    ((CabbageButton*)layoutComps[idx])->button->setWantsKeyboardFocus(true);
#endif
    ((CabbageButton*)layoutComps[idx])->button->getProperties().set(String("index"), idx);
    //if control is not part of a plant, add mouse listener
    if(cAttr.getStringProp("reltoplant").isEmpty())
        layoutComps[idx]->addMouseListener(this, true);
    layoutComps[idx]->getProperties().set(CabbageIDs::lineNumber, cAttr.getNumProp(CabbageIDs::lineNumber));
    layoutComps[idx]->getProperties().set(CabbageIDs::index, idx);

    //set visiblilty
    layoutComps[idx]->setVisible((cAttr.getNumProp(CabbageIDs::visible)==1 ? true : false));
}

//+++++++++++++++++++++++++++++++++++++++++++
//                       insert record button
//+++++++++++++++++++++++++++++++++++++++++++
void CabbagePluginAudioProcessorEditor::InsertRecordButton(CabbageGUIClass &cAttr)
{
    layoutComps.add(new CabbageButton(cAttr));
    int idx = layoutComps.size()-1;

    float left = cAttr.getNumProp(CabbageIDs::left);
    float top = cAttr.getNumProp(CabbageIDs::top);
    float width = cAttr.getNumProp(CabbageIDs::width);
    float height = cAttr.getNumProp(CabbageIDs::height);
    setPositionOfComponent(left, top, width, height, layoutComps[idx], cAttr.getStringProp("reltoplant"));
    ((CabbageButton*)layoutComps[idx])->button->addListener(this);
    //only one record button can be use on each instrument
    ((CabbageButton*)layoutComps[idx])->button->setName("recordbutton");
    if(cAttr.getStringArrayProp(CabbageIDs::text).size()>0)
        ((CabbageButton*)layoutComps[idx])->button->setButtonText("Start Recording");
#ifdef Cabbage_Build_Standalone
    ((CabbageButton*)layoutComps[idx])->button->setWantsKeyboardFocus(true);
#endif
    ((CabbageButton*)layoutComps[idx])->button->getProperties().set(String("index"), idx);
    //if control is not part of a plant, add mouse listener
    if(cAttr.getStringProp("reltoplant").isEmpty())
        layoutComps[idx]->addMouseListener(this, true);
    layoutComps[idx]->getProperties().set(CabbageIDs::lineNumber, cAttr.getNumProp(CabbageIDs::lineNumber));
    layoutComps[idx]->getProperties().set(CabbageIDs::index, idx);
    //set visiblilty
    layoutComps[idx]->setVisible((cAttr.getNumProp(CabbageIDs::visible)==1 ? true : false));
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//     Soundfiler
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CabbagePluginAudioProcessorEditor::InsertSoundfiler(CabbageGUIClass &cAttr)
{
    float left = cAttr.getNumProp(CabbageIDs::left);
    float top = cAttr.getNumProp(CabbageIDs::top);
    float width = cAttr.getNumProp(CabbageIDs::width);
    float height = cAttr.getNumProp(CabbageIDs::height);

    layoutComps.add(new CabbageSoundfiler(cAttr));

    int idx = layoutComps.size()-1;
    setPositionOfComponent(left, top, width, height, layoutComps[idx], cAttr.getStringProp("reltoplant"));
    layoutComps[idx]->getProperties().set(String("plant"), var(cAttr.getStringProp("plant")));
    //if control is not part of a plant, add mouse listener
    if(cAttr.getStringProp("reltoplant").isEmpty())
        layoutComps[idx]->addMouseListener(this, true);
    layoutComps[idx]->getProperties().set(CabbageIDs::lineNumber, cAttr.getNumProp(CabbageIDs::lineNumber));
    layoutComps[idx]->getProperties().set(CabbageIDs::index, idx);
    ((CabbageSoundfiler*)layoutComps[idx])->addChangeListener(this);
    //set visiblilty
    layoutComps[idx]->setVisible((cAttr.getNumProp(CabbageIDs::visible)==1 ? true : false));

    //load initial files/tables if any are set
    int numberOfTables = cAttr.getStringArrayProp(CabbageIDs::tablenumber).size();
    tableBuffer.clear();
    for(int y=0; y<numberOfTables; y++)
    {
        int tableNumber = cAttr.getIntArrayPropValue(CabbageIDs::tablenumber, y);
        tableValues.clear();
        tableValues = getFilter()->getTableFloats(tableNumber);
        if(tableBuffer.getNumSamples()<tableValues.size())
            tableBuffer.setSize(numberOfTables, tableValues.size());
        tableBuffer.addFrom(y, 0, tableValues.getRawDataPointer(), tableValues.size());
    }
    dynamic_cast<CabbageSoundfiler*>(layoutComps[idx])->setWaveform(tableBuffer, numberOfTables);

}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//     GenTable
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CabbagePluginAudioProcessorEditor::InsertGenTable(CabbageGUIClass &cAttr)
{
    float left = cAttr.getNumProp(CabbageIDs::left);
    float top = cAttr.getNumProp(CabbageIDs::top);
    float width = cAttr.getNumProp(CabbageIDs::width);
    float height = cAttr.getNumProp(CabbageIDs::height);

    layoutComps.add(new CabbageGenTable(cAttr));

    int idx = layoutComps.size()-1;
    setPositionOfComponent(left, top, width, height, layoutComps[idx], cAttr.getStringProp("reltoplant"));
    layoutComps[idx]->getProperties().set(String("plant"), var(cAttr.getStringProp("plant")));
    //if control is not part of a plant, add mouse listener
    if(cAttr.getStringProp("reltoplant").isEmpty())
        layoutComps[idx]->addMouseListener(this, true);
		
    layoutComps[idx]->getProperties().set(CabbageIDs::lineNumber, cAttr.getNumProp(CabbageIDs::lineNumber));
    layoutComps[idx]->getProperties().set(CabbageIDs::index, idx);
    ((CabbageSoundfiler*)layoutComps[idx])->addChangeListener(this);
	
    //set visiblilty
    layoutComps[idx]->setVisible((cAttr.getNumProp(CabbageIDs::visible)==1 ? true : false));


	TableManager* table = dynamic_cast<CabbageGenTable*>(layoutComps[idx])->table;
	

    var tables = cAttr.getVarArrayProp(CabbageIDs::tablenumber);
	for(int y=0; y<tables.size(); y++)	
    {
		int tableNumber = tables[y];
		if(tableNumber>0)
		{
			//Logger::writeToLog("Table Number:"+String(tableNumber));

			StringArray pFields = getFilter()->getTableStatement(tableNumber);
			int genRoutine = pFields[4].getIntValue();

			tableValues.clear();
			tableValues = getFilter()->getTableFloats(tableNumber);
			
			table->addTable(44100, cAttr.getStringArrayPropValue(CabbageIDs::tablecolour, y), genRoutine, 
																	cAttr.getFloatArrayProp("amprange"), 
																	tableNumber, this);
			if(abs(genRoutine)==1)
			{
				tableBuffer.clear();
				int channels = 1;//for now only works in mono;;
				tableBuffer.setSize(channels, tableValues.size());
				tableBuffer.addFrom(0, 0, tableValues.getRawDataPointer(), tableValues.size());	
				table->setWaveform(tableBuffer, tableNumber);		
			}
			else
			{
				table->setWaveform(tableValues, tableNumber);
				table->enableEditMode(pFields, tableNumber);
			}
			if(cAttr.getStringProp(CabbageIDs::drawmode).toLowerCase()=="vu")
				table->setDrawMode("vu");
		}
    }
	
	table->configTableSizes(cAttr.getVarArrayProp(CabbageIDs::tableconfig));
	table->bringTableToFront(1);	

	if(cAttr.getNumProp(CabbageIDs::startpos)>-1)
		table->setRange(cAttr.getNumProp(CabbageIDs::startpos), cAttr.getNumProp(CabbageIDs::endpos));	
	if(cAttr.getNumProp(CabbageIDs::zoom)!=0)
		table->setZoomFactor(cAttr.getNumProp(CabbageIDs::zoom));

}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//     DirectoryList
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CabbagePluginAudioProcessorEditor::InsertDirectoryList(CabbageGUIClass &cAttr)
{
    layoutComps.add(new CabbageDirectoryList(cAttr.getStringProp(CabbageIDs::name),
                    cAttr.getStringProp(CabbageIDs::channel),
                    cAttr.getStringProp("workingDir"),
                    cAttr.getStringProp("fileType")));

    //add soundfiler object to main processor..
    //getFilter()->soundFilers.add(((Soundfiler*)layoutComps[idx])->transportSource);
    //check to see if widgets is anchored
    //if it is offset its position accordingly.
    int idx = layoutComps.size()-1;
    float left = cAttr.getNumProp(CabbageIDs::left);
    float top = cAttr.getNumProp(CabbageIDs::top);
    float width = cAttr.getNumProp(CabbageIDs::width);
    float height = cAttr.getNumProp(CabbageIDs::height);
    setPositionOfComponent(left, top, width, height, layoutComps[idx], cAttr.getStringProp("reltoplant"));
    ((CabbageDirectoryList*)layoutComps[idx])->directoryList->addActionListener(this);
    layoutComps[idx]->getProperties().set(String("plant"), var(cAttr.getStringProp("plant")));
    layoutComps[idx]->getProperties().set(CabbageIDs::lineNumber, cAttr.getNumProp(CabbageIDs::lineNumber));
    layoutComps[idx]->getProperties().set(CabbageIDs::index, idx);
    //if control is not part of a plant, add mouse listener
    if(cAttr.getStringProp("retoplant").isEmpty())
        layoutComps[idx]->addMouseListener(this, true);
    //set visiblilty
    layoutComps[idx]->setVisible((cAttr.getNumProp(CabbageIDs::visible)==1 ? true : false));

}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//     Multitab
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CabbagePluginAudioProcessorEditor::InsertMultiTab(CabbageGUIClass &cAttr)
{
    jassert(1);
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//      Snapshot control for saving and recalling pre-sets
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CabbagePluginAudioProcessorEditor::InsertSnapshot(CabbageGUIClass &cAttr)
{
    showMessage("Snapshot has been deprecated. Please use a filebutton and a combobox instead. See docs", &getLookAndFeel() );
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//      MIDI keyboard, I've this listed as non-interactive
// as it only sends MIDI, it doesn't communicate over the software bus
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CabbagePluginAudioProcessorEditor::InsertMIDIKeyboard(CabbageGUIClass &cAttr)
{
    layoutComps.add(new MidiKeyboardComponent(getFilter()->keyboardState,
                    MidiKeyboardComponent::horizontalKeyboard));
    int idx = layoutComps.size()-1;

    float left = cAttr.getNumProp(CabbageIDs::left);
    float top = cAttr.getNumProp(CabbageIDs::top);
    float width = cAttr.getNumProp(CabbageIDs::width);
    float height = cAttr.getNumProp(CabbageIDs::height);
    setPositionOfComponent(left, top, width, height, layoutComps[idx], cAttr.getStringProp("reltoplant"));
    ((MidiKeyboardComponent*)layoutComps[idx])->setLowestVisibleKey(cAttr.getNumProp(CabbageIDs::value));
    layoutComps[idx]->getProperties().set(String("plant"), var(cAttr.getStringProp("plant")));
#ifdef Cabbage_Build_Standalone
    layoutComps[idx]->setWantsKeyboardFocus(true);
    layoutComps[idx]->setAlwaysOnTop(true);
#endif
    //if control is not part of a plant, add mouse listener
    if(cAttr.getStringProp("retoplant").isEmpty())
        layoutComps[idx]->addMouseListener(this, true);
    layoutComps[idx]->setName("midiKeyboard");
    layoutComps[idx]->getProperties().set(CabbageIDs::lineNumber, cAttr.getNumProp(CabbageIDs::lineNumber));
    layoutComps[idx]->getProperties().set(CabbageIDs::index, idx);
    //set visiblilty
    layoutComps[idx]->setVisible((cAttr.getNumProp(CabbageIDs::visible)==1 ? true : false));
}

//=======================================================================================
//      interactive components - 'insert' methods followed by event methods
//=======================================================================================
//+++++++++++++++++++++++++++++++++++++++++++
//                                      slider
//+++++++++++++++++++++++++++++++++++++++++++
void CabbagePluginAudioProcessorEditor::InsertSlider(CabbageGUIClass &cAttr)
{
    float left = cAttr.getNumProp(CabbageIDs::left);
    float top = cAttr.getNumProp(CabbageIDs::top);
    float width = cAttr.getNumProp(CabbageIDs::width);
    float height = cAttr.getNumProp(CabbageIDs::height);
    comps.add(new CabbageSlider(cAttr));
    int idx = comps.size()-1;
    setPositionOfComponent(left, top, width, height, comps[idx], cAttr.getStringProp("reltoplant"));
    ((CabbageSlider*)comps[idx])->slider->addListener(this);
    comps[idx]->getProperties().set(String("midiChan"), cAttr.getNumProp("midichan"));
    comps[idx]->getProperties().set(String("midiCtrl"), cAttr.getNumProp("midictrl"));
    ((CabbageSlider*)comps[idx])->slider->getProperties().set(String("index"), idx);
    //if control is not part of a plant, add mouse listener
    //if(cAttr.getStringProp("reltoplant").isEmpty())
    //    comps[idx]->addMouseListener(this, true);
    comps[idx]->getProperties().set(CabbageIDs::lineNumber, cAttr.getNumProp(CabbageIDs::lineNumber));
    comps[idx]->getProperties().set(CabbageIDs::index, idx);
    //set visiblilty
    comps[idx]->setVisible((cAttr.getNumProp(CabbageIDs::visible)==1 ? true : false));
}
//+++++++++++++++++++++++++++++++++++++++++++
//                                     numberbox/slider
//+++++++++++++++++++++++++++++++++++++++++++
void CabbagePluginAudioProcessorEditor::InsertNumberBox(CabbageGUIClass &cAttr)
{
    float left = cAttr.getNumProp(CabbageIDs::left);
    float top = cAttr.getNumProp(CabbageIDs::top);
    float width = cAttr.getNumProp(CabbageIDs::width);
    float height = cAttr.getNumProp(CabbageIDs::height);
    comps.add(new CabbageNumberBox(cAttr));
    int idx = comps.size()-1;
    setPositionOfComponent(left, top, width, height, comps[idx], cAttr.getStringProp("reltoplant"));
    ((CabbageNumberBox*)comps[idx])->slider->addListener(this);
    comps[idx]->getProperties().set(String("midiChan"), cAttr.getNumProp("midichan"));
    comps[idx]->getProperties().set(String("midiCtrl"), cAttr.getNumProp("midictrl"));
    ((CabbageNumberBox*)comps[idx])->slider->getProperties().set(String("index"), idx);
    //if control is not part of a plant, add mouse listener
    if(cAttr.getStringProp("reltoplant").isEmpty())
        comps[idx]->addMouseListener(this, true);
    comps[idx]->getProperties().set(CabbageIDs::lineNumber, cAttr.getNumProp(CabbageIDs::lineNumber));
    comps[idx]->getProperties().set(CabbageIDs::index, idx);
    //set visiblilty
    comps[idx]->setVisible((cAttr.getNumProp(CabbageIDs::visible)==1 ? true : false));
}

/******************************************/
/*             slider event               */
/******************************************/
void CabbagePluginAudioProcessorEditor::sliderValueChanged (Slider* sliderThatWasMoved)
{
#ifndef Cabbage_No_Csound

    int i = sliderThatWasMoved->getProperties().getWithDefault("index", -9999);
    if(i==-9999)jassert(1);
	
    if(getFilter()->getGUICtrls(i).getStringProp(CabbageIDs::name)==sliderThatWasMoved->getParentComponent()->getName())
    {
#ifndef Cabbage_Build_Standalone
        getFilter()->getGUICtrls(i).setNumProp(CabbageIDs::value, (float)sliderThatWasMoved->getValue());
        float min = getFilter()->getGUICtrls(i).getNumProp("min");
        float range = getFilter()->getGUICtrls(i).getNumProp("range");
        //normalise parameters in plugin mode.
        getFilter()->beginParameterChangeGesture(i);
        getFilter()->setParameter(i, (float)((sliderThatWasMoved->getValue()-min)/range));
        getFilter()->setParameterNotifyingHost(i, (float)((sliderThatWasMoved->getValue()-min)/range));
        //getFilter()->setParameterNotifyingHost(i, (float)sliderThatWasMoved->getValue());
        getFilter()->endParameterChangeGesture(i);
#else
        float value = sliderThatWasMoved->getValue();//getFilter()->getGUICtrls(i).getNumProp(CabbageIDs::value);
        getFilter()->beginParameterChangeGesture(i);
        getFilter()->setParameter(i, (float)sliderThatWasMoved->getValue());
        getFilter()->setParameterNotifyingHost(i, (float)sliderThatWasMoved->getValue());
        getFilter()->endParameterChangeGesture(i);

#endif
    }
#endif
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//     Popup menu
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CabbagePluginAudioProcessorEditor::InsertPopupMenu(CabbageGUIClass &cAttr)
{
    float left = cAttr.getNumProp(CabbageIDs::left);
    float top = cAttr.getNumProp(CabbageIDs::top);
    float width = cAttr.getNumProp(CabbageIDs::width);
    float height = cAttr.getNumProp(CabbageIDs::height);

    layoutComps.add(new CabbagePopupMenu(cAttr));

    int idx = layoutComps.size()-1;
    popupMenus.add(idx);
    //setPositionOfComponent(left, top, width, height, layoutComps[idx], cAttr.getStringProp("reltoplant"));
    layoutComps[idx]->getProperties().set(CabbageIDs::lineNumber, cAttr.getNumProp(CabbageIDs::lineNumber));
    layoutComps[idx]->getProperties().set(CabbageIDs::index, idx);
}

//+++++++++++++++++++++++++++++++++++++++++++
//                                      button
//+++++++++++++++++++++++++++++++++++++++++++
void CabbagePluginAudioProcessorEditor::InsertButton(CabbageGUIClass &cAttr)
{
    comps.add(new CabbageButton(cAttr));
    int idx = comps.size()-1;
    float left = cAttr.getNumProp(CabbageIDs::left);
    float top = cAttr.getNumProp(CabbageIDs::top);
    float width = cAttr.getNumProp(CabbageIDs::width);
    float height = cAttr.getNumProp(CabbageIDs::height);
    setPositionOfComponent(left, top, width, height, comps[idx], cAttr.getStringProp("reltoplant"));
    ((CabbageButton*)comps[idx])->button->addListener(this);
#ifdef Cabbage_Build_Standalone
    ((CabbageButton*)comps[idx])->button->setWantsKeyboardFocus(true);
#endif
    ((CabbageButton*)comps[idx])->button->getProperties().set(String("index"), idx);
    if(!cAttr.getNumProp(CabbageIDs::visible))
        comps[idx]->setVisible(false);
    //if control is not part of a plant, add mouse listener
    if(cAttr.getStringProp("reltoplant").isEmpty())
        comps[idx]->addMouseListener(this, true);
    comps[idx]->getProperties().set(CabbageIDs::lineNumber, cAttr.getNumProp(CabbageIDs::lineNumber));
    comps[idx]->getProperties().set(CabbageIDs::index, idx);
    //set visiblilty
    comps[idx]->setVisible((cAttr.getNumProp(CabbageIDs::visible)==1 ? true : false));
}

//+++++++++++++++++++++++++++++++++++++++++++
//                                      checkbox
//+++++++++++++++++++++++++++++++++++++++++++
void CabbagePluginAudioProcessorEditor::InsertCheckBox(CabbageGUIClass &cAttr)
{
    comps.add(new CabbageCheckbox(cAttr));
    int idx = comps.size()-1;
    float left = cAttr.getNumProp(CabbageIDs::left);
    float top = cAttr.getNumProp(CabbageIDs::top);
    float width = cAttr.getNumProp(CabbageIDs::width);
    float height = cAttr.getNumProp(CabbageIDs::height);
    setPositionOfComponent(left, top, width, height, comps[idx], cAttr.getStringProp("reltoplant"));
    ((CabbageCheckbox*)comps[idx])->button->addListener(this);
#ifdef Cabbage_Build_Standalone
    ((CabbageCheckbox*)comps[idx])->button->setWantsKeyboardFocus(true);
#endif
    ((CabbageCheckbox*)comps[idx])->button->getProperties().set(String("index"), idx);
    if(!cAttr.getNumProp(CabbageIDs::visible))
        comps[idx]->setVisible(false);
    //if control is not part of a plant, add mouse listener
    if(cAttr.getStringProp("reltoplant").isEmpty())
        comps[idx]->addMouseListener(this, true);
    comps[idx]->getProperties().set(CabbageIDs::lineNumber, cAttr.getNumProp(CabbageIDs::lineNumber));
    comps[idx]->getProperties().set(CabbageIDs::index, idx);
    //set visiblilty
    comps[idx]->setVisible((cAttr.getNumProp(CabbageIDs::visible)==1 ? true : false));
}

/*****************************************************/
/*     button/filebutton/checkbox press event        */
/*****************************************************/
//for unlatched buttons...
void CabbagePluginAudioProcessorEditor::buttonStateChanged(Button* button)
{
    if(button->isMouseButtonDown())
    {
        for(int i=0; i<(int)getFilter()->getGUICtrlsSize(); i++)
        {
            //find correct control from vector
            //+++++++++++++button+++++++++++++
            if(getFilter()->getGUICtrls(i).getStringProp(CabbageIDs::name)==button->getName())
            {
                if(getFilter()->getGUICtrls(i).getStringProp(CabbageIDs::type)==String("button")&&
                        getFilter()->getGUICtrls(i).getNumProp("latched")==0)
                {
                    //toggle button values
                    if(getFilter()->getGUICtrls(i).getNumProp(CabbageIDs::value)==0)
                    {
                        getFilter()->setParameterNotifyingHost(i, 1.f);
                        getFilter()->setParameter(i, 1.f);
                        getFilter()->getGUICtrls(i).setNumProp(CabbageIDs::value, 1);
                    }
                    else
                    {
                        getFilter()->setParameterNotifyingHost(i, 0.f);
                        getFilter()->getGUICtrls(i).setNumProp(CabbageIDs::value, 0);
                        getFilter()->setParameter(i, 0.f);
                    }
                    //toggle text values
                    for(int o=0; o<getFilter()->getGUICtrls(i).getStringArrayProp("text").size(); o++)
                        Logger::writeToLog(getFilter()->getGUICtrls(i).getStringArrayPropValue("text", o));

                    if(getFilter()->getGUICtrls(i).getStringArrayPropValue("text", 1).equalsIgnoreCase(button->getButtonText()))
                        button->setButtonText(getFilter()->getGUICtrls(i).getStringArrayPropValue("text", 0));
                    else
                        button->setButtonText(getFilter()->getGUICtrls(i).getStringArrayPropValue("text", 1));
                }

            }
        }
    }
}


void CabbagePluginAudioProcessorEditor::buttonClicked(Button* button)
{
#ifndef Cabbage_No_Csound
//thi is funked!
//Logger::writeToLog(button->getName());
    if(!getFilter()->isGuiEnabled())
    {
        if(button->isEnabled())      // check button is ok before sending data to on named channel
        {
            if(dynamic_cast<TextButton*>(button)) //check what type of button it is
            {
                //deal with non-interactive buttons first..
                if(button->getName()=="source")
                {
//                              getFilter()->createAndShowSourceEditor(lookAndFeel);
                }
                else if(button->getName()=="infobutton")
                {
                    String file = getFilter()->getCsoundInputFile().getParentDirectory().getFullPathName();
#ifdef LINUX
                    ChildProcess process;
                    file.append("/", 5);
                    file.append(button->getProperties().getWithDefault("filename", ""), 1024);
                    if(!process.start("xdg-open "+file))
                        CabbageUtils::showMessage("Couldn't show file", &getLookAndFeel());
#else
                    file.append("\\", 5);
                    file.append(button->getProperties().getWithDefault("filename", ""), 1024);
                    if(!infoWindow)
                    {
                        //showMessage(file);
                        infoWindow = new InfoWindow(lookAndFeel, file);
                        infoWindow->centreWithSize(600, 400);
                        infoWindow->setAlwaysOnTop(true);
                        infoWindow->toFront(true);
                        infoWindow->setVisible(true);
                    }
                    else
                        infoWindow->setVisible(true);
#endif
                }
                else if(button->getName()=="recordbutton")
                {

                    if(button->getButtonText()=="Start Recording")
                    {
                        button->setButtonText("Stop Recording");
                        getFilter()->startRecording();
                    }
                    else
                    {
                        button->setButtonText("Start Recording");
                        getFilter()->stopRecording();
                    }
                }


                //check layoutControls for fileButtons, these are once off buttons....
                //for(int i=0;i<(int)getFilter()->getGUILayoutCtrlsSize();i++){//find correct control from vector
                int i = button->getProperties().getWithDefault("index", -9999);
                //Logger::writeToLog(button->getName());
                if(isPositiveAndBelow(i, getFilter()->getGUILayoutCtrlsSize()))
                    if(getFilter()->getGUILayoutCtrls(i).getStringProp(CabbageIDs::name)==button->getName())
                    {
                        if(getFilter()->getGUILayoutCtrls(i).getStringProp(CabbageIDs::type)==String("filebutton"))
                        {
                            WildcardFileFilter wildcardFilter ("*.*", String::empty, "Foo files");


                            const String filetype = getFilter()->getGUILayoutCtrls(i).getStringProp("filetype");
                            const String selectedDir = getFilter()->getGUILayoutCtrls(i).getStringProp("workingdir");
                            File directory;
                            if(selectedDir.isNotEmpty())
                                directory = File(selectedDir);
                            else
                                directory = File::getCurrentWorkingDirectory();


                            File selectedFile;
                            //check for snapshot mode
                            if(getFilter()->getGUILayoutCtrls(i).getStringProp("mode").equalsIgnoreCase("snapshot") &&
                                    getFilter()->getGUILayoutCtrls(i).getStringProp("filetype").contains("snaps"))
                            {
                                //here we write the files with generic names for quick saving
                                Array<File> dirFiles;
                                File pluginDir;
                                String currentFileLocation = getFilter()->getCsoundInputFile().getParentDirectory().getFullPathName();
                                //Logger::writeToLog(currentFileLocation);
                                if(selectedDir.length()<1)
                                {
                                    pluginDir = File(currentFileLocation);
                                }
                                else
                                    pluginDir = File(selectedDir);
                                pluginDir.findChildFiles(dirFiles, 2, false, filetype);

                                //now check existing files in directory and make sure we use a unique name
                                for(int i=0; i<100; i++)
                                {
                                    String newName = getFilter()->getCsoundInputFile().getFileNameWithoutExtension()+"_"+String(i);
#ifdef WIN32
                                    String fullFileName = pluginDir.getFullPathName()+"\\"+newName+".snaps";
#else
                                    String fullFileName = pluginDir.getFullPathName()+"/"+newName+".snaps";
#endif
                                    //Logger::writeToLog(fullFileName);
                                    bool allowSave = true;
                                    for(int y=0; y<dirFiles.size(); y++)
                                    {
                                        if(dirFiles[y].getFileNameWithoutExtension().equalsIgnoreCase(newName))
                                            allowSave = false;
                                    }
                                    if(allowSave)
                                    {
                                        savePresetsFromParameters(File(fullFileName), "create");
                                        refreshDiskReadingGUIControls("combobox");
                                        return;
                                    }
                                }


                            }
                            //else let the user specify the filename for the snapshot
                            else
                            {
#if !defined(AndroidBuild)
                                if(!getFilter()->getGUILayoutCtrls(i).getStringProp("filetype").contains("snaps"))
                                {
                                    FileChooser fc("Open", directory.getFullPathName(), filetype, UseNativeDialogue);
                                    //determine whether to poen file or directory
                                    if(getFilter()->getGUILayoutCtrls(i).getStringProp("mode")=="file")
                                    {
                                        if(fc.browseForFileToOpen())
                                        {
                                            selectedFile = fc.getResult();
                                            getFilter()->messageQueue.addOutgoingChannelMessageToQueue(getFilter()->getGUILayoutCtrls(i).getStringProp(CabbageIDs::channel),
                                                    selectedFile.getFullPathName().replace("\\", "\\\\"),
                                                    "string");
                                        }
                                    }

                                    else if(getFilter()->getGUILayoutCtrls(i).getStringProp("mode")=="directory")
                                    {
                                        if(fc.browseForDirectory())
                                        {
                                            selectedFile = fc.getResult();
                                            Logger::writeToLog(selectedFile.getFullPathName());
                                            getFilter()->messageQueue.addOutgoingChannelMessageToQueue(getFilter()->getGUILayoutCtrls(i).getStringProp(CabbageIDs::channel),
                                                    selectedFile.getFullPathName().replace("\\", "\\\\"),
                                                    "string");
                                        }
                                    }
                                }
                                else
                                {
                                    FileChooser fc("Select a file to save", directory.getFullPathName(), filetype, UseNativeDialogue);
                                    if(fc.browseForFileToSave(true))
                                    {
                                        selectedFile = fc.getResult();
                                        if(filetype.contains("snaps"))
                                            savePresetsFromParameters(selectedFile, "create");
                                        else
                                            getFilter()->messageQueue.addOutgoingChannelMessageToQueue(getFilter()->getGUILayoutCtrls(i).getStringProp(CabbageIDs::channel),
                                                    selectedFile.getFullPathName(),
                                                    "string");
                                        refreshDiskReadingGUIControls("combobox");
                                    }
                                }
#endif
                            }

                        }
                    }


                //for(int i=0;i<(int)getFilter()->getGUICtrlsSize();i++){//find correct control from vector
                i = button->getProperties().getWithDefault("index", -9999);

                //+++++++++++++button+++++++++++++
                if(isPositiveAndBelow(i, getFilter()->getGUICtrlsSize()))
                    if(getFilter()->getGUICtrls(i).getStringProp(CabbageIDs::name)==button->getName())
                    {
                        if(getFilter()->getGUICtrls(i).getStringProp(CabbageIDs::type)==String("button"))
                        {
                            //toggle button values
                            if(getFilter()->getGUICtrls(i).getNumProp(CabbageIDs::value)==0)
                            {
                                getFilter()->setParameterNotifyingHost(i, 1.f);
                                getFilter()->setParameter(i, 1.f);
                                //getFilter()->getCsound()->SetChannel(getFilter()->getGUICtrls(i).getStringProp(CabbageIDs::channel).toUTF8(), 1.f);
                                getFilter()->getGUICtrls(i).setNumProp(CabbageIDs::value, 1);
                            }
                            else
                            {
                                getFilter()->setParameterNotifyingHost(i, 0.f);
                                //getFilter()->getCsound()->SetChannel(getFilter()->getGUICtrls(i).getStringProp(CabbageIDs::channel).toUTF8(), 0.f);
                                getFilter()->getGUICtrls(i).setNumProp(CabbageIDs::value, 0);
                                getFilter()->setParameter(i, 0.f);
                            }
                            //toggle text values
                            for(int o=0; o<getFilter()->getGUICtrls(i).getStringArrayProp("text").size(); o++)
                                Logger::writeToLog(getFilter()->getGUICtrls(i).getStringArrayPropValue("text", o));

                            if(getFilter()->getGUICtrls(i).getStringArrayPropValue("text", 1).equalsIgnoreCase(button->getButtonText()))
                                button->setButtonText(getFilter()->getGUICtrls(i).getStringArrayPropValue("text", 0));
                            else
                                button->setButtonText(getFilter()->getGUICtrls(i).getStringArrayPropValue("text", 1));
                        }

                    }
                /*
                				//show plants as popup window
                				for(int p=0;p<getFilter()->getGUILayoutCtrlsSize();p++){
                						if(getFilter()->getGUILayoutCtrls(p).getStringProp("plant") ==button->getName()){
                						int index = button->getProperties().getWithDefault(String("index"), 0);
                						subPatch[index]->setVisible(true);
                						subPatch[index]->setAlwaysOnTop(true);
                						subPatch[index]->toFront(true);
                						i=getFilter()->getGUICtrlsSize();
                						break;
                						}
                					}
                */
            }

            else if(dynamic_cast<ToggleButton*>(button))
            {
                //for(int i=0;i<(int)getFilter()->getGUICtrlsSize();i++)//find correct control from vector
                int i = button->getProperties().getWithDefault("index", -9999);
                if(getFilter()->getGUICtrls(i).getStringProp(CabbageIDs::name)==button->getName())
                {
                    //      Logger::writeToLog(String(button->getToggleStateValue().getValue()));

                    if(button->getToggleState())
                    {
                        button->setToggleState(true, dontSendNotification);
                        getFilter()->setParameter(i, 1.f);
                        getFilter()->setParameterNotifyingHost(i, 1.f);
                        //getFilter()->getCsound()->SetChannel(getFilter()->getGUICtrls(i).getStringProp(CabbageIDs::channel).toUTF8(), 1.f);
                        getFilter()->getGUICtrls(i).setNumProp(CabbageIDs::value, 1);
                    }
                    else
                    {
                        button->setToggleState(false, dontSendNotification);
                        getFilter()->setParameter(i, 0.f);
                        getFilter()->setParameterNotifyingHost(i, 0.f);
                        //getFilter()->getCsound()->SetChannel(getFilter()->getGUICtrls(i).getStringProp(CabbageIDs::channel).toUTF8(), 0.f);
                        getFilter()->getGUICtrls(i).setNumProp(CabbageIDs::value, 0);
                        //Logger::writeToLog("pressed");
                    }
                    //getFilter()->getCsound()->SetChannel(getFilter()->getGUICtrls(i).getStringProp(CabbageIDs::channel).toUTF8(), button->getToggleStateValue().getValue());
                    //getFilter()->setParameterNotifyingHost(i, button->getToggleStateValue().getValue());
                }
            }
        }

    }//end of GUI enabled check
#endif
}


//+++++++++++++++++++++++++++++++++++++++++++
//                                      combobox
//+++++++++++++++++++++++++++++++++++++++++++
void CabbagePluginAudioProcessorEditor::InsertComboBox(CabbageGUIClass &cAttr)
{
    String currentFileLocation = getFilter()->getCsoundInputFile().getParentDirectory().getFullPathName();
    if(File::isAbsolutePath(cAttr.getStringProp(CabbageIDs::workingdir))!=true)
        cAttr.setStringProp(CabbageIDs::workingdir, currentFileLocation);

    comps.add(new CabbageComboBox(cAttr));

    int idx = comps.size()-1;
    float left = cAttr.getNumProp(CabbageIDs::left);
    float top = cAttr.getNumProp(CabbageIDs::top);
    float width = cAttr.getNumProp(CabbageIDs::width);
    float height = cAttr.getNumProp(CabbageIDs::height);
    setPositionOfComponent(left, top, width, height, comps[idx], cAttr.getStringProp("reltoplant"));
    ((CabbageComboBox*)comps[idx])->combo->addListener(this);
    ((CabbageComboBox*)comps[idx])->combo->getProperties().set(String("index"), idx);
    if(!cAttr.getNumProp(CabbageIDs::visible))
        comps[idx]->setVisible(false);
    //if control is not part of a plant, add mouse listener
    if(cAttr.getStringProp("reltoplant").isEmpty())
        comps[idx]->addMouseListener(this, true);
    comps[idx]->getProperties().set(CabbageIDs::lineNumber, cAttr.getNumProp(CabbageIDs::lineNumber));
    comps[idx]->getProperties().set(CabbageIDs::index, idx);
    //set visiblilty
    comps[idx]->setVisible((cAttr.getNumProp(CabbageIDs::visible)==1 ? true : false));

}

/******************************************/
/*             combobBox event            */
/******************************************/
void CabbagePluginAudioProcessorEditor::comboBoxChanged (ComboBox* combo)
{
#ifndef Cabbage_No_Csound
    if(combo->isEnabled()) // before sending data to on named channel
    {
//for(int i=0;i<(int)getFilter()->getGUICtrlsSize();i++)//find correct control from vector
        int i = combo->getProperties().getWithDefault("index", -9999);
        if(getFilter()->getGUICtrls(i).getStringProp(CabbageIDs::name)==combo->getName())
        {

            if(getFilter()->getGUICtrls(i).getStringProp("filetype").contains("snaps"))
            {
                String filename;
                String workingDir = getFilter()->getGUICtrls(i).getStringProp("workingdir");
                if(workingDir.isEmpty())
                    workingDir = getFilter()->getCsoundInputFile().getParentDirectory().getFullPathName();
#ifdef WIN32
                filename = workingDir+"\\"+combo->getText()+".snaps";
#else
                filename = workingDir+"/"+combo->getText()+".snaps";
#endif
                //Logger::writeToLog(filename);
                if(File(filename).existsAsFile())
                    restoreParametersFromPresets(XmlDocument::parse(File(filename)));
                //File(combo->getText())
            }

#ifndef Cabbage_Build_Standalone
            getFilter()->setParameter(i, (float)(combo->getSelectedItemIndex()+1)/(getFilter()->getGUICtrls(i).getNumProp("comborange")));
            getFilter()->setParameterNotifyingHost(i, (float)(combo->getSelectedItemIndex()+1)/(getFilter()->getGUICtrls(i).getNumProp("comborange")));
#else
            getFilter()->setParameter(i, (float)(combo->getSelectedItemIndex()+1));
            getFilter()->setParameterNotifyingHost(i, (float)(combo->getSelectedItemIndex()+1));
#endif
        }
    }

#endif
}

//+++++++++++++++++++++++++++++++++++++++++++
//                                      xypad
//+++++++++++++++++++++++++++++++++++++++++++
void CabbagePluginAudioProcessorEditor::InsertXYPad(CabbageGUIClass &cAttr)
{
    /*
    Our filters control vector contains two xypads, one for the X channel and one for the Y
    channel. Our editor only needs to display one so the xypad with 'dummy' appended to the name
    will be created but not shown.

    We also need to check to see whether the processor editor has been 're-opened'. If so we
    don't need to recreate the automation
    */
    int idx=0;

    if(getFilter()->haveXYAutosBeenCreated())
    {
        comps.add(new CabbageXYController(getFilter()->getXYAutomater(xyPadIndex),
                                          cAttr.getStringProp(CabbageIDs::name),
                                          cAttr.getStringProp("text"),
                                          "",
                                          cAttr.getNumProp("minx"),
                                          cAttr.getNumProp("maxx"),
                                          cAttr.getNumProp("miny"),
                                          cAttr.getNumProp("maxy"),
                                          xyPadIndex,
                                          cAttr.getNumProp("decimalplaces"),
                                          cAttr.getStringProp(CabbageIDs::colour),
                                          cAttr.getStringProp(CabbageIDs::fontcolour),
                                          cAttr.getNumProp("valuex"),
                                          cAttr.getNumProp("valuey")));
        xyPadIndex++;
        idx = comps.size()-1;

        if(!cAttr.getNumProp(CabbageIDs::visible))
            comps[idx]->setVisible(false);

    }
    else
    {

        getFilter()->addXYAutomater(new XYPadAutomation());
        getFilter()->getXYAutomater(getFilter()->getXYAutomaterSize()-1)->addChangeListener(getFilter());
        getFilter()->getXYAutomater(getFilter()->getXYAutomaterSize()-1)->xChannel = cAttr.getStringProp("xchannel");
        getFilter()->getXYAutomater(getFilter()->getXYAutomaterSize()-1)->yChannel = cAttr.getStringProp("ychannel");
        cAttr.setNumProp("xyautoindex", getFilter()->getXYAutomaterSize()-1);

        comps.add(new CabbageXYController(getFilter()->getXYAutomater(getFilter()->getXYAutomaterSize()-1),
                                          cAttr.getStringProp(CabbageIDs::name),
                                          cAttr.getStringProp("text"),
                                          "",
                                          cAttr.getNumProp("minx"),
                                          cAttr.getNumProp("maxx"),
                                          cAttr.getNumProp("miny"),
                                          cAttr.getNumProp("maxy"),
                                          getFilter()->getXYAutomaterSize()-1,
                                          cAttr.getNumProp("decimalPlaces"),
                                          cAttr.getStringProp(CabbageIDs::colour),
                                          cAttr.getStringProp(CabbageIDs::fontcolour),
                                          cAttr.getNumProp("valuex"),
                                          cAttr.getNumProp("valuey")));
        idx = comps.size()-1;
        getFilter()->getXYAutomater(getFilter()->getXYAutomaterSize()-1)->paramIndex = idx;
        Logger::writeToLog("Number of XYAutos:"+String(getFilter()->getXYAutomaterSize()));
        //set visiblilty
        comps[idx]->setVisible((cAttr.getNumProp(CabbageIDs::visible)==1 ? true : false));

    }

    float left = cAttr.getNumProp(CabbageIDs::left);
    float top = cAttr.getNumProp(CabbageIDs::top);
    float width = cAttr.getNumProp(CabbageIDs::width);
    float height = cAttr.getNumProp(CabbageIDs::height);
    //check to see if widgets is anchored
    //if it is offset its position accordingly.
    int relY=0,relX=0;
    if(layoutComps.size()>0)
    {
        if(comps[idx])
            for(int y=0; y<layoutComps.size(); y++)
                if(cAttr.getStringProp("reltoplant").length()>0)
                {
                    if(layoutComps[y]->getProperties().getWithDefault(String("plant"), -99).toString().equalsIgnoreCase(cAttr.getStringProp("reltoplant")))
                    {
                        positionComponentWithinPlant("", left, top, width, height, layoutComps[y], comps[idx]);
                    }
                }
                else
                {
                    comps[idx]->setBounds(left+relX, top+relY, width, height);
                    if(!cAttr.getStringProp(CabbageIDs::name).containsIgnoreCase("dummy"))
                        componentPanel->addAndMakeVisible(comps[idx]);
                }
    }
    else
    {
        comps[idx]->setBounds(left+relX, top+relY, width, height);
        if(!cAttr.getStringProp(CabbageIDs::name).containsIgnoreCase("dummy"))
            componentPanel->addAndMakeVisible(comps[idx]);
    }


    float max = cAttr.getNumProp("maxx");
    float min = cAttr.getNumProp("minx");
    float valueX = cabbageABS(min-cAttr.getNumProp("valuex"))/cabbageABS(min-max);
    //Logger::writeToLog(String("X:")+String(valueX));
    max = cAttr.getNumProp("maxy");
    min = cAttr.getNumProp("miny");
    float valueY = cabbageABS(min-cAttr.getNumProp("valuey"))/cabbageABS(min-max);
    //Logger::writeToLog(String("Y:")+String(valueY));
    //((CabbageXYController*)comps[idx])->xypad->setXYValues(cAttr.getNumProp("valueX"),
    // cAttr.getNumProp("valueY"));

    if(!cAttr.getStringProp(CabbageIDs::name).containsIgnoreCase("dummy"))
    {
        getFilter()->setParameter(idx, cAttr.getNumProp("valuey"));
        getFilter()->setParameter(idx+1, cAttr.getNumProp("valuey"));
    }



    // getFilter()->getXYAutomater(getFilter()->getXYAutomaterSize()-1)->setXValue(cAttr.getNumProp("valueX"));
    // getFilter()->getXYAutomater(getFilter()->getXYAutomaterSize()-1)->setYValue(cAttr.getNumProp("valueY"));
#ifdef Cabbage_Build_Standalone
    comps[idx]->setWantsKeyboardFocus(false);
#endif
    //if(!cAttr.getStringProp(CabbageIDs::name).containsIgnoreCase("dummy"))
    // actionListenerCallback(cAttr.getStringProp(CabbageIDs::name));
    comps[idx]->getProperties().set(CabbageIDs::lineNumber, cAttr.getNumProp(CabbageIDs::lineNumber));
    comps[idx]->getProperties().set(CabbageIDs::index, idx);
}


//+++++++++++++++++++++++++++++++++++++++++++
//                                      table
//+++++++++++++++++++++++++++++++++++++++++++
void CabbagePluginAudioProcessorEditor::InsertTable(CabbageGUIClass &cAttr)
{
    int tableSize=0;
    int tableNumber = cAttr.getIntArrayPropValue("tablenumber", 0);
    Array<int> tableSizes;
#ifndef Cabbage_No_Csound
    //fill array with points from table, if table is valid
    if(getFilter()->getCompileStatus()==0 && getFilter()->getCsound())
    {
        if(cAttr.getIntArrayProp("tablenumber").size()>1)
            for(int i=0; i<cAttr.getIntArrayProp("tablenumber").size(); i++)
            {
                tableSizes.add(getFilter()->getCsound()->TableLength(cAttr.getIntArrayPropValue("tablenumber",i)));

                if(tableSize<getFilter()->getCsound()->TableLength(cAttr.getIntArrayPropValue("tablenumber",i)))
                {
                    tableSize = getFilter()->getCsound()->TableLength(cAttr.getIntArrayPropValue("tablenumber",i));
                }
            }
        else
        {
            tableSize = getFilter()->getCsound()->TableLength(cAttr.getIntArrayPropValue("tablenumber", 0));
            tableSizes.add(tableSize);
        }
    }
    else
    {
        //if table is not valid fill our array with at least one dummy point
    }
#endif
    //tableSizes.clear();
    //for(int i=0;i<cAttr.getStringArrayProp(CabbageIDs::channel).size();i++)
    // tableSizes.add(tableSize);


    layoutComps.add(new CabbageTable(cAttr.getStringProp(CabbageIDs::name),
                                     cAttr.getStringProp("text"),
                                     cAttr.getStringProp(CabbageIDs::caption),
                                     cAttr.getStringArrayProp(CabbageIDs::channel),
                                     cAttr.getIntArrayProp("tablenumber"),
                                     tableSizes,
                                     cAttr.getIntArrayProp("drawmode"),
                                     cAttr.getIntArrayProp("resizemode"),
                                     cAttr.getFloatArrayProp("amprange"),
                                     cAttr.getStringArrayProp(CabbageIDs::tablecolour),
                                     cAttr.getNumProp("readonly"),
                                     (bool)cAttr.getNumProp("stack"),
                                     this));

    int idx = layoutComps.size()-1;
    float left = cAttr.getNumProp(CabbageIDs::left);
    float top = cAttr.getNumProp(CabbageIDs::top);
    float width = cAttr.getNumProp(CabbageIDs::width);
    float height = cAttr.getNumProp(CabbageIDs::height);
    setPositionOfComponent(left, top, width, height,layoutComps[idx], cAttr.getStringProp("reltoplant"));


    //finally, add tables to widget
    ((CabbageTable*)layoutComps[idx])->addTables();

    //set visiblilty
    layoutComps[idx]->setVisible((cAttr.getNumProp(CabbageIDs::visible)==1 ? true : false));
    //if control is not part of a plant, add mouse listener
    if(cAttr.getStringProp("reltoplant").isEmpty())
        layoutComps[idx]->addMouseListener(this, true);
    layoutComps[idx]->getProperties().set(CabbageIDs::lineNumber, cAttr.getNumProp(CabbageIDs::lineNumber));
    layoutComps[idx]->getProperties().set(CabbageIDs::index, idx);

    int numberOfTables = cAttr.getStringArrayProp(CabbageIDs::tablenumber).size();
    for(int y=0; y<numberOfTables; y++)
    {
        tableNumber = cAttr.getIntArrayPropValue(CabbageIDs::tablenumber, y);
        Array <float, CriticalSection> tableValues = getFilter()->getTableFloats(tableNumber);
        ((CabbageTable*)layoutComps[idx])->fillTable(y, tableValues);
        //	StringArray statement = getFilter()->getTableEvtCode(tableNumber);
        //	((CabbageTable*)layoutComps[idx])->setTableEvtCode(y, statement);
    }

}


/*********************************************************/
/*     actionlistener method 							 */
/*********************************************************/
void CabbagePluginAudioProcessorEditor::actionListenerCallback (const String& message)
{
//the first part of this method receives messages from the GUI editor layout/Main panel and updates the
//source code accordingly. The second half is used for messages being sent from GUI widgets
    StringArray csdArray;

//START OF TEST FOR MESSAGES FROM THE PROPS DIALOG
    if(message == "Message sent from PropertiesDialog")
    {
#ifdef Cabbage_Build_Standalone
        csdArray.clear();
        //showMessage(currentLineNumber);
        //Logger::writeToLog("LineNumber:"+String(lineNumber));
        csdArray.addLines(getFilter()->getCsoundInputFileText());
        csdArray.set(currentLineNumber, CabbageGUIClass::getCabbageCodeFromIdentifiers(propsWindow->updatedIdentifiers));
        getFilter()->updateCsoundFile(csdArray.joinIntoString("\n"));
        getFilter()->highlightLine(csdArray[currentLineNumber]);
        //Logger::writeToLog(csdArray[currentLineNumber]);
        getFilter()->setGuiEnabled(true);
        getFilter()->createGUI(csdArray.joinIntoString("\n"), true);
        layoutEditor->selectedFilters.deselectAll();
        //getFilter()->sendActionMessage("GUI Updated, controls added");
        //Logger::writeToLog(getCodeFromIdentifiers(propsWindow->updatedIdentifiers));
#endif
    }
    else if(message.contains("Message sent from CabbageMainPanel:delete:"))
        showMessage("Delte");
//END OF TEST FROM PROPS

//CALL LISTENER METHOD FOR CUSTOM COMPONENTS THAT USE ACTION BROADCASTERS
    else
    {
        Logger::writeToLog("Unknwn message:"+message);
    }
}


//=============================================================================
void CabbagePluginAudioProcessorEditor::updateSize()
{
#ifdef Cabbage_Build_Standalone
    if(!getFilter()->getCsoundInputFile().loadFileAsString().isEmpty())
    {
        //break up lines in csd file into a string array
        StringArray csdArray;
        csdArray.addLines(getFilter()->getCsoundInputFileText());
        for(int i=0; i<csdArray.size(); i++)
        {
            CabbageGUIClass CAttr(csdArray[i], -99);
            if(csdArray[i].contains("</Cabbage>"))
                break;

            if(csdArray[i].contains("form"))
            {
                String newSize = "size("+String(getWidth())+", "+String(getHeight())+")";
                //Logger::writeToLog(newSize);
                csdArray.set(i, replaceIdentifier(csdArray[i], "size(", newSize));
            }
        }
        getFilter()->updateCsoundFile(csdArray.joinIntoString("\n"));
        getFilter()->sendActionMessage("GUI Updated, controls added, resized");
    }
#endif
}
//=============================================================================
bool CabbagePluginAudioProcessorEditor::keyPressed(const juce::KeyPress &key ,Component *)
{
    if(key.getTextDescription()=="ctrl + B")
        getFilter()->sendActionMessage("MENU COMMAND: manual update instrument");

    if(key.getTextDescription()=="ctrl + O")
        getFilter()->sendActionMessage("MENU COMMAND: open instrument");

    if(key.getTextDescription()=="ctrl + U")
        getFilter()->sendActionMessage("MENU COMMAND: manual update GUI");
#ifdef MACOSX
    if(key.getTextDescription()=="cmd + M")
        getFilter()->sendActionMessage("MENU COMMAND: suspend audio");
#else
    if(key.getTextDescription()=="ctrl + M")
        getFilter()->sendActionMessage("MENU COMMAND: suspend audio");
#endif
    if(key.getTextDescription()=="ctrl + E")
    {
        getFilter()->sendActionMessage("MENU COMMAND: toggle edit");
    }

    return true;
}

//=========================================================================================
//			resfresh GUI controls that read from disk..
//========================================================================================
void CabbagePluginAudioProcessorEditor::refreshDiskReadingGUIControls(String typeOfControl)
{
    for(int i=0; i<getFilter()->getGUICtrlsSize(); i++)
    {
        //if typeOfControl == combobox, user must have file type set for comboboxes to refresh
        if(getFilter()->getGUICtrls(i).getStringProp(CabbageIDs::type).equalsIgnoreCase(typeOfControl) &&
                getFilter()->getGUICtrls(i).getStringProp("filetype").isNotEmpty())
        {
            CabbageComboBox* cabCombo = dynamic_cast<CabbageComboBox*>(comps[i]);
            if(cabCombo)
            {
                File pluginDir;
                int currentItemID = cabCombo->combo->getSelectedId();
                String currentFileLocation = getFilter()->getCsoundInputFile().getParentDirectory().getFullPathName();


                if(getFilter()->getGUICtrls(i).getStringProp("workingDir").isEmpty())
                    pluginDir = File(currentFileLocation);
                else
                    pluginDir = File(getFilter()->getGUICtrls(i).getStringProp("workingdir"));

                const String filetype = getFilter()->getGUICtrls(i).getStringProp("filetype");
                Array<File> dirFiles;
                pluginDir.findChildFiles(dirFiles, 2, false, filetype);
                cabCombo->combo->clear(dontSendNotification);
                for (int i = 0; i < dirFiles.size(); ++i)
                    if(filetype.contains("snaps"))
                        cabCombo->combo->addItem(dirFiles[i].getFileNameWithoutExtension(), i+1);
                    else
                        cabCombo->combo->addItem(dirFiles[i].getFileName(), i+1);

                cabCombo->combo->setSelectedId(currentItemID, dontSendNotification);
            }
        }
    }
}


//=========================================================================================
//			save and recall presets
//========================================================================================
void CabbagePluginAudioProcessorEditor::savePresetsFromParameters(File selectedFile, String mode)
{
    XmlElement xml (getFilter()->getCsoundInputFile().getFileNameWithoutExtension());
    for(int i=0; i<getFilter()->getGUICtrlsSize(); i++)
        xml.setAttribute(getFilter()->getGUICtrls(i).getStringProp(CabbageIDs::channel), getFilter()->getGUICtrls(i).getNumProp(CabbageIDs::value));

    File file(selectedFile.getFullPathName());
    file.replaceWithText(xml.createDocument(""));
}

void CabbagePluginAudioProcessorEditor::restoreParametersFromPresets(XmlElement* xmlState)
{
    ScopedPointer<XmlElement> xml;
    xml = xmlState;
    // make sure that it's actually our type of XML object..
    if (xml->hasTagName (getFilter()->getCsoundInputFile().getFileNameWithoutExtension()))
    {
        for(int i=0; i<getFilter()->getNumParameters(); i++)
        {
            float newValue = (float)xml->getDoubleAttribute(getFilter()->getGUICtrls(i).getStringProp(CabbageIDs::channel));

#ifndef Cabbage_Build_Standalone

            float range = getFilter()->getGUICtrls(i).getNumProp("range");
            float comboRange = getFilter()->getGUICtrls(i).getNumProp("comborange");
            //Logger::writeToLog("inValue:"+String(newValue));
            float min = getFilter()->getGUICtrls(i).getNumProp("min");

            if(getFilter()->getGUICtrls(i).getStringProp(CabbageIDs::type)=="rslider")
                Logger::writeToLog("slider");

            if(getFilter()->getGUICtrls(i).getStringProp(CabbageIDs::type)=="xypad")
                newValue = (jmax(0.f, newValue)/range)+min;
            else if(getFilter()->getGUICtrls(i).getStringProp(CabbageIDs::type)=="combobox")//combo box value need to be rounded...
                newValue = (newValue/comboRange);
            else if(getFilter()->getGUICtrls(i).getStringProp(CabbageIDs::type)=="checkbox" ||
                    getFilter()->getGUICtrls(i).getStringProp(CabbageIDs::type)=="button")
                range=1;
            else
                newValue = (newValue/range)+min;
#endif

            if(!getFilter()->getGUICtrls(i).getStringProp("filetype").contains("snaps")	)
                getFilter()->setParameter(i, newValue);
        }
    }
}
//==========================================================================================
//Gets called periodically to update GUI controls with values coming from Csound
//==========================================================================================



void CabbagePluginAudioProcessorEditor::updateGUIControls()
{
// update our GUI so that whenever a VST parameter is changed in the
// host the corresponding GUI control gets updated.


#ifndef Cabbage_No_Csound

//only allow changes to take place when we are NOT in gui edit mode
    if(!getFilter()->isGuiEnabled())
    {

        for(int y=0; y<getFilter()->getXYAutomaterSize(); y++)
        {
            if(getFilter()->getXYAutomater(y))
                getFilter()->getXYAutomater(y)->update();
        }

//don't check the whole vector here, only ones that have been updated
        for(int index=0; index<getFilter()->dirtyControls.size(); index++)
        {
            int i = getFilter()->dirtyControls[index];
            inValue = getFilter()->getParameter(i);
            if(i<getFilter()->getGUICtrlsSize())
            {
                if(getFilter()->getGUICtrls(i).getStringProp(CabbageIDs::type)==CabbageIDs::hslider||
                        getFilter()->getGUICtrls(i).getStringProp(CabbageIDs::type)==CabbageIDs::rslider||
                        getFilter()->getGUICtrls(i).getStringProp(CabbageIDs::type)==CabbageIDs::vslider)
                {
                    CabbageSlider* cabSlider = dynamic_cast<CabbageSlider*>(comps[i]);
                    if(cabSlider)
                    {
#ifndef Cabbage_Build_Standalone
                        float val = getFilter()->getGUICtrls(i).getNumProp(CabbageIDs::range)*getFilter()->getParameter(i)+
                                    getFilter()->getGUICtrls(i).getNumProp(CabbageIDs::min);
                        cabSlider->slider->setValue(val, dontSendNotification);
#else
                        //Logger::writeToLog(String(inValue));
                        cabSlider->slider->setValue(inValue, sendNotification);
#endif
                    }
                }

                else if(getFilter()->getGUICtrls(i).getStringProp(CabbageIDs::type)==CabbageIDs::button)
                {
                    CabbageButton* cabButton = dynamic_cast<CabbageButton*>(comps[i]);
                    if(cabButton)
                    {
                        //Logger::writeToLog("Button:"+String(inValue));
                        cabButton->button->setButtonText(getFilter()->getGUICtrls(i).getStringArrayPropValue(CabbageIDs::text, inValue));
                    }
                }

                else if(getFilter()->getGUICtrls(i).getStringProp(CabbageIDs::type)==CabbageIDs::xypad &&
                        getFilter()->getGUICtrls(i).getStringProp(CabbageIDs::xychannel).equalsIgnoreCase("x"))
                {
                    if(comps[i])
                    {
#ifndef Cabbage_Build_Standalone
                        float xRange = getFilter()->getGUICtrls(i).getNumProp(CabbageIDs::range);
                        float xMin = getFilter()->getGUICtrls(i).getNumProp(CabbageIDs::minx);
                        float yMin = getFilter()->getGUICtrls(i).getNumProp(CabbageIDs::miny);
                        float yRange = getFilter()->getGUICtrls(i+1).getNumProp(CabbageIDs::range);
                        ((CabbageXYController*)comps[i])->xypad->setXYValues(getFilter()->getParameter(i)*xRange+xMin, getFilter()->getParameter(i+1)*yRange+yMin);
#else
                        ((CabbageXYController*)comps[i])->xypad->setXYValues(getFilter()->getParameter(i), getFilter()->getParameter(i+1));
#endif
                    }
                }


                else if(getFilter()->getGUICtrls(i).getStringProp(CabbageIDs::type)==CabbageIDs::combobox)
                {
                    float val;
                    NotificationType notify;
                    if(getFilter()->getGUICtrls(i).getStringProp(CabbageIDs::filetype).contains("snaps"))
                        notify = sendNotification;
                    else
                        notify = dontSendNotification;
#ifdef Cabbage_Build_Standalone
                    val = getFilter()->getParameter(i);
                    ((CabbageComboBox*)comps[i])->combo->setSelectedItemIndex((int)val-1, notify);
                    //Logger::writeToLog(String("timerCallback():")+String(val));
#else
                    //needed to move combobox to full when controlled by a host
                    if(getFilter()->getParameter(i)>=0.98)
                        val = getFilter()->getGUICtrls(i).getNumProp(CabbageIDs::comborange);
                    else
                        val = getFilter()->getGUICtrls(i).getNumProp("comborange")*getFilter()->getParameter(i);
                    //Logger::writeToLog(String("timerCallback():")+String(val));
                    ((CabbageComboBox*)comps[i])->combo->setSelectedItemIndex(int(val)-1, notify);
#endif
                }

                else if(getFilter()->getGUICtrls(i).getStringProp(CabbageIDs::type)==CabbageIDs::checkbox)
                {
                    if(comps[i])
                    {
                        //update look of contorl if need be
                        if(getFilter()->getGUICtrls(i).getStringProp(CabbageIDs::identchannelmessage).isNotEmpty())
                            ((CabbageCheckbox*)comps[i])->update(getFilter()->getGUICtrls(i));
                        int val = getFilter()->getGUICtrls(i).getNumProp(CabbageIDs::value);
                        ((CabbageCheckbox*)comps[i])->button->setToggleState((bool)val, sendNotification);
                    }
                }
            }
        }

//now going to move through array again only this time updating position/sizes/colour, etc
        for(int index=0; index<(int)getFilter()->dirtyControls.size(); index++)
        {
            int i = getFilter()->dirtyControls[index];
            if(i<getFilter()->getGUICtrlsSize())
            {
                if(getFilter()->getGUICtrls(i).getStringProp(CabbageIDs::identchannelmessage).isNotEmpty())
                {
                    if(getFilter()->getGUICtrls(i).getStringProp(CabbageIDs::type)==CabbageIDs::hslider||
                            getFilter()->getGUICtrls(i).getStringProp(CabbageIDs::type)==CabbageIDs::rslider||
                            getFilter()->getGUICtrls(i).getStringProp(CabbageIDs::type)==CabbageIDs::vslider)
                    {
                        ((CabbageSlider*)comps[i])->update(getFilter()->getGUICtrls(i));
                    }

                    else if(getFilter()->getGUICtrls(i).getStringProp(CabbageIDs::type)==CabbageIDs::button)
                    {
                        ((CabbageButton*)comps[i])->update(getFilter()->getGUICtrls(i));
                        String buttonText = getFilter()->getGUICtrls(i).getStringArrayPropValue(CabbageIDs::text, getFilter()->getGUICtrls(i).getNumProp(CabbageIDs::value));
                        ((CabbageButton*)comps[i])->button->setButtonText(buttonText);
                        getFilter()->getGUICtrls(i).setStringProp(CabbageIDs::identchannelmessage, "");
                    }

                    else if(getFilter()->getGUICtrls(i).getStringProp(CabbageIDs::type)==CabbageIDs::checkbox)
                    {
                        ((CabbageCheckbox*)comps[i])->update(getFilter()->getGUICtrls(i));
                        getFilter()->getGUICtrls(i).setStringProp(CabbageIDs::identchannelmessage, "");
                    }

                    else if(getFilter()->getGUICtrls(i).getStringProp(CabbageIDs::type)==CabbageIDs::combobox)
                    {
                        ((CabbageComboBox*)comps[i])->update(getFilter()->getGUICtrls(i));
                        //int value = getFilter()->getGUICtrls(i).getNumProp(CabbageIDs::value);
                        ((CabbageComboBox*)comps[i])->combo->clear();
                        StringArray prop = getFilter()->getGUICtrls(i).getStringArrayProp(CabbageIDs::text);
                        for(int cnt=0; cnt<prop.size(); cnt++)
                            ((CabbageComboBox*)comps[i])->combo->addItem(getFilter()->getGUICtrls(i).getStringArrayPropValue(CabbageIDs::text, cnt), cnt+1);

                        ((CabbageComboBox*)comps[i])->combo->setSelectedItemIndex(getFilter()->getGUICtrls(i).getNumProp(CabbageIDs::value)-1);

                        getFilter()->getGUICtrls(i).setStringProp(CabbageIDs::identchannelmessage, "");
                    }
                    else if(getFilter()->getGUICtrls(i).getStringProp(CabbageIDs::type)==CabbageIDs::xypad)
                    {
                        ((CabbageXYController*)comps[i])->update(getFilter()->getGUICtrls(i));
                    }

                }
            }

        }

//getFilter()->dirtyControls.clear();
//the following code looks after updating any objects that don't get recognised as plugin parameters,
//for example, table objects don't get listed by the host as a paramters. Likewise the csoundoutput widget..
        for(int i=0; i<getFilter()->getGUILayoutCtrlsSize(); i++)
        {
            //csoundoutput
            if(getFilter()->getGUILayoutCtrls(i).getStringProp(CabbageIDs::type).containsIgnoreCase("csoundoutput"))
            {
                ((CabbageTextbox*)layoutComps[i])->editor->setText(getFilter()->getCsoundOutput());
                ((CabbageTextbox*)layoutComps[i])->editor->setCaretPosition(getFilter()->getCsoundOutput().length());
                if(getFilter()->getGUILayoutCtrls(i).getStringProp(CabbageIDs::identchannelmessage).isNotEmpty())
                    ((CabbageTextbox*)layoutComps[i])->update(getFilter()->getGUILayoutCtrls(i));
                getFilter()->getGUILayoutCtrls(i).setStringProp(CabbageIDs::identchannelmessage, "");
            }
            //label
            else if(getFilter()->getGUILayoutCtrls(i).getStringProp(CabbageIDs::type).equalsIgnoreCase("label") &&
                    getFilter()->getGUILayoutCtrls(i).getStringProp(CabbageIDs::identchannelmessage).isNotEmpty())
            {
                ((CabbageLabel*)layoutComps[i])->update(getFilter()->getGUILayoutCtrls(i));
                getFilter()->getGUILayoutCtrls(i).setStringProp(CabbageIDs::identchannelmessage, "");
            }
            //textbox
            else if(getFilter()->getGUILayoutCtrls(i).getStringProp(CabbageIDs::type).equalsIgnoreCase("textbox") &&
                    getFilter()->getGUILayoutCtrls(i).getStringProp(CabbageIDs::identchannelmessage).isNotEmpty())
            {
                ((CabbageTextbox*)layoutComps[i])->update(getFilter()->getGUILayoutCtrls(i));
                getFilter()->getGUILayoutCtrls(i).setStringProp(CabbageIDs::identchannelmessage, "");
            }
            //groupbox
            else if(getFilter()->getGUILayoutCtrls(i).getStringProp(CabbageIDs::type).equalsIgnoreCase("groupbox") &&
                    getFilter()->getGUILayoutCtrls(i).getStringProp(CabbageIDs::identchannelmessage).isNotEmpty())
            {
                String message = getFilter()->getGUILayoutCtrls(i).getStringProp(CabbageIDs::identchannelmessage);
                if(message.contains("show(1)") && getFilter()->getGUILayoutCtrls(i).getNumProp(CabbageIDs::popup)==1)
                {
                    int index = ((CabbageGroupbox*)layoutComps[i])->getProperties().getWithDefault(String("popupPlantIndex"), 0);
                    if(subPatches[index])
                    {
                        //getFilter()->getGUILayoutCtrls(i).setNumProp(CabbageIDs::left, 0);
                        //getFilter()->getGUILayoutCtrls(i).setNumProp(CabbageIDs::top, 0);
                        subPatches[index]->setVisible(true);
                        subPatches[index]->setAlwaysOnTop(true);
                        subPatches[index]->toFront(true);
                    }
                }
                ((CabbageGroupbox*)layoutComps[i])->update(getFilter()->getGUILayoutCtrls(i));
                getFilter()->getGUILayoutCtrls(i).setStringProp(CabbageIDs::identchannelmessage, "");
            }
            //soundfiler
            else if(getFilter()->getGUILayoutCtrls(i).getStringProp(CabbageIDs::type).equalsIgnoreCase("soundfiler") &&
                    getFilter()->getGUILayoutCtrls(i).getStringProp(CabbageIDs::identchannelmessage).isNotEmpty())
            {
                String message = getFilter()->getGUILayoutCtrls(i).getStringProp(CabbageIDs::identchannelmessage);
                if(message.contains("tablenumber")||message.contains("tablenumbers"))
                {
                    int numberOfTables = getFilter()->getGUILayoutCtrls(i).getStringArrayProp(CabbageIDs::tablenumber).size();
                    tableBuffer.clear();
                    for(int y=0; y<numberOfTables; y++)
                    {
                        int tableNumber = getFilter()->getGUILayoutCtrls(i).getIntArrayPropValue(CabbageIDs::tablenumber, y);
                        tableValues.clear();
                        tableValues = getFilter()->getTableFloats(tableNumber);
                        if(tableBuffer.getNumSamples()<tableValues.size())
                            tableBuffer.setSize(numberOfTables, tableValues.size());
                        tableBuffer.addFrom(y, 0, tableValues.getRawDataPointer(), tableValues.size());
                    }
                    ((CabbageSoundfiler*)layoutComps[i])->setWaveform(tableBuffer, numberOfTables);

                }
                else if(message.contains("file("))
                    ((CabbageSoundfiler*)layoutComps[i])->setFile(getFilter()->getGUILayoutCtrls(i).getStringProp(CabbageIDs::file));

                ((CabbageSoundfiler*)layoutComps[i])->update(getFilter()->getGUILayoutCtrls(i));
                getFilter()->getGUILayoutCtrls(i).setStringProp(CabbageIDs::identchannelmessage, "");
            }
            //image
            else if(getFilter()->getGUILayoutCtrls(i).getStringProp(CabbageIDs::type).equalsIgnoreCase("image") &&
                    getFilter()->getGUILayoutCtrls(i).getStringProp(CabbageIDs::identchannelmessage).isNotEmpty())
            {
                ((CabbageImage*)layoutComps[i])->update(getFilter()->getGUILayoutCtrls(i));
                getFilter()->getGUILayoutCtrls(i).setStringProp(CabbageIDs::identchannelmessage, "");
                repaint();
            }
            //table
            else if((getFilter()->getGUILayoutCtrls(i).getStringProp(CabbageIDs::type)==CabbageIDs::table) &&
                    getFilter()->getGUILayoutCtrls(i).getStringProp(CabbageIDs::identchannelmessage).isNotEmpty())
            {
                String message = getFilter()->getGUILayoutCtrls(i).getStringProp(CabbageIDs::identchannelmessage);
                if(message.contains("tablenumber")||message.contains("tablenumbers"))
                {
                    int numberOfTables = getFilter()->getGUILayoutCtrls(i).getStringArrayProp(CabbageIDs::tablenumber).size();
                    for(int y=0; y<numberOfTables; y++)
                    {
                        int tableNumber = getFilter()->getGUILayoutCtrls(i).getIntArrayPropValue(CabbageIDs::tablenumber, y);
                        tableValues.clear();
                        tableValues = getFilter()->getTableFloats(tableNumber);
                        ((CabbageTable*)layoutComps[i])->fillTable(y, tableValues);
                    }
                }
                getFilter()->getGUILayoutCtrls(i).setStringProp(CabbageIDs::identchannelmessage, "");
            }
            //gentable
            else if((getFilter()->getGUILayoutCtrls(i).getStringProp(CabbageIDs::type)==CabbageIDs::gentable) &&
            		getFilter()->getGUILayoutCtrls(i).getStringProp(CabbageIDs::identchannelmessage).isNotEmpty())
            		{
            		String message = getFilter()->getGUILayoutCtrls(i).getStringProp(CabbageIDs::identchannelmessage);
            		if(message.contains("tablenumber")||message.contains("tablenumbers"))
            			{
            			int numberOfTables = getFilter()->getGUILayoutCtrls(i).getStringArrayProp(CabbageIDs::tablenumber).size();
            			for(int y=0;y<numberOfTables;y++)
            				{
            				tableBuffer.clear();
            				int tableNumber = getFilter()->getGUILayoutCtrls(i).getIntArrayPropValue(CabbageIDs::tablenumber, y);
            				TableManager* table = dynamic_cast<CabbageGenTable*>(layoutComps[i])->table;
            				StringArray pFields = getFilter()->getTableStatement(tableNumber);
            				//tableValues.clear();
            				tableValues = getFilter()->getTableFloats(tableNumber);
            				table->setWaveform(tableValues, tableNumber, false);
            				}
						}
					((CabbageGenTable*)layoutComps[i])->update(getFilter()->getGUILayoutCtrls(i));
            		getFilter()->getGUILayoutCtrls(i).setStringProp(CabbageIDs::identchannelmessage, "");
            	}
        }


#ifdef Cabbage_Build_Standalone
        //make sure that the instrument needs midi before turning this on
        MidiMessage message(0xf4, 0, 0, 0);

        if(!getFilter()->ccBuffer.isEmpty())
        {
            MidiBuffer::Iterator y(getFilter()->ccBuffer);
            int messageFrameRelativeTothisProcess;
            while (y.getNextEvent (message, messageFrameRelativeTothisProcess))
            {
                if(message.isController())
                    for(int i=0; i<(int)getFilter()->getGUICtrlsSize(); i++)
                        if((message.getChannel()==getFilter()->getGUICtrls(i).getNumProp(CabbageIDs::midichan))&&
                                (message.getControllerNumber()==getFilter()->getGUICtrls(i).getNumProp(CabbageIDs::midictrl)))
                        {
                            float value = message.getControllerValue()/127.f*
                                          (getFilter()->getGUICtrls(i).getNumProp("max")-getFilter()->getGUICtrls(i).getNumProp("min")+
                                           getFilter()->getGUICtrls(i).getNumProp("min"));

                            if(getFilter()->getGUICtrls(i).getStringProp(CabbageIDs::type)==String(CabbageIDs::hslider)||
                                    getFilter()->getGUICtrls(i).getStringProp(CabbageIDs::type)==String(CabbageIDs::rslider)||
                                    getFilter()->getGUICtrls(i).getStringProp(CabbageIDs::type)==String(CabbageIDs::vslider))
                            {
                                if(comps[i])
                                    ((CabbageSlider*)comps[i])->slider->setValue(value, dontSendNotification);
                            }
                            else if(getFilter()->getGUICtrls(i).getStringProp(CabbageIDs::type)==String(CabbageIDs::button))
                            {
                                if(comps[i])
                                    ((CabbageButton*)comps[i])->button->setButtonText(getFilter()->getGUICtrls(i).getStringArrayPropValue("text", 1-(int)value));
									//setButtonText(getFilter()->getGUICtrls(i).getItems(1-(int)value));
                            }
                            else if(getFilter()->getGUICtrls(i).getStringProp(CabbageIDs::type)==String(CabbageIDs::combobox))
                            {
                                if(comps[i])
                                {
                                    //((CabbageComboBox*)comps[i])->combo->setSelectedId((int)value+1.5, false);
                                }
                            }
                            else if(getFilter()->getGUICtrls(i).getStringProp(CabbageIDs::type)==String(CabbageIDs::checkbox))
                            {
                                if(comps[i])
                                    if(value==0)
                                        ((CabbageCheckbox*)comps[i])->button->setToggleState(0, dontSendNotification);
                                    else
                                        ((Button*)comps[i])->setToggleState(1, dontSendNotification);
                            }
                            getFilter()->getCsound()->SetChannel(getFilter()->getGUICtrls(i).getStringProp(CabbageIDs::channel).toUTF8(), value);
                            if(message.isController())
                                if(getFilter()->getMidiDebug())
                                {
                                    String debugInfo =  String("MIDI Channel:    ")+String(message.getChannel())+String(" \t")+
                                                        String("MIDI Controller: ")+String(message.getControllerNumber())+String("\t")+
                                                        String("MIDI Value:      ")+String(message.getControllerValue())+String("\n");
                                    getFilter()->addDebugMessage(debugInfo);
                                    getFilter()->sendChangeMessage();
                                }
                        }
            }
        }
        getFilter()->ccBuffer.clear();
#endif
    }

#endif


}

//==============================================================================
//update frames displayed by layout editor
//==============================================================================
void CabbagePluginAudioProcessorEditor::updateLayoutEditorFrames()
{
#ifdef Cabbage_Build_Standalone
    layoutEditor->updateFrames();
#endif
}