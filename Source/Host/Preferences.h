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
#ifndef HOSTPREF_H
#define HOSTPREF_H

#include "../JuceLibraryCode/JuceHeader.h"
#include "../CabbageUtils.h"
#include "../CabbageAudioDeviceSelectorComponent.h"

ApplicationProperties& getAppProperties();


//================================================================
class PreferencesComp   : public Component, public ActionListener
{
	
public:
	PreferencesComp();	
	~PreferencesComp(){}
	
	void resized();
	void actionListenerCallback(const String& message);

    class ListboxContents  : public ListBoxModel, public ActionBroadcaster
    {
        // The following methods implement the necessary virtual functions from ListBoxModel,
        // telling the listbox how many rows there are, painting them, etc.
		public:
		ListboxContents();		
        int getNumRows() override;
        void paintListBoxItem (int rowNumber, Graphics& g,
                               int width, int height, bool rowIsSelected) override;		
		void listBoxItemClicked(int row, const MouseEvent &);		
		
		private:
			StringArray contents;
    };
	
private:
	ListBox prefsListBox;
    ListboxContents listBoxModel;
};

//================================================================
class CabbagePreferences   : public DocumentWindow
{
	
	PreferencesComp mainComp;
	
public:
	CabbagePreferences();	
	~CabbagePreferences();
	
	void addAudioSelector(Component* selector);
	void closeButtonPressed();


private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbagePreferences);

};

#endif