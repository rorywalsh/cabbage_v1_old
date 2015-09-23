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

#ifndef __MAINHOSTWINDOW_JUCEHEADER__
#define __MAINHOSTWINDOW_JUCEHEADER__

#include "FilterGraph.h"
#include "GraphEditorPanel.h"
#include "../CabbageUtils.h"
#include "CabbagePluginListComponent.h"
#include "Preferences.h"

//==============================================================================
namespace CommandIDs
{
static const int open                   = 0x30000;
static const int save                   = 0x30001;
static const int saveAs                 = 0x30002;
static const int showPluginListEditor   = 0x30100;
static const int showAudioSettings      = 0x30200;
static const int showAboutBox           = 0x30303;
static const int preferences            = 0x30400;
static const int setCabbageFileDirectory= 0x30500;
static const int viewSidepanel          = 0x30600;
static const int midiLearn	            = 0x30700;
static const int viewBottomPanel        = 0x30800;
}

ApplicationCommandManager& getCommandManager();
ApplicationProperties& getAppProperties();

//==============================================================================
/**
*/
class MainHostWindow    : public DocumentWindow,
    public MenuBarModel,
    public ApplicationCommandTarget,
    public ChangeListener,
    public FileDragAndDropTarget,
    public DragAndDropContainer
{
public:
    //==============================================================================
    MainHostWindow();
    ~MainHostWindow();

    //==============================================================================
    void closeButtonPressed();
    void changeListenerCallback (ChangeBroadcaster*);

    bool isInterestedInFileDrag (const StringArray& files);
    void fileDragEnter (const StringArray& files, int, int);
    void fileDragMove (const StringArray& files, int, int);
    void fileDragExit (const StringArray& files);
    void filesDropped (const StringArray& files, int, int);

    StringArray getMenuBarNames();
    PopupMenu getMenuForIndex (int topLevelMenuIndex, const String& menuName);
    void menuItemSelected (int menuItemID, int topLevelMenuIndex);
    ApplicationCommandTarget* getNextCommandTarget();
    void getAllCommands (Array <CommandID>& commands);
    void getCommandInfo (CommandID commandID, ApplicationCommandInfo& result);
    bool perform (const InvocationInfo& info);
    bool tryToQuitApplication();

    void launchPreferencesDialogue();
    void createPlugin (const PluginDescription* desc, int x, int y);

    void addPluginsToMenu (PopupMenu& m) const;
    //add native Cabbage filters to list...nice.
    void addCabbagePluginsToMenu (PopupMenu& m, Array<File> &cabbageFiles) const;
    const PluginDescription* getChosenType (const int menuID) const;

    GraphDocumentComponent* getGraphDocument() const;


private:
    //==============================================================================
    AudioDeviceManager deviceManager;
    CabbageAudioDeviceSelectorComponent* audioSettingsComp;
    CabbagePluginListComponent* pluginList;
    const File deadMansPedalFile;
    TooltipWindow tooltipWindow;
    AudioPluginFormatManager formatManager;

    OwnedArray <PluginDescription> internalTypes;
    KnownPluginList knownPluginList;
    KnownPluginList::SortMethod pluginSortMethod;

    class PluginListWindow;
    ScopedPointer <PluginListWindow> pluginListWindow;

    void showAudioSettings();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainHostWindow)
};


#endif   // __MAINHOSTWINDOW_JUCEHEADER__
