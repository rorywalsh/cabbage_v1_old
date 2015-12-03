/*
  Copyright (C) 2012 Rory Walsh

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

#include "CodeWindow.h"

#if !defined(Cabbage_Build_Standalone) && !defined(CABBAGE_HOST)
ApplicationProperties* appProperties = nullptr;
#endif

//==============================================================================
CodeWindow::CodeWindow(String name):DocumentWindow (name, Colours::white,
            DocumentWindow::allButtons),
    fontSize(15),
    showOutput(true),
    csoundOutputText(""),
    debugMessage(""),
    firstTime(true),
    font(String("Courier New"), 15, 1),
    isColumnModeEnabled(false),
    isEditModeEnabled(false),
    isInstrTabEnabled(false)
{

#if !defined(Cabbage_Build_Standalone) && !defined(CABBAGE_HOST)
    PropertiesFile::Options options;
    options.applicationName     = "Cabbage";
    options.filenameSuffix      = "settings";
    options.osxLibrarySubFolder = "Preferences";
    appProperties = new ApplicationProperties();
    //set fallback file for default properties...
    appProperties->setStorageParameters (options);
#endif

    setApplicationCommandManagerToWatch(&commandManager);
    commandManager.registerAllCommandsForTarget(this);
    addKeyListener(commandManager.getKeyMappings());

    textEditor = new CsoundCodeEditor(csoundDoc, &csoundToker);
    csoundOutputComponent = new CsoundOutputComponent("Output");
    csoundDebuggerComponent = new CsoundDebuggerComponent("Output");
    restoreWindowStateFromString (appProperties->getUserSettings()->getValue ("mainWindowPos"));

    setSize(1200, 800);

    splitBottomWindow = new SplitComponent(*csoundOutputComponent, *csoundDebuggerComponent, true);
#ifdef CABBAGE_HOST
    splitBottomWindow->SetSplitBarPosition(getWidth()/2);
#else
    splitBottomWindow->SetSplitBarPosition(getWidth()/2);
#endif
    splitWindow = new SplitComponent(*textEditor, *splitBottomWindow, false);
    splitWindow->SetFitToParent(false);
    textEditor->editor[textEditor->currentEditor]->addActionListener(this);
    //splitBottomWindow->SetSplitBarPosition(getWidth());

    this->setTitleBarHeight(20);
    this->setColour(DocumentWindow::backgroundColourId, Colour(20, 20, 20));


    setMenuBar(this, 25);

    setVisible(true);

    this->setWantsKeyboardFocus(false);


    if(!appProperties->getUserSettings()->getValue("EditorColourScheme", var(0)).getIntValue())
        setEditorColourScheme("white");
    else if(appProperties->getUserSettings()->getValue("EditorColourScheme", var(0)).getIntValue())
        setEditorColourScheme("dark");

    RecentlyOpenedFilesList recentFiles;
    recentFiles.restoreFromString (appProperties->getUserSettings()
                                   ->getValue ("recentlyOpenedFiles"));

    csdFile = recentFiles.getFile (0);
    csoundDoc.replaceAllContent(csdFile.loadFileAsString());

    setResizable(true, false);

    String opcodeFile = File(File::getSpecialLocation(File::currentExecutableFile)).getParentDirectory().getFullPathName();
    opcodeFile += "/opcodes.txt";
    Logger::writeToLog(opcodeFile);

    if(File(opcodeFile).existsAsFile())
        textEditor->editor[textEditor->currentEditor]->setOpcodeStrings(File(opcodeFile).loadFileAsString());
    //else csound->Message("Could not open opcodes.txt file, parameter display disabled..");


    bool showConsole = (appProperties->getUserSettings()->getValue("ShowEditorConsole").getIntValue()==1 ? true : false);
    if(showConsole == false)
        splitWindow->SetSplitBarPosition(this->getHeight());
    else
        splitWindow->SetSplitBarPosition(this->getHeight()-(this->getHeight()/4));

    setContentNonOwned(splitWindow, false);
    showEditorConsole();

}

//==============================================================================
CodeWindow::~CodeWindow()
{
#if !defined(Cabbage_Build_Standalone) && !defined(CABBAGE_HOST)
    deleteAndZero(appProperties);
    appProperties = nullptr;
#endif
    setMenuBar(nullptr);
    setApplicationCommandManagerToWatch(nullptr);
    commandManager.deleteInstance();
    deleteAndZero(textEditor);
}
//==============================================================================
void CodeWindow::showEditorConsole()
{
    if(cUtils::getPreference(appProperties, "ShowEditorConsole")==1)
    {
        splitWindow->SetSplitBarPosition(getHeight()-(getHeight()/4));
        //splitBottomWindow->SetSplitBarPosition(getWidth());
#ifdef BUILD_DEBUGGER
        // splitBottomWindow->SetSplitBarPosition(getWidth()/2);
#endif
    }
    else
        splitWindow->SetSplitBarPosition(getHeight());
}
//==============================================================================
StringArray CodeWindow::getMenuBarNames()
{
    const char* const names[] = { "File", "Edit", "View", "Debug", "Help", 0 };
    return StringArray (names);
}

//==============================================================================
void CodeWindow::getAllCommands (Array <CommandID>& commands)
{
    const CommandID ids[] =
    {
        CommandIDs::fileNew,
        CommandIDs::fileOpen,
        CommandIDs::fileSave,
        CommandIDs::fileSaveAs,
        CommandIDs::fileSaveAndClose,
        CommandIDs::fileCloseAux,
        CommandIDs::fileQuit,
        CommandIDs::fileKeyboardShorts,
        CommandIDs::editUndo,
        CommandIDs::editCopy,
        CommandIDs::editCut,
        CommandIDs::editPaste,
        CommandIDs::editRedo,
        CommandIDs::editColumnEdit,
        CommandIDs::editToggleComments,
        CommandIDs::editZoomIn,
        CommandIDs::editZoomOut,
        CommandIDs::editMode,
        CommandIDs::whiteBackground,
        CommandIDs::blackBackground,
        CommandIDs::insertFromRepo,
        CommandIDs::editSearchReplace,
        CommandIDs::addFromRepo,
        CommandIDs::insertRecentEvent,
        CommandIDs::openPythonEditor,
        CommandIDs::viewInstrumentsTabs,
        CommandIDs::viewLinesNumbers,
        CommandIDs::viewOpcodeHelp,
        CommandIDs::viewCsoundHelp,
        CommandIDs::viewCabbageHelp,
        CommandIDs::viewCsoundOutput,
        CommandIDs::setBreakpoint,
        CommandIDs::removeBreakpoint,
        CommandIDs::continueDebug,
        CommandIDs::nextDebug
    };
    commands.addArray (ids, sizeof (ids) / sizeof (ids [0]));
}

//==============================================================================
void CodeWindow::menuItemSelected (int menuItemID, int topLevelMenuIndex)
{
    if (menuItemID >= 100 && menuItemID < 200)
    {
        RecentlyOpenedFilesList recentFiles;
        recentFiles.restoreFromString (appProperties->getUserSettings()
                                       ->getValue ("recentlyOpenedFiles"));

        csdFile = recentFiles.getFile (menuItemID - 100);
        textEditor->editor[textEditor->currentEditor]->getDocument().replaceAllContent(csdFile.loadFileAsString());
        setName(csdFile.getFullPathName());
    }
}

//==============================================================================
void CodeWindow::setFontSize(String zoom)
{

#if defined(WIN32)
    String font = "Consolas";
#elif defined(MACOSX)
    String font = "Courier New";
#else
    String font = "Droid Sans Mono";
#endif

    if(zoom==String("in"))
    {
        textEditor->editor[textEditor->currentEditor]->setFont(Font(font, ++fontSize, 1));
        cUtils::setPreference(appProperties, "FontSize", String(fontSize));
    }
    else
    {
        textEditor->editor[textEditor->currentEditor]->setFont(Font(font, --fontSize, 1));
        cUtils::setPreference(appProperties, "FontSize", String(fontSize));
    }
}

//==============================================================================
void CodeWindow::getCommandInfo (const CommandID commandID, ApplicationCommandInfo& result)
{
    KeyboardShortcutKeys shortcuts(appProperties->getUserSettings()->getXmlValue("KeyboardShortcutXmlData"));
    switch (commandID)
    {
        //file commands
    case CommandIDs::fileNew:
        result.setInfo (String("New"), String("Create a new file"), CommandCategories::file, 0);
        result.addDefaultKeypress ('n', ModifierKeys::commandModifier);
        break;
    case CommandIDs::fileOpen:
#ifdef Cabbage_Build_Standalone
        result.setInfo (String("Open Auxiliary file"), String("Open a file"), CommandCategories::file, 0);
#else
        result.setInfo (String("Open file"), String("Open a file"), CommandCategories::file, 0);
#endif
        result.addDefaultKeypress ('o', ModifierKeys::commandModifier);
        break;
    case CommandIDs::fileSave:
        result.setInfo (String("Save(Update Instrument)"), String("Save a file"), CommandCategories::file, 0);
        result.addDefaultKeypress ('s', ModifierKeys::commandModifier);
        break;
    case CommandIDs::fileSaveAs:
        result.setInfo (String("Save as"), String("Save file as.."), CommandCategories::file, 0);
        result.addDefaultKeypress ('s', ModifierKeys::shiftModifier | ModifierKeys::commandModifier);
        break;
    case CommandIDs::fileSaveAndClose:
        result.setInfo (String("Save and close"), String("Save and close.."), CommandCategories::file, 0);
        result.addDefaultKeypress ('s', ModifierKeys::altModifier | ModifierKeys::commandModifier);
        break;
    case CommandIDs::fileQuit:
        result.setInfo (String("Close editor"), String("Close"), CommandCategories::file, 0);
        result.addDefaultKeypress ('q', ModifierKeys::shiftModifier | ModifierKeys::commandModifier);
        break;
    case CommandIDs::fileCloseAux:
        result.setInfo (String("Close Auxiliary file"), String("Close file"), CommandCategories::file, 0);
        result.addDefaultKeypress ('w', ModifierKeys::commandModifier);
        break;
    case CommandIDs::fileKeyboardShorts:
        result.setInfo (String("Keyboard Shortcuts"), String("Update GUI"), CommandCategories::file, 0);
        break;
        //edit commands
    case CommandIDs::editUndo:
        result.setInfo (String("Undo"), String("Undo last action"), CommandCategories::edit, 0);
        result.addDefaultKeypress ('z', ModifierKeys::commandModifier);
        break;
    case CommandIDs::editRedo:
        result.setInfo (String("Redo"), String("Redo last action"), CommandCategories::edit, 0);
        result.addDefaultKeypress ('z', ModifierKeys::shiftModifier | ModifierKeys::commandModifier);
        break;
    case CommandIDs::editCut:
        result.setInfo (String("Cut"), String("Cut selection"), CommandCategories::edit, 0);
        result.addDefaultKeypress ('x', ModifierKeys::commandModifier);
        break;
    case CommandIDs::editCopy:
        result.setInfo (String("Copy"), String("Copy selection"), CommandCategories::edit, 0);
        result.addDefaultKeypress ('c', ModifierKeys::commandModifier);
        break;
    case CommandIDs::editPaste:
        result.setInfo (String("Paste"), String("Paste selection"), CommandCategories::edit, 0);
        result.addDefaultKeypress ('v', ModifierKeys::commandModifier);
        break;
    case CommandIDs::editColumnEdit:
        result.setInfo (String("Column Edit mode"), String("Column Edit"), CommandCategories::edit, 0);
        result.setTicked(isColumnModeEnabled);
        result.addDefaultKeypress ('l', ModifierKeys::commandModifier);
        break;
    case CommandIDs::editToggleComments:
        result.setInfo (String("Toggle comments"), String("Toggle comments"), CommandCategories::edit, 0);
        result.addDefaultKeypress ('/', ModifierKeys::commandModifier);
        break;
    case CommandIDs::editSearchReplace:
        result.setInfo(String("Search or Replace"), String("Search Replace"), CommandCategories::edit, 0);
        result.addDefaultKeypress ('f', ModifierKeys::commandModifier);
        break;
    case CommandIDs::editZoomIn:
        result.setInfo (String("Zoom in"), String("Zoom in"), CommandCategories::edit, 0);
        result.addDefaultKeypress('[', ModifierKeys::commandModifier);
        break;
    case CommandIDs::editZoomOut:
        result.setInfo (String("Zoom out"), String("Zoom out"), CommandCategories::edit, 0);
        result.addDefaultKeypress (']', ModifierKeys::commandModifier);
        break;
    case CommandIDs::editMode:
        result.setInfo (String("Edit mode"), String("Edit Mode"), CommandCategories::edit, 0);
        result.setTicked(isEditModeEnabled);
        result.addDefaultKeypress ('e', ModifierKeys::commandModifier);
        break;
    case CommandIDs::whiteBackground:
        result.setInfo (String("White background"), String("White scheme"), CommandCategories::edit, 0);
        break;
    case CommandIDs::blackBackground:
        result.setInfo (String("Dark background"), String("Dark scheme"), CommandCategories::edit, 0);
        break;
    case CommandIDs::insertFromRepo:
        result.setInfo (String("Insert from repo"), String("Insert from repo"), CommandCategories::edit, 0);
        //result.defaultKeypresses.add (KeyPress::createFromDescription(shortcuts.getKeyPress("InsertFromRepo")));
        //result.addDefaultKeypress (KeyPress::createFromDescription(shortcuts.getKeyPress("InsertFromRepo")));
        break;
    case CommandIDs::addFromRepo:
        result.setInfo (String("Add from repo"), String("Add from repo"), CommandCategories::edit, 0);
        result.addDefaultKeypress ('j', ModifierKeys::commandModifier);
        break;
    case CommandIDs::insertRecentEvent:
        result.setInfo (String("Insert recent event"), String("Insert recent event"), CommandCategories::edit, 0);
        result.addDefaultKeypress ('e', ModifierKeys::commandModifier | ModifierKeys::altModifier);
        break;
    case CommandIDs::openPythonEditor:
        result.setInfo (String("Insert Python code"), String("Insert Python code"), CommandCategories::edit, 0);
        result.addDefaultKeypress ('p', ModifierKeys::commandModifier | ModifierKeys::altModifier);
        break;

    case CommandIDs::viewInstrumentsTabs:
        result.setInfo (String("Show instruments tabs"), String("Show Instruments tabs"), CommandCategories::view, 0);
        result.addDefaultKeypress ('i', ModifierKeys::commandModifier);
        result.setTicked(isInstrTabEnabled);
        break;
    case CommandIDs::viewLinesNumbers:
        result.setInfo (String("Show line numbers"), String("Show line numbers"), CommandCategories::view, 0);
        break;
    case CommandIDs::viewOpcodeHelp:
        result.setInfo (String("Show opcode help"), String("Show opcode help"), CommandCategories::view, 0);
        break;

    case CommandIDs::viewCsoundHelp:
#ifndef MACOSX
        result.setInfo (String("View Csound Manual"), String("View Csound Manual"), CommandCategories::help, 0);
        result.defaultKeypresses.add(KeyPress(KeyPress::F1Key));
#else
        result.setInfo (String("View Csound Manual"), String("View Csound Manual"), CommandCategories::help, 0);
        result.addDefaultKeypress ('1', ModifierKeys::commandModifier);
#endif
        break;


    case CommandIDs::setBreakpoint:
        result.setInfo (String("Add Breakpoint"), String("Add Breakpoint"), CommandCategories::debug, 0);
        result.addDefaultKeypress ('b', ModifierKeys::commandModifier);
        break;

    case CommandIDs::removeBreakpoint:
        result.setInfo (String("Remove Breakpoint"), String("Remove Breakpoint"), CommandCategories::debug, 0);
        result.addDefaultKeypress ('b', ModifierKeys::commandModifier | ModifierKeys::shiftModifier);
        break;

    case CommandIDs::continueDebug:
#ifndef MACOSX
        result.setInfo (String("Continue"), String("Continue"), CommandCategories::debug, 0);
        result.defaultKeypresses.add(KeyPress(KeyPress::F5Key));
#else
        result.setInfo (String("Continue"), String("Continue"), CommandCategories::debug, 0);
        result.addDefaultKeypress ('5', ModifierKeys::commandModifier);
#endif
        break;

    case CommandIDs::nextDebug:
#ifndef MACOSX
        result.setInfo (String("Next"), String("Next"), CommandCategories::debug, 0);
        result.defaultKeypresses.add(KeyPress(KeyPress::F10Key));
#else
        result.setInfo (String("Next"), String("Next"), CommandCategories::debug, 0);
        result.addDefaultKeypress ('7', ModifierKeys::commandModifier);
#endif
        break;

    case CommandIDs::viewCabbageHelp:
        result.setInfo (String("View Cabbage Manual"), String("View Cabbage Manual"), CommandCategories::help, 0);
        result.defaultKeypresses.add(KeyPress(KeyPress::F1Key));
        break;
    case CommandIDs::viewCsoundOutput:
        result.setInfo (String("View Csound Output"), String("View Csound Output"), CommandCategories::help, 0);
        result.addDefaultKeypress ('p', ModifierKeys::commandModifier);
        bool isTicked = (appProperties->getUserSettings()->getValue("ShowEditorConsole").getIntValue()==1 ? true : false);
        result.setTicked(isTicked);
        break;
    }
}

//==============================================================================
PopupMenu CodeWindow::getMenuForIndex (int topLevelMenuIndex, const String& menuName)
{
    PopupMenu m1, m2;
    m1.clear();
    m2.clear();
    m2.setLookAndFeel(&getLookAndFeel());
    if(topLevelMenuIndex==0)
    {
        //m1.addCommandItem(&commandManager, CommandIDs::fileNew);
        m1.addCommandItem(&commandManager, CommandIDs::fileOpen);
        m1.addCommandItem(&commandManager, CommandIDs::fileCloseAux);

        //RecentlyOpenedFilesList recentFiles;
        //recentFiles.restoreFromString (appProperties->getUserSettings()
        //                                       ->getValue ("recentlyOpenedFiles"));

        //PopupMenu recentFilesMenu;
        //recentFiles.createPopupMenuItems (recentFilesMenu, 100, true, true);
        //m1.addSubMenu ("Open recent file", recentFilesMenu);

        m1.addCommandItem(&commandManager, CommandIDs::fileSave);
        m1.addCommandItem(&commandManager, CommandIDs::fileSaveAs);
        m1.addCommandItem(&commandManager, CommandIDs::fileSaveAndClose);
        m1.addCommandItem(&commandManager, CommandIDs::fileQuit);
        return m1;
    }

    else if(topLevelMenuIndex==1)
    {
        m1.addCommandItem(&commandManager, CommandIDs::editUndo);
        m1.addCommandItem(&commandManager, CommandIDs::editRedo);
        m1.addCommandItem(&commandManager, CommandIDs::editCut);
        m1.addCommandItem(&commandManager, CommandIDs::editCopy);
        m1.addCommandItem(&commandManager, CommandIDs::editPaste);
        m1.addSeparator();
#ifdef CABBAGE_HOST
        m1.addCommandItem(&commandManager, CommandIDs::editMode);
        m1.addSeparator();
#endif
        m1.addCommandItem(&commandManager, CommandIDs::editToggleComments);
        m1.addCommandItem(&commandManager, CommandIDs::editSearchReplace);
        m1.addCommandItem(&commandManager, CommandIDs::editColumnEdit);
        m1.addSeparator();
        m2.addCommandItem(&commandManager, CommandIDs::editZoomIn);
        m2.addCommandItem(&commandManager, CommandIDs::editZoomOut);
        m1.addSubMenu(String("Font Size"), m2);
        m2.clear();
        m2.addCommandItem(&commandManager, CommandIDs::whiteBackground);
        m2.addCommandItem(&commandManager, CommandIDs::blackBackground);
        m1.addSubMenu("Change editor theme", m2);
        //m1.addCommandItem(&commandManager, CommandIDs::insertFromRepo);
        //m1.addCommandItem(&commandManager, CommandIDs::addFromRepo);
        //m1.addCommandItem(&commandManager, CommandIDs::insertRecentEvent);
        //m1.addCommandItem(&commandManager, CommandIDs::openPythonEditor);
        return m1;
    }

    else if(topLevelMenuIndex==2)
    {
        //m1.addCommandItem(&commandManager, CommandIDs::viewInstrumentsTabs);
        m1.addCommandItem(&commandManager, CommandIDs::viewCsoundOutput);
        //m1.addCommandItem(&commandManager, CommandIDs::viewLinesNumbers);
        //m1.addCommandItem(&commandManager, CommandIDs::viewOpcodeHelp);
        //m1.addCommandItem(&commandManager, CommandIDs::commOrcUpdateChannel);
        //m1.addCommandItem(&commandManager, CommandIDs::commOrchUpdateMultiLine);
        //m1.addCommandItem(&commandManager, CommandIDs::commOrchUpdateSingleLine);
        //m1.addCommandItem(&commandManager, CommandIDs::commScoUpdateMultiLine);
        //m1.addCommandItem(&commandManager, CommandIDs::commScoUpdateSingleLine);

        return m1;
    }

    else if(topLevelMenuIndex==3)
    {
        m1.addCommandItem(&commandManager, CommandIDs::setBreakpoint);
        m1.addCommandItem(&commandManager, CommandIDs::removeBreakpoint);
        m1.addCommandItem(&commandManager, CommandIDs::continueDebug);
        m1.addCommandItem(&commandManager, CommandIDs::nextDebug);
        return m1;
    }

    else if(topLevelMenuIndex==4)
    {
        m1.addCommandItem(&commandManager, CommandIDs::viewCsoundHelp);
        m1.addCommandItem(&commandManager, CommandIDs::viewCabbageHelp);
        return m1;
    }

    else return m1;
}

//==============================================================================
bool CodeWindow::perform (const InvocationInfo& info)
{
    Logger::writeToLog(String(info.commandID));
    //---------------------------------------------------------------------------------------------
    if(info.commandID==CommandIDs::fileNew)
    {
        String tempFile = File::getSpecialLocation(File::userHomeDirectory).getFullPathName()+"/liveCodeSession.csd";
        Logger::writeToLog(tempFile);
        csdFile = tempFile;
        csoundDoc.replaceAllContent(csdFile.loadFileAsString());
        toggleTextWindows();
    }
    else if(info.commandID==CommandIDs::fileSave)
    {
        Logger::writeToLog("fileSaved");
        if(textEditor->currentEditor!=0)
        {
            cUtils::showMessage("Saving an auxillary file!");
            textEditor->saveAuxFile();
        }
        else
        {
            isEditModeEnabled=false;
            sendActionMessage("fileSaved");
        }
    }
    else if(info.commandID==CommandIDs::fileSaveAndClose)
    {
        Logger::writeToLog("fileSaved");
        if(textEditor->currentEditor!=0)
        {
            cUtils::showMessage("Saving an auxillary file!");
            textEditor->saveAuxFile();
        }
        else
        {
            isEditModeEnabled=false;
            sendActionMessage("fileSaveAndClose");
        }
    }
    else if(info.commandID==CommandIDs::fileSaveAs)
    {
        Logger::writeToLog("fileSaveAs");
        sendActionMessage("fileSaveAs");
    }
    else if(info.commandID==CommandIDs::fileOpen)
    {
#ifdef Cabbage_Build_Standalone
        FileChooser openFC(String("Open a file..."), File::nonexistent, String("*.csd;*.py;*.txt"), UseNativeDialogue);
        if(openFC.browseForFileToOpen())
        {
            Logger::writeToLog(openFC.getResult().getFullPathName());
            textEditor->addNewFile(openFC.getResult());
            setColourScheme("dark");
            this->setName(openFC.getResult().getFullPathName());
            textEditor->showTab(openFC.getResult().getFileName());
        }
#else
        sendActionMessage("open file");
#endif

    }
    else if(info.commandID==CommandIDs::fileQuit)
    {
        //JUCEApplication::getInstance()->systemRequestedQuit();
        textEditor->saveAllFiles();
    }

    else if(info.commandID==CommandIDs::editUndo)
    {
        textEditor->editor[textEditor->currentEditor]->undo();
    }

    else if(info.commandID==CommandIDs::fileKeyboardShorts)
    {
        DialogWindow::LaunchOptions o;
        o.content.setOwned(new ShortcutsPanel());
        o.dialogTitle                   = TRANS("Keyboard Shortcuts");
        o.dialogBackgroundColour        = Colours::black;
        o.resizable                     = false;
        o.useNativeTitleBar				= false;
        o.escapeKeyTriggersCloseButton  = true;
        //o.content->setLookAndFeel(&this->getLookAndFeel());
        o.launchAsync();
    }

    else if(info.commandID==CommandIDs::fileCloseAux)
    {
        textEditor->closeCurrentFile();
    }

    else if(info.commandID==CommandIDs::editCut)
    {
        textEditor->editor[textEditor->currentEditor]->cutToClipboard();
    }
    else if(info.commandID==CommandIDs::editCopy)
    {
        textEditor->editor[textEditor->currentEditor]->copyToClipboard();
    }
    else if(info.commandID==CommandIDs::editPaste)
    {
        textEditor->editor[textEditor->currentEditor]->insertText(SystemClipboard::getTextFromClipboard());
        //textEditor->editor[textEditor->currentEditor]->pasteFromClipboard();
    }

    else if(info.commandID==CommandIDs::editRedo)
    {
        textEditor->editor[textEditor->currentEditor]->redo();
    }
    else if(info.commandID==CommandIDs::editToggleComments)
    {
        textEditor->editor[textEditor->currentEditor]->toggleComments();
    }
    else if(info.commandID==CommandIDs::editSearchReplace)
    {
        textEditor->enableColumnEdit(false);
        textEditor->searchReplaceComp->setSearchText(textEditor->getCurrentSelectedText());
        textEditor->searchReplaceComp->setVisible(true);
        textEditor->helpComp->setVisible(false);
        textEditor->searchReplaceComp->grabKeyboardFocus();
    }
    else if(info.commandID==CommandIDs::editZoomIn)
    {
        setFontSize("in");
    }
    else if(info.commandID==CommandIDs::editColumnEdit)
    {
        if(isColumnModeEnabled)
            textEditor->enableColumnEdit(false);
        else
            textEditor->enableColumnEdit(true);

        isColumnModeEnabled=!isColumnModeEnabled;

    }
    else if(info.commandID==CommandIDs::editMode)
    {
        isEditModeEnabled=!isEditModeEnabled;
        if(isEditModeEnabled)
            sendActionMessage("enableEditMode");
        else
            sendActionMessage("disableEditMode");

    }
    else if(info.commandID==CommandIDs::editZoomOut)
    {
        setFontSize("out");
    }
    else if(info.commandID==CommandIDs::whiteBackground)
    {
        setEditorColourScheme("white");
    }

    else if(info.commandID==CommandIDs::blackBackground)
    {
        setEditorColourScheme("dark");
    }

    else if(info.commandID==CommandIDs::insertRecentEvent)
    {

    }
    else if(info.commandID==CommandIDs::insertFromRepo)
    {
        CodeWindow::insertFromRepo();
    }

    else if(info.commandID==CommandIDs::openPythonEditor)
    {
        /*if(pythonEditor==nullptr){
        	pythonEditor = new PythonEditor("Python Editor");
        	//pythonEditor->textEditor->setLookAndFeel(lookAndFeel);
        	//pythonEditor->setLookAndFeel(lookAndFeel);
        	pythonEditor->addActionListener(this);
        	pythonEditor->setAlwaysOnTop(true);
        	pythonEditor->setVisible(true);
        	pythonEditor->textEditor->setColour(CodeEditorComponent::backgroundColourId, Colour::fromRGB(40, 40, 40));
        }

        pythonEditor->toFront(true);
        cabbageTimer->startTimedEvent(1, "pythonFocus");
        //pythonEditor->textEditor->highlightLine("oscil 1");

         */
    }

    else if(info.commandID==CommandIDs::viewInstrumentsTabs)
    {
        if(isInstrTabEnabled)
            textEditor->showInstrs(false);
        else
            textEditor->showInstrs(true);

        isInstrTabEnabled=!isInstrTabEnabled;
    }

    else if(info.commandID==CommandIDs::viewLinesNumbers)
    {
        Logger::writeToLog("show line numbers");
    }

    else if(info.commandID==CommandIDs::continueDebug)
    {
        sendActionMessage("ContinueDebug");
    }

    else if(info.commandID==CommandIDs::setBreakpoint)
    {
        textEditor->editor[0]->modifyInstrumentBreakpoint(false);
    }
    else if(info.commandID==CommandIDs::removeBreakpoint)
    {
        textEditor->editor[0]->modifyInstrumentBreakpoint(true);
    }

    else if(info.commandID==CommandIDs::nextDebug)
    {
        sendActionMessage("NextDebug");
    }

    else if(info.commandID==CommandIDs::addFromRepo)
    {
        textEditor->editor[textEditor->currentEditor]->addToRepository();
    }

    else if(info.commandID==CommandIDs::viewCsoundOutput)
    {
        if(appProperties->getUserSettings()->getValue("ShowEditorConsole").getIntValue()==1)
        {
            splitWindow->SetSplitBarPosition(this->getHeight());
            appProperties->getUserSettings()->setValue("ShowEditorConsole", "0");
        }

        else
        {
            splitWindow->SetSplitBarPosition(this->getHeight()-(this->getHeight()/4));
            //cabbageCsoundEditor->splitWindow->SetSplitBarPosition(cabbageCsoundEditor->getHeight()-(cabbageCsoundEditor->getHeight()/4));
#ifdef BUILD_DEBUGGER
            splitBottomWindow->SetSplitBarPosition(getWidth()/2);
#endif
            appProperties->getUserSettings()->setValue("ShowEditorConsole", "1");
        }
    }

    else if(info.commandID==CommandIDs::viewCsoundHelp)
    {
        sendActionMessage("toggleCsoundOutput");
        sendActionMessage("hideOutputWindow");
        toggleManuals("Csound");
    }

    else if(info.commandID==CommandIDs::viewCabbageHelp)
    {
        sendActionMessage("toggleCsoundOutput");
        sendActionMessage("hideOutputWindow");
        toggleManuals("Cabbage");
    }

    return true;
}

