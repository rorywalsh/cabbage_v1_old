/*
  Copyright (C) 2009 Rory Walsh

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

#include "Table.h"

//==============================================================================
// Class to hold all tables
//==============================================================================
TableManager::TableManager(): Component(), zoom(0.0)
{
	addAndMakeVisible(zoomIn = new RoundButton("zoomIn", Colours::white));
	addAndMakeVisible(zoomOut = new RoundButton("zoomOut", Colours::white));
    zoomIn->toFront(false);
	zoomIn->addChangeListener(this);
    zoomOut->toFront(false);	
	zoomOut->addChangeListener(this);
}	
//==============================================================================	
void TableManager::changeListenerCallback(ChangeBroadcaster *source)
{	
	RoundButton* button = dynamic_cast<RoundButton*>(source);
    if(button)
    {
        if(button->getName()=="zoomIn")
        {
            zoom+=0.1;
			for(int i=0;i<tables.size();i++)
            tables[i]->setZoomFactor(jmin(0.9, zoom));
        }
        else if(button->getName()=="zoomOut")
        {   
			//zoom out in one go....
			for(int i=0;i<tables.size();i++)
			tables[i]->setZoomFactor(0.0);
			zoom = 0.0;
        }
		else{
			for(int i =0; i<tables.size();i++)
			if(button->getName().getIntValue()==tables[i]->tableNumber)
				{
				tables[i]->toFront(true);
				tables[i]->getHandleViewer()->showHandles(true);
				}
			else
				tables[i]->getHandleViewer()->showHandles(false);
			bringButtonsToFront();
		}
        repaint();
    }
}	
//==============================================================================	
void TableManager::addTable(int sr, const String col, int gen, Array<float> ampRange, int ftnumber, ChangeListener* listener)
{
	GenTable* table = new GenTable();	
	table->addChangeListener(listener);
	table->addChangeListener(this);
	table->addTable(sr, col, gen, ampRange);
	table->tableNumber = ftnumber;
	table->scrollbar->addListener(this);
	table->addChangeListener(listener);
	addAndMakeVisible(table);
	tables.add(table);
	
	RoundButton* button = new RoundButton(String(ftnumber), Colours::findColourForName(col, Colours::white));
	button->addChangeListener(this);
	addAndMakeVisible(button);
	tableButtons.add(button);
	
	resized();	
}

void TableManager::scrollBarMoved (ScrollBar* scrollBarThatHasMoved, double newRangeStart)
{
	ScrollBar* scroll = dynamic_cast<ScrollBar*>(scrollBarThatHasMoved);
    if(scroll)
    {
		for(int i=0;i<tables.size();i++)
		{
			float moveBy = newRangeStart/scrollBarThatHasMoved->getCurrentRange().getLength();
			moveBy = tables[i]->visibleRange.getLength()*moveBy;
			//Logger::writeToLog("table_range:"+String(scrollBarThatHasMoved->getCurrentRange().getLength()));
			tables[i]->setRange(tables[i]->visibleRange.movedToStartAt (moveBy), true);
		}
	}
			
}
//==============================================================================
void TableManager::resized()
{
	for(int i =0; i<tables.size();i++){
		tables[i]->setBounds(0, 0, getWidth(), getHeight());
	}	
		
	bringButtonsToFront();
}
//==============================================================================
void TableManager::bringButtonsToFront()
{
    zoomIn->setBounds(getWidth()-43, getHeight()-20, 20, 20);
	zoomIn->toFront(true);
    zoomOut->setBounds(getWidth()-20, getHeight()-20, 20, 20);	
	zoomOut->toFront(true);
	for(int i=0;i<tableButtons.size();i++)
	{
		tableButtons[i]->setBounds(getWidth()-65-(i*18), getHeight()-18, 15, 15);
		tableButtons[i]->toFront(true);				
	}	
}
//==============================================================================
void TableManager::setWaveform(AudioSampleBuffer buffer, int ftNumber)
{
	for( int i=0;i<tables.size();i++)
		if(ftNumber==tables[i]->tableNumber)
			tables[i]->setWaveform(buffer);	
}
//==============================================================================
void TableManager::setWaveform(Array<float, CriticalSection> buffer, int ftNumber, bool updateRange)
{
	for( int i=0;i<tables.size();i++)
		if(ftNumber==tables[i]->tableNumber)
			tables[i]->setWaveform(buffer, updateRange);		
}
//==============================================================================
void TableManager::enableEditMode(StringArray pFields, int ftNumber)
{
	for( int i=0;i<tables.size();i++)
		if(ftNumber==tables[i]->tableNumber)
			tables[i]->enableEditMode(pFields);
}	
//==============================================================================
void TableManager::bringTableToFront(int ftNumber)
{
	for( int i=0;i<tables.size();i++)
	{
		tables[i]->scrollbarReduction = (tables.size()*20)+50;
		if(ftNumber==tables[i]->tableNumber)
		{
			tables[i]->toFront(true);
		}
		tables[i]->resized();
	}		
	
	bringButtonsToFront();		
}

//==============================================================================
// GenTable display  component
//==============================================================================
GenTable::GenTable():	thumbnailCache (5),
    currentPlayPosition(0),
    mouseDownX(0),
    mouseUpX(0),
    drawWaveform(false),
    regionWidth(1),
    loopLength(0),
    scrubberPosition(0),
    currentPositionMarker(new DrawableRectangle()),
    genRoutine(0),
    zoom(0.0),
    currentWidth(0),
    normalised(0),
    tableNumber(-1)
{
    thumbnail=nullptr;
    addAndMakeVisible(scrollbar = new ScrollBar(false));
    scrollbar->setRangeLimits (visibleRange);
    //scrollbar->setAutoHide (false);
    scrollbar->addListener(this);
    currentPositionMarker->setFill (Colours::white.withAlpha (0.85f));
    addAndMakeVisible(currentPositionMarker);
    
    handleViewer = new HandleViewer();
    addAndMakeVisible(handleViewer);

    minMax.setStart(0);
    minMax.setEnd(0);
	handleViewer->minMax = minMax;
}
//==============================================================================
GenTable::~GenTable()
{
    scrollbar->removeListener (this);
    if(thumbnail)
        thumbnail->removeChangeListener (this);
}
//==============================================================================
void GenTable::addTable(int sr, const String col, int gen, Array<float> ampRange)
{
    sampleRate = sr;
    colour = Colours::findColourForName(col, Colours::white);
    genRoutine = gen;
	handleViewer->gen = gen;
	handleViewer->colour = Colours::findColourForName(col, Colours::white);
    if(ampRange.size()>1)
    {
        minMax.setStart(ampRange[0]);
        minMax.setEnd(ampRange[1]);
		handleViewer->minMax = minMax;
    }
    //set up table according to type of GEN used to create it
    if(genRoutine==1)
    {
        formatManager.registerBasicFormats();
        thumbnail = new AudioThumbnail(2, formatManager, thumbnailCache);
        thumbnail->addChangeListener (this);
        setZoomFactor (0.0);
    }
}
//==============================================================================
void GenTable::changeListenerCallback(ChangeBroadcaster *source)
{
    HandleComponent* handle = dynamic_cast<HandleComponent*>(source);

    if(handle)
    {
        changeMessage = "updateFunctionTable";
        sendChangeMessage();
    }


}
//==============================================================================
void GenTable::resized()
{
    handleViewer->setSize(getWidth(), getHeight()-25);

	
	Logger::writeToLog(String(getLocalBounds().getHeight()-20));
    if(scrollbar)
        scrollbar->setBounds (getLocalBounds().withWidth(getWidth()-scrollbarReduction).removeFromBottom (20).reduced (2));
}
//==============================================================================

void GenTable::scrollBarMoved (ScrollBar* scrollBarThatHasMoved, double newRangeStart)
{
	//visibleRange = visibleRange.movedToStartAt (newRangeStart);
	//setRange (visibleRange.movedToStartAt (newRangeStart), true);	
	//sendChangeMessage();
}

void GenTable::setFile (const File& file)
{
    if (!file.isDirectory())
    {
        genRoutine==1;
        AudioFormatManager format;
        format.registerBasicFormats();
        //registers wav and aif format (just nescearry one time if you alays use the "getInstance()" method)
        AudioFormatReader* reader = format.createReaderFor(file);
        //creates a reader for the result file (may file, if the result/opened file is no wav or aif)
        if(reader) //if a reader got created
        {
            AudioSampleBuffer buffer(reader->numChannels, reader->lengthInSamples);
            buffer.clear();
            buffer.setSize(reader->numChannels, reader->lengthInSamples);
            reader->read(&buffer,0, buffer.getNumSamples(), 0, true, true);
            setWaveform(buffer);
        }
        delete reader;
    }
    repaint(0, 0, getWidth(), getHeight());
}

//==============================================================================
Array<double> GenTable::getPfields()
{
    Array<double> values;
    double prevXPos=0, currXPos=0;

    for(int i=0; i<handleViewer->handles.size(); i++)
    {
        currXPos = handleViewer->handles[i]->xPosRelative*waveformBuffer.size();
		Logger::writeToLog(String(handleViewer->handles.size()));
		//add x position
        values.add(jmax(0.0, currXPos-prevXPos));

		//hack to prevent csound from bawking with a 0 in gen05
		float amp = pixelToAmp(handleViewer->getHeight(), minMax, handleViewer->handles[i]->getPosition().getY());
		if(abs(genRoutine)==5)
			amp = jmax(0.001f, amp);
		else
			amp = jmax(0.f, amp);        
		//add y position
		values.add(amp);

        prevXPos = roundToIntAccurate(handleViewer->handles[i]->xPosRelative*waveformBuffer.size());
    }
    return values;
}

//==============================================================================
void GenTable::setWaveform(AudioSampleBuffer buffer)
{
    if(genRoutine==1)
    {
        tableSize = buffer.getNumSamples();
        thumbnail->clear();
        repaint();
		Logger::writeToLog(String(buffer.getNumChannels()));
        thumbnail->reset(buffer.getNumChannels(), 44100, buffer.getNumSamples());
        thumbnail->addBlock(0, buffer, 0, buffer.getNumSamples());
        const Range<double> newRange (0.0, thumbnail->getTotalLength());
        scrollbar->setRangeLimits (newRange);
        setRange (newRange);
        setZoomFactor(zoom);
        Logger::writeToLog("updating waveform:Length "+String(thumbnail->getTotalLength()));
        repaint();
    }
}

void GenTable::setWaveform(Array<float, CriticalSection> buffer, bool updateRange)
{
    if(genRoutine != 1)
    {
        waveformBuffer = buffer;
        tableSize = buffer.size();
        handleViewer->tableSize = tableSize;

        if(buffer.size() > 22050)
            CabbageUtils::showMessage("Tables of sizes over 22050 samples should be created using GEN01", &this->getLookAndFeel());

        if(updateRange == true)
        {
            const Range<double> newRange (0.0, buffer.size()/sampleRate);
            scrollbar->setRangeLimits (newRange);
            setRange (newRange);
            setZoomFactor (zoom);
        }

        if(minMax.getLength()==0)
		{
            minMax = findMinMax(buffer);
			handleViewer->minMax = minMax;
		}

        repaint();
    }

}
//==============================================================================
void GenTable::enableEditMode(StringArray pFields)
{
    //if dealing with normalised table check pfield amps..
    Array<float, CriticalSection> pFieldAmps;
    pFieldAmps.add (pFields[5].getFloatValue());

    for(int i=6; i<pFields.size(); i+=2)
        pFieldAmps.add(pFields[i+1].getFloatValue());

    Range<float> pFieldMinMax = findMinMax(pFieldAmps);
    normalised = pFields[4].getIntValue();
    float xPos = 0;
    handleViewer->handles.clear();
    if(pFields.size()>0)
    {
        if(abs(genRoutine)==7 || abs(genRoutine)==5)
        {
            float pFieldAmpValue = (normalised<0 ? pFields[5].getFloatValue() : pFields[5].getFloatValue()/pFieldMinMax.getEnd());
            handleViewer->addHandle(0, ampToPixel(handleViewer->getHeight(), minMax, pFieldAmpValue));

            Logger::writeToLog("Coordinates:("+String(xPos)+", "+String(ampToPixel(handleViewer->getHeight(), minMax, pFieldAmpValue))+")");

            for(int i=6; i<pFields.size(); i+=2)
            {
                xPos = xPos + pFields[i].getFloatValue();
                pFieldAmpValue = (normalised<0 ? pFields[i+1].getFloatValue() : pFields[i+1].getFloatValue()/pFieldMinMax.getEnd());
                handleViewer->addHandle(xPos/(float)waveformBuffer.size(), ampToPixel(handleViewer->getHeight(), minMax, pFieldAmpValue));
                Logger::writeToLog("Coordinates:("+String(xPos)+", "+String(ampToPixel(handleViewer->getHeight(), minMax, pFieldAmpValue))+")");
            }

        }
        handleViewer->fixEdgePoints();

    }

}

//==============================================================================
void GenTable::setZoomFactor (double amount)
{
    zoom = amount;


    if(genRoutine==1)
    {
        if (thumbnail->getTotalLength() > 0)
        {
            const double newScale = jmax (0.001, thumbnail->getTotalLength() * (1.0 - jlimit (0.0, 0.99, amount)));
            const double timeAtCentre = xToTime (getWidth() / 2.0f);
			if(amount!=0)
				setRange (Range<double> (timeAtCentre - newScale * 0.5, timeAtCentre + newScale * 0.5));
			else
				setRange (Range<double> (0, thumbnail->getTotalLength()));
        }
    }
    else
    {
        const double newScale = jmax (0.00001, waveformBuffer.size()/sampleRate * (1.0 - jlimit (0.0, 0.9999, amount)));
        const double timeAtCentre = xToTime (getWidth() / 2.0f);
		if(amount!=0)
			setRange (Range<double> (timeAtCentre - newScale * 0.5, timeAtCentre + newScale * 0.5));
		else
			setRange (Range<double> (0, waveformBuffer.size()/sampleRate));
    }

    repaint();
}

//==============================================================================
void GenTable::mouseWheelMove (const MouseEvent&, const MouseWheelDetails& wheel)
{
    /*
    if(genRoutine==1)
    {
    	if (thumbnail->getTotalLength() > 0.0)
    	{
    		double newStart = visibleRange.getStart() - wheel.deltaX * (visibleRange.getLength()) / 10.0;
    		newStart = jlimit (0.0, jmax (0.0, thumbnail->getTotalLength() - (visibleRange.getLength())), newStart);
    		setRange (Range<double> (newStart, newStart + visibleRange.getLength()));
    		repaint();
    	}
    }
    else
    {
    		double newStart = visibleRange.getStart() - wheel.deltaX * (visibleRange.getLength()) / 10.0;
    		newStart = jlimit (0.0, jmax (0.0, thumbnail->getTotalLength() - (visibleRange.getLength())), newStart);
    		setRange (Range<double> (newStart, newStart + visibleRange.getLength()));
    		repaint();
    }
     */
}

