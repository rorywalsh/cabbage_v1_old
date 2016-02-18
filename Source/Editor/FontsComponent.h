#ifndef __FontsComponent_H_615ECE56__
#define __FontsComponent_H_615ECE56__

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


//==============================================================================
class FontsComponent  : public Component,
    public ActionBroadcaster,
    private ListBoxModel,
    private Slider::Listener,
    private Button::Listener,
    private ComboBox::Listener
{
public:
    FontsComponent()
        : heightLabel (String::empty, "Height:"),
          kerningLabel (String::empty, "Kerning:"),
          scaleLabel  (String::empty, "Scale:"),
          styleLabel ("Style"),
          boldToggle ("Bold"),
          italicToggle ("Italic")
    {
        setOpaque (true);

        addAndMakeVisible (listBox);
        addAndMakeVisible (demoTextBox);

        Font::findFonts(fonts);   // Generate the list of fonts

        //run through fonts and remove anything that's note mono spaced..
        for(int i=fonts.size(); i>=0; i--)
        {
            Font font (fonts [i]);
            if(font.getTypefaceName().containsIgnoreCase("mono")==false)
            {
                fonts.remove(i);
            }
        }

        for(int i=0; i<fonts.size(); i++)
        {
            Font font (fonts [i]);
            cUtils::debug(font.getTypefaceName());
        }

        listBox.setRowHeight (20);
        listBox.setModel (this);   // Tell the listbox where to get its data model
        listBox.selectRow(-1);

        // ..and pick a random font to select intially
        demoTextBox.setColour(TextEditor::backgroundColourId, Colours::white);
        demoTextBox.setColour(TextEditor::textColourId, Colours::black);
        demoTextBox.setMultiLine (true);
        demoTextBox.setReturnKeyStartsNewLine (true);
        demoTextBox.setText ("Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor incididunt "
                             "ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco "
                             "laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor ");

        demoTextBox.setCaretPosition (0);
    }

    //==============================================================================
    void paint (Graphics& g) override
    {
        g.fillAll(Colours::white);
    }

    void resized() override
    {
        listBox.setBounds(0, 0, getWidth(), getHeight());
        //demoTextBox.setBounds(0, getHeight()*.3, getWidth(), getHeight()*.7);
    }

    void sliderValueChanged (Slider* sliderThatWasMoved) override
    {

    }

    void buttonClicked (Button* buttonThatWasClicked) override
    {
        if (buttonThatWasClicked == &boldToggle)            refreshPreviewBoxFont();
        else if (buttonThatWasClicked == &italicToggle)     refreshPreviewBoxFont();
    }

    // The following methods implement the ListBoxModel virtual methods:
    int getNumRows() override
    {
        return fonts.size();
    }

    void listBoxItemDoubleClicked(int row, const MouseEvent &e)
    {
        sendActionMessage(currentFont);
    }


    void paintListBoxItem (int rowNumber, Graphics& g,
                           int width, int height, bool rowIsSelected) override
    {
        if (rowIsSelected)
            g.fillAll (Colours::cornflowerblue);
        else
            g.fillAll(Colours::white);

        Font font (fonts [rowNumber]);

        AttributedString s;
        s.setWordWrap (AttributedString::none);
        s.setJustification (Justification::centredLeft);
        s.append (font.getTypefaceName(), font.withPointHeight (height * 0.7f), Colours::black);
        s.append ("   " + font.getTypefaceName(), Font (height * 0.5f, Font::italic), Colours::grey);
        currentFont = font.getTypefaceName();
        s.draw (g, Rectangle<int> (width, height).expanded (-4, 50).toFloat());
    }

    void selectedRowsChanged (int /*lastRowselected*/) override
    {
        refreshPreviewBoxFont();
    }

private:
    Array<Font> fonts;
    StringArray currentStyleList;
    String currentFont;

    ListBox listBox;
    TextEditor demoTextBox;
    Label heightLabel, kerningLabel, scaleLabel, styleLabel;
    Slider heightSlider, kerningSlider, scaleSlider;
    ToggleButton boldToggle, italicToggle;
    ComboBox styleBox;

    StretchableLayoutManager verticalLayout;
    ScopedPointer<StretchableLayoutResizerBar> verticalDividerBar;

    void refreshPreviewBoxFont()
    {
        const bool bold = boldToggle.getToggleState();
        const bool italic = italicToggle.getToggleState();
        const bool useStyle = ! (bold || italic);

        Font font (fonts [listBox.getSelectedRow()]);

        font = font.withPointHeight ((float) heightSlider.getValue())
               .withExtraKerningFactor ((float) kerningSlider.getValue())
               .withHorizontalScale ((float) scaleSlider.getValue());

        if (bold)    font = font.boldened();
        if (italic)  font = font.italicised();

        updateStylesList (font);

        styleBox.setEnabled (useStyle);

        if (useStyle)
            font = font.withTypefaceStyle (styleBox.getText());

        demoTextBox.applyFontToAllText (font);
    }

    void updateStylesList (const Font& newFont)
    {
        const StringArray newStyles (newFont.getAvailableStyles());

        if (newStyles != currentStyleList)
        {
            currentStyleList = newStyles;

            styleBox.clear();
            styleBox.addItemList (newStyles, 1);
            styleBox.setSelectedItemIndex (0);
        }
    }

    void comboBoxChanged (ComboBox* box) override
    {
        if (box == &styleBox)
            refreshPreviewBoxFont();
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FontsComponent)
};

#endif