//=======================================================
// toggle manuals
//=======================================================
void CodeWindow::toggleManuals(String manual)
{

    if(manual=="Cabbage")
    {
        showCabbageHelp();
        return;
    }

    String helpDir;
    helpDir = appProperties->getUserSettings()->getValue("CsoundHelpDir", "");


    if(helpDir.length()<2)
        cUtils::showMessage("Please set the Csound manual directory in the Preference menu", &getLookAndFeel());
    else
    {
        CodeDocument::Position pos1, pos2;
        pos1 = textEditor->editor[textEditor->currentEditor]->getDocument().findWordBreakBefore(textEditor->editor[textEditor->currentEditor]->getCaretPos());
        pos2 = textEditor->editor[textEditor->currentEditor]->getDocument().findWordBreakAfter(textEditor->editor[textEditor->currentEditor]->getCaretPos());
        String opcode = csoundDoc.getTextBetween(pos1, pos2);
        int lineIndex = textEditor->editor[textEditor->currentEditor]->getCaretPos().getLineNumber();
        if(textEditor->getCabbageSectionRange().contains(lineIndex))
        {
            showCabbageHelp();
            return;
        }
#if defined(LINUX) || defined(MACOSX)
        URL urlCsound(helpDir+"/"+opcode.trim()+String(".html"));
        String homePage = helpDir+"/index.html";
#else
        URL urlCsound(helpDir+"\\"+opcode.trim()+String(".html"));
        String homePage = helpDir+"\\index.html";
#endif
        String urlCabbage;
        ChildProcess process;
        File temp1(urlCsound.toString(false));
        if(temp1.exists())
        {
#ifdef LINUX
            if(!process.start(String("xdg-open "+urlCsound.toString(false)).toUTF8()))
                cUtils::showMessage("Couldn't show file, see 'Set Csound manual directory' in Options->Preferences", &getLookAndFeel());
#else
            urlCsound.launchInDefaultBrowser();
#endif
        }
        else
        {
            if(File(homePage).existsAsFile())
            {
#ifdef LINUX
                if(!process.start("xdg-open "+homePage))
                    cUtils::showMessage("Couldn't show file, see 'Set Csound manual directory' in Options->Preferences", &getLookAndFeel());
#else
                URL(homePage).launchInDefaultBrowser();
#endif
            }
            else
                cUtils::showMessage("Couldn't show file, see 'Set Csound manual directory' in Options->Preferences", &getLookAndFeel());

        }


    }
}
//==============================================================================
void CodeWindow::showCabbageHelp()
{
    String path;
    StringArray tokens;
    tokens.addTokens(textEditor->editor[textEditor->currentEditor]->getLineText(), false);
#if defined(LINUX) || defined(MACOSX)
    path = File::getSpecialLocation(File::currentExecutableFile).getParentDirectory().getFullPathName()+"/Docs/_book/"+tokens[0]+".html";
#else
    path = File::getSpecialLocation(File::currentExecutableFile).getParentDirectory().getFullPathName()+"\\Docs\\_book\\"+tokens[0]+".html";
#endif





    if(!File(path).existsAsFile())
        cUtils::showMessage(
            "Could not find Cabbage manual. Make sure\n\
	it is located in the Docs folder in the same\n\
	directory as the main Cabbage executable", &getLookAndFeel());
    else
    {
        ChildProcess process;
        File temp1(path);
        if(temp1.exists())
        {
#ifdef LINUX
            if(!process.start("xdg-open "+path))
                cUtils::showMessage("Couldn't show file", &getLookAndFeel());
#else
            URL(path).launchInDefaultBrowser();
#endif
        }
    }
}
//==============================================================================
void CodeWindow::toggleTextWindows()
{

}
//==============================================================================
void CodeWindow::setEditorColourScheme(String theme)
{
    setColourScheme(theme);
}
//==============================================================================
void CodeWindow::actionListenerCallback(const String &message)
{
    if(message=="Launch help")
        toggleManuals("Csound");
    sendActionMessage(message);
}
//==============================================================================
void CodeWindow::setColourScheme(String theme)
{
    if(theme=="white")
    {
        textEditor->editor[textEditor->currentEditor]->setColourScheme(csoundToker.getDefaultColourScheme());
        textEditor->editor[textEditor->currentEditor]->setColour(CodeEditorComponent::backgroundColourId, Colours::white);
        textEditor->editor[textEditor->currentEditor]->setColour(CaretComponent::caretColourId, Colours::black);
        textEditor->editor[textEditor->currentEditor]->setColour(CodeEditorComponent::highlightColourId, Colours::lime);
        appProperties->getUserSettings()->setValue("EditorColourScheme", 0);
        repaint();
    }
    else if(theme=="dark")
    {
        textEditor->editor[textEditor->currentEditor]->setColourScheme(csoundToker.getDarkColourScheme());
        textEditor->editor[textEditor->currentEditor]->setColour(CaretComponent::caretColourId, Colours::white);
#ifdef CABBAGE_HOST
        textEditor->editor[textEditor->currentEditor]->setColour(CodeEditorComponent::backgroundColourId, Colour::fromRGB(30, 30, 30));
#else
        textEditor->editor[textEditor->currentEditor]->setColour(CodeEditorComponent::backgroundColourId, Colour::fromRGB(30, 30, 30));
#endif
        textEditor->editor[textEditor->currentEditor]->setColour(CodeEditorComponent::highlightColourId, Colours::green.withAlpha(.6f));
        appProperties->getUserSettings()->setValue("EditorColourScheme", 1);
    }
}

