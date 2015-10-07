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
#include "InternalFilters.h"
#include "../CabbageAudioDeviceSelectorComponent.h"


//==============================================================================
class MainHostWindow::PluginListWindow  : public DocumentWindow
{
public:
    PluginListWindow (MainHostWindow& owner_, AudioPluginFormatManager& formatManager)
        : DocumentWindow ("Available Plugins", Colours::black,
                          DocumentWindow::minimiseButton | DocumentWindow::closeButton),
        owner (owner_)
    {
        const File deadMansPedalFile (getAppProperties().getUserSettings()
                                      ->getFile().getSiblingFile ("RecentlyCrashedPluginsList"));

        setContentOwned (new CabbagePluginListComponent (formatManager,
                         owner.knownPluginList,
                         deadMansPedalFile,
                         getAppProperties().getUserSettings()), true);

        getContentComponent()->setColour(Label::textColourId, Colours::red);
        getContentComponent()->setColour(TextEditor::textColourId, Colours::red);
        getContentComponent()->lookAndFeelChanged();

        setResizable (true, false);
        setResizeLimits (300, 400, 800, 1500);
        setTopLeftPosition (60, 60);

        restoreWindowStateFromString (getAppProperties().getUserSettings()->getValue ("listWindowPos"));
        setVisible (true);
    }

    ~PluginListWindow()
    {
        getAppProperties().getUserSettings()->setValue ("listWindowPos", getWindowStateAsString());

        clearContentComponent();
    }

    void closeButtonPressed()
    {
        owner.pluginListWindow = nullptr;
    }

private:
    MainHostWindow& owner;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginListWindow)
};

//==============================================================================
MainHostWindow::MainHostWindow()
    : DocumentWindow (JUCEApplication::getInstance()->getApplicationName(), Colours::black,
                      DocumentWindow::allButtons),
    deadMansPedalFile (getAppProperties().getUserSettings()
                       ->getFile().getSiblingFile ("RecentlyCrashedPluginsList"))
{
    setColour(DocumentWindow::textColourId, Colours::whitesmoke);
    setColour(DocumentWindow::backgroundColourId, cUtils::getDarkerBackgroundSkin());

    formatManager.addDefaultFormats();
    formatManager.addFormat (new InternalPluginFormat());

    ScopedPointer<XmlElement> savedAudioState (getAppProperties().getUserSettings()
            ->getXmlValue ("audioDeviceState"));


    setResizable (true, false);
    setResizeLimits (500, 400, 10000, 10000);
    centreWithSize (800, 600);

    deviceManager.initialise(2, 2, savedAudioState, true);

    //deviceManager.initialiseWithDefaultDevices(8, 8);

    //if(!deviceManager.getCurrentAudioDevice())
    //	deviceManager.initialiseWithDefaultDevices(2, 2);

    setContentOwned (new GraphDocumentComponent (formatManager, &deviceManager), false);

    restoreWindowStateFromString (getAppProperties().getUserSettings()->getValue ("mainWindowPos"));

    setVisible (true);

    InternalPluginFormat internalFormat;
    internalFormat.getAllTypes (internalTypes);

    ScopedPointer<XmlElement> savedPluginList (getAppProperties().getUserSettings()->getXmlValue ("pluginList"));

    if (savedPluginList != nullptr)
        knownPluginList.recreateFromXml (*savedPluginList);

    pluginSortMethod = (KnownPluginList::SortMethod) getAppProperties().getUserSettings()
                       ->getIntValue ("pluginSortMethod", KnownPluginList::sortByManufacturer);

    knownPluginList.addChangeListener (this);
    addKeyListener (getCommandManager().getKeyMappings());
    Process::setPriority (Process::HighPriority);

#if JUCE_MAC
    setMacMainMenu (this);
#else
    setMenuBar (this, 24);
#endif

    getCommandManager().setFirstCommandTarget (this);

    String userDir = appProperties->getUserSettings()->getValue("CabbageFilePaths");
    if(!File(userDir).exists())
        cUtils::showMessage("There is no user directory set for Cabbage instruments. Please go to Preferences and select the directory/directories where you store your Cabbage files");

}

