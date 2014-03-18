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

//==============================================================================
// container for text editor and tab/instr buttons
//==============================================================================
CsoundCodeEditor::CsoundCodeEditor(CodeDocument &document, CodeTokeniser *codeTokeniser):
showTabButtons(false),
instrWidth(60),
showInstrumentButtons(false)
{
	addAndMakeVisible(editor = new CsoundCodeEditorComponenet("csound", document, codeTokeniser));
	
	addAndMakeVisible(helpComp = new HelpComp());
	editor->addActionListener(this);
	
	if(CabbageUtils::getPreference(appProperties, "showTabs")==1)
	{
	showTabs(true);
	showInstrs(true);
	}

}

void CsoundCodeEditor::resized()
{
	if(showTabButtons&&showInstrumentButtons){
		editor->setBounds(instrWidth+20, 20, getWidth()-instrWidth-20, (getHeight()-20)-60);
		helpComp->setBounds(instrWidth+20, getHeight()-60, getWidth()-instrWidth-20, 30);	
	}	
	else if(showTabButtons && !showInstrumentButtons){
		editor->setBounds(0, 20, getWidth(), (getHeight()-60));	
		helpComp->setBounds(0, getHeight()-60, getWidth(), 30);
	}
	else if(showInstrumentButtons && !showTabButtons){
		editor->setBounds(instrWidth+5, 0, getWidth()-instrWidth-5, getHeight()-60);	
		helpComp->setBounds(instrWidth+5, getHeight()-60, getWidth()-instrWidth-5, 30);
	}
	else{
		editor->setBounds(0, 0, getWidth(), getHeight()-60);
		helpComp->setBounds(0, getHeight()-60, getWidth(), 30);
	}
	
	
		
	if(showTabButtons)
	{
		for(int i=0;i<tabButtons.size();i++)
			if(!showInstrumentButtons)
			tabButtons[i]->setBounds(20+(i*103), 2, 100, 15);	
			else
			tabButtons[i]->setBounds(instrWidth+20+(i*103), 2, 100, 15);	
	}	
	if(showInstrumentButtons)
	{
		for(int i=0;i<instrButtons.size();i++)
		instrButtons[i]->setBounds(2, 20+(i*18), instrWidth, 15);	
	}	
	
}

void CsoundCodeEditor::showTabs(bool show)
{
	if(show)
		{
		tabButtons.clear(true);
		StringArray array;
		array.add("Cabbage code");
		array.add("Csound code");
		for(int i=0;i<2;i++)
			{
			tabButtons.add(new FlatButton(array[i], i));
			addAndMakeVisible(tabButtons[tabButtons.size()-1]);
			tabButtons[tabButtons.size()-1]->isActive(bool(1-i));
			tabButtons[tabButtons.size()-1]->addActionListener(this);
			}

			showTabButtons = true;
		}
	else{
		tabButtons.clear(true);
		showTabButtons = false;
	}
	resized();
}

void CsoundCodeEditor::showInstrs(bool show)
{
	if(show){
	instrButtons.clear(true);	
	StringArray tmpArray;
	int noInstruments=0;
	tmpArray.addLines(this->getAllText());
	for(int i=0;i<tmpArray.size();i++)
	if(tmpArray[i].contains("instr ")){
		noInstruments++;
		instrButtons.add(new FlatButton(tmpArray[i].substring(tmpArray[i].indexOf("instr ")+6), i));
		addAndMakeVisible(instrButtons[instrButtons.size()-1]);
		instrButtons[instrButtons.size()-1]->addActionListener(this);
		if(Font(13).getStringWidth(tmpArray[i])>instrWidth)
			instrWidth = Font(13).getStringWidth(tmpArray[i]);
			
		}		
		
		if(noInstruments>3)
			showInstrumentButtons = true;
		else
			showInstrumentButtons = false;
				
	}
	else{
		instrButtons.clear(true);
		showInstrumentButtons = false;		
	}

resized();	
}

