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

#ifndef __PLUGINEDITOR_H_F4EBBBA1__
#define __PLUGINEDITOR_H_F4EBBBA1__

#include  "../CabbageCustomWidgets.h"
#include "../../JuceLibraryCode/JuceHeader.h"
#include "CabbagePluginProcessor.h"
#include "../CabbagePropertiesDialog.h"
#include "../CabbageUtils.h"

extern CabbageLookAndFeel* lookAndFeel;
extern CabbageLookAndFeelBasic* lookAndFeelBasic;

#ifdef Cabbage_Build_Standalone
extern ApplicationProperties* appProperties;
#endif

class CabbageMainPanel;
class ComponentLayoutEditor;
class CabbageCornerResizer;
//==============================================================================
// window used to display popup plants
//==============================================================================
class CabbagePlantWindow  : public DocumentWindow
{

public:
    CabbagePlantWindow(const String& title, const Colour& backgroundColour)
        : DocumentWindow (title, backgroundColour, DocumentWindow::closeButton)
    {
        setName(title);
    };

    ~CabbagePlantWindow()
    {
    };

//	void mouseMove(const MouseEvent& event)
//	{
//		Logger::writeToLog("planty");
//		//Logger::writeToLog(String(this->getContentComponent()->getMouseXYRelative().getX()));
//	}
//
//
    void closeButtonPressed()
    {
        setMinimised(true);
        setVisible(false);
        setAlwaysOnTop(false);

    };

    TooltipWindow tooltipWindow;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbagePlantWindow);
};
//==============================================================================
// component that sits inside of main viewport class
//==============================================================================
class CabbageViewportComponent : public Component
{

public:
    CabbageViewportComponent() : Component("CabbageViewportComponent")
    {
        this->setInterceptsMouseClicks(false, true);
    }
    ~CabbageViewportComponent() {}

    void paint(Graphics &g)
    {
        Viewport* const viewport = findParentComponentOfClass<Viewport>(); //Get the parent viewport
        if(viewport != nullptr) //Check for nullness
        {
            juce::Rectangle<int> viewRect(viewport->getViewPositionX(), viewport->getViewPositionY(), viewport->getViewWidth(), viewport->getViewHeight()); //Get the current displayed area in the viewport
        }
    }
};

//==============================================================================
// pointData class for holding info about each segment of a line
//==============================================================================
class PointData
{
public:
    Point <int> point;
    int curveType;
    PointData(Point<int> point, int curveType):point(point),curveType(curveType)
    {}
    ~PointData() {}
};

//==============================================================================
// main GUI editor window
//==============================================================================
class CabbagePluginAudioProcessorEditor  : public AudioProcessorEditor,
    public cUtils,
    public SliderListener,
    public ComboBoxListener,
    public ButtonListener,
    public KeyListener,
    public ChangeBroadcaster,
    public ChangeListener,
    public ActionListener,
    public ActionBroadcaster,
    public Timer

