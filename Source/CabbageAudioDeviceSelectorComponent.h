/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2013 - Raw Material Software Ltd.

   Permission is granted to use this software under the terms of either:
   a) the GPL v2 (or any later version)
   b) the Affero GPL v3

   Details of these licenses can be found at: www.gnu.org/licenses

   JUCE is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

   ------------------------------------------------------------------------------

   To release a closed-source product which uses JUCE, commercial licenses are
   available: visit www.juce.com for more information.

  ==============================================================================
   *
   * hack of JUCE AudioDeviceSelectorComponent
   *
*/

#ifndef CABBAGE_AUDIODEVICESELECTORCOMPONENT_H_INCLUDED
#define CABBAGE_AUDIODEVICESELECTORCOMPONENT_H_INCLUDED
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

#include "../JuceLibraryCode/JuceHeader.h"
#include "CabbageLookAndFeel.h"
#include "CabbageUtils.h"

extern ApplicationProperties* appProperties;

//==============================================================================
class CabbageAudioDeviceSelectorComponent  : public Component,
    private ComboBoxListener,
    private ChangeListener
{
public:
    //==============================================================================
    CabbageAudioDeviceSelectorComponent (AudioDeviceManager& deviceManager,
                                         int minAudioInputChannels,
                                         int maxAudioInputChannels,
                                         int minAudioOutputChannels,
                                         int maxAudioOutputChannels,
                                         bool showMidiInputOptions,
                                         bool showMidiOutputSelector,
                                         bool showChannelsAsStereoPairs,
                                         bool hideAdvancedOptionsWithButton);

    /** Destructor */
    ~CabbageAudioDeviceSelectorComponent();

    /** The device manager that this component is controlling */
    AudioDeviceManager& deviceManager;

    //==============================================================================
    /** @internal */
    void resized() override;
    /** @internal */
    void childBoundsChanged (Component*) override;

private:
    //==============================================================================
    ScopedPointer<ComboBox> deviceTypeDropDown;
    ScopedPointer<Label> deviceTypeDropDownLabel;
    ScopedPointer<Component> audioDeviceSettingsComp;

    String audioDeviceSettingsCompType;
    const int minOutputChannels, maxOutputChannels, minInputChannels, maxInputChannels;
    const bool showChannelsAsStereoPairs;
    const bool hideAdvancedOptionsWithButton;

    class MidiInputSelectorComponentListBox;
    friend struct ContainerDeletePolicy<MidiInputSelectorComponentListBox>;
    ScopedPointer<MidiInputSelectorComponentListBox> midiInputsList;
    ScopedPointer<ComboBox> midiOutputSelector;
    ScopedPointer<Label> midiInputsLabel, midiOutputLabel;

    void comboBoxChanged (ComboBox*) override;
    void changeListenerCallback (ChangeBroadcaster*) override;
    void updateAllControls();

    ScopedPointer<CabbageLookAndFeel> lookAndFeel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageAudioDeviceSelectorComponent)
};


#endif   // CABBAGE_AUDIODEVICESELECTORCOMPONENT_H_INCLUDED
