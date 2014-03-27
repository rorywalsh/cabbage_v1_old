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
showInstrumentButtons(false),
currentEditor(0),
searchStartIndex(0),
highlightedWord(false),
documentIndex(0)
{
	editor.add(new CsoundCodeEditorComponenet("csound", document, codeTokeniser));
	addAndMakeVisible(editor[currentEditor]);
	editor[currentEditor]->getDocument().setSavePoint();
	
	openFiles.add("CABBAGE_CSOUND_FILE");
	
	addAndMakeVisible(helpComp = new HelpComp());
	helpComp->setVisible(false);
	addAndMakeVisible(searchReplaceComp = new SearchReplaceComp());
	searchReplaceComp->addChangeListener(this);
	searchReplaceComp->setVisible(true);
	
	editor[currentEditor]->addChangeListener(this);
	editor[currentEditor]->addActionListener(this);
	searchReplaceComp->searchButton->addChangeListener(this);
	searchReplaceComp->replaceOnceButton->addChangeListener(this);
	searchReplaceComp->replaceAllButton->addChangeListener(this);
	
	tabButtons.add(new FlatButton("Cabbage code", tabButtons.size()-1, "Native"));
	addAndMakeVisible(tabButtons[tabButtons.size()-1]);
	tabButtons[tabButtons.size()-1]->addChangeListener(this);
	tabButtons[tabButtons.size()-1]->isActive(true);
	tabButtons.add(new FlatButton("Csound code", tabButtons.size()-1, "Native"));
	addAndMakeVisible(tabButtons[tabButtons.size()-1]);
	tabButtons[tabButtons.size()-1]->addChangeListener(this);
	tabButtons[tabButtons.size()-1]->isActive(false);
	showTabButtons = true;
}

CsoundCodeEditor::~CsoundCodeEditor()
{
	editor.clear();	
	codeDocuments.clear();
};
//==============================================================================
int CsoundCodeEditor::saveAllFiles()
{
	int result=0;
	int returnVal=1;
	
	if(openFiles[0]=="CABBAGE_CSOUND_FILE")
		return 1;
	
	
	for(int i=0;i<editor.size();i++)
		{
			if(editor[i]->getDocument().hasChangedSinceSavePoint())
			{
			String message = File(openFiles[i]).getFileName()+String(" has changed. Would like to save your changes?");		
			result = CabbageUtils::showYesNoMessage(message, &this->getLookAndFeel(), 1);	
			if(result==0)
			{
				File(openFiles[i]).replaceWithText(editor[i]->getAllText());
				returnVal = 1;
			}
			else if(result==1)
				returnVal = 1;
			else if(result==2)
				returnVal = 0;
			}
		}
	return returnVal;
}
//==============================================================================
void CsoundCodeEditor::setSavePoint()
{
editor[currentEditor]->getDocument().setSavePoint();	
}

//==============================================================================
void CsoundCodeEditor::enableColumnEdit(bool enable)
{
	editor[currentEditor]->enableColumnEditMode(enable);
}