//==============================================================================
void GenTable::setRange(Range<double> newRange, bool isScrolling)
{
    visibleRange = newRange;
    //set visible ranges in samples...
    scrollbar->setCurrentRange (visibleRange, dontSendNotification);
	Logger::writeToLog("TableNumber:"+String(tableNumber));
    Logger::writeToLog("VisibleStart:"+String(visibleRange.getStart()*sampleRate));
    Logger::writeToLog("VisibleEnd:"+String(visibleRange.getEnd()*sampleRate));
    Logger::writeToLog("VisibleLength:"+String(visibleRange.getLength()*sampleRate));
	Logger::writeToLog("Zoom:"+String(zoom));

    if(genRoutine!=1)
    {
		
		visibleStart = visibleRange.getStart()*sampleRate;
		visibleEnd = visibleRange.getEnd()*sampleRate;
		visibleLength = visibleRange.getLength()*sampleRate;
        if(!isScrolling)
        {
            double newWidth = double(getWidth())*(double(waveformBuffer.size())/visibleLength);
            double leftOffset = newWidth*(visibleStart/(double)waveformBuffer.size());
            Logger::writeToLog("width:"+String(newWidth));
			handleViewer->setSize(newWidth, handleViewer->getHeight());	
            handleViewer->setTopLeftPosition(-leftOffset, 0);
        }
        else
        {
            double leftOffset = handleViewer->getWidth()*(visibleStart/(double)waveformBuffer.size());
            handleViewer->setTopLeftPosition(-leftOffset, 0);
        }

    }
	repaint();
}
//==============================================================================
void GenTable::paint (Graphics& g)
{
    g.fillAll (Colours::transparentBlack);
    g.setColour (colour);
    Rectangle<int> thumbArea (getLocalBounds());
    thumbArea.setHeight(getHeight()-14);
    thumbArea.setTop(10.f);
    float prevY=thumbArea.getHeight(), prevX=-1, currY=0, currX=1000;

    //if gen01 then use an audio thumbnail class
    if(genRoutine==1)
    {
        thumbnail->drawChannels(g, thumbArea.reduced (2), visibleRange.getStart(), visibleRange.getEnd(), .8f);
        g.setColour(colour.contrasting(.5f).withAlpha(.7f));
        float zoomFactor = thumbnail->getTotalLength()/visibleRange.getLength();
        regionWidth = (regionWidth=2 ? 2 : regionWidth*zoomFactor);
        //g.fillRect(timeToX(currentPlayPosition), 10.f, (regionWidth==2 ? 2 : regionWidth*zoomFactor), (float)getHeight()-26.f);
    }

    //else draw the waveform directly
    else
    {
        double numPixelsPerIndex = (double)thumbArea.getWidth() / visibleLength;
        double waveformThickness = 4;
        double thumbHeight = thumbArea.reduced(2).getHeight();
        for(int i=visibleStart+1; i<visibleEnd; i++)
        {

            currY = ampToPixel(thumbHeight, minMax, waveformBuffer[i]);
            
			g.setColour(colour.withAlpha(.3f));
			//if(i%jmax(2, int(10*(1-zoom)))==1)
			g.drawVerticalLine(prevX, getHeight()-(getHeight()-prevY)+2, getHeight()-25);
			
			//g.setColour(this->colour);
			//g.drawLine(prevX, prevY, prevX+numPixelsPerIndex, prevY, 4);
			
            prevX = jmax(0.0, prevX + numPixelsPerIndex);
			prevY = currY;
        }
    }
}

