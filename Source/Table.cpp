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
									gen(1)
{	
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
		
	
}
//==============================================================================	
GenTable::~GenTable()
{
	scrollbar->removeListener (this);
	thumbnail->removeChangeListener (this);
}
//==============================================================================	
void GenTable::addTable(int sr, Colour col, StringArray fstatement)
{
	sampleRate = sr;
	colour = col;
	//tableNumber = tableNumer
	//if(gen==1)
	//{
	formatManager.registerBasicFormats();  
	thumbnail = new AudioThumbnail(2, formatManager, thumbnailCache); 
	thumbnail->addChangeListener (this);
	//}		
}
//==============================================================================	
void GenTable::changeListenerCallback(ChangeBroadcaster *source)
{
	ZoomButton* button = dynamic_cast<ZoomButton*>(source);
	if(button){
		if(button->getName()=="zoomIn")
			setZoomFactor(jmin(1.0, zoom+=0.1));
		else
			setZoomFactor(jmax(0.0, zoom-=0.1));
	}
	repaint();
	Logger::writeToLog("GenTable Change listener:"+String(thumbnail->getTotalLength()));
}
//==============================================================================
void GenTable::resized()
{
	zoomIn->setBounds(getWidth()-43, getHeight()-40, 20, 20);
	zoomOut->setBounds(getWidth()-20, getHeight()-40, 20, 20);
	if(scrollbar)
		scrollbar->setBounds (getLocalBounds().removeFromBottom (20).reduced (2));
}
//==============================================================================	    
void GenTable::scrollBarMoved (ScrollBar* scrollBarThatHasMoved, double newRangeStart)
{
	if (scrollBarThatHasMoved == scrollbar)
		setRange (visibleRange.movedToStartAt (newRangeStart));
}
	
void GenTable::setFile (const File& file)
{
   if (! file.isDirectory())
     {
		gen==1;
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
			setWaveform(buffer, reader->numChannels);
		}		 
		delete reader; 
      }
	repaint(0, 0, getWidth(), getHeight());
}

//==============================================================================
void GenTable::setWaveform(AudioSampleBuffer buffer, int channels)
{            
	thumbnail->clear();
	repaint();
	thumbnail->reset(channels, 44100, buffer.getNumSamples());	
	thumbnail->addBlock(0, buffer, 0, buffer.getNumSamples());
	const Range<double> newRange (0.0, thumbnail->getTotalLength());
	scrollbar->setRangeLimits (newRange);
	setRange (newRange);
	setZoomFactor(zoom);
	repaint();
	Logger::writeToLog("updating waveform");
}

//==============================================================================
void GenTable::setZoomFactor (double amount)
{
	//instead of using thumbnail length, just use table lenght
	//will probably have to update code so that it uses samples intead of 
	//time...	
	
	if (thumbnail->getTotalLength() > 0)
	{
		const double newScale = jmax (0.001, thumbnail->getTotalLength() * (1.0 - jlimit (0.0, 0.99, amount)));
		const double timeAtCentre = xToTime (getWidth() / 2.0f);
		setRange (Range<double> (timeAtCentre - newScale * 0.5, timeAtCentre + newScale * 0.5));
	}
	zoom = amount;
	repaint();
}
//==============================================================================
void GenTable::setRange(Range<double> newRange)
{
	visibleRange = newRange;
	scrollbar->setCurrentRange (visibleRange);
	repaint();
}
//==============================================================================
void GenTable::paint (Graphics& g)
{
		g.fillAll (Colours::transparentBlack);
		g.setColour (colour);
	
        if(gen==1)
        {			
            Rectangle<int> thumbArea (getLocalBounds());
            thumbArea.setHeight(getHeight()-14);
			thumbArea.setTop(10.f);
            thumbnail->drawChannels(g, thumbArea.reduced (2),
                                    visibleRange.getStart(), visibleRange.getEnd(), .8f);
		//if(GEN05 then draw envelope data)	
      

		g.setColour(colour.contrasting(.5f).withAlpha(.7f));
		float zoomFactor = thumbnail->getTotalLength()/visibleRange.getLength();
		//regionWidth = (regionWidth=2 ? 2 : regionWidth*zoomFactor)
		g.fillRect(timeToX(currentPlayPosition), 10.f, (regionWidth==2 ? 2 : regionWidth*zoomFactor), (float)getHeight()-26.f);	
		}
}

//==============================================================================
void GenTable::mouseWheelMove (const MouseEvent&, const MouseWheelDetails& wheel)
{
	if (thumbnail->getTotalLength() > 0.5)
	{
		double newStart = visibleRange.getStart() - wheel.deltaX * (visibleRange.getLength()) / 10.0;
		newStart = jlimit (0.0, jmax (0.0, thumbnail->getTotalLength() - (visibleRange.getLength())), newStart);

		setRange (Range<double> (newStart, newStart + visibleRange.getLength()));

		repaint();
	}
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
	//Logger::writeToLog("mouseOver GenTable");
	//if(thumbnail->getTotalLength()>0.01){
	//	zoomIn->setVisible(true);
	//	zoomOut->setVisible(true);
	//}
}
//==============================================================================
void GenTable::mouseExit(const MouseEvent& e)
{
	//	zoomIn->setVisible(false);
	//	zoomOut->setVisible(false);
}
//==============================================================================
void GenTable::mouseDrag(const MouseEvent& e)
{
	//if(GEN01 then provide access to highlighted regions, otherwise it will be too messy)
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

//==============================================================================
//GenThumbnail class
//==============================================================================
GenThumbnail::GenThumbnail(int gen, Colour colour)
{
	
}
//==============================================================================
GenThumbnail::~GenThumbnail()
{
	
}
//==============================================================================
void GenThumbnail::paint(Graphics &g)
{
	
}