{
public:
    CabbagePluginAudioProcessorEditor (CabbagePluginAudioProcessor* ownerFilter);
    ~CabbagePluginAudioProcessorEditor();

    void paint (Graphics& g);
    void resized();
    void setEditMode(bool on);
    void InsertGUIControls(CabbageGUIType cAttr);
    void ksmpsYieldCallback();
    void refreshDiskReadingGUIControls(String typeOfControl);
    void updateSize();
    void restoreParametersFromPresets(XmlElement* xmlData);
    //used in Android mode to rescale according to screen res
    ScopedPointer<CabbagePropertiesDialog> propsWindow;
    //main GUI controls vectors..
    OwnedArray<Component> comps;
    OwnedArray<Component> layoutComps;
    void updateLayoutEditorFrames();
    ScopedPointer<CabbageTable> cabTable;
    ScopedPointer<CabbageCornerResizer> resizer;
    Point<int> getMousePos();
    Array<int> popupMenus;
    void updateGUIControls();
    OwnedArray<CabbagePlantWindow> subPatches;
    //CabbagePluginAudioProcessor* filter;
    CabbagePluginAudioProcessor* getFilter() const
    {
        return static_cast <CabbagePluginAudioProcessor*> (getAudioProcessor());
    }

    int currentPopupIndex;
    void resizeChildren();
    void showBubble(Component* comp, String message)
    {
        popupBubble->showAt(comp, AttributedString(message), 550);
    }

private:
    WildcardFileFilter wildcardFilter;
    Image logo1, logo2;
    void setPositionOfComponent(float x, float y, float width, float height, Component* comp, String reltoplant);
    void createfTableData(Table* table, bool sendToCsound);
    bool keyPressed(const juce::KeyPress &,Component *);
    void updateSizesAndPositionsOfComponents(int newLine = 0);
    void deleteComponents();
    void duplicateComponents();
    void populateLineNumberArray(StringArray csdArray);
    void addToRepository(String entryName);
    void convertIntoPlant();
    void breakUpPlant();
    void sendBack(bool toBack);
    void sendForward(bool toFront);
    void InsertGroupBox(CabbageGUIType &cAttr);
    void comboBoxChanged (ComboBox* combo);
    void InsertComboBox(CabbageGUIType &cAttr);
    void InsertSoundfiler(CabbageGUIType &cAttr);
    void InsertSourceButton(CabbageGUIType &cAttr);
    void InsertDirectoryList(CabbageGUIType &cAttr);
    void setupWindow(CabbageGUIType &cAttr);
    void InsertSlider(CabbageGUIType &cAttr);
    void InsertTextEditor(CabbageGUIType &cAttr);
    void sliderValueChanged (Slider*);
    void InsertButton(CabbageGUIType &cAttr);
    void InsertCheckBox(CabbageGUIType &cAttr);
    void InsertCsoundOutput(CabbageGUIType &cAttr);
    void InsertMIDIKeyboard(CabbageGUIType &cAttr);
    void InsertXYPad(CabbageGUIType &cAttr);
    void InsertFileButton(CabbageGUIType &cAttr);
    void InsertRecordButton(CabbageGUIType &cAttr);
    void InsertImage(CabbageGUIType &cAttr);
    void InsertLabel(CabbageGUIType &cAttr);
    void InsertTable(CabbageGUIType &cAttr);
    void InsertMultiTab(CabbageGUIType &cAttr);
    void InsertEncoder(CabbageGUIType &cAttr);
    void InsertInfoButton(CabbageGUIType &cAttr);
    void InsertLineSeparator(CabbageGUIType &cAttr);
    void InsertScope(CabbageGUIType &cAttr);
    void InsertPatternMatrix(CabbageGUIType &cAttr);
    void InsertSnapshot(CabbageGUIType &cAttr);
    void InsertTransport(CabbageGUIType &cAttr);
    void InsertListbox(CabbageGUIType &cAttr);
    void InsertPopupMenu(CabbageGUIType &cAttr);
    void InsertGenTable(CabbageGUIType &cAttr);
    void InsertTextbox(CabbageGUIType &cAttr);
    void InsertRangeSlider(CabbageGUIType &cAttr);
    void InsertSignalDisplay(CabbageGUIType &cAttr);
    //example insert stepper method
    void InsertStepper(CabbageGUIType &cAttr);
    void InsertNumberBox(CabbageGUIType &cAttr);
    void buttonClicked(Button*);
    void textButtonClicked(Button* button);
    void toggleButtonClicked(Button* button);
    void mouseMove(const MouseEvent& event);
    void mouseDrag(const MouseEvent& event);
    void mouseDown(const MouseEvent& event);
    void mouseUp(const MouseEvent& event);
    void buttonStateChanged(Button*);
    void showInsertControlsMenu(int x, int y);
    void insertScoreStatementText(Table *table, bool overwrite);
    void savePresetsFromParameters(File selectedFile, String mode);
    void updatefTableData(GenTable* table);
    bool checkForIdentifierMessage(CabbageGUIType cAttr, String type)
    {
        if(cAttr.getStringProp(CabbageIDs::type).equalsIgnoreCase(type) &&
                cAttr.getStringProp(CabbageIDs::identchannelmessage).isNotEmpty())
            return true;

        else
            return false;
    }
    void timerCallback();
    int csoundOutputWidget;
    int mouseX, mouseY;
    bool LOCKED;
    void insertComponentsFromCabbageText(StringArray text, bool useOffset);
    Array<int> lineNumbers;
    Array<int> plantLineNumbers;
    int currentLineNumber;
    File SnapShotFile;
    String presetFileText;
    String tempPlantText;
    void actionListenerCallback (const String& message);
    int zero_dbfs;
    StringArray tempArray;
    StringArray pastEvents;
    Array <float, CriticalSection> tableValues;
    AudioSampleBuffer tableBuffer;
    String lastOpenedDirectory;
    ScopedPointer<Viewport> viewport;
    ScopedPointer<CabbageViewportComponent> viewportComponent;

    bool keyStateChanged(bool onoff)
    {
        keyIsPressed = onoff;
        return false;
    }
    bool keyIsPressed;
    bool isMouseDown;
    bool showScrollbars;
    void positionComponentWithinPlant(String type, float left, float top, float width, float height, Component *layout, Component *control);
    //ScopedPointer<CabbagePlantWindow> subPatch;
    OwnedArray<CabbageButton> plantButton;
    ScopedPointer<InfoWindow> infoWindow;
#ifdef Cabbage_GUI_Editor
    ScopedPointer<CabbageMainPanel> componentPanel;
    ScopedPointer<ComponentLayoutEditor> layoutEditor;
#else
    ScopedPointer<Component> componentPanel;
#endif
    ScopedPointer<CabbageKeyboard> midiKeyboard;
    ScopedPointer<LookAndFeel_V1> feely;
    ComponentBoundsConstrainer resizeLimits;

    AudioPlayHead::CurrentPositionInfo hostInfo;
    void changeListenerCallback (ChangeBroadcaster *source);
    Colour formColour, fontColour;
    String authorText;
    String formPic;
    float inValue;
    int xyPadIndex;
    ScopedPointer<CabbageLookAndFeel> lookAndFeel;
    ScopedPointer<CabbageLookAndFeelBasic> basicLookAndFeel;
    ScopedPointer<Label> debugLabel;
    StringArray scoreEvents;
    String globalSVGPath;
    String consoleMessages;
    Array<int> radioGroups;
    ScopedPointer<BubbleMessageComponent> popupBubble;

    TooltipWindow tooltipWindow;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbagePluginAudioProcessorEditor);

};


//==============================================================================
class CabbageCornerResizer : public ResizableCornerComponent
{
    CabbagePluginAudioProcessorEditor* editor;
public:
    CabbageCornerResizer(CabbagePluginAudioProcessorEditor* parent, Component* comp, ComponentBoundsConstrainer *constrainer):
        ResizableCornerComponent(comp, constrainer), editor(parent) {};
    ~CabbageCornerResizer() {};

    void mouseUp(const MouseEvent &e)
    {
        editor->updateSize();
    }


};

#endif  // __PLUGINEDITOR_H_F4EBBBA1__