void CsoundCodeEditor::actionListenerCallback(const String &message)
{
		Logger::writeToLog("Change editor text:"+message);
		if(message=="Cabbage code"){
		tabButtons[0]->isActive(true);
		tabButtons[1]->isActive(false);
		editor->moveCaretTo(CodeDocument::Position(editor->getDocument(), 0), false);	
		editor->scrollToLine(0);	
		}
		else if(message=="Csound code"){
		tabButtons[0]->isActive(false);
		tabButtons[1]->isActive(true);			
		editor->moveCaretTo(CodeDocument::Position(editor->getDocument(), 
												editor->getAllText().indexOf("</Cabbage>")+10),
												false);	
		
		editor->scrollToLine(editor->getCaretPos().getLineNumber());
		}
		else if(message.contains("helpDisplay")){
		//CodeDocument::Position newPos = editor->getCaretPos();
		//editor->setCaretPos(editor->getCharacterBounds(editor->getCaretPos()));
		helpComp->setText(String(" --- ")+editor->getOpcodeToken(2).removeCharacters("\""), 
								   editor->getOpcodeToken(3).removeCharacters("\""));
		//editor->setCaretPos(editor->getCharacterBounds(editor->getCaretPos()));
		}
		else{
		String instrument = "instr "+message;
		editor->moveCaretTo(CodeDocument::Position(editor->getDocument(), 
												editor->getAllText().indexOf(instrument)),
												false);	
		
		editor->scrollToLine(editor->getCaretPos().getLineNumber());
		}
	
}
//==============================================================================
CsoundCodeEditorComponenet::CsoundCodeEditorComponenet(String type, CodeDocument &document, CodeTokeniser *codeTokeniser)
					: CodeEditorComponent(document, codeTokeniser), type(type), textChanged(false)
{
	document.addListener(this);
	setColour(CodeEditorComponent::backgroundColourId, Colour::fromRGB(35, 35, 35));
	setColour(CodeEditorComponent::lineNumberBackgroundId, Colour::fromRGB(10, 10, 10));
	setColour(CodeEditorComponent::highlightColourId, Colours::green.withAlpha(.6f)); 
	setColour(CaretComponent::caretColourId, Colours::white);
	setColour(TextEditor::backgroundColourId, Colours::black);
	setColour(TextEditor::textColourId, Colours::white);
	setLineNumbersShown(40);
	setColour(CodeEditorComponent::highlightColourId, Colours::cornflowerblue); 
	setColour(CodeEditorComponent::lineNumberTextId, Colours::whitesmoke);
	setLineNumbersShown(true);
	#if defined(WIN32) || defined(MACOSX)
	setFont(Font(String("Consolas"), 13, 1));			
	#else
	setFont(Font(String("Droid Sans Mono"), 13, 1));
	#endif	
}
//==============================================================================
CsoundCodeEditorComponenet::~CsoundCodeEditorComponenet()
{
	
}
//==============================================================================
void CsoundCodeEditorComponenet::highlightLine(String line){
		String temp = getDocument().getAllContent();
		Range<int> range;
		moveCaretTo(CodeDocument::Position (getDocument(), temp.indexOf(line)+line.length()), false);
		moveCaretTo(CodeDocument::Position (getDocument(), temp.indexOf(line)), true);
		//range.setStart(temp.indexOf(line)+line.length());
		//range.setEnd(temp.indexOf(line));	
		//range.setStart(1);
		//range.setEnd(100);		
		//setHighlightedRegion(range);
}
//==============================================================================
bool CsoundCodeEditorComponenet::keyPressed (const KeyPress& key)
{
	//Logger::writeToLog(String(key.getKeyCode()));	
	//if (key.getTextDescription().contains("cursor up") || key.getTextDescription().contains("cursor down") 
    //    || key.getTextDescription().contains("cursor left") || key.getTextDescription().contains("cursor right"))  
	//handleEscapeKey();

	if (! TextEditorKeyMapper<CodeEditorComponent>::invokeKeyFunction (*this, key))
    {
			
        if (key == KeyPress::returnKey)
			handleReturnKey();                               
			
        else if (key == KeyPress::escapeKey)                                
			handleEscapeKey();
        //else if (key == KeyPress ('[', ModifierKeys::commandModifier, 0))   unindentSelection();
        //else if (key == KeyPress (']', ModifierKeys::commandModifier, 0))   indentSelection();
        else if (key.getTextCharacter() >= ' ')                            
		insertTextAtCaret (String::charToString (key.getTextCharacter()));
		//insertMultiTextAtCaret(String::charToString (key.getTextCharacter()));

		else if(key.getKeyCode() ==  268435488)
			handleTabKey("backwards");
		else if(key ==  KeyPress::tabKey)
			handleTabKey("forwards");
			
			
        else                                                                
		return false;
    }
    //handleUpdateNowIfNeeded();
    return true;
}
//==============================================================================
void CsoundCodeEditorComponenet::handleReturnKey (){
	insertNewLine("\n");
	//sendActionMessage("make popup invisible");				
}	
//==============================================================================
void CsoundCodeEditorComponenet::insertText(String text){
	pos1 = getCaretPos();
	getDocument().insertText(pos1, text);
}
//==============================================================================
void CsoundCodeEditorComponenet::insertNewLine(String text){
	pos1 = getCaretPos();
	StringArray csdArray;
	csdArray.addLines(getAllText());
	String curLine = csdArray[pos1.getLineNumber()];	
	int numberOfTabs=0;
	String tabs;
	while(curLine.substring(numberOfTabs, numberOfTabs+1).equalsIgnoreCase("\t")){
		tabs.append("\t", 8);
		numberOfTabs++;
	}
	Logger::writeToLog("Number of tabs:"+String(numberOfTabs));
	getDocument().insertText(pos1, text+tabs);
}
//==============================================================================
void CsoundCodeEditorComponenet::insertMultiTextAtCaret (String text)
{
	//sendActionMessage("make popup invisible");
	StringArray csdArray;
	csdArray.addLines(getAllText());
	String curLine;	
	CodeDocument::Position newPos, indexPos;
	newPos = getCaretPos();//getSelectionStartCaretPos();
	int currentLine = getCaretPos().getLineNumber();
	int index = newPos.getIndexInLine();
	Logger::writeToLog(String(index));
	
	StringArray selectedText;
	selectedText.addLines(getTextInRange(this->getHighlightedRegion()));
	
	
	for(int i=0;i<selectedText.size();i++){
		curLine = newPos.getLineText();
		Logger::writeToLog(String(curLine.length()));
		/* need to check for tabs and add four spaces!!*/		
		for(int y=curLine.length();y<index+2;y++){
			getDocument().insertText(CodeDocument::Position(getDocument(), newPos.getLineNumber(), curLine.length()), " ");
			newPos = newPos.movedBy(1);
			//curLine = csdArray[currentLine+i];
		}

		getDocument().insertText(newPos, text);	
		newPos = newPos.movedByLines(1);
		}
	//sendActionMessage("make popup invisible");
}
//==============================================================================
void CsoundCodeEditorComponenet::handleTabKey(String direction)
{	
	/*multi line action, get highlited text, find the position of
	 * it within the text editor, remove it from editor and reinsert it with
	 * formatting
	 */
	 
	StringArray selectedText, csdArray;
	selectedText.addLines(getSelectedText());
	csdArray.addLines(getAllText());
	String csdText;
	String currentLine;	

	if(direction.equalsIgnoreCase("forwards")){
		//single line tab
		if(selectedText.size()<1){
			insertTabAtCaret();
			return;
		}
		else{ 
		//multiline tab
		int indexOfText = getAllText().indexOf(getSelectedText());
		csdText = getAllText().replace(getSelectedText(), "");
		for(int i=0;i<selectedText.size();i++)
						selectedText.set(i, "\t"+selectedText[i]);	

		csdText = csdText.replaceSection(indexOfText, 0, selectedText.joinIntoString("\n"));
		}
		
	}
	else if(direction.equalsIgnoreCase("backwards"))
	//single line back tab
	if(selectedText.size()<1){
		pos1 = getCaretPos();
		//Logger::writeToLog(csdArray[pos1.getLineNumber()]);
		currentLine = csdArray[pos1.getLineNumber()];
		if(csdArray[pos1.getLineNumber()].substring(0, 1).contains("\t")){
			csdArray.set(pos1.getLineNumber(), currentLine.substring(1));
			csdText = csdArray.joinIntoString("\n");
		}
		else
			return;
	}
	//multiline back tab
		else{ 
		//multiline tab
		int indexOfText = getAllText().indexOf(getSelectedText());
		csdText = getAllText().replace(getSelectedText(), "");
		for(int i=0;i<selectedText.size();i++)
			if(selectedText[i].substring(0, 1).equalsIgnoreCase("\t"))
						selectedText.set(i, selectedText[i].substring(1));	

		csdText = csdText.replaceSection(indexOfText, 0, selectedText.joinIntoString("\n"));
		}

	//Logger::writeToLog(newTextArray.joinIntoString("\n"));
	setAllText(csdText);	
	if(selectedText.size()>0)
		highlightLine(selectedText.joinIntoString("\n"));
	else
		moveCaretTo(CodeDocument::Position (getDocument(), getAllText().indexOf(currentLine.substring(1))), false);

	//sendActionMessage("make popup invisible");	
}
//==============================================================================
void CsoundCodeEditorComponenet::toggleComments()
{
	StringArray selectedText;
	selectedText.addLines(getSelectedText());
	StringArray csdArray;
	csdArray.addLines(this->getAllText());
	String lastLine;

	for(int i=0;i<csdArray.size();i++)
		for(int y=0;y<selectedText.size();y++)
			if(selectedText[y]==csdArray[i]){
				if(!csdArray[i].equalsIgnoreCase("")){
					if(selectedText[y].substring(0, 1).equalsIgnoreCase(";"))
					csdArray.set(i, selectedText[y].substring(1));
					else
					csdArray.set(i, ";"+selectedText[y]);	
					lastLine = selectedText[y].substring(1);
				}
			}
			
	this->setAllText(csdArray.joinIntoString("\n"));
	moveCaretTo(CodeDocument::Position (getDocument(), getAllText().indexOf(lastLine)+lastLine.length()), false);
}

