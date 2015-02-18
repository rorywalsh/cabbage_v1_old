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
AutomationProcessor::AutomationProcessor():
newTableAdded(false),
updateCount(0)
{
	csound = new Csound();
	csound->SetHostData(this);

    csoundParams = new CSOUND_PARAMS();

    csoundParams->nchnls_override = this->getNumOutputChannels();

    csoundParams->displays = 0;
    csound->SetParams(csoundParams);

    
	File csdFile("/home/rory/Desktop/test.csd");
	
    csCompileResult = csound->Compile(const_cast<char*>(csdFile.getFullPathName().toUTF8().getAddress()));
    //csoundSetBreakpointCallback(csound->GetCsound(), breakpointCallback, (void*)this);
    if(csCompileResult==OK)
    {
        Logger::writeToLog("compiled Ok");
        csdKsmps = csound->GetKsmps();
        
        if(csound->GetSpout()==nullptr);
        CSspout = csound->GetSpout();
        CSspin  = csound->GetSpin();
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
}


AutomationEditor* AutomationProcessor::getEditor()
{
	if((AutomationEditor*)this->getActiveEditor())
		return (AutomationEditor*)this->getActiveEditor();
	else
		assert(0);
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
	messageQueue.addOutgoingTableUpdateMessageToQueue(statement, id);
	sendOutgoingMessagesToCsound();
	//getEditor()->updateComboBoxItems();
		
}

void AutomationProcessor::updateAutomatableNodefStatement(int tableNumber, String newStatement)
{
	for(int i=0;i<automatableNodes.size();i++)
	{
		if(automatableNodes.getReference(i).fTableNumber==tableNumber)
			automatableNodes.getReference(i).fStatement = newStatement;
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
		//getEditor()->addTable(tableNum, gen);	
		newTableAdded = false;	
	}
}

void AutomationProcessor::updateAutomationValues()
{
	//automationCurveValue = csound->GetChannel("automationCurve1");
	for(int i=0;i<automatableNodes.size();i++)
	{
		automatableNodes.getReference(i).value = csound->GetChannel(automatableNodes.getReference(i).channelName.toUTF8().getAddress());
	}
	
	//put the brakes on this. No need to update that often...
	updateCount = (updateCount>100 ? 0:updateCount+1);
	
	if(updateCount==0)
		getEditor()->updateScrubberPosition(csound->GetChannel("scrubberPosition"), automatableNodes.getReference(0).fTableNumber);

	sendChangeMessage();
}

//==============================================================================
void AutomationProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{

		float* audioBuffer;
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
							automationCurveValue = csound->GetChannel("automationCurve1");
							
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

//==============================================================================
// getTable data from Csound so table editor can draw table
//==============================================================================
StringArray AutomationProcessor::getTableStatement(int tableNum)
{
    StringArray fdata;
    fdata.add(String::empty);
#ifndef Cabbage_No_Csound
    if(csCompileResult==OK)
    {
        MYFLT* argsPtr, *temp;
        int noOfArgs = csoundGetTableArgs(csound->GetCsound(), &argsPtr, tableNum);
        if(noOfArgs!=-1)
        {
            int tableSize = csound->GetTable(temp, tableNum);
            fdata.add(String(tableNum));
            fdata.add("0");
            fdata.add(String(tableSize));
            if(noOfArgs==0)
                fdata.add(String(1));
            else
                for(int i=0; i<noOfArgs; i++)
                {
                    fdata.add(String(argsPtr[i]));
                }
        }
    }
#endif
    return fdata;
}
//==============================================================================
const Array<double, CriticalSection> AutomationProcessor::getTable(int tableNum)
{
    Array<double, CriticalSection> points;
    if(csCompileResult==OK)
    {
        int tableSize=0;
#ifndef Cabbage_No_Csound
        MYFLT* temp;
        tableSize = csound->GetTable(temp, tableNum);
#else
        float *temp;
#endif
        if(tableSize>0)
            points = Array<double, CriticalSection>(temp, tableSize);
    }
    return points;
}

const Array<float, CriticalSection> AutomationProcessor::getTableFloats(int tableNum)
{
    Array<float, CriticalSection> points;
    if(csCompileResult==OK)
    {
        points.clear();
        
        int tableSize=0;
#ifndef Cabbage_No_Csound
        
        tableSize = csound->TableLength(tableNum);
        temp.clear();
        //not good if table size is -1!
        if(tableSize<0)
            return points;
        
        temp.reserve(tableSize);
        csound->TableCopyOut(tableNum, &temp[0]);
#else
        float *temp;
#endif
        if(tableSize>0)
            points = Array<float, CriticalSection>(&temp[0], tableSize);
    }
    return points;
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