MainHostWindow::~MainHostWindow()
{

#if JUCE_MAC
    setMacMainMenu (nullptr);
#else
    setMenuBar (nullptr);
#endif

    knownPluginList.removeChangeListener (this);
    internalTypes.clear();

    getAppProperties().getUserSettings()->setValue ("mainWindowPos", getWindowStateAsString());
    clearContentComponent();
}

void MainHostWindow::closeButtonPressed()
{
    getGraphDocument()->removeAllComponentsFromBottomPanel();
    tryToQuitApplication();
}

bool MainHostWindow::tryToQuitApplication()
{
    PluginWindow::closeAllCurrentlyOpenWindows();

    if (getGraphDocument() == nullptr
            || getGraphDocument()->graph.saveIfNeededAndUserAgrees() == FileBasedDocument::savedOk)
    {
        JUCEApplication::quit();
        return true;
    }

    return false;
}

void MainHostWindow::changeListenerCallback (ChangeBroadcaster*)
{
    menuItemsChanged();

    // save the plugin list every time it gets chnaged, so that if we're scanning
    // and it crashes, we've still saved the previous ones
    ScopedPointer<XmlElement> savedPluginList (knownPluginList.createXml());

    if (savedPluginList != nullptr)
    {
        getAppProperties().getUserSettings()->setValue ("pluginList", savedPluginList);
        getAppProperties().saveIfNeeded();
    }
}

StringArray MainHostWindow::getMenuBarNames()
{
    const char* const names[] = { "File", "View", "Options", nullptr };

    return StringArray (names);
}

PopupMenu MainHostWindow::getMenuForIndex (int topLevelMenuIndex, const String& /*menuName*/)
{
    PopupMenu menu;

    if (topLevelMenuIndex == 0)
    {
        // "File" menu
        menu.addCommandItem (&getCommandManager(), CommandIDs::open);

        RecentlyOpenedFilesList recentFiles;
        recentFiles.restoreFromString (getAppProperties().getUserSettings()
                                       ->getValue ("recentFilterGraphFiles"));

        PopupMenu recentFilesMenu;
        recentFiles.createPopupMenuItems (recentFilesMenu, 100, true, true);
        menu.addSubMenu ("Open recent file", recentFilesMenu);

        menu.addCommandItem (&getCommandManager(), CommandIDs::save);
        menu.addCommandItem (&getCommandManager(), CommandIDs::saveAs);
        menu.addSeparator();
        menu.addCommandItem (&getCommandManager(), StandardApplicationCommandIDs::quit);
    }
    else if (topLevelMenuIndex == 1)
    {
        menu.addCommandItem (&getCommandManager(), CommandIDs::viewSidepanel);
        menu.addCommandItem (&getCommandManager(), CommandIDs::viewBottomPanel);
        menu.addCommandItem (&getCommandManager(), CommandIDs::midiMappings);
    }
    else if (topLevelMenuIndex == 2)
    {
        // "Options" menu
        menu.addCommandItem (&getCommandManager(), CommandIDs::preferences);
        menu.addCommandItem (&getCommandManager(), CommandIDs::midiLearn);
        menu.addSeparator();
        menu.addCommandItem (&getCommandManager(), CommandIDs::showAboutBox);
    }

    return menu;
}

