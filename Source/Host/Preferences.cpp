/*
  Copyright (C) 2007 Rory Walsh

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

CabbagePreferences::CabbagePreferences() : DocumentWindow ("Preferences", Colours::black,
						  DocumentWindow::allButtons)
{
		setResizable(false, false);
		centreWithSize(660, 500);
		setContentOwned(&mainComp, true);
		setColour(TabbedComponent::outlineColourId, Colours::black);
}
	
CabbagePreferences::~CabbagePreferences(){}

void CabbagePreferences::closeButtonPressed()
{
		delete this;
}

void CabbagePreferences::addAudioSelector(Component* selector)
{
	mainComp.addAndMakeVisible(selector);
	selector->setBounds(160, 10, 500, 450);
}	

//====================== Main Component ===================================
PreferencesComp::PreferencesComp() 
{
    prefsListBox.setModel (&listBoxModel);
    prefsListBox.setMultipleSelectionEnabled (false);

    addAndMakeVisible (prefsListBox);
	listBoxModel.addActionListener(this);
	prefsListBox.selectRow(0);
	setSize(650, 500);

}

void PreferencesComp::resized()
{
	prefsListBox.setBounds(10, 10, 160, getHeight()-20);
}

void PreferencesComp::actionListenerCallback(const String& message)
{
	this->getChildComponent(1)->setVisible(false);
}

//====================== ListBox Component ===================================
// when a user clicks on an item in the listbox, the CabbagePreferencesComp will update
// what the users see
PreferencesComp::ListboxContents::ListboxContents()
{
	contents.add("Audio Setup");
	contents.add("Plugins");
	contents.add("Csound");
	contents.add("Editor");
	contents.add("General");			
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
