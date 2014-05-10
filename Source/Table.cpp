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
	addAndMakeVisible(zoomIn = new ZoomButton("zoomIn"));
	addAndMakeVisible(zoomOut = new ZoomButton("zoomOut"));
	zoomIn->addChangeListener(this);
	zoomOut->addChangeListener(this);
	handleViewer = new HandleViewer();
	handleViewer->addChangeListener(this);
	addAndMakeVisible(handleViewer);
	zoomIn->toFront(false);
	zoomOut->toFront(false);
}
//==============================================================================	
GenTable::~GenTable()
{
	scrollbar->removeListener (this);
	if(thumbnail)
	thumbnail->removeChangeListener (this);
}
//==============================================================================	
void GenTable::addTable(int sr, const String col, int gen)
{
	sampleRate = sr;
	colour = Colours::findColourForName(col, Colours::white);
	genRoutine = gen;
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
	ZoomButton* button = dynamic_cast<ZoomButton*>(source);
	
	if(button)
	{
		if(button->getName()=="zoomIn")
		{
			zoom+=0.1;
			setZoomFactor(jmin(0.9, zoom));
		}
		else
		{
			zoom-=0.1;
			if(zoom<=0)
				setZoomFactor(0.0);
			else
				setZoomFactor(zoom);
		}
		repaint();
	}
	
	HandleViewer* viewer = dynamic_cast<HandleViewer*>(source);
	
	if(viewer)
	{
		changeMessage = "updateFunctionTable";
		sendChangeMessage();
	}
	

}
//==============================================================================
void GenTable::resized()
{
	zoomIn->setBounds(getWidth()-43, getHeight()-40, 20, 20);
	zoomOut->setBounds(getWidth()-20, getHeight()-40, 20, 20);
	handleViewer->setSize(getWidth(), getHeight()-25);
	if(scrollbar)
		scrollbar->setBounds (getLocalBounds().removeFromBottom (20).reduced (2));
}
//==============================================================================	    
void GenTable::scrollBarMoved (ScrollBar* scrollBarThatHasMoved, double newRangeStart)
{
	if (scrollBarThatHasMoved == scrollbar)
		setRange (visibleRange.movedToStartAt (newRangeStart), true);
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
Array<float> GenTable::getPfields()
{ 
	Array<float> values;
	float prevXPos=0, currXPos=0;
	
	for(int i=0; i<handleViewer->handles.size(); i++)
	{
		currXPos = round(handleViewer->handles[i]->xPosRelative*waveformBuffer.size());
		values.add(jmax(0.f, currXPos-prevXPos));
		values.add(pixelToAmp(handleViewer->getHeight(), minMax, handleViewer->handles[i]->getPosition().getY()+5));
		prevXPos = round(handleViewer->handles[i]->xPosRelative*waveformBuffer.size());
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
		setZoomFactor(zoom);
		Logger::writeToLog("updating waveform:Length "+String(thumbnail->getTotalLength()));
		repaint();
	}
}

void GenTable::setWaveform(Array<float, CriticalSection> buffer, int ftNumber, bool updateRange)
{ 
	if(genRoutine != 1)
	{
		waveformBuffer = buffer;
		tableNumber = ftNumber;
		tableSize = buffer.size();
		
		if(buffer.size() > 22050)
			CabbageUtils::showMessage("Tables of sizes over 22050 samples should be created using GEN01", &this->getLookAndFeel());
		
		if(updateRange == true)
		{
			const Range<double> newRange (0.0, buffer.size()/sampleRate);
			scrollbar->setRangeLimits (newRange);
			setRange (newRange);		
			setZoomFactor (zoom);	
			
		}
		
		minMax = findMinMax(buffer);
		repaint();
	}
	
}
//==============================================================================
void GenTable::enableEditMode(StringArray pFields)
{
		//if dealing with normalised table check pfield amps..
		Array<float, CriticalSection> pFieldAmps;
		pFieldAmps.add (pFields[5].getFloatValue());
		
		for(int i=6;i<pFields.size();i+=2)
			pFieldAmps.add(pFields[i+1].getFloatValue());

		Range<float> pFieldMinMax = findMinMax(pFieldAmps);
		normalised = pFields[4].getIntValue();
		float xPos = 0;		
		if(pFields.size()>0)
		{
			if(genRoutine==7 || genRoutine==5)
			{
				float pFieldAmpValue = (normalised<0 ? pFields[5].getFloatValue() : pFields[5].getFloatValue()/pFieldMinMax.getEnd());
				handleViewer->addHandle(0, ampToPixel(handleViewer->getHeight(), minMax, pFieldAmpValue));
				
				Logger::writeToLog("Coordinates:("+String(xPos)+", "+String(ampToPixel(handleViewer->getHeight(), minMax, pFieldAmpValue))+")");
				
				for(int i=6;i<pFields.size();i+=2)
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
			setRange (Range<double> (timeAtCentre - newScale * 0.5, timeAtCentre + newScale * 0.5));
		}		
	}
	else{
		const double newScale = jmax (0.00001, waveformBuffer.size()/sampleRate * (1.0 - jlimit (0.0, 0.9999, amount)));
		const double timeAtCentre = xToTime (getWidth() / 2.0f);
		setRange (Range<double> (timeAtCentre - newScale * 0.5, timeAtCentre + newScale * 0.5));
		
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
	scrollbar->setCurrentRange (visibleRange);
	//set visible ranges in samples...
	visibleStart = visibleRange.getStart()*sampleRate;
	Logger::writeToLog("VisibleStart:"+String(visibleStart));
	Logger::writeToLog("VisibleEnd:"+String(visibleEnd));
	Logger::writeToLog("VisibleLength:"+String(visibleLength));
	visibleEnd = visibleRange.getEnd()*sampleRate;
	visibleLength = visibleRange.getLength()*sampleRate;
	
	if(genRoutine!=1)
	{		
		if(!isScrolling)
		{		
			float newWidth = getWidth()*(float(waveformBuffer.size())/visibleLength);
			float leftOffset = newWidth*(visibleStart/(float)waveformBuffer.size());
			Logger::writeToLog("leftOffset:"+String(leftOffset));
			handleViewer->setSize(newWidth, handleViewer->getHeight());	
			handleViewer->setTopLeftPosition(-leftOffset, 0);
		}
		else
		{
			float leftOffset = handleViewer->getWidth()*(visibleStart/(float)waveformBuffer.size());
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
		float prevY=thumbArea.getHeight(), prevX=-1, currY=0, currX=0;
		
		//if gen01 then use an audio thumbnail class
        if(genRoutine==1)
        {			
            thumbnail->drawChannels(g, thumbArea.reduced (2), visibleRange.getStart(), visibleRange.getEnd(), .8f); 
			g.setColour(colour.contrasting(.5f).withAlpha(.7f));
			float zoomFactor = thumbnail->getTotalLength()/visibleRange.getLength();
			regionWidth = (regionWidth=2 ? 2 : regionWidth*zoomFactor);
			g.fillRect(timeToX(currentPlayPosition), 10.f, (regionWidth==2 ? 2 : regionWidth*zoomFactor), (float)getHeight()-26.f);	
		}

		//else draw the waveform directly
		else
		{
			float numPixelsPerIndex = (float)thumbArea.getWidth() / visibleLength;
			float waveformThickness = 4;
			float thumbHeight = thumbArea.reduced(2).getHeight();
			for(int i=visibleStart+1;i<visibleEnd;i++)
			{
				currY = ampToPixel(thumbHeight, minMax, waveformBuffer[i]);
				g.drawLine(prevX, prevY, prevX+numPixelsPerIndex, prevY, 4);				
				prevX = jmax(0.f, prevX + numPixelsPerIndex);
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

};

HandleViewer::~HandleViewer()
{
};

//==============================================================================
HandleComponent* HandleViewer::addHandle(float x, float  y)
{
	HandleComponent* handle = new HandleComponent(x, handles.size(), true);
	handle->setTopLeftPosition(getWidth()*x-5, y-5);
	handle->addChangeListener(this);
	handles.add(handle);
	addAndMakeVisible(handles[handles.size()-1]);
}
//==============================================================================	
void HandleViewer::mouseDown(const MouseEvent& e)
{
	Logger::writeToLog("mouse down - handle viewr");
}	
//==============================================================================
void HandleViewer::resized()
{
	for(int i=0;i<handles.size();i++)
	{
		handles[i]->setTopLeftPosition(getWidth()*handles[i]->xPosRelative-5, handles[i]->getPosition().getY());
		Logger::writeToLog(String(getWidth()*handles[i]->xPosRelative-5));
	}
		
		//setTopLeftPosition((getWidth()*handles[i]->xPosRelative)-5, y-5);
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
void HandleViewer::changeListenerCallback(ChangeBroadcaster *source)
{
	sendChangeMessage();	
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
	if (handles.size() > 0) {
		handles.removeObject(thisHandle, true);
	}
}
//==================================================================================
HandleComponent::HandleComponent(float xPos, int _index, bool fixed):
									index(_index), x(0), y(0), colour(colour), fixed(fixed)
{
	xPosRelative = xPos;
	this->setInterceptsMouseClicks(true, false);
	setSize(10, 10);
	Logger::writeToLog("Index:"+String(index));
}

HandleComponent::~HandleComponent()
{
}

void HandleComponent::paint (Graphics& g)
{
	g.setColour (Colours::white.withAlpha(.5f));
	g.fillEllipse(0, 0, getWidth(), getHeight());	
}

HandleViewer* HandleComponent::getParentComponent()
{
	return (HandleViewer*)Component::getParentComponent();
}

void HandleComponent::removeThisHandle()
{
	getParentComponent()->removeHandle (this);	
}

void HandleComponent::mouseEnter (const MouseEvent& e)
{
	setMouseCursor (MouseCursor::DraggingHandCursor);
	Logger::writeToLog("Current index:"+String(index));
}

void HandleComponent::mouseUp (const MouseEvent& e)
{
	//sendChangeMessage();	
}

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
	if(e.mods.isRightButtonDown() == true)
	{
		//pop.addItem(1, "Linear");
		//subm.addItem(2, "Convex");
		//subm.addItem(3, "Concave");
		//pop.addSubMenu("Expon", subm);
		pop.addItem(4, "Delete");
		
		const int result = pop.show();
		if(result==4){
			changeMessage = "removeHandle";
			sendChangeMessage();
		}
	}
}

HandleComponent* HandleComponent::getPreviousHandle()
{
	return getParentComponent()->getPreviousHandle(this);
}

HandleComponent* HandleComponent::getNextHandle()
{
	return getParentComponent()->getNextHandle(this);
}


void HandleComponent::mouseDrag (const MouseEvent& e)
{
	HandleComponent* previousHandle = getPreviousHandle();
	HandleComponent* nextHandle = getNextHandle();
	bool fixed = this->getProperties().getWithDefault("fixedPos", false);
		
		
	int leftLimit = previousHandle == 0 ? 0 : previousHandle->getX()+1;
	int rightLimit = nextHandle == 0 ? getParentWidth()-previousHandle->getHeight() : nextHandle->getX()-1;
	int topLimit = previousHandle == 0 ? 0 : previousHandle->getX()+1;

	float dragX = x+e.getDistanceFromDragStartX();
	float dragY = y+e.getDistanceFromDragStartY();
		
	//dragger.dragComponent(this, e, &resizeLimits);
	if(dragX < leftLimit) 
		dragX = leftLimit;
	if(dragX > rightLimit) 
		dragX = rightLimit;
	if(dragY< -5) 
		dragY = -5;
	if(dragY > getParentComponent()->getHeight()-5) 
		dragY = getParentComponent()->getHeight()-5;
	if(fixed) 
		dragX = x;
		
	this->setTopLeftPosition(dragX, dragY);
	xPosRelative = dragX/this->getParentComponent()->getWidth();
	Logger::writeToLog("xPosRelative:"+String(xPosRelative));

	sendChangeMessage();
}