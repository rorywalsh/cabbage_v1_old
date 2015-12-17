#include "AndroidStandaloneFilterWindow.h"

String filename("");
int bufferSize;

extern StandaloneFilterWindow* filterWindow;

static const char* openGLRendererName = "OpenGL Renderer";

StandaloneFilterWindow::StandaloneFilterWindow ()
    : DocumentWindow ("Cabbage", Colours::black, DocumentWindow::minimiseButton | DocumentWindow::closeButton),
      lookAndFeel(new CabbageLookAndFeel()),
      editorShowing(true),
      globalScale(1.f),
      firstRun(true)
{
    //setOpenGLRenderingEngine();
    ///Desktop::getInstance().setGlobalScaleFactor(0.5f);
    setTitleBarButtonsRequired(0, false);
    //setUsingNativeTitleBar (true);
    Component::setLookAndFeel(lookAndFeel);
    pluginHolder = new StandalonePluginHolder ();
    //centreWithSize(getWidth(), getHeight());
    getProperties().set("colour", Colour(58, 110, 182).toString());
    lookAndFeelChanged();
    setFullScreen(true);
    loadFile(filename);
    createEditorComp();

    //setSize(200, 200);
    setName(pluginHolder->processor->getName());
    setVisible (true);
}

StandaloneFilterWindow::~StandaloneFilterWindow()
{
#if JUCE_OPENGL
    openGLContext.detach();
#endif

#ifndef AndroidDebug

    pluginHolder->stopPlaying();
#endif

    deleteEditorComp();
    pluginHolder = nullptr;
}


//==============================================================================
// rendering routines
//==============================================================================
StringArray StandaloneFilterWindow::getRenderingEngines()
{
    StringArray renderingEngines;

    if (ComponentPeer* peer = getPeer())
        renderingEngines = peer->getAvailableRenderingEngines();

#if JUCE_OPENGL
    renderingEngines.add (openGLRendererName);
#endif

    return renderingEngines;
}

void StandaloneFilterWindow::setRenderingEngine (int index)
{
    //showMessageBubble (getRenderingEngines()[index]);

#if JUCE_OPENGL
    if (getRenderingEngines()[index] == openGLRendererName)
    {
        openGLContext.attachTo (*this);
        return;
    }

    openGLContext.detach();
#endif

    if (ComponentPeer* peer = getPeer())
        peer->setCurrentRenderingEngine (index);
}

void StandaloneFilterWindow::setOpenGLRenderingEngine()
{
    setRenderingEngine (getRenderingEngines().indexOf (openGLRendererName));
}

int StandaloneFilterWindow::getActiveRenderingEngine()
{
#if JUCE_OPENGL
    if (openGLContext.isAttached())
        return getRenderingEngines().indexOf (openGLRendererName);
#endif

    if (ComponentPeer* peer = getPeer())
        return peer->getCurrentRenderingEngine();

    return 0;
}

//==============================================================================
AudioProcessor* StandaloneFilterWindow::getAudioProcessor() const noexcept
{
    return pluginHolder->processor;
}
AudioDeviceManager& StandaloneFilterWindow::getDeviceManager() const noexcept
{
    return pluginHolder->deviceManager;
}

void StandaloneFilterWindow::createEditorComp()
{
    setContentOwned (getAudioProcessor()->createEditorIfNeeded(), true);
    if(firstRun)
    {
        desktopRect = Desktop::getInstance().getDisplays().getMainDisplay().userArea.toFloat();
        firstRun=false;
    }
}

void StandaloneFilterWindow::deleteEditorComp()
{
    if (AudioProcessorEditor* ed = dynamic_cast<AudioProcessorEditor*> (getContentComponent()))
    {
        pluginHolder->processor->editorBeingDeleted (ed);
        clearContentComponent();
    }
}

/** Deletes and re-creates the plugin, resetting it to its default state. */
void StandaloneFilterWindow::resetToDefaultState()
{
    pluginHolder->stopPlaying();
    deleteEditorComp();
    pluginHolder->deletePlugin();

    pluginHolder->createPlugin("");
    setName(pluginHolder->processor->getName());
    createEditorComp();
    pluginHolder->startPlaying();
}

