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
zoomInButton("zoomInButton", DrawableButton::ImageOnButtonBackground),
zoomOutButton("zoomOutButton", DrawableButton::ImageOnButtonBackground),
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
	zoomInButton.addListener(this);
	addAndMakeVisible(&zoomInButton);
	zoomOutButton.addListener(this);
	addAndMakeVisible(&zoomOutButton);	

	playButton.setLookAndFeel(&basicLook);
	stopButton.setLookAndFeel(&basicLook);
	openButton.setLookAndFeel(&basicLook);
	zoomOutButton.setLookAndFeel(&basicLook);
	zoomInButton.setLookAndFeel(&basicLook);
	
	zoomOutButton.getProperties().set("isRounded", true);
	zoomInButton.getProperties().set("isRounded", true);
	
	openButton.setColour(TextButton::buttonColourId, Colours::white);	
	playButton.setColour(TextButton::buttonColourId, Colours::white);
	playButton.setColour(TextButton::buttonOnColourId, Colours::yellow);
	zoomOutButton.setColour(TextButton::buttonColourId, Colours::white);
	zoomInButton.setColour(TextButton::buttonColourId, Colours::white);
	
	playButton.setClickingTogglesState(true);	
	
	stopButton.setColour(TextButton::buttonColourId, Colours::white);	
	
	playButton.setImages(cUtils::createPlayButtonPath(25), 
						 cUtils::createPlayButtonPath(25), 
						 cUtils::createPauseButtonPath(25), 
						 cUtils::createPlayButtonPath(25), 
						 cUtils::createPauseButtonPath(25));

	openButton.setImages(cUtils::createOpenButtonPath(25));		
	stopButton.setImages(cUtils::createStopButtonPath(25));
	
	zoomInButton.setImages(cUtils::createZoomInButtonPath(25));
	zoomOutButton.setImages(cUtils::createZoomOutButtonPath(25));
	
	//viewport = new Viewport();
	waveformDisplay->setBounds(10, 10, 500, 200);
	//viewport->setViewedComponent(waveformDisplay, false);
	//viewport->setScrollBarsShown(true, true);
	addAndMakeVisible(waveformDisplay);
    setSize (500, 250);

	if(File(getFilter()->getCurrentFile()).existsAsFile())
		waveformDisplay->setFile(File(getFilter()->getCurrentFile()));
	//waveformDisplay->setZoomFactor(1);

}

AudioFilePlaybackEditor::~AudioFilePlaybackEditor()
{
	waveformDisplay->stopTimer();
    getFilter()->editorBeingDeleted(this);
}

//==============================================================================
void AudioFilePlaybackEditor::resized()
{
	waveformDisplay->setBounds(BUTTON_SIZE+7, 5, getWidth()-(BUTTON_SIZE+12), getHeight()-10);
	//viewport->setBounds(BUTTON_SIZE+7, 5, getWidth()-20, getHeight()-10);	
	stopButton.setBounds(3, 5, BUTTON_SIZE, BUTTON_SIZE);
	playButton.setBounds(3, BUTTON_SIZE+5, BUTTON_SIZE, BUTTON_SIZE);
	openButton.setBounds(3, ((BUTTON_SIZE)*2)+5, BUTTON_SIZE, BUTTON_SIZE);
	zoomInButton.setBounds(3, ((BUTTON_SIZE)*3)+5, BUTTON_SIZE, BUTTON_SIZE);
	zoomOutButton.setBounds(3, ((BUTTON_SIZE)*4)+5, BUTTON_SIZE, BUTTON_SIZE);
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