//==============================================================================
float GenTable::ampToPixel(int height, Range<float> minMax, float sampleVal)
{
    float amp =  (sampleVal-minMax.getStart()) / minMax.getLength();
    return jmax(0.f, ((1-amp) * height));
}

float GenTable::pixelToAmp(int height, Range<float> minMax, float pixelY)
{
    float amp =  ((1-(pixelY/height))*minMax.getLength())+minMax.getStart();
    return amp;
}
//==============================================================================
void GenTable::mouseDown (const MouseEvent& e)
{
    Logger::writeToLog("mouseDown GenTable");
    //will need to add handles here..and only select region if gen01
    if(!e.mods.isPopupMenu())
    {
        regionWidth = (1.01-zoom)*1.5;
        currentPlayPosition = jmax (0.0, xToTime ((float) e.x));
        loopStart = e.x;
        loopLength =  0;
        repaint();
        sendChangeMessage();
    }
}
//==============================================================================
void GenTable::mouseEnter(const MouseEvent& e)
{

}
//==============================================================================
void GenTable::mouseExit(const MouseEvent& e)
{

}
//==============================================================================
void GenTable::mouseDrag(const MouseEvent& e)
{
    if(genRoutine==1)
    {
        if(this->getLocalBounds().contains(e.getPosition()))
        {
            if(e.mods.isLeftButtonDown())
            {
                double zoomFactor = visibleRange.getLength()/thumbnail->getTotalLength();
                regionWidth = abs(e.getDistanceFromDragStartX())*zoomFactor;
                Logger::writeToLog(String(e.getDistanceFromDragStartX()));
                if(e.getDistanceFromDragStartX()<0)
                    currentPlayPosition = jmax (0.0, xToTime (loopStart+(float)e.getDistanceFromDragStartX()));
                float widthInTime = ((float)e.getDistanceFromDragStartX() / (float)getWidth()) * (float)thumbnail->getTotalLength();
                loopLength = jmax (0.0, widthInTime*zoomFactor);
            }
            repaint();
        }
    }
}
//==============================================================================
void GenTable::setScrubberPos(double pos)
{
    //might need to be taken care of on a higher level
    currentPositionMarker->setVisible (true);
    pos = (pos/(thumbnail->getTotalLength()*sampleRate))*thumbnail->getTotalLength();
    currentPositionMarker->setRectangle (Rectangle<float> (timeToX (pos) - 0.75f, 10,
                                         1.5f, (float) (getHeight() - scrollbar->getHeight()-10)));
    if(pos<0.5)
        setRange (visibleRange.movedToStartAt(0));

    if(visibleRange.getEnd()<=thumbnail->getTotalLength())
        setRange (visibleRange.movedToStartAt (jmax(0.0, pos - (visibleRange.getLength() / 2.0))));

}
//==============================================================================
void GenTable::mouseUp(const MouseEvent& e)
{
    sendChangeMessage();
}


