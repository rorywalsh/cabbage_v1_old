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
TableManager::TableManager(): Component(), zoom(0.0), largestTable(0), scrubberPosition(0),
							  scrubberFreq(0), shouldShowTableButtons(true), shouldShowZoomButtons(true),
							  mainFooterHeight(25)
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
			{
				tables[i]->setZoomFactor(jmin(0.9, zoom));
			}
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
					if(button->getMode()==1)
					{
					tables[i]->setVisible(true);
					tables[i]->toFront(true);
					if(tables[i]->genRoutine != 2)
						tables[i]->getHandleViewer()->showHandles(true);
					}
					else if(button->getMode()==0)
					tables[i]->setVisible(false);

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
	table->tableNumber = ftnumber;
	table->addChangeListener(listener);
	table->addChangeListener(this);
	table->scrollbar->addListener(this);
	table->addChangeListener(listener);
	table->addTable(sr, col, gen, ampRange);
	addAndMakeVisible(table);
	tables.add(table);
	RoundButton* button = new RoundButton(String(ftnumber), Colours::findColourForName(col, Colours::white));
	button->addChangeListener(this);
	addAndMakeVisible(button);
	tableButtons.add(button);	
	resized();	

}
//==============================================================================
void TableManager::setAmpRanges(Array<float> ampRange)
{
	if(ampRange.size()>2)
		if(ampRange[2]==0)
			for(int i=0;i<tables.size();i++)
			{
				tables[i]->setAmpRanges(ampRange);
			}
		else
		{
			//check for valid table
			if(getTableFromFtNumber(ampRange[2])!=nullptr)
				getTableFromFtNumber(ampRange[2])->setAmpRanges(ampRange);
		}	
}
//==============================================================================
void TableManager::setZoomFactor(double newZoom)
{
	for(int i=0;i<tables.size();i++)
	{	
	if(newZoom<0){
		showZoomButtons(false);
		tables[i]->showScrollbar(false);
		//tables[i]->setZoomFactor(abs(newZoom));
	}
	else
	tables[i]->setZoomFactor(newZoom);
	}
}
//==============================================================================
void TableManager::setRange(double start, double end)
{
	for(int i=0;i<tables.size();i++)
	{	
	tables[i]->setSampleRange(start, end);
	}
}

void TableManager::setDrawMode(String mode)
{
	for(int i=0;i<tables.size();i++)
	{	
	if(tables[i]->genRoutine==2)
		tables[i]->drawAsVUMeter = true;
	}
}

//==============================================================================
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

void TableManager::setScrubberPos(double pos, int ftnumber)
{
	scrubberPosition = pos/getTableFromFtNumber(ftnumber)->tableSize;
	for(int i=0;i<tables.size();i++)
	tables[i]->setScrubberPos(scrubberPosition);
}

void TableManager::timerCallback()
{
	scrubberPosition += scrubberFreq;//getLargestTable()->tableSize*scrubberFreq;
	for(int i=0;i<tables.size();i++)
	tables[i]->setScrubberPos(scrubberPosition);	
}
//==============================================================================
void TableManager::resized()
{
	//if tables are stacked on each, do the math here...
	
	for(int i =0; i<tables.size();i++){
		tables[i]->setBounds(0, 0, getWidth(), getHeight());
	}	
	
	if(tableConfigList.size()==1)
		shouldShowTableButtons=false;
		
	mainFooterHeight = (shouldShowZoomButtons == true ? 25 : 0);		
	
	for(int i=0;i<tableConfigList.size();i++)
	{
		if(tableConfigList[i].size()>0)
		for(int y=0;y<tableConfigList[i].size();y++)
		{	
			int tableNumber = tableConfigList[i].getArray()->getReference(y);
			int ySpacing, yPos, height;
			//if not the bottom table..
			if(getTableFromFtNumber(tableNumber)!=nullptr)
			{
				if(y!=tableConfigList[i].size()-1)
				{
					ySpacing = y*((getHeight()-mainFooterHeight)/tableConfigList[i].size());
					height = (getHeight()-mainFooterHeight)/tableConfigList[i].size();				
					getTableFromFtNumber(tableNumber)->showScrollbar(false);
					getTableFromFtNumber(tableNumber)->mainFooterHeight = 0;
					getTableFromFtNumber(tableNumber)->paintFooterHeight = 0;
					yPos = ySpacing+height;	
				}
				else{
					height = getHeight()-5-yPos;
					ySpacing = yPos;
				}
				
				int width = getWidth();
				
				getTableFromFtNumber(tableNumber)->setBounds(0, ySpacing, getWidth(), height);	
			}
		}
	}
	
	bringButtonsToFront();
}

