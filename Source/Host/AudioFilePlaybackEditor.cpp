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

#include "AudioFilePlaybackProcessor.h"
#include "AudioFilePlaybackEditor.h"

#define BUTTON_SIZE 25
//==============================================================================
AudioFilePlaybackEditor::AudioFilePlaybackEditor (AudioFilePlaybackProcessor* ownerFilter): 
AudioProcessorEditor (ownerFilter), 
playButton("playButton", DrawableButton::ImageOnButtonBackground),
stopButton("stopButton", DrawableButton::ImageOnButtonBackground),
openButton("openButton", DrawableButton::ImageOnButtonBackground),
basicLook()
{
    AudioFormatManager formatManager;
    formatManager.registerBasicFormats();  
	waveformDisplay = new WaveformDisplay(formatManager, getFilter()->bufferingAudioFileSource, getFilter()->sourceSampleRate);
	setOpaque(false);
	playButton.addListener(this);
	addAndMakeVisible(&playButton);
	stopButton.addListener(this);
	addAndMakeVisible(&stopButton);
	openButton.addListener(this);
	addAndMakeVisible(&openButton);
	

	playButton.setLookAndFeel(&basicLook);
	stopButton.setLookAndFeel(&basicLook);
	openButton.setLookAndFeel(&basicLook);
	
	openButton.setColour(TextButton::buttonColourId, Colours::white);
	
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
	
	Path openPath;
	DrawablePath openImage;

	openPath.startNewSubPath(4, 2);
	openPath.lineTo(18, 2);
	openPath.lineTo(18, 5);
	openPath.lineTo(22, 5);
	openPath.lineTo(22, BUTTON_SIZE-5);
	openPath.lineTo(4, BUTTON_SIZE-5);
	openPath.lineTo(4, 2);
	openImage.setPath(openPath);
	
    openPath.startNewSubPath(8, 7);
    openPath.lineTo(24, 7);
    openPath.closeSubPath();

	openImage.setFill(Colours::white);
	openImage.setStrokeFill(Colours::green.darker(.9f));
	openImage.setStrokeThickness(4);	
	
	
	
	openButton.setImages(&openImage);		
	
	viewport = new Viewport();
	waveformDisplay->setBounds(10, 10, 500, 200);
	viewport->setViewedComponent(waveformDisplay, false);
	viewport->setScrollBarsShown(true, true);
	addAndMakeVisible(viewport);
    setSize (500, 250);

	if(File(getFilter()->getCurrentFile()).existsAsFile())
		waveformDisplay->setFile(File(getFilter()->getCurrentFile()));
	//waveformDisplay->setZoomFactor(1);

}

AudioFilePlaybackEditor::~AudioFilePlaybackEditor()
{
	waveformDisplay->stopTimer();
}

//==============================================================================
void AudioFilePlaybackEditor::resized()
{
	waveformDisplay->setBounds(waveformDisplay->getBounds().withWidth(getWidth()-20).withHeight(getHeight()-50));
	viewport->setBounds(10, 10, getWidth()-20, getHeight()-50);
	
	stopButton.setBounds(10, getHeight()-35, BUTTON_SIZE, BUTTON_SIZE);
	playButton.setBounds(BUTTON_SIZE+15, getHeight()-35, BUTTON_SIZE, BUTTON_SIZE);
	openButton.setBounds((BUTTON_SIZE*2)+20, getHeight()-35, BUTTON_SIZE, BUTTON_SIZE);
}
//==============================================================================
void AudioFilePlaybackEditor::paint (Graphics& g)
{
	g.fillAll(Colours::black);
}
//==============================================================================
void AudioFilePlaybackEditor::itemDropped (const DragAndDropTarget::SourceDetails& dragSourceDetails)
{
	if(FileTreeComponent* fileComp = dynamic_cast<FileTreeComponent*>(dragSourceDetails.sourceComponent.get()))			
	{
		getFilter()->setupAudioFile(fileComp->getSelectedFile());
		getFilter()->prepareToPlay(0, 512);
		if(getFilter()->bufferingAudioFileSource)
		{
			waveformDisplay->source = getFilter()->bufferingAudioFileSource;
			waveformDisplay->setFile(fileComp->getSelectedFile());	
		}	
	}
}
	
//==============================================================================
void AudioFilePlaybackEditor::buttonClicked(Button *button)
{
	
	if(button->getName()=="playButton")
	{
		if(getFilter()->bufferingAudioFileSource)
		{
			if(!getFilter()->isSourcePlaying)
				waveformDisplay->startTimer(10);
			else
				waveformDisplay->stopTimer();

			getFilter()->isSourcePlaying=!getFilter()->isSourcePlaying;			
		}
	}
	
	if(button->getName()=="stopButton")
	{
		if(getFilter()->bufferingAudioFileSource)
		{
			playButton.setToggleState(false, dontSendNotification);
			waveformDisplay->stopTimer();
			getFilter()->isSourcePlaying=false;
			waveformDisplay->resetPlaybackPosition();
			getFilter()->bufferingAudioFileSource->setNextReadPosition(0);	
		}	
	}
	
	else if(button->getName()=="openButton")
	{
	    FileChooser fc ("Open file");

	    if (fc.browseForFileToOpen())
		{
			getFilter()->setupAudioFile(fc.getResult());
			getFilter()->prepareToPlay(0, 512);
			if(getFilter()->bufferingAudioFileSource)
			{
				waveformDisplay->source = getFilter()->bufferingAudioFileSource;
				waveformDisplay->setFile(fc.getResult());	
			}
					
			//waveformDisplay->setZoomFactor(1);
		}	
	}
			

}