//===============================================================================
// Component that holds an array of handle points and sits on top of a table
// whilst in edit mode
//==================================================================================
HandleViewer::HandleViewer():Component()
{
    addAndMakeVisible(label = new Label());
    label->setVisible(false);
};

HandleViewer::~HandleViewer()
{
};

//==============================================================================
void HandleViewer::addHandle(double x, double y)
{
    int indx;		
	GenTable* table = findParentComponentOfClass <GenTable>();
	
	if(table)
	{
		HandleComponent* handle = new HandleComponent(x, y/getHeight(), handles.size(), false);
		handle->setTopLeftPosition((double)getWidth()*x, y);
		handle->setSize((double)getWidth()/40.0, (double)getWidth()/40.0);
		handle->addChangeListener(table);
		handle->addActionListener(this);
		//Logger::writeToLog(String(indx));
		handles.add(handle);
		addAndMakeVisible(handles[handles.size()-1]);
	}
}

void HandleViewer::insertHandle(double x, double y)
{
    int indx;	
	
	GenTable* table = findParentComponentOfClass <GenTable>();
	if(table)
	{
		for (int i=1; i<handles.size(); i++)
		{
			if (x*getWidth() >= handles[i-1]->getX() && x*getWidth() < handles[i]->getX())
			{
				indx = i;
				Logger::writeToLog("Handle number: "+String(indx+1));
			}
		}
		
    HandleComponent* handle = new HandleComponent(x, y/getHeight(), handles.size(), false);
    handle->setTopLeftPosition(getWidth()*x, y);
	//handle->setSize(getWidth()/40, getWidth()/40);
    handle->addChangeListener(table);
    handle->addActionListener(this);
	addAndMakeVisible(handle);
	handles.insert(indx, handle);
	handle->sendChangeMessage();
	}

}