void MainHostWindow::menuItemSelected (int menuItemID, int /*topLevelMenuIndex*/)
{
    GraphDocumentComponent* const graphEditor = getGraphDocument();

    if (menuItemID == 250)
    {
        //graphEditor->showSidebarPanel(!graphEditor->isSidebarPanelShowing());
    }
    else if (menuItemID >= 100 && menuItemID < 200)
    {
        RecentlyOpenedFilesList recentFiles;
        recentFiles.restoreFromString (getAppProperties().getUserSettings()
                                       ->getValue ("recentFilterGraphFiles"));

        if (graphEditor != nullptr && graphEditor->graph.saveIfNeededAndUserAgrees() == FileBasedDocument::savedOk)
        {
            graphEditor->graph.loadFrom (recentFiles.getFile (menuItemID - 100), true);
            graphEditor->refreshPluginsInSidebarPanel();
            graphEditor->removeAllComponentsFromBottomPanel();
        }
    }
    else if (menuItemID >= 200 && menuItemID < 210)
    {
        if (menuItemID == 200)     pluginSortMethod = KnownPluginList::defaultOrder;
        else if (menuItemID == 201)     pluginSortMethod = KnownPluginList::sortAlphabetically;
        else if (menuItemID == 202)     pluginSortMethod = KnownPluginList::sortByCategory;
        else if (menuItemID == 203)     pluginSortMethod = KnownPluginList::sortByManufacturer;
        else if (menuItemID == 204)     pluginSortMethod = KnownPluginList::sortByFileSystemLocation;

        getAppProperties().getUserSettings()->setValue ("pluginSortMethod", (int) pluginSortMethod);

        menuItemsChanged();
    }
    else
    {
        createPlugin (getChosenType (menuItemID),
                      proportionOfWidth  (0.3f + Random::getSystemRandom().nextFloat() * 0.6f),
                      proportionOfHeight (0.3f + Random::getSystemRandom().nextFloat() * 0.6f));
    }
}

void MainHostWindow::createPlugin (const PluginDescription* desc, int x, int y)
{
    GraphDocumentComponent* const graphEditor = getGraphDocument();

    if (graphEditor != nullptr)
        graphEditor->createNewPlugin (desc, x, y, false, "");
}

void MainHostWindow::addPluginsToMenu (PopupMenu& m) const
{
    PopupMenu menu;
    knownPluginList.addToMenu (menu, pluginSortMethod);
    m.addSubMenu("Third Party", menu);

    menu.clear();
    m.addSeparator();
    for (int i = 0; i < internalTypes.size(); ++i)
        menu.addItem (i + 9000, internalTypes.getUnchecked(i)->name);

    menu.addItem(10000, "Soundfile player");

    GraphDocumentComponent* const graphEditor = getGraphDocument();
    if(graphEditor)
    {
        //if(!graphEditor->graph.isAutomationAdded())
        //    menu.addItem(10001, "Automation track");
    }
    m.addSubMenu("Devices", menu);
}

