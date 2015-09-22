/*
  Copyright (C) 2014 Rory Walsh

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
#include "../JuceLibraryCode/JuceHeader.h"
#include "Preferences.h"

//===================== Document Window ========================================

CabbagePreferences::CabbagePreferences()
//: DocumentWindow ("Preferences", Colour(20, 20, 20), DocumentWindow::allButtons)
{
    //setResizable(true, true);
    //centreWithSize(660, 460);
    //setContentOwned(&mainComp, true);
    //setColour(TabbedComponent::outlineColourId, Colours::black);
    addAndMakeVisible(mainComp);
    setSize(660, 460);
}

CabbagePreferences::~CabbagePreferences() {}

void CabbagePreferences::closeButtonPressed()
{
    delete this;
}

void CabbagePreferences::addComponent(String type, Component* comp)
{
    mainComp.addComponent(type, comp);
}

void CabbagePreferences::addPluginList(Component* plugins)
{
    mainComp.addChildComponent(plugins);
    plugins->setVisible(false);
    plugins->setBounds(160, 10, getWidth()-170, 450);
}

//====================== Main Component ===================================
PreferencesComp::PreferencesComp(): cabbageFilesPathList(), preferencesLabel("")
{
    prefsListBox.setModel (&listBoxModel);
    prefsListBox.setMultipleSelectionEnabled (false);
    prefsListBox.setColour(ListBox::ColourIds::backgroundColourId, Colour(30, 30, 30));
    addAndMakeVisible (prefsListBox);
    listBoxModel.addActionListener(this);
    prefsListBox.selectRow(0);
    cabbageFilesPathList.setBounds(170, 30, getWidth()-180, 420);
    addAndMakeVisible(cabbageFilesPathList);

    cabbageFilesPathList.setVisible(false);

    preferencesLabel.setFont(cUtils::getComponentFont().withHeight(18.f));
    preferencesLabel.setColour(Label::textColourId, cUtils::getComponentFontColour());
    preferencesLabel.setBounds(170, 10, getWidth()-180, 20);
    preferencesLabel.setJustificationType(Justification::left);
    addAndMakeVisible(&preferencesLabel);

    preferencesInfo.setFont(cUtils::getComponentFont().withHeight(14.f));
    preferencesInfo.setColour(Label::textColourId, cUtils::getComponentFontColour());
    preferencesInfo.setBounds(170, getHeight()-90, getWidth()-180, 80);
    preferencesInfo.setJustificationType(Justification::centredLeft);
    addAndMakeVisible(&preferencesInfo);
    preferencesInfo.setText("Lower buffer sizes will give better latency, but may cause audio drop outs. You will need to play with these settings until you find the best combination. When working with Cabbage files, try to use buffer sizes that are multiple integers of ksmps. This should reduce the likelihood of audio dropouts.", dontSendNotification);


    cabbageFilesPathList.setColour(FileSearchPathListComponent::backgroundColourId, cUtils::getDarkerBackgroundSkin());
    setSize(660, 460);

}

void PreferencesComp::addComponent(String type, Component* comp)
{
    if( type == "audioSelector")
    {
        audioSelector=comp;
        addAndMakeVisible(audioSelector);
        audioSelector->setBounds(160, 10, getWidth()-170, getHeight()-70);
    }
    else if( type == "pluginList")
    {
        pluginList=comp;
        addAndMakeVisible(pluginList);
        pluginList->setVisible(false);
        pluginList->setBounds(170, 30, getWidth()-180, getHeight()-120);
    }
}

void PreferencesComp::resized()
{
    prefsListBox.setBounds(10, 10, 150, getHeight()-20);
    cabbageFilesPathList.setBounds(170, 30, getWidth()-180, getHeight()-120);
    preferencesLabel.setBounds(170, 10, getWidth()-180, 20);
    preferencesInfo.setBounds(170, getHeight()-90, getWidth()-180, 80);

    if (pluginList)
        pluginList->setBounds(170, 30, getWidth()-180, getHeight()-120);
    if (audioSelector)
        audioSelector->setBounds(160, 10, getWidth()-170, getHeight()-70);

}

void PreferencesComp::actionListenerCallback(const String& message)
{
    int index = message.getIntValue()+1;

    pluginList->setVisible(false);
    audioSelector->setVisible(false);
    cabbageFilesPathList.setVisible(false);
    preferencesLabel.setVisible(false);
    preferencesInfo.setVisible(true);


    if(index==1) //audioSelector
    {
        audioSelector->setVisible(true);
        preferencesInfo.setText("Lower buffer sizes will give better latency, but may cause audio drop outs. You will need to play with these settings until you find the best combination. When working with Cabbage files, try to use buffer sizes that are multiple integers of ksmps. This should reduce the likelihood of audio dropouts.", dontSendNotification);

    }
    else if(index==2)
    {
        pluginList->setVisible(true);
        preferencesLabel.setText("List of third party plugins", dontSendNotification);
        preferencesLabel.setVisible(true);
        preferencesInfo.setVisible(false);
    }
    else if(index==3)
    {
        preferencesLabel.setText("Folders containing Cabbage .csd files", dontSendNotification);
        preferencesLabel.setVisible(true);
        cabbageFilesPathList.setVisible(true);
        preferencesInfo.setText("The above directories should contain Cabbage .csd files(with -n -d passed to the CsOptions and an appropriate <Cabbage> section). Regular Csound files will not work correctly. Cabbage will dynamically search these directories each time you right-click in the patcher window. To speed up the search and loading of files into the plugin menu Cabbage only searches to a depth of 1 sub-folder. ", dontSendNotification);
    }


}

//====================== ListBox Component ===================================
// when a user clicks on an item in the listbox, the CabbagePreferencesComp will update
// what the users see
PreferencesComp::ListboxContents::ListboxContents()
{
    contents.add("Audio Setup");
    contents.add("Third Party Plugins");
    contents.add("Cabbage Plugins");
    //contents.add("Editor");
    //contents.add("General");
}

int PreferencesComp::ListboxContents::getNumRows()
{
    return contents.size();
}

void PreferencesComp::ListboxContents::paintListBoxItem (int rowNumber, Graphics& g,
        int width, int height, bool rowIsSelected)
{
    if (rowIsSelected)
        g.fillAll (Colours::cornflowerblue);

    g.setColour (rowIsSelected ? Colours::black : Colours::green);
    g.setFont (cUtils::getComponentFont());

    g.drawText (contents[rowNumber],
                5, 0, width, height,
                Justification::centredLeft, true);
}

void PreferencesComp::ListboxContents::listBoxItemClicked(int row, const MouseEvent &)
{
    sendActionMessage(String(row));
}
