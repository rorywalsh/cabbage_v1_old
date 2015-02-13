/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2013 - Raw Material Software Ltd.

   Permission is granted to use this software under the terms of either:
   a) the GPL v2 (or any later version)
   b) the Affero GPL v3

   Details of these licenses can be found at: www.gnu.org/licenses

   JUCE is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

   ------------------------------------------------------------------------------

   To release a closed-source product which uses JUCE, commercial licenses are
   available: visit www.juce.com for more information.

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "MainHostWindow.h"
#include "FilterGraph.h"
#include "InternalFilters.h"
#include "GraphEditorPanel.h"
#include "PluginWrapperProcessor.h"
#include "../Plugin/CabbagePluginProcessor.h"


//==============================================================================
const int FilterGraph::midiChannelNumber = 0x1000;

FilterGraph::FilterGraph (AudioPluginFormatManager& formatManager_)
    : FileBasedDocument (filenameSuffix,
                         filenameWildcard,
                         "Load a filter graph",
                         "Save a filter graph"),
    formatManager (formatManager_), lastUID (0)
{
    setChangedFlag (false);
}

FilterGraph::~FilterGraph()
{
    graph.clear();
}

uint32 FilterGraph::getNextUID() noexcept
{
    return ++lastUID;
}

//==============================================================================
int FilterGraph::getNumFilters() const noexcept
{
    return graph.getNumNodes();
}

const AudioProcessorGraph::Node::Ptr FilterGraph::getNode (const int index) const noexcept
{
    return graph.getNode (index);
}

const AudioProcessorGraph::Node::Ptr FilterGraph::getNodeForId (const uint32 uid) const noexcept
{
    return graph.getNodeForId (uid);
}

void FilterGraph::addFilter (const PluginDescription* desc, double x, double y)
{
    if (desc != nullptr)
    {
        AudioProcessorGraph::Node* node = nullptr;

        String errorMessage;
		if(desc->pluginFormatName!="Cabbage" && desc->pluginFormatName!="Internal")
		{
			if(PluginWrapper* instance = new PluginWrapper(formatManager.createPluginInstance (*desc, graph.getSampleRate(), graph.getBlockSize(), errorMessage)))
			{
				instance->setPlayConfigDetails( desc->numInputChannels,
												desc->numOutputChannels,
												graph.getSampleRate(),
												graph.getBlockSize());
				instance->setPluginName(desc->name);
				//cUtils::debug("num params", instance->getNumParameters());
				node = graph.addNode (instance);
				node->properties.set("pluginType", "ThirdParty");
				node->properties.set("pluginName", desc->name);
			}
		}
		else if(desc->pluginFormatName=="Internal")
		{
			if (AudioPluginInstance* instance = formatManager.createPluginInstance (*desc, graph.getSampleRate(), graph.getBlockSize(), errorMessage))
			{
				node = graph.addNode (instance);
				node->properties.set("pluginType", "Internal");
				node->properties.set("pluginName", desc->name);
			}
		}
	
		else if(desc->pluginFormatName=="Cabbage")
		{
			CabbagePluginAudioProcessor* cabbageNativePlugin = new CabbagePluginAudioProcessor(desc->fileOrIdentifier, false, AUDIO_PLUGIN);
			int numChannels = cabbageNativePlugin->getNumberCsoundOutChannels();
			//create GUI for selected plugin...
			cabbageNativePlugin->createGUI(File(desc->fileOrIdentifier).loadFileAsString(), true);			
			cabbageNativePlugin->setPlayConfigDetails(cabbageNativePlugin->getNumberCsoundOutChannels(), 
													  cabbageNativePlugin->getNumberCsoundOutChannels(), 
													  cabbageNativePlugin->getCsoundSamplingRate(),
													  cabbageNativePlugin->getCsoundKsmpsSize());
            node = graph.addNode (cabbageNativePlugin);	
			node->properties.set("pluginName", cabbageNativePlugin->getPluginName());
			//native Cabbage plugins don't have plugin descriptors, so we create one here..
			ScopedPointer<XmlElement> xmlElem;
			xmlElem = desc->createXml();
			String xmlText = xmlElem->createDocument("");
			node->properties.set("pluginType", "Cabbage");
			node->properties.set("pluginDesc", xmlText);
		}
		
        if (node != nullptr)
        {
            node->properties.set ("x", x);
            node->properties.set ("y", y);
			lastNodeID = node->nodeId;
			//create node listener with unique nodeID;
			audioProcessorListeners.add(new NodeAudioProcessorListener(node->nodeId));
			audioProcessorListeners[audioProcessorListeners.size()-1]->addChangeListener(this);
			node->getProcessor()->addListener(audioProcessorListeners[audioProcessorListeners.size()-1]);
            changed();
        }
        else
        {
            AlertWindow::showMessageBox (AlertWindow::WarningIcon,
                                         TRANS("Couldn't create filter"),
                                         errorMessage);
        }
    }
}