void TableManager::showZoomButtons(bool show)
{
	shouldShowZoomButtons = show;	
	if(show==false)
	{
		this->zoomIn->setVisible(false);
		this->zoomOut->setVisible(false);
	}
	resized();
}

void TableManager::showTableButtons(bool show)
{
	shouldShowTableButtons = show;
}

void TableManager::configTableSizes(var intableConfigList)
{
	tableConfigList = intableConfigList;
	resized();
}
//==============================================================================
GenTable* TableManager::getTableFromFtNumber(int ftnumber)
{
	for(int i=0;i<tables.size();i++)
	{
		if(tables[i]->tableNumber==ftnumber)
			return tables[i];		
	}
	return nullptr;
}

//==============================================================================
GenTable* TableManager::getLargestTable()
{
	int size=0, index=0;
	for(int i=0;i<tables.size();i++)
	{
		if(tables[i]->tableSize>size)
		{
			size = tables[i]->tableSize;
			index = i;
			
		}
	}
	if(tables[index])
		return tables[index];
	return nullptr;
}
//==============================================================================
void TableManager::bringButtonsToFront()
{
	if(shouldShowZoomButtons == true)
	{
		zoomIn->setBounds(getWidth()-43, getHeight()-20, 20, 20);
		zoomIn->toFront(true);
		zoomOut->setBounds(getWidth()-20, getHeight()-20, 20, 20);	
		zoomOut->toFront(true);		
	}
	else{
		zoomIn->setVisible(false);// setEnabled(false);
		zoomOut->setVisible(false);//setEnabled(false);			
	}

	for(int i=0;i<tableButtons.size();i++)
	{
		if(shouldShowTableButtons == true)
		{
			tableButtons[i]->setBounds(getWidth()-65-(i*18), getHeight()-18, 15, 15);
			tableButtons[i]->toFront(true);		
		}		
		else
			tableButtons[i]->setVisible(false);
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
		
		if(shouldShowTableButtons == true)
		tables[i]->scrollbarReduction = (tables.size()*20)+50;
		else
			tables[i]->scrollbarReduction = 50;
			
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
    tableNumber(-1),
	showScroll(true),
	shouldScroll(true),
	mainFooterHeight(25),
	paintFooterHeight(25),
	quantiseSpace(0.01),
	qsteps(0),
	drawAsVUMeter(false)
{
    thumbnail=nullptr;
    addAndMakeVisible(scrollbar = new ScrollBar(false));
    scrollbar->setRangeLimits (visibleRange);
    //scrollbar->setAutoHide (false);
    scrollbar->addListener(this);
    currentPositionMarker->setFill (Colours::lime.withAlpha(.1f));
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
void GenTable::addTable(int sr, const String col, int igen, Array<float> ampRange)
{
    sampleRate = sr;
    colour = Colours::findColourForName(col, Colours::white);
    genRoutine = abs(igen);
	handleViewer->gen = abs(igen);
	realGenRoutine = igen;
	handleViewer->colour = Colours::findColourForName(col, Colours::white);
	
    setAmpRanges(ampRange);		
		
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
void GenTable::setAmpRanges(Array<float> ampRange)
{
	if(ampRange.size()>2)
	{
		Logger::writeToLog(String(ampRange.size()));
		
		if(ampRange[2]==tableNumber || ampRange[2]==0)
		{
			minMax.setStart(ampRange[0]);
			minMax.setEnd(ampRange[1]);
			handleViewer->minMax = minMax;
		}
		
		if(ampRange.size()>3){
			quantiseSpace = ampRange[3];
			qsteps = quantiseSpace/minMax.getEnd();
			if(qsteps==1)
				handleViewer->showHandles(false);
		}
	}
	repaint();
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
    handleViewer->setSize(getWidth(), getHeight()-mainFooterHeight);	
    if(scrollbar)
		if(showScroll)
        scrollbar->setBounds (getLocalBounds().withWidth(getWidth()-scrollbarReduction).removeFromBottom (mainFooterHeight-5).reduced(2));
		else
		scrollbar->setBounds (-1000, 0, 100, 10);	
}

void GenTable::showScrollbar(bool show)
{
	showScroll = show;
	paintFooterHeight = (show==true ? 25 : 0);
	resized();
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
		if(genRoutine==7 || genRoutine==5)
		{
			currXPos = handleViewer->handles[i]->xPosRelative*waveformBuffer.size();
			//add x position
			values.add(jmax(0.0, currXPos-prevXPos));
			//hack to prevent csound from bawking with a 0 in gen05
			float amp = pixelToAmp(handleViewer->getHeight(), minMax, handleViewer->handles[i]->getPosition().getY());
			if(genRoutine==5)
				amp = jmax(0.001f, amp);
			else
				amp = jmax(0.f, amp);        
			//add y position
			values.add(amp);
			prevXPos = roundToIntAccurate(handleViewer->handles[i]->xPosRelative*waveformBuffer.size());
		}
		else if(genRoutine==2)
		{
			float amp = pixelToAmp(handleViewer->getHeight(), minMax, handleViewer->handles[i]->getPosition().getY());
			values.add(amp);			
			//Logger::writeToLog("amp for index:"+String(i)+" == "+String(amp));
		}
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
        thumbnail->reset(buffer.getNumChannels(), 44100, buffer.getNumSamples());
        thumbnail->addBlock(0, buffer, 0, buffer.getNumSamples());
        const Range<double> newRange (0.0, thumbnail->getTotalLength());
        scrollbar->setRangeLimits (newRange);
        setRange (newRange);
        //setZoomFactor(zoom);
        //Logger::writeToLog("updating waveform:Length "+String(thumbnail->getTotalLength()));
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
void GenTable::enableEditMode(StringArray m_pFields)
{
    //turns on edit mode by adding handles to the handleViewer
	
	//only assign original pfields
	if(m_pFields.size()>1)
		pFields = m_pFields;
		
    Array<float, CriticalSection> pFieldAmps;
    pFieldAmps.add (pFields[5].getFloatValue());

    for(int i=6; i<pFields.size(); i+=2)
        pFieldAmps.add(pFields[i+1].getFloatValue());

    Range<float> pFieldMinMax = findMinMax(pFieldAmps);
    normalised = pFields[4].getIntValue();
    double xPos = 0;
    handleViewer->handles.clear();
    if(pFields.size()>0)
    {
        if(genRoutine==7 || genRoutine==5)
        {
            float pFieldAmpValue = (normalised<0 ? pFields[5].getFloatValue() : pFields[5].getFloatValue()/pFieldMinMax.getEnd());
            handleViewer->addHandle(0, ampToPixel(handleViewer->getHeight(), minMax, pFieldAmpValue), 12, 12, this->colour);

            for(int i=6; i<pFields.size(); i+=2)
            {
                xPos = xPos + pFields[i].getFloatValue();
                pFieldAmpValue = (normalised<0 ? pFields[i+1].getFloatValue() : pFields[i+1].getFloatValue()/pFieldMinMax.getEnd());
                handleViewer->addHandle(xPos/(double)waveformBuffer.size(), ampToPixel(handleViewer->getHeight(), minMax, pFieldAmpValue), 12, 12, this->colour);
			}
			handleViewer->fixEdgePoints(genRoutine);
        }
	    else if(genRoutine==2)
        {
			double width = (getWidth()/tableSize);
            float pFieldAmpValue = (normalised<0 ? pFields[5].getFloatValue() : pFields[5].getFloatValue()/pFieldMinMax.getEnd());
            handleViewer->addHandle(0, ampToPixel(handleViewer->getHeight(), minMax, pFieldAmpValue), width+1, 5, this->colour);
			for(double i=6; i<pFields.size(); i++)
            {
                xPos = (i-5.0)/(double(tableSize))*tableSize;
                pFieldAmpValue = (normalised<0 ? pFields[i].getFloatValue() : pFields[i].getFloatValue()/pFieldMinMax.getEnd());
                handleViewer->addHandle(xPos/tableSize, ampToPixel(handleViewer->getHeight(), minMax, pFieldAmpValue), width+1, 5, this->colour);
			}
			handleViewer->fixEdgePoints(genRoutine);
			handleViewer->showHandles(false);
        }

    }

}

//==============================================================================
void GenTable::setZoomFactor (double amount)
{
	//set zoom factor, between 0 and 1
    zoom = amount;
    if(genRoutine==1)
    {
        if (thumbnail->getTotalLength() > 0)
        {
            const double newScale = jmax (0.001, thumbnail->getTotalLength() * (1.0 - jlimit (0.0, 0.99, amount)));
            const double timeAtCentre = xToTime (getWidth() / 2.0f);
			if(amount!=0)
			{
				setRange (Range<double> (timeAtCentre - newScale * 0.5, timeAtCentre + newScale * 0.5));	
				setRange (Range<double> (timeAtCentre - newScale * 0.5, timeAtCentre + newScale * 0.5));
				
			}
			else
				setRange (Range<double> (0, thumbnail->getTotalLength()));
        }
    }
    else
    {
		if(visibleRange.getLength()==0)
			visibleRange.setLength(waveformBuffer.size()/sampleRate);		
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
void GenTable::setSampleRange(double start, double end)
{
    if(genRoutine!=1)
    {
	setRange(Range<double>(start/sampleRate, end/sampleRate));
	}
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
	if(newRange.getLength()>0)
	{
		//set visible ranges in samples...
		scrollbar->setCurrentRange (visibleRange, dontSendNotification);
		if(genRoutine!=1)
		{
			
			visibleStart = visibleRange.getStart()*sampleRate;
			//Logger::writeToLog("VisibleStart:"+String(visibleRange.getStart()));
			visibleEnd = visibleRange.getEnd()*sampleRate;
			//Logger::writeToLog("visibleEnd:"+String(visibleRange.getEnd()));
			visibleLength = visibleRange.getLength()*sampleRate;
			Logger::writeToLog("Table Number:"+String(tableNumber)+String(" VisibleLength:")+String(visibleRange.getLength()));
			if(!isScrolling)
			{
				double newWidth = double(getWidth())*(double(waveformBuffer.size())/visibleLength);
				double leftOffset = newWidth*(visibleStart/(double)waveformBuffer.size());
				handleViewer->setSize(newWidth, handleViewer->getHeight());	
				handleViewer->setTopLeftPosition(-leftOffset, 0);
			}
			else
			{
				double leftOffset = handleViewer->getWidth()*(visibleStart/(double)waveformBuffer.size());
				handleViewer->setTopLeftPosition(-leftOffset, 0);
			}

		}
	}
	repaint();
}
//==============================================================================
void GenTable::paint (Graphics& g)
{
    g.fillAll (Colours::transparentBlack);
	//set thumbArea, this is the area of the painted image
    thumbArea = getLocalBounds();
    thumbArea.setHeight(getHeight()-paintFooterHeight);
    float prevY=0, prevX=0, currY=0;

    //if gen01 then use an audio thumbnail class
    if(genRoutine==1)
    {
        g.setColour (colour);
		thumbnail->drawChannels(g, thumbArea.reduced (2), visibleRange.getStart(), visibleRange.getEnd(), .8f);
        g.setColour(colour.contrasting(.5f).withAlpha(.7f));
        float zoomFactor = thumbnail->getTotalLength()/visibleRange.getLength();
        regionWidth = (regionWidth=2 ? 2 : regionWidth*zoomFactor);
   }

    //else draw the waveform directly onto this component
	//edit handles get placed on the handleViewer, which is placed on top of this component
    else
    {
		Path path;
		path.startNewSubPath(0, thumbArea.getHeight()+5.f);
        numPixelsPerIndex = (double)thumbArea.getWidth() / visibleLength;
        double waveformThickness = 4;
        double thumbHeight = thumbArea.getHeight();
		prevY = ampToPixel(thumbHeight, minMax, waveformBuffer[0]);
		prevY = prevY+5.f;
        for(float i=visibleStart; i<visibleEnd; i++)
        {
			//minMax is the range of the current waveforms amplitude
            currY = ampToPixel(thumbHeight, minMax, waveformBuffer[i]);
			currY = currY+5.f;

			g.setColour(colour.withAlpha(.2f));
			
			if(genRoutine==2)
			{
				//when qsteps == 1 we draw a grid
				if(qsteps==1)
				{
					currY = currY-5.f;
					g.setColour(colour.withAlpha(.3f));	
					g.drawRoundedRectangle(prevX+2, 2.f, numPixelsPerIndex-4, thumbHeight-4, numPixelsPerIndex*0.1, 1.f);
					g.setColour(colour.withAlpha(.6f));
					if(thumbHeight-(thumbHeight-currY)==0)
					g.fillRoundedRectangle(prevX+3, thumbHeight-(thumbHeight-currY)+3.f, numPixelsPerIndex-6, thumbHeight-currY-6.f, numPixelsPerIndex*0.1);	
				}//else we draw a simple bar graph representation....
				else{
					path.lineTo(prevX, currY);
					path.lineTo(prevX+numPixelsPerIndex, currY);	
				}
			}
			else{
				path.lineTo(prevX+numPixelsPerIndex, prevY);			
			}
            prevX = jmax(0.0, prevX + numPixelsPerIndex);
			prevY = currY;
        }
		
	path.lineTo(prevX, thumbArea.getHeight());
	path.closeSubPath();
	
	if(drawAsVUMeter)
	{
		ColourGradient grad(Colours::yellow, 0.f, 0.f, Colours::red, thumbArea.toFloat().getWidth(), thumbArea.toFloat().getHeight(), false);
		grad.addColour(.5, Colours::lime);
		g.setGradientFill(grad);		
	}

	g.fillPath(path);
	g.setColour(colour.darker());
	if(qsteps!=1)
	g.strokePath(path, PathStrokeType(1));
    }
}

//==============================================================================
float GenTable::ampToPixel(int height, Range<float> minMax, float sampleVal)
{
	//caluclate amp value based on pixel...
	float amp =  (sampleVal-minMax.getStart()) / minMax.getLength();
    return jmax(0.f, ((1-amp) * height));
}

float GenTable::pixelToAmp(int height, Range<float> minMax, float pixelY)
{
	//caluclate pixel value based on amp...
    float amp =  ((1-(pixelY/height))*minMax.getLength())+minMax.getStart();
    return amp;
}
//==============================================================================
void GenTable::mouseDown (const MouseEvent& e)
{
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
		//draw rectangle when users select audio in a gen01 table...
        if(this->getLocalBounds().contains(e.getPosition()))
        {
            if(e.mods.isLeftButtonDown())
            {
                double zoomFactor = visibleRange.getLength()/thumbnail->getTotalLength();
                regionWidth = abs(e.getDistanceFromDragStartX())*zoomFactor;
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
void GenTable::setXPosition(double pos)
{
    if(genRoutine==1)
    {
			setRange (visibleRange.movedToStartAt(pos));		
	}
	else
	{
			setRange (visibleRange.movedToStartAt(pos));		
	}
}
//==============================================================================
void GenTable::setScrubberPos(double pos)
{
    //set the position of the scrubber. pos will be between 0 and 1
    if(genRoutine==1)
    {
		currentPositionMarker->setVisible (true);
	
		//assign time values in seconds to pos..		
		double timePos = pos*thumbnail->getTotalLength()*sampleRate;		
		timePos = (timePos/(thumbnail->getTotalLength()*sampleRate))*thumbnail->getTotalLength();
		//set position of scrubber rectangle
		currentPositionMarker->setRectangle (juce::Rectangle<float> (timeToX (timePos) - 0.75f, 0,
											 1.5f, (float) (getHeight() - 20)));

		if(this->showScroll)
		{
		//take care of scrolling...
		if(timePos<thumbnail->getTotalLength()/25.f)
			setRange (visibleRange.movedToStartAt(0));
		else
		if(visibleRange.getEnd()<=thumbnail->getTotalLength() && zoom>0.0)
				setRange (visibleRange.movedToStartAt (jmax(0.0, timePos - (visibleRange.getLength() / 2.0))));
		}
	}
    else
    {		
		currentPositionMarker->setVisible (true);
		double waveformLengthSeconds = (double)waveformBuffer.size()/sampleRate;		
		double timePos = pos*waveformLengthSeconds;		
		currentPositionMarker->setRectangle (juce::Rectangle<float> (timeToX (timePos), 0,
											 numPixelsPerIndex, thumbArea.getHeight()));
		
		if(this->showScroll)
		{									 
		if(timePos<(waveformLengthSeconds)/25.f)
			setRange (visibleRange.movedToStartAt(0));
		else
			if(visibleRange.getEnd()<=waveformLengthSeconds && zoom>0.0)
				setRange (visibleRange.movedToStartAt (jmax(0.0, timePos - (visibleRange.getLength()/2.0))));
		}		
	}
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
void HandleViewer::addHandle(double x, double y, double width, double height, Colour colour)
{
	//add a handle component to our handleViewer
    int indx;		
	GenTable* table = getParentTable();
	
	if(table)
	{
		//set up handle, and pass relative x and y values as well as gen and colour
		HandleComponent* handle = new HandleComponent(x, y/getHeight(), handles.size(), false, table->genRoutine, colour);
		
		//if gen02 use setTopLeft, else use setCentrePostion
		if(table->genRoutine!=2)
			handle->setCentrePosition((double)getWidth()*x, y+5.f);
		else
			handle->setTopLeftPosition((double)getWidth()*x, y-2.5);
		
		handle->setSize(width, height);
		handle->addChangeListener(table);
		handle->addActionListener(this);
		handle->status=true;
		handles.add(handle);
		addAndMakeVisible(handles[handles.size()-1]);
		
	}
}

void HandleViewer::insertHandle(double x, double y, Colour colour)
{
	//add a handle component to our handleViewer. This should be combined with
	//above function...
    int indx;	
	GenTable* table = findParentComponentOfClass <GenTable>();
	if(table)
	{
		for (int i=1; i<handles.size(); i++)
		{
			if (x*getWidth() >= handles[i-1]->getX() && x*getWidth() < handles[i]->getX())
			{
				indx = i;
				//Logger::writeToLog("Handle number: "+String(indx+1));
			}
		}
		
    HandleComponent* handle = new HandleComponent(x, y/getHeight(), handles.size(), false, table->genRoutine, colour);
    handle->setCentrePosition((double)getWidth()*x, y+5);
    handle->addChangeListener(table);
    handle->addActionListener(this);
	addAndMakeVisible(handle);
	handles.insert(indx, handle);
	handle->status=true;
	handle->sendChangeMessage();
	}

}

//==============================================================================
void HandleViewer::showHandles(bool show)
{
/*	shouldShowHandles = show;
	for (int i=1; i<handles.size(); i++)
	{
		handles[i]->setColour(Colours::transparentBlack);
	}*/
}
//==============================================================================
void HandleViewer::mouseDown(const MouseEvent& e)
{	
	if(this->gen!=10)
	positionHandle(e);
}

void HandleViewer::mouseDrag(const MouseEvent& e)
{	
	if(gen==2 && shouldShowHandles)
	positionHandle(e);
}

void HandleViewer::positionHandle(const MouseEvent& e)
{
	//positions the handle when a user sends a mouse down on the handleViewer
    if(gen==1)
		return;		
	
	//determine whether of not we are in toggle on.off(grid) mode..
	double steps = (minMax.getEnd()/getParentTable()->quantiseSpace);
	double ySnapPos;	
	
	if(gen!=2)
		insertHandle(double(e.x)/(double)getWidth(), (float)e.y, colour);
    else if(gen==2)
		for(int i=0;i<handles.size();i++)
		{
			double handleX = handles[i]->getPosition().getX();

			if(e.x>handleX && e.x<handleX+handles[i]->getWidth())
			{
				if(steps==1){	//if toggle mode is enabled..
				//handles[i]->setVisible(false);
				handles[i]->status=!handles[i]->status;
				handles[i]->setTopLeftPosition(handles[i]->getPosition().withY(getSnapPosition(getHeight()*int(handles[i]->status))));
				handles[i]->setRelativePositions(handles[i]->getPosition().toDouble().withY(getSnapPosition(getHeight()*double(handles[i]->status))));	
				handles[i]->sendChangeMessage();
				}
				else{
				handles[i]->setTopLeftPosition(handles[i]->getPosition().withY(getSnapPosition(double(e.y))));	
				handles[i]->setRelativePositions(handles[i]->getPosition().toDouble().withY(getSnapPosition(double(e.y))));
				handles[i]->sendChangeMessage();
				}		
			}		
		}
    String message;
    message = String(String(handles.size())+" "+String(e.x)+" "+String(getSnapPosition(e.y)));
	//showLabel(message);
}

double HandleViewer::getSnapPosition(const double y)
{
	//return snapped position if qunatise is one
	double ySnapPos = 0;		
	double steps = (minMax.getEnd()/getParentTable()->quantiseSpace);
	double jump = (getParentTable()->quantiseSpace/minMax.getEnd())*getHeight();
	for(double c=0;c<=steps;c++)
	{
		if(y > (c*jump)-jump/2.f && y < ((c+1)*jump+(jump/2.f))) 
			ySnapPos = c*jump;	
	}
	return ySnapPos;
}

//==============================================================================
void HandleViewer::resized()
{
    for(int i=0; i<handles.size(); i++)
    {
		if(this->gen!=2)
        handles[i]->setCentrePosition(((double)getWidth()*handles[i]->xPosRelative), ((double)getHeight()*handles[i]->yPosRelative));
        else{
		handles[i]->setTopLeftPosition(((double)getWidth()*handles[i]->xPosRelative), ((double)getHeight()*handles[i]->yPosRelative));
 		Logger::writeToLog(String(getWidth()/tableSize));
		handles[i]->setSize(getWidth()/tableSize, 5.f);	
		//handles[i]->setVisible(false);
		showHandles(false);
		}  
	}
}
//==============================================================================
void HandleViewer::repaint(Graphics &g)
{
    g.fillAll(Colours::transparentBlack);
}
//==============================================================================
void HandleViewer::fixEdgePoints(int gen)
{
	//fix outer handles so they can't be dragged from the edges
	if(gen==7 || gen==5)
	{ 
		if(handles.size()>1)
		{
			handles[0]->getProperties().set("fixedPos", true);
			handles[handles.size()-1]->getProperties().set("fixedPos", true);
		}	
	}
	//with a GEN02 all points are fixed...
	else if(abs(gen)==2)
	{
		for(int i=0;i<handles.size();i++)
		{
			handles[i]->getProperties().set("fixedPos", true);
		}		
	}

}
//==============================================================================
int HandleViewer::getHandleIndex(HandleComponent* thisHandle)
{
    return handles.indexOf(thisHandle);
}

//==============================================================================
void HandleViewer::showLabel(String message)
{
	
	StringArray mess;
    mess.addTokens(message, " ");
	int offsetY=0, offsetX=10;

	float amp = GenTable::pixelToAmp(getHeight(), minMax, mess[2].getIntValue());	
	int currXPos = ((mess[1].getFloatValue()/(float)getWidth())*this->tableSize);
	
	if(abs(gen)==5)
		amp = jmax(0.001f, amp);
	else
		amp = jmax(0.f, amp);
	
    if(mess[2].getIntValue()>getHeight()/2)
        offsetY=-18;
    if(mess[1].getIntValue()>getWidth()/2)
        offsetX=-60;
		
	label->setVisible(true);
	label->setBounds(mess[1].getIntValue()+offsetX, mess[2].getIntValue()+offsetY, 60, 20);
	label->setColour(Label::textColourId, colour);
	label->setColour(Label::backgroundColourId, colour.contrasting());
	String text = String(currXPos)+", "+String(amp);
    label->setText(text, dontSendNotification);	
}
//==============================================================================
void HandleViewer::actionListenerCallback(const String &message)
{
	//this is called to show the coordinates and position the labal accordingly
    if(message=="mouseUp")
	{
		label->setVisible(false);
		return;
	}
	
	showLabel(message);
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
HandleComponent::HandleComponent(double xPos, double yPos, int _index, bool fixed, int gen, Colour colour):
    index(_index), x(0), y(0), colour(colour), fixed(fixed), status(false)
{
    //our main handle object. xPos and xPos are always between 0 and 1
	//we convert them to pixel positions later, based on the size of the handleViewer
	xPosRelative = xPos;
	yPosRelative = yPos;
	genRoutine = gen;	
    this->setInterceptsMouseClicks(true, false);
    setSize(12, 12);
}
//==================================================================================
HandleComponent::~HandleComponent()
{
}
//==================================================================================
void HandleComponent::setColour(Colour icolour)
{
	colour = icolour;
}

void HandleComponent::paint (Graphics& g)
{
    g.setColour(Colours::transparentBlack);
	//if gen02 use long rectangles
	if(genRoutine==2)
	{
//		g.setColour(colour.withAlpha(.5f));
//		g.fillRect(getLocalBounds());
//		g.setColour(Colours::whitesmoke);
//		g.drawRect(getLocalBounds(), 1.f);
	}
	else//draw a circle..
	{
		g.setColour(Colours::whitesmoke);
		//g.drawLine(0, (getHeight()/2.f), getWidth(), (getHeight()/2.f), 1.f);
		//g.drawLine(getWidth()/2.f, 0, getWidth()/2.f, getHeight(), 1.f);
		//g.setColour(colour);
		g.drawEllipse(getLocalBounds().reduced(1.4).toFloat(), 1.f);
		g.setColour(colour.withAlpha(.5f));
		g.fillEllipse(getLocalBounds().reduced(2).toFloat());
	}
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
    String message;
    message = String(String(index)+" "+String(getX())+" "+String(getY()));
	//send an action message to the handleViewer to display the coordinates label
	sendActionMessage(message);
}
//==================================================================================
void HandleComponent::mouseUp (const MouseEvent& e)
{
    mouseStatus = "mouseUp";
	//send a message to kill the coordinates label...
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
static void popupMenuCallback(int result, HandleComponent* handleComp)
{

	bool fixed = handleComp->getProperties().getWithDefault("fixedPos", false);
	
	if(result==4)
	{
		if(!fixed)
		handleComp->removeThisHandle();
	}

}
//==================================================================================
void HandleComponent::mouseDown (const MouseEvent& e)
{
	//users can delete handles here, and will be able to set the curve type
	//when gen16 is added to the mix. 
    x = getX();
    y = getY();

    setMouseCursor (MouseCursor::DraggingHandCursor);
    dragger.startDraggingComponent (this, e);

    if ((e.mods.isShiftDown() == true) && (e.mods.isRightButtonDown() == true))
        removeThisHandle();

    PopupMenu pop, subm;
    pop.setLookAndFeel(&this->getTopLevelComponent()->getLookAndFeel());
    subm.setLookAndFeel(&this->getTopLevelComponent()->getLookAndFeel());
	
    if(e.mods.isRightButtonDown() == true)
    {
        pop.addItem(4, "Delete");
		pop.showMenuAsync(PopupMenu::Options(), ModalCallbackFunction::forComponent(popupMenuCallback, this));
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
void HandleComponent::setRelativePositions(Point<double> mouse)
{
    //convert position so that it's scaled between 0 and 1 
	xPosRelative = jlimit(0.0, 1.0, mouse.getX()/(double)this->getParentComponent()->getWidth());
	yPosRelative = jlimit(0.0, 1.0, mouse.getY()/(double)this->getParentComponent()->getHeight());
}
//==================================================================================
void HandleComponent::mouseDrag (const MouseEvent& e)
{
    //when a handle is dragged, we update its position and send a message
	//to Cabbage to update the Csound function table(CabbagePluginEditor.cpp)
	HandleComponent* previousHandle = getPreviousHandle();
    HandleComponent* nextHandle = getNextHandle();
    
	bool fixed = this->getProperties().getWithDefault("fixedPos", false);

    int leftLimit = previousHandle == 0 ? 0 : previousHandle->getX()+1;
    int rightLimit = nextHandle == 0 ? getParentWidth()-previousHandle->getHeight() : nextHandle->getX()-1;
    int topLimit = previousHandle == 0 ? 0 : previousHandle->getX()+1;

    double dragX = x+e.getDistanceFromDragStartX();
    double dragY = y+e.getDistanceFromDragStartY();

	//should use a boundsConstrainer here....
    if(dragX < leftLimit)
        dragX = leftLimit;
    if(dragX > rightLimit)
        dragX = rightLimit;
    if(fixed)
        dragX = x;

    dragY = jlimit(0.0, getParentComponent()->getHeight()+5.0, dragY);

	HandleViewer* viewer = getParentHandleViewer();


	if(genRoutine!=2)
		setCentrePosition(dragX, viewer->getSnapPosition(dragY));
	else
		setTopLeftPosition(dragX, viewer->getSnapPosition(dragY));
	
    setRelativePositions(Point<double>(dragX, viewer->getSnapPosition(dragY)));

	//create message to send to handleViewer to display the current coordinates
    String message;
    message = String(String(index)+" "+String(dragX)+" "+String(viewer->getSnapPosition(dragY)));
    mouseStatus = "mouseDrag";
    sendActionMessage(message);
    sendChangeMessage();
}