//==============================================================================
void CsoundCodeEditor::resized()
{
	editor[currentEditor]->toFront(true);

	if(showTabButtons&&showInstrumentButtons){
		editor[currentEditor]->setBounds(instrWidth+20, 20, getWidth()-instrWidth-20, (getHeight()-55));
		helpComp->setBounds(instrWidth+20, getHeight()-30, getWidth()-instrWidth-20, 30);	
		searchReplaceComp->setBounds(instrWidth+20, getHeight()-30, getWidth()-instrWidth-20, 30);	
	}	
	else if(showTabButtons && !showInstrumentButtons){
		editor[currentEditor]->setBounds(0, 20, getWidth(), (getHeight()-55));	
		searchReplaceComp->setBounds(33, getHeight()-30, getWidth()-33, 30);
		helpComp->setBounds(33, getHeight()-30, getWidth()-33, 30);
		
	}
	else if(showInstrumentButtons && !showTabButtons){
		editor[currentEditor]->setBounds(instrWidth+5, 0, getWidth()-instrWidth-5, getHeight()-55);	
		searchReplaceComp->setBounds(instrWidth+5, getHeight()-30, getWidth()-instrWidth-5, 30);
		helpComp->setBounds(instrWidth+5, getHeight()-30, getWidth()-instrWidth-5, 30);

	}
	else{
		editor[currentEditor]->setBounds(0, 0, getWidth(), getHeight()-55);
		searchReplaceComp->setBounds(33, getHeight()-30, getWidth()-33, 30);
		helpComp->setBounds(33, getHeight()-30, getWidth()-33, 30);

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

//==============================================================================
void CsoundCodeEditor::paint(Graphics& g)
{
	g.fillAll(CabbageUtils::getBackgroundSkin());
	g.setColour(Colour(10, 10, 10));
	g.fillRect(0, 200, getWidth(), getHeight()-200);
}
	
//==============================================================================
void CsoundCodeEditor::showTab(String name)
{
	tabButtons.add(new FlatButton(name, tabButtons.size()-1, "Aux"));
	addAndMakeVisible(tabButtons[tabButtons.size()-1]);
	tabButtons[tabButtons.size()-1]->isActive(true);
	tabButtons[tabButtons.size()-1]->addChangeListener(this);
	showTabButtons = true;
	resized();
}
//==============================================================================
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
		instrButtons.add(new FlatButton(tmpArray[i].substring(tmpArray[i].indexOf("instr ")+6), i, "Instr"));
		addAndMakeVisible(instrButtons[instrButtons.size()-1]);
		instrButtons[instrButtons.size()-1]->addChangeListener(this);
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

//nt showing instruments buttons just yet
showInstrumentButtons = false;	
resized();	
}
//==============================================================================
Range<int> CsoundCodeEditor::getCabbageSectionRange()
{
	Range<int> range;
	range.setStart(0);
	range.setEnd(0);
	StringArray array;
	array.addLines(editor[currentEditor]->getAllText());
	for(int i=0;i<array.size();i++)
		if(array[i]=="</Cabbage>")
			range.setEnd(i);
	return range;	
}
//==============================================================================
void CsoundCodeEditor::addNewFile(File newFile)
{	
	codeDocuments.add(new CodeDocument());
	editor.add(new CsoundCodeEditorComponenet("csound", *codeDocuments[codeDocuments.size()-1], &codeToker));
	documentIndex++;
	currentEditor = editor.size()-1;	
	for(int i=0;i<tabButtons.size();i++)
		if(i!=currentEditor+1)
		tabButtons[i]->isActive(false);
	
	editor[currentEditor]->addActionListener(this);
	addAndMakeVisible(editor[currentEditor]);
	editor[currentEditor]->toFront(true);
	editor[currentEditor]->loadContent(newFile.loadFileAsString());
	//editor[currentEditor]->getDocument().replaceAllContent(newFile.loadFileAsString());
	editor[currentEditor]->getDocument().setSavePoint();
	openFiles.add(newFile.getFullPathName());	
	resized();
}
//==============================================================================
String CsoundCodeEditor::getAllText(){
	String text = editor[0]->getDocument().getAllContent();
	return text; 	
}	
//==============================================================================
void CsoundCodeEditor::setAllText(String text){
	editor[0]->loadContent(text);
}
//==============================================================================
void CsoundCodeEditor::highlightLine(String line){
		String temp = editor[currentEditor]->getDocument().getAllContent();
		Range<int> range;
		editor[currentEditor]->moveCaretTo(CodeDocument::Position (editor[currentEditor]->getDocument(), temp.indexOf(line)+line.length()), false);
		editor[currentEditor]->moveCaretTo(CodeDocument::Position (editor[currentEditor]->getDocument(), temp.indexOf(line)), true);
}
//==============================================================================
void CsoundCodeEditor::closeCurrentFile()
{
	if(openFiles.size()>1)
	{	
		String file = openFiles[currentEditor];
		openFiles.remove(currentEditor);
		editor[currentEditor]->enableColumnEditMode(false);
		editor.remove(currentEditor);
		
		for(int i=0;i<tabButtons.size();i++)
		{	
			if(tabButtons[i]->getName()==File(file).getFileName()){
				tabButtons.remove(i);
				for(int y=i;y<tabButtons.size();y++)
					tabButtons[y]->currentTab--;
			}
		}
		
		setActiveTab(currentEditor);
		currentEditor--;
		resized();
		
	}
}
//==============================================================================
void CsoundCodeEditor::setActiveTab(int index)
{
	tabButtons[index]->isActive(true);
	for(int i=0;i<tabButtons.size();i++)
	if(i!=index)
	tabButtons[i]->isActive(false);	
}
//==============================================================================
void CsoundCodeEditor::saveAuxFile()
{
	File file(openFiles[currentEditor]);;
	file.replaceWithText(editor[currentEditor]->getAllText());
}
//==============================================================================
void CsoundCodeEditor::changeListenerCallback(juce::ChangeBroadcaster* source)
{
		if(dynamic_cast<FlatButton*>(source))
			{
			FlatButton* button = dynamic_cast<FlatButton*>(source);
			if(button->type=="Native")
				{
				//editor[currentEditor]->setVisible(false);
				currentEditor=0;
				//editor[currentEditor]->setVisible(true);
				editor[currentEditor]->toFront(true);	
				if(button->getName()=="Cabbage code")
					{
					showInstrs(true);
					tabButtons[0]->isActive(true);
					editor[currentEditor]->moveCaretTo(CodeDocument::Position(editor[currentEditor]->getDocument(), 0), false);	
					editor[currentEditor]->scrollToLine(0);	
					for(int i=0;i<tabButtons.size();i++)
						if(i!=0)
						tabButtons[i]->isActive(false);
					//resized();
					repaint();
					}
				else if(button->getName()=="Csound code")
					{
					tabButtons[1]->isActive(true);			
					editor[currentEditor]->moveCaretTo(CodeDocument::Position(editor[currentEditor]->getDocument(), 
															editor[currentEditor]->getAllText().indexOf("<CsInstruments>")+16),
															false);					
					editor[currentEditor]->scrollToLine(editor[currentEditor]->getCaretPos().getLineNumber());
					for(int i=0;i<tabButtons.size();i++)
						if(i!=1)
						tabButtons[i]->isActive(false);		
					}		
				}
		else if(button->type=="Aux")
			{	
				currentEditor = button->currentTab;	
				//CabbageUtils::showMessage(currentEditor);
				editor[currentEditor]->toFront(true);
				editor[currentEditor]->enableColumnEditMode(false);
				button->isActive(true);
				for(int i=0;i<tabButtons.size();i++)
					if(i!=currentEditor+1)
					tabButtons[i]->isActive(false);
				showInstrs(false);
			}
		else if(button->getName()=="SearchButton")
			{
			findText(searchReplaceComp->getSearchText());
			}
		else if(button->getName()=="ReplaceOnceButton")
			{
			replaceText(searchReplaceComp->getSearchText(), searchReplaceComp->getReplaceText());
			searchReplaceComp->setVisible(true);
			helpComp->setVisible(false);
			}
		else if(button->getName()=="ReplaceAllButton")
			{
			while(replaceText(searchReplaceComp->getSearchText(), searchReplaceComp->getReplaceText())!=-1);
			searchReplaceComp->setVisible(true);
			helpComp->setVisible(false);
			}
	}
	else if(dynamic_cast<SearchReplaceComp*>(source))
			{
			if(searchReplaceComp->getCurrentTextEditor()=="SearchEditor")
			findText(searchReplaceComp->getSearchText().trim());		
			else
				replaceText(searchReplaceComp->getSearchText(), searchReplaceComp->getReplaceText());
			}
	
}

//=========================================================================
int CsoundCodeEditor::findText(String text)
{
if(searchReplaceComp->getSearchText().isNotEmpty())
	{
	String fullText = editor[currentEditor]->getAllText();
	String searchTerm = text;
	searchStartIndex = fullText.indexOf(searchStartIndex, searchTerm);
	if(searchStartIndex==-1)return -1;
	editor[currentEditor]->moveCaretTo(CodeDocument::Position(editor[currentEditor]->getDocument(), 
							editor[currentEditor]->getAllText().indexOf(searchStartIndex, searchTerm)),
							false);
	editor[currentEditor]->moveCaretTo(CodeDocument::Position(editor[currentEditor]->getDocument(), 
							editor[currentEditor]->getAllText().indexOf(searchStartIndex, searchTerm)+searchTerm.length()),
							true);	
	searchStartIndex=searchStartIndex+searchTerm.length();
	if(fullText.indexOf(searchStartIndex, searchTerm)<searchStartIndex)
		searchStartIndex=0;
	}
	return searchStartIndex;		
}

//=========================================================================
String CsoundCodeEditor::getCurrentSelectedText()
{
Range<int> range = editor[currentEditor]->getHighlightedRegion();
if(range.getLength()>0)
		return editor[currentEditor]->getAllText().substring(range.getStart(), range.getEnd());
else
	return "";
}
//=========================================================================
int CsoundCodeEditor::replaceText(String text, String replaceWith)
{
	String fullText = editor[currentEditor]->getAllText();
	Range<int> range = editor[currentEditor]->getHighlightedRegion();
	if(range.getLength()==0)
		{
			if(findText(text)>0)
			{
			range = editor[currentEditor]->getHighlightedRegion();
			editor[currentEditor]->getDocument().replaceSection(range.getStart(), range.getEnd(), replaceWith);
			findText(text);
			}
		}
	else{
		editor[currentEditor]->getDocument().replaceSection(range.getStart(), range.getEnd(), replaceWith);
		findText(text);
		}
	return searchStartIndex;
}

//=========================================================================
void CsoundCodeEditor::actionListenerCallback(const juce::String& message)
{
if(message.contains("helpDisplay"))
	{
	if(!helpComp->isVisible()){
		searchReplaceComp->setVisible(false);
		helpComp->setVisible(true);
		resized();
	}
	//CodeDocument::Position newPos = editor->getCaretPos();
	//editor->setCaretPos(editor->getCharacterBounds(editor->getCaretPos()));
	if(helpComp->isVisible())
	helpComp->setText(editor[currentEditor]->getOpcodeToken(2).removeCharacters("\""), 
							   editor[currentEditor]->getOpcodeToken(3).removeCharacters("\""));
	//editor->setCaretPos(editor->getCharacterBounds(editor->getCaretPos()));
	}
	else{
	String instrument = "instr "+message;
	editor[currentEditor]->moveCaretTo(CodeDocument::Position(editor[currentEditor]->getDocument(), 
											editor[currentEditor]->getAllText().indexOf(instrument)),
											false);	
	
	editor[currentEditor]->scrollToLine(editor[currentEditor]->getCaretPos().getLineNumber());
	}	
	
}


//==============================================================================
CsoundCodeEditorComponenet::CsoundCodeEditorComponenet(String type, CodeDocument &document, CodeTokeniser *codeTokeniser)
					: CodeEditorComponent(document, codeTokeniser), type(type), columnEditMode(false)
{
	document.addListener(this);
	setColour(CodeEditorComponent::backgroundColourId, Colour::fromRGB(35, 35, 35));
	setColour(CodeEditorComponent::lineNumberBackgroundId, Colour::fromRGB(10, 10, 10));
	//toggle this when in column-edit mode
	setColour(CodeEditorComponent::highlightColourId, Colours::cornflowerblue.withAlpha(.2f)); 
	setColour(CaretComponent::caretColourId, Colours::white);
	setColour(TextEditor::backgroundColourId, Colours::black);
	setColour(CodeEditorComponent::defaultTextColourId, Colours::white);
	setLineNumbersShown(40);
	//setColour(CodeEditorComponent::highlightColourId, Colours::yellow); 
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
		moveCaretTo(CodeDocument::Position (getDocument(), temp.indexOf(line)+line.length()), false);
		moveCaretTo(CodeDocument::Position (getDocument(), temp.indexOf(line)), true);
}

//==============================================================================
void CsoundCodeEditorComponenet::highlightLines(int firstLine, int lastLine){
		moveCaretTo(CodeDocument::Position (getDocument(), firstLine, 0), false);
		moveCaretTo(CodeDocument::Position (getDocument(), lastLine, 5000), true);
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
		{
			if(!columnEditMode)                        
					insertTextAtCaret (String::charToString (key.getTextCharacter()));
			else
					insertMultiLineTextAtCaret(String::charToString (key.getTextCharacter()));
		}
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
void CsoundCodeEditorComponenet::insertText(String text)
{
	getDocument().insertText(getCaretPos(), text);
}
//==============================================================================
void CsoundCodeEditorComponenet::insertNewLine(String text){
	pos1 = getCaretPos();
	StringArray csdArray;
	csdArray.addLines(getAllText());
	String curLine = csdArray[pos1.getLineNumber()];	
	int numberOfTabs=0;
	int numberOfSpaces=0;
	String tabs;
	while(curLine.substring(numberOfTabs, numberOfTabs+1).equalsIgnoreCase("\t")){
		tabs.append("\t", 8);
		numberOfTabs++;
	}
	
	while(curLine.substring(numberOfSpaces, numberOfSpaces+1).equalsIgnoreCase(" ")){
		tabs.append(" ", 8);
		numberOfSpaces++;
	}
	
	Logger::writeToLog("Number of tabs:"+String(numberOfTabs));
	getDocument().insertText(pos1, text+tabs);
}

//==============================================================================
bool CsoundCodeEditorComponenet::pasteFromClipboard()
{
    const String clip (SystemClipboard::getTextFromClipboard());
    if (clip.isNotEmpty())
		if(!columnEditMode)
        insertText (clip);
		else
		insertMultiLineTextAtCaret(clip);		

    return true;
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
void CsoundCodeEditorComponenet::enableColumnEditMode(bool enable)
{
if(enable)
	{
	setColour(CodeEditorComponent::highlightColourId, Colours::cornflowerblue.withAlpha(.0f)); 
	columnEditMode=true;
	}
else{
	setColour(CodeEditorComponent::highlightColourId, Colours::cornflowerblue.withAlpha(.2f)); 
	columnEditMode=false;
	}
}


//==============================================================================
void CsoundCodeEditorComponenet::updateCaretPosition()
{
	//Logger::writeToLog("Updating caret position");
	if(columnEditMode==1){
	StringArray selectedText;
	selectedText.addLines(getTextInRange(this->getHighlightedRegion()));
	CodeDocument::Position startPos(this->getDocument(), getHighlightedRegion().getStart());
	CodeDocument::Position endPos(this->getDocument(), getHighlightedRegion().getEnd());
	Rectangle<int> newCaretPosition(getCharacterBounds(startPos));
	newCaretPosition.setHeight(getCharacterBounds (getCaretPos()).getHeight()*selectedText.size());
	setCaretPos(newCaretPosition);
	}
	else
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
bool CsoundCodeEditorComponenet::deleteBackwards (const bool moveInWholeWordSteps)
{
	CodeDocument::Position startPos(getDocument(), getHighlightedRegion().getStart());
	CodeDocument::Position endPos(getDocument(), getHighlightedRegion().getEnd());
	
	if(columnEditMode)
	{
		//sendActionMessage("make popup invisible");
		StringArray csdArray;
		csdArray.addLines(getAllText());
		String curLine;	
		CodeDocument::Position newPos, indexPos;

		int indexInLine = startPos.getIndexInLine();		
			
		for(int i=startPos.getLineNumber();i<endPos.getLineNumber()+1;i++)
		{
			while(csdArray[i].length()<indexInLine)
				{
				csdArray.getReference(i).append(" ", 1);
				}
			csdArray.set(i, csdArray[i].replaceSection(indexInLine-1, 1, ""));	
		}
		setAllText(csdArray.joinIntoString("\n"));
		
		CodeDocument::Position newStartPos(getDocument(), startPos.getLineNumber(), indexInLine-1);
		CodeDocument::Position newEndPos(getDocument(), endPos.getLineNumber(), indexInLine-1);
		moveCaretTo(newStartPos, false);
		moveCaretTo(newEndPos, true);			
	}
	else
	{	
		
		if (moveInWholeWordSteps)
		{
			getDocument().deleteSection(getCaretPos().getPosition(), getCaretPos().getPosition()+1);
			moveCaretTo (getDocument().findWordBreakBefore (getCaretPos()), true);
		}
		else if ( !isHighlightActive() )
		{
			startPos = getCaretPos();
			startPos.moveBy (-1);
			getDocument().deleteSection(startPos.getPosition(), getCaretPos().getPosition());
		}
		else
		{
			getDocument().deleteSection(startPos.getPosition(), endPos.getPosition());
		}

    
	}
    return true;
}
//==============================================================================
bool CsoundCodeEditorComponenet::deleteForwards (const bool moveInWholeWordSteps)
{
	CodeDocument::Position startPos(getDocument(), getHighlightedRegion().getStart());
	CodeDocument::Position endPos(getDocument(), getHighlightedRegion().getEnd());
	
	if(columnEditMode)
	{
		//sendActionMessage("make popup invisible");
		StringArray csdArray;
		csdArray.addLines(getAllText());
		String curLine;	
		CodeDocument::Position newPos, indexPos;

		int indexInLine = startPos.getIndexInLine();		
			
		for(int i=startPos.getLineNumber();i<endPos.getLineNumber()+1;i++)
		{
			while(csdArray[i].length()<indexInLine)
				{
				csdArray.getReference(i).append(" ", 1);
				}
			csdArray.set(i, csdArray[i].replaceSection(indexInLine, 1, ""));	
		}
		setAllText(csdArray.joinIntoString("\n"));
		
		CodeDocument::Position newStartPos(getDocument(), startPos.getLineNumber(), indexInLine);
		CodeDocument::Position newEndPos(getDocument(), endPos.getLineNumber(), indexInLine);
		moveCaretTo(newStartPos, false);
		moveCaretTo(newEndPos, true);			
	}	
	
	else
	{
		if (moveInWholeWordSteps)
		{
			getDocument().deleteSection(getCaretPos().getPosition(), getCaretPos().getPosition()+1);
			moveCaretTo (getDocument().findWordBreakBefore (getCaretPos()), true);
		}
		else if ( !isHighlightActive() )
		{
			startPos.moveBy (+1);
			getDocument().deleteSection(getCaretPos().getPosition(), startPos.getPosition());
		}
		else
		{
			getDocument().deleteSection(startPos.getPosition(), endPos.getPosition());
		}
	}
    return true;
}
//==============================================================================
/*
void CsoundCodeEditorComponenet::insertTextAtCaret (const String &textToInsert)
{
	if(!columnEditMode)                        
		insertTextAtCaret (textToInsert);
	else
		insertMultiLineTextAtCaret(textToInsert);
}*/
//==============================================================================
void CsoundCodeEditorComponenet::insertMultiLineTextAtCaret (String text)
{
	//sendActionMessage("make popup invisible");
	StringArray csdArray;
	csdArray.addLines(getAllText());
	String curLine;	
	CodeDocument::Position newPos, indexPos;

	CodeDocument::Position startPos(getDocument(), getHighlightedRegion().getStart());
	CodeDocument::Position endPos(getDocument(), getHighlightedRegion().getEnd());
	int indexInLine = startPos.getIndexInLine();
	
	
	for(int i=startPos.getLineNumber();i<endPos.getLineNumber()+1;i++)
	{
		while(csdArray[i].length()<indexInLine)
			{
			csdArray.getReference(i).append(" ", 1);
			}
		csdArray.set(i, csdArray[i].replaceSection(indexInLine, 0, text));	
	}
	setAllText(csdArray.joinIntoString("\n"));
	
	CodeDocument::Position newStartPos(getDocument(), startPos.getLineNumber(), indexInLine+text.length());
	CodeDocument::Position newEndPos(getDocument(), endPos.getLineNumber(), indexInLine+text.length());
	moveCaretTo(newStartPos, false);
	moveCaretTo(newEndPos, true);	
}
//==============================================================================
void CsoundCodeEditorComponenet::handleTabKey(String direction)
{	
	/* multi line action, get highlited text, find the position of
	 * it within the text editor, remove it from editor and reinsert it with
	 * formatting
	 */
	 
	StringArray selectedText, csdArray;
	//selectedText.addLines(getSelectedText());
	csdArray.addLines(getAllText());
	String csdText;
	String currentLine;	
	CodeDocument::Position startPos(this->getDocument(), getHighlightedRegion().getStart());
	CodeDocument::Position endPos(this->getDocument(), getHighlightedRegion().getEnd());
	pos1 = getCaretPos();

	if(direction.equalsIgnoreCase("forwards")){
		//single line tab
		if(getHighlightedRegion().getLength()==0){
			insertTabAtCaret();
			return;
		}
		else{ 
		//multiline tab
		for(int i=startPos.getLineNumber();i<endPos.getLineNumber()+1;i++)
						csdArray.set(i, "\t"+csdArray[i]);	
		}
		setAllText(csdArray.joinIntoString("\n"));	
		highlightLines(startPos.getLineNumber(), endPos.getLineNumber());
		
	}
	else if(direction.equalsIgnoreCase("backwards"))
	//single line back tab
	if(getHighlightedRegion().getLength()<1)
		{
		//no need for shift tab, just hit backspace
		}
		//multiline back tab
		else{ 
			for(int i=startPos.getLineNumber();i<endPos.getLineNumber()+1;i++)
				if(csdArray[i].substring(0, 1).equalsIgnoreCase("\t"))
							csdArray.set(i, csdArray[i].substring(1));	

			setAllText(csdArray.joinIntoString("\n"));	
			highlightLines(startPos.getLineNumber(), endPos.getLineNumber());
		}
}
//==============================================================================
bool CsoundCodeEditorComponenet::moveCaretLeft (const bool moveInWholeWordSteps, const bool selecting)
{
	CodeDocument::Position startPos(getDocument(), getHighlightedRegion().getStart());
	CodeDocument::Position endPos(getDocument(), getHighlightedRegion().getEnd());
	
		if (isHighlightActive() && ! (selecting || moveInWholeWordSteps))
		{
			moveCaretTo (startPos, false);
			if(!columnEditMode)
			return true;
		}

		if (moveInWholeWordSteps)
			moveCaretTo (getDocument().findWordBreakBefore (getCaretPos()), selecting);

		else
			moveCaretTo (startPos.movedBy (-1), selecting);


	if(columnEditMode)
	{
		CodeDocument::Position newStartPos(getDocument(), startPos.getLineNumber(), getCaretPos().getIndexInLine());
		CodeDocument::Position newEndPos(getDocument(), endPos.getLineNumber(), getCaretPos().getIndexInLine());	
		moveCaretTo(newStartPos, false);
		moveCaretTo(newEndPos, true);			
	}
	
    return true;
}
//==============================================================================
bool CsoundCodeEditorComponenet::moveCaretRight (const bool moveInWholeWordSteps, const bool selecting)
{
	CodeDocument::Position startPos(getDocument(), getHighlightedRegion().getStart());
	CodeDocument::Position endPos(getDocument(), getHighlightedRegion().getEnd());
	

    if (isHighlightActive() && ! (selecting || moveInWholeWordSteps))
    {
        moveCaretTo (endPos, false);
		if(!columnEditMode)
        return true;
    }

    if (moveInWholeWordSteps)
        moveCaretTo (getDocument().findWordBreakAfter (getCaretPos()), selecting);
    else
        moveCaretTo (endPos.movedBy (1), selecting);

	if(columnEditMode)
	{
		CodeDocument::Position newStartPos(getDocument(), startPos.getLineNumber(), getCaretPos().getIndexInLine());
		CodeDocument::Position newEndPos(getDocument(), endPos.getLineNumber(), getCaretPos().getIndexInLine());	
		moveCaretTo(newStartPos, false);
		moveCaretTo(newEndPos, true);			
	}


    return true;
}
//==============================================================================
void CsoundCodeEditorComponenet::codeDocumentTextDeleted(int start,int end)
{	

}
