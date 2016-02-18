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
#include "../CabbageCallOutBox.h"


extern ApplicationProperties* appProperties;
//extern CabbageTimer* cabbageTimer;


class FlatButton;
class HelpComp;
class SearchReplaceComp;

//============================================================
// class for displaying colour selector
//============================================================
class ColourPallete : public ColourSelector
{
public:
    ColourPallete(): ColourSelector()
    {
        setColour(ColourSelector::backgroundColourId, Colours::black);
        setColour(ColourSelector::labelTextColourId, Colours::white);
        //setup swatches for colour selector.
        swatchColours.set(0, Colour(0xFF000000));
        swatchColours.set(1, Colour(0xFFFFFFFF));
        swatchColours.set(2, Colour(0xFFFF0000));
        swatchColours.set(3, Colour(0xFF00FF00));
        swatchColours.set(4, Colour(0xFF0000FF));
        swatchColours.set(5, Colour(0xFFFFFF00));
        swatchColours.set(6, Colour(0xFFFF00FF));
        swatchColours.set(7, Colour(0xFF00FFFF));
        swatchColours.set(8, Colour(0x80000000));
        swatchColours.set(9, Colour(0x80FFFFFF));
        swatchColours.set(10, Colour(0x80FF0000));
        swatchColours.set(11, Colour(0x8000FF00));
        swatchColours.set(12, Colour(0x800000FF));
        swatchColours.set(13, Colour(0x80FFFF00));
        swatchColours.set(14, Colour(0x80FF00FF));
        swatchColours.set(15, Colour(0x8000FFFF));
    };

    ~ColourPallete() {};


    int getNumSwatches() const
    {
        return swatchColours.size();
    }

    Colour getSwatchColour(int index) const
    {
        return swatchColours[index];
    }

    void setSwatchColour (const int index, const Colour &newColour) const
    {
        swatchColours.getReference(index) = newColour;
    }

    String getNameOfParent()
    {
        return nameOfParent;
    }

    void setNameOfParent(String parent)
    {
        nameOfParent = parent;
    }

private:
    String nameOfParent;
    Array <Colour> swatchColours;

};

//============================================================
// code editor
//============================================================
class CsoundCodeEditorComponenet : public CodeEditorComponent,
    public ActionBroadcaster,
    public ChangeBroadcaster,
    public ChangeListener,
    public CodeDocument::Listener
{
public:
    CodeDocument::Position positionInCode;
    CsoundCodeEditorComponenet(String type, CodeDocument &document, CodeTokeniser *codeTokeniser);
    ~CsoundCodeEditorComponenet();
    bool keyPressed (const KeyPress& key);
    void handleTabKey(String direction);
    void toggleComments();
    void handleDirectionKey() {}

    void handleEscapeKey()
    {
        if(type=="python")
            sendActionMessage("make python invisible");
        if(type=="csound")
            sendActionMessage("make popup invisible");
    }

    enum DragType
    {
        notDragging,
        draggingSelectionStart,
        draggingSelectionEnd
    };

    DragType dragType;

    void editorHasScrolled();
    bool skipBackwardsToPreviousTab();
    bool moveCaretLeft (const bool moveInWholeWordSteps, const bool selecting);
    bool moveCaretRight (const bool moveInWholeWordSteps, const bool selecting);
    bool deleteBackwards (const bool moveInWholeWordSteps);
    bool deleteForwards (const bool moveInWholeWordSteps);
    void handleReturnKey ();
    void addPopupMenuItems (PopupMenu &menuToAddTo, const MouseEvent *mouseClickEvent);
    void performPopupMenuAction (int menuItemID);
    void modifyInstrumentBreakpoint(bool remove);
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
    void undoText();
    void setAllText(String text);
    void highlightLine(String line);
    void highlightLines(int firstLine, int lastLine);
    void codeDocumentTextDeleted(int,int);
    void codeDocumentTextInserted(const juce::String &,int);
    bool pasteFromClipboard();
    bool cutToClipboard();
    void insertNewLine(String text);

    void enableColumnEditMode(bool enable);
    void setOpcodeStrings(String opcodes)
    {
        opcodeStrings.addLines(opcodes);
    }

    void insertTextAtCaret (const String &textToInsert);
    void updateCaretPosition();
    void insertMultiLineTextAtCaret(String text);
    String getOpcodeToken(int index)
    {
        return opcodeTokens[index];
    }

    bool columnEditMode;
    void mouseWheelMove (const MouseEvent&, const MouseWheelDetails&);


    void setBackgroundColour(Colour col)
    {
        backgroundColour = col;
    }

    void changeListenerCallback(ChangeBroadcaster *source);

    ColourPallete colourSelector;

private:
    int xPos, yPos, prevXpos;
    CodeDocument::Position pos1, pos2;
    Colour selectedColour, backgroundColour;
    int fontSize;
    String font;
    String type;
    StringArray opcodeStrings;
    StringArray opcodeTokens;


};