//==============================================================================
void StandaloneFilterWindow::closeButtonPressed()
{
    JUCEApplicationBase::quit();
}

void StandaloneFilterWindow::buttonClicked (Button*)
{

}

void StandaloneFilterWindow::loadFile(String filename)
{
    File file(filename);
    pluginHolder->stopPlaying();
    pluginHolder->reloadAudioDeviceState();
    deleteEditorComp();
    pluginHolder->deletePlugin();

    if(file.existsAsFile())
    {
        //cUtils::showMessage(file.loadFileAsString());
        pluginHolder->createPlugin(file.getFullPathName());

        createEditorComp();

        float pluginWidth = pluginHolder->processor->getActiveEditor()->getWidth();
        float pluginHeight = pluginHolder->processor->getActiveEditor()->getHeight();

        //cUtils::showMessage(String(globalScale)+":"+String(desktopRect.getWidth())+":"+String(pluginWidth));

        //this will causes all plugins to resize to fit the screen on android.
        //Desktop::getInstance().setGlobalScaleFactor(1.f/globalScale);
        globalScale = desktopRect.getWidth()/pluginWidth;
        //Desktop::getInstance().setGlobalScaleFactor(globalScale);

        //setOpenGLRenderingEngine()
        setName(pluginHolder->processor->getName());
        pluginHolder->startPlaying();
        clearContentComponent();
        setContentOwned (getAudioProcessor()->createEditorIfNeeded(), true);
        setOpenGLRenderingEngine();
        //cUtils::showMessage(rect.getHeight());
        StringArray csdArray;
        csdArray.addLines(file.loadFileAsString());
        for(int i=0; i<csdArray.size(); i++)
        {
            if(csdArray[i].contains("form "))
            {
                CabbageGUIClass cAttr(csdArray[i], -99);
                this->getProperties().set("colour", cAttr.getStringProp(CabbageIDs::colour));
                this->lookAndFeelChanged();
            }
        }
    }

}


//============================================================
// plugin holder
//============================================================
StandalonePluginHolder::StandalonePluginHolder ()
{
    createPlugin("");
    setupAudioDevices();
    reloadPluginState();
    startPlaying();
}

StandalonePluginHolder::~StandalonePluginHolder()
{
    deletePlugin();
    shutDownAudioDevices();
}

//==============================================================================
void StandalonePluginHolder::createPlugin(String file)
{
    AudioProcessor::setTypeOfNextNewPlugin (AudioProcessor::wrapperType_Standalone);

    if(file.isEmpty())
#ifndef AndroidDebug
        processor = createPluginFilter("");
#else
        processor = createCabbagePluginFilter("", false, 1);
#endif
    else
#ifndef AndroidDebug
        processor = createPluginFilter(file);
#else
        processor = createCabbagePluginFilter(file, false, 1);
#endif

    if(processor == nullptr) // Your createPluginFilter() function must return a valid object!
        cUtils::showMessage("Something not right in plugin");


    //processor->createGUI(tempFile.loadFileAsString(), true);

    AudioProcessor::setTypeOfNextNewPlugin (AudioProcessor::wrapperType_Undefined);
    processor->setPlayConfigDetails(JucePlugin_MaxNumInputChannels,
                                    JucePlugin_MaxNumOutputChannels,
                                    44100, 64);

    //showAudioSettingsDialog();
}

void StandalonePluginHolder::deletePlugin()
{
    stopPlaying();
    processor = nullptr;
    Logger::writeToLog("deleting plugin");
}

String StandalonePluginHolder::getFilePatterns (const String& fileSuffix)
{
    if (fileSuffix.isEmpty())
        return String();

    return (fileSuffix.startsWithChar ('.') ? "*" : "*.") + fileSuffix;
}

//==============================================================================
void StandalonePluginHolder::startPlaying()
{
    player.setProcessor (processor);
}

void StandalonePluginHolder::stopPlaying()
{
    player.setProcessor (nullptr);
}

