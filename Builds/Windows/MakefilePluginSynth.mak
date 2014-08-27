# Cabbage Makefile for Windows

# (this disables dependency generation if multiple architectures are set)
DEPFLAGS := $(if $(word 2, $(TARGET_ARCH)), , -MMD)

# Default csound include path
CSOUND_INCLUDE ?= "C:\Users\RoryWalsh\Documents\SourceCode\cabbageaudio\csound\include"

# Default Csound library path
CSOUND_LIBRARY ?= "C:\Users\RoryWalsh\Documents\SourceCode\cabbageaudio\csound\build\libcsound64.dll.a"

ASIO_SDK ?= "C:\SDKs\ASIOSDK2.3\common"
VST_SDK ?= "C:\SDKs\vstsdk2.4"
JUCE_LIBRARY_CODE ?= "..\..\JuceLibraryCode"

ifndef CONFIG
  CONFIG=Debug
endif

ifndef TARGET
  TARGET := CabbagePluginSynth.dll 
endif

ifeq ($(CONFIG),Debug)
  BINDIR := build
  LIBDIR := build
  OBJDIR := build/intermediate/Debug
  OUTDIR := build

  ifeq ($(TARGET_ARCH),)
    TARGET_ARCH := -march=pentium4
  endif

  CPPFLAGS := $(DEPFLAGS) -I $(JUCE_LIBRARY_CODE) -I $(VST_SDK) -I $(ASIO_SDK) -I $(CSOUND_INCLUDE) -D "Cabbage_Plugin_Synth=1" -D__MINGW32__=1 -D__MINGW_EXTENSION=1 -DJUCE_MINGW=1 -DUSE_DOUBLES=1 -DCSOUND6 -DWIN32 -DUSE_DOUBLE=1 -DJUCER_CODEBLOCKS_20734A5D=1
  CFLAGS += $(CPPFLAGS) $(TARGET_ARCH) -g -ggdb -O -Wwrite-strings -Wno-reorder -std=gnu++0x -static-libgcc -static-libstdc++ -mstackrealign -static
  CXXFLAGS += $(CFLAGS)
  LDFLAGS += -L$(BINDIR) -L$(LIBDIR) -shared -fvisibility=hidden -lgdi32 -luser32 -lkernel32 -lcomctl32 -lcomdlg32 -limm32 -lole32 -loleaut32 -lrpcrt4 -lshlwapi -luuid -lversion -lwininet -lwinmm -lws2_32 -lwsock32 $(CSOUND_LIBRARY)
  LDDEPS :=
  RESFLAGS := -I $(CSOUND_INCLUDE) -I $(VST_SDK) -I $(ASIO_SDK) -I $(CSOUND_INCLUDE) -D "CSOUND6=1" -D "USE_DOUBLE=1"  -D "LINUX=1" -D "DEBUG=1" -D "_DEBUG=1" -D "JUCER_LINUX_MAKE_7346DA2A=1" -I ../../JuceLibraryCode -I ../../JuceLibraryCode/modules
  BLDCMD = $(CXX) -o $(OUTDIR)/$(TARGET) $(OBJECTS) $(LDFLAGS) $(RESOURCES) -mwindows $(TARGET_ARCH) -static-libgcc -static-libstdc++ -mstackrealign -static
endif