void FilterGraph::removeFilter (const uint32 id)
{
    PluginWindow::closeCurrentlyOpenWindowsFor (id);

    if (graph.removeNode (id))
        changed();
}

void FilterGraph::disconnectFilter (const uint32 id)
{
    if (graph.disconnectNode (id))
        changed();
}

void FilterGraph::removeIllegalConnections()
{
    if (graph.removeIllegalConnections())
        changed();
}

void FilterGraph::setNodePosition (const int nodeId, double x, double y)
{
    const AudioProcessorGraph::Node::Ptr n (graph.getNodeForId (nodeId));

    if (n != nullptr)
    {
        n->properties.set ("x", jlimit (0.0, 1.0, x));
        n->properties.set ("y", jlimit (0.0, 1.0, y));
    }
}

void FilterGraph::getNodePosition (const int nodeId, double& x, double& y) const
{
    x = y = 0;

    const AudioProcessorGraph::Node::Ptr n (graph.getNodeForId (nodeId));

    if (n != nullptr)
    {
        x = (double) n->properties ["x"];
        y = (double) n->properties ["y"];
    }
}

//==============================================================================
int FilterGraph::getNumConnections() const noexcept
{
    return graph.getNumConnections();
}

const AudioProcessorGraph::Connection* FilterGraph::getConnection (const int index) const noexcept
{
    return graph.getConnection (index);
}

const AudioProcessorGraph::Connection* FilterGraph::getConnectionBetween (uint32 sourceFilterUID, int sourceFilterChannel,
        uint32 destFilterUID, int destFilterChannel) const noexcept
{
    return graph.getConnectionBetween (sourceFilterUID, sourceFilterChannel,
                                       destFilterUID, destFilterChannel);
}

bool FilterGraph::canConnect (uint32 sourceFilterUID, int sourceFilterChannel,
                              uint32 destFilterUID, int destFilterChannel) const noexcept
{
    return graph.canConnect (sourceFilterUID, sourceFilterChannel,
                             destFilterUID, destFilterChannel);
}

bool FilterGraph::addConnection (uint32 sourceFilterUID, int sourceFilterChannel,
                                 uint32 destFilterUID, int destFilterChannel)
{
    const bool result = graph.addConnection (sourceFilterUID, sourceFilterChannel,
                        destFilterUID, destFilterChannel);

    if (result)
        changed();

    return result;
}

void FilterGraph::removeConnection (const int index)
{
    graph.removeConnection (index);
    changed();
}

void FilterGraph::removeConnection (uint32 sourceFilterUID, int sourceFilterChannel,
                                    uint32 destFilterUID, int destFilterChannel)
{
    if (graph.removeConnection (sourceFilterUID, sourceFilterChannel,
                                destFilterUID, destFilterChannel))
        changed();
}

void FilterGraph::clear()
{
    PluginWindow::closeAllCurrentlyOpenWindows();

    graph.clear();
    changed();
}

//==============================================================================
String FilterGraph::getDocumentTitle()
{
    if (! getFile().exists())
        return "Unnamed";

    return getFile().getFileNameWithoutExtension();
}

Result FilterGraph::loadDocument (const File& file)
{
	graph.clear();
    XmlDocument doc (file);
    ScopedPointer<XmlElement> xml (doc.getDocumentElement());

    if (xml == nullptr || ! xml->hasTagName ("FILTERGRAPH"))
        return Result::fail ("Not a valid filter graph file");

    restoreFromXml (*xml);
    return Result::ok();
}

