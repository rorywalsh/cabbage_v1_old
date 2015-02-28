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
	csound = new Csound();
	csound->SetHostData(this);
	csoundParams = new CSOUND_PARAMS();
	csoundParams->nchnls_override = this->getNumOutputChannels();
	csoundParams->displays = 0;
	csound->SetParams(csoundParams);
	
	#if defined(MACOSX)
		File csdFile("/Users/walshr/sourcecode/cabbageaudio/cabbage/Source/Host/AutomationTrack.csd");
	#else
		File csdFile("/home/rory/sourcecode/cabbageaudio/cabbage/Source/Host/AutomationTrack.csd");
	#endif
	
	csCompileResult = csound->Compile(const_cast<char*>(csdFile.getFullPathName().toUTF8().getAddress()));
	
	if(csCompileResult==OK)
	{
		Logger::writeToLog("compiled Ok");
		csdKsmps = csound->GetKsmps();
		if(csound->GetSpout()==nullptr);
		CSspout = csound->GetSpout();
		CSspin = csound->GetSpin();
		cs_scale = csound->Get0dBFS();
		csndIndex = csound->GetKsmps();
		this->setLatencySamples(csound->GetKsmps());
		updateHostDisplay();
		csoundStatus = true;
	}
	
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
											int index,
											int gen,
											String statement)
{
	const String tableNumber(String(id)+String::formatted("%03d", index));
	if(statement.isEmpty())
	{
		statement = "f"+tableNumber+" 0 4096 -7 0 4096 0";
	}	
	const String channel("auto_"+tableNumber);
	AutomationProcessor::AutomatableNode node(nodeName, parameterName, id, index);
	node.fStatement = statement;
	node.channelName = channel;	
	node.genRoutine = -7;
	node.fTableNumber = tableNumber.getIntValue();
	
	automatableNodes.add(node);		
	newTableAdded = true;		
	envelopes.add(AbstractEnvelope());    	

	if(AutomationEditor* editor = getEditor())
	{
		editor->updateComboBoxItems();		
	}
	
	messageQueue.addOutgoingTableUpdateMessageToQueue(statement, tableNumber.getIntValue());
	sendOutgoingMessagesToCsound();
		
}

AutomationProcessor::AutomatableNode AutomationProcessor::getAutomatableNode(int index)
{
	return automatableNodes.getReference(index);
}

int AutomationProcessor::getNumberOfAutomatableNodes()
{
	return automatableNodes.size();
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
	{
		updateEnvPoints(0, gainEnv->getHandlePoints());
		updatefTableData(gainEnv);
	}
}

void AutomationProcessor::updateAutomationValues()
{
	//put the brakes on this. No need to update that often...
	updateCount = (updateCount>100 ? 0:updateCount+1);
    scrubberPosition = csound->GetChannel("scrubberPosition");
	
	if(updateCount==0)
	{
		for(int i=0;i<automatableNodes.size();i++)
		{
			String test = automatableNodes.getReference(i).channelName; 
			graph->updateAutomatedNodes(automatableNodes.getReference(i).nodeID, 
										automatableNodes.getReference(i).parameterIndex, 
										csound->GetChannel(automatableNodes.getReference(i).channelName.toUTF8().getAddress()));
		}								
	}
}

void AutomationProcessor::updatefTableData(BreakpointEnvelope* table)
{
#ifndef Cabbage_No_Csound
	int genRoutine = -7;
    Array<double> pFields = table->getEnvelopeAsPfields();
    if( genRoutine==-7)
    {
        FUNC *ftpp;
        EVTBLK  evt;
        memset(&evt, 0, sizeof(EVTBLK));
        evt.pcnt = 5+pFields.size();
        evt.opcod = 'f';
        evt.p[0]=0;

        //setting table number to 0.
        evt.p[1]=0;
        evt.p[2]=0;
        evt.p[3]=4096;
        evt.p[4]=genRoutine;
        if(genRoutine==5)
        {
            for(int i=0; i<pFields.size()-1; i++)
                evt.p[5+i]= jmax(0.00001, pFields[i+1]);
        }
        else if(genRoutine==-7)
        {
            for(int i=0; i<pFields.size()-1; i++)
                evt.p[5+i]= pFields[i+1];
        }
        else
        {
            for(int i=0; i<pFields.size(); i++)
                evt.p[5+i] = pFields[i];
        }


        StringArray fStatement;
		int pCnt=0;
        for(int i=0; i<evt.pcnt-1; i++)
		{
            fStatement.add(String(evt.p[i]));
			//cUtils::debug(i, fStatement[i]);
			pCnt=i;
		}

        //now set table number and set score char to f
        fStatement.set(1, String(table->getUid()));
        fStatement.set(0, "f");

        //Logger::writeToLog(fStatement.joinIntoString(" "));
        messageQueue.addOutgoingTableUpdateMessageToQueue(fStatement.joinIntoString(" "), table->getUid());
    }
#endif
}

