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
    setOpenGLRenderingEngine();
    //Desktop::getInstance().setKioskModeComponent(this);
    setTitleBarButtonsRequired(0, false);
    Component::setLookAndFeel(lookAndFeel);
    pluginHolder = new StandalonePluginHolder ();
    desktopRect = Desktop::getInstance().getDisplays().getMainDisplay().userArea.toDouble();
    loadFile(filename);
    setVisible (true);
    setFullScreen(true);
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
#if JUCE_OPENGL
    if (getRenderingEngines()[index] == openGLRendererName)
    {
        //cUtils::showMessage("Attaching to opengl");
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

AudioProcessorEditor* StandaloneFilterWindow::createEditorComp()
{
    if (AudioProcessorEditor* ed = getAudioProcessor()->createEditorIfNeeded())
    {
        return getAudioProcessor()->createEditorIfNeeded();
    }
    else
        return nullptr;
}

void StandaloneFilterWindow::deleteEditorComp()
{
    if (AudioProcessorEditor* ed = dynamic_cast<AudioProcessorEditor*> (getContentComponent()))
    {
        pluginHolder->processor->editorBeingDeleted (ed);
        clearContentComponent();
    }
}

//==============================================================================
void StandaloneFilterWindow::closeButtonPressed()
{
    JUCEApplicationBase::quit();
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
        float originalPluginWidth=100;
        float originalPluginHeight=100;
        StringArray csdArray;
        csdArray.addLines(file.loadFileAsString());
        for(int i=0; i<csdArray.size(); i++)
        {
            if(csdArray[i].contains("form "))
            {
                CabbageGUIType cAttr(csdArray[i], -99);
                originalPluginWidth = cAttr.getNumProp(CabbageIDs::width);
                originalPluginHeight = cAttr.getNumProp(CabbageIDs::height);
                this->getProperties().set("colour", cAttr.getStringProp(CabbageIDs::colour));
                this->lookAndFeelChanged();
            }
        }

        Point<float> scale;

        if(originalPluginWidth>=originalPluginHeight)
        {
            scale = Point<float>(desktopRect.getWidth()/originalPluginWidth,
                                 (desktopRect.getHeight()/originalPluginHeight)*.95);
        }
        else
        {
            scale = Point<float>(desktopRect.getHeight()/originalPluginHeight,
                                 (desktopRect.getHeight()/originalPluginHeight)*.95);
        }

        pluginHolder->createPlugin(file.getFullPathName(), scale);
        AudioProcessorEditor* ed = createEditorComp();
        setContentOwned(ed, true);
        //createEditorComp();

        ed->setSize(desktopRect.getWidth(), desktopRect.getHeight());
        //this->resized();

        setName(pluginHolder->processor->getName());
        this->setFullScreen(true);
        pluginHolder->startPlaying();

    }

}


//============================================================
// plugin holder
//============================================================
StandalonePluginHolder::StandalonePluginHolder ()
{
    createPlugin("", Point<float>(1.f, 1.f));
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
void StandalonePluginHolder::createPlugin(String file, Point<float> scale)
{
    AudioProcessor::setTypeOfNextNewPlugin (AudioProcessor::wrapperType_Standalone);

    if(file.isEmpty())
#ifndef AndroidDebug
        processor = createPluginFilter("", scale);
#else
        processor = createCabbagePluginFilter("", false, 1);
#endif
    else
#ifndef AndroidDebug
        processor = createPluginFilter(file, scale);
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