//==============================================================================
void CodeWindow::timerCallback()
{

}
//==============================================================================
void CodeWindow::insertFromRepo()
{
    int repoIndex = 1;
    PopupMenu m;
    StringArray repoEntries;
    m.setLookAndFeel(&this->getLookAndFeel());
    ScopedPointer<XmlElement> xmlElement;
    xmlElement = appProperties->getUserSettings()->getXmlValue("CopeRepoXmlData");

    forEachXmlChildElement (*xmlElement, e)
    {
        m.addItem(repoIndex, e->getTagName());
        repoEntries.add(e->getTagName());
        repoIndex++;
    }


    const int result = m.showAt(getCaretScreenPosition());

    forEachXmlChildElement (*xmlElement, e)
    {
        if(e->getTagName()==repoEntries[result-1])
            textEditor->editor[textEditor->currentEditor]->insertText(e->getAllSubText());
    }
    xmlElement = nullptr;
}

//===============================================================================
void CodeWindow::newFile(String type)
{
    String untitledCSD;
    if(type=="effect")
    {
        untitledCSD=
            "<Cabbage>\n"
            "form caption(\"Untitled\") size(400, 300), colour(58, 110, 182), pluginID(\"def1\")\n"
            "rslider bounds(296, 162, 100, 100), channel(\"gain\"), text(\"Gain\"), trackercolour(\"lime\"), outlinecolour(0, 0, 0, 50), textcolour(\"black\")\n"
            "\n"
            "</Cabbage>\n"
            "<CsoundSynthesizer>\n"
            "<CsOptions>\n"
            "-n -d -+rtmidi=NULL -M0 -m0d \n"
            "</CsOptions>\n"
            "<CsInstruments>\n"
            "; Initialize the global variables. \n"
            "sr = 44100\n"
            "ksmps = 32\n"
            "nchnls = 2\n"
            "0dbfs = 1\n"
            "\n"
            "\n"
            "instr 1\n"
            "kGain chnget \"gain\"\n"
            "\n"
            "a1 inch 1\n"
            "a2 inch 2\n"
            "\n"
            "outs a1*kGain, a2*kGain\n"
            "endin\n"
            "\n"
            "</CsInstruments>\n"
            "<CsScore>\n"
            ";causes Csound to run for about 7000 years...\n"
            "f0 z\n"
            ";starts instrument 1 and runs it for a week\n"
            "i1 0 [60*60*24*7] \n"
            "</CsScore>\n"
            "</CsoundSynthesizer>\n"
            "";
    }
    else if(type=="instrument")
    {
        untitledCSD=
            "<Cabbage>\n"
            "form caption(\"Untitled\") size(400, 300), colour(58, 110, 182), pluginID(\"def1\")\n"
            "keyboard bounds(8, 158, 381, 95)\n"
            "</Cabbage>\n"
            "<CsoundSynthesizer>\n"
            "<CsOptions>\n"
            "-n -d -+rtmidi=NULL -M0 -m0d --midi-key-cps=4 --midi-velocity-amp=5\n"
            "</CsOptions>\n"
            "<CsInstruments>\n"
            "; Initialize the global variables. \n"
            "sr = 44100\n"
            "ksmps = 32\n"
            "nchnls = 2\n"
            "0dbfs = 1\n"
            "\n"
            ";instrument will be triggered by keyboard widget\n"
            "instr 1\n"
            "kEnv madsr .1, .2, .6, .4\n"
            "aOut vco2 p5, p4\n"
            "outs aOut*kEnv, aOut*kEnv\n"
            "endin\n"
            "\n"
            "</CsInstruments>\n"
            "<CsScore>\n"
            ";causes Csound to run for about 7000 years...\n"
            "f0 z\n"
            "</CsScore>\n"
            "</CsoundSynthesizer>\n"
            "";
    }
    csoundDoc.replaceAllContent(untitledCSD);
    unSaved = true;
    sendActionMessage("fileSavedAs");
}