Result FilterGraph::saveDocument (const File& file)
{
    ScopedPointer<XmlElement> xml (createXml());

    if (! xml->writeToFile (file, String::empty))
        return Result::fail ("Couldn't write to the file");

    return Result::ok();
}

File FilterGraph::getLastDocumentOpened()
{
    RecentlyOpenedFilesList recentFiles;
    recentFiles.restoreFromString (getAppProperties().getUserSettings()
                                   ->getValue ("recentFilterGraphFiles"));

    return recentFiles.getFile (0);
}

void FilterGraph::setLastDocumentOpened (const File& file)
{
    RecentlyOpenedFilesList recentFiles;
    recentFiles.restoreFromString (getAppProperties().getUserSettings()
                                   ->getValue ("recentFilterGraphFiles"));

    recentFiles.addFile (file);

    getAppProperties().getUserSettings()
    ->setValue ("recentFilterGraphFiles", recentFiles.toString());
}

//==============================================================================
static XmlElement* createNodeXml (AudioProcessorGraph::Node* const node) noexcept
{
    PluginDescription pd;

    if(AudioPluginInstance* plugin = dynamic_cast <AudioPluginInstance*> (node->getProcessor()))
		plugin->fillInPluginDescription (pd);
	else if(PluginWrapper* plugin = dynamic_cast <PluginWrapper*> (node->getProcessor()))
		plugin->fillInPluginDescription (pd);
	else if(dynamic_cast <CabbagePluginAudioProcessor*> (node->getProcessor()))
		{
		//grab description of native plugin for saving...
		String xmlPluginDescriptor = node->properties.getWithDefault("pluginDesc", "").toString();
		cUtils::debug(xmlPluginDescriptor);
		XmlElement* xmlElem;
		xmlElem = XmlDocument::parse(xmlPluginDescriptor);
		pd.loadFromXml(*xmlElem);
		}
		
    XmlElement* e = new XmlElement ("FILTER");
    e->setAttribute ("uid", (int) node->nodeId);
    e->setAttribute ("x", node->properties ["x"].toString());
    e->setAttribute ("y", node->properties ["y"].toString());
    e->setAttribute ("uiLastX", node->properties ["uiLastX"].toString());
    e->setAttribute ("uiLastY", node->properties ["uiLastY"].toString());
    e->addChildElement (pd.createXml());

    XmlElement* state = new XmlElement ("STATE");

    MemoryBlock m;
    node->getProcessor()->getStateInformation (m);
    state->addTextElement (m.toBase64Encoding());
    e->addChildElement (state);

    return e;
}