//==============================================================================
void HandleViewer::showHandles(bool show)
{
	for (int i=1; i<handles.size(); i++)
	{
		handles[i]->setVisible(show);
	}
}
//==============================================================================
void HandleViewer::mouseDown(const MouseEvent& e)
{	
    insertHandle(double(e.x)/(double)getWidth(), (float)e.y);
	Logger::writeToLog("CHV Width:"+String(getWidth()));
}
//==============================================================================
void HandleViewer::resized()
{
    for(int i=0; i<handles.size(); i++)
    {
        handles[i]->setTopLeftPosition(((double)getWidth()*handles[i]->xPosRelative), ((double)getHeight()*handles[i]->yPosRelative));
    }
}
//==============================================================================
void HandleViewer::repaint(Graphics &g)
{
    g.fillAll(Colours::transparentBlack);
}
//==============================================================================
void HandleViewer::fixEdgePoints()
{
    if(handles.size()>1)
    {
        handles[0]->getProperties().set("fixedPos", true);
        handles[handles.size()-1]->getProperties().set("fixedPos", true);
    }

}
//==============================================================================
int HandleViewer::getHandleIndex(HandleComponent* thisHandle)
{
    return handles.indexOf(thisHandle);
}

//==============================================================================
void HandleViewer::actionListenerCallback(const String &message)
{
    if(message=="mouseUp")
	{
		label->setVisible(false);
		return;
	}
	
	label->setVisible(true);
    int offsetY=0, offsetX=10;
    StringArray mess;
    mess.addTokens(message, " ");
	
    if(mess[2].getIntValue()>getHeight()/2)
        offsetY=-18;
    if(mess[1].getIntValue()>getWidth()/2)
        offsetX=-60;
		
    label->setBounds(mess[1].getIntValue()+offsetX, mess[2].getIntValue()+offsetY, 60, 20);
	float amp = GenTable::pixelToAmp(getHeight(), minMax, mess[2].getIntValue());	
	int currXPos = round((mess[1].getFloatValue()/(float)getWidth())*this->tableSize);
	
	if(abs(gen)==5)
		amp = jmax(0.001f, amp);
	else
		amp = jmax(0.f, amp);
		
	label->setColour(Label::textColourId, colour);
	label->setColour(Label::backgroundColourId, colour.contrasting());
	String text = String(currXPos)+", "+String(amp);
    label->setText(text, dontSendNotification);
}

