/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/

#include "AudioFilePlaybackProcessor.h"
#include "AudioFilePlaybackEditor.h"


//==============================================================================
AudioFilePlaybackProcessor::AudioFilePlaybackProcessor():
thread("audio file playback"), 
isSourcePlaying(false), 
sourceSampleRate(44100),
rmsLeft(0),
rmsRight(0),
updateCounter(0)
{
	//setupAudioFile(File("/home/rory/Documents/BeesInMarch4.wav"));
	bufferingAudioFileSource = nullptr;
	parameterNames.add("Volume");
	parameterNames.add("Pan");
	parameterNames.add("StartStop");
	parameterNames.add("Position");	
}

AudioFilePlaybackProcessor::~AudioFilePlaybackProcessor()
{
	thread.stopThread(10);
	delete bufferingAudioFileSource;
	fileSource= nullptr;
}

void AudioFilePlaybackProcessor::setupAudioFile (File soundfile)
{
	if(soundfile.exists())
	{
		AudioFormatManager formatManager;
		formatManager.registerBasicFormats();   
		AudioFormatReader* reader = formatManager.createReaderFor (soundfile);   
		if (reader != 0)
		{
			fileSource = new AudioFormatReaderSource (reader, true);
			bufferingAudioFileSource = new BufferingAudioSource(fileSource, thread, true, 32768, 2);
			samplingRate =reader->sampleRate;
			thread.startThread();
			currentFile = soundfile.getFullPathName();
		}		
	}
}



void AudioFilePlaybackProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
	if(sampleRate==0)
		sampleRate = samplingRate;
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
	if(bufferingAudioFileSource)
		bufferingAudioFileSource->prepareToPlay(samplesPerBlock, sampleRate);
}

void AudioFilePlaybackProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
	if(bufferingAudioFileSource)
	{
		AudioSampleBuffer output (2, buffer.getNumSamples());
		sourceChannelInfo.buffer = &output;
		sourceChannelInfo.startSample = 0;
		sourceChannelInfo.numSamples = output.getNumSamples();

		if(isSourcePlaying)
			bufferingAudioFileSource->getNextAudioBlock(sourceChannelInfo); 
		else
			output.clear();

		for (int channel = 0; channel < getNumInputChannels(); ++channel)
		{
			buffer.copyFrom(channel, 0, output, channel, 0, sourceChannelInfo.numSamples);
		}

		for (int i = getNumInputChannels(); i < getNumOutputChannels(); ++i)
		{
			buffer.clear (i, 0, buffer.getNumSamples());
		}
		
		rmsLeft = buffer.getRMSLevel(0, 0, buffer.getNumSamples());
		rmsRight = buffer.getRMSLevel(1, 0, buffer.getNumSamples());
		
		
		if(updateCounter==0) 
			sendActionMessage("rmsValues "+String(rmsLeft)+" "+String(rmsRight));
			
		updateCounter++;
		if(updateCounter>5)
			updateCounter=0;	
	}
	
}

//==============================================================================
const String AudioFilePlaybackProcessor::getName() const
{
    return JucePlugin_Name;
}

int AudioFilePlaybackProcessor::getNumParameters()
{
    return 	parameterNames.size();
}

float AudioFilePlaybackProcessor::getParameter (int index)
{
    return 0.0f;
}

void AudioFilePlaybackProcessor::setParameter (int index, float newValue)
{
}

const String AudioFilePlaybackProcessor::getParameterName (int index)
{
    return 	parameterNames.getReference(index);
}

const String AudioFilePlaybackProcessor::getParameterText (int index)
{
    return String::empty;
}

const String AudioFilePlaybackProcessor::getInputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

const String AudioFilePlaybackProcessor::getOutputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

bool AudioFilePlaybackProcessor::isInputChannelStereoPair (int index) const
{
    return true;
}

bool AudioFilePlaybackProcessor::isOutputChannelStereoPair (int index) const
{
    return true;
}

bool AudioFilePlaybackProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool AudioFilePlaybackProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

int AudioFilePlaybackProcessor::getNumPrograms()
{
    return 0;
}

int AudioFilePlaybackProcessor::getCurrentProgram()
{
    return 0;
}

void AudioFilePlaybackProcessor::setCurrentProgram (int index)
{
}

const String AudioFilePlaybackProcessor::getProgramName (int index)
{
    return String::empty;
}

void AudioFilePlaybackProcessor::changeProgramName (int index, const String& newName)
{
}


void AudioFilePlaybackProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

//==============================================================================
bool AudioFilePlaybackProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* AudioFilePlaybackProcessor::createEditor()
{
    return new AudioFilePlaybackEditor (this);
}

//==============================================================================
void AudioFilePlaybackProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
	XmlElement xml ("SOUNDFILER_PLUGIN_SETTINGS");
	xml.setAttribute("Soundfile", currentFile);	
	copyXmlToBinary (xml, destData);
}

void AudioFilePlaybackProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
	ScopedPointer<XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
	if (xmlState->hasTagName ("SOUNDFILER_PLUGIN_SETTINGS"))
	{	
		for(int i=0; i<xmlState->getNumAttributes(); i++)
		{
			if(xmlState->getAttributeName(i)=="Soundfile")
			{
				setupAudioFile(File(xmlState->getAttributeValue(i)));
			}
		}		
	}
}