//add native filters to list of plugins.
void MainHostWindow::addCabbagePluginsToMenu (PopupMenu& m, Array<File> &cabbageFiles) const
{
    int menuSize = m.getNumItems();

    PopupMenu menu;
    PopupMenu subMenu;
    int fileCnt=0;


    String examplesDir;
    //cUtils::debug("Example Directory:"+dir);
    if(!File(examplesDir).exists())
    {
#if defined(LINUX) || defined(MACOSX)
        examplesDir = File::getSpecialLocation(File::currentExecutableFile).getParentDirectory().getFullPathName()+"/Examples";
#else
        examplesDir = File::getSpecialLocation(File::currentExecutableFile).getParentDirectory().getFullPathName()+"\\Examples";
#endif
    }


    String userDir = getAppProperties().getUserSettings()->getValue("CabbageFilePaths");

    if(File(userDir).exists())
    {
        FileSearchPath filePaths(appProperties->getUserSettings()->getValue("CabbageFilePaths"));
        //cUtils::showMessage(appProperties->getUserSettings()->getValue("CabbageFilePaths"))

        //add all files in root of specifed directories
        for(int i=0; i<filePaths.getNumPaths(); i++)
        {
            File pluginDir(filePaths[i]);
            pluginDir.findChildFiles(cabbageFiles, File::findFiles, false, "*.csd");

        }

        for (int i = 0; i < cabbageFiles.size(); ++i)
            menu.addItem (i+1, cabbageFiles[i].getFileNameWithoutExtension());


        //fileCnt = cabbageFiles.size();

        //increment menu size and serach recursively through all subfolders in specified dirs
        for(int i=0; i<filePaths.getNumPaths(); i++)
        {
            Array<File> subFolders;
            File searchDir(filePaths[i]);
            subFolders.add(searchDir);
            searchDir.findChildFiles(subFolders, File::findDirectories, true);

            //remove parent dirs from array
            for(int p=0; p<filePaths.getNumPaths(); p++)
                subFolders.removeAllInstancesOf(filePaths[p]);

            PopupMenu subMenu;
            for (int subs = 0; subs < subFolders.size(); subs++)
            {
                cUtils::debug(subFolders[subs].getFullPathName());
                fileCnt = cabbageFiles.size();
                subFolders[subs].findChildFiles(cabbageFiles, File::findFiles, false, "*.csd");
                subMenu.clear();

                for (int fileIndex=fileCnt+1; fileIndex < cabbageFiles.size(); fileIndex++)
                    subMenu.addItem (fileIndex+1, cabbageFiles[fileIndex].getFileNameWithoutExtension());


                menu.addSubMenu(subFolders[subs].getFileNameWithoutExtension(), subMenu);
            }

            subMenu.clear();
        }


        menu.addSeparator();
        menu.setLookAndFeel(&this->getLookAndFeel());
        m.addSubMenu("User", menu);
    }

    int nonExamplesSize = cabbageFiles.size();

    menu.clear();
    FileSearchPath exampleFilePaths(examplesDir);
    //add all files in root of specifed directories
    for(int i=0; i<exampleFilePaths.getNumPaths(); i++)
    {
        File pluginDir(exampleFilePaths[i]);
        pluginDir.findChildFiles(cabbageFiles, File::findFiles, false, "*.csd");

    }

    for (int i = nonExamplesSize; i < cabbageFiles.size(); ++i)
        menu.addItem (i+1, cabbageFiles[i].getFileNameWithoutExtension());


    //fileCnt = cabbageFiles.size();

    //increment menu size and serach recursively through all subfolders in specified dirs
    for(int i=0; i<exampleFilePaths.getNumPaths(); i++)
    {
        Array<File> subFolders;
        File searchDir(exampleFilePaths[i]);
        subFolders.add(searchDir);
        searchDir.findChildFiles(subFolders, File::findDirectories, true);

        //remove parent dirs from array
        for(int p=0; p<exampleFilePaths.getNumPaths(); p++)
            subFolders.removeAllInstancesOf(exampleFilePaths[p]);

        PopupMenu subMenu;
        for (int subs = 0; subs < subFolders.size(); subs++)
        {
            cUtils::debug(subFolders[subs].getFullPathName());
            fileCnt = cabbageFiles.size();
            subFolders[subs].findChildFiles(cabbageFiles, File::findFiles, false, "*.csd");
            subMenu.clear();

            for (int fileIndex=fileCnt+1; fileIndex < cabbageFiles.size(); fileIndex++)
                subMenu.addItem (fileIndex+1, cabbageFiles[fileIndex].getFileNameWithoutExtension());


            menu.addSubMenu(subFolders[subs].getFileNameWithoutExtension(), subMenu);
        }

        subMenu.clear();
    }

    m.addSubMenu("Native", menu);
}

const PluginDescription* MainHostWindow::getChosenType (const int menuID) const
{
    if (menuID >= 9000 && menuID < 9000 + internalTypes.size())
        return internalTypes [menuID - 9000];

    return knownPluginList.getType (knownPluginList.getIndexChosenByMenu (menuID));
}

//==============================================================================
ApplicationCommandTarget* MainHostWindow::getNextCommandTarget()
{
    return findFirstTargetParentComponent();
}

void MainHostWindow::getAllCommands (Array <CommandID>& commands)
{
    // this returns the set of all commands that this target can perform..
    const CommandID ids[] = { CommandIDs::open,
                              CommandIDs::save,
                              CommandIDs::saveAs,
                              CommandIDs::showPluginListEditor,
                              CommandIDs::showAudioSettings,
                              CommandIDs::showAboutBox,
                              CommandIDs::preferences,
                              CommandIDs::viewSidepanel,
                              CommandIDs::viewBottomPanel,
                              CommandIDs::midiLearn,
                              CommandIDs::midiMappings,
                              CommandIDs::setCabbageFileDirectory
                            };

    commands.addArray (ids, numElementsInArray (ids));
}

