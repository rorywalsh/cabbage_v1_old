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
	void codeDocumentTextDeleted(int,int);
	void codeDocumentTextInserted(const juce::String &,int);
	bool textChanged;
	void insertNewLine(String text);
	void setOpcodeStrings(String opcodes){
	opcodeStrings.addLines(opcodes);
	}
	
	void updateCaretPosition();
	void insertMultiTextAtCaret(String text);
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
						 public ActionListener
{
	bool showTabButtons;
	bool showInstrumentButtons;
	int instrWidth;
	
public:	
	CsoundCodeEditor(CodeDocument &document, CodeTokeniser *codeTokeniser);
	~CsoundCodeEditor(){};


	String getAllText(){
		String text = editor->getDocument().getAllContent();
		return text; 	
	}	

	void setAllText(String text){
		editor->getDocument().replaceAllContent(text);
	}
	
	void highlightLine(String line){
			String temp = editor->getDocument().getAllContent();
			Range<int> range;
			editor->moveCaretTo(CodeDocument::Position (editor->getDocument(), temp.indexOf(line)+line.length()), false);
			editor->moveCaretTo(CodeDocument::Position (editor->getDocument(), temp.indexOf(line)), true);
	}	
	
	void showTabs(bool show);
	void showInstrs(bool show);
	void actionListenerCallback(const String &message);
	
	OwnedArray<FlatButton> tabButtons;
	OwnedArray<FlatButton> instrButtons;
	ScopedPointer<HelpComp> helpComp;
	
	bool textChanged;	
	ScopedPointer<CsoundCodeEditorComponenet> editor;
	void resized();
	Range<int> getCabbageSectionRange();
};


class FlatButton : public Component,
					public ActionBroadcaster
{
	String name;
	int currentTab;
	bool active;
	bool isMouseDown;
	
	public:
		FlatButton(String name, int currentTab): 	name(name), 
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
		sendActionMessage(name);
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
		g.fillAll(Colour::fromRGB(40,40,40));
		g.setColour(Colours::yellow);
		g.drawRect(0, 0, getWidth()-1, getHeight()-1, 1);
		g.setFont(14);
		g.setColour(Colours::cornflowerblue);
		g.drawFittedText(syntax, 10, 5, getWidth(), getHeight(), Justification::topLeft, 100, 1);
		int width = Font(14).getStringWidth(syntax);
		g.setColour(Colours::white);				
		g.drawFittedText(info, width+30, 5, getWidth(), getHeight(), Justification::topLeft, 100, 1);
	}
	
	void resized(){
		setSize(getWidth(), getHeight());	
	}
	
bool firstTime;
private:

String syntax, info;

};

#endif