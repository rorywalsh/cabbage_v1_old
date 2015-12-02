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

#include "jni.h"
//  and make it create an instance of the filter subclass that you're building.
///extern CabbagePluginAudioProcessor* JUCE_CALLTYPE createCabbagePluginFilter(String inputfile, bool guiOnOff, int plugType);

#ifndef AndroidDebug
extern AudioProcessor* JUCE_CALLTYPE createPluginFilter(String file);
#else
extern AudioProcessor* JUCE_CALLTYPE createCabbagePluginFilter(String file, bool refresh, int pluginType);
#endif


#include <android/log.h>

#define  LOG_TAG    "someTag"

#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define  LOGW(...)  __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)


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
    StandalonePluginHolder ();

    ~StandalonePluginHolder();
    //==============================================================================
    void createPlugin(String file);
    void deletePlugin();
    static String getFilePatterns (const String& fileSuffix);
    void startPlaying();
    void stopPlaying();
    //==============================================================================
    /** Shows an audio properties dialog box modally. */
    void showAudioSettingsDialog();
    void saveAudioDeviceState();
    void reloadAudioDeviceState();
    void savePluginState();
    void showAlertBox();
    void reloadPluginState();
    //==============================================================================
    //ScopedPointer<PropertySet> settings;
    ScopedPointer<AudioProcessor> processor;
    //ScopedPointer<CabbagePluginAudioProcessor> processor;
    AudioDeviceManager deviceManager;
    AudioProcessorPlayer player;

private:
    void setupAudioDevices();
    void shutDownAudioDevices();

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
    public ButtonListener
{
public:
    //==============================================================================
    /** Creates a window with a given title and colour.
        The settings object can be a PropertySet that the class should use to
        store its settings - the object that is passed-in will be owned by this
        class and deleted automatically when no longer needed. (It can also be null)
    */
    StandaloneFilterWindow();

    ~StandaloneFilterWindow();
    //==============================================================================
    AudioProcessor* getAudioProcessor() const noexcept;
    AudioDeviceManager& getDeviceManager() const noexcept;
    void createEditorComp();
    void deleteEditorComp();
    /** Deletes and re-creates the plugin, resetting it to its default state. */
    void resetToDefaultState();
    //==============================================================================
    void closeButtonPressed();
    void buttonClicked (Button*);
    void loadFile(String filename);
    void resized();

    ScopedPointer<CabbageLookAndFeel> lookAndFeel;
    //ScopedPointer<FileBrowser> fileBrowser;
    bool editorShowing;

private:
    //==============================================================================
    TextButton optionsButton;
    ScopedPointer<StandalonePluginHolder> pluginHolder;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StandaloneFilterWindow)

};


#endif   // JUCE_STANDALONEFILTERWINDOW_H_INCLUDED
