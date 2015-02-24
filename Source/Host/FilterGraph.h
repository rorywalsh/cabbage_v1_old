/*
  Copyright (c) 2013 - Raw Material Software Ltd, Rory Walsh

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

#ifndef __FILTERGRAPH_JUCEHEADER__
#define __FILTERGRAPH_JUCEHEADER__

class FilterInGraph;
class FilterGraph;
class NodeAudioProcessorListener;
class GraphDocumentComponent;

#include "../Source/Plugin/CabbagePluginProcessor.h"
#include "../Source/Plugin/CabbagePluginEditor.h"
#include "../CabbagePropertiesDialog.h"


const char* const filenameSuffix = ".filtergraph";
const char* const filenameWildcard = "*.filtergraph";

//simple class to hold midi mappings
class CabbageMidiMapping  
{
public:
	CabbageMidiMapping(int nodeID, int paramIndex, int chan, int ctrl):
														channel(chan),
														controller(ctrl),
														nodeId(nodeID),
														parameterIndex(paramIndex){}
	
int channel, controller, nodeId, parameterIndex;
};

//==============================================================================
/**
    A collection of filters and some connections between them.
*/
class FilterGraph   : public FileBasedDocument,
					  public ChangeListener,
					  public HighResolutionTimer,
					  public ActionBroadcaster,
					  public ActionListener
{
	
public:
    //==============================================================================
    FilterGraph (AudioPluginFormatManager& formatManager);
    ~FilterGraph();

    //==============================================================================
    AudioProcessorGraph& getGraph() noexcept         { return graph; }

    int getNumFilters() const noexcept;
    const AudioProcessorGraph::Node::Ptr getNode (const int index) const noexcept;
    const AudioProcessorGraph::Node::Ptr getNodeForId (const uint32 uid) const noexcept;

    void addFilter (const PluginDescription* desc, double x, double y);
	
	
	AudioProcessorGraph::Node::Ptr createNode(const PluginDescription* desc, int uid=-1);
	
	void addNativeCabbageFilter (String fileName, double x, double y);

    void removeFilter (const uint32 filterUID);
    void disconnectFilter (const uint32 filterUID);

    void removeIllegalConnections();

    void setNodePosition (const int nodeId, double x, double y);
    void getNodePosition (const int nodeId, double& x, double& y) const;

	void changeListenerCallback (ChangeBroadcaster*);
	void actionListenerCallback (const String &message);
	
	int getLastMovedNodeId()
	{
		return lastChangedNodeId;
	}

	int getLastMovedNodeParameterIndex()
	{
		return lastChangedNodeParameter;
	}	
    //==============================================================================
    int getNumConnections() const noexcept;
    const AudioProcessorGraph::Connection* getConnection (const int index) const noexcept;

    const AudioProcessorGraph::Connection* getConnectionBetween (uint32 sourceFilterUID, int sourceFilterChannel,
            uint32 destFilterUID, int destFilterChannel) const noexcept;

    bool canConnect (uint32 sourceFilterUID, int sourceFilterChannel,
                     uint32 destFilterUID, int destFilterChannel) const noexcept;

    bool addConnection (uint32 sourceFilterUID, int sourceFilterChannel,
                        uint32 destFilterUID, int destFilterChannel);

    void removeConnection (const int index);

    void removeConnection (uint32 sourceFilterUID, int sourceFilterChannel,
                           uint32 destFilterUID, int destFilterChannel);

    void clear();

	String findControllerForparameter(int32 nodeID, int parameterIndex);

    //==============================================================================

    XmlElement* createXml() const;
    void restoreFromXml (const XmlElement& xml);

    //==============================================================================
    String getDocumentTitle();
    Result loadDocument (const File& file);
    Result saveDocument (const File& file);
    File getLastDocumentOpened();
    void setLastDocumentOpened (const File& file);
	void createNodeFromXml (const XmlElement& xml);

	void addNodesToAutomationTrack(int32 id, int index);

    static const int midiChannelNumber;
	Array<CabbageMidiMapping> midiMappings;
	
	//------- play info and timer stuff ---------------
	void hiResTimerCallback();
	
	class AudioPlaybackHead: public AudioPlayHead
	{
		public:
			bool getCurrentPosition (CurrentPositionInfo &result)
			{
				result = playHeadPositionInfo;
				return true;
			}
			
			void setIsPlaying(bool val){  playHeadPositionInfo.isPlaying=val;	}
			
			void setTimeInSeconds(int val){  playHeadPositionInfo.timeInSeconds=val;	}
			int getTimeInSeconds(){  return playHeadPositionInfo.timeInSeconds;	}
			
			void setPPQPosition(int val){  playHeadPositionInfo.ppqPosition=val;	}
			int getPPQPosition(){  return playHeadPositionInfo.ppqPosition;	}
			
		
		private:
			AudioPlayHead::CurrentPositionInfo playHeadPositionInfo;
	};

	void setIsPlaying(bool value, bool reset=false);
	void setBPM(int bpm);
	
	int getTimeInSeconds(){		return audioPlayHead.getTimeInSeconds();	}
	int getPPQPosition(){		return audioPlayHead.getPPQPosition();		}
	void setEditedNodeId(int id){	IdForNodeBeingEdited=id;		}
	int getEditedNodeId(){	return IdForNodeBeingEdited;			}

private:
    //==============================================================================
    AudioPluginFormatManager& formatManager;
    AudioProcessorGraph graph;
	AudioPlaybackHead audioPlayHead;
	int32 automationNodeID;
	
	OwnedArray<NodeAudioProcessorListener> audioProcessorListeners;
	int lastChangedNodeId;
	int lastChangedNodeParameter;
	int IdForNodeBeingEdited;
    uint32 lastUID;
    uint32 getNextUID() noexcept;
	uint32 lastNodeID;
	Array<String> pluginTypes;
	uint32 nodeId;
	float PPQN, currentBPM, playPosition;
	int ppqPosition, subTicks;
	int timeInSeconds; 
	
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FilterGraph)
};

//==============================================================================
// set one of these up for each node, this way we can tell which instance of a
// plugin is triggering the callback
//==============================================================================
class NodeAudioProcessorListener : public AudioProcessorListener,
										public ChangeBroadcaster
{

public:
	NodeAudioProcessorListener(int _nodeID):nodeId(_nodeID){}
    void audioProcessorChanged (AudioProcessor* processor){	   this->removeAllChangeListeners();	}
    void audioProcessorParameterChanged(AudioProcessor* processor, int parameterIndex, float newValue);

	int nodeId;
	int parameterIndex;
};



#endif   // __FILTERGRAPH_JUCEHEADER__
