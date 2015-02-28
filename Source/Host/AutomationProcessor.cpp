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

#include "AutomationProcessor.h"
#include "AutomationEditor.h"


//==============================================================================
AutomationProcessor::AutomationProcessor(FilterGraph* filterGraph):
graph(filterGraph),
newTableAdded(false),
updateCount(0),
envSampleIndex(0),
totalLength(10.f),
isSourcePlaying(false),
shouldLoop(false),
isLinkedToMasterTransport(false)
{
	suspendProcessing(true);
}

AutomationProcessor::~AutomationProcessor()
{
if(getEditor())
    delete getEditor();
	
envelopes.clear();
}


AutomationEditor* AutomationProcessor::getEditor()
{
	if((AutomationEditor*)this->getActiveEditor())
		return (AutomationEditor*)this->getActiveEditor();
	else
		return nullptr;
}

void AutomationProcessor::addAutomatableNode(String nodeName, 
											 String parameterName, 
											 int32 id, 
											 int index)
{
	
	AutomationProcessor::AutomatableNode node(nodeName, parameterName, id, index);
	
	automatableNodes.add(node);		
	newTableAdded = true;		
	envelopes.add(AbstractEnvelope());
	
    const Colour tableColour = Colour(Random::getSystemRandom().nextInt(255),
                         Random::getSystemRandom().nextInt(255),
                         Random::getSystemRandom().nextInt(255));
    	

	if(AutomationEditor* editor = getEditor())
	{
		editor->updateComboBoxItems();
		editor->addTable(tableColour);
		newTableAdded = false;			
	}
		
}

AutomationProcessor::AutomatableNode AutomationProcessor::getAutomatableNode(int index)
{
	return automatableNodes.getReference(index);
}

int AutomationProcessor::getNumberOfAutomatableNodes()
{
	return automatableNodes.size();
}

//==============================================================================
void AutomationProcessor::updateAutomationValues()
{
	//put the brakes on this. No need to update that often...
	updateCount = (updateCount>5 ? 0:updateCount+1);
    scrubberPosition = envSampleIndex/44100.f;
	if(updateCount==0)
	{
		for(int i=0;i<automatableNodes.size();i++)
			graph->updateAutomatedNodes(automatableNodes.getReference(i).nodeID, 
										automatableNodes.getReference(i).parameterIndex, 
										automatableNodes.getReference(i).value);
		
		
	}
}

void AutomationProcessor::updateEnvPoints(int env, Array<Point<double>> points)
{
	envelopes.getReference(env).envPoints.clear();
	for(int i=0;i<points.size();i++)
	{
		envelopes.getReference(env).envPoints.add(points[i].getX());
		envelopes.getReference(env).envPoints.add(points[i].getY());
	}
	
	//envelopes.getReference(env).envPoints.swapWith(points);
}

void AutomationProcessor::changeListenerCallback(ChangeBroadcaster* source)
{
	if(BreakpointEnvelope* gainEnv = (BreakpointEnvelope*)source)
		updateEnvPoints(0, gainEnv->getHandlePoints());
}

float AutomationProcessor::getGainEnvelop(int envIndex)
{ 
//	int currentPointIndex = envelopes[envIndex].currentPointIndex;
//	
//	int duration = envelopes[envIndex].envPoints[currentPointIndex*2+2]*
//									totalLength-envelopes[envIndex].envPoints[currentPointIndex*2]*totalLength;;
//
//	duration = duration*44100;
//	float gainOutputValue;
//	AbstractEnvelope currentEnv(envelopes[envIndex]);
//
//	if(currentEnv.currentPointIndex<currentEnv.envPoints.size()/2)
//	{
//		const int index = currentEnv.currentSampleIndex;
//		if(index<duration)
//		{
//			const float amp1 = 1.f-currentEnv.envPoints[currentPointIndex*2];
//			const float amp2 = 1.f-currentEnv.envPoints[currentPointIndex*2+2];
//			float scale =  (float(index)/float(duration));
//			gainOutputValue = amp1 + (float(index)/float(duration))*(amp2-amp1); 
//			envelopes.getReference(envIndex).currentSampleIndex++;
//			return gainOutputValue;
//		}
//		else
//		{
//			envelopes.getReference(envIndex).currentPointIndex+1;
//			envelopes.getReference(envIndex).currentSampleIndex=0;
//			return gainOutputValue;
//		}
//	}
//	else 
		return 0.f;		
}

