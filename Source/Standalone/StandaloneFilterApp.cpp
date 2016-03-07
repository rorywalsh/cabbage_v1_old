#include "CabbageStandaloneDialog.h"
#include "../CabbageGUIClass.h"
#include "../CabbageUtils.h"
#include "../CabbageLookAndFeel.h"



ApplicationProperties* appProperties = nullptr;
PropertySet* defaultPropSet = nullptr;
String currentApplicationDirectory;

StringArray undoHistory;




class CabbageStandalone : public JUCEApplication
{
public:
    CabbageStandalone()
    {
    }

    void initialise(const String& commandLineParameters)
    {
        File thisFile(File::getSpecialLocation(File::currentExecutableFile));
        currentApplicationDirectory =thisFile.getParentDirectory().getFullPathName();
        //CabbageUtils::showMessage(commandLineParameters);
        PropertiesFile::Options options;
        options.applicationName     = "Cabbage";
        options.filenameSuffix      = "settings";
        options.osxLibrarySubFolder = "Preferences";

        appProperties = new ApplicationProperties();
        //set fallback file for default properties...

        appProperties->setStorageParameters (options);

        defaultPropSet = new PropertySet();
        ScopedPointer<XmlElement> xml;
        xml = new XmlElement("PLANTS");
        String homeDir = appProperties->getCommonSettings(true)->getFile().getParentDirectory().getFullPathName();
        String manualPath;
#if !defined(MACOSX)
        manualPath = File::getSpecialLocation(File::currentExecutableFile).getParentDirectory().getFullPathName()+"/CsoundDocs";
#else
        manualPath = "/Library/Frameworks/CsoundLib64.framework/Versions/6.0/Resources/Manual";
#endif
        defaultPropSet->setValue("CsoundHelpDir", manualPath);
        defaultPropSet->setValue("PlantFileDir", homeDir+"/Plants");
        defaultPropSet->setValue("ExamplesDir", homeDir+"/Examples/Examples");
        defaultPropSet->setValue("DisablePluginInfo", 0);
        defaultPropSet->setValue("ShowEditorConsole", 1);
        defaultPropSet->setValue("ExternalEditor", 0);
        defaultPropSet->setValue("UseCabbageIO", 1);
        defaultPropSet->setValue("ShowConsoleWithEditor", 1);
        defaultPropSet->setValue("UsingCabbageCsound", 1);
        defaultPropSet->setValue("AudioEnabled", 1);
        defaultPropSet->setValue("DisableCompilerErrorWarning", 0);
        defaultPropSet->setValue("SetAlwaysOnTop", 1);
        defaultPropSet->setValue("GridSize", 4);
        defaultPropSet->setValue("PlantRepository", xml);
        defaultPropSet->setValue("EditorColourScheme", 0);
        defaultPropSet->setValue("showTabs", 1);
        defaultPropSet->setValue("EnablePopupDisplay", 1);
        defaultPropSet->setValue("ShowNativeFileDialogues", 1);
        defaultPropSet->setValue("EnableNativePopup", 0);
        defaultPropSet->setValue("windowX", 100);
        defaultPropSet->setValue("windowY", 100);
#if defined(WIN32)
        defaultPropSet->setValue("Fonttype", "Consolas");
#elif defined(MACOSX)
        defaultPropSet->setValue("Fonttype", "Courier New");
#else
        defaultPropSet->setValue("Fonttype", "Droid Sans Mono");
#endif

        appProperties->getUserSettings()->setFallbackPropertySet(defaultPropSet);
        filterWindow = new StandaloneFilterWindow (String("Cabbage"), Colours::black, commandLineParameters);
        filterWindow->setTitleBarButtonsRequired (DocumentWindow::allButtons, false);
        filterWindow->setVisible (true);
        //turn off resizeable...
        filterWindow->setResizable(true, true);
        juce::Thread::setCurrentThreadName("cabbage");
    }

    void shutdown()
    {
        filterWindow = 0;// = nullptr;
        if(appProperties==nullptr)
            cUtils::debug("null");
        appProperties->closeFiles();
        deleteAndZero(appProperties);
        deleteAndZero(defaultPropSet);
        deleteAndZero(lookAndFeel);
        deleteAndZero(lookAndFeelBasic);
    }

    const String getApplicationName()
    {
        return String("cabbage");
    }

    const String getApplicationVersion()
    {
        return String("0.0");
    }

    bool moreThanOneInstanceAllowed()
    {
        return true;
    }

private:
    ScopedPointer<StandaloneFilterWindow> filterWindow;
};

START_JUCE_APPLICATION (CabbageStandalone)
