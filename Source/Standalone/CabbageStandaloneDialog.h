/*
  Copyright (C) 2009 Rory Walsh

  Cabbage is free software; you can redistribute it
  and/or modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  Cabbage is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with Csound; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
  02111-1307 USA
*/

#ifndef __JUCE_STANDALONEFILTERWINDOW_JUCEHEADER__
#define __JUCE_STANDALONEFILTERWINDOW_JUCEHEADER__

#include "../Editor/CodeWindow.h"
#include "../CabbageUtils.h"
#include "../Plugin/CabbagePluginProcessor.h"
#include "../Plugin/CabbagePluginEditor.h"
#include "../CabbageAudioDeviceSelectorComponent.h"

extern ApplicationProperties* appProperties;
extern PropertySet* defaultPropSet;
extern String currentApplicationDirectory;
extern StringArray undoHistory;
class StandaloneFileDialogue;

//==============================================================================
class CsoundMessageConsole : public DocumentWindow
{
    ScopedPointer<TextEditor> textEditor;
public:
    CsoundMessageConsole(String title, Colour bgcolour, int top, int left): DocumentWindow("Csound output console", bgcolour, DocumentWindow::allButtons, true)
    {
        textEditor = new TextEditor();
        textEditor->setColour(Label::outlineColourId, Colours::white);
        textEditor->setColour(TextEditor::backgroundColourId, Colours::black);
        textEditor->setColour(TextEditor::textColourId, Colours::cornflowerblue);
        textEditor->setMultiLine(true);
        textEditor->setFont(Font("Arial", 14, 0));
        setSize(700, 200);
        setTopLeftPosition(top, left);
        setAlwaysOnTop(true);
        setResizable(true, false);
        this->setTitleBarHeight(20);
        textEditor->setBounds(0, 30, getWidth(), getHeight());
        setContentOwned(textEditor, true);
        setAlpha(.8f);
    };

    ~CsoundMessageConsole() {};

    void closeButtonPressed()
    {
        setVisible(false);
    }

    void setText(String text)
    {
        textEditor->setColour(TextEditor::textColourId, Colours::cornflowerblue);
        textEditor->setText(text);
        textEditor->setCaretPosition(textEditor->getText().length());
    }

    String getText()
    {
        return textEditor->getText();
    }

};

//==============================================================================
// This is the main host window. It gets instatiated in StandaloneFilterApp.cpp
//==============================================================================
class StandaloneFilterWindow    :   public DocumentWindow,
    public Button::Listener,
    public ChangeListener,
    public CabbageUtils,
    public Timer,
    public ActionListener,
    public KeyListener
{
public:
    //==============================================================================
    StandaloneFilterWindow (const String& title,
                            const Colour& backgroundColour, String commandLine);

    ~StandaloneFilterWindow();
    //=================================================================
    void resetFilter(bool shouldResetFilter);
    void saveState();
    long cabbageFindPluginID(unsigned char *buf, size_t len, const char *s);
    void loadState();
    virtual void showAudioSettingsDialog();
    virtual PropertySet* getGlobalSettings();

    bool keyPressed(const juce::KeyPress &,Component *);
    //==============================================================================
    void closeButtonPressed();
    void buttonClicked (Button*);
    void showInfoWindow(String text);
    void resized();
    void changeListenerCallback(ChangeBroadcaster *source);
    void actionListenerCallback (const String& message);
    void openFile(String _csdfile);
    void saveFile();
    void saveFileAs();
    void showEditorConsole();
    void setupWindowDimensions();
    void m_ShowMessage(String mess, LookAndFeel* feel, String title="Cabbage");
    int exportPlugin(String type, bool saveAs, String fileName="");
    Array<File> cabbageFiles;
    ScopedPointer<StandaloneFileDialogue> standaloneFileDialogue;

    bool isGuiEnabled()
    {
        return isGUIOn;
    }

    void setGuiEnabled(bool onoff)
    {
        isGUIOn = onoff;
    }

    void setCurrentLine(int line)
    {
        currentLine=line;
    }

    int getCurrentLine()
    {
        return currentLine;
    }

private:
    AudioDeviceManager::AudioDeviceSetup audioDeviceSetup;
    bool updateEditorOutputConsole;
	bool isUsingExternalEditor;
    Time lastSaveTime;
    void openTextEditor();
    bool standaloneMode;
    bool cabbageDance;
    bool timerRunning;
    bool AudioEnabled;
    bool isAFileOpen;
    bool hasEditorBeingOpened;
    int setUniquePluginID(File inFile, File csdFile, bool AU);
    float yAxis;
    void timerCallback();
    ScopedPointer<CabbagePluginAudioProcessor> filter;
    ScopedPointer<AudioDeviceManager> deviceManager;
    void sendMessageToWinXound(String messageType, String message);
    void sendMessageToWinXound(String messageType, int value);
    void batchProcess(String pluginType, bool dir);
    AudioProcessorPlayer player;
    TextButton optionsButton;
    void deleteFilter();
    File csdFile, originalCsdFile;
    bool isGUIOn;
    int currentLine;
    bool pipeOpenedOk;
    ScopedPointer<CabbageLookAndFeel> lookAndFeel;
    ScopedPointer<LookAndFeel> oldLookAndFeel;
    ScopedPointer<CodeWindow> cabbageCsoundEditor;
    String consoleMessages;
    ScopedPointer<CsoundMessageConsole> outputConsole;
    StringArray previousScoreEvents;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StandaloneFilterWindow);
};

//==============================================================================
// Dialogue for choosing files if in standalone mode..
//==============================================================================
class StandaloneFileDialogue : public DocumentWindow
{
public:
//container for file combo selector
    class MainComponent : public Component,
        public ComboBox::Listener,
        public ActionBroadcaster
    {
    public:
        ScopedPointer<ComboBox> combo;
        MainComponent()
        {
            addAndMakeVisible(combo = new ComboBox());
            combo->setText("Please select a .csd file");
            combo->addListener(this);
        }
        void resized()
        {
            combo->setBounds(5, 5, 240, 20);
        }

        void comboBoxChanged (ComboBox *combo)
        {
            sendActionMessage("FileChooserDialog:"+String(combo->getSelectedId()));
        }

        void paint(Graphics &g)
        {
            g.fillAll(Colours::black);
        }
    };

    ScopedPointer<MainComponent> mainComponent;
    Array<File> cabbageFiles;
    StandaloneFileDialogue(String title, Colour bgcolour):
        DocumentWindow(title, bgcolour, DocumentWindow::allButtons, true)
    {
        centreWithSize(250, 30);
        setResizeLimits(250, 30, 32768, 32768);
        mainComponent = new MainComponent();
        mainComponent->setBounds(0, 0, 250, 30);
        setAlwaysOnTop(true);
        this->setTitleBarHeight(20);
        setContentOwned(mainComponent, true);
    }

    void addItemsToCombo(Array<File> files)
    {
        cabbageFiles = files;
        for(int i=0; i<files.size(); i++)
            mainComponent->combo->addItem(files[i].getFileName(), i+1);
    }

    void setCurrentFile(String file)
    {
        for(int i=0; i<cabbageFiles.size(); i++)
            if(cabbageFiles[i].getFileName()==file)
                mainComponent->combo->setSelectedItemIndex(i+1);
    }

    void closeButtonPressed()
    {
        setVisible(false);
    }


};

#endif   // __JUCE_STANDALONEFILTERWINDOW_JUCEHEADER__