void AutomationProcessor::generateEnvelope(AudioSampleBuffer& buffer)
{
	for(int i=0;i<buffer.getNumSamples();i++)
	{
		for(int i=0;i<automatableNodes.size();i++)
			automatableNodes.getReference(i).value  = getGainEnvelop(i);
		
		envSampleIndex++;
		if(envSampleIndex>totalLength*44100)
			envSampleIndex=0;

	}
	//buffer.clear();
}
	
//==============================================================================
void AutomationProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
		if(isLinkedToMasterTransport)
		{
			if (getPlayHead() != 0 && getPlayHead()->getCurrentPosition(hostInfo))
			{
				if((!hostInfo.isPlaying && hostInfo.ppqPosition==0))
				{
					isSourcePlaying=true;
					envSampleIndex=0;
				}
				
				if(hostInfo.isPlaying)
				{
					if(isSourcePlaying)
						generateEnvelope(buffer);
				}					
			}
		}
		else
		{
			if(isSourcePlaying)
				generateEnvelope(buffer);
		}
	
	updateAutomationValues();	
	//buffer.clear();
		
}
//==============================================================================
void AutomationProcessor::getStateInformation (MemoryBlock& destData)
{
	XmlElement xml ("AUTOMATION_PLUGIN_SETTINGS");
	
	for(int i=0;i<automatableNodes.size();i++)
	{
		cUtils::showMessage(createAutomationXML (automatableNodes.getReference(i))->getAllSubText());
		xml.addChildElement (createAutomationXML (automatableNodes.getReference(i)));
	}	

	xml.writeToFile(File("/home/rory/Desktop/output.txt"), String::empty); 
	copyXmlToBinary (xml, destData);
}
//==============================================================================
XmlElement* AutomationProcessor::createAutomationXML(AutomationProcessor::AutomatableNode node)
{
	XmlElement* innerXml = new XmlElement ("NODES");
	innerXml->setAttribute("nodeName", node.nodeName);
	innerXml->setAttribute("parameterName", node.parametername);
	innerXml->setAttribute("nodeId", node.nodeID);
	innerXml->setAttribute("parameterId", node.parameterIndex);
	return innerXml;
}
//==============================================================================
void AutomationProcessor::setStateInformation (const void* data, int sizeInBytes)
{
	ScopedPointer<XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
	// check we're looking at the right kind of document..
	//cUtils::showMessage(xmlState->getNumAttributes());
		 
	// check we're looking at the right kind of document..
	if (xmlState->hasTagName ("AUTOMATION_PLUGIN_SETTINGS"))
	{
		forEachXmlChildElement(*xmlState, e)
		{
			if (e->hasTagName ("NODES"))
			{	
				const String nodeName = e->getStringAttribute("nodename");
				const String parametername = e->getStringAttribute("parameterName");
				const int nodeID = e->getIntAttribute("nodeId");
				const int parameterIndex = e->getIntAttribute("parameterId");			
				addAutomatableNode(nodeName, parametername, nodeID, parameterIndex);
			}	 
		}
	}
}

//==============================================================================
const String AutomationProcessor::getName() const
{
    return JucePlugin_Name;
}

int AutomationProcessor::getNumParameters()
{
    return 0;
}

float AutomationProcessor::getParameter (int index)
{
    return 0.0f;
}

void AutomationProcessor::setParameter (int index, float newValue)
{
}

const String AutomationProcessor::getParameterName (int index)
{
    return String();
}

const String AutomationProcessor::getParameterText (int index)
{
    return String();
}

const String AutomationProcessor::getInputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

const String AutomationProcessor::getOutputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

bool AutomationProcessor::isInputChannelStereoPair (int index) const
{
    return true;
}

bool AutomationProcessor::isOutputChannelStereoPair (int index) const
{
    return true;
}

bool AutomationProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AutomationProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AutomationProcessor::silenceInProducesSilenceOut() const
{
    return false;
}

double AutomationProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AutomationProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AutomationProcessor::getCurrentProgram()
{
    return 0;
}

void AutomationProcessor::setCurrentProgram (int index)
{
}

const String AutomationProcessor::getProgramName (int index)
{
    return String();
}

void AutomationProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void AutomationProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void AutomationProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

//==============================================================================
bool AutomationProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* AutomationProcessor::createEditor()
{
    return new AutomationEditor (*this);
}