void FilterGraph::createNodeFromXml (const XmlElement& xml)
{
    PluginDescription desc;

    forEachXmlChildElement (xml, e)
    {
        if (desc.loadFromXml (*e))
            break;
    }


	AudioProcessorGraph::Node::Ptr node = nullptr;
	
	String errorMessage;
	if(desc.pluginFormatName!="Cabbage" && desc.pluginFormatName!="Internal")
	{
		if(PluginWrapper* instance = new PluginWrapper(formatManager.createPluginInstance (desc, graph.getSampleRate(), graph.getBlockSize(), errorMessage)))
		{
			instance->setPlayConfigDetails( desc.numInputChannels,
											desc.numOutputChannels,
											graph.getSampleRate(),
											graph.getBlockSize());
			instance->setPluginName(desc.name);
			node = graph.addNode (instance, xml.getIntAttribute ("uid"));
			
			node->properties.set("pluginName", desc.name);
		}
	}
	else if(desc.pluginFormatName=="Internal")
	{
		if (AudioPluginInstance* instance = formatManager.createPluginInstance (desc, graph.getSampleRate(), graph.getBlockSize(), errorMessage))
		{
			node = graph.addNode (instance, xml.getIntAttribute ("uid"));
			node->properties.set("pluginType", "Internal");
			node->properties.set("pluginName", desc.name);
		}
	}

	else if(desc.pluginFormatName=="Cabbage")
	{
		CabbagePluginAudioProcessor* cabbageNativePlugin = new CabbagePluginAudioProcessor(desc.fileOrIdentifier, false, AUDIO_PLUGIN);
		
		//create GUI for selected plugin...
		cabbageNativePlugin->createGUI(File(desc.fileOrIdentifier).loadFileAsString(), true);			
		cabbageNativePlugin->setPlayConfigDetails(cabbageNativePlugin->getNumberCsoundOutChannels(), 
												  cabbageNativePlugin->getNumberCsoundOutChannels(), 
												  cabbageNativePlugin->getCsoundSamplingRate(),
												  cabbageNativePlugin->getCsoundKsmpsSize());
		node = graph.addNode (cabbageNativePlugin, xml.getIntAttribute ("uid"));	
		node->properties.set("pluginName", cabbageNativePlugin->getPluginName());
		//native Cabbage plugins don't have plugin descriptors, so we create one here..
		ScopedPointer<XmlElement> xmlElem;
		xmlElem = desc.createXml();
		String xmlText = xmlElem->createDocument("");
		node->properties.set("pluginDesc", xmlText);
		node->properties.set("pluginType", "Cabbage");
	}	
	

    //AudioProcessorGraph::Node::Ptr node (graph.addNode (instance, xml.getIntAttribute ("uid")));

    if (const XmlElement* const state = xml.getChildByName ("STATE"))
    {
        MemoryBlock m;
        m.fromBase64Encoding (state->getAllSubText());

        node->getProcessor()->setStateInformation (m.getData(), (int) m.getSize());
    }

    node->properties.set ("x", xml.getDoubleAttribute ("x"));
    node->properties.set ("y", xml.getDoubleAttribute ("y"));
    node->properties.set ("uiLastX", xml.getIntAttribute ("uiLastX"));
    node->properties.set ("uiLastY", xml.getIntAttribute ("uiLastY"));
	node->properties.set("pluginName", desc.name);

}

XmlElement* FilterGraph::createXml() const
{
    XmlElement* xml = new XmlElement ("FILTERGRAPH");
	cUtils::debug("graph.getNumNodes()", graph.getNumNodes());

    for (int i = 0; i < graph.getNumNodes(); ++i)
        xml->addChildElement (createNodeXml (graph.getNode (i)));

    for (int i = 0; i < graph.getNumConnections(); ++i)
    {
        const AudioProcessorGraph::Connection* const fc = graph.getConnection(i);

        XmlElement* e = new XmlElement ("CONNECTION");

        e->setAttribute ("srcFilter", (int) fc->sourceNodeId);
        e->setAttribute ("srcChannel", fc->sourceChannelIndex);
        e->setAttribute ("dstFilter", (int) fc->destNodeId);
        e->setAttribute ("dstChannel", fc->destChannelIndex);

        xml->addChildElement (e);
    }

    return xml;
}

void FilterGraph::restoreFromXml (const XmlElement& xml)
{
    clear();

    forEachXmlChildElementWithTagName (xml, e, "FILTER")
    {
        createNodeFromXml (*e);
        changed();
    }

    forEachXmlChildElementWithTagName (xml, e, "CONNECTION")
    {
		cUtils::debug("srcFilter", e->getIntAttribute ("srcFilter"));
        addConnection ((uint32) e->getIntAttribute ("srcFilter"),
                       e->getIntAttribute ("srcChannel"),
                       (uint32) e->getIntAttribute ("dstFilter"),
                       e->getIntAttribute ("dstChannel"));
    }
	graph.removeIllegalConnections();
}

void FilterGraph::changeListenerCallback(ChangeBroadcaster* source)
{
	 lastChangedNodeId = ((NodeAudioProcessorListener*)source)->nodeId;
	 lastChangedNodeParameter = ((NodeAudioProcessorListener*)source)->parameterIndex;
}

//==========================================================================
// parameter callback for node, used to map midi messages to parameters
//==========================================================================
void NodeAudioProcessorListener::audioProcessorParameterChanged(AudioProcessor* processor, int parameterIndex, float newValue)
{
	sendChangeMessage();
}

