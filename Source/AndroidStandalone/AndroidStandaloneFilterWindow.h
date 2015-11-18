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

#ifndef JUCE_STANDALONEFILTERWINDOW_H_INCLUDED
#define JUCE_STANDALONEFILTERWINDOW_H_INCLUDED

//#include "../Editor/CodeWindow.h"
#include "../CabbageUtils.h"
#include "../Plugin/CabbagePluginProcessor.h"
#include "../Plugin/CabbagePluginEditor.h"
#include "../CabbageAudioDeviceSelectorComponent.h"


//  and make it create an instance of the filter subclass that you're building.
///extern CabbagePluginAudioProcessor* JUCE_CALLTYPE createCabbagePluginFilter(String inputfile, bool guiOnOff, int plugType);

extern AudioProcessor* JUCE_CALLTYPE createPluginFilter(String file);
//==============================================================================
/**
    An object that creates and plays a standalone instance of an AudioProcessor.

    The object will create your processor using the same createPluginFilter()
    function that the other plugin wrappers use, and will run it through the
    computer's audio/MIDI devices using AudioDeviceManager and AudioProcessorPlayer.
*/
class StandalonePluginHolder
{
public:
    /** Creates an instance of the default plugin.

        The settings object can be a PropertySet that the class should use to
        store its settings - the object that is passed-in will be owned by this
        class and deleted automatically when no longer needed. (It can also be null)
    */
    StandalonePluginHolder (PropertySet* settingsToUse)
        : settings (settingsToUse)
    {
        createPlugin("");
        setupAudioDevices();
        reloadPluginState();
        startPlaying();
    }

    ~StandalonePluginHolder()
    {
        deletePlugin();
        shutDownAudioDevices();
    }

    //==============================================================================
    void createPlugin(String file)
    {
        AudioProcessor::setTypeOfNextNewPlugin (AudioProcessor::wrapperType_Standalone);

		if(file.isEmpty())
			processor = createPluginFilter("");
		else
			processor = createPluginFilter(file);

        if(processor == nullptr) // Your createPluginFilter() function must return a valid object!
            cUtils::showMessage("Something not right in plugin");


        //processor->createGUI(tempFile.loadFileAsString(), true);

        AudioProcessor::setTypeOfNextNewPlugin (AudioProcessor::wrapperType_Undefined);

        processor->setPlayConfigDetails (JucePlugin_MaxNumInputChannels,
                                         JucePlugin_MaxNumOutputChannels,
                                         22050, 1024);
    }

    void deletePlugin()
    {
        stopPlaying();
        processor = nullptr;
        Logger::writeToLog("deleting plugin");
    }

    static String getFilePatterns (const String& fileSuffix)
    {
        if (fileSuffix.isEmpty())
            return String();

        return (fileSuffix.startsWithChar ('.') ? "*" : "*.") + fileSuffix;
    }

    //==============================================================================
    void startPlaying()
    {
        player.setProcessor (processor);
    }

    void stopPlaying()
    {
        player.setProcessor (nullptr);
    }

    //==============================================================================
    /** Shows an audio properties dialog box modally. */
    void showAudioSettingsDialog()
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

    void saveAudioDeviceState()
    {
        if (settings != nullptr)
        {
            ScopedPointer<XmlElement> xml (deviceManager.createStateXml());
            settings->setValue ("audioSetup", xml);
        }
    }

    void reloadAudioDeviceState()
    {
        ScopedPointer<XmlElement> savedState;

        if (settings != nullptr)
            savedState = settings->getXmlValue ("audioSetup");

        deviceManager.initialise (processor->getNumInputChannels(),
                                  processor->getNumOutputChannels(),
                                  savedState,
                                  true);
    }

    //==============================================================================
    void savePluginState()
    {
        if (settings != nullptr && processor != nullptr)
        {
            MemoryBlock data;
            processor->getStateInformation (data);

            settings->setValue ("filterState", data.toBase64Encoding());
        }
    }

    void reloadPluginState()
    {
        if (settings != nullptr)
        {
            MemoryBlock data;

            if (data.fromBase64Encoding (settings->getValue ("filterState")) && data.getSize() > 0)
                processor->setStateInformation (data.getData(), (int) data.getSize());
        }
    }

    //==============================================================================
    ScopedPointer<PropertySet> settings;
    ScopedPointer<AudioProcessor> processor;
    //ScopedPointer<CabbagePluginAudioProcessor> processor;
    AudioDeviceManager deviceManager;
    AudioProcessorPlayer player;

private:
    void setupAudioDevices()
    {
        Logger::writeToLog("gotcha!!");
        deviceManager.addAudioCallback (&player);
        deviceManager.addMidiInputCallback (String::empty, &player);

        reloadAudioDeviceState();
    }

    void shutDownAudioDevices()
    {
        saveAudioDeviceState();

        deviceManager.removeMidiInputCallback (String::empty, &player);
        deviceManager.removeAudioCallback (&player);
    }


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StandalonePluginHolder)
};

