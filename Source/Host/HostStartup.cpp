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
#include "../CabbageLookAndFeel.h"

#if ! (JUCE_PLUGINHOST_VST || JUCE_PLUGINHOST_VST3 || JUCE_PLUGINHOST_AU)
#error "If you're building the audio plugin host, you probably want to enable VST and/or AU support"
#endif

ApplicationProperties* appProperties = nullptr;
//==============================================================================
class PluginHostApp  : public JUCEApplication
{
public:
    PluginHostApp() {}

    void initialise (const String& commandLine) override
    {
        // initialise our settings file..

        PropertiesFile::Options options;
        options.applicationName     = "CabbageHost";
        options.filenameSuffix      = "settings";
        options.osxLibrarySubFolder = "Preferences";

        appProperties = new ApplicationProperties();
        appProperties->setStorageParameters (options);

        defaultPropSet = new PropertySet();
        ScopedPointer<XmlElement> xml;
        xml = new XmlElement("PLANTS");
        String homeDir = appProperties->getCommonSettings(true)->getFile().getParentDirectory().getFullPathName();
        String manualPath;
#if defined(LINUX) || defined(MACOSX)
        manualPath = File::getSpecialLocation(File::currentExecutableFile).getParentDirectory().getFullPathName()+"/CsoundDocs/index.html";
        
#else
        manualPath = "Library/Frameworks/CsoundLib64.framework/Versions/6.0/Resources/Manual/index.html";
#endif
        String documentsFolder = File::getSpecialLocation(File::userDocumentsDirectory).getFullPathName();
        defaultPropSet->setValue("CabbageFilePaths", documentsFolder);
        defaultPropSet->setValue("CsoundHelpDir", manualPath);
        defaultPropSet->setValue("PlantFileDir", homeDir+"/Plants");
        defaultPropSet->setValue("ExamplesDir", homeDir+"/Examples/Examples");
        defaultPropSet->setValue("DisablePluginInfo", 0);
        defaultPropSet->setValue("ShowEditorConsole", 0);
        defaultPropSet->setValue("ExternalEditor", 0);
        defaultPropSet->setValue("UseCabbageIO", 1);
        defaultPropSet->setValue("ShowConsoleWithEditor", 0);
        defaultPropSet->setValue("UsingCabbageCsound", 1);
        defaultPropSet->setValue("AudioEnabled", 1);
        defaultPropSet->setValue("DisableGUIEditModeWarning", 1);
        defaultPropSet->setValue("SetAlwaysOnTop", 1);
        defaultPropSet->setValue("PlantRepository", xml);
        defaultPropSet->setValue("EditorColourScheme", 1);
        defaultPropSet->setValue("showTabs", 1);
        defaultPropSet->setValue("EnablePopupDisplay", 1);
		defaultPropSet->setValue("ShowNativeFileDialogues", 1);
        defaultPropSet->setValue("EnableNativePopup", 0);
        defaultPropSet->setValue("windowX", 100);
        defaultPropSet->setValue("windowY", 100);
        appProperties->getUserSettings()->setFallbackPropertySet(defaultPropSet);


        LookAndFeel::setDefaultLookAndFeel (&lookAndFeel);

        mainWindow = new MainHostWindow();
        mainWindow->setUsingNativeTitleBar (false);

        commandManager.registerAllCommandsForTarget (this);
        commandManager.registerAllCommandsForTarget (mainWindow);

        mainWindow->menuItemsChanged();

        if (commandLine.isNotEmpty()
                && ! commandLine.trimStart().startsWith ("-")
                && mainWindow->getGraphDocument() != nullptr)
            mainWindow->getGraphDocument()->graph.loadFrom (File::getCurrentWorkingDirectory()
                    .getChildFile (commandLine), true);
    }

    void shutdown() override
    {
        mainWindow = nullptr;
        delete appProperties;;
        
        LookAndFeel::setDefaultLookAndFeel (nullptr);
    }

    void systemRequestedQuit() override
    {
        if (mainWindow != nullptr)
            mainWindow->tryToQuitApplication();
        else
            JUCEApplicationBase::quit();
    }

    const String getApplicationName() override       {
        return "Cabbage Patcher";
    }
    const String getApplicationVersion() override    {
        return ProjectInfo::versionString;
    }
    bool moreThanOneInstanceAllowed() override       {
        return true;
    }

    ApplicationCommandManager commandManager;
    //ScopedPointer<ApplicationProperties> appProperties;
    CabbageLookAndFeel lookAndFeel;
	ScopedPointer<PropertySet> defaultPropSet;

private:
    ScopedPointer<MainHostWindow> mainWindow;
};

static PluginHostApp& getApp()                      {
    return *dynamic_cast<PluginHostApp*>(JUCEApplication::getInstance());
}
ApplicationCommandManager& getCommandManager()      {
    return getApp().commandManager;
}
ApplicationProperties& getAppProperties()           {
    return *appProperties;
}


// This kicks the whole thing off..
START_JUCE_APPLICATION (PluginHostApp)
