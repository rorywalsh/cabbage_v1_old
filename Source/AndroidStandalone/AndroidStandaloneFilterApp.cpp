#include "AndroidStandaloneFilterWindow.h"
#include "../CabbageGUIClass.h"
#include "../CabbageUtils.h"
#include "../CabbageLookAndFeel.h"

ApplicationProperties* appProperties = nullptr;
PropertySet* defaultPropSet = nullptr;

StandaloneFilterWindow* filterWindow;

class CabbageStandalone : public JUCEApplication
{
public:
    CabbageStandalone()
    {
    }

    void initialise(const String& commandLineParameters)
    {        
		filterWindow = new StandaloneFilterWindow ();
		//Desktop::getInstance().setGlobalScaleFactor(1.3);
		juce::Thread::setCurrentThreadName("cabbage");
    }

    void shutdown()
    {
        filterWindow = nullptr;
        appProperties->closeFiles();
        deleteAndZero(appProperties);
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
        return false;
    }

private:

};


START_JUCE_APPLICATION (CabbageStandalone)
