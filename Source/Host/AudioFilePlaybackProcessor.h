/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/

#ifndef __AUDIOFILEPLUGINPROCESSOR_H_99BF5AFC__
#define __AUDIOFILEPLUGINPROCESSOR_H_99BF5AFC__

#include "../JuceLibraryCode/JuceHeader.h"


//==============================================================================
/**
*/
class AudioFilePlaybackProcessor  : public AudioProcessor,
									public ActionBroadcaster
{
public:
    //==============================================================================
    AudioFilePlaybackProcessor();
    ~AudioFilePlaybackProcessor();

      //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock);
    void releaseResources();

    void processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages);

    //==============================================================================
    AudioProcessorEditor* createEditor();
    bool hasEditor() const;

    //==============================================================================
    const String getName() const;
	
	void setBeatOffset(int offset)
	{
		beatOffset = offset;
	}

	void linkToMasterTransport(bool val)
	{
		isLinkedToMasterTransport = val;
	} 

	bool linkedToMasterTransport()
	{
		return isLinkedToMasterTransport;
	}
 
	int getBeatsOffset()
	{
		return beatOffset;
	}
 
	int getNumParameters();

    float getParameter (int index);
    void setParameter (int index, float newValue);

    const String getParameterName (int index);
    const String getParameterText (int index);

    const String getInputChannelName (int channelIndex) const;
    const String getOutputChannelName (int channelIndex) const;
    bool isInputChannelStereoPair (int index) const;
    bool isOutputChannelStereoPair (int index) const;

    bool acceptsMidi() const;
    bool producesMidi() const;


    //==============================================================================
    int getNumPrograms();
    int getCurrentProgram();
    void setCurrentProgram (int index);
    const String getProgramName (int index);
    void changeProgramName (int index, const String& newName);

    //==============================================================================
    void getStateInformation (MemoryBlock& destData);
    void setStateInformation (const void* data, int sizeInBytes);
	bool silenceInProducesSilenceOut(void)const {return true;}
	double getTailLengthSeconds(void)const { return 1.f; }

	String getCurrentFile(){		return currentFile;		}
	
	void setupAudioFile (File soundfile);
	bool isSourcePlaying;
	int sourceSampleRate;
	BufferingAudioSource* bufferingAudioFileSource;
	AudioPlayHead::CurrentPositionInfo hostInfo;
	void playSoundFile(AudioSampleBuffer& buffer);
	float getGainEnvelop(int& index);
	
	void addEnvDataPoint(Point<double> point);
	
	void clearEnvDataPoint()
	{
		envPoints.clear();
	}	
private:
	AudioSourceChannelInfo sourceChannelInfo;
	PositionableAudioSource* fileSource;
	AudioSampleBuffer* audioBuffer;
	int samplingRate;
    TimeSliceThread thread;
	float rmsLeft, rmsRight;
	int beatOffset;
	String currentFile;
	int updateCounter;
	int numFileChannels;
	int totalLength;
	float gainOutputValue;
	bool isLinkedToMasterTransport;
	StringArray parameterNames;
	float gain, pan;
	Array<Point<double>> envPoints;
	int envPointIncr, sampleIndex;
	//PositionableAudioSource* currentAudioFileSource;
	

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioFilePlaybackProcessor);
};

#endif  // __PLUGINPROCESSOR_H_99BF5AFC__
