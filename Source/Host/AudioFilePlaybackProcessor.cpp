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
updateCounter(0),
beatOffset(0),
gain(1.f),
pan(.5f),
envPointIncr(0),
sampleIndex(0),
gainOutputValue(1),
isLinkedToMasterTransport(false)
{
	//setupAudioFile(File("/home/rory/Documents/BeesInMarch4.wav"));
	bufferingAudioFileSource = nullptr;
	parameterNames.add("Gain");
	parameterNames.add("Pan");
}

AudioFilePlaybackProcessor::~AudioFilePlaybackProcessor()
{
	thread.stopThread(10);
	isSourcePlaying = false;
	delete bufferingAudioFileSource;
	fileSource= nullptr;
}

void AudioFilePlaybackProcessor::setupAudioFile (File soundfile)
{
	if(soundfile.exists())
	{
		AudioFormatManager formatManager;
		formatManager.registerBasicFormats();   
		AudioFormatReader* reader = formatManager.createReaderFor(soundfile);   
		if (reader != 0)
		{
			fileSource = new AudioFormatReaderSource (reader, true);
			numFileChannels = reader->numChannels;
			//setting up buffer with two channels, regardless of the number of channels in our soundfile
			bufferingAudioFileSource = new BufferingAudioSource(fileSource, thread, true, 32768, 2);
			samplingRate =reader->sampleRate;
			thread.startThread();
			currentFile = soundfile.getFullPathName();
			totalLength = bufferingAudioFileSource->getTotalLength();
		}		
	}
}



void AudioFilePlaybackProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
	if(sampleRate==0)
		sampleRate = samplingRate;

	if(bufferingAudioFileSource)
		bufferingAudioFileSource->prepareToPlay(samplesPerBlock, sampleRate);
}

void AudioFilePlaybackProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
	if(isLinkedToMasterTransport)
	{
		if (getPlayHead() != 0 && getPlayHead()->getCurrentPosition(hostInfo))
		{
			if((!hostInfo.isPlaying && hostInfo.ppqPosition==0))
			{
				bufferingAudioFileSource->setNextReadPosition(0);
				sampleIndex=0;
			}
			
			if(hostInfo.isPlaying && hostInfo.ppqPosition>=beatOffset)
			{
				playSoundFile(buffer);
			}
			else
			{
				for (int i = getNumInputChannels(); i < getNumOutputChannels(); ++i)
				{
					buffer.clear (i, 0, buffer.getNumSamples());
				}			
			}
				
		}
	}
	else
		playSoundFile(buffer);
}

float AudioFilePlaybackProcessor::getGainEnvelop(int& index)
{ 
	const int duration = envPoints[envPointIncr+1].getX()*totalLength-envPoints[envPointIncr].getX()*totalLength;;
	
	if(envPointIncr<envPoints.size()-1)
	{
		if(index<duration)
		{
			const float amp1 = 1.f-envPoints[envPointIncr].getY();
			const float amp2 = 1.f-envPoints[envPointIncr+1].getY();
			float scale =  (float(index)/float(duration));
			gainOutputValue = amp1 + (float(index)/float(duration))*(amp2-amp1); 
			return gainOutputValue;
		}
		else
		{
			envPointIncr++;
			index=0;
			return gainOutputValue;
		}
	}
	else return 0.f;
}

void AudioFilePlaybackProcessor::addEnvDataPoint(Point<double> point)
{
	envPoints.add(point);
}
	
void AudioFilePlaybackProcessor::playSoundFile(AudioSampleBuffer& buffer)
{
	float *audioFileBuffer, *audioOutputBuffer;
	if(bufferingAudioFileSource)
	{
		AudioSampleBuffer output (2, buffer.getNumSamples());
		sourceChannelInfo.buffer = &output;
		sourceChannelInfo.startSample = 0;
		sourceChannelInfo.numSamples = output.getNumSamples();

		if(bufferingAudioFileSource->getNextReadPosition()>=bufferingAudioFileSource->getTotalLength())
		{
			bufferingAudioFileSource->setNextReadPosition(0);
			envPointIncr=0;
			sampleIndex=0;
		}

		if(isSourcePlaying)
			bufferingAudioFileSource->getNextAudioBlock(sourceChannelInfo); 
		else
			output.clear();
		
		for(int i=0;i<buffer.getNumSamples();i++)
		{
			float gainValue = getGainEnvelop(sampleIndex)*gain;
			for (int channel = 0; channel < output.getNumChannels(); ++channel)
			{
				audioFileBuffer = output.getWritePointer(channel, 0);
				audioOutputBuffer = buffer.getWritePointer(channel, 0);
				audioOutputBuffer[i] = audioFileBuffer[i]*gainValue;
			}

			sampleIndex++;
		}

//		for (int channel = 0; channel < getNumOutputChannels(); ++channel)
//		{
//			buffer.copyFrom(channel, 0, output, channel, 0, sourceChannelInfo.numSamples);
//		}

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
    if(index==0)
		return gain;
	else
		return pan;
}

void AudioFilePlaybackProcessor::setParameter (int index, float newValue)
{
	if(index==0)
		gain = newValue;
	else if(index==1)
		gain = newValue;
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
	XmlElement xml ("SOUNDFILER_PLUGIN_SETTINGS");
	xml.setAttribute("Soundfile", currentFile);	
	xml.setAttribute("gain", gain);
	xml.setAttribute("pan", pan);
	xml.setAttribute("isLinkedToMasterTransport", this->isLinkedToMasterTransport);
	xml.setAttribute("beatOffset", this->beatOffset);
	copyXmlToBinary (xml, destData);
}

void AudioFilePlaybackProcessor::setStateInformation (const void* data, int sizeInBytes)
{
	ScopedPointer<XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
	if (xmlState->hasTagName ("SOUNDFILER_PLUGIN_SETTINGS"))
	{	
		setupAudioFile(xmlState->getStringAttribute("Soundfile"));
		gain = xmlState->getDoubleAttribute("gain");
		pan = xmlState->getDoubleAttribute("pan");
		isLinkedToMasterTransport = (bool)xmlState->getIntAttribute("isLinkedToMasterTransport");
		beatOffset = xmlState->getIntAttribute("beatOffset");
	}
}