void MainHostWindow::getCommandInfo (const CommandID commandID, ApplicationCommandInfo& result)
{
    const String category ("General");

    switch (commandID)
    {
    case CommandIDs::open:
        result.setInfo ("Open...",
                        "Opens a filter graph file",
                        category, 0);
        result.defaultKeypresses.add (KeyPress ('o', ModifierKeys::commandModifier, 0));
        break;

    case CommandIDs::save:
        result.setInfo ("Save",
                        "Saves the current graph to a file",
                        category, 0);
        result.defaultKeypresses.add (KeyPress ('s', ModifierKeys::commandModifier, 0));
        break;

    case CommandIDs::saveAs:
        result.setInfo ("Save As...",
                        "Saves a copy of the current graph to a file",
                        category, 0);
        result.defaultKeypresses.add (KeyPress ('s', ModifierKeys::shiftModifier | ModifierKeys::commandModifier, 0));
        break;

    case CommandIDs::showPluginListEditor:
        result.setInfo ("Edit the list of available plug-Ins...", String::empty, category, 0);
        result.addDefaultKeypress ('p', ModifierKeys::commandModifier);
        break;

    case CommandIDs::showAudioSettings:
        result.setInfo ("Change the audio device settings", String::empty, category, 0);
        result.addDefaultKeypress ('a', ModifierKeys::commandModifier);
        break;

    case CommandIDs::setCabbageFileDirectory:
        result.setInfo ("Set the Cabbage plugin directory", String::empty, category, 0);
        break;

    case CommandIDs::showAboutBox:
        result.setInfo ("About...", String::empty, category, 0);
        break;

    case CommandIDs::preferences:
        result.setInfo ("Preferences", String::empty, category, 0);
        break;

    case CommandIDs::viewSidepanel:
        result.setInfo ("View Sidepanel", String::empty, category, 0);
        result.defaultKeypresses.add (KeyPress('l', ModifierKeys::commandModifier, 0));
        break;

    case CommandIDs::viewBottomPanel:
        result.setInfo ("View Bottom Panel", String::empty, category, 0);
        result.defaultKeypresses.add (KeyPress('b', ModifierKeys::commandModifier, 0));
        break;

    case CommandIDs::midiLearn:
        result.setInfo ("MIDI Learn", String::empty, category, 0);
        result.defaultKeypresses.add (KeyPress('m', ModifierKeys::commandModifier, 0));
        break;

    case CommandIDs::midiMappings:
        result.setInfo ("MIDI Mappings", String::empty, category, 0);
        break;

    default:
        break;
    }
}

bool MainHostWindow::perform (const InvocationInfo& info)
{

    GraphDocumentComponent* const graphEditor = getGraphDocument();
    String credits;
    String dir = appProperties->getUserSettings()->getValue("CabbagePluginDirectory", "");
    FileChooser browser(String("Please select the Cabbage Plugin directoy..."), File(dir), String("*.csd"));

    switch (info.commandID)
    {
    case CommandIDs::open:
        if (graphEditor != nullptr && graphEditor->graph.saveIfNeededAndUserAgrees() == FileBasedDocument::savedOk)
        {
            graphEditor->graph.loadFromUserSpecifiedFile (true);
            graphEditor->removeAllComponentsFromBottomPanel();
            graphEditor->refreshPluginsInSidebarPanel();

        }
        break;

    case CommandIDs::save:
        if (graphEditor != nullptr)
            graphEditor->graph.save (true, true);
        break;

    case CommandIDs::saveAs:
        if (graphEditor != nullptr)
            graphEditor->graph.saveAs (File::nonexistent, true, true, true);
        break;

    case CommandIDs::showPluginListEditor:
        if (pluginListWindow == nullptr)
            pluginListWindow = new PluginListWindow (*this, formatManager);

        pluginListWindow->toFront (true);
        break;

    case CommandIDs::showAudioSettings:
        showAudioSettings();
        break;

    case CommandIDs::setCabbageFileDirectory:
        if(browser.browseForDirectory())
            appProperties->getUserSettings()->setValue("CabbagePluginDirectory", browser.getResult().getFullPathName());
        break;

    case CommandIDs::showAboutBox:
        cUtils::showMessage("About", "Cabbage Studio v0.1.2 Beta");
        break;

    case CommandIDs::viewSidepanel:
        graphEditor->showSidebarPanel(!graphEditor->isSidebarPanelShowing());
        break;

    case CommandIDs::viewBottomPanel:
        graphEditor->showBottomPanel(!graphEditor->isBottomPanelShowing());
        break;

    case CommandIDs::preferences:
        launchPreferencesDialogue();
        break;

    case CommandIDs::midiLearn:
        graphEditor->toggleMIDILearn();
        break;

    case CommandIDs::midiMappings:
        showMidiMappings();
        break;

    default:
        return false;
    }

    return true;
}

