# Cabbage Makefile for Windows

# (this disables dependency generation if multiple architectures are set)
DEPFLAGS := $(if $(word 2, $(TARGET_ARCH)), , -MMD)

# Default csound include path
CSOUND_INCLUDE ?= "C:\Users\rory\Documents\sourceCode\cabbageaudio\csound\include"

# Default Csound library path
CSOUND_LIBRARY ?= "C:\Users\rory\Documents\sourceCode\cabbageaudio\csound\build\libcsound64.dll.a"

ASIO_SDK ?= "C:\SDKs\ASIOSDK2.3\common"
VST_SDK ?= "C:\SDKs\vstsdk2.4"
JUCE_LIBRARY_CODE ?= "..\..\JuceLibraryCode"

ifndef CONFIG
  CONFIG=Debug
endif

CC = gcc.exe
CXX = g++.exe
AR = ar.exe
LD = g++.exe
WINDRES = windres.exe

ifeq ($(CONFIG),Debug)
  BINDIR := build
  LIBDIR := build
  OBJDIR := build/host/intermediate/Debug
  OUTDIR := build

  ifeq ($(TARGET_ARCH),)
    TARGET_ARCH := -march=pentium4
  endif

  CPPFLAGS := $(DEPFLAGS) -I $(JUCE_LIBRARY_CODE) -D"CABBAGE_HOST=1" -I $(VST_SDK) -I $(ASIO_SDK) -DBUILD_DEBUGGER=1 -I $(CSOUND_INCLUDE) -D__MINGW32__=1 -D__MINGW_EXTENSION=1 -DJucePlugin_Build_Standalone=1 -DCabbage_GUI_Editor=1 -DCabbage_Build_Standalone=1 -DWIN32 -DJUCE_MINGW=1 -DUSE_DOUBLES=1 -DCSOUND6 -DUSE_DOUBLE=1 -DJUCER_CODEBLOCKS_20734A5D=1
  CFLAGS += $(CPPFLAGS) $(TARGET_ARCH) -g -ggdb -O -Wno-reorder -Wwrite-strings -Wmain -std=gnu++0x -mstackrealign -static-libgcc -static-libstdc++ -static 
  CXXFLAGS += $(CFLAGS)
  LDFLAGS += -lgdi32 -luser32 -lkernel32 -lcomctl32 -lcomdlg32 -limm32 -lole32 -loleaut32 -lrpcrt4 -lshlwapi -luuid -lversion -lwininet -lwinmm -lws2_32 -lwsock32 $(CSOUND_LIBRARY) -static
  LDDEPS :=
  RESFLAGS := -I $(CSOUND_INCLUDE) -I $(VST_SDK) -D"CABBAGE_HOST=1" -I $(ASIO_SDK) -I $(CSOUND_INCLUDE) -DBUILD_DEBUGGER=1 -D "Cabbage_Build_Standalone=1" -D "CSOUND6=1" -D "_DEBUG=1" -D "Cabbage_GUI_Editor=1" -D "USE_DOUBLE=1"  -D "LINUX=1" -D "DEBUG=1" -D "_DEBUG=1" -D "JUCER_LINUX_MAKE_7346DA2A=1" -I ../../JuceLibraryCode -I ../../JuceLibraryCode/modules
  TARGET := CabbageHost.exe
  BLDCMD = $(CXX) -o $(OUTDIR)/$(TARGET) $(OBJECTS) $(LDFLAGS) $(RESOURCES) -mwindows $(TARGET_ARCH) cabbage32.o -static-libgcc -static-libstdc++ -mstackrealign -static  $(OSCPACK_LIBRARY) 
endif

ifeq ($(CONFIG),Release)
  BINDIR := build
  LIBDIR := build
  OBJDIR := build/host/intermediate/Release
  OUTDIR := build

  ifeq ($(TARGET_ARCH),)
    TARGET_ARCH := -march=pentium4
    endif

  CPPFLAGS := $(DEPFLAGS) -I $(JUCE_LIBRARY_CODE) -D"CABBAGE_HOST=1" -I $(VST_SDK) -I $(ASIO_SDK) -DBUILD_DEBUGGER=1 -I $(CSOUND_INCLUDE) -D__MINGW32__=1 -D__MINGW_EXTENSION=1 -DRELEASE=1 -DJucePlugin_Build_Standalone=1 -DCabbage_GUI_Editor=1 -DCabbage_Build_Standalone=1 -DWIN32 -DJUCE_MINGW=1 -DUSE_DOUBLES=1 -DCSOUND6 -DUSE_DOUBLE=1 -DJUCER_CODEBLOCKS_20734A5D=1
  CFLAGS += $(CPPFLAGS) $(TARGET_ARCH) -O -Wno-reorder -Wwrite-strings -Wmain -std=gnu++0x -mstackrealign -static-libgcc -static-libstdc++ -static 
  CXXFLAGS += $(CFLAGS)
  LDFLAGS += -lgdi32 -luser32 -lkernel32 -lcomctl32 -lcomdlg32 -limm32 -lole32 -loleaut32 -lrpcrt4 -lshlwapi -luuid -lversion -lwininet -lwinmm -lws2_32 -lwsock32 $(CSOUND_LIBRARY) -static
  LDDEPS :=
  RESFLAGS := -I $(CSOUND_INCLUDE) -I $(VST_SDK) -D"CABBAGE_HOST=1" -I $(ASIO_SDK) -I $(CSOUND_INCLUDE) -DBUILD_DEBUGGER=1 -D "Cabbage_Build_Standalone=1" -D "CSOUND6=1" -D "_DEBUG=1" -D "Cabbage_GUI_Editor=1" -D "USE_DOUBLE=1"  -D "LINUX=1" -D "DEBUG=1" -D "_DEBUG=1" -D "JUCER_LINUX_MAKE_7346DA2A=1" -I ../../JuceLibraryCode -I ../../JuceLibraryCode/modules
  TARGET := CabbageHost.exe
  BLDCMD = $(CXX) -o $(OUTDIR)/$(TARGET) $(OBJECTS) $(LDFLAGS) $(RESOURCES) -mwindows $(TARGET_ARCH) cabbage32.o -static-libgcc -static-libstdc++ -mstackrealign -static
endif


OBJECTS := \
  $(OBJDIR)/BinaryData_5ba7f54.o \
  $(OBJDIR)/CabbageLookAndFeel_220a01a6.o \
  $(OBJDIR)/FilterGraph_62e9c017.o \
  $(OBJDIR)/GraphEditorPanel_3dbd4872.o \
  $(OBJDIR)/HostStartup_5ce96f96.o \
  $(OBJDIR)/InternalFilters_beb54bdf.o \
  $(OBJDIR)/CabbageAudioDeviceSelectorComponent_87e6d820.o \
  $(OBJDIR)/MainHostWindow_e920295a.o \
  $(OBJDIR)/CabbagePluginListComponent_e920298a.o \
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


.PHONY: clean

$(OUTDIR)/$(TARGET): $(OBJECTS) $(LDDEPS) $(RESOURCES)
	@echo Linking CabbagePatcher
	-@mkdir -p $(BINDIR)
	-@mkdir -p $(LIBDIR)
	-@mkdir -p $(OUTDIR)
	@$(BLDCMD)

clean:
	@echo Cleaning CabbagePatcher
	@$(CLEANCMD)

strip:
	@echo Stripping CabbagePatcher
	-@strip --strip-unneeded $(OUTDIR)/$(TARGET)

$(OBJDIR)/BinaryData_5ba7f54.o: ../../Source/BinaryData.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling BinaryData.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/CabbageLookAndFeel_220a01a6.o: ../../Source/CabbageLookAndFeel.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling CabbageLookAndFeel.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/FilterGraph_62e9c017.o: ../../Source/Host/FilterGraph.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling FilterGraph.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/GraphEditorPanel_3dbd4872.o: ../../Source/Host/GraphEditorPanel.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling GraphEditorPanel.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/CabbagePluginListComponent_e920298a.o: ../../Source/Host/CabbagePluginListComponent.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling GraphEditorPanel.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/CabbageAudioDeviceSelectorComponent_87e6d820.o: ../../Source/CabbageAudioDeviceSelectorComponent.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling CabbageAudioDeviceSelectorComponent.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/HostStartup_5ce96f96.o: ../../Source/Host/HostStartup.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling HostStartup.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/InternalFilters_beb54bdf.o: ../../Source/Host/InternalFilters.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling InternalFilters.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/MainHostWindow_e920295a.o: ../../Source/Host/MainHostWindow.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling MainHostWindow.cpp"
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

-include $(OBJECTS:%.o=%.d)
