# Automatically generated makefile, created by the Introjucer
# Don't edit this file! Your changes will be overwritten when you re-save the Introjucer project!

LOCAL_PATH := $(call my-dir)

# Default csound include path
ifndef CSOUND_INCLUDE_DIR
CSOUND_INCLUDE_DIR ?= "/usr/local/include/csound"
endif

# Default csound library path
ifndef CSOUND_LIBRARY_DIR
CSOUND_LIBRARY_DIR ?= "/home/rory/sourcecode/cabbageaudio/csound-android-6.05.0/CsoundAndroid/libs/armeabi-v7a"
endif

# Default csound jni path
ifndef CSOUND_JNI_DIR
CSOUND_JNI_DIR ?= "/home/rory/sourcecode/cabbageaudio/csound-android-6.05.0/CsoundAndroid/jni"
endif

include $(CLEAR_VARS)
LOCAL_MODULE           := sndfile
LOCAL_SRC_FILES        := /home/rory/sourcecode/cabbageaudio/csound-android-6.05.0/CsoundAndroid/libs/armeabi-v7a/libsndfile.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE           := csoundandroid
LOCAL_SRC_FILES        := /home/rory/sourcecode/cabbageaudio/csound-android-6.05.0/CsoundAndroid/libs/armeabi-v7a/libcsoundandroid.so

LOCAL_SHARED_LIBRARIES += gnustl_shared sndfile csoundandroid

include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)

ifeq ($(TARGET_ARCH_ABI), armeabi-v7a)
    LOCAL_ARM_MODE := arm
endif

LOCAL_MODULE := juce_jni
LOCAL_SRC_FILES := \
  ../../../Source/BinaryData.cpp\
  ../../../Source/CabbageAudioDeviceSelectorComponent.cpp\
  ../../../Source/CabbageCallOutBox.cpp\
  ../../../Source/CabbageGUIClass.cpp\
  ../../../Source/CabbageLookAndFeel.cpp\
  ../../../Source/CabbageMainPanel.cpp\
  ../../../Source/CabbagePropertiesDialog.cpp\
  ../../../Source/CabbageTable.cpp\
  ../../../Source/ComponentLayoutEditor.cpp\
  ../../../Source/Plugin/CabbagePluginEditor.cpp\
  ../../../Source/Plugin/CabbagePluginProcessor.cpp\
  ../../../Source/Soundfiler.cpp\
  ../../../Source/Table.cpp\
  ../../../Source/XYPad.cpp\
  ../../../Source/XYPadAutomation.cpp\
  ../../../Source/AndroidStandalone/AndroidStandaloneFilterApp.cpp\
  ../../../Source/AndroidStandalone/AndroidStandaloneFilterWindow.cpp\
  ../../../Source/AndroidStandalone/AndroidCsound.cpp\
  ../../../JuceLibraryCode/modules/juce_audio_basics/juce_audio_basics.cpp\
  ../../../JuceLibraryCode/modules/juce_audio_devices/juce_audio_devices.cpp\
  ../../../JuceLibraryCode/modules/juce_audio_formats/juce_audio_formats.cpp\
  ../../../JuceLibraryCode/modules/juce_audio_processors/juce_audio_processors.cpp\
  ../../../JuceLibraryCode/modules/juce_audio_utils/juce_audio_utils.cpp\
  ../../../JuceLibraryCode/modules/juce_core/juce_core.cpp\
  ../../../JuceLibraryCode/modules/juce_cryptography/juce_cryptography.cpp\
  ../../../JuceLibraryCode/modules/juce_data_structures/juce_data_structures.cpp\
  ../../../JuceLibraryCode/modules/juce_events/juce_events.cpp\
  ../../../JuceLibraryCode/modules/juce_graphics/juce_graphics.cpp\
  ../../../JuceLibraryCode/modules/juce_gui_basics/juce_gui_basics.cpp\
  ../../../JuceLibraryCode/modules/juce_gui_extra/juce_gui_extra.cpp\
  ../../../JuceLibraryCode/modules/juce_opengl/juce_opengl.cpp\

ifeq ($(NDK_DEBUG),1)  
  LOCAL_CPPFLAGS += -fsigned-char -fexceptions -frtti -g -I "../../JuceLibraryCode" -I "../../../sourcecode/juce4/modules" -O0 -std=c++11 -std=gnu++11 -D "JUCE_ANDROID=1" -D "AndroidBuild=1" -D "JUCE_ANDROID_API_VERSION=10" -D "JUCE_ANDROID_ACTIVITY_CLASSNAME=com_yourcompany_cabbage_Cabbage" -D JUCE_ANDROID_ACTIVITY_CLASSPATH=\"com/yourcompany/cabbage/Cabbage\" -D "DEBUG=1" -D "_DEBUG=1" -D "JUCER_ANDROID_7F0E4A25=1" -D "JUCE_APP_VERSION=1.0.0" -D "JUCE_APP_VERSION_HEX=0x10000" -I$(CSOUND_INCLUDE_DIR) -I$(CSOUND_JNI_DIR)
  LOCAL_LDLIBS := -L$(CSOUND_LIBRARY_DIR) -lcsoundandroid -lsndfile -llog -lGLESv2 -landroid -lEGL 
  LOCAL_CFLAGS += -fsigned-char -fexceptions -frtti -g -I "../../JuceLibraryCode" -I "../../../sourcecode/juce4/modules" -O0 -std=c++11 -std=gnu++11 -D "JUCE_ANDROID=1" -D "JUCE_ANDROID_API_VERSION=10" -D "JUCE_ANDROID_ACTIVITY_CLASSNAME=com_yourcompany_cabbage_Cabbage" -D JUCE_ANDROID_ACTIVITY_CLASSPATH=\"com/yourcompany/cabbage/Cabbage\" -D "DEBUG=1" -D "AndroidBuild=1" -D "_DEBUG=1" -D "JUCER_ANDROID_7F0E4A25=1" -D "JUCE_APP_VERSION=1.0.0" -D "JUCE_APP_VERSION_HEX=0x10000" -I$(CSOUND_INCLUDE_DIR) -I$(CSOUND_JNI_DIR)
  LOCAL_LDLIBS := -L$(CSOUND_LIBRARY_DIR) -lcsoundandroid -lsndfile -llog -lGLESv2 -landroid -lEGL 
else
  LOCAL_CPPFLAGS += -fsigned-char -fexceptions -frtti -I "../../JuceLibraryCode" -I "../../../sourcecode/juce4/modules" -O3 -std=c++11 -std=gnu++11 -D "JUCE_ANDROID=1" -D "AndroidBuild=1" -D "JUCE_ANDROID_API_VERSION=10" -D "JUCE_ANDROID_ACTIVITY_CLASSNAME=com_yourcompany_cabbage_Cabbage" -D JUCE_ANDROID_ACTIVITY_CLASSPATH=\"com/yourcompany/cabbage/Cabbage\" -D "NDEBUG=1" -D "JUCER_ANDROID_7F0E4A25=1" -D "JUCE_APP_VERSION=1.0.0" -D "JUCE_APP_VERSION_HEX=0x10000" -I$(CSOUND_INCLUDE_DIR) -I$(CSOUND_JNI_DIR)
  LOCAL_LDLIBS := -L$(CSOUND_LIBRARY_DIR) -lcsoundandroid -lsndfile -llog -lGLESv2 -landroid -lEGL 
  LOCAL_CFLAGS += -fsigned-char -fexceptions -frtti -I "../../JuceLibraryCode" -I "../../../sourcecode/juce4/modules" -O3 -std=c++11 -std=gnu++11 -D "JUCE_ANDROID=1" -D "AndroidBuild=1" -D "JUCE_ANDROID_API_VERSION=10" -D "JUCE_ANDROID_ACTIVITY_CLASSNAME=com_yourcompany_cabbage_Cabbage" -D JUCE_ANDROID_ACTIVITY_CLASSPATH=\"com/yourcompany/cabbage/Cabbage\" -D "NDEBUG=1" -D "JUCER_ANDROID_7F0E4A25=1" -D "JUCE_APP_VERSION=1.0.0" -D "JUCE_APP_VERSION_HEX=0x10000" -I$(CSOUND_INCLUDE_DIR) -I$(CSOUND_JNI_DIR)
  LOCAL_LDLIBS := -L$(CSOUND_LIBRARY_DIR) -llog -lGLESv2 -landroid -lEGL -lcsoundandroid -lsndfile
endif


include $(BUILD_SHARED_LIBRARY)
