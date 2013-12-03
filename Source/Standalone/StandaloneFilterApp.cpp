#include "CabbageStandaloneDialog.h"
#include "../CabbageGUIClass.h"
#include "../CabbageUtils.h"
#include "../CabbageLookAndFeel.h"

ApplicationProperties* appProperties = nullptr;
PropertySet* defaultPropSet = nullptr;
String currentApplicationDirectory;
CabbageTimer* cabbageTimer =nullptr;
StringArray undoHistory;




class CabbageStandalone : public JUCEApplication
    {
       public:
          CabbageStandalone()
          {
          }

          void initialise(const String& /*commandLineParameters*/)
          {
			File thisFile(File::getSpecialLocation(File::currentExecutableFile));
			currentApplicationDirectory =thisFile.getParentDirectory().getFullPathName();
			//CabbageUtils::showMessage(currentApplicationDirectory);
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
			String plantDir = appProperties->getCommonSettings(true)->getFile().getParentDirectory().getFullPathName();
			Logger::writeToLog(plantDir);
			defaultPropSet->setValue("PlantFileDir", plantDir);
			defaultPropSet->setValue("ExamplesDir", plantDir+"/Examples");
			defaultPropSet->setValue("DisablePluginInfo", 0);
			defaultPropSet->setValue("AutoUpdate", 0);
			defaultPropSet->setValue("UseCabbageIO", 1);
			defaultPropSet->setValue("AudioEnabled", 1);
			defaultPropSet->setValue("DisableGUIEditModeWarning", 0);
			defaultPropSet->setValue("SetAlwaysOnTop", 1);	
			defaultPropSet->setValue("PlantRepository", xml);
			defaultPropSet->setValue("EditorColourScheme", 1);
			
			appProperties->getUserSettings()->setFallbackPropertySet(defaultPropSet);
			filterWindow = new StandaloneFilterWindow (String("Cabbage"), Colours::black);
            filterWindow->setTitleBarButtonsRequired (DocumentWindow::allButtons, false);
            filterWindow->setVisible (true);
            //turn off resizeable...
			//filterWindow->setResizable (true, true);
            juce::Thread::setCurrentThreadName("cabbage");
          }

          void shutdown()
          {
			filterWindow = 0;// = nullptr;
			appProperties->closeFiles();
			deleteAndZero(appProperties);
			deleteAndZero(defaultPropSet);
			deleteAndZero(cabbageTimer);
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
