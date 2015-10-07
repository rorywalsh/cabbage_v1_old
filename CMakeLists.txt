cmake_minimum_required(VERSION 2.8)

project(Cabbage)

set(CABBAGE_SRCS
  Source/BinaryData.cpp
  Source/CabbageCallOutBox.cpp
  Source/CabbageGUIClass.cpp
  Source/CabbageLookAndFeel.cpp
  Source/CabbageMainPanel.cpp
  Source/CabbagePropertiesDialog.cpp
  Source/CabbageTable.cpp
  Source/Table.cpp 
  Source/ComponentLayoutEditor.cpp
  Source/Editor/SplitComponent.cpp
  Source/CabbageAudioDeviceSelectorComponent.cpp
  Source/Editor/CodeEditor.cpp
  Source/Editor/CodeWindow.cpp
  Source/Editor/CommandManager.cpp
  Source/Plugin/CabbagePluginEditor.cpp
  Source/Plugin/CabbagePluginProcessor.cpp
  Source/Soundfiler.cpp
  Source/Standalone/CabbageStandaloneDialog.cpp
  Source/Standalone/StandaloneFilterApp.cpp
  Source/XYPad.cpp
  Source/XYPadAutomation.cpp
  JuceLibraryCode/modules/juce_audio_basics/juce_audio_basics.cpp
  JuceLibraryCode/modules/juce_audio_devices/juce_audio_devices.cpp
  JuceLibraryCode/modules/juce_audio_formats/juce_audio_formats.cpp
  JuceLibraryCode/modules/juce_audio_processors/juce_audio_processors.cpp
  JuceLibraryCode/modules/juce_audio_utils/juce_audio_utils.cpp
  JuceLibraryCode/modules/juce_core/juce_core.cpp
  JuceLibraryCode/modules/juce_cryptography/juce_cryptography.cpp
  JuceLibraryCode/modules/juce_data_structures/juce_data_structures.cpp
  JuceLibraryCode/modules/juce_events/juce_events.cpp
  JuceLibraryCode/modules/juce_graphics/juce_graphics.cpp
  JuceLibraryCode/modules/juce_gui_basics/juce_gui_basics.cpp
  JuceLibraryCode/modules/juce_gui_extra/juce_gui_extra.cpp)

set(CABBAGE_LIBS
  X11 Xext Xinerama asound dl freetype pthread rt m csound64 sndfile)

include_directories(/usr/include)
include_directories(/usr/include/freetype2)
include_directories(/usr/local/include/csound)
include_directories(JuceLibraryCode)
include_directories(JuceLibraryCode/modules)

add_executable(cabbage-standalone ${CABBAGE_SRCS})
target_link_libraries(cabbage-standalone ${CABBAGE_LIBS})