//==============================================================================
HandleComponent* HandleViewer::getPreviousHandle(HandleComponent* thisHandle)
{
    int thisHandleIndex = handles.indexOf(thisHandle);

    if(thisHandleIndex <= 0)
        return 0;
    else
        return handles.getUnchecked(thisHandleIndex-1);
}
//==============================================================================
HandleComponent* HandleViewer::getNextHandle(HandleComponent* thisHandle)
{
    int thisHandleIndex = handles.indexOf(thisHandle);

    if(thisHandleIndex == -1 || thisHandleIndex >= handles.size()-1)
        return 0;
    else
        return handles.getUnchecked(thisHandleIndex+1);
}
//==============================================================================
void HandleViewer::removeHandle (HandleComponent* thisHandle)
{
    if (handles.size() > 0)
    {
        handles.removeObject(thisHandle, true);
    }
	
	if(handles.size()>0)
		handles[0]->sendChangeMessage();
}

//==================================================================================
HandleComponent::HandleComponent(double xPos, double yPos, int _index, bool fixed):
    index(_index), x(0), y(0), colour(colour), fixed(fixed)
{
    xPosRelative = xPos;
	yPosRelative = yPos;	
	//Logger::writeToLog("Pos for handle:"+String(index)+"="+String(xPos));
    this->setInterceptsMouseClicks(true, false);
    setSize(10, 10);
}
//==================================================================================
HandleComponent::~HandleComponent()
{
}
//==================================================================================
void HandleComponent::paint (Graphics& g)
{
    g.setColour (Colours::white.withAlpha(.5f));
    g.fillEllipse(0, 0, getWidth(), getHeight());
}
//==================================================================================
HandleViewer* HandleComponent::getParentComponent()
{
    return (HandleViewer*)Component::getParentComponent();
}
//==================================================================================
void HandleComponent::removeThisHandle()
{
    getParentComponent()->removeHandle (this);
}
//==================================================================================
void HandleComponent::mouseEnter (const MouseEvent& e)
{
    setMouseCursor (MouseCursor::DraggingHandCursor);
    Logger::writeToLog("Current index:"+String(index));
    String message;
    message = String(String(index)+" "+String(getX())+" "+String(getY()));
	sendActionMessage(message);
}
//==================================================================================
void HandleComponent::mouseUp (const MouseEvent& e)
{
    mouseStatus = "mouseUp";
	sendActionMessage("mouseUp");
    sendChangeMessage();
}
//==================================================================================
void HandleComponent::mouseExit (const MouseEvent& e)
{
    mouseStatus = "mouseUp";
	sendActionMessage("mouseUp");
}
//==================================================================================
void HandleComponent::mouseDown (const MouseEvent& e)
{

    x = getX();
    y = getY();

    setMouseCursor (MouseCursor::DraggingHandCursor);
    dragger.startDraggingComponent (this, e);

    if ((e.mods.isShiftDown() == true) && (e.mods.isRightButtonDown() == true))
        removeThisHandle();

    PopupMenu pop, subm;
    pop.setLookAndFeel(&this->getTopLevelComponent()->getLookAndFeel());
    subm.setLookAndFeel(&this->getTopLevelComponent()->getLookAndFeel());
	bool fixed = this->getProperties().getWithDefault("fixedPos", false);
	
    if(e.mods.isRightButtonDown() == true)
    {
        //pop.addItem(1, "Linear");
        //subm.addItem(2, "Convex");
        //subm.addItem(3, "Concave");
        //pop.addSubMenu("Expon", subm);
        pop.addItem(4, "Delete");

        const int result = pop.show();
        if(result==4)
        {
			if(!fixed)
			removeThisHandle();
        }
    }
}
//==================================================================================
HandleComponent* HandleComponent::getPreviousHandle()
{
    return getParentComponent()->getPreviousHandle(this);
}
//==================================================================================
HandleComponent* HandleComponent::getNextHandle()
{
    return getParentComponent()->getNextHandle(this);
}
//==================================================================================
void HandleComponent::mouseDrag (const MouseEvent& e)
{
    HandleComponent* previousHandle = getPreviousHandle();
    HandleComponent* nextHandle = getNextHandle();
    


    int leftLimit = previousHandle == 0 ? 0 : previousHandle->getX()+1;
    int rightLimit = nextHandle == 0 ? getParentWidth()-previousHandle->getHeight() : nextHandle->getX()-1;
    int topLimit = previousHandle == 0 ? 0 : previousHandle->getX()+1;

    double dragX = x+e.getDistanceFromDragStartX();
    double dragY = y+e.getDistanceFromDragStartY();

    //dragger.dragComponent(this, e, &resizeLimits);
    if(dragX < leftLimit)
        dragX = leftLimit;
    if(dragX > rightLimit)
        dragX = rightLimit;
    if(dragY< 0)
        dragY = 0;
    if(dragY > getParentComponent()->getHeight())
        dragY = getParentComponent()->getHeight();
    if(fixed)
        dragX = x;

    this->setTopLeftPosition(dragX, dragY);
    xPosRelative = jlimit(0.0, 1.0, dragX/(double)this->getParentComponent()->getWidth());
	yPosRelative = jlimit(0.0, 1.0, dragY/(double)this->getParentComponent()->getHeight());
    //Logger::writeToLog("xPosRelative:"+String(xPosRelative));
    String message;
    message = String(String(index)+" "+String(dragX)+" "+String(dragY));
    //Logger::writeToLog(message);
    mouseStatus = "mouseDrag";
    sendActionMessage(message);
    sendChangeMessage();
}