void AutomationProcessor::sendOutgoingMessagesToCsound()
{
	if(csCompileResult==OK)
	{
		if (getPlayHead() != 0 && getPlayHead()->getCurrentPosition (hostInfo))
		{
			csound->SetChannel(CabbageIDs::hostbpm.toUTF8(), hostInfo.bpm);
			csound->SetChannel(CabbageIDs::timeinseconds.toUTF8(), hostInfo.timeInSeconds);
			csound->SetChannel(CabbageIDs::isplaying.toUTF8(), hostInfo.isPlaying);
			csound->SetChannel(CabbageIDs::isrecording.toUTF8(), hostInfo.isRecording);
			csound->SetChannel(CabbageIDs::hostppqpos.toUTF8(), hostInfo.ppqPosition);
			csound->SetChannel(CabbageIDs::timeinsamples.toUTF8(), hostInfo.timeInSamples);
			csound->SetChannel(CabbageIDs::timeSigDenom.toUTF8(), hostInfo.timeSigDenominator);
			csound->SetChannel(CabbageIDs::timeSigNum.toUTF8(), hostInfo.timeSigNumerator);
		}
		for(int i=0; i<messageQueue.getNumberOfOutgoingChannelMessagesInQueue(); i++)
		{
			//update Csound function tables with values from table widget
			if(messageQueue.getOutgoingChannelMessageFromQueue(i).type=="updateTable")
			{
				//Logger::writeToLog(messageQueue.getOutgoingChannelMessageFromQueue(i).fStatement.toUTF8());
				const int tableNum = messageQueue.getOutgoingChannelMessageFromQueue(i).tableNumber;
				//update table data for saving...
				updateAutomatableNodefStatement(tableNum, messageQueue.getOutgoingChannelMessageFromQueue(i).fStatement);
				csound->InputMessage(messageQueue.getOutgoingChannelMessageFromQueue(i).fStatement.getCharPointer());
			}
			//catch string messags
			else if(messageQueue.getOutgoingChannelMessageFromQueue(i).type==CabbageIDs::stringchannel)
			{
				Logger::writeToLog(messageQueue.getOutgoingChannelMessageFromQueue(i).channelName);
				Logger::writeToLog(messageQueue.getOutgoingChannelMessageFromQueue(i).stringVal);
				csound->SetChannel(messageQueue.getOutgoingChannelMessageFromQueue(i).channelName.getCharPointer(),
				messageQueue.getOutgoingChannelMessageFromQueue(i).stringVal.toUTF8().getAddress());
			}
			else
				csound->SetChannel(messageQueue.getOutgoingChannelMessageFromQueue(i).channelName.getCharPointer(),
			messageQueue.getOutgoingChannelMessageFromQueue(i).value);
			}
			messageQueue.flushOutgoingChannelMessages();
		}
		
		if(newTableAdded == true)
		{
			//call performKsmps to instantiate new tables...
			const String iStatement("i1 0 36000 "+String(automatableNodes.getReference(automatableNodes.size()-1).fTableNumber));
			csound->InputMessage(iStatement.toUTF8().getAddress());
			csound->PerformKsmps();
			const int gen = automatableNodes.getReference(automatableNodes.size()-1).genRoutine;
			const int tableNum = automatableNodes.getReference(automatableNodes.size()-1).fTableNumber;
			if(AutomationEditor* editor = getEditor())
			{
			const Colour tableColour = Colour(Random::getSystemRandom().nextInt(255),
                         Random::getSystemRandom().nextInt(255),
                         Random::getSystemRandom().nextInt(255));
			editor->addTable(tableColour, tableNum);
			}
			
			newTableAdded = false;
		}
}	

void AutomationProcessor::updateAutomatableNodefStatement(int tableNumber, String newStatement)
{
	for(int i=0;i<automatableNodes.size();i++)
	{
		if(automatableNodes.getReference(i).fTableNumber==tableNumber)
		automatableNodes.getReference(i).fStatement = newStatement;
	}
}	
//==============================================================================
void AutomationProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
		int numSamples = buffer.getNumSamples();
		if(isSuspended()==false)
		{
			if(csCompileResult==OK)
			{
					for(int i=0; i<numSamples; i++, csndIndex++)
					{
						if(csndIndex == csdKsmps)
						{
							getCallbackLock().enter(); 
							sendOutgoingMessagesToCsound();
							updateAutomationValues();								
							csCompileResult = csound->PerformKsmps();   					
							getCallbackLock().exit();
							csndIndex = 0;
						}
						
						buffer.clear();	   
					}

			}
		}
		
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
	innerXml->setAttribute("genRoutine", node.genRoutine);
	innerXml->setAttribute("fStatement", node.fStatement);	
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
				const int genRoutine = e->getIntAttribute("genRoutine");
				const String fStatement = e->getStringAttribute("fStatement");				
				addAutomatableNode(nodeName, parametername, nodeID, parameterIndex, genRoutine, fStatement);
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
