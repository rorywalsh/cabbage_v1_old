# Automatically generated makefile, created by the Introjucer
# Don't edit this file! Your changes will be overwritten when you re-save the Introjucer project!

# (this disables dependency generation if multiple architectures are set)
DEPFLAGS := $(if $(word 2, $(TARGET_ARCH)), , -MMD)

ifndef CONFIG
  CONFIG=Debug
endif

ifeq ($(CONFIG),Debug)
  BINDIR := build
  LIBDIR := build
  OBJDIR := build/intermediate/Debug
  OUTDIR := build

  ifeq ($(TARGET_ARCH),)
    TARGET_ARCH := -march=native
  endif

  CPPFLAGS := $(DEPFLAGS) -D "LINUX=1" -D "DEBUG=1" -D "_DEBUG=1" -D "JUCER_LINUX_MAKE_7346DA2A=1" -D "JUCE_APP_VERSION=1.0.0" -D "JUCE_APP_VERSION_HEX=0x10000" -I /usr/include -I /usr/include/freetype2 -I ../../JuceLibraryCode
  CFLAGS += $(CPPFLAGS) $(TARGET_ARCH) -g -ggdb -O0
  CXXFLAGS += $(CFLAGS)
  LDFLAGS += $(TARGET_ARCH) -L$(BINDIR) -L$(LIBDIR) -L/usr/X11R6/lib/ 
  LDDEPS :=
  RESFLAGS :=  -D "LINUX=1" -D "DEBUG=1" -D "_DEBUG=1" -D "JUCER_LINUX_MAKE_7346DA2A=1" -D "JUCE_APP_VERSION=1.0.0" -D "JUCE_APP_VERSION_HEX=0x10000" -I /usr/include -I /usr/include/freetype2 -I ../../JuceLibraryCode
  TARGET := Cabbage
  BLDCMD = $(CXX) -o $(OUTDIR)/$(TARGET) $(OBJECTS) $(LDFLAGS) $(RESOURCES) $(TARGET_ARCH)
  CLEANCMD = rm -rf $(OUTDIR)/$(TARGET) $(OBJDIR)
endif

ifeq ($(CONFIG),Release)
  BINDIR := build
  LIBDIR := build
  OBJDIR := build/intermediate/Release
  OUTDIR := build

  ifeq ($(TARGET_ARCH),)
    TARGET_ARCH := -march=native
  endif

  CPPFLAGS := $(DEPFLAGS) -D "LINUX=1" -D "NDEBUG=1" -D "JUCER_LINUX_MAKE_7346DA2A=1" -D "JUCE_APP_VERSION=1.0.0" -D "JUCE_APP_VERSION_HEX=0x10000" -I /usr/include -I /usr/include/freetype2 -I ../../JuceLibraryCode
  CFLAGS += $(CPPFLAGS) $(TARGET_ARCH) -Os
  CXXFLAGS += $(CFLAGS)
  LDFLAGS += $(TARGET_ARCH) -L$(BINDIR) -L$(LIBDIR) -fvisibility=hidden -L/usr/X11R6/lib/ 
  LDDEPS :=
  RESFLAGS :=  -D "LINUX=1" -D "NDEBUG=1" -D "JUCER_LINUX_MAKE_7346DA2A=1" -D "JUCE_APP_VERSION=1.0.0" -D "JUCE_APP_VERSION_HEX=0x10000" -I /usr/include -I /usr/include/freetype2 -I ../../JuceLibraryCode
  TARGET := Cabbage
  BLDCMD = $(CXX) -o $(OUTDIR)/$(TARGET) $(OBJECTS) $(LDFLAGS) $(RESOURCES) $(TARGET_ARCH)
  CLEANCMD = rm -rf $(OUTDIR)/$(TARGET) $(OBJDIR)
endif

OBJECTS := \
  $(OBJDIR)/AndroidStandaloneFilterApp_b7642d59.o \
  $(OBJDIR)/BinaryData_5ba7f54.o \
  $(OBJDIR)/CabbageAudioDeviceSelectorComponent_1c91e814.o \
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
  $(OBJDIR)/Table_27a5702f.o \
  $(OBJDIR)/XYPad_6eaa3453.o \
  $(OBJDIR)/XYPadAutomation_2865c48a.o \

.PHONY: clean

$(OUTDIR)/$(TARGET): $(OBJECTS) $(LDDEPS) $(RESOURCES)
	@echo Linking Cabbage
	-@mkdir -p $(BINDIR)
	-@mkdir -p $(LIBDIR)
	-@mkdir -p $(OUTDIR)
	@$(BLDCMD)

clean:
	@echo Cleaning Cabbage
	@$(CLEANCMD)

strip:
	@echo Stripping Cabbage
	-@strip --strip-unneeded $(OUTDIR)/$(TARGET)

$(OBJDIR)/AndroidStandaloneFilterApp_b7642d59.o: ../../Source/AndroidStandalone/AndroidStandaloneFilterApp.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling AndroidStandaloneFilterApp.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/BinaryData_5ba7f54.o: ../../Source/BinaryData.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling BinaryData.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/CabbageAudioDeviceSelectorComponent_1c91e814.o: ../../Source/CabbageAudioDeviceSelectorComponent.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling CabbageAudioDeviceSelectorComponent.cpp"
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

$(OBJDIR)/Table_27a5702f.o: ../../Source/Table.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling Table.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/XYPad_6eaa3453.o: ../../Source/XYPad.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling XYPad.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/XYPadAutomation_2865c48a.o: ../../Source/XYPadAutomation.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling XYPadAutomation.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

-include $(OBJECTS:%.o=%.d)
