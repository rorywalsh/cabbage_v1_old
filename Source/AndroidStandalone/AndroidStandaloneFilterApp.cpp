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
        filterWindow->setTitleBarButtonsRequired (DocumentWindow::allButtons, false);
        filterWindow->setVisible (true);
        //turn off resizeable...
        filterWindow->setResizable(false, false);
        filterWindow->setFullScreen(true);
        juce::Thread::setCurrentThreadName("cabbage");
    }

    void shutdown()
    {
        filterWindow = 0;// = nullptr;
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
        return true;
    }

private:

};


START_JUCE_APPLICATION (CabbageStandalone)
