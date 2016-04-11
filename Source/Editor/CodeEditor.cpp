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
    editor.add(new CsoundCodeEditorComponent("csound", document, codeTokeniser));
    editor[currentEditor]->setOpaque(false);
    addAndMakeVisible(editor[currentEditor]);
    editor[currentEditor]->getDocument().setSavePoint();
    openFiles.add("CABBAGE_CSOUND_FILE");

    helpComp = new HelpComp();
    searchReplaceComp = new SearchReplaceComp();

#ifndef CABBAGE_HOST
    addAndMakeVisible(searchReplaceComp);
    searchReplaceComp->setVisible(true);
#endif

    addAndMakeVisible(helpComp);
    helpComp->setVisible(false);
    searchReplaceComp->addChangeListener(this);


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


    for(int i=0; i<editor.size(); i++)
    {
        if(editor[i]->getDocument().hasChangedSinceSavePoint())
        {
            String message = File(openFiles[i]).getFileName()+String(" has changed. Would like to save your changes?");
            result = cUtils::showYesNoMessage(message, &this->getLookAndFeel(), 1);
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


    if(showTabButtons&&showInstrumentButtons)
    {
        editor[currentEditor]->setBounds(instrWidth-10, 20, getWidth()-instrWidth-10, (getHeight()-55));
        helpComp->setBounds(instrWidth+10, getHeight()-30, getWidth()-instrWidth, 30);
        searchReplaceComp->setBounds(instrWidth+10, getHeight()-30, getWidth()-instrWidth, 30);
    }
    else if(showTabButtons && !showInstrumentButtons)
    {
        editor[currentEditor]->setBounds(33, 20, getWidth()-33, (getHeight()-55));
        searchReplaceComp->setBounds(33, getHeight()-30, getWidth()-33, 30);
        helpComp->setBounds(33, getHeight()-30, getWidth()-33, 30);

    }
    else
    {
        editor[currentEditor]->setBounds(0, 0, getWidth(), getHeight() - (helpComp->isVisible() ? 30 : 0));
        //searchReplaceComp->setBounds(0, getHeight()-30, getWidth()-33, 30);
        helpComp->setBounds(0, getHeight()-30, getWidth(), 30);

    }

    if(showTabButtons)
    {
        for(int i=0; i<tabButtons.size(); i++)
            if(!showInstrumentButtons)
                tabButtons[i]->setBounds(20+(i*103), 2, 100, 15);
            else
                tabButtons[i]->setBounds(instrWidth+20+(i*103), 2, 100, 15);
    }
    if(showInstrumentButtons)
    {
        for(int i=0; i<instrButtons.size(); i++)
        {
            instrButtons[i]->setBounds(2, 20+(i*18), instrWidth, 15);
            instrButtons[i]->toFront(true);
        }
    }

}

//==============================================================================
void CsoundCodeEditor::paint(Graphics& g)
{
    g.fillAll(cUtils::getDarkerBackgroundSkin());
    g.setColour(backgroundColour);
    g.fillRect(35, 20, getWidth(), getHeight());
#ifndef CABBAGE_HOST
    int index = 0;
    if(editor[currentEditor] != 0)
    {
        g.setColour(juce::Colours::white);
        g.setFont(editor[currentEditor]->getFont());
        bool highlightedLine = false;
        int firstLineToDraw = editor[currentEditor]->getFirstLineOnScreen();
        int lastLineToDraw = firstLineToDraw + editor[currentEditor]->getNumLinesOnScreen() + 2;

        for (int j = firstLineToDraw; j < lastLineToDraw; ++j)
        {

            for(int i=0; i<selectedRegions.size(); i++)
            {
                g.setColour(selectedRegions[i]->getColour());
                if(selectedRegions[i]->getLinesRange().contains(Range<int>(j, j+1)))
                    g.fillRoundedRectangle(35, (editor[currentEditor]->getLineHeight() * index)+editor[currentEditor]->getLineHeight(), getWidth()-54, editor[currentEditor]->getLineHeight()*2, 10);
            }

            for(int i=1; i<selectedEditorTokens.size(); i++)
            {
                if(appProperties->getUserSettings()->getIntValue("EditorColourScheme", 0)==1)
                    g.setColour(Colour(80,80,80));
                else
                    g.setColour(Colours::cornflowerblue.withAlpha(.4f));
                if(selectedEditorTokens[i]->getLineNumber() == j)
                {
                    int startPos = selectedEditorTokens[i]->getPositionOfTokenInLine().getStart();
                    int endPos = selectedEditorTokens[i]->getPositionOfTokenInLine().getEnd();
                    int xPos = editor[currentEditor]->getFont().getStringWidth(" ")*startPos+35;
                    int width = startPos+(endPos-startPos)*editor[currentEditor]->getFont().getStringWidth(" ");
                    g.fillRoundedRectangle(xPos, (editor[currentEditor]->getLineHeight() * index)+editor[currentEditor]->getLineHeight(), width, editor[currentEditor]->getLineHeight(), 3);

                }
            }

            g.setColour(juce::Colours::white);
            for(int i=0; i<breakpointLines.size(); i++)
            {
                //CodeDocument::Position startPos(editor[currentEditor]->getDocument(), editor[currentEditor]->getCaretPos().getPosition());
                if(j+1==breakpointLines[i])
                {
                    g.setColour(Colours::brown);
                    g.fillRoundedRectangle(10, (editor[currentEditor]->getLineHeight() * index)+20, 28, editor[currentEditor]->getLineHeight(), 4);
                    g.setColour(Colours::white);
                    highlightedLine = true;
                }
            }
            if(highlightedLine == true)
            {
                g.setColour(Colours::black);
                g.drawFittedText(String(j+1), 0, (editor[currentEditor]->getLineHeight() * index)+20, 33, editor[currentEditor]->getLineHeight(),
                                 juce::Justification::centredRight, false);
                g.setColour(Colours::white);
                highlightedLine = false;
            }
            else if(highlightedLine == false)
            {
                g.drawFittedText(String(j+1), 0, (editor[currentEditor]->getLineHeight() * index)+20, 33, editor[currentEditor]->getLineHeight(),
                                 juce::Justification::centredRight, false);
            }

            index += 1;
        }
    }

#endif
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
    if(show)
    {
        instrButtons.clear(true);
        StringArray tmpArray;
        int noInstruments=0;
        tmpArray.addLines(this->getAllText());
        for(int i=0; i<tmpArray.size(); i++)
            if(tmpArray[i].contains("instr "))
            {
                noInstruments++;
                instrButtons.add(new FlatButton(tmpArray[i].substring(tmpArray[i].indexOf("instr ")+6), i, "Instr"));
                addAndMakeVisible(instrButtons[instrButtons.size()-1]);
                instrButtons[instrButtons.size()-1]->addChangeListener(this);
                if(Font(13).getStringWidth(tmpArray[i])>instrWidth)
                    instrWidth = Font(13).getStringWidth(tmpArray[i]);
            }

        //if(noInstruments>3)
        //	showInstrumentButtons = true;
        //else
        showInstrumentButtons = true;

    }
    else
    {
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
    for(int i=0; i<array.size(); i++)
        if(array[i]=="</Cabbage>")
            range.setEnd(i);
    return range;
}
//==============================================================================
void CsoundCodeEditor::addNewFile(File newFile)
{
    codeDocuments.add(new CodeDocument());
    editor.add(new CsoundCodeEditorComponent("csound", *codeDocuments[codeDocuments.size()-1], &codeToker));
    documentIndex++;
    currentEditor = editor.size()-1;
    for(int i=0; i<tabButtons.size(); i++)
        if(i!=currentEditor+1)
            tabButtons[i]->isActive(false);

    Logger::writeToLog(newFile.loadFileAsString());
    editor[currentEditor-1]->setVisible(false);
    editor[currentEditor]->addActionListener(this);
    addAndMakeVisible(editor[currentEditor]);
    editor[currentEditor]->toFront(true);
    editor[currentEditor]->setOpaque(false);
    editor[currentEditor]->loadContent(newFile.loadFileAsString());
    //editor[currentEditor]->getDocument().replaceAllContent(newFile.loadFileAsString());
    editor[currentEditor]->getDocument().setSavePoint();
    openFiles.add(newFile.getFullPathName());
    resized();
}
//==============================================================================
String CsoundCodeEditor::getAllText()
{
    String text = editor[0]->getDocument().getAllContent();
    return text;
}
//==============================================================================
void CsoundCodeEditor::setAllText(String text)
{
    int lineNumber = editor[0]->getFirstLineOnScreen();
    editor[0]->getDocument().replaceAllContent(text);
    editor[0]->scrollToLine(lineNumber);
    editor[0]->getDocument().setSavePoint();
}
//==============================================================================
void CsoundCodeEditor::highlightLine(String line)
{
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

        for(int i=0; i<tabButtons.size(); i++)
        {
            if(tabButtons[i]->getName()==File(file).getFileName())
            {
                tabButtons.remove(i);
                for(int y=i; y<tabButtons.size(); y++)
                    tabButtons[y]->currentTab--;
            }
        }

        if(currentEditor>0)
        {
            setActiveTab(currentEditor);
            currentEditor--;
            resized();
        }

    }
}
//==============================================================================
void CsoundCodeEditor::setActiveTab(int index)
{
    tabButtons[index]->isActive(true);
    for(int i=0; i<tabButtons.size(); i++)
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
    if(CsoundCodeEditorComponent* editor = dynamic_cast<CsoundCodeEditorComponent*>(source))
    {
        cUtils::debug("hti me!");
        selectedEditorTokens.swapWith(editor->selectedTokens);
        return;
    }

    if(FlatButton* button = dynamic_cast<FlatButton*>(source))
    {

        if(button->type=="Native")
        {
            //editor[currentEditor]->setVisible(false);
            currentEditor=0;
            editor[currentEditor]->setVisible(true);
            editor[currentEditor]->toFront(true);

            for( int i=0; i<editor.size(); i++)
            {
                if(i==currentEditor)
                {
                    editor[i]->setVisible(true);
                    editor[i]->toFront(true);
                }
                else
                {
                    editor[i]->setVisible(false);
                    editor[i]->toFront(false);
                }
            }

            if(button->getName()=="Cabbage code")
            {
                showInstrs(true);
                tabButtons[0]->isActive(true);
                editor[currentEditor]->moveCaretTo(CodeDocument::Position(editor[currentEditor]->getDocument(),
                                                   editor[currentEditor]->getAllText().indexOf("<Cabbage>")),
                                                   false);
                editor[currentEditor]->scrollToLine(editor[currentEditor]->getCaretPos().getLineNumber());
                for(int i=0; i<tabButtons.size(); i++)
                    if(i!=0)
                        tabButtons[i]->isActive(false);
                //resized();
                //repaint();
            }
            else if(button->getName()=="Csound code")
            {
                tabButtons[1]->isActive(true);
                editor[currentEditor]->moveCaretTo(CodeDocument::Position(editor[currentEditor]->getDocument(),
                                                   editor[currentEditor]->getAllText().indexOf("<CsoundSynthesizer>")+19),
                                                   false);
                editor[currentEditor]->scrollToLine(editor[currentEditor]->getCaretPos().getLineNumber());
                for(int i=0; i<tabButtons.size(); i++)
                    if(i!=1)
                        tabButtons[i]->isActive(false);
            }
        }
        else if(button->type=="Aux")
        {
            currentEditor = button->currentTab;
            //cUtils::showMessage(currentEditor);
            //editor[currentEditor]->toFront(true);

            for( int i=0; i<editor.size(); i++)
            {
                if(i==currentEditor)
                {
                    editor[i]->setVisible(true);
                    editor[i]->toFront(true);
                }
                else
                {
                    editor[i]->setVisible(false);
                    editor[i]->toFront(false);
                }
            }

            editor[currentEditor]->enableColumnEditMode(false);
            button->isActive(true);
            for(int i=0; i<tabButtons.size(); i++)
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
        else if(button->type=="Instr")
        {
            String instrument = "instr "+button->getName();
            editor[currentEditor]->moveCaretTo(CodeDocument::Position(editor[currentEditor]->getDocument(),
                                               editor[currentEditor]->getAllText().indexOf(instrument)),
                                               false);

            editor[currentEditor]->scrollToLine(editor[currentEditor]->getCaretPos().getLineNumber());
        }

        return;
    }
    else if(dynamic_cast<SearchReplaceComp*>(source))
    {
        if(searchReplaceComp->getCurrentTextEditor()=="SearchEditor")
            findText(searchReplaceComp->getSearchText().trim());
        else
            replaceText(searchReplaceComp->getSearchText(), searchReplaceComp->getReplaceText());

        return;
    }

}

//=========================================================================
int CsoundCodeEditor::findText(String text)
{
    if(searchReplaceComp->getSearchText().isNotEmpty())
        editor[currentEditor]->findText(text);
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
    else
    {
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
#ifdef CABBAGE_HOST
        helpComp->setVisible(true);
        resized();
#else
        if(!helpComp->isVisible())
        {
            searchReplaceComp->setVisible(false);
            helpComp->setVisible(true);
            resized();
        }
#endif
    }
    else if(message.contains("InstrumentBreakpoint"))
    {
        String info = message;

        int lineNumber = message.substring(message.indexOf("_")+1).getIntValue();
        if(breakpointLines.size()>=0)
            if(message.contains("Set"))
            {
                breakpointLines.add(lineNumber+1);
            }
            else
            {
                breakpointLines.removeAllInstancesOf(lineNumber+1);
            }

        //cUtils::showMessage(breakpointLines.size());

        sendActionMessage(message);
        repaint();
    }
    else if(message.contains("HightlightRegionOfInterest"))
    {
        String range  = message.substring(message.indexOf("_")+1);
        int hStart = range.substring(0, range.indexOf(":")+1).getIntValue();
        range =  range.substring(range.indexOf(":")+1);
        int hEnd = range.substring(0, range.indexOf("=")).getIntValue();
        String colourString = range.substring(range.indexOf("=")+1);
        Colour colour = Colour::fromString(colourString);
        SelectedRegion selR(Range<int>(hStart, hEnd), colour);
        selectedRegions.add(new SelectedRegion(Range<int>(hStart, hEnd), colour));
        repaint();
    }
    else if(message.contains("RemoveRegionOfInterest_"))
    {
        int lineNumber  = message.substring(message.indexOf("_")+1).getIntValue();
        for(int i=0; i<selectedRegions.size(); i++)
        {
            if(selectedRegions[i]->getLinesRange().contains(Range<int>(lineNumber, lineNumber)))
            {
                selectedRegions.remove(i);
            }
        }
        repaint();
    }
    else if(message.contains("HightlightMultipleInstances"))
    {


    }
    else if(message=="Launch help")
        sendActionMessage("Launch help");

    if(helpComp->isVisible())
        helpComp->setText(editor[currentEditor]->getOpcodeToken(2).removeCharacters("\""),
                          editor[currentEditor]->getOpcodeToken(3).removeCharacters("\""));

}


//==============================================================================
CsoundCodeEditorComponent::CsoundCodeEditorComponent(String type, CodeDocument &document, CodeTokeniser *codeTokeniser)
    : CodeEditorComponent(document, codeTokeniser), type(type), columnEditMode(false), fontSize(15), autoCompleteListBox()
{
    document.addListener(this);
    setColour(CodeEditorComponent::backgroundColourId, Colour::fromRGB(35, 35, 35));
    setColour(CodeEditorComponent::lineNumberBackgroundId, cUtils::getDarkerBackgroundSkin());
    //toggle this when in column-edit mode
    setColour(CodeEditorComponent::highlightColourId, Colours::lime.withAlpha(.3f));
    setColour(CaretComponent::caretColourId, Colours::white);
    setColour(TextEditor::backgroundColourId, Colours::black);
    setColour(CodeEditorComponent::defaultTextColourId, Colours::white);
    //setLineNumbersShown(40);
    //setColour(CodeEditorComponent::highlightColourId, Colours::yellow);
    setColour(CodeEditorComponent::lineNumberTextId, Colours::whitesmoke);

    autoCompleteListBox.setRowHeight (16);
    autoCompleteListBox.setModel (this);   // Tell the listbox where to get its data model
    addChildComponent(autoCompleteListBox);

    setLineNumbersShown(false);

    fontSize = (cUtils::getPreference(appProperties, "FontSize")>7 ?
                cUtils::getPreference(appProperties, "FontSize") : 13);

    font = cUtils::getPreference(appProperties, "Fonttype", "");

#ifdef CabbageHost
#ifdef WIN32
    font = "Consolas";
#elif defined(MACOSX)
    font = "Courier New";
#else
    font = "Droid Sans Mono";
#endif
#endif

    setFont(Font(font, fontSize, 0));

    variableNames.add("Test1");
    variableNames.add("Test2");
    variableNames.add("Test3");
    variableNames.add("Test4");
    autoCompleteListBox.updateContent();

}
//==============================================================================
CsoundCodeEditorComponent::~CsoundCodeEditorComponent()
{

}
//==============================================================================
void CsoundCodeEditorComponent::highlightLine(String line)
{
    String temp = getDocument().getAllContent();
    moveCaretTo(CodeDocument::Position (getDocument(), temp.indexOf(line)+line.length()), false);
    moveCaretTo(CodeDocument::Position (getDocument(), temp.indexOf(line)), true);
}

//==============================================================================
void CsoundCodeEditorComponent::highlightLines(int firstLine, int lastLine)
{
    moveCaretTo(CodeDocument::Position (getDocument(), firstLine, 0), false);
    moveCaretTo(CodeDocument::Position (getDocument(), lastLine, 5000), true);
}
//==============================================================================
bool CsoundCodeEditorComponent::keyPressed (const KeyPress& key)
{
    //Logger::writeToLog(String(key.getKeyCode()));
    if (key.getTextDescription().contains("cursor up") || key.getTextDescription().contains("cursor down"))
    {
        if(autoCompleteListBox.isVisible())
        {
            int selectedRow = autoCompleteListBox.getSelectedRow();
            if(key.getTextDescription().contains("cursor down"))
                autoCompleteListBox.selectRow(jmax(0, selectedRow+1));
            else if(key.getTextDescription().contains("cursor up"))
                autoCompleteListBox.selectRow(jmax(0, selectedRow-1));
            return true;

        }
    }

    this->getParentComponent()->repaint();

    if (key == KeyPress ('z', ModifierKeys::commandModifier, 0))
        undoText();

    if (! TextEditorKeyMapper<CodeEditorComponent>::invokeKeyFunction (*this, key))
    {

        if (key == KeyPress::returnKey)
            handleReturnKey();

        else if (key == KeyPress::escapeKey)
            handleEscapeKey();
        //else if (key == KeyPress (']', ModifierKeys::commandModifier, 0))   indentSelection();
        else if (key.getTextCharacter() >= ' ')
        {
            insertTextAtCaret (String::charToString (key.getTextCharacter()));
            scrollToKeepCaretOnScreen();
        }
        else if(key.getKeyCode() ==  KeyPress::tabKey && key.getModifiers().isShiftDown())
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
void CsoundCodeEditorComponent::handleReturnKey ()
{
    if(autoCompleteListBox.isVisible() && autoCompleteListBox.getSelectedRow()!=-1)
    {
        pos1 = getDocument().findWordBreakBefore(getCaretPos());
        pos2 = getCaretPos();
        getDocument().deleteSection(pos1, pos2);
        //cUtils::debug(currentWord);
        insertText(variableNamesToShow[autoCompleteListBox.getSelectedRow()]);
        autoCompleteListBox.setVisible(false);
        return;
    }

    if(getSelectedText().length()>0)
    {
        CodeDocument::Position startPos(this->getDocument(), getHighlightedRegion().getStart());
        CodeDocument::Position endPos(this->getDocument(), getHighlightedRegion().getEnd());
        this->getDocument().deleteSection(startPos, endPos);
        insertNewLine("\n");
    }
    else
        insertNewLine("\n");

    scrollToKeepCaretOnScreen();
    autoCompleteListBox.setVisible(false);
}
//==============================================================================
void CsoundCodeEditorComponent::editorHasScrolled()
{
    if(getParentComponent())
        this->getParentComponent()->repaint();
}
//==============================================================================
void CsoundCodeEditorComponent::mouseWheelMove (const MouseEvent& e, const MouseWheelDetails& mouse)
{

    if(e.mods.isCommandDown() && mouse.deltaY>0)
    {
        setFont(Font(font, (fontSize<100 ? ++fontSize : 100), 1));
        cUtils::setPreference(appProperties, "FontSize", String(fontSize));
    }
    else if(e.mods.isCommandDown() && mouse.deltaY<0)
    {
        setFont(Font(font, (fontSize>8 ? --fontSize : 8), 1));
        cUtils::setPreference(appProperties, "FontSize", String(fontSize));
    }
    else
    {
        if(mouse.deltaY<0)
            scrollBy(10);
        else
            scrollBy(-10);
    }
}

//==============================================================================
static void codeEditorMenuCallback (int menuResult, CsoundCodeEditorComponent* editor)
{
    if (editor != nullptr && menuResult != 0)
        editor->performPopupMenuAction (menuResult);
}

void CsoundCodeEditorComponent::mouseDown (const MouseEvent& e)
{
//    newTransaction();
    dragType = notDragging;

    if (e.mods.isPopupMenu())
    {
        setMouseCursor (MouseCursor::NormalCursor);

        if (getHighlightedRegion().isEmpty())
        {
            CodeDocument::Position start, end;
            getDocument().findTokenContaining (getPositionAt (e.x, e.y), start, end);

            if (start.getPosition() < end.getPosition())
                selectRegion (start, end);
        }

        PopupMenu m;
        m.setLookAndFeel (&getLookAndFeel());
        addPopupMenuItems (m, &e);

        m.showMenuAsync (PopupMenu::Options(),
                         ModalCallbackFunction::forComponent (codeEditorMenuCallback, this));
    }
    else
    {
        beginDragAutoRepeat (100);
        moveCaretTo (getPositionAt (e.x, e.y), e.mods.isShiftDown());
        autoCompleteListBox.setVisible(false);
        selectedTokens.clear();
        sendChangeMessage();
    }
}
//==============================================================================
void CsoundCodeEditorComponent::mouseDoubleClick (const MouseEvent& e)
{
    CodeDocument::Position tokenStart (getPositionAt (e.x, e.y));
    CodeDocument::Position tokenEnd (tokenStart);

    if (e.getNumberOfClicks() > 2)
        getDocument().findLineContaining (tokenStart, tokenStart, tokenEnd);
    else
        getDocument().findTokenContaining (tokenStart, tokenStart, tokenEnd);

    selectRegion (tokenStart, tokenEnd);

    String word = getDocument().getTextBetween(tokenStart, tokenEnd);


    int instances = 0;
    selectedTokens.clear();
    while(findText(word, true)!=-2);


    cUtils::debug(selectedTokens.size());
    sendChangeMessage();

    dragType = notDragging;
}

//==============================================================================
int CsoundCodeEditorComponent::findText(String text, bool multi)
{
    Range<int> positionOfWordInLine;
    String fullText = getDocument().getAllContent();
    String searchTerm = text;
    searchStartIndex = fullText.indexOf(searchStartIndex, searchTerm);
    if(searchStartIndex==-1)return -1;

    if(multi==true)
    {

        CodeDocument::Position start(getDocument(), fullText.indexOf(searchStartIndex, searchTerm));
        CodeDocument::Position end(getDocument(), fullText.indexOf(searchStartIndex, searchTerm)+searchTerm.length());
        const int lineNumber = start.getLineNumber();

        //deal with tabs
        String line = getDocument().getLine(lineNumber);
        if(line.contains("\t"))
        {
            line = line.replace("\t", "    ");
            const int startIndex = line.indexOf(0, searchTerm);
            const int endIndex =   line.indexOf(0, searchTerm)+searchTerm.length();
            positionOfWordInLine.setStart(startIndex);
            positionOfWordInLine.setEnd(endIndex);
        }

        else
        {
            positionOfWordInLine.setStart(start.getIndexInLine());
            positionOfWordInLine.setEnd(end.getIndexInLine());
        }

        selectedTokens.add(new SelectedToken(lineNumber, positionOfWordInLine));


    }

    else
    {
        moveCaretTo(CodeDocument::Position(getDocument(),
                                           fullText.indexOf(searchStartIndex, searchTerm)),
                    false);
        moveCaretTo(CodeDocument::Position(getDocument(),
                                           fullText.indexOf(searchStartIndex, searchTerm)+searchTerm.length()),
                    true);
    }

    searchStartIndex=searchStartIndex+searchTerm.length();

    if(fullText.indexOf(searchStartIndex, searchTerm)<searchStartIndex)
        searchStartIndex=-2;

    return searchStartIndex;
}

//==============================================================================
void CsoundCodeEditorComponent::insertText(String text)
{
    if(this->isHighlightActive())
    {
        getDocument().replaceSection(getHighlightedRegion().getStart(), getHighlightedRegion().getEnd(), "");
    }

    getDocument().insertText(getCaretPos(), text);

    if(variableNamesToShow.size()>0)
    {
        const int height = jmin(200, variableNamesToShow.size()*16);
        autoCompleteListBox.setBounds(getCaretRectangle().getX(),getCaretRectangle().getY()+16, 300, height);
    }
}
//==============================================================================
void CsoundCodeEditorComponent::insertNewLine(String text)
{
    pos1 = getCaretPos();
    StringArray csdArray;
    csdArray.addLines(getAllText());
    String curLine = csdArray[pos1.getLineNumber()];
    int numberOfTabs=0;
    int numberOfSpaces=0;
    String tabs;
    while(curLine.substring(numberOfTabs, numberOfTabs+1).equalsIgnoreCase("\t"))
    {
        tabs.append("\t", 8);
        numberOfTabs++;
    }

    while(curLine.substring(numberOfSpaces, numberOfSpaces+1).equalsIgnoreCase(" "))
    {
        tabs.append(" ", 8);
        numberOfSpaces++;
    }

    Logger::writeToLog("Number of tabs:"+String(numberOfTabs));
    getDocument().insertText(pos1, text+tabs);
}

//==============================================================================
bool CsoundCodeEditorComponent::pasteFromClipboard()
{
    const String clip (SystemClipboard::getTextFromClipboard());
    if (clip.isNotEmpty())
        if(!columnEditMode)
            insertText (clip);
        else
            insertMultiLineTextAtCaret(clip);

    return true;
}

bool CsoundCodeEditorComponent::cutToClipboard()
{
    CodeDocument::Position startPos(this->getDocument(), getHighlightedRegion().getStart());
    CodeDocument::Position endPos(this->getDocument(), getHighlightedRegion().getEnd());
    copyToClipboard();
    getDocument().deleteSection(startPos, endPos);
    moveCaretTo(startPos, false);
    return true;
}

//==============================================================================
void CsoundCodeEditorComponent::toggleComments()
{

    CodeDocument::Position startPos(this->getDocument(), getHighlightedRegion().getStart());
    CodeDocument::Position endPos(this->getDocument(), getHighlightedRegion().getEnd());

    cUtils::debug(startPos.getLineNumber());
    cUtils::debug(endPos.getLineNumber());

    StringArray selectedText;
    selectedText.addLines(getSelectedText());
    StringArray csdArray;
    csdArray.addLines(this->getAllText());


    for(int i=startPos.getLineNumber(); i<=endPos.getLineNumber(); i++)
    {
        String lineText = csdArray[i];
        if(lineText.trim().startsWith(";"))
            csdArray.set(i, lineText.substring(lineText.indexOf(";")+1));
        else
            csdArray.set(i, ";"+lineText);

    }

//
//    String lastLine;
//
//    for(int i=0; i<csdArray.size(); i++)
//        for(int y=0; y<selectedText.size(); y++)
//            if(selectedText[y]==csdArray[i])
//            {
//                if(!csdArray[i].equalsIgnoreCase(""))
//                {
//                    if(selectedText[y].substring(0, 1).equalsIgnoreCase(";"))
//                        csdArray.set(i, selectedText[y].substring(1));
//                    else
//                        csdArray.set(i, ";"+selectedText[y]);
//                    lastLine = selectedText[y].substring(1);
//                }
//            }
//
    this->setAllText(csdArray.joinIntoString("\n"));
    moveCaretTo(CodeDocument::Position(getDocument(), endPos.getLineNumber(), 1000), false);
}

//=================== addPopupMenuItems =======================
void CsoundCodeEditorComponent::addPopupMenuItems (PopupMenu &menuToAddTo, const MouseEvent *mouseClickEvent)
{
    menuToAddTo.addItem(1, "Cut");
    menuToAddTo.addItem(2, "Copy");
    menuToAddTo.addItem(3, "Paste");
    menuToAddTo.addItem(4, "Select All");
    menuToAddTo.addSeparator();
    menuToAddTo.addItem(14, "Toggle comments");
    menuToAddTo.addSeparator();
    menuToAddTo.addItem(13, "Opcode Help: Inline");
    menuToAddTo.addItem(5, "Opcode Help: Manual");
    menuToAddTo.addSeparator();
    menuToAddTo.addItem(6, "Mark region of interest");
    menuToAddTo.addItem(7, "Remove region of interest");
    menuToAddTo.addItem(11, "Add instrument breakpoint");
    menuToAddTo.addItem(12, "Remove instrument breakpoint");
    menuToAddTo.addSeparator();
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
Rectangle<int> CsoundCodeEditorComponent::getCaretPoisition()
{
    pos1 = getCaretPos();
    return getCharacterBounds(pos1);
}

void CsoundCodeEditorComponent::undoText()
{
    CodeDocument::Position startPos = getCaretPos();
    //getDocument().undo();
    moveCaretTo(startPos, false);
}
//==============================================================================
void CsoundCodeEditorComponent::changeListenerCallback(ChangeBroadcaster *source)
{
//	colourPallete* cs = dynamic_cast <ColourPallete*> (source);
//	if(cs->getNameOfParent()==name)
//		colour = cs->getCurrentColour();
//	repaint();
//	value.resize(0);
//	value.append(colour.getRed());
//	value.append(colour.getGreen());
//	value.append(colour.getBlue());
//	value.append(colour.getAlpha());
}
//==============================================================================
void CsoundCodeEditorComponent::performPopupMenuAction (int menuItemID)
{
    if(menuItemID==1)
        this->cutToClipboard();
    else if(menuItemID==2)
        this->copyToClipboard();
    else if(menuItemID==3)
        this->pasteFromClipboard();
    else if(menuItemID==4)
        this->selectAll();
    else if(menuItemID==5)
        sendActionMessage("Launch help");

    else if(menuItemID==6)
    {
        colourSelector.setBounds(0, 0, 300, 300);
        colourSelector.addChangeListener(this);
        colourSelector.setNameOfParent("Test");
        juce::Rectangle<int> rect;
        CabbageCallOutBox callOut (colourSelector, rect, nullptr);
        callOut.setTopLeftPosition(Desktop::getInstance().getDisplays().getMainDisplay().userArea.getCentre());
        callOut.setAlwaysOnTop(true);

#if !defined(AndroidBuild)
        callOut.runModalLoop();
#endif
        String colour = colourSelector.getCurrentColour().toString();

        CodeDocument::Position startPos(getDocument(), getHighlightedRegion().getStart());
        CodeDocument::Position endPos(getDocument(), getHighlightedRegion().getEnd());
        String message = "HightlightRegionOfInterest_"+String(startPos.getLineNumber())+":"+String(endPos.getLineNumber())+"="+colour;
        sendActionMessage(message);
    }

    else if(menuItemID==13)
    {
        pos1 = getDocument().findWordBreakBefore(getCaretPos());
        String line = getDocument().getLine(pos1.getLineNumber());
        showOpcodeInlineHelp(line);
    }

    else if(menuItemID==7)
    {
        String message = "RemoveRegionOfInterest_"+String(getCaretPos().getLineNumber());
        sendActionMessage(message);
    }

    else if(menuItemID==1000)
    {
        pos1 = getDocument().findWordBreakBefore(getCaretPos());
        String line = getDocument().getLine(pos1.getLineNumber());
        Logger::writeToLog(line);
        line = getTextInRange(this->getHighlightedRegion());
        Logger::writeToLog(line);
        sendActionMessage(line);
    }

    //add to repo
    else if(menuItemID==10)
    {
        addToRepository();
    }

    //add to repo
    else if(menuItemID==14)
    {
        toggleComments();
    }

    //insert from repo
    else if(menuItemID>=100)
    {
        ScopedPointer<XmlElement> xmlElement;
        xmlElement = appProperties->getUserSettings()->getXmlValue("CopeRepoXmlData");

        forEachXmlChildElement (*xmlElement, e)
        {
            if(e->getTagName()==repoEntries[menuItemID-100])
                insertText(e->getAllSubText());
        }
        xmlElement = nullptr;
    }

    else if(menuItemID==11)
        modifyInstrumentBreakpoint(false);

    else if(menuItemID==12)
        modifyInstrumentBreakpoint(true);

};

//==============================================================================
void CsoundCodeEditorComponent::modifyInstrumentBreakpoint(bool remove)
{
    int index=0;
    CodeDocument::Position startPos(getDocument(), getCaretPos().getPosition());
    StringArray csdArray;
    csdArray.addLines(getAllText());
    while(!(csdArray[startPos.getLineNumber()-index].contains("instr ")) &&
            (startPos.getLineNumber()-index>0))
        index++;

    String instrumentLine = csdArray[startPos.getLineNumber()-index];
    String instrumentNumber = instrumentLine.substring(instrumentLine.indexOf("instr")+5,
                              (instrumentLine.indexOf(";")>0 ? instrumentLine.indexOf(";") : 1000)).trim();
    if(!remove)
    {
        //this->highlightLines(startPos.getLineNumber()-index, startPos.getLineNumber()-index);
        sendActionMessage("SetInstrumentBreakpoint:"+String(instrumentNumber)+"_"+String(startPos.getLineNumber()-index));
    }
    else
        sendActionMessage("RemoveInstrumentBreakpoint:"+String(instrumentNumber)+"_"+String(startPos.getLineNumber()-index));

}

//==============================================================================
void CsoundCodeEditorComponent::addRepoToSettings()
{

}

//==============================================================================
void CsoundCodeEditorComponent::enableColumnEditMode(bool enable)
{
    if(enable)
    {
        setColour(CodeEditorComponent::highlightColourId, Colours::cornflowerblue.withAlpha(.0f));
        setColour(CaretComponent::caretColourId, Colours::yellow);
        columnEditMode=true;
    }
    else
    {
        setColour(CodeEditorComponent::highlightColourId, Colours::cornflowerblue.withAlpha(.2f));
        setColour(CaretComponent::caretColourId, Colours::white);
        columnEditMode=false;
    }
}


//==============================================================================
void CsoundCodeEditorComponent::updateCaretPosition()
{
    //Logger::writeToLog("Updating caret position");
    if(columnEditMode==1)
    {
        StringArray selectedText;
        selectedText.addLines(getTextInRange(this->getHighlightedRegion()));
        if(this->getHighlightedRegion().getLength()==0)
        {
            setCaretPos(getCharacterBounds (getCaretPos()));
            return;
        }
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
void CsoundCodeEditorComponent::addToRepository()
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

    if(repoEntryName!="enter name")
    {
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
String CsoundCodeEditorComponent::getLineText()
{
    StringArray csdLines;
    csdLines.addLines(getDocument().getAllContent());
    pos1 = getDocument().findWordBreakBefore(getCaretPos());
    Logger::writeToLog(csdLines[pos1.getLineNumber()]);
    return csdLines[pos1.getLineNumber()];
}

//==============================================================================
String CsoundCodeEditorComponent::getAllText()
{
    return getDocument().getAllContent();
}
//==============================================================================
void CsoundCodeEditorComponent::setAllText(String text)
{
    getDocument().replaceAllContent(text);
}
//==============================================================================
String CsoundCodeEditorComponent::getTempChannelInstr()
{
    String channel = "event_i \"i\", 999.999, 0, .1\n";
    channel << "instr 999\n";
    channel << getLineText() << "\n";
    channel << "endin\n";
    Logger::writeToLog(channel);
    return channel;
}
//==============================================================================
String CsoundCodeEditorComponent::getSelectedText()
{
    String selectedText = getTextInRange(this->getHighlightedRegion());
    Logger::writeToLog(selectedText);
    return selectedText;
}
//==============================================================================
StringArray CsoundCodeEditorComponent::getSelectedTextArray()
{
    StringArray tempArray;
    String selectedText = getTextInRange(this->getHighlightedRegion());
    tempArray.addLines(selectedText);
    Logger::writeToLog(selectedText);
    return tempArray;
}
//==============================================================================
String CsoundCodeEditorComponent::getInstrumentText()
{
    StringArray csdLines;
    csdLines.addLines(getDocument().getAllContent());
    pos1 = getDocument().findWordBreakBefore(getCaretPos());
    int startOfInstrDef, endOfInstrDef;
    int index = pos1.getLineNumber();
    while(index>0)
    {
        if(csdLines[index].contains("instr"))
        {
            startOfInstrDef = index-1;
            break;
        }
        else
            index--;
    }

    index = pos1.getLineNumber();
    while(index>0)
    {
        if(csdLines[index].contains("endin"))
        {
            endOfInstrDef = index;
            break;
        }
        else
            index++;
    }

    String selectedText="";
    for(int i = startOfInstrDef; i<=endOfInstrDef; i++)
        selectedText += csdLines[i]+"\n";
    Logger::writeToLog(selectedText);
    return selectedText;
}
//==============================================================================
void CsoundCodeEditorComponent::parseTextForVariables()
{
    String csdText = getDocument().getAllContent();
    StringArray tokens;
    variableNames.clear();
    tokens.addTokens(csdText, "  (),*%=\t", "");

    for(int i=0; i<tokens.size(); i++)
    {
        const String currentWord = tokens[i];
        if(currentWord.startsWith("a") || currentWord.startsWith("i") ||
                currentWord.startsWith("k") ||currentWord.startsWith("S") ||
                currentWord.startsWith("f"))
        {
            if(!currentWord.contains("\n") && currentWord.isNotEmpty())
                variableNames.addIfNotAlreadyThere(currentWord);
        }
    }
}

//==============================================================================
void CsoundCodeEditorComponent::showOpcodeInlineHelp(String lineFromCsd)
{
    String opcodeHelpString;
    StringArray syntaxTokens, csdLineTokens;
    csdLineTokens.clear();
    csdLineTokens.addTokens(lineFromCsd, " ,\t", "");

    for(int i=0; i<opcodeStrings.size(); i++)
    {
        opcodeHelpString = opcodeStrings[i];
        syntaxTokens.clear();
        syntaxTokens.addTokens(opcodeHelpString, ";", "\"");
        if(syntaxTokens.size()>3)
            for(int x=0; x<csdLineTokens.size(); x++)
            {
                //Logger::writeToLog(csdLineTokens[x]);
                if(syntaxTokens[0].removeCharacters("\"")==csdLineTokens[x].trim())
                {
                    if(syntaxTokens[0].length()>3)
                    {
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
//==============================================================================
void CsoundCodeEditorComponent::showAutoComplete(String currentWord)
{
    for (int i = 0; i < variableNames.size(); ++i)
    {
        const String item (variableNames[i]);
        if (item.startsWith (currentWord))
        {
            variableNamesToShow.addIfNotAlreadyThere(item.trim());
            autoCompleteListBox.updateContent();
            autoCompleteListBox.setVisible(true);
        }
    }
}
//==============================================================================
void CsoundCodeEditorComponent::codeDocumentTextInserted(const juce::String &text,int)
{

    pos1 = getDocument().findWordBreakBefore(getCaretPos());
    pos2 = getDocument().findWordBreakAfter(getCaretPos());
    String currentWord = getDocument().getTextBetween(pos1, pos2);
    cUtils::debug(currentWord);
    currentWord = currentWord.trim();

    if(currentWord.startsWith("a") || currentWord.startsWith("i") ||
            currentWord.startsWith("k") ||currentWord.startsWith("S") ||
            currentWord.startsWith("f"))
    {
        if(text==" " && currentWord.isNotEmpty())
        {
            variableNames.addIfNotAlreadyThere(currentWord);
            autoCompleteListBox.updateContent();
        }
    }

    variableNamesToShow.clear();
    autoCompleteListBox.setVisible(false);

    if(currentWord.isNotEmpty())
    {
        showAutoComplete(currentWord);
    }


    const String lineFromCsd = getDocument().getLine(pos1.getLineNumber());
    showOpcodeInlineHelp(lineFromCsd);

}
//==============================================================================
bool CsoundCodeEditorComponent::deleteBackwards (const bool moveInWholeWordSteps)
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

        for(int i=startPos.getLineNumber(); i<endPos.getLineNumber()+1; i++)
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

        pos1 = getDocument().findWordBreakBefore(getCaretPos());
        pos2 = getDocument().findWordBreakAfter(getCaretPos());
        String currentWord = getDocument().getTextBetween(pos1, pos2);
        variableNamesToShow.clear();
        autoCompleteListBox.setVisible(false);

    }

    scrollToKeepCaretOnScreen();

    return true;
}
//==============================================================================
bool CsoundCodeEditorComponent::deleteForwards (const bool moveInWholeWordSteps)
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

        for(int i=startPos.getLineNumber(); i<endPos.getLineNumber()+1; i++)
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

        pos1 = getDocument().findWordBreakBefore(getCaretPos());
        pos2 = getDocument().findWordBreakAfter(getCaretPos());
        String currentWord = getDocument().getTextBetween(pos1, pos2);
        variableNamesToShow.clear();
        autoCompleteListBox.setVisible(false);

        if(currentWord.isNotEmpty())
        {
            showAutoComplete(currentWord);
        }
    }
    return true;
}
//==============================================================================
void CsoundCodeEditorComponent::insertTextAtCaret (const String &textToInsert)
{
    if(!columnEditMode)
        insertText(textToInsert);
    else
        insertMultiLineTextAtCaret(textToInsert);
}
//==============================================================================
void CsoundCodeEditorComponent::insertMultiLineTextAtCaret (String text)
{
    //sendActionMessage("make popup invisible");
    StringArray csdArray;
    csdArray.addLines(getAllText());
    String curLine;
    CodeDocument::Position newPos, indexPos;

    CodeDocument::Position startPos(getDocument(), getHighlightedRegion().getStart());
    CodeDocument::Position endPos(getDocument(), getHighlightedRegion().getEnd());
    int indexInLine = startPos.getIndexInLine();


    for(int i=startPos.getLineNumber(); i<endPos.getLineNumber()+1; i++)
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
void CsoundCodeEditorComponent::handleTabKey(String direction)
{
    /* multi line action, get highlited text, find the position of
     * it within the text editor, remove it from editor and reinsert it with
     * formatting
     */

    StringArray selectedText, csdArray;
    //selectedText.addLines(getSelectedText());
    csdArray.addLines(getAllText());
    String csdText;
    CodeDocument::Position startPos(this->getDocument(), getHighlightedRegion().getStart());
    CodeDocument::Position endPos(this->getDocument(), getHighlightedRegion().getEnd());
    pos1 = getCaretPos();

    if(direction.equalsIgnoreCase("forwards"))
    {
        //single line tab
        if(getHighlightedRegion().getLength()==0)
        {
            insertTabAtCaret();
            return;
        }
        else
        {
            //multiline tab
            for(int i=startPos.getLineNumber(); i<endPos.getLineNumber()+1; i++)
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
        else
        {
            for(int i=startPos.getLineNumber(); i<endPos.getLineNumber()+1; i++)
                if(csdArray[i].substring(0, 1).equalsIgnoreCase("\t"))
                    csdArray.set(i, csdArray[i].substring(1));

            setAllText(csdArray.joinIntoString("\n"));
            highlightLines(startPos.getLineNumber(), endPos.getLineNumber());
        }

}
//==============================================================================
bool CsoundCodeEditorComponent::moveCaretLeft (const bool moveInWholeWordSteps, const bool selecting)
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
bool CsoundCodeEditorComponent::moveCaretRight (const bool moveInWholeWordSteps, const bool selecting)
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
void CsoundCodeEditorComponent::codeDocumentTextDeleted(int start,int end)
{

}