//============================================================
// class that holds our code editor
//============================================================
class CsoundCodeEditor : public Component,
    public ChangeListener,
    public ActionListener,
    public ActionBroadcaster
{
    bool showTabButtons;
    bool showInstrumentButtons;
    int instrWidth;
    int searchStartIndex;
    bool highlightedWord;
    Array<int> breakpointLines;

    class SelectedRegion
    {
        Range<int> lines;
        Colour colour;
    public:
        SelectedRegion(Range<int> _lines, Colour _col):lines(_lines), colour(_col)
        {}
        ~SelectedRegion() {}

        Range<int> getLinesRange()
        {
            return lines;
        }
        Colour getColour()
        {
            return colour;
        }
    };

public:
    CsoundCodeEditor(CodeDocument &document, CodeTokeniser *codeTokeniser);
    ~CsoundCodeEditor();

    int saveAllFiles();
    void setSavePoint();

    void setShowTabButtons(bool show)
    {
        showTabButtons = show;
    }
    void changeListenerCallback(juce::ChangeBroadcaster* source);
    String getAllText();
    void setAllText(String text);
    void highlightLine(String line);
    void showTab(String name);
    void showInstrs(bool show);
    int findText(String text);
    String getCurrentSelectedText();
    int replaceText(String text, String replaceWith);
    void actionListenerCallback(const juce::String&);
    void closeCurrentFile();
    void setActiveTab(int index);
    void saveAuxFile();
    OwnedArray<FlatButton> tabButtons;
    OwnedArray<FlatButton> instrButtons;
    ScopedPointer<HelpComp> helpComp;
    ScopedPointer<SearchReplaceComp> searchReplaceComp;
    bool textChanged;
    OwnedArray<CsoundCodeEditorComponenet> editor;
    int currentEditor;
    void addNewFile(File newFile);
    void paint(Graphics& g);
    void resized();
    Range<int> getCabbageSectionRange();
    OwnedArray<CodeDocument> codeDocuments;
    CsoundTokeniser codeToker;
    StringArray openFiles;
    int documentIndex;
    void enableColumnEdit(bool enable);
    Colour backgroundColour;

    Array<SelectedRegion> selectedRegions;
};


