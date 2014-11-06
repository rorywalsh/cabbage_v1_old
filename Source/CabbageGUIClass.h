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

//   You should have received a copy of the GNU Lesser General Public
  License along with Csound; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
  02111-1307 USA

*/
#ifndef CABBPARSE_H
#define CABBPARSE_H

#include <stdio.h>
#include <string>
#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <algorithm>
#include <fstream>
#include <sstream>

#include "CabbageUtils.h"

//creating this as a singleton as I don't wish to create
//this array over and oer again when dealing with the CabbageGUIClass
//objects.
class IdentArray : public StringArray
{
public:
    IdentArray() : StringArray("tablecolour")
    {

        add("tablecolours");
        add("bounds");
        add("fontcolour");
        add("size");
        add("items");
        add("pos");
        add("min");
        add("max");
        add("value");
        add("tabpage");
        add("guirefresh");
        add("midictrl");
        add("stack");
        add("line");
        add("populate");
        add("range");
        add("amprange");
        add("rangex");
        add("rangey");
        add("plant");
        add("channeltype");
        add("channels");
        add("channel");
		add("channelarray");	
		add("identchannelarray");		
        add("name");
        add("textbox");
        add("caption");
        add("kind");
        add("config");
        add("align");
        add("beveltype");
        add("wrap");
        //add(" mode(");
        //add(",mode(");
        add("mode");
        add("tablenumber");
        add("tablenum");
        add("tablenumbers");
        add("tablenums");
        add("fill");
        add("logger");
        add("file");
        add("outlinecolour");
        add("master");
        add("shape");
        add("textcolour");
        add("scale");
        add("key(");
        add("pluginid");
        add("trackercolour");
        add("preset");
        add("popup");
        add("include");
        add("show");
        add("author");
        add("drawmode");
        add("resizemode");
        add("readonly");
        add("latched");
        add("identchannel");
        add("visible");
        add("scrubberposition");
		add("scroll");
        add("zoom");
		add("samplerange");
		add("scrollbars");
		add("socket");
		//colour and text must go at end to prevent fall through
        add("colour");
		add("text");
    }
    ~IdentArray()
    {
// this ensures that no dangling pointers are left when the
// singleton is deleted.
        clearSingletonInstance();
    }
    juce_DeclareSingleton (IdentArray, false)
};

namespace CabbageIDs
{
//identifiers
static const Identifier top = "top";
static const Identifier left = "left";
static const Identifier width = "width";
static const Identifier height = "height";
static const Identifier min = "min";
static const Identifier max = "max";
static const Identifier value = "value";
static const Identifier channel = "channel";
static const Identifier channelarray = "channelarray";
static const Identifier identchannelarray = "identchannelarray";
static const Identifier outlinecolour = "outlinecolour";
static const Identifier fillcolour = "fillcolour";
static const Identifier textcolour = "textcolour";
static const Identifier trackercolour = "trackercolour";
static const Identifier tablecolour = "tablecolour";
static const Identifier fontcolour= "fontcolour";
static const Identifier colour = "colour";
static const Identifier items = "items";
static const Identifier text = "text";
static const Identifier range = "range";
static const Identifier sliderrange = "sliderrange";
static const Identifier amprange = "amprange";
static const Identifier caption = "caption";
static const Identifier basetype = "basetype";
static const Identifier textbox = "textbox";
static const Identifier name = "name";
static const Identifier type = "type";
static const Identifier sliderskew = "sliderskew";
static const Identifier sliderincr = "sliderincr";
static const Identifier midichan = "midichan";
static const Identifier midictrl = "midictrl";
static const Identifier kind = "kind";
static const Identifier decimalplaces = "decimalplaces";
static const Identifier mode = "mode";
static const Identifier shape = "shape";
static const Identifier channeltype = "channeltype";
static const Identifier comborange = "comborange";
static const Identifier populate = "populate";
static const Identifier popup = "popup";
static const Identifier plant = "plant";
static const Identifier line = "line";
static const Identifier tablenumber = "tablenumber";
static const Identifier tableconfig = "tableconfig";
static const Identifier resizemode = "resizemode";
static const Identifier drawmode = "drawmode";
static const Identifier readonly = "readonly";
static const Identifier xyautoindex = "xyautoindex";
static const Identifier file = "file";
static const Identifier latched = "latched";
static const Identifier xchannel = "xchannel";
static const Identifier ychannel = "ychannel";
static const Identifier minx = "minx";
static const Identifier miny = "miny";
static const Identifier maxx = "maxx";
static const Identifier maxy = "maxy";
static const Identifier logger = "logger";
static const Identifier stack = "stack";
static const Identifier valuex = "valuex";
static const Identifier valuey = "valuey";
static const Identifier pluginid = "pluginid";
static const Identifier tabs = "tabs";
static const Identifier wrap = "wrap";
static const Identifier align = "align";
static const Identifier tabbed = "tabbed";
static const Identifier rangey = "rangey";
static const Identifier rangex = "rangex";
static const Identifier include = "include";
static const Identifier tabpage = "tabpage";
static const Identifier filetype = "filetype";
static const Identifier workingdir = "workingdir";
static const Identifier author = "author";
static const Identifier xychannel = "xychannel";
static const Identifier guirefresh = "guirefresh";
static const Identifier identchannel = "identchannel";
static const Identifier identchannelmessage = "identchannelmessage";
static const Identifier visible = "visible";
static const Identifier lineNumber = "linenumber";
static const Identifier scrubberposition = "scrubberposition";
static const Identifier startpoint = "startpoint";
static const Identifier endpoint = "endpoint";
static const Identifier zoom = "zoom";
static const Identifier visiblelength = "visiblelength";
static const Identifier samplerange = "samplerange";
static const Identifier startpos = "startpos";
static const Identifier endpos = "endpos";
static const Identifier show = "show";
static const Identifier child = "child";
static const Identifier scrollbars = "scrollbars";
static const Identifier oscport = "oscport";
static const Identifier oscaddress = "oscaddress";

//type of widgets/controls/messages
static const String combobox = "combobox";
static const String numberbox = "numberbox";
static const String rslider = "rslider";
static const String hslider = "hslider";
static const String vslider = "vslider";
static const String checkbox = "checkbox";
static const String soundfiler = "sounfiler";
static const String button = "button";
static const String infobutton = "infobutton";
static const String filebutton = "filebutton";
static const String texteditor = "texteditor";
static const String table = "table";
static const String gentable = "gentable";
static const String groupbox = "groupbox";
static const String image = "image";
static const String popupmenu = "popupmenu";
static const String label = "label";
static const String keyboard = "keyboard";
static const String oscserver = "oscserver";
static const String form = "form";
static const String directorylist = "directorylist";
static const String index = "index";
static const String xypad = "xypad";
static const String stringchannel = "string";
static const String hostbpm = "HOST_BPM";
static const String timeinseconds = "TIME_IN_SECONDS";
static const String isplaying = "IS_PLAYING";
static const String isrecording = "IS_RECORDING";
static const String hostppqpos = "HOST_PPQ_POS";
static const String mousex = "MOUSE_X";
static const String mousey = "MOUSE_Y";
static const String mousedownleft = "MOUSE_DOWN_LEFT";
static const String mousedownright = "MOUSE_DOWN_RIGHT";
static const String mousedownlmiddle = "MOUSE_DOWN_MIDDLE";


static const String csoundoutput = "csoundoutput";

};




