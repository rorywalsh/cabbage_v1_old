/*
  Copyright (C) 2009 Rory Walsh,
   This file is based on dRowAudio's FFT demo...

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


#ifndef __CABBAGE_SPEC_UTILS_H__
#define __CABBAGE_SPEC_UTILS_H__

#include "../../JuceLibraryCode/JuceHeader.h"

//==============================================================================
class CabbageSpectral : public Component,
    public Button::Listener,
    public Slider::Listener
{
public:
    //==============================================================================
    CabbageSpectral(): renderThread ("FFT Render Thread"),
        spectroscope (8),
        sonogram (8)
    {

        //addAndMakeVisible (&audioOscilloscope);
        //addAndMakeVisible (&spectroscope);
        addAndMakeVisible (&sonogram);

        renderThread.addTimeSliceClient (&spectroscope);
        renderThread.addTimeSliceClient (&sonogram);
        renderThread.startThread (3);

        addAndMakeVisible (&logSpectroscopeButton);
        addAndMakeVisible (&logSonogramButton);

        logSpectroscopeButton.setButtonText ("Log Frequency Scale");
        logSonogramButton.setButtonText ("Log Frequency Scale");

        logSpectroscopeButton.setClickingTogglesState (true);
        logSonogramButton.setClickingTogglesState (true);

        logSpectroscopeButton.addListener (this);
        logSonogramButton.addListener (this);

        addAndMakeVisible (&sonogramSpeedSlider);
        sonogramSpeedSlider.setRange (1.0, 10.0, 1.0);
        sonogramSpeedSlider.setValue (sonogram.getBlockWidth());
        sonogramSpeedSlider.setTextBoxStyle (Slider::NoTextBox, true, 0, 0);
        sonogramSpeedSlider.addListener (this);
    }

    ~CabbageSpectral()
    {
        renderThread.stopThread (500);
        renderThread.removeTimeSliceClient (&spectroscope);
        renderThread.removeTimeSliceClient (&sonogram);


        logSpectroscopeButton.removeListener (this);
        logSonogramButton.removeListener (this);
        sonogramSpeedSlider.removeListener (this);
    }

    void paint (Graphics& g) {}

    void resized()
    {
        const int h = getHeight();
        const int w = getWidth();
        const int m = 5;
        const int ch = (h - (5 * m)) / 4;

        //audioOscilloscope.setBounds (m, m, w - (2 * m), ch);
        //spectroscope.setBounds (m, ch + (2 * m), w - (2 * m), ch);
        //sonogram.setBounds (m, (2 * ch) + (3 * m), w - (2 * m), (2 * ch) + m);
        sonogram.setBounds(getLocalBounds());
        //logSpectroscopeButton.setBounds (spectroscope.getX(), spectroscope.getY(), 150, 18);
        logSonogramButton.setBounds     (sonogram.getX(), sonogram.getY(), 150, 18);

        sonogramSpeedSlider.setBounds   (logSonogramButton.getRight() + m, logSonogramButton.getY(),
                                         100, 18);
    }

    void buttonClicked (Button* button)
    {
        if (button == &logSpectroscopeButton)
        {
            spectroscope.setLogFrequencyDisplay (logSpectroscopeButton.getToggleState());
        }
        else if (button == &logSonogramButton)
        {
            sonogram.setLogFrequencyDisplay (logSonogramButton.getToggleState());
        }
    }

    void sliderValueChanged (Slider* slider)
    {
        if (slider == &sonogramSpeedSlider)
        {
            sonogram.setBlockWidth ((int) sonogramSpeedSlider.getValue());
        }
    }

    //==============================================================================

    void processBlock (const float* inputChannelData, int numSamples)
    {
        if (inputChannelData != nullptr)
        {
            audioOscilloscope.processBlock (inputChannelData, numSamples);
            spectroscope.copySamples (inputChannelData, numSamples);
            sonogram.copySamples (inputChannelData, numSamples);
        }
    }

private:
    //==============================================================================
    TimeSliceThread renderThread;
    drow::AudioOscilloscope audioOscilloscope;
    drow::Spectroscope spectroscope;
    drow::Sonogram sonogram;

    //==============================================================================
    ToggleButton logSpectroscopeButton, logSonogramButton;
    Slider sonogramSpeedSlider;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageSpectral);
};

#endif //__DROWAUDIODEMO_FFTDEMO_H__