class FlatButton : public Component,
    public ChangeBroadcaster
{
public:
    FlatButton(String name, int currentTab, String type): 	name(name),
        currentTab(currentTab),
        type(type),
        isMouseDown(false),
        genericColour(Colours::brown)
    {
        setName(name);
        if(name=="Csound code")
            active = true;
        else
            active=false;


    }
    ~FlatButton() {}

    void mouseDown(const MouseEvent& event)
    {
        sendChangeMessage();
        Logger::writeToLog("mouseDown");
        isMouseDown = true;
        repaint();
    }

    void mouseUp(const MouseEvent& event)
    {
        Logger::writeToLog("mouseUp");
        isMouseDown = false;
        repaint();
    }

    void isActive(bool isActive)
    {
        active = isActive;
        repaint();
    }


    void paint(Graphics &g)
    {
        g.fillAll(Colour(20, 20, 20));
        if(type=="Native")
        {
            if(name.contains("code"))
            {
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
        else if(type=="Aux")
        {
            if(!active)
                g.setColour(genericColour.darker(.9f));
            else
                g.setColour(genericColour.darker(.2f));
            g.fillRoundedRectangle(0, 0, getWidth(), getHeight(), 5.f);

            if(!active)
                g.setColour(Colours::white.darker(.9f));
            else
                g.setColour(Colours::white);

            g.drawRoundedRectangle(0, 0, getWidth(), getHeight(), 5.f, 2);
            g.drawFittedText(name, 0, 0, getWidth(), getHeight(), Justification::centred, 1, 1.f);

        }

        else
        {
            if(isMouseDown)
                g.setColour(genericColour.darker(.2f));
            else
                g.setColour(genericColour.darker(.6f));

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
    Colour genericColour;

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

    ~HelpComp() {};

    void setText(String _info, String _syntax)
    {
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

    void resized()
    {
        setSize(getWidth(), getHeight());
    }

    bool firstTime;
private:

    String syntax, info;

};

//============================================================
// class for displaying SearchReplaceComp
//============================================================
class SearchReplaceComp : public Component,
    public TextEditor::Listener,
    public ChangeBroadcaster
{
public:
    ScopedPointer<FlatButton> searchButton, replaceAllButton, replaceOnceButton;

    SearchReplaceComp()
    {
        addAndMakeVisible(searchEditor = new TextEditor("SearchEditor"));
        searchEditor->addListener(this);
        addAndMakeVisible(replaceEditor = new TextEditor("ReplaceWithEditor"));
        replaceEditor->addListener(this);
        addAndMakeVisible(searchButton = new FlatButton("Search", 0, ""));
        searchButton->setName("SearchButton");
        addAndMakeVisible(replaceOnceButton = new FlatButton("Replace Once", 0, ""));
        replaceOnceButton->setName("ReplaceOnceButton");
        replaceOnceButton->genericColour = Colours::cornflowerblue;
        addAndMakeVisible(replaceAllButton = new FlatButton("Replace All", 0, ""));
        replaceAllButton->genericColour = Colours::cornflowerblue;
        replaceAllButton->setName("ReplaceAllButton");
        addAndMakeVisible(findLabel = new Label("FindLabel"));
        addAndMakeVisible(replaceLabel = new Label("ReplaceLabel"));

        replaceLabel->setText("Replace with:", dontSendNotification);
        replaceLabel->setColour(Label::textColourId, Colours::whitesmoke);
        findLabel->setText("Find:", dontSendNotification);
        findLabel->setColour(Label::textColourId, Colours::whitesmoke);

        replaceEditor->setColour(TextEditor::backgroundColourId, Colours::whitesmoke);
        replaceEditor->setColour(TextEditor::textColourId, Colours::black);
        replaceEditor->setColour(CaretComponent::caretColourId, Colours::black);

        searchEditor->setColour(TextEditor::backgroundColourId, Colours::whitesmoke);
        searchEditor->setColour(TextEditor::textColourId, Colours::black);
        searchEditor->setColour(CaretComponent::caretColourId, Colours::black);
    }

    ~SearchReplaceComp() {};

    void paint(Graphics& g)
    {
        g.fillAll(cUtils::getDarkerBackgroundSkin());
    }

    void resized()
    {
        findLabel->setBounds(0, 4, 30, 20);
        searchEditor->setBounds(35, 4, 150, 20);
        searchButton->setBounds(190, 4, 100, 20);
        replaceLabel->setBounds(295, 4, 50, 20);
        replaceEditor->setBounds(350, 4, 150, 20);
        replaceOnceButton->setBounds(505, 4, 100, 20);
        replaceAllButton->setBounds(610 , 4, 100, 20);
    }

    String getSearchText()
    {
        return searchEditor->getText();
    }

    void setSearchText(String text)
    {
        if(! text.containsAnyOf ("\r\n"))
            searchEditor->setText(text);
        else
            searchEditor->setText("");
    }

    String getReplaceText()
    {
        return replaceEditor->getText();
    }

    void textEditorReturnKeyPressed (TextEditor &editor)
    {
        currentEditorName = editor.getName();
        sendChangeMessage();
    }

    String getCurrentTextEditor()
    {
        return currentEditorName;
    }

private:

    String currentEditorName;
    ScopedPointer<TextEditor> searchEditor, replaceEditor;
    ScopedPointer<Label> findLabel, replaceLabel;
    String syntax, info;

};
#endif