//=================== addPopupMenuItems =======================
void CsoundCodeEditorComponenet::addPopupMenuItems (PopupMenu &menuToAddTo, const MouseEvent *mouseClickEvent)
{
	menuToAddTo.addItem(1, "Cut");
	menuToAddTo.addItem(1, "Copy");
	menuToAddTo.addItem(1, "Paste");
	menuToAddTo.addItem(1, "Select All");
	menuToAddTo.addSeparator();
	menuToAddTo.addItem(1, "Undo");
	menuToAddTo.addItem(1, "Redo");
	menuToAddTo.addItem(10, "Add to repo");
	PopupMenu m;
	int repoIndex = 100;
	ScopedPointer<XmlElement> xmlElement;
	xmlElement = appProperties->getUserSettings()->getXmlValue("CopeRepoXmlData");
	if(xmlElement)
	forEachXmlChildElement (*xmlElement, e)
		{
		m.addItem(repoIndex, e->getTagName());
		repoEntries.add(e->getTagName());
		repoIndex++;
		}	
	xmlElement =nullptr;	
	menuToAddTo.addSubMenu("Insert from repo", m);
};

//==============================================================================
Rectangle<int> CsoundCodeEditorComponenet::getCaretPoisition()
{
pos1 = getCaretPos();
return getCharacterBounds(pos1);		
}

