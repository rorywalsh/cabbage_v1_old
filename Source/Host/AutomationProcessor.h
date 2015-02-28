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

#ifndef AUTOMATIONPLUGINPROCESSOR_H_INCLUDED
#define AUTOMATIONPLUGINPROCESSOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "FilterGraph.h"
#include <csound.hpp>
#include "../CabbageMessageSystem.h"
#include "BreakpointEnvelope.h"



class AutomationEditor;
//==============================================================================
/**
*/
class AutomationProcessor  : public AudioProcessor,
							 public ChangeBroadcaster,
							 public ChangeListener
{
public:

	class AutomatableNode 
	{
	public:
		int32 nodeID;
		int parameterIndex;
		int fTableNumber;
		int genRoutine;
		String nodeName;
		String parametername;
		String fStatement;
		int size;
		String channelName;
		float value;
		
		AutomatableNode(String node, String name, int32 id, int index):
		nodeID(id), parameterIndex(index), nodeName(node), parametername(name),
		size(4096)
		{}
	};

	class AbstractEnvelope
	{
	public:
		AbstractEnvelope():shape(1), currentPointIndex(0), currentSampleIndex(0)
		{
		cUtils::debug("Why is this being called so often?");	
		}
		Array<float> envPoints;
		int shape, currentPointIndex, currentSampleIndex; 		
	};

    //==============================================================================
    AutomationProcessor(FilterGraph* filterGraph);
    ~AutomationProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
	void generateEnvelope(AudioSampleBuffer& buffer);
    void releaseResources() override;
	XmlElement* createAutomationXML(AutomationProcessor::AutomatableNode node);
    void processBlock (AudioSampleBuffer&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    int getNumParameters() override;
    float getParameter (int index) override;
    void setParameter (int index, float newValue) override;

    const String getParameterName (int index) override;
    const String getParameterText (int index) override;

    const String getInputChannelName (int channelIndex) const override;
    const String getOutputChannelName (int channelIndex) const override;
    bool isInputChannelStereoPair (int index) const override;
    bool isOutputChannelStereoPair (int index) const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool silenceInProducesSilenceOut() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;	
	void sendOutgoingMessagesToCsound();
	AutomatableNode getAutomatableNode(int index);
	int getNumberOfAutomatableNodes();
	void updateAutomationValues();
	void updatefTableData(BreakpointEnvelope* table);
	void updateAutomatableNodefStatement(int tableNumber, String newStatement);
	void updateEnvPoints(int env, Array<Point<double>> points);
	void changeListenerCallback(ChangeBroadcaster* source);	
	float getGainEnvelop(int envIndex);
	
	
	float getAutomationValue()
	{
		const ScopedLock sl (getCallbackLock());
		return automationCurveValue;
	}
	
	CSOUND* getCsoundStruct()
	{
		return csound->GetCsound();
	}	
	
	AutomationEditor* getEditor();
	float automationCurveValue;
	void addAutomatableNode(String nodeName, String parameterString, int32 id, int index, int gen=-1, String statement="");
	
    const double getScrubberPosition()
    {
        //const MessageManagerLock lock;
        return scrubberPosition;
    }
    
	bool isSourcePlaying;
	CabbageMessageQueue messageQueue;
private:
	bool shouldLoop;
	bool isLinkedToMasterTransport;
	FilterGraph* graph;	
	int sourceSampleRate;
	ScopedPointer<XmlElement> xmlState;
    double scrubberPosition;
	Array<AutomatableNode> automatableNodes;
	AudioPlayHead::CurrentPositionInfo hostInfo;
	bool newTableAdded;
	float totalLength;
	int updateCount;
	int envSampleIndex;
	Array<AbstractEnvelope> envelopes;

	//=== Csound stuff ======
	std::vector<MYFLT> temp;
	bool csoundStatus;
	int csCompileResult;
	ScopedPointer<CSOUND_PARAMS> csoundParams;
	MYFLT cs_scale;
	ScopedPointer<Csound> csound; //Csound instance
	MYFLT *CSspin, *CSspout; //Csound audio IO pointers
	int csndIndex; //Csound sample counter
	int csdKsmps;
	int CSCompResult; //result of Csound performKsmps
	int pos;
	controlChannelInfo_s* csoundChanList;
	int numCsoundChannels; //number of Csound channels

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AutomationProcessor)
};


#endif  // PLUGINPROCESSOR_H_INCLUDED