//==============================================================================
/** Shows an audio properties dialog box modally. */
void StandalonePluginHolder::showAudioSettingsDialog()
{
    DialogWindow::LaunchOptions o;
    o.content.setOwned (new AudioDeviceSelectorComponent (deviceManager,
                        processor->getNumInputChannels(),
                        processor->getNumInputChannels(),
                        processor->getNumOutputChannels(),
                        processor->getNumOutputChannels(),
                        true, false,
                        true, false));
    o.content->setSize (500, 450);

    o.dialogTitle                   = TRANS("Audio Settings");
    o.dialogBackgroundColour        = Colour (0xfff0f0f0);
    o.escapeKeyTriggersCloseButton  = true;
    o.useNativeTitleBar             = true;
    o.resizable                     = false;

    o.launchAsync();
}

void StandalonePluginHolder::saveAudioDeviceState()
{
    //if (settings != nullptr)
    // {
    //     ScopedPointer<XmlElement> xml (deviceManager.createStateXml());
    //     settings->setValue ("audioSetup", xml);
    // }
}

void StandalonePluginHolder::reloadAudioDeviceState()
{
    ScopedPointer<XmlElement> savedState;

    //if (settings != nullptr)
    //    savedState = settings->getXmlValue ("audioSetup");
    AudioDeviceManager::AudioDeviceSetup config;

    deviceManager.initialise (processor->getNumInputChannels(),
                              processor->getNumOutputChannels(),
                              savedState,
                              true);

    deviceManager.getAudioDeviceSetup(config);
    const Array<int> bufferSizes (deviceManager.getCurrentAudioDevice()->getAvailableBufferSizes());
    if(bufferSize>1)
    {
        bufferSize = bufferSizes[bufferSizes.size()-1];
        //cUtils::showMessage(bufferSize);
    }
    else if(bufferSize==1)
    {
        bufferSize = bufferSizes[round((bufferSizes.size()-1)/2)];
        //cUtils::showMessage(bufferSize);
    }
    else
        bufferSize = 0;


    config.bufferSize = bufferSize;
    deviceManager.setAudioDeviceSetup (config, true);
    //showAudioSettingsDialog();
    //cUtils::showMessage(deviceManager.getCurrentAudioDevice()->getCurrentBufferSizeSamples());
}

//==============================================================================
void StandalonePluginHolder::savePluginState()
{
    // if (settings != nullptr && processor != nullptr)
    // {
    //     MemoryBlock data;
    //     processor->getStateInformation (data);

    //     settings->setValue ("filterState", data.toBase64Encoding());
    // }
}

void StandalonePluginHolder::showAlertBox()
{
    cUtils::showMessage("Hello");
}

void StandalonePluginHolder::reloadPluginState()
{
    // if (settings != nullptr)
    // {
    //     MemoryBlock data;

    //     if (data.fromBase64Encoding (settings->getValue ("filterState")) && data.getSize() > 0)
    //         processor->setStateInformation (data.getData(), (int) data.getSize());
    // }
}

void StandalonePluginHolder::setupAudioDevices()
{
    Logger::writeToLog("gotcha!!");
    deviceManager.addAudioCallback (&player);
    deviceManager.addMidiInputCallback (String::empty, &player);

    reloadAudioDeviceState();
}

void StandalonePluginHolder::shutDownAudioDevices()
{
    saveAudioDeviceState();

    deviceManager.removeMidiInputCallback (String::empty, &player);
    deviceManager.removeAudioCallback (&player);
}

#ifdef JUCE_ANDROID
extern "C" __attribute__ ((visibility("default"))) void Java_com_yourcompany_cabbage_Cabbage_loadCabbageFile (JNIEnv* env, jobject activity,jstring testString, jint suggestedBufferSize)

{
    bufferSize = (int)suggestedBufferSize;
    const char *inCStr = env->GetStringUTFChars(testString,nullptr);
    if(!filterWindow)
        filename = String(inCStr);
    else
        filterWindow->loadFile(String(inCStr));
    env->ReleaseStringUTFChars(testString, inCStr);
}

#endif