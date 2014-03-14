#ifndef __MAINCOMPONENT_H_615ECE56__
#define __MAINCOMPONENT_H_615ECE56__

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

#include "CodeEditor.h"
#include "KeyboardShortcuts.h"
#include "../Plugin/CabbagePluginProcessor.h"
//class LiveCsound;
class PythonEditor;


//==============================================================================
//Main window. This window holds a 2 textEditors. One for the code, and the other for
//displaying Csound output messages
//==============================================================================
class CodeWindow : public DocumentWindow,
					 public ApplicationCommandTarget,
					 public MenuBarModel,
					 public ActionListener,
					 public ActionBroadcaster,
					 public CodeDocument::Listener,
					 public Timer
{
public:
	CodeWindow(String name);
	~CodeWindow();
		CodeDocument csoundDoc;
	//==============================================================================
	StringArray getMenuBarNames()
	{
		const char* const names[] = { "File", "Edit", "Help", 0 };
		return StringArray (names);
	}
	
	//==============================================================================
	void getAllCommands (Array <CommandID>& commands)
	{
		const CommandID ids[] = {
									CommandIDs::fileNew,
									CommandIDs::fileOpen,
									CommandIDs::fileSave,
									CommandIDs::fileSaveAs,	
									CommandIDs::fileQuit,
									CommandIDs::fileKeyboardShorts,	
									CommandIDs::editUndo,
									CommandIDs::editCopy,
									CommandIDs::editCut,
									CommandIDs::editPaste,
									CommandIDs::editRedo,
									CommandIDs::editToggleComments,
									CommandIDs::editZoomIn,
									CommandIDs::editZoomOut,
									CommandIDs::whiteBackground,
									CommandIDs::blackBackground,
									CommandIDs::startSession,
									CommandIDs::insertFromRepo,
									CommandIDs::AudioSettings,
									CommandIDs::addFromRepo,
									CommandIDs::insertRecentEvent,
									CommandIDs::openPythonEditor,
									CommandIDs::commOrchUpdateInstrument,
									CommandIDs::commOrchUpdateMultiLine,
									CommandIDs::commOrchUpdateSingleLine,
									CommandIDs::commScoUpdateMultiLine,
									CommandIDs::commScoUpdateSingleLine,
									CommandIDs::commOrcUpdateChannel,
									CommandIDs::viewCsoundHelp,	
									CommandIDs::viewCabbageHelp	
									};
		commands.addArray (ids, sizeof (ids) / sizeof (ids [0]));
	}
	
	//==============================================================================
	void menuItemSelected (int menuItemID, int topLevelMenuIndex){
		if (menuItemID >= 100 && menuItemID < 200)
			{
			RecentlyOpenedFilesList recentFiles;
			recentFiles.restoreFromString (appProperties->getUserSettings()
												->getValue ("recentlyOpenedFiles"));

			csdFile = recentFiles.getFile (menuItemID - 100);
			textEditor->editor->getDocument().replaceAllContent(csdFile.loadFileAsString());
			setName(csdFile.getFullPathName());
			}		
	}
	
	//==============================================================================
	void setFontSize(String zoom)
	{
		
		#if defined(WIN32) || defined(MACOSX)
		String font = "Consolas";			
		#else
		String font = "Droid Sans Mono";
		#endif			
		
		if(zoom==String("in"))
		textEditor->editor->setFont(Font(font, ++fontSize, 1));
		else
		textEditor->editor->setFont(Font(font, --fontSize, 1));
	}	
	
	
	//================= command methods ====================
	ApplicationCommandTarget* getNextCommandTarget(){
		return findFirstTargetParentComponent();
	}

	void getCommandInfo (const CommandID commandID, ApplicationCommandInfo& result);
	PopupMenu getMenuForIndex (int topLevelMenuIndex, const String& menuName);
	bool perform (const InvocationInfo& info);
	void toggleManuals(String manual);

	void setEditorColourScheme(String theme);
	void actionListenerCallback(const String &message);

	void timerCallback();
	void toggleTextWindows();
	
	Rectangle<int> getCaretScreenPosition(){
		Rectangle<int> rect(textEditor->editor->getCaretPoisition());
		rect.setLeft(rect.getX()+this->getTopLevelComponent()->getX()+100);
		rect.setTop(rect.getY()+this->getTopLevelComponent()->getY()+45);
		return rect;		
	}


	void closeButtonPressed(){
	//JUCEApplication::getInstance()->systemRequestedQuit();
	this->setVisible(false);
	}

	void codeDocumentTextDeleted(int,int){}
	void codeDocumentTextInserted(const juce::String &,int){}
	void codeDocumentChanged (const CodeDocument::Position &affectedTextStart, const CodeDocument::Position &affectedTextEnd);
	void insertFromRepo();
	
	void setText(String file){
	textEditor->setAllText(file);
	}


	String getText(){
	return textEditor->getAllText();	
	}
	
	
	void setColourScheme(String theme){
	if(theme=="white"){
			textEditor->editor->setColourScheme(csoundToker.getDefaultColourScheme());
			textEditor->setColour(CodeEditorComponent::backgroundColourId, Colours::white);
			textEditor->setColour(CaretComponent::caretColourId, Colours::black);
			textEditor->setColour(CodeEditorComponent::highlightColourId, Colours::cornflowerblue);
			appProperties->getUserSettings()->setValue("EditorColourScheme", 0); 			
			}
	else if(theme=="dark"){
			textEditor->editor->setColourScheme(csoundToker.getDarkColourScheme());
			textEditor->setColour(CaretComponent::caretColourId, Colours::white);
			textEditor->setColour(CodeEditorComponent::backgroundColourId, Colour::fromRGB(20, 20, 20));
			textEditor->setColour(CodeEditorComponent::highlightColourId, Colours::green.withAlpha(.6f)); 
			appProperties->getUserSettings()->setValue("EditorColourScheme", 1);
			}	
	}
	
	void newFile(String type);
	bool unSaved;

	StringArray recentEvents;
	//ScopedPointer<CabbageLookAndFeel> lookAndFeel;

	String csoundOutputText;
	bool firstTime;
	String debugMessage;
	bool showOutput;
	CommandManager commandManager;
	File csdFile;
	int fontSize;
	String ASCIICabbage;

	StringArray opcodeStrings;
	CsoundCodeEditor* textEditor;
	CsoundTokeniser csoundToker;
	Font font;
	ScopedPointer<WebBrowserComponent> htmlHelp;
	bool showingHelp;
};



#endif  // __MAINCOMPONENT_H_615ECE56__
