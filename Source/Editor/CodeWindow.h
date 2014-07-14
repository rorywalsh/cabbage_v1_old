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
#include "SplitComponent.h"
#include "../Plugin/CabbagePluginProcessor.h"
//class LiveCsound;
class PythonEditor;

//========== csound output console class =================
class CsoundOutputComponent : public Component
{
    ScopedPointer<TextEditor> textEditor;
public:
    CsoundOutputComponent(String title): Component()
    {
        textEditor = new TextEditor();
        textEditor->setColour(Label::outlineColourId, Colours::white);
        textEditor->setColour(TextEditor::backgroundColourId, CabbageUtils::getDarkerBackgroundSkin());
        textEditor->setColour(TextEditor::textColourId, Colours::cornflowerblue);
        textEditor->setMultiLine(true);
        textEditor->setFont(Font("Arial", 14, 0));
        addAndMakeVisible(textEditor, true);
        setText("hewe;roiuew asdfhsa ldkjfhsd lfauskdhf lsadiufh dslifuhsadlifu hdsaflisaud hflidsau hflidsa uf");
    };

    ~CsoundOutputComponent() {};


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

    void resized()
    {
        textEditor->setBounds(20, 21, getWidth()-25, getHeight()-30);
    }

    void paint(Graphics& g)
    {
        g.fillAll(CabbageUtils::getDarkerBackgroundSkin());
        g.setColour(Colours::white);
        g.drawRoundedRectangle(getLocalBounds().toFloat(), 2, 2);
        g.drawFittedText("Csound output", getLocalBounds().withHeight(18), Justification::centred, 1, 1.f);
    }

};

//========== csound debugger output console class =================
class CsoundDebuggerComponent : public Component
{
    ScopedPointer<TextEditor> textEditor;
public:
    CsoundDebuggerComponent(String title): Component()
    {
        textEditor = new TextEditor();
        textEditor->setColour(Label::outlineColourId, Colours::white);
        textEditor->setColour(TextEditor::backgroundColourId, CabbageUtils::getDarkerBackgroundSkin());
        textEditor->setColour(TextEditor::textColourId, Colours::white);
        textEditor->setMultiLine(true);
#if defined(WIN32)
        textEditor->setFont(Font(String("Consolas"), 13, 1));
#elif defined(MACOSX)
        textEditor->setFont(Font(String("Courier New"), 13, 1));
#else
        textEditor->setFont(Font(String("Droid Sans Mono"), 13, 1));
#endif
        addAndMakeVisible(textEditor, true);
    };

    ~CsoundDebuggerComponent() {};


    void setText(String text)
    {
        StringArray textArray;
        textArray.addLines(text);

        textArray.removeEmptyStrings(true);
        for(int i=2; i<textArray.size(); i++)
        {
            StringArray tempArray;
            String formattedLine="";
            tempArray.addTokens(textArray[i], " ");
            tempArray.removeDuplicates(true);
            tempArray.removeEmptyStrings();

            for(int j=0; j<tempArray.size(); j++)
            {
                formattedLine+=tempArray[j].removeCharacters(" ").paddedRight(' ', 20);
                textArray.set(i, formattedLine);
            }


        }

        textEditor->setColour(TextEditor::textColourId, Colours::lime);
        textEditor->setText(textArray.joinIntoString("\n"));
    }

    String getText()
    {
        return textEditor->getText();
    }

    void resized()
    {
        textEditor->setBounds(20, 21, getWidth()-25, getHeight()-30);
    }

    void paint(Graphics& g)
    {
        g.fillAll(CabbageUtils::getDarkerBackgroundSkin());
        g.setColour(Colours::white);
        g.drawRoundedRectangle(getLocalBounds().toFloat(), 2, 2);
        g.drawFittedText("Debugger output", getLocalBounds().withHeight(18), Justification::centred, 1, 1.f);
    }

};
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
    StringArray getMenuBarNames();
    void getAllCommands (Array <CommandID>& commands);
    void menuItemSelected (int menuItemID, int topLevelMenuIndex);
    void setFontSize(String zoom);

    //================= command methods ====================
    ApplicationCommandTarget* getNextCommandTarget()
    {
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

    juce::Rectangle<int> getCaretScreenPosition()
    {
        juce::Rectangle<int> rect(textEditor->editor[textEditor->currentEditor]->getCaretPoisition());
        rect.setLeft(rect.getX()+this->getTopLevelComponent()->getX()+100);
        rect.setTop(rect.getY()+this->getTopLevelComponent()->getY()+45);
        return rect;
    }


    void closeButtonPressed()
    {
        //JUCEApplication::getInstance()->systemRequestedQuit();
        this->setVisible(false);
    }

    void codeDocumentTextDeleted(int,int) {}
    void showCabbageHelp();
    void codeDocumentTextInserted(const juce::String &,int) {}
    void codeDocumentChanged (const CodeDocument::Position &affectedTextStart, const CodeDocument::Position &affectedTextEnd);
    void insertFromRepo();

    void setText(String text, String file)
    {
        textEditor->setAllText(text);
        textEditor->openFiles.getReference(0) = file;
    }


    String getText()
    {
        return textEditor->getAllText();
    }


    void setColourScheme(String theme);
    void newFile(String type);
    bool unSaved;
    StringArray recentEvents;
    String csoundOutputText;
    bool firstTime;
    String debugMessage;
    bool showOutput;
    CommandManager commandManager;
    File csdFile;
    int fontSize;
    String ASCIICabbage;
    ScopedPointer<SplitComponent> splitWindow;
    ScopedPointer<SplitComponent> splitBottomWindow;
    ScopedPointer<CsoundOutputComponent> csoundOutputComponent;
    ScopedPointer<CsoundDebuggerComponent> csoundDebuggerComponent;

    StringArray opcodeStrings;
    CsoundCodeEditor* textEditor;
    CsoundTokeniser csoundToker;
    Font font;
    bool isColumnModeEnabled;
    bool isInstrTabEnabled;
    bool isCsoundOutputEnabled;
};



#endif  // __MAINCOMPONENT_H_615ECE56__