ifeq ($(CONFIG),Release)
  BINDIR := build
  LIBDIR := build
  OBJDIR := build/intermediate/Release
  OUTDIR := build

  ifeq ($(TARGET_ARCH),)
    TARGET_ARCH := -march=pentium4
  endif

  CPPFLAGS := $(DEPFLAGS) -I $(JUCE_LIBRARY_CODE) -I $(VST_SDK) -I $(ASIO_SDK) -I $(CSOUND_INCLUDE) -D "Cabbage_Plugin_Synth=1" -D__MINGW32__=1 -D__MINGW_EXTENSION=1 -DJUCE_MINGW=1 -DUSE_DOUBLES=1 -DCSOUND6 -DUSE_DOUBLE=1 -DJUCER_CODEBLOCKS_20734A5D=1
  CFLAGS += $(CPPFLAGS) $(TARGET_ARCH) -O -Wwrite-strings -Wno-reorder -std=gnu++0x -static-libgcc -static-libstdc++ -mstackrealign -static
  CXXFLAGS += $(CFLAGS)
  LDFLAGS += -L$(BINDIR) -L$(LIBDIR) -shared -fvisibility=hidden -lgdi32 -luser32 -lkernel32 -lcomctl32 -lcomdlg32 -limm32 -lole32 -loleaut32 -lrpcrt4 -lshlwapi -luuid -lversion -lwininet -lwinmm -lws2_32 -lwsock32 $(CSOUND_LIBRARY)
  LDDEPS :=
  RESFLAGS := -I $(CSOUND_INCLUDE) -I $(VST_SDK) -I $(ASIO_SDK) -I $(CSOUND_INCLUDE) -D "CSOUND6=1" -D "USE_DOUBLE=1"  -D "LINUX=1" -D "DEBUG=1" -D "_DEBUG=1" -D "JUCER_LINUX_MAKE_7346DA2A=1" -I ../../JuceLibraryCode -I ../../JuceLibraryCode/modules
  BLDCMD = $(CXX) -o $(OUTDIR)/$(TARGET) $(OBJECTS) $(LDFLAGS) $(RESOURCES) -mwindows $(TARGET_ARCH) -static-libgcc -static-libstdc++ -mstackrealign -static
endif

OBJECTS := \
  $(OBJDIR)/BinaryData_5ba7f54.o \
  $(OBJDIR)/CabbageCallOutBox_1ced38fd.o \
  $(OBJDIR)/CabbageGUIClass_79b9049f.o \
  $(OBJDIR)/CabbageLookAndFeel_220a01a6.o \
  $(OBJDIR)/CabbageMainPanel_12c1333.o \
  $(OBJDIR)/CabbagePropertiesDialog_5e61b3fd.o \
  $(OBJDIR)/CabbageTable_d003e736.o \
  $(OBJDIR)/ComponentLayoutEditor_aa38c835.o \
  $(OBJDIR)/csPerfThread_f9d0b9cc.o \
  $(OBJDIR)/DirectoryContentsComponent_616f5a9f.o \
  $(OBJDIR)/CabbagePluginEditor_5a11f64e.o \
  $(OBJDIR)/CabbagePluginProcessor_73d6661b.o \
  $(OBJDIR)/Soundfiler_35ae1cd0.o \
  $(OBJDIR)/XYPad_6eaa3453.o \
  $(OBJDIR)/Table_35ae1cd9.o \
  $(OBJDIR)/XYPadAutomation_2865c48a.o \
  $(OBJDIR)/juce_audio_basics_2442e4ea.o \
  $(OBJDIR)/juce_audio_devices_a4c8a728.o \
  $(OBJDIR)/juce_audio_formats_d349f0c8.o \
  $(OBJDIR)/juce_audio_processors_44a134a2.o \
  $(OBJDIR)/juce_audio_utils_f63b12e8.o \
  $(OBJDIR)/juce_core_aff681cc.o \
  $(OBJDIR)/juce_cryptography_25c7e826.o \
  $(OBJDIR)/juce_data_structures_bdd6d488.o \
  $(OBJDIR)/juce_events_79b2840.o \
  $(OBJDIR)/juce_graphics_c8f1e7a4.o \
  $(OBJDIR)/juce_gui_basics_a630dd20.o \
  $(OBJDIR)/juce_gui_extra_7767d6a8.o \
  $(OBJDIR)/juce_PluginUtilities_e2e19a34.o \
  $(OBJDIR)/juce_VST_Wrapper_bb62e93d.o \

.PHONY: clean

$(OUTDIR)/$(TARGET): $(OBJECTS) $(LDDEPS) $(RESOURCES)
	@echo Linking CabbagePluginSynth
	-@mkdir -p $(BINDIR)
	-@mkdir -p $(LIBDIR)
	-@mkdir -p $(OUTDIR)
	@$(BLDCMD)

clean_obj:
	@echo Cleaning CabbagePluginSynth Object files
	-@rm -rf $(OBJDIR)/*
	-@rm -rf $(OBJDIR)

clean:
	@echo Cleaning CabbagePluginSynth
	-@rm -f $(OUTDIR)/$(TARGET)
	-@rm -rf $(OBJDIR)/*
	-@rm -rf $(OBJDIR)

strip:
	@echo Stripping CabbagePluginSynth
	-@strip --strip-unneeded $(OUTDIR)/$(TARGET)

$(OBJDIR)/BinaryData_5ba7f54.o: ../../Source/BinaryData.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling BinaryData.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/CabbageCallOutBox_1ced38fd.o: ../../Source/CabbageCallOutBox.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling CabbageCallOutBox.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/CabbageGUIClass_79b9049f.o: ../../Source/CabbageGUIClass.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling CabbageGUIClass.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/CabbageLookAndFeel_220a01a6.o: ../../Source/CabbageLookAndFeel.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling CabbageLookAndFeel.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/CabbageMainPanel_12c1333.o: ../../Source/CabbageMainPanel.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling CabbageMainPanel.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/CabbagePropertiesDialog_5e61b3fd.o: ../../Source/CabbagePropertiesDialog.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling CabbagePropertiesDialog.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/CabbageTable_d003e736.o: ../../Source/CabbageTable.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling CabbageTable.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/Table_35ae1cd9.o: ../../Source/Table.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling Table.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/ComponentLayoutEditor_aa38c835.o: ../../Source/ComponentLayoutEditor.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling ComponentLayoutEditor.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/csPerfThread_f9d0b9cc.o: ../../Source/csPerfThread.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling csPerfThread.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/DirectoryContentsComponent_616f5a9f.o: ../../Source/DirectoryContentsComponent.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling DirectoryContentsComponent.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/CabbagePluginEditor_5a11f64e.o: ../../Source/Plugin/CabbagePluginEditor.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling CabbagePluginEditor.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/CabbagePluginProcessor_73d6661b.o: ../../Source/Plugin/CabbagePluginProcessor.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling CabbagePluginProcessor.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/Soundfiler_35ae1cd0.o: ../../Source/Soundfiler.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling Soundfiler.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/XYPad_6eaa3453.o: ../../Source/XYPad.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling XYPad.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/XYPadAutomation_2865c48a.o: ../../Source/XYPadAutomation.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling XYPadAutomation.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_audio_basics_2442e4ea.o: ../../JuceLibraryCode/modules/juce_audio_basics/juce_audio_basics.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_audio_basics.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_audio_devices_a4c8a728.o: ../../JuceLibraryCode/modules/juce_audio_devices/juce_audio_devices.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_audio_devices.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_audio_formats_d349f0c8.o: ../../JuceLibraryCode/modules/juce_audio_formats/juce_audio_formats.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_audio_formats.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_audio_processors_44a134a2.o: ../../JuceLibraryCode/modules/juce_audio_processors/juce_audio_processors.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_audio_processors.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_audio_utils_f63b12e8.o: ../../JuceLibraryCode/modules/juce_audio_utils/juce_audio_utils.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_audio_utils.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_core_aff681cc.o: ../../JuceLibraryCode/modules/juce_core/juce_core.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_core.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_cryptography_25c7e826.o: ../../JuceLibraryCode/modules/juce_cryptography/juce_cryptography.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_cryptography.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_data_structures_bdd6d488.o: ../../JuceLibraryCode/modules/juce_data_structures/juce_data_structures.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_data_structures.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_events_79b2840.o: ../../JuceLibraryCode/modules/juce_events/juce_events.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_events.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_graphics_c8f1e7a4.o: ../../JuceLibraryCode/modules/juce_graphics/juce_graphics.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_graphics.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_gui_basics_a630dd20.o: ../../JuceLibraryCode/modules/juce_gui_basics/juce_gui_basics.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_gui_basics.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_gui_extra_7767d6a8.o: ../../JuceLibraryCode/modules/juce_gui_extra/juce_gui_extra.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_gui_extra.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_PluginUtilities_e2e19a34.o: ../../JuceLibraryCode/modules/juce_audio_plugin_client/utility/juce_PluginUtilities.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_PluginUtilities.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_VST_Wrapper_bb62e93d.o: ../../JuceLibraryCode/modules/juce_audio_plugin_client/VST/juce_VST_Wrapper.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_VST_Wrapper.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

-include $(OBJECTS:%.o=%.d)