//==============================================================================
/**
    A class that can be used to run a simple standalone application containing your filter.

    Just create one of these objects in your JUCEApplicationBase::initialise() method, and
    let it do its work. It will create your filter object using the same createPluginFilter() function
    that the other plugin wrappers use.
*/
class StandaloneFilterWindow    : public DocumentWindow,
	private FileBrowserListener,
    public ButtonListener   // (can't use Button::Listener due to VC2005 bug)
{
public:
    //==============================================================================
    /** Creates a window with a given title and colour.
        The settings object can be a PropertySet that the class should use to
        store its settings - the object that is passed-in will be owned by this
        class and deleted automatically when no longer needed. (It can also be null)
    */
    StandaloneFilterWindow (const String& title,
                            Colour backgroundColour,
                            PropertySet* settingsToUse)
        : DocumentWindow (title, backgroundColour, DocumentWindow::minimiseButton | DocumentWindow::closeButton),
          optionsButton ("Open"), lookAndFeel(new CabbageLookAndFeel()),
		  editorShowing(true),
		  thread ("audio file preview"),
          directoryList(nullptr, thread),
          fileTreeComp (directoryList)
    {
        setTitleBarButtonsRequired (0, false);
		Component::setLookAndFeel(lookAndFeel);
        Component::addAndMakeVisible (optionsButton);
		optionsButton.setLookAndFeel(lookAndFeel);
        optionsButton.addListener (this);
        optionsButton.setTriggeredOnMouseDown (true);
		this->setTitleBarHeight(50);
        pluginHolder = new StandalonePluginHolder (settingsToUse);

	

		wildcardFilter = new WildcardFileFilter("*.csd", "*", "File Filter"),
		directoryList.setFileFilter((FileFilter*)wildcardFilter);

		getProperties().set("colour", Colour(58, 110, 182).toString());
		lookAndFeelChanged();

			
        createEditorComp();
		
        directoryList.setDirectory(File("/sdcard"), true, true);
        thread.startThread (3);
		fileTreeComp.setColour (FileTreeComponent::backgroundColourId, cUtils::getBackgroundSkin());
		fileTreeComp.setItemHeight(40);
        fileTreeComp.addListener (this);
		fileTreeComp.setBounds(0, 0, getWidth(), getHeight());
		//addAndMakeVisible(&fileTreeComp);

        if (PropertySet* props = pluginHolder->settings)
        {
            const int x = props->getIntValue ("windowX", -100);
            const int y = props->getIntValue ("windowY", -100);

            if (x != -100 && y != -100)
                setBoundsConstrained (juce::Rectangle<int> (x, y, getWidth(), getHeight()));
            else
                centreWithSize (getWidth(), getHeight());
        }
        else
        {
            centreWithSize (getWidth(), getHeight());
        }
    }

    ~StandaloneFilterWindow()
    {
        if (PropertySet* props = pluginHolder->settings)
        {
            props->setValue ("windowX", getX());
            props->setValue ("windowY", getY());
        }

        pluginHolder->stopPlaying();
        deleteEditorComp();
        pluginHolder = nullptr;
    }

    //==============================================================================
    AudioProcessor* getAudioProcessor() const noexcept
    {
        return pluginHolder->processor;
    }
    AudioDeviceManager& getDeviceManager() const noexcept
    {
        return pluginHolder->deviceManager;
    }

    void createEditorComp()
    {
        setContentOwned (getAudioProcessor()->createEditorIfNeeded(), true);
    }

    void deleteEditorComp()
    {
        if (AudioProcessorEditor* ed = dynamic_cast<AudioProcessorEditor*> (getContentComponent()))
        {
            pluginHolder->processor->editorBeingDeleted (ed);
            clearContentComponent();
        }
    }

    /** Deletes and re-creates the plugin, resetting it to its default state. */
    void resetToDefaultState()
    {
        pluginHolder->stopPlaying();
        deleteEditorComp();
        pluginHolder->deletePlugin();

        if (PropertySet* props = pluginHolder->settings)
            props->removeValue ("filterState");

        pluginHolder->createPlugin("");
        createEditorComp();
        pluginHolder->startPlaying();
    }

    //==============================================================================
    void closeButtonPressed() override
    {
        JUCEApplicationBase::quit();
    }

    void buttonClicked (Button*) override
    {
			clearContentComponent();
			setContentNonOwned(&fileTreeComp, true);
    }

    void selectionChanged() override  {}
    void fileClicked (const File&, const MouseEvent&) override          {}
	
    void fileDoubleClicked (const File& file) override                       
	{
		pluginHolder->stopPlaying();
        deleteEditorComp();
        pluginHolder->deletePlugin();

		if(file.existsAsFile())
		{
			//cUtils::showMessage(file.loadFileAsString());
			pluginHolder->createPlugin(file.getFullPathName());
			
			createEditorComp();
			pluginHolder->startPlaying();
			clearContentComponent();
			setContentOwned (getAudioProcessor()->createEditorIfNeeded(), true);
			
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
	
    void browserRootChanged (const File&) override                      {}
	
    void resized() override
    {
        DocumentWindow::resized();
        optionsButton.setBounds (8, 6, 130, getTitleBarHeight() - 8);
    }


    ScopedPointer<StandalonePluginHolder> pluginHolder;
	ScopedPointer<CabbageLookAndFeel> lookAndFeel;
	ScopedPointer<Component> fileBrowser;
	TimeSliceThread thread;
    DirectoryContentsList directoryList;
    FileTreeComponent fileTreeComp;
	ScopedPointer<WildcardFileFilter> wildcardFilter;
	bool editorShowing;

private:
    //==============================================================================
    TextButton optionsButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StandaloneFilterWindow)
};

#endif   // JUCE_STANDALONEFILTERWINDOW_H_INCLUDED