class CabbageGUIClass : public CabbageUtils
{
    double width, height, top, left;
	/*, isRect, min, max, minX, minY, maxX, tabbed, maxY, comboRange, fftSize, overlapSize, frameSize,
           noOfMenus, onoff, midiChan, midiCtrl, sliderRange, xypadRangeY, xypadRangeX, noSteps, noPatterns, pvsChannel, alpha,
           line, anchor, linkTo, scaleX, scaleY, value, valueX, valueY, maxItems, sliderIncr, sliderSkew, decimalPlaces, rCtrls, lineIsVertical;
    StringArray items, onoffcaptions, key, channels, snapshotData, colours;*/

    Array<int> vuConfig;
    Array<int> tableNumbers;
    Array<float> tableChannelValues;


    //JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageGUIClass);
public:
    //constructor
    NamedValueSet cabbageIdentifiers;
    CabbageGUIClass(String str, int ID);
    CabbageGUIClass() {};
    ~CabbageGUIClass();
    int parse(String str, String identifier);
    float getNumProp(Identifier prop);
    float getNumProp(Identifier prop, int index);
    void setNumProp(Identifier prop, float val);
    void setTableChannelValues(int index, float val);
    float getTableChannelValues(int index);
    void addTableChannelValues();
    void setStringProp(Identifier prop, String val);
    void setStringProp(Identifier prop, int index, String value);
    String getStringProp(Identifier prop);
    String getStringProp(Identifier prop, int index);
    String getPropsString();
    String getColourProp(Identifier prop);
    float getNumPropVal(Identifier prop);
    void setNumPropVal(Identifier prop, float val);
    static String getCabbageCodeFromIdentifiers(NamedValueSet props);
    static String getStringForIdentifier(var props, String identifier, String type);

    Rectangle<int> getBounds()
    {
        Rectangle<int> bounds(left, top, width, height);
        return bounds;
    }

    //static methods used for updating look and pos of GUI controls
    static Rectangle<int> getBoundsFromText(String text);
    static Colour getColourFromText(String text);
    static String getTextFromText(String text);
    static Point<int> getSizeFromText(String text);
    static Point<int> getPosFromText(String text);
    static float getSkewFromText(String text);
    static var getVarArrayFromText(String text);




    void setBounds(Rectangle<int> bounds)
    {
        left = bounds.getX();
        top = bounds.getY();
        width = bounds.getWidth();
        height = bounds.getHeight();
    }

    static StringArray getIdentifiers()
    {
        StringArray test;
        return test;
    }

    Rectangle<int> getComponentBounds();


    StringArray getStringArrayProp(Identifier prop);
    String getStringArrayPropValue(Identifier prop, int index);

    int getIntArrayPropValue(Identifier prop, int index);
    Array<int> getIntArrayProp(Identifier prop);
	
	var getVarArrayProp(Identifier prop);

    float getFloatArrayPropValue(Identifier prop, int index);
    Array<float> getFloatArrayProp(Identifier prop);

    void setStringArrayPropValue(Identifier prop, int index, String value);
    void setStringArrayProp(Identifier prop, var value);


};

#endif

