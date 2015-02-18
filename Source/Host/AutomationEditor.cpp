/*
  Copyright (c) 2015 - Rory Walsh

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

#include "AutomationProcessor.h"
#include "AutomationEditor.h"

#define BUTTON_SIZE 25
//==============================================================================
AutomationEditor::AutomationEditor (AutomationProcessor& p): 
AudioProcessorEditor(&p), 
processor(p),
tableManager(),
popupBubble(500),
playButton("playButton", DrawableButton::ImageOnButtonBackground),
stopButton("stopButton", DrawableButton::ImageOnButtonBackground),
autoCombo(),
basicLook()
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
	addAndMakeVisible(&tableManager);

    popupBubble.setColour(BubbleComponent::backgroundColourId, Colours::white);
    popupBubble.setBounds(0, 0, 50, 20);
    addChildComponent(popupBubble);
	addAndMakeVisible(autoCombo);
    popupBubble.setAlwaysOnTop(true);
	setOpaque(false);
	
	playButton.addListener(this);
	addAndMakeVisible(&playButton);
	stopButton.addListener(this);
	addAndMakeVisible(&stopButton);
	playButton.setLookAndFeel(&basicLook);
	stopButton.setLookAndFeel(&basicLook);	
	playButton.setColour(TextButton::buttonColourId, Colours::white);
	playButton.setColour(TextButton::buttonOnColourId, Colours::yellow);
	playButton.setClickingTogglesState(true);	
	stopButton.setColour(TextButton::buttonColourId, Colours::white);	
	
	Path playPath;
	playPath.addTriangle(0, 0, BUTTON_SIZE, BUTTON_SIZE/2, 0, BUTTON_SIZE);
	DrawablePath playImage;
	playImage.setFill(Colours::green.darker(.9f));
	playImage.setPath(playPath);
	
	Path pausePath;
	pausePath.addRectangle(0, 0, BUTTON_SIZE*.4, BUTTON_SIZE);
	pausePath.addRectangle(BUTTON_SIZE*.5, 0, BUTTON_SIZE*.4, BUTTON_SIZE);
	DrawablePath pauseImage;
	pauseImage.setFill(Colours::green.darker(.9f));
	pauseImage.setPath(pausePath);
	playButton.setImages(&playImage, &playImage, &pauseImage, &playImage, &pauseImage);
	
	Path stopPath;
	stopPath.addRectangle(0, 0, BUTTON_SIZE, BUTTON_SIZE);
	DrawablePath stopImage;
	stopImage.setFill(Colours::green.darker(.9f));
	stopImage.setPath(stopPath);
	stopButton.setImages(&stopImage);	
	
	ampRange.add(0);
	ampRange.add(1);
	ampRange.add(-1);	
	
	if(getFilter())
	{
		for(int i=0;i<getFilter()->getNumberOfAutomatableNodes();i++)
			addTable(getFilter()->getAutomatableNode(i).fTableNumber, getFilter()->getAutomatableNode(i).genRoutine); 
	}
	
	updateComboBoxItems();
	
    setSize (200, 400);		
}

AutomationEditor::~AutomationEditor()
{
}

//==============================================================================
void AutomationEditor::paint (Graphics& g)
{
    g.fillAll (Colours::black);
}

void AutomationEditor::resized()
{
	tableManager.setBounds(10, 10, getWidth()-20, getHeight()-50);
	stopButton.setBounds(10, getHeight()-35, BUTTON_SIZE, BUTTON_SIZE);
	playButton.setBounds(BUTTON_SIZE+15, getHeight()-35, BUTTON_SIZE, BUTTON_SIZE);
	autoCombo.setBounds((BUTTON_SIZE*3)+20, getHeight()-35, BUTTON_SIZE*10, BUTTON_SIZE);
}
//==============================================================================
void AutomationEditor::updateComboBoxItems()
{
	autoCombo.clear();
	for(int i=0;i<getFilter()->getNumberOfAutomatableNodes();i++)
	{
		String info = getFilter()->getAutomatableNode(i).nodeName+": "+getFilter()->getAutomatableNode(i).parametername;
		autoCombo.addItem(info, i+1);
		autoCombo.setSelectedId(i+1);
	}
}
	
//==============================================================================
void AutomationEditor::changeListenerCallback(ChangeBroadcaster* source)
{
	GenTable* genTable = dynamic_cast<GenTable*>(source);
	if(genTable)
	{
		if((genTable->getCurrentHandle() && genTable->displayAsGrid()!=1))
			popupBubble.showAt(genTable->getCurrentHandle(), AttributedString(genTable->getCoordinates()), 1050);		
		if(genTable->changeMessage == "updateFunctionTable")
			updatefTableData(genTable);
	}	
}
//==============================================================================
void AutomationEditor::addTable(int tableNumber, int genRoutine)
{
	
	const Colour tableColour(Colour(Random::getSystemRandom().nextInt(255),
							 Random::getSystemRandom().nextInt(255),
							 Random::getSystemRandom().nextInt(255)));
	
	Array <float, CriticalSection> tableValues;
	//tableValues.clear();
	tableValues = getFilter()->getTableFloats(tableNumber);
	if(tableValues.size()>0)
	{
		tableManager.setZoomFactor(0);
		StringArray pFields = getFilter()->getTableStatement(tableNumber);
		int genRoutine = pFields[4].getIntValue();
	
		if(getFilter()->compiledOk()==OK)
		{
			tableManager.addTable(44100, 
							tableColour, 
							genRoutine,
							ampRange,
							tableNumber, this);
							
			tableNumbers.append(tableNumber);
			tableManager.setWaveform(tableValues, tableNumber);
			tableManager.enableEditMode(pFields, tableNumber);
			tableManager.setOutlineThickness(1);		
			tableManager.setGridColour(Colour(20, 20, 20));		
			tableManager.setBackgroundColour(Colour(10, 10, 10));				
			tableManager.showTableButtons(false);
			tableManager.bringTableToFront(tableNumber);
		}
	}

	
	
}
//==============================================================================
void AutomationEditor::buttonClicked(Button* button)
{
	if(button->getName()=="playButton")
	{
		if(button->getToggleState()==true)
		{
			getFilter()->suspendProcessing(false);	
			getFilter()->messageQueue.addOutgoingChannelMessageToQueue("isPlaying", 1.0, "");
		}	
		else
		{
			getFilter()->suspendProcessing(true);	
			getFilter()->messageQueue.addOutgoingChannelMessageToQueue("isPlaying", 0.0, "");
		}
	}
	
	if(button->getName()=="stopButton")
	{
		getFilter()->messageQueue.addOutgoingChannelMessageToQueue("isPlaying", 0.0, "");
		getFilter()->suspendProcessing(true);	
		if(playButton.getToggleState()==true)
			playButton.setToggleState(false, dontSendNotification);
	}
	
}
//==============================================================================
void AutomationEditor::updateScrubberPosition(double position, int ftable)
{
	tableManager.setScrubberPos(position, ftable);
}

//================================================================================
void AutomationEditor::updatefTableData(GenTable* table)
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
		int pCnt=0;
        for(int i=0; i<evt.pcnt-1; i++)
		{
            fStatement.add(String(evt.p[i]));
			cUtils::debug(i, fStatement[i]);
			pCnt=i;
		}
		
		if(table->genRoutine!=2)
		{
			fStatement.add(String(1));
			fStatement.add(String(evt.p[pCnt]));			
		}

        //now set table number and set score char to f
        fStatement.set(1, String(table->tableNumber));
        fStatement.set(0, "f");

        //Logger::writeToLog(fStatement.joinIntoString(" "));
        getFilter()->getCsoundStruct()->hfgens( getFilter()->getCsoundStruct(), &ftpp, &evt, 1);
        Array<float, CriticalSection> points;

        points = Array<float, CriticalSection>(ftpp->ftable, ftpp->flen);
        table->setWaveform(points, false);
        //table->enableEditMode(fStatement);

        getFilter()->messageQueue.addOutgoingTableUpdateMessageToQueue(fStatement.joinIntoString(" "), table->tableNumber);
    }
#endif
}