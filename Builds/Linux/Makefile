# Automatically generated makefile, created by the Introjucer
# Don't edit this file! Your changes will be overwritten when you re-save the Introjucer project!

ifndef CONFIG
  CONFIG=Debug
endif

ifeq ($(TARGET_ARCH),)
  TARGET_ARCH := -march=native
endif

# (this disables dependency generation if multiple architectures are set)
DEPFLAGS := $(if $(word 2, $(TARGET_ARCH)), , -MMD)

ifeq ($(CONFIG),Debug)
  BINDIR := build
  LIBDIR := build
  OBJDIR := build/intermediate/Debug
  OUTDIR := build
  CPPFLAGS := $(DEPFLAGS) -D "LINUX=1" -D "DEBUG=1" -D "_DEBUG=1" -D "JUCER_LINUX_MAKE_7346DA2A=1" -I /usr/include -I /usr/include/freetype2 -I ../../JuceLibraryCode
  CFLAGS += $(CPPFLAGS) $(TARGET_ARCH) -g -ggdb -O0
  CXXFLAGS += $(CFLAGS) 
  LDFLAGS += -L$(BINDIR) -L$(LIBDIR) -L/usr/X11R6/lib/ 
  LDDEPS :=
  RESFLAGS :=  -D "LINUX=1" -D "DEBUG=1" -D "_DEBUG=1" -D "JUCER_LINUX_MAKE_7346DA2A=1" -I /usr/include -I /usr/include/freetype2 -I ../../JuceLibraryCode
  TARGET := Cabbage
  BLDCMD = $(CXX) -o $(OUTDIR)/$(TARGET) $(OBJECTS) $(LDFLAGS) $(RESOURCES) $(TARGET_ARCH)
endif

ifeq ($(CONFIG),Release)
  BINDIR := build
  LIBDIR := build
  OBJDIR := build/intermediate/Release
  OUTDIR := build
  CPPFLAGS := $(DEPFLAGS) -D "LINUX=1" -D "NDEBUG=1" -D "JUCER_LINUX_MAKE_7346DA2A=1" -I /usr/include -I /usr/include/freetype2 -I ../../JuceLibraryCode
  CFLAGS += $(CPPFLAGS) $(TARGET_ARCH) -Os
  CXXFLAGS += $(CFLAGS) 
  LDFLAGS += -L$(BINDIR) -L$(LIBDIR) -L/usr/X11R6/lib/ 
  LDDEPS :=
  RESFLAGS :=  -D "LINUX=1" -D "NDEBUG=1" -D "JUCER_LINUX_MAKE_7346DA2A=1" -I /usr/include -I /usr/include/freetype2 -I ../../JuceLibraryCode
  TARGET := Cabbage
  BLDCMD = $(CXX) -o $(OUTDIR)/$(TARGET) $(OBJECTS) $(LDFLAGS) $(RESOURCES) $(TARGET_ARCH)
endif

OBJECTS := \
  $(OBJDIR)/csPerfThread_f9d0b9cc.o \
  $(OBJDIR)/CabbageCsoundEditor_d96a9ea1.o \
  $(OBJDIR)/CabbageEditorCommandManager_7b31d637.o \
  $(OBJDIR)/CabbageEditorWindow_16f98d45.o \
  $(OBJDIR)/CabbagePluginEditor_5a11f64e.o \
  $(OBJDIR)/CabbagePluginProcessor_73d6661b.o \
  $(OBJDIR)/CabbageStandaloneDialog_72d272cd.o \
  $(OBJDIR)/StandaloneFilterApp_d9ee3663.o \
  $(OBJDIR)/BinaryData_5ba7f54.o \
  $(OBJDIR)/CabbageGUIClass_79b9049f.o \
  $(OBJDIR)/CabbageLookAndFeel_220a01a6.o \
  $(OBJDIR)/CabbageMainPanel_12c1333.o \
  $(OBJDIR)/CabbageTable_d003e736.o \
  $(OBJDIR)/ComponentLayoutEditor_aa38c835.o \

.PHONY: clean

$(OUTDIR)/$(TARGET): $(OBJECTS) $(LDDEPS) $(RESOURCES)
	@echo Linking Cabbage
	-@mkdir -p $(BINDIR)
	-@mkdir -p $(LIBDIR)
	-@mkdir -p $(OUTDIR)
	@$(BLDCMD)

clean:
	@echo Cleaning Cabbage
	-@rm -f $(OUTDIR)/$(TARGET)
	-@rm -rf $(OBJDIR)/*
	-@rm -rf $(OBJDIR)

strip:
	@echo Stripping Cabbage
	-@strip --strip-unneeded $(OUTDIR)/$(TARGET)

$(OBJDIR)/csPerfThread_f9d0b9cc.o: ../../Source/csPerfThread.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling csPerfThread.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/CabbageCsoundEditor_d96a9ea1.o: ../../Source/Editor/CabbageCsoundEditor.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling CabbageCsoundEditor.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/CabbageEditorCommandManager_7b31d637.o: ../../Source/Editor/CabbageEditorCommandManager.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling CabbageEditorCommandManager.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/CabbageEditorWindow_16f98d45.o: ../../Source/Editor/CabbageEditorWindow.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling CabbageEditorWindow.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/CabbagePluginEditor_5a11f64e.o: ../../Source/Plugin/CabbagePluginEditor.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling CabbagePluginEditor.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/CabbagePluginProcessor_73d6661b.o: ../../Source/Plugin/CabbagePluginProcessor.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling CabbagePluginProcessor.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/CabbageStandaloneDialog_72d272cd.o: ../../Source/Standalone/CabbageStandaloneDialog.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling CabbageStandaloneDialog.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/StandaloneFilterApp_d9ee3663.o: ../../Source/Standalone/StandaloneFilterApp.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling StandaloneFilterApp.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/BinaryData_5ba7f54.o: ../../Source/BinaryData.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling BinaryData.cpp"
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

$(OBJDIR)/CabbageTable_d003e736.o: ../../Source/CabbageTable.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling CabbageTable.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/ComponentLayoutEditor_aa38c835.o: ../../Source/ComponentLayoutEditor.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling ComponentLayoutEditor.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

-include $(OBJECTS:%.o=%.d)
