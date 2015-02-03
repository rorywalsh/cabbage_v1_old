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
class CsoundPathList : public FileSearchPathListComponent, public ActionBroadcaster
{
	public:
		CsoundPathList()
		{
				paths.add("/usr/local");
				paths.add("/usr/lib");
				numRows=1;
				
		};
		
        int getNumRows()
		{
			return numRows;
		}
		
        void paintListBoxItem (int rowNumber, Graphics& g,
                               int width, int height, bool rowIsSelected)	
								{
									if (rowIsSelected)
										g.fillAll (Colours::cornflowerblue);
									else
										g.fillAll(Colour(50, 50, 50));

									g.setColour (rowIsSelected ? Colours::black : Colours::green);
									g.setFont (cUtils::getComponentFont());

									g.drawText (getPath()[rowNumber].getFullPathName(),
									5, 0, width, height,
									Justification::centredLeft, true);
									
									numRows = getPath().getNumPaths();
									
								}
	
		void listBoxItemClicked(int row, const MouseEvent &)
		{
			sendActionMessage(String(row));
		}	


		
	private:
			StringArray paths;
			int numRows;	
};

//================================================================
class PreferencesComp   : public Component, public ActionListener
{
	
public:
	PreferencesComp();	
	~PreferencesComp()
    {

    }
	
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
	
    void addComponent(String type, Component* comp);
    
private:
	ListBox prefsListBox;
    ListboxContents listBoxModel;
    ScopedPointer<Component> audioSelector;
    ScopedPointer<Component> pluginList;
    CsoundPathList csoundPathList;
    Label preferencesLabel;
};

//================================================================
class CabbagePreferences   : public DocumentWindow
{
	
	PreferencesComp mainComp;
	
public:
	CabbagePreferences();	
	~CabbagePreferences();
	
	void addPluginList(Component* selector);
	void closeButtonPressed();
    void addComponent(String type, Component* comp);


private:
   
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbagePreferences);

};

#endif