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

#ifndef __CODE_EDITOR__
#define __CODE_EDITOR__

#include "../../JuceLibraryCode/JuceHeader.h"
#include "CsoundTokeniser.h"
#include "PythonTokeniser.h"
#include "CommandManager.h"
#include "../CabbageUtils.h"


extern ApplicationProperties* appProperties;
extern CabbageTimer* cabbageTimer;

class FlatButton;
class HelpComp;

class CsoundCodeEditorComponenet : public CodeEditorComponent,
						 public ActionBroadcaster,
						 public ChangeBroadcaster,
						 public CodeDocument::Listener
{
public:
	CodeDocument::Position positionInCode;
	CsoundCodeEditorComponenet(String type, CodeDocument &document, CodeTokeniser *codeTokeniser);
	~CsoundCodeEditorComponenet();
	bool keyPressed (const KeyPress& key);
	void handleTabKey(String direction);
	void toggleComments();
	void handleDirectionKey(){}

	void handleEscapeKey(){
	if(type=="python")
		sendActionMessage("make python invisible");
	if(type=="csound")
		sendActionMessage("make popup invisible");	
	}
	
	void handleReturnKey ();	
	void addPopupMenuItems (PopupMenu &menuToAddTo, const MouseEvent *mouseClickEvent);
	void performPopupMenuAction (int menuItemID);
	String getLineText();
	String getTempChannelInstr();
	String getSelectedText();
	String getInstrumentText();
	Rectangle<int> getCaretPoisition();
	StringArray getSelectedTextArray();
	void addRepoToSettings();
	void insertText(String text);
	void addToRepository();
	StringArray repoEntries;
	String getAllText();
	void setAllText(String text);
	void highlightLine(String line);
	void highlightLines(int firstLine, int lastLine);
	void codeDocumentTextDeleted(int,int);
	void codeDocumentTextInserted(const juce::String &,int);
	bool textChanged;
	void insertNewLine(String text);
	void changeListenerCallback(juce::ChangeBroadcaster* source);
	void setOpcodeStrings(String opcodes){
	opcodeStrings.addLines(opcodes);
	}
	
	void updateCaretPosition();
	void insertMultiLineTextAtCaret(String text);
	String getOpcodeToken(int index){
	return opcodeTokens[index];	
	}	
	
	private:
		int xPos, yPos, prevXpos;
		CodeDocument::Position pos1, pos2;
		Colour selectedColour;
		String type;
		StringArray opcodeStrings;
		StringArray opcodeTokens;
	};

//=================================================================
class CsoundCodeEditor : public Component,
						 public ChangeListener,
						 public ActionListener
{
	bool showTabButtons;
	bool showInstrumentButtons;
	int instrWidth;
	
public:	
	CsoundCodeEditor(CodeDocument &document, CodeTokeniser *codeTokeniser);
	~CsoundCodeEditor(){
	editor.clear();	
	};

	void changeListenerCallback(juce::ChangeBroadcaster* source);
	String getAllText();
	void setAllText(String text);	
	void highlightLine(String line);	
	void showTab(String name);
	void showInstrs(bool show);
	void actionListenerCallback(const juce::String&);

	void saveAuxFile();	
	OwnedArray<FlatButton> tabButtons;
	OwnedArray<FlatButton> instrButtons;
	ScopedPointer<HelpComp> helpComp;	
	bool textChanged;	
	OwnedArray<CsoundCodeEditorComponenet> editor;
	int currentEditor;
	void addNewFile(File newFile);	
	void paint(Graphics& g);	
	void resized();
	Range<int> getCabbageSectionRange();
	CodeDocument currentDoc[20];;
	CsoundTokeniser codeToker;
	StringArray openFiles;
};


class FlatButton : public Component,
					public ChangeBroadcaster
{
public:
		FlatButton(String name, int currentTab, String type): 	name(name),
													type(type),
													currentTab(currentTab),
													isMouseDown(false)
		{
		setName(name);
		if(name=="Csound code")
			active = true;
		else 
			active=false;
		
		}
		~FlatButton(){}
		
	void mouseDown(const MouseEvent& event){
		sendChangeMessage();
		Logger::writeToLog("mouseDown");
		isMouseDown = true;
		repaint();		
	}
	
	void mouseUp(const MouseEvent& event){
		Logger::writeToLog("mouseUp");
		isMouseDown = false;
		repaint();		
	}	
		
	void isActive(bool isActive){
		active = isActive;
		repaint();
	}	
		
	void paint(Graphics &g){
		g.fillAll(Colour(20, 20, 20));
		if(type=="Native")
		{
		if(name.contains("code")){
			Logger::writeToLog("changing code paint, currentTab:"+String(currentTab));
			if(active)
				g.setColour(Colours::cornflowerblue.darker(.4f));		
			else
				g.setColour(Colours::cornflowerblue.darker(.9f));	
		
			g.fillRoundedRectangle(getLocalBounds().toFloat(), 5.f);
			if(active)
				g.setColour(Colours::white);
			else
				g.setColour(Colours::white.darker(.9f));
				
			g.drawRoundedRectangle(getLocalBounds().toFloat(), 5.f, 2);
			g.drawFittedText(name, getLocalBounds(), Justification::centred, 1, 1.f);
			}
			
		}	
		else if(type=="Aux"){
			Logger::writeToLog("Painting Aux");
			if(!active)
				g.setColour(Colours::brown.darker(.9f));
			else
				g.setColour(Colours::brown.darker(.2f));
			g.fillRoundedRectangle(getLocalBounds().toFloat(), 5.f);
			
			if(!active)
				g.setColour(Colours::white.darker(.9f));	
			else
				g.setColour(Colours::white);
					
			g.drawRoundedRectangle(getLocalBounds().toFloat(), 5.f, 2);
			g.drawFittedText(name, getLocalBounds(), Justification::centred, 1, 1.f);			
			
		}
		else{
			float number = name.substring(6, 5).getDoubleValue();
			if(isMouseDown)
				g.setColour(Colours::brown.darker(.2f));
			else
				g.setColour(Colours::brown.darker(.6f));

			g.fillRoundedRectangle(getLocalBounds().toFloat(), 5.f);
			g.setColour(Colours::white);
			g.drawRoundedRectangle(getLocalBounds().toFloat(), 5.f, 2);
			g.drawFittedText(name, getLocalBounds(), Justification::centred, 1, 1.f);
			}
		}
	String name;
	int currentTab;
	bool active;
	bool isMouseDown;
	String type;

};

//============================================================
// class for displaying popup text
//============================================================
class HelpComp : public Component
{
public:
	HelpComp():firstTime(true)
	{
		this->setInterceptsMouseClicks(false, false);
		setSize(10, 50);
	}	
	
	~HelpComp(){};
	
	void setText(String _info, String _syntax){
		syntax=_syntax;
		info=_info;
		repaint();
	}

	void paint(Graphics& g)
	{
		g.fillAll(Colour::fromRGB(10, 10, 10));
		g.setColour(Colours::yellow);
		g.drawRect(0, 0, getWidth()-1, getHeight()-1, 1);
		g.setFont(14);
		g.setColour(Colours::cornflowerblue);
		g.drawFittedText(syntax, 10, 5, getWidth(), getHeight(), Justification::topLeft, 100, 1);
		int width = Font(14).getStringWidth(syntax);
		g.setColour(Colours::white);				
		g.drawFittedText(info, width+30, 4, getWidth(), getHeight(), Justification::topLeft, 100, 1);
	}
	
	void resized(){
		setSize(getWidth(), getHeight());	
	}
	
bool firstTime;
private:

String syntax, info;

};

#endif