void MainHostWindow::showMidiMappings()
{
    StringArray midiMap;
    GraphDocumentComponent* const graphEditor = getGraphDocument();

    graphEditor->showMidiMappings();
}

void MainHostWindow::launchPreferencesDialogue()
{
    CabbageAudioDeviceSelectorComponent* audioSettingsComp = new CabbageAudioDeviceSelectorComponent(deviceManager,
            0, 32,	0, 32, true, true, true, false);
    audioSettingsComp->setSize (500, 450);
    CabbagePluginListComponent* pluginList = new CabbagePluginListComponent(formatManager,
            knownPluginList,
            deadMansPedalFile,
            getAppProperties().getUserSettings());
    ScopedPointer<CabbagePreferences> prefWindow;
    prefWindow = new CabbagePreferences();
    prefWindow->addComponent("pluginList", pluginList);
    prefWindow->addComponent("audioSelector", audioSettingsComp);

    DialogWindow::showModalDialog("Preferences", prefWindow, this, Colours::black, true);

    ScopedPointer<XmlElement> audioState (deviceManager.createStateXml());
    getAppProperties().getUserSettings()->setValue ("audioDeviceState", audioState);
    getAppProperties().saveIfNeeded();

}


void MainHostWindow::showAudioSettings()
{
    CabbageAudioDeviceSelectorComponent audioSettingsComp (deviceManager,
            0, 32,
            0, 32,
            true, true, true, false);

    audioSettingsComp.setSize (500, 450);

    DialogWindow::LaunchOptions o;
    o.content.setNonOwned(&audioSettingsComp);
    o.dialogTitle                   = "Audio Settings";
    o.componentToCentreAround       = this;
    o.dialogBackgroundColour        = Colour(24, 24, 24);
    o.escapeKeyTriggersCloseButton  = true;
    o.useNativeTitleBar             = false;
    o.resizable                     = false;

    o.runModal();

    ScopedPointer<XmlElement> audioState (deviceManager.createStateXml());

    getAppProperties().getUserSettings()->setValue ("audioDeviceState", audioState);
    getAppProperties().getUserSettings()->saveIfNeeded();

    GraphDocumentComponent* const graphEditor = getGraphDocument();

    if (graphEditor != nullptr)
        graphEditor->graph.removeIllegalConnections();
}

bool MainHostWindow::isInterestedInFileDrag (const StringArray&)
{
    return true;
}

void MainHostWindow::fileDragEnter (const StringArray&, int, int)
{

}

void MainHostWindow::fileDragMove (const StringArray&, int, int)
{

}

void MainHostWindow::fileDragExit (const StringArray&)
{

}

void MainHostWindow::filesDropped (const StringArray& files, int x, int y)
{
    GraphDocumentComponent* const graphEditor = getGraphDocument();

    if (graphEditor != nullptr)
    {
        if (files.size() == 1 && File (files[0]).hasFileExtension (filenameSuffix))
        {
            if (graphEditor->graph.saveIfNeededAndUserAgrees() == FileBasedDocument::savedOk)
                graphEditor->graph.loadFrom (File (files[0]), true);
        }
        else
        {
            OwnedArray <PluginDescription> typesFound;
            knownPluginList.scanAndAddDragAndDroppedFiles (formatManager, files, typesFound);

            Point<int> pos (graphEditor->getLocalPoint (this, Point<int> (x, y)));

            for (int i = 0; i < jmin (5, typesFound.size()); ++i)
                createPlugin (typesFound.getUnchecked(i), pos.getX(), pos.getY());
        }
    }
}

GraphDocumentComponent* MainHostWindow::getGraphDocument() const
{
    return dynamic_cast <GraphDocumentComponent*> (getContentComponent());
}