//==============================================================================
void CsoundCodeEditorComponenet::performPopupMenuAction (int menuItemID){
	if(menuItemID==1000){	
		pos1 = getDocument().findWordBreakBefore(getCaretPos());
		String line = getDocument().getLine(pos1.getLineNumber());			
		Logger::writeToLog(line);
		line = getTextInRange(this->getHighlightedRegion());
		Logger::writeToLog(line);
		sendActionMessage(line);			
	}

	//add to repo
	else if(menuItemID==10){
			addToRepository();
	}

	//insert from repo
	else if(menuItemID>=100){
	ScopedPointer<XmlElement> xmlElement;
	xmlElement = appProperties->getUserSettings()->getXmlValue("CopeRepoXmlData");

	forEachXmlChildElement (*xmlElement, e)
		{
		if(e->getTagName()==repoEntries[menuItemID-100])
			insertText(e->getAllSubText());
		}
	xmlElement = nullptr;
	}	
	
};
//==============================================================================
void CsoundCodeEditorComponenet::addRepoToSettings()
{

}
//==============================================================================
void CsoundCodeEditorComponenet::updateCaretPosition()
{
	/*
	Logger::writeToLog("Updating caret position");
	int columnEdit = 1;
	
	if(columnEdit==1){
	StringArray selectedText;
	selectedText.addLines(getTextInRange(this->getHighlightedRegion()));
	Rectangle<int> newCaretPosition(getCharacterBounds(getSelectionStartCaretPosition()));
	newCaretPosition.setHeight(getCharacterBounds (getCaretPos()).getHeight()*selectedText.size());
	caret->setCaretPosition (newCaretPosition);
	}
	else*/
	setCaretPos(getCharacterBounds (getCaretPos()));
}
//==============================================================================
void CsoundCodeEditorComponenet::addToRepository()
{
	AlertWindow alert("Add to Repository", "Enter a name and hit 'escape'", AlertWindow::NoIcon, this->getTopLevelComponent()); 
	//CabbageLookAndFeel basicLookAndFeel;
	//alert.setLookAndFeel(&basicLookAndFeel);
	alert.setColour(TextEditor::textColourId, Colours::white);
	alert.setColour(TextEditor::highlightedTextColourId, Colours::lime);
	alert.addTextEditor("textEditor", "enter name", "");
	alert.getTextEditor("textEditor")->setColour(TextEditor::textColourId, Colours::lime);	
	alert.runModalLoop();
	Logger::writeToLog(alert.getTextEditorContents("textEditor"));
	String repoEntryName = alert.getTextEditorContents("textEditor");
	
	if(repoEntryName!="enter name"){
		ScopedPointer<XmlElement> repoXml;
		ScopedPointer<XmlElement> newEntryXml;
		repoXml = appProperties->getUserSettings()->getXmlValue("CopeRepoXmlData");
		newEntryXml = new XmlElement(repoEntryName);
		newEntryXml->addTextElement(getSelectedText());
		if(newEntryXml)
		repoXml->addChildElement(newEntryXml);
		appProperties->getUserSettings()->setValue("CopeRepoXmlData", repoXml);	
		
		repoXml = nullptr;
		newEntryXml = nullptr;
	}
}
//==============================================================================
String CsoundCodeEditorComponenet::getLineText(){
	StringArray csdLines;
	csdLines.addLines(getDocument().getAllContent());
	pos1 = getDocument().findWordBreakBefore(getCaretPos());
	Logger::writeToLog(csdLines[pos1.getLineNumber()]);	
	return csdLines[pos1.getLineNumber()];
}	
//==============================================================================
String CsoundCodeEditorComponenet::getAllText(){
	return getDocument().getAllContent();
}	
//==============================================================================
void CsoundCodeEditorComponenet::setAllText(String text){
	getDocument().replaceAllContent(text);
}
//==============================================================================
String CsoundCodeEditorComponenet::getTempChannelInstr(){
	String channel = "event_i \"i\", 999.999, 0, .1\n";
	channel << "instr 999\n";
	channel << getLineText() << "\n";
	channel << "endin\n";	
	Logger::writeToLog(channel);
	return channel;
}
//==============================================================================
String CsoundCodeEditorComponenet::getSelectedText(){
	String selectedText = getTextInRange(this->getHighlightedRegion());
	Logger::writeToLog(selectedText);	
	return selectedText;
}
//==============================================================================
StringArray CsoundCodeEditorComponenet::getSelectedTextArray(){
	StringArray tempArray;
	String selectedText = getTextInRange(this->getHighlightedRegion());
	tempArray.addLines(selectedText);
	Logger::writeToLog(selectedText);	
	return tempArray;
}
//==============================================================================
String CsoundCodeEditorComponenet::getInstrumentText(){
	StringArray csdLines;
	csdLines.addLines(getDocument().getAllContent());
	pos1 = getDocument().findWordBreakBefore(getCaretPos());
	int startOfInstrDef, endOfInstrDef;
	int index = pos1.getLineNumber();
	while(index>0){
	if(csdLines[index].contains("instr")){
		startOfInstrDef = index-1;
		break;
		}
	else
		index--;
	}

	index = pos1.getLineNumber();
	while(index>0){
	if(csdLines[index].contains("endin")){
		endOfInstrDef = index;
		break;
		}
	else
		index++;
	}		
	
	String selectedText="";
	for(int i = startOfInstrDef;i<=endOfInstrDef;i++)
	selectedText += csdLines[i]+"\n";
	Logger::writeToLog(selectedText);	
	return selectedText;
}
//==============================================================================
void CsoundCodeEditorComponenet::codeDocumentTextInserted(const juce::String &,int)
{
	
textChanged = true;
pos1 = getDocument().findWordBreakBefore(getCaretPos());
String lineFromCsd = getDocument().getLine(pos1.getLineNumber());

if(CabbageUtils::getPreference(appProperties, "EnablePopupDisplay"))
	{
	String opcodeHelpString;
	StringArray syntaxTokens, csdLineTokens;
	csdLineTokens.clear();
	csdLineTokens.addTokens(lineFromCsd, " ,\t", "");

		for(int i=0;i<opcodeStrings.size();i++){
			opcodeHelpString = opcodeStrings[i];
			syntaxTokens.clear();	
			syntaxTokens.addTokens(opcodeHelpString, ";", "\"");
			if(syntaxTokens.size()>3)
			for(int x=0;x<csdLineTokens.size();x++){
				//Logger::writeToLog(csdLineTokens[x]);
				if(syntaxTokens[0].removeCharacters("\"")==csdLineTokens[x].trim()){
					if(syntaxTokens[0].length()>3){
							//Logger::writeToLog(syntaxTokens[0]);
							sendActionMessage("helpDisplay"+syntaxTokens[2]);
							opcodeTokens = syntaxTokens;
							x=csdLineTokens.size();
							i=opcodeStrings.size();	
						}
					}	
					
			}
			
		}
	}
}
//==============================================================================
void CsoundCodeEditorComponenet::codeDocumentTextDeleted(int,int){
textChanged = true;
//sendActionMessage("make popup invisible");	
}
