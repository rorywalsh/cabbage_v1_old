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


#include "CabbageGUIClass.h"
//===============================================================================
// Main Cabbage abstract GUI class
//===============================================================================
CabbageGUIClass::CabbageGUIClass(String compStr, int ID):
    width(0),
    height(0),
    left(0),
    top(0)
{
//Default values are assigned to all attributres
//before parsing begins
    cabbageIdentifiers.set("scalex", 1);
    cabbageIdentifiers.set("scaley", 1);
	cabbageIdentifiers.set(CabbageIDs::alpha, 1.f);

    StringArray strTokens;
    strTokens.addTokens(compStr, " ", "\"");

    cabbageIdentifiers.set(CabbageIDs::channelarray, "");
    //===============sliders==================//
    if(strTokens[0].trim() == "hslider" || strTokens[0].trim() == "hslider2" || strTokens[0].trim() == "hslider3")
    {
        cabbageIdentifiers.set(CabbageIDs::basetype, "interactive");
        cabbageIdentifiers.set(CabbageIDs::top, 10);
        cabbageIdentifiers.set(CabbageIDs::left, 10);
        cabbageIdentifiers.set(CabbageIDs::width, 150);
        cabbageIdentifiers.set(CabbageIDs::height, 50);
        var channels;
        channels.append("hslider");
        cabbageIdentifiers.set(CabbageIDs::channel, channels);
        cabbageIdentifiers.set(CabbageIDs::min, 0);
        cabbageIdentifiers.set(CabbageIDs::max, 100);
        cabbageIdentifiers.set(CabbageIDs::value, 50);
        cabbageIdentifiers.set(CabbageIDs::text, "");
        cabbageIdentifiers.set(CabbageIDs::textbox, 0.f);
        cabbageIdentifiers.set(CabbageIDs::caption, "");
        cabbageIdentifiers.set(CabbageIDs::colour, Colours::whitesmoke.toString());
        cabbageIdentifiers.set(CabbageIDs::trackercolour, Colours::lime.toString());
        cabbageIdentifiers.set(CabbageIDs::fontcolour, CabbageUtils::getComponentFontColour().toString());
		cabbageIdentifiers.set(CabbageIDs::textcolour, CabbageUtils::getComponentFontColour().toString());
        cabbageIdentifiers.set(CabbageIDs::sliderskew, 1);
        cabbageIdentifiers.set(CabbageIDs::sliderincr, .01);
        cabbageIdentifiers.set(CabbageIDs::midichan, -99);
        cabbageIdentifiers.set(CabbageIDs::midictrl, -99);
        //these don't appear in the props dialog
        cabbageIdentifiers.set(CabbageIDs::name, "hslider");
        cabbageIdentifiers.set(CabbageIDs::type, cabbageIdentifiers.getWithDefault("name", "").toString());
        cabbageIdentifiers.set(CabbageIDs::name, cabbageIdentifiers.getWithDefault("name", "").toString()+String(ID));
		if(strTokens[0].trim() == "hslider2")
			cabbageIdentifiers.set(CabbageIDs::kind, "horizontal2");
		else if(strTokens[0].trim() == "hslider3")
			cabbageIdentifiers.set(CabbageIDs::kind, "horizontal3");
		else
			cabbageIdentifiers.set(CabbageIDs::kind, "horizontal");
        cabbageIdentifiers.set(CabbageIDs::decimalplaces, 1);
        cabbageIdentifiers.set(CabbageIDs::identchannel, "");
        cabbageIdentifiers.set(CabbageIDs::visible, 1);


    }
    else if(strTokens[0].trim() == "oscserver")
    {
        cabbageIdentifiers.set(CabbageIDs::basetype, "layout");
        cabbageIdentifiers.set(CabbageIDs::oscaddress, "");
        cabbageIdentifiers.set(CabbageIDs::oscaddress, 0);
    }
    else if(strTokens[0].trim() == "vslider" || strTokens[0].trim() == "vslider2" ||  strTokens[0].trim() == "vslider3")
    {
        cabbageIdentifiers.set(CabbageIDs::basetype, "interactive");
        cabbageIdentifiers.set(CabbageIDs::top, 10);
        cabbageIdentifiers.set(CabbageIDs::left, 10);
        cabbageIdentifiers.set(CabbageIDs::width, 50);
        cabbageIdentifiers.set(CabbageIDs::height, 150);
        var channels;
        channels.append("vslider");
        cabbageIdentifiers.set(CabbageIDs::channel, channels);
        cabbageIdentifiers.set(CabbageIDs::min, 0);
        cabbageIdentifiers.set(CabbageIDs::max, 100);
        cabbageIdentifiers.set(CabbageIDs::value, 50);
        cabbageIdentifiers.set(CabbageIDs::text, "");
        cabbageIdentifiers.set(CabbageIDs::textbox, 0.f);
        cabbageIdentifiers.set(CabbageIDs::caption, "");
        cabbageIdentifiers.set(CabbageIDs::colour, Colours::whitesmoke.toString());
        cabbageIdentifiers.set(CabbageIDs::trackercolour, Colours::lime.toString());
        cabbageIdentifiers.set(CabbageIDs::fontcolour, CabbageUtils::getComponentFontColour().toString());
		cabbageIdentifiers.set(CabbageIDs::textcolour, CabbageUtils::getComponentFontColour().toString());
        cabbageIdentifiers.set(CabbageIDs::sliderskew, 1);
        cabbageIdentifiers.set(CabbageIDs::sliderincr, .01);
        cabbageIdentifiers.set(CabbageIDs::midichan, -99);
        cabbageIdentifiers.set(CabbageIDs::midictrl, -99);
        //these don't appear in the props dialog
        cabbageIdentifiers.set(CabbageIDs::name, "vslider");
        cabbageIdentifiers.set(CabbageIDs::type, cabbageIdentifiers.getWithDefault("name", "").toString());
        cabbageIdentifiers.set(CabbageIDs::name, cabbageIdentifiers.getWithDefault("name", "").toString()+String(ID));
		if(strTokens[0].trim() == "vslider2")
			cabbageIdentifiers.set(CabbageIDs::kind, "vertical2");
		else if(strTokens[0].trim() == "vslider3")
			cabbageIdentifiers.set(CabbageIDs::kind, "vertical3");
		else
			cabbageIdentifiers.set(CabbageIDs::kind, "vertical");
			
        cabbageIdentifiers.set(CabbageIDs::decimalplaces, 1);
        cabbageIdentifiers.set(CabbageIDs::identchannel, "");
        cabbageIdentifiers.set(CabbageIDs::visible, 1);

    }
    else if(strTokens[0].trim() == "rslider")
    {
        cabbageIdentifiers.set(CabbageIDs::basetype, "interactive");
        cabbageIdentifiers.set(CabbageIDs::top, 10);
        cabbageIdentifiers.set(CabbageIDs::left, 10);
        cabbageIdentifiers.set(CabbageIDs::width, 60);
        cabbageIdentifiers.set(CabbageIDs::height, 60);
        var channels;
        channels.append("rslider");
        cabbageIdentifiers.set(CabbageIDs::channel, channels);
        cabbageIdentifiers.set(CabbageIDs::min, 0);
        cabbageIdentifiers.set(CabbageIDs::max, 100);
        cabbageIdentifiers.set(CabbageIDs::value, 50);
        cabbageIdentifiers.set(CabbageIDs::sliderskew, 1);
        cabbageIdentifiers.set(CabbageIDs::sliderincr, .01);
        cabbageIdentifiers.set(CabbageIDs::text, "");
        cabbageIdentifiers.set(CabbageIDs::textbox, 0.f);
        cabbageIdentifiers.set(CabbageIDs::caption, "");
        cabbageIdentifiers.set(CabbageIDs::colour, Colours::whitesmoke.toString());
		cabbageIdentifiers.set(CabbageIDs::outlinecolour, Colours::whitesmoke.toString());
        cabbageIdentifiers.set(CabbageIDs::trackercolour, Colours::lime.toString());
        cabbageIdentifiers.set(CabbageIDs::fontcolour, CabbageUtils::getComponentFontColour().toString());
		cabbageIdentifiers.set(CabbageIDs::textcolour, CabbageUtils::getComponentFontColour().toString());
        cabbageIdentifiers.set(CabbageIDs::midichan, -99);
        cabbageIdentifiers.set(CabbageIDs::midictrl, -99);
        //these don't appear in the props dialog
        cabbageIdentifiers.set(CabbageIDs::name, "rslider");
        cabbageIdentifiers.set(CabbageIDs::type, cabbageIdentifiers.getWithDefault("name", "").toString());
        cabbageIdentifiers.set(CabbageIDs::name, cabbageIdentifiers.getWithDefault("name", "").toString()+String(ID));
        cabbageIdentifiers.set(CabbageIDs::kind, "rotary");
        cabbageIdentifiers.set(CabbageIDs::decimalplaces, 1);
        cabbageIdentifiers.set(CabbageIDs::identchannel, "");
        cabbageIdentifiers.set(CabbageIDs::trackerthickness, .05);
        cabbageIdentifiers.set(CabbageIDs::visible, 1);
    }

    else if(strTokens[0].trim() == "source")
    {
        /*	  cabbageIdentifiers.set("basetype", "layout");
              top = 10;
              left = 10;
              width = 400;
              height = 240;
              channel = "";
              name = "source";
        	  caption = "";
        	  type = name;
              //name.append(String(ID), 1024);
        	   */
    }

    //===============info button==================//
    else if(strTokens[0].trim() == "infobutton")
    {
        cabbageIdentifiers.set("basetype", "layout");
        top = 10;
        left = 10;
        width = 600;
        height = 440;
        //name.append(String(ID), 1024);
        cabbageIdentifiers.set(CabbageIDs::top, 10);
        cabbageIdentifiers.set(CabbageIDs::left, 10);
        cabbageIdentifiers.set(CabbageIDs::width, 80);
        cabbageIdentifiers.set(CabbageIDs::height, 40);
        cabbageIdentifiers.set(CabbageIDs::text, "");
        cabbageIdentifiers.set(CabbageIDs::caption, "");
        cabbageIdentifiers.set(CabbageIDs::colour, Colours::black.toString());
        cabbageIdentifiers.set(CabbageIDs::fontcolour, Colours::white.toString());
        cabbageIdentifiers.set(CabbageIDs::onfontcolour, Colours::white.toString());
        cabbageIdentifiers.set(CabbageIDs::type, "infobutton");
        cabbageIdentifiers.set(CabbageIDs::name, "infobutton");
        cabbageIdentifiers.set(CabbageIDs::name, cabbageIdentifiers.getWithDefault("name", "").toString()+String(ID));
        cabbageIdentifiers.set(CabbageIDs::identchannel, "");
        cabbageIdentifiers.set(CabbageIDs::visible, 1);
    }

    //===============directory list==================//
    else if(strTokens[0].trim() == "directorylist")
    {
        cabbageIdentifiers.set("basetype", "layout");
        /*
                  top = 10;
                  left = 10;
                  width = 600;
                  height = 440;
                  channel = "";
                  name = "directorylist";
        		  caption = "";
        		  type = name;
                  //name.append(String(ID), 1024);

        	 */
    }


    //===============record button==================//
    else if(strTokens[0].trim() == "recordbutton")
    {
        cabbageIdentifiers.set("basetype", "layout");
        top = 10;
        left = 10;
        width = 80;
        height = 40;
        var array;
        array.append("Record");
        array.append("Record");


        cabbageIdentifiers.set(CabbageIDs::top, 10);
        cabbageIdentifiers.set(CabbageIDs::left, 10);
        cabbageIdentifiers.set(CabbageIDs::width, 80);
        cabbageIdentifiers.set(CabbageIDs::height, 40);
        var channels;
        channels.append("recordbutton");
        cabbageIdentifiers.set(CabbageIDs::channel, channels);
        cabbageIdentifiers.set(CabbageIDs::value, 1);
        cabbageIdentifiers.set(CabbageIDs::text, array);
        cabbageIdentifiers.set(CabbageIDs::caption, "");
        cabbageIdentifiers.set(CabbageIDs::colour, Colours::black.toString());
        cabbageIdentifiers.set(CabbageIDs::fontcolour, Colours::white.toString());
        cabbageIdentifiers.set(CabbageIDs::onfontcolour, Colours::white.toString());
        cabbageIdentifiers.set(CabbageIDs::type, "recordbutton");
        cabbageIdentifiers.set(CabbageIDs::name, "recordbutton");
        cabbageIdentifiers.set(CabbageIDs::mode, "file");
        cabbageIdentifiers.set(CabbageIDs::identchannel, "");
        cabbageIdentifiers.set("name", cabbageIdentifiers.getWithDefault("name", "").toString()+String(ID));
        var populate;
        populate.append("");
        populate.append("");
        cabbageIdentifiers.set("populate", populate);
        cabbageIdentifiers.set(CabbageIDs::visible, 1);
    }
    //===============file button==================//
    else if(strTokens[0].trim() == "filebutton")
    {
        cabbageIdentifiers.set("basetype", "layout");
        top = 10;
        left = 10;
        width = 80;
        height = 40;
        var array;
        array.append("Open file");
        array.append("Open file");


        cabbageIdentifiers.set(CabbageIDs::top, 10);
        cabbageIdentifiers.set(CabbageIDs::left, 10);
        cabbageIdentifiers.set(CabbageIDs::width, 80);
        cabbageIdentifiers.set(CabbageIDs::height, 40);
        var channels;
        channels.append("filebutton");
        cabbageIdentifiers.set(CabbageIDs::channel, channels);
        cabbageIdentifiers.set(CabbageIDs::value, 1);
        cabbageIdentifiers.set(CabbageIDs::text, array);
        cabbageIdentifiers.set(CabbageIDs::caption, "");
        cabbageIdentifiers.set(CabbageIDs::colour, Colours::black.toString());
        cabbageIdentifiers.set(CabbageIDs::fontcolour, Colours::white.toString());
        cabbageIdentifiers.set(CabbageIDs::onfontcolour, Colours::white.toString());
        cabbageIdentifiers.set(CabbageIDs::type, "filebutton");
        cabbageIdentifiers.set(CabbageIDs::name, "filebutton");
        cabbageIdentifiers.set(CabbageIDs::mode, "file");
        cabbageIdentifiers.set(CabbageIDs::identchannel, "");
        cabbageIdentifiers.set("name", cabbageIdentifiers.getWithDefault("name", "").toString()+String(ID));
        var populate;
        populate.append("");
        populate.append("");
        cabbageIdentifiers.set("populate", populate);
        cabbageIdentifiers.set(CabbageIDs::visible, 1);
    }

    //===============button==================//
    else if(strTokens[0].trim() == "button")
    {
        cabbageIdentifiers.set("basetype", "interactive");
        top = 10;
        left = 10;
        width = 80;
        height = 40;
        var array;
        array.append("Push me");
        array.append("Push me");

        cabbageIdentifiers.set(CabbageIDs::top, 10);
        cabbageIdentifiers.set(CabbageIDs::left, 10);
        cabbageIdentifiers.set(CabbageIDs::width, 80);
        cabbageIdentifiers.set(CabbageIDs::height, 40);
        var channels;
        channels.append("buttonchan");
        cabbageIdentifiers.set(CabbageIDs::channel, channels);
        cabbageIdentifiers.set(CabbageIDs::value, 0);
        cabbageIdentifiers.set(CabbageIDs::text, array);
        cabbageIdentifiers.set(CabbageIDs::caption, "");
        cabbageIdentifiers.set(CabbageIDs::colour, Colours::black.toString());
        cabbageIdentifiers.set(CabbageIDs::fontcolour, Colours::white.toString());
        cabbageIdentifiers.set(CabbageIDs::oncolour, Colours::black.toString());
        cabbageIdentifiers.set(CabbageIDs::onfontcolour, Colours::white.toString());
        cabbageIdentifiers.set(CabbageIDs::latched, 1);
        cabbageIdentifiers.set(CabbageIDs::type, "button");
        cabbageIdentifiers.set(CabbageIDs::name, "button");
        cabbageIdentifiers.set(CabbageIDs::name, cabbageIdentifiers.getWithDefault("name", "").toString()+String(ID));
        cabbageIdentifiers.set(CabbageIDs::identchannel, "");
        cabbageIdentifiers.set(CabbageIDs::radiogroup, 0);
        cabbageIdentifiers.set(CabbageIDs::visible, 1);

    }

    //===============checkbox==================//
    else if(strTokens[0].trim() == "checkbox")
    {
        cabbageIdentifiers.set("basetype", "interactive");
        cabbageIdentifiers.set(CabbageIDs::top, 10);
        cabbageIdentifiers.set(CabbageIDs::left, 10);
        cabbageIdentifiers.set(CabbageIDs::width, 100);
        cabbageIdentifiers.set(CabbageIDs::height, 22);
        var channels;
        channels.append("checkchan");
        cabbageIdentifiers.set(CabbageIDs::channel, channels);
        cabbageIdentifiers.set(CabbageIDs::text, "");
        cabbageIdentifiers.set(CabbageIDs::value, 0);
        cabbageIdentifiers.set(CabbageIDs::caption, "");
        cabbageIdentifiers.set(CabbageIDs::shape, "square");
        cabbageIdentifiers.set(CabbageIDs::type, "checkbox");
        cabbageIdentifiers.set(CabbageIDs::oncolour, Colours::lime.toString());
        cabbageIdentifiers.set(CabbageIDs::colour, Colours::black.toString());
        cabbageIdentifiers.set(CabbageIDs::fontcolour, CabbageUtils::getComponentFontColour().toString());
        cabbageIdentifiers.set(CabbageIDs::name, "checkbox");
        cabbageIdentifiers.set(CabbageIDs::name, cabbageIdentifiers.getWithDefault("name", "").toString()+String(ID));
        cabbageIdentifiers.set(CabbageIDs::identchannel, "");
        cabbageIdentifiers.set(CabbageIDs::radiogroup, 0);
        cabbageIdentifiers.set(CabbageIDs::visible, 1);
    }

    //===============numberbox==================//
    else if(strTokens[0].trim() == "numberbox")
    {
        cabbageIdentifiers.set("basetype", "interactive");
        cabbageIdentifiers.set(CabbageIDs::top, 10);
        cabbageIdentifiers.set(CabbageIDs::left, 10);
        cabbageIdentifiers.set(CabbageIDs::width, 100);
        cabbageIdentifiers.set(CabbageIDs::height, 22);
        var channels;
        channels.append("numberbox");
        cabbageIdentifiers.set(CabbageIDs::channel, channels);
        cabbageIdentifiers.set(CabbageIDs::min, 0);
        cabbageIdentifiers.set(CabbageIDs::max, 100);
        cabbageIdentifiers.set(CabbageIDs::value, 50);
        cabbageIdentifiers.set(CabbageIDs::sliderskew, 1);
        cabbageIdentifiers.set(CabbageIDs::sliderincr, .01);
        cabbageIdentifiers.set(CabbageIDs::text, "");
        cabbageIdentifiers.set(CabbageIDs::value, 0);
        cabbageIdentifiers.set(CabbageIDs::caption, "");
        cabbageIdentifiers.set(CabbageIDs::type, "numberbox");
        cabbageIdentifiers.set(CabbageIDs::colour, CabbageUtils::getDarkerBackgroundSkin().toString());
        cabbageIdentifiers.set(CabbageIDs::fontcolour, Colours::whitesmoke.toString());
		cabbageIdentifiers.set(CabbageIDs::textcolour, CabbageUtils::getComponentFontColour().toString());
        cabbageIdentifiers.set(CabbageIDs::name, "numberbox");
        cabbageIdentifiers.set(CabbageIDs::name, cabbageIdentifiers.getWithDefault("name", "").toString()+String(ID));
        cabbageIdentifiers.set(CabbageIDs::identchannel, "");
        cabbageIdentifiers.set(CabbageIDs::visible, 1);
    }
    //===============popupmenu==================//
    else if(strTokens[0].trim() == "popupmenu")
    {
        cabbageIdentifiers.set("basetype", "layout");

        var array;
        array.append("Item 1");
        array.append("Item 2");
        array.append("Item 3");
        array.append("Item 4");

        cabbageIdentifiers.set(CabbageIDs::top, 10);
        cabbageIdentifiers.set(CabbageIDs::left, 10);
        cabbageIdentifiers.set(CabbageIDs::width, 80);
        cabbageIdentifiers.set(CabbageIDs::height, 20);
        var channels;
        channels.append("popupchan");
        cabbageIdentifiers.set(CabbageIDs::channel, channels);
        cabbageIdentifiers.set(CabbageIDs::channeltype, "number");
        cabbageIdentifiers.set(CabbageIDs::text, array);
        cabbageIdentifiers.set(CabbageIDs::value, 1);

        cabbageIdentifiers.set(CabbageIDs::colour, Colours::black.toString());
        cabbageIdentifiers.set(CabbageIDs::fontcolour, CabbageUtils::getComponentFontColour().toString());
        cabbageIdentifiers.set(CabbageIDs::type, "popupmenu");
        cabbageIdentifiers.set(CabbageIDs::name, "popupmenu");

        var populate;
        populate.append("");
        populate.append("");
        cabbageIdentifiers.set(CabbageIDs::populate, populate);
        cabbageIdentifiers.set(CabbageIDs::name, cabbageIdentifiers.getWithDefault("name", "").toString()+String(ID));
        cabbageIdentifiers.set(CabbageIDs::identchannel, "");
        cabbageIdentifiers.set(CabbageIDs::visible, 1);
    }
    //===============combobox==================//
    else if(strTokens[0].trim() == "combobox")
    {
        cabbageIdentifiers.set("basetype", "interactive");

        top = 10;
        left = 10;
        width = 80;
        height = 22;
        //add one item by befault, will be overwritten by users values
        var array;
        array.append("Item 1");
        array.append("Item 2");
        array.append("Item 3");
        array.append("Item 4");
        cabbageIdentifiers.set(CabbageIDs::top, 10);
        cabbageIdentifiers.set(CabbageIDs::left, 10);
        cabbageIdentifiers.set(CabbageIDs::width, 80);
        cabbageIdentifiers.set(CabbageIDs::height, 20);
        var channels;
        channels.append("combochan");
        cabbageIdentifiers.set(CabbageIDs::channel, channels);
        cabbageIdentifiers.set(CabbageIDs::channeltype, "number");
        cabbageIdentifiers.set(CabbageIDs::text, array);
        cabbageIdentifiers.set(CabbageIDs::value, 1);
        cabbageIdentifiers.set(CabbageIDs::caption, "");
        cabbageIdentifiers.set(CabbageIDs::colour, Colours::black.toString());
        cabbageIdentifiers.set(CabbageIDs::fontcolour, CabbageUtils::getComponentFontColour().toString());
        cabbageIdentifiers.set(CabbageIDs::type, "combobox");
        cabbageIdentifiers.set(CabbageIDs::name, "combobox");
        cabbageIdentifiers.set(CabbageIDs::comborange, 4);
        var populate;
        populate.append("");
        populate.append("");
        cabbageIdentifiers.set(CabbageIDs::populate, populate);
        cabbageIdentifiers.set(CabbageIDs::name, cabbageIdentifiers.getWithDefault("name", "").toString()+String(ID));
        cabbageIdentifiers.set(CabbageIDs::identchannel, "");
        cabbageIdentifiers.set(CabbageIDs::visible, 1);
    }
    //===============label==================//
    else if(strTokens[0].trim() == "label")
    {
        cabbageIdentifiers.set("basetype", "layout");
        cabbageIdentifiers.set(CabbageIDs::top, 10);
        cabbageIdentifiers.set(CabbageIDs::left, 10);
        cabbageIdentifiers.set(CabbageIDs::width, 80);
        cabbageIdentifiers.set(CabbageIDs::height, 16);
        cabbageIdentifiers.set(CabbageIDs::text, "hello");
        cabbageIdentifiers.set(CabbageIDs::min, 1);
        cabbageIdentifiers.set(CabbageIDs::name, "label");
        cabbageIdentifiers.set(CabbageIDs::type, "label");
        cabbageIdentifiers.set(CabbageIDs::type, cabbageIdentifiers.getWithDefault("name", "").toString());
        cabbageIdentifiers.set(CabbageIDs::name, cabbageIdentifiers.getWithDefault("name", "").toString()+String(ID));
        cabbageIdentifiers.set(CabbageIDs::colour, Colours::transparentBlack.toString());
        cabbageIdentifiers.set(CabbageIDs::fontcolour, CabbageUtils::getComponentFontColour().toString());
        cabbageIdentifiers.set(CabbageIDs::align, "centre");
        cabbageIdentifiers.set(CabbageIDs::identchannel, "");
        cabbageIdentifiers.set(CabbageIDs::visible, 1);
    }
    //===============multitab==================//
    else if(strTokens[0].trim() == "multitab")
    {
        /*
        cabbageIdentifiers.set("basetype", "layout");
        top = 10;
        left = 10;
        width = 100;
        height = 16;
        channel = "multitab";
        colour = CabbageUtils::getComponentFontColour();
        fontcolour = CabbageUtils::getComponentFontColour();
        items.add("Tab 1");
        name = "multitab";
        type = name;
        name.append(String(ID), 1024);
         */
    }
    //===============soundfiler==================//
    else if(strTokens[0].trim() == "soundfiler")
    {
        cabbageIdentifiers.set("basetype", "layout");
        top = 10;
        left = 10;
        width = 300;
        height = 200;

        cabbageIdentifiers.set(CabbageIDs::top, 10);
        cabbageIdentifiers.set(CabbageIDs::left, 10);
        cabbageIdentifiers.set(CabbageIDs::width, 300);
        cabbageIdentifiers.set(CabbageIDs::height, 200);
        var channels;
        channels.append("pos");
        channels.append("end");
        cabbageIdentifiers.set(CabbageIDs::channel, channels);
        cabbageIdentifiers.set(CabbageIDs::colour, CabbageUtils::getComponentFontColour().toString());
        cabbageIdentifiers.set(CabbageIDs::fontcolour, CabbageUtils::getComponentFontColour().toString());
        cabbageIdentifiers.set(CabbageIDs::type, "soundfiler");
        cabbageIdentifiers.set(CabbageIDs::name, "soundfiler");
        cabbageIdentifiers.set(CabbageIDs::file, "");
        cabbageIdentifiers.set(CabbageIDs::name, cabbageIdentifiers.getWithDefault("name", "").toString()+String(ID));
        cabbageIdentifiers.set(CabbageIDs::identchannel, "");
        cabbageIdentifiers.set(CabbageIDs::visible, 1);
        cabbageIdentifiers.set(CabbageIDs::scrubberposition, 0);
        cabbageIdentifiers.set(CabbageIDs::zoom, 0);
        cabbageIdentifiers.set(CabbageIDs::startpos, 0);
        cabbageIdentifiers.set(CabbageIDs::tablenumber, -1);
    }
    //===============gentable==================//
    else if(strTokens[0].trim() == "gentable")
    {
        cabbageIdentifiers.set("basetype", "layout");
        top = 10;
        left = 10;
        width = 300;
        height = 200;

        var tableColours;


        tableColours.append("white");
        tableColours.append("cornflowerblue");
        tableColours.append("yellow");
        tableColours.append("lime");
        tableColours.append("green");
        tableColours.append("pink");
        tableColours.append("brown");
        tableColours.append("purple");
        tableColours.append("lightblue");
        tableColours.append("darkgreen");
        tableColours.append("lightgreen");
        tableColours.append("mango");



        cabbageIdentifiers.set(CabbageIDs::top, 10);
        cabbageIdentifiers.set(CabbageIDs::left, 10);
        cabbageIdentifiers.set(CabbageIDs::width, 300);
        cabbageIdentifiers.set(CabbageIDs::height, 200);
        var channels;
        channels.append("pos");
        channels.append("end");
        cabbageIdentifiers.set(CabbageIDs::channel, channels);
        cabbageIdentifiers.set(CabbageIDs::colour, CabbageUtils::getComponentFontColour().toString());
        cabbageIdentifiers.set(CabbageIDs::fontcolour, CabbageUtils::getComponentFontColour().toString());
        cabbageIdentifiers.set(CabbageIDs::type, "gentable");
        cabbageIdentifiers.set(CabbageIDs::tablecolour, tableColours);
        cabbageIdentifiers.set(CabbageIDs::name, "gentable");
        cabbageIdentifiers.set(CabbageIDs::amprange, 0);
        cabbageIdentifiers.set(CabbageIDs::drawmode, "");
        cabbageIdentifiers.set(CabbageIDs::file, "");
        cabbageIdentifiers.set(CabbageIDs::name, cabbageIdentifiers.getWithDefault("name", "").toString()+String(ID));
        cabbageIdentifiers.set(CabbageIDs::identchannel, "");
        cabbageIdentifiers.set(CabbageIDs::visible, 1);
        cabbageIdentifiers.set(CabbageIDs::scrubberposition, 0);
        cabbageIdentifiers.set(CabbageIDs::zoom, 0);
        cabbageIdentifiers.set(CabbageIDs::startpos, 0);
        cabbageIdentifiers.set(CabbageIDs::tablenumber, -1);
		cabbageIdentifiers.set(CabbageIDs::tablegridcolour, Colours::whitesmoke.withAlpha(.1f).toString());
    }

    //===============image==================//
    else if(strTokens[0].trim() == "image")
    {
        cabbageIdentifiers.set("basetype", "layout");
        top = 10;
        left = 10;
        width = 160;
        height = 120;


        cabbageIdentifiers.set(CabbageIDs::top, 10);
        cabbageIdentifiers.set(CabbageIDs::left, 10);
        cabbageIdentifiers.set(CabbageIDs::width, 160);
        cabbageIdentifiers.set(CabbageIDs::height, 120);
        cabbageIdentifiers.set(CabbageIDs::shape, "rounded");
        cabbageIdentifiers.set(CabbageIDs::colour, Colours::white.toString());
        cabbageIdentifiers.set(CabbageIDs::outlinecolour, Colours::white.toString());
        cabbageIdentifiers.set(CabbageIDs::outlinethickness, 0);
        cabbageIdentifiers.set(CabbageIDs::popup, 0);
        cabbageIdentifiers.set(CabbageIDs::plant, "");
        cabbageIdentifiers.set(CabbageIDs::type, "image");
        cabbageIdentifiers.set(CabbageIDs::file, "");
        cabbageIdentifiers.set(CabbageIDs::name, "image");
        cabbageIdentifiers.set(CabbageIDs::name, cabbageIdentifiers.getWithDefault("name", "").toString()+String(ID));
        cabbageIdentifiers.set(CabbageIDs::identchannel, "");
        cabbageIdentifiers.set(CabbageIDs::visible, 1);
    }
    //===============groupbox==================//
    else if(strTokens[0].trim() == "groupbox")
    {
        cabbageIdentifiers.set(CabbageIDs::basetype, "layout");
        cabbageIdentifiers.set(CabbageIDs::top, 10);
        cabbageIdentifiers.set(CabbageIDs::left, 10);
        cabbageIdentifiers.set(CabbageIDs::width, 80);
        cabbageIdentifiers.set(CabbageIDs::height, 22);
        cabbageIdentifiers.set(CabbageIDs::colour, CabbageUtils::getComponentSkin().toString());
        cabbageIdentifiers.set(CabbageIDs::fontcolour, CabbageUtils::getComponentFontColour().toString());
        cabbageIdentifiers.set(CabbageIDs::popup, 0);
        cabbageIdentifiers.set(CabbageIDs::plant, "");
        cabbageIdentifiers.set(CabbageIDs::child, 0);
        cabbageIdentifiers.set(CabbageIDs::linethickness, 1);
        cabbageIdentifiers.set(CabbageIDs::type, "groupbox");
        cabbageIdentifiers.set(CabbageIDs::name, "groupbox");
        cabbageIdentifiers.set(CabbageIDs::name, cabbageIdentifiers.getWithDefault("name", "").toString()+String(ID));
        cabbageIdentifiers.set(CabbageIDs::identchannel, "");
        cabbageIdentifiers.set(CabbageIDs::visible, 1);

    }
    //===============line==================//
    else if(strTokens[0].trim() == "line")
    {
        cabbageIdentifiers.set("basetype", "layout");
        top = 10;
        left = 10;
        width = 180;
        height = 2;

        cabbageIdentifiers.set(CabbageIDs::top, 10);
        cabbageIdentifiers.set(CabbageIDs::left, 10);
        cabbageIdentifiers.set(CabbageIDs::width, 160);
        cabbageIdentifiers.set(CabbageIDs::height, 2);
        cabbageIdentifiers.set(CabbageIDs::colour, Colours::white.toString());
        cabbageIdentifiers.set(CabbageIDs::type, "line");
        cabbageIdentifiers.set(CabbageIDs::name, "line");
        cabbageIdentifiers.set(CabbageIDs::name, cabbageIdentifiers.getWithDefault("name", "").toString()+String(ID));
        cabbageIdentifiers.set(CabbageIDs::identchannel, "");
        cabbageIdentifiers.set(CabbageIDs::visible, 1);
    }

    //===============csoundoutput==================//
    else if(strTokens[0].trim() == "csoundoutput")
    {
        cabbageIdentifiers.set("basetype", "layout");
        top = 10;
        left = 10;
        width = 400;
        height = 200;
        cabbageIdentifiers.set(CabbageIDs::top, 10);
        cabbageIdentifiers.set(CabbageIDs::left, 10);
        cabbageIdentifiers.set(CabbageIDs::width, 400);
        cabbageIdentifiers.set(CabbageIDs::text, "Csound output");
        cabbageIdentifiers.set(CabbageIDs::height, 200);
        cabbageIdentifiers.set(CabbageIDs::colour, Colours::black.toString());
        cabbageIdentifiers.set(CabbageIDs::fontcolour, Colours::cornflowerblue.toString());
        cabbageIdentifiers.set(CabbageIDs::name, "csoundoutput");
        cabbageIdentifiers.set(CabbageIDs::wrap, 0);

        cabbageIdentifiers.set(CabbageIDs::type, "csoundoutput");
        cabbageIdentifiers.set(CabbageIDs::name, cabbageIdentifiers.getWithDefault("name", "").toString()+String(ID));
        cabbageIdentifiers.set(CabbageIDs::identchannel, "");
        cabbageIdentifiers.set(CabbageIDs::visible, 1);
    }
    //===============textbox==================//
    else if(strTokens[0].trim() == "textbox")
    {
        cabbageIdentifiers.set("basetype", "layout");
        top = 10;
        left = 10;
        width = 400;
        height = 200;
        cabbageIdentifiers.set(CabbageIDs::top, 10);
        cabbageIdentifiers.set(CabbageIDs::left, 10);
        cabbageIdentifiers.set(CabbageIDs::width, 400);
        cabbageIdentifiers.set(CabbageIDs::text, "");
        cabbageIdentifiers.set(CabbageIDs::height, 200);
        cabbageIdentifiers.set(CabbageIDs::colour, Colours::black.toString());
        cabbageIdentifiers.set(CabbageIDs::fontcolour, Colours::white.toString());
        cabbageIdentifiers.set(CabbageIDs::name, "textbox");
        cabbageIdentifiers.set(CabbageIDs::type, "textbox");
        cabbageIdentifiers.set(CabbageIDs::wrap, 0);
        cabbageIdentifiers.set(CabbageIDs::file, "");
        cabbageIdentifiers.set(CabbageIDs::name, cabbageIdentifiers.getWithDefault("name", "").toString()+String(ID));
        cabbageIdentifiers.set(CabbageIDs::identchannel, "");
        cabbageIdentifiers.set(CabbageIDs::visible, 1);
    }

    //===============textbox==================//
    else if(strTokens[0].trim() == "texteditor")
    {
        cabbageIdentifiers.set("basetype", "layout");
        top = 10;
        left = 10;
        width = 400;
        height = 200;
        cabbageIdentifiers.set(CabbageIDs::top, 10);
        cabbageIdentifiers.set(CabbageIDs::left, 10);
        cabbageIdentifiers.set(CabbageIDs::width, 400);
        cabbageIdentifiers.set(CabbageIDs::text, "");
        cabbageIdentifiers.set(CabbageIDs::height, 200);
        cabbageIdentifiers.set(CabbageIDs::channel, "texteditor");
        cabbageIdentifiers.set(CabbageIDs::colour, Colours::white.toString());
        cabbageIdentifiers.set(CabbageIDs::fontcolour, Colours::black.toString());
        cabbageIdentifiers.set(CabbageIDs::name, "texteditor");
        cabbageIdentifiers.set(CabbageIDs::type, "texteditor");
        cabbageIdentifiers.set(CabbageIDs::name, cabbageIdentifiers.getWithDefault("name", "").toString()+String(ID));
        cabbageIdentifiers.set(CabbageIDs::identchannel, "");
        cabbageIdentifiers.set(CabbageIDs::visible, 1);
    }


    //===============vemeter==================//
    else if(strTokens[0].trim() == "vumeter")
    {
        /*
        cabbageIdentifiers.set("basetype", "layout");
        top = 10;
        left = 10;
        width = 400;
        height = 200;
        vuConfig.clear();
        colour = Colours::white;
        name = "vumeter";
        type = name;
        name.append(String(ID), 1024);
         */
    }
    //===============table==================//
    else if(strTokens[0].trim() == "table")
    {
        cabbageIdentifiers.set("basetype", "layout");
        top = 10;
        left = 10;
        width = 400;
        height = 200;

        var tableColours;
        tableColours.append("white");
        tableColours.append("cornflowerblue");
        tableColours.append("yellow");
        tableColours.append("lime");
        tableColours.append("green");
        tableColours.append("pink");

        cabbageIdentifiers.set(CabbageIDs::top, 10);
        cabbageIdentifiers.set(CabbageIDs::left, 10);
        cabbageIdentifiers.set(CabbageIDs::width, 400);
        cabbageIdentifiers.set(CabbageIDs::height, 200);
        cabbageIdentifiers.set(CabbageIDs::tablenumber, 1);
        cabbageIdentifiers.set(CabbageIDs::drawmode, "");
        cabbageIdentifiers.set(CabbageIDs::resizemode, 0);
        cabbageIdentifiers.set(CabbageIDs::readonly, 0);
        cabbageIdentifiers.set(CabbageIDs::tablecolour, tableColours);
        cabbageIdentifiers.set(CabbageIDs::amprange, 0);
        cabbageIdentifiers.set(CabbageIDs::type, "table");
        cabbageIdentifiers.set(CabbageIDs::stack, 0);
        cabbageIdentifiers.set(CabbageIDs::name, cabbageIdentifiers.getWithDefault("name", "").toString()+String(ID));
        cabbageIdentifiers.set(CabbageIDs::identchannel, "");
        cabbageIdentifiers.set(CabbageIDs::visible, 1);
    }
    else if(strTokens[0].trim() == "pvsview")
    {
        /*
        cabbageIdentifiers.set("basetype", "layout");
          top = 10;
          left = 10;
          width = 400;
          height = 200;
          colour = Colours::white;
          name = "pvsview";
          type = name;
          value = 1;
          tableNum = 1;
          name.append(String(ID), 1024);
          overlapSize = 256;
          fftSize = 1024;
          frameSize = 1024;
           */
    }
    //===============xypad==================//
    else if(strTokens[0].trim() == "xypad")
    {
        cabbageIdentifiers.set("basetype", "interactive");
        top = 10;
        left = 10;
        width = 200;
        height = 200;

        var channels;
        channels.append("xChan");
        channels.append("yChan");
        cabbageIdentifiers.set(CabbageIDs::xyautoindex, 0);
        cabbageIdentifiers.set(CabbageIDs::xchannel, channels[0]);
        cabbageIdentifiers.set(CabbageIDs::ychannel, channels[1]);
        cabbageIdentifiers.set(CabbageIDs::top, 10);
        cabbageIdentifiers.set(CabbageIDs::left, 10);
        cabbageIdentifiers.set(CabbageIDs::caption, "");
        cabbageIdentifiers.set(CabbageIDs::width, 200);
        cabbageIdentifiers.set(CabbageIDs::height, 200);
        cabbageIdentifiers.set(CabbageIDs::minx, 0);
        cabbageIdentifiers.set(CabbageIDs::maxx, 200);
        cabbageIdentifiers.set(CabbageIDs::text, "");
        cabbageIdentifiers.set(CabbageIDs::miny, 0);
        cabbageIdentifiers.set(CabbageIDs::maxy, 200);
        cabbageIdentifiers.set(CabbageIDs::valuex, 0);
        cabbageIdentifiers.set(CabbageIDs::valuey, 0);
        cabbageIdentifiers.set(CabbageIDs::colour, Colours::lime.toString());
        cabbageIdentifiers.set(CabbageIDs::fontcolour, Colours::cornflowerblue.toString());
        cabbageIdentifiers.set(CabbageIDs::type, "xypad");
        cabbageIdentifiers.set(CabbageIDs::name, "xypad");
        cabbageIdentifiers.set(CabbageIDs::name, cabbageIdentifiers.getWithDefault("name", "").toString()+String(ID));
        cabbageIdentifiers.set(CabbageIDs::identchannel, "");
        cabbageIdentifiers.set(CabbageIDs::visible, 1);
    }
    //===============keyboard==================//
    else if(strTokens[0].trim() == "keyboard")
    {
        cabbageIdentifiers.set("basetype", "layout");
        top = 10;
        left = 10;
        width = 400;
        height = 100;

        cabbageIdentifiers.set(CabbageIDs::top, 10);
        cabbageIdentifiers.set(CabbageIDs::left, 10);
        cabbageIdentifiers.set(CabbageIDs::width, 400);
        cabbageIdentifiers.set(CabbageIDs::height, 100);
        cabbageIdentifiers.set(CabbageIDs::value, 60);
        cabbageIdentifiers.set(CabbageIDs::type, "keyboard");
        cabbageIdentifiers.set(CabbageIDs::name, "keyboard");
        cabbageIdentifiers.set(CabbageIDs::name, cabbageIdentifiers.getWithDefault("name", "").toString()+String(ID));
        cabbageIdentifiers.set(CabbageIDs::visible, 1);
    }
    //===============form==================//
    else if(strTokens[0].trim() == "form")
    {
        cabbageIdentifiers.set(CabbageIDs::basetype, "layout");
        cabbageIdentifiers.set(CabbageIDs::top, 10);
        cabbageIdentifiers.set(CabbageIDs::left, 10);
        cabbageIdentifiers.set(CabbageIDs::width, 600);
        cabbageIdentifiers.set(CabbageIDs::height, 300);
        cabbageIdentifiers.set(CabbageIDs::caption, "");
        cabbageIdentifiers.set(CabbageIDs::name, "form");
        cabbageIdentifiers.set(CabbageIDs::type, "form");
        cabbageIdentifiers.set(CabbageIDs::guirefresh, 100);
        cabbageIdentifiers.set(CabbageIDs::identchannel, "");
        cabbageIdentifiers.set(CabbageIDs::visible, 1);
        cabbageIdentifiers.set(CabbageIDs::scrollbars, 1);

    }

    else if(strTokens[0].trim() == "hostbpm")
    {
        cabbageIdentifiers.set(CabbageIDs::basetype, "layout");
        cabbageIdentifiers.set(CabbageIDs::name, "hostbpm");
        cabbageIdentifiers.set(CabbageIDs::type, "hostbpm");
    }

    else if(strTokens[0].trim() == "hostppqpos")
    {
        cabbageIdentifiers.set(CabbageIDs::basetype, "layout");
        cabbageIdentifiers.set(CabbageIDs::name, "hostppqpos");
        cabbageIdentifiers.set(CabbageIDs::type, "hostppqpos");
    }

    else if(strTokens[0].trim() == "hostplaying")
    {
        cabbageIdentifiers.set(CabbageIDs::basetype, "layout");
        cabbageIdentifiers.set(CabbageIDs::name, "hostplaying");
        cabbageIdentifiers.set(CabbageIDs::type, "hostplaying");
    }

    else if(strTokens[0].trim() == "hostrecording")
    {
        cabbageIdentifiers.set(CabbageIDs::basetype, "layout");
        cabbageIdentifiers.set(CabbageIDs::name, "hostrecording");
        cabbageIdentifiers.set(CabbageIDs::type, "hostrecording");
    }

    else if(strTokens[0].trim() == "hosttime")
    {
        cabbageIdentifiers.set(CabbageIDs::basetype, "layout");
        cabbageIdentifiers.set(CabbageIDs::name, "hosttime");
        cabbageIdentifiers.set(CabbageIDs::type, "hosttime");
    }

//parse the text now that all default values ahve been assigned
    parse(compStr, "");
}

CabbageGUIClass::~CabbageGUIClass()
{

}
//===========================================================================================
// this method parsing the Cabbage text and set each of the Cabbage indentifers
//===========================================================================================
int CabbageGUIClass::parse(String inStr, String identifier)
{
    IdentArray identArray;
    //Logger::writeToLog(str);
    //remove any text after a semicolon and take out tabs..
    String str = inStr.removeCharacters("\t");
    if(str.indexOf(0, ";")!=-1)
        str = str.substring(0, str.indexOf(0, ";"));

    //if identifier string is not empty, use that, otherwise
    //search through identArray list of known identifiers
    if(identifier.isNotEmpty())
        identArray.add(identifier);
    else
        identArray = *IdentArray::getInstance(); // creates the singleton if there isn't already one.



    String typeOfWidget="";

    //Logger::writeToLog(String(identArray.size()));
    //retrieve paramters consisting of strings
    StringArray strTokens, tempTokens;
    tempTokens.addTokens(str, ", \t", "\"");
    typeOfWidget = tempTokens[0];
    //Logger::writeToLog(typeOfWidget);

    for(int indx=0; indx<identArray.size(); indx++)
    {
        //check to see if identifier is part of input string..turn to lowercase first..
        //Logger::writeToLog("index:"+String(indx)+" arrayItem:"+identArray.getReference(indx)+" line:\n"+str);
        //int identPos = str.toLowerCase().replaceCharacters("()", "  ").indexOfWholeWordIgnoreCase(identArray[indx]);
        //if(str.substring(identPos).toLowerCase().contains(identArray[indx]+"("))
        int identPos = str.toLowerCase().indexOf(" "+identArray[indx]+"(");
        if(identPos<0)
            identPos = str.toLowerCase().indexOf(","+identArray[indx]+"(");
        if(identPos>-1)
        {
            String newString = str.substring(identPos+identArray[indx].length());
            //showMessage(newString, nullptr);
            String tstr = newString.substring(newString.indexOf("(")+1, newString.indexOf(0, ")"));
            //showMessage(tstr, nullptr);

            if(tstr.length()==0)
                return 0;



            strTokens.addTokens(tstr.removeCharacters("\t)\""), ",", "\"");

            if(identArray[indx].equalsIgnoreCase("name"))
            {
                cabbageIdentifiers.set(CabbageIDs::name, strTokens[0].trim());
            }
            else if(identArray[indx].equalsIgnoreCase("plant"))
            {
                cabbageIdentifiers.set(CabbageIDs::plant, strTokens[0].trim());
            }
            else if(identArray[indx].equalsIgnoreCase("caption"))
            {
                cabbageIdentifiers.set(CabbageIDs::caption, strTokens[0].trim());
            }

            else if(identArray[indx].equalsIgnoreCase("channel")||
                    identArray[indx].equalsIgnoreCase("channels")||
                    identArray[indx].equalsIgnoreCase("channelarray"))
            {
                var array;
                array.append(strTokens[0].trim());
                if(str.containsIgnoreCase("xypad"))
                {
                    cabbageIdentifiers.set(CabbageIDs::xchannel, strTokens[0].trim());
                    cabbageIdentifiers.set(CabbageIDs::ychannel, strTokens[1].trim());
                }
                else if(str.containsIgnoreCase("vumeter"))
                {
                    array.resize(0);
                    for(int u=0; u<strTokens.size(); u++)
                    {
                        array.append(strTokens[u].trim());
                    }
                }
                else
                {
                    if(strTokens.size()>1)
                        for(int i=1; i<strTokens.size(); i++)
                        {
                            array.append(strTokens[i].trim());
                        }
                }

                cabbageIdentifiers.set(CabbageIDs::channel, array);
                if(identArray[indx].equalsIgnoreCase("channelarray"))
                {
                    var channelArray, identChannelArray;
                    int size = strTokens[1].getIntValue();
                    for(int i=0; i<size; i++)
                    {
                        channelArray.append(String(strTokens[0])+String(i+1));
                        identChannelArray.append(String(strTokens[0])+String("_ident")+String(i+1));

                    }
                    cabbageIdentifiers.set(CabbageIDs::channelarray, channelArray);
                    cabbageIdentifiers.set(CabbageIDs::identchannelarray, identChannelArray);
                }
            }

            else if(identArray[indx].equalsIgnoreCase("colour:0")||
                    identArray[indx].equalsIgnoreCase("colours("))
            {
                cabbageIdentifiers.set(CabbageIDs::colour, getColourFromText(strTokens.joinIntoString(",")).toString());
            }

            else if(identArray[indx].equalsIgnoreCase("colour:1") ||
                    identArray[indx].equalsIgnoreCase("colour"))
            {
                if(typeOfWidget=="checkbox" || typeOfWidget=="button")
                {
                    cabbageIdentifiers.set(CabbageIDs::oncolour, getColourFromText(strTokens.joinIntoString(",")).toString());
                }
                else
                    cabbageIdentifiers.set(CabbageIDs::colour, getColourFromText(strTokens.joinIntoString(",")).toString());
            }

            else if(identArray[indx].equalsIgnoreCase("fontcolour")||
                    identArray[indx].equalsIgnoreCase("fontcolour:1"))
            {
                if(typeOfWidget=="button")
                    cabbageIdentifiers.set(CabbageIDs::onfontcolour, getColourFromText(strTokens.joinIntoString(",")).toString());
                else
                    cabbageIdentifiers.set(CabbageIDs::fontcolour, getColourFromText(strTokens.joinIntoString(",")).toString());

            }
            else if(identArray[indx].equalsIgnoreCase("fontcolour:0"))
            {
                cabbageIdentifiers.set(CabbageIDs::fontcolour, getColourFromText(strTokens.joinIntoString(",")).toString());
            }
			
            else if(identArray[indx].equalsIgnoreCase("tablegridcolour"))
            {
                cabbageIdentifiers.set(CabbageIDs::tablegridcolour, getColourFromText(strTokens.joinIntoString(",")).toString());
            }
			
            else if(identArray[indx].equalsIgnoreCase("tablecolour") ||
                    identArray[indx].equalsIgnoreCase("tablecolours"))
            {
                var colours;
                for(int i=0; i<strTokens.size(); i++)
                {
                    colours.append(strTokens[i].trim());
                    //Logger::writeToLog(strTokens[i].trim());
                }
                cabbageIdentifiers.set(CabbageIDs::tablecolour, colours);
            }

            else if(identArray[indx].equalsIgnoreCase("trackercolour"))
            {
                cabbageIdentifiers.set(CabbageIDs::trackercolour, getColourFromText(strTokens.joinIntoString(",")).toString());
            }

            else if(identArray[indx].equalsIgnoreCase("kind"))
            {
                cabbageIdentifiers.set(CabbageIDs::kind, strTokens[0].trim());
            }

            else if(identArray[indx].equalsIgnoreCase("file"))
            {
                cabbageIdentifiers.set(CabbageIDs::file, strTokens[0].trim());
            }

            else if(identArray[indx].equalsIgnoreCase("svgpath"))
            {
                cabbageIdentifiers.set(CabbageIDs::svgpath, strTokens[0].trim());
            }

            else if(identArray[indx].equalsIgnoreCase("fillcolour"))
            {
                cabbageIdentifiers.set(CabbageIDs::fillcolour, getColourFromText(strTokens.joinIntoString(",")).toString());
            }

            else if(identArray[indx].equalsIgnoreCase("shape"))
            {
                cabbageIdentifiers.set(CabbageIDs::shape, strTokens[0].trim());
            }
            else if(identArray[indx].equalsIgnoreCase("outlinecolour"))
            {
                cabbageIdentifiers.set(CabbageIDs::outlinecolour, getColourFromText(strTokens.joinIntoString(",")).toString());
            }
            else if(identArray[indx].equalsIgnoreCase("textcolour"))
            {
                cabbageIdentifiers.set(CabbageIDs::textcolour, getColourFromText(strTokens.joinIntoString(",")).toString());
            }

            else if(identArray[indx].equalsIgnoreCase("pluginid"))
            {
                cabbageIdentifiers.set(CabbageIDs::pluginid, strTokens[0].trim());
            }

            else if(identArray[indx].equalsIgnoreCase("items")||
                    identArray[indx].equalsIgnoreCase("text"))
            {
                //CabbageUtils::showMessage(typeOfWidget+":"+text);
                var value;
                int comboRange = 0;
                for(int i= 0; i<(int)strTokens.size(); i++)
                {
                    //Logger::writeToLog(strTokens[i]);
                    String test = strTokens[i];
                    value.append(strTokens[i].trim());
                    //if((int)strTokens.size()<2) {
                    //	items.add(strTokens[i]);
                    //	value.append(strTokens[i].trim());
                    //	i++;
                    //	}
                    comboRange = i+1;
                }
                //Logger::writeToLog(String(value.size())+typeOfWidget);
                if((value.size()==1) && ((typeOfWidget=="button") || (typeOfWidget=="filebutton")))
                {
                    value.append(strTokens[0]);
                    //Logger::writeToLog(strTokens[0].trim());
                }

                cabbageIdentifiers.set(CabbageIDs::text, value);
                cabbageIdentifiers.set(CabbageIDs::comborange, comboRange);
            }


            else if(identArray[indx].equalsIgnoreCase("preset"))
            {
                //CabbageUtils::showMessage("preset is no longer supported, see docs");
            }

            else if(identArray[indx].equalsIgnoreCase("mode") ||
                    identArray[indx].equalsIgnoreCase(",mode"))
            {
                cabbageIdentifiers.set(CabbageIDs::mode, strTokens[0].trim());
            }

            else if(identArray[indx].equalsIgnoreCase("native"))
            {
                cabbageIdentifiers.set("native", strTokens[0].trim());
            }

            else if(identArray[indx].equalsIgnoreCase("channeltype"))
            {
                cabbageIdentifiers.set(CabbageIDs::channeltype, strTokens[0].trim());
            }

            else if(identArray[indx].equalsIgnoreCase("identchannel"))
            {
                cabbageIdentifiers.set(CabbageIDs::identchannel, strTokens[0].trim());
            }

            else if(identArray[indx].equalsIgnoreCase("populate"))
            {
                cabbageIdentifiers.set(CabbageIDs::filetype, strTokens[0].trim());
                if(strTokens.size()>1)
                {
                    cabbageIdentifiers.set(CabbageIDs::workingdir, strTokens[1].trim());
                }
            }

            else if(identArray[indx].equalsIgnoreCase("author"))
            {
                cabbageIdentifiers.set(CabbageIDs::author, strTokens[0].trim());
            }

            else if(identArray[indx].equalsIgnoreCase("align"))
            {
                cabbageIdentifiers.set(CabbageIDs::align, strTokens[0].trim());
            }


            else if(identArray[indx].equalsIgnoreCase("tabs"))
            {
                var value;
                for(int i= 0; i<(int)strTokens.size(); i++)
                {
                    value.append(strTokens[i].trim());
                }
                cabbageIdentifiers.set(CabbageIDs::tabs, strTokens[0].trim());
            }

            else if(identArray[indx].equalsIgnoreCase("tabpage"))
            {
                cabbageIdentifiers.set(CabbageIDs::tabpage, strTokens[0].trim());
            }

            else if(identArray[indx].equalsIgnoreCase("include"))
            {
                var array;
                for(int i=0; i<strTokens.size(); i++)
                {
                    array.append(strTokens[i].trim());
                }

                cabbageIdentifiers.set(CabbageIDs::include, array);
            }

            else if(identArray[indx].equalsIgnoreCase("drawmode"))
            {
                var value;
                if(strTokens.size()>0)
                    for(int i=0; i<strTokens.size(); i++)
                    {
                        value.append(strTokens[i].trim());
                    }
                cabbageIdentifiers.set(CabbageIDs::drawmode, value);
            }
            //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //numeric paramters
            //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            else if(identArray[indx].equalsIgnoreCase("socket"))
            {
                if(strTokens.size()>=2)
                {
                    //sizeText = identArray.getReference(indx)+tstr+")";
                    String address = strTokens[0].trim();
                    int port = strTokens[1].trim().getIntValue();
                    cabbageIdentifiers.set(CabbageIDs::oscaddress, address);
                    cabbageIdentifiers.set(CabbageIDs::oscport, port);
                }
            }

            else if(identArray[indx].equalsIgnoreCase("size"))
            {
                if(strTokens.size()>=2)
                {
                    //sizeText = identArray.getReference(indx)+tstr+")";
                    width = strTokens[0].trim().getFloatValue();
                    height = strTokens[1].trim().getFloatValue();
                    cabbageIdentifiers.set(CabbageIDs::width, strTokens[0].trim().getFloatValue());
                    cabbageIdentifiers.set(CabbageIDs::height, strTokens[1].trim().getFloatValue());
                }
            }

            else if(identArray[indx].equalsIgnoreCase("bounds"))
            {
                if(strTokens.size()<3)
                {
                    //debugMessage ="WARNING: Not enough paramters passed to bounds(): usage pos(top, left width, height\")";
                }
                else
                {
                    left = strTokens[0].trim().getFloatValue();
                    top = strTokens[1].trim().getFloatValue();
                    width = strTokens[2].trim().getFloatValue();
                    if(strTokens.size()==3)
                    {
                        height = width;
                        cabbageIdentifiers.set(CabbageIDs::height, width);
                    }
                    else
                    {
                        height = strTokens[3].trim().getFloatValue();
                        cabbageIdentifiers.set(CabbageIDs::left, strTokens[0].trim().getFloatValue());
                        cabbageIdentifiers.set(CabbageIDs::top, strTokens[1].trim().getFloatValue());
                        cabbageIdentifiers.set(CabbageIDs::width, strTokens[2].trim().getFloatValue());
                        cabbageIdentifiers.set(CabbageIDs::height, strTokens[3].trim().getFloatValue());
                    }

                }
            }
            //!!! This haven't been set for the generic data system
            else if(identArray[indx].equalsIgnoreCase("config"))
            {
                vuConfig.clear();
                for(int i= 0; i<(int)strTokens.size(); i++)
                {
                    vuConfig.add(strTokens[i].trim().getFloatValue());
                }
            }

            else if(identArray[indx].equalsIgnoreCase("pos"))
            {
                if(strTokens.size()<2)
                {
                    //debugMessage ="WARNING: Not enough paramters passed to pos(): usage pos(top, left\")";
                }
                else
                {
                    left = strTokens[0].trim().getFloatValue();
                    top = strTokens[1].trim().getFloatValue();
                    cabbageIdentifiers.set(CabbageIDs::left, strTokens[0].trim().getFloatValue());
                    cabbageIdentifiers.set(CabbageIDs::top, strTokens[1].trim().getFloatValue());
                }
            }

            else if(identArray[indx].equalsIgnoreCase("range"))
            {
                if(strTokens.size()<3)
                {
                    //debugMessage ="WARNING: Not enough paramters passed to range(): usage range(minx, max, value, incr\")";
                }
                else
                {
                    StringArray tempArray;
                    double min = strTokens[0].trim().getDoubleValue();// getFloatValue();
                    double max = strTokens[1].trim().getDoubleValue();//.getFloatValue();
                    cabbageIdentifiers.set(CabbageIDs::min, strTokens[0].trim().getDoubleValue());
                    cabbageIdentifiers.set(CabbageIDs::max, strTokens[1].trim().getDoubleValue());
                    tempArray.add(strTokens[0].trim());
                    tempArray.add(strTokens[1].trim());
                    if(strTokens.size()>2)
                    {
                        cabbageIdentifiers.set(CabbageIDs::value, strTokens[2].trim().getDoubleValue());
                        tempArray.add(strTokens[2].trim());
                    }


                    if(strTokens.size()>3)
                    {
                        cabbageIdentifiers.set(CabbageIDs::sliderskew, strTokens[3].trim().getDoubleValue());
                        tempArray.add(strTokens[3].trim());
                    }

                    if(strTokens.size()>4)
                    {
                        tempArray.add(strTokens[4].trim());
                        cabbageIdentifiers.set(CabbageIDs::sliderincr, strTokens[4].trim().getDoubleValue());
                    }

                    double sliderRange = max-min;
                    int decimalPlaces = getNumberOfDecimalPlaces(tempArray);
                    if(decimalPlaces<1 && max<=1)
                        decimalPlaces = 2;
                    else
                        decimalPlaces = getNumberOfDecimalPlaces(tempArray);
                    cabbageIdentifiers.set(CabbageIDs::decimalplaces, decimalPlaces);
                    cabbageIdentifiers.set(CabbageIDs::range, sliderRange);

                }
            }
            else if(identArray[indx].equalsIgnoreCase("rangex"))
            {
                if(strTokens.size()<3)
                {
                    //debugMessage ="WARNING: Not enough paramters passed to range(): usage range(minx, max, value\")";
                }
                else
                {
                    float minX = strTokens[0].removeCharacters("()").trim().getFloatValue();
                    float maxX = strTokens[1].removeCharacters("()").trim().getFloatValue();
                    float valueX = strTokens[2].removeCharacters("()").trim().getFloatValue();

                    cabbageIdentifiers.set(CabbageIDs::minx, strTokens[0].trim().getDoubleValue());
                    cabbageIdentifiers.set(CabbageIDs::maxx, strTokens[1].getDoubleValue());
                    cabbageIdentifiers.set(CabbageIDs::valuex, strTokens[2].getDoubleValue());
                    cabbageIdentifiers.set(CabbageIDs::rangex, maxX-minX);

                    float xypadRangeX = maxX-minX;
                    cabbageIdentifiers.set(CabbageIDs::decimalplaces, strTokens[3].trim().getDoubleValue());
                }
            }
            else if(identArray[indx].equalsIgnoreCase("rangey"))
            {
                if(strTokens.size()<3)
                {
                    //debugMessage ="WARNING: Not enough paramters passed to range(): usage range(minx, max, value\")";
                }
                else
                {
                    float minY = strTokens[0].trim().getFloatValue();
                    float maxY = strTokens[1].trim().getFloatValue();
                    float valueY = strTokens[2].trim().getFloatValue();

                    cabbageIdentifiers.set(CabbageIDs::miny, strTokens[0].trim().getDoubleValue());
                    cabbageIdentifiers.set(CabbageIDs::maxy, strTokens[1].trim().getDoubleValue());
                    cabbageIdentifiers.set(CabbageIDs::valuey, strTokens[2].trim().getDoubleValue());
                    cabbageIdentifiers.set(CabbageIDs::rangey, maxY-minY);
                    cabbageIdentifiers.set(CabbageIDs::decimalplaces, strTokens[3].trim().getDoubleValue());
                }
            }
            else if(identArray[indx].equalsIgnoreCase("min"))
            {
                cabbageIdentifiers.set(CabbageIDs::minvalue, strTokens[0].trim().getFloatValue());
            }
            else if(identArray[indx].equalsIgnoreCase("midictrl"))
            {
                if(strTokens.size()<2)
                {
                    //debugMessage ="WARNING: Not enough paramters passed to midiCtrl(): usage midiCtrl(midiChan, midiCtrl\")";
                }
                else
                {
                    cabbageIdentifiers.set(CabbageIDs::midichan, strTokens[0].trim().getFloatValue());
                    cabbageIdentifiers.set(CabbageIDs::midictrl, strTokens[0].trim().getFloatValue());
                }
            }
            else if(identArray[indx].equalsIgnoreCase("max"))
            {
                cabbageIdentifiers.set(CabbageIDs::maxvalue, strTokens[0].trim().getFloatValue());
            }

            else if(identArray[indx].equalsIgnoreCase("sliderincr"))
            {
                cabbageIdentifiers.set(CabbageIDs::sliderincr, strTokens[0].trim().getFloatValue());
            }

            else if(identArray[indx].equalsIgnoreCase("sliderskew"))
            {
                cabbageIdentifiers.set(CabbageIDs::sliderskew, strTokens[0].trim().getFloatValue());
            }

            else if(identArray[indx].equalsIgnoreCase("visible"))
            {
                cabbageIdentifiers.set(CabbageIDs::visible, strTokens[0].trim().getFloatValue());
            }

            else if(identArray[indx].equalsIgnoreCase("stack"))
            {
                cabbageIdentifiers.set(CabbageIDs::stack, strTokens[0].trim().getFloatValue());
            }

            else if(identArray[indx].equalsIgnoreCase("tab"))
            {
                cabbageIdentifiers.set(CabbageIDs::tabbed, strTokens[0].trim().getFloatValue());
            }

            else if(identArray[indx].equalsIgnoreCase("latched"))
            {
                cabbageIdentifiers.set(CabbageIDs::latched, strTokens[0].trim().getFloatValue());
            }

            else if(identArray[indx].equalsIgnoreCase("alpha"))
            {
                cabbageIdentifiers.set(CabbageIDs::alpha, strTokens[0].trim().getFloatValue());
            }
			
            else if(identArray[indx].equalsIgnoreCase("radiogroup"))
            {
                cabbageIdentifiers.set(CabbageIDs::radiogroup, strTokens[0].trim().getFloatValue());
            }

            else if(identArray[indx].equalsIgnoreCase("guirefresh"))
            {
                cabbageIdentifiers.set(CabbageIDs::guirefresh, strTokens[0].trim().getFloatValue());
            }

            else if(identArray[indx].equalsIgnoreCase("textbox"))
            {
                cabbageIdentifiers.set(CabbageIDs::textbox, strTokens[0].trim().getFloatValue());
            }

            else if(identArray[indx].equalsIgnoreCase("scrubberposition"))
            {
                var scrubberInfo;
                scrubberInfo.append(strTokens[0].trim().getIntValue());
                for(int i=1; i<strTokens.size(); i++)
                {
                    scrubberInfo.append(strTokens[i].trim().getDoubleValue());
                    cabbageIdentifiers.set(CabbageIDs::scrubberposition, scrubberInfo);
                }

                cabbageIdentifiers.set(CabbageIDs::scrubberposition, scrubberInfo);

            }

            else if(identArray[indx].equalsIgnoreCase("logger"))
            {
                cabbageIdentifiers.set(CabbageIDs::logger, strTokens[0].trim().getIntValue());
            }

            else if(identArray[indx].equalsIgnoreCase("child"))
            {
                cabbageIdentifiers.set(CabbageIDs::child, strTokens[0].trim().getIntValue());
            }

            else if(identArray[indx].equalsIgnoreCase("show"))
            {
                cabbageIdentifiers.set(CabbageIDs::show, strTokens[0].trim().getIntValue());
            }

            else if(identArray[indx].equalsIgnoreCase("zoom"))
            {
                cabbageIdentifiers.set(CabbageIDs::zoom, strTokens[0].trim().getFloatValue());
                if(strTokens.size()>1)
                    cabbageIdentifiers.set(CabbageIDs::startpos, strTokens[1].trim().getFloatValue());
                else
                    cabbageIdentifiers.set(CabbageIDs::startpos, -1);
            }

            else if(identArray[indx].equalsIgnoreCase("samplerange"))
            {
                cabbageIdentifiers.set(CabbageIDs::startpos, strTokens[0].trim().getFloatValue());
                if(strTokens.size()>1)
                    cabbageIdentifiers.set(CabbageIDs::endpos, strTokens[1].trim().getFloatValue());
                else
                    cabbageIdentifiers.set(CabbageIDs::endpos, -1);
            }

            else if(identArray[indx].equalsIgnoreCase("wrap"))
            {
                cabbageIdentifiers.set(CabbageIDs::wrap, strTokens[0].trim().getFloatValue());
            }

            else if(identArray[indx].equalsIgnoreCase("readonly"))
            {
                cabbageIdentifiers.set(CabbageIDs::readonly, strTokens[0].trim().getFloatValue());
            }

            else if(identArray[indx].equalsIgnoreCase("tablenum")||
                    (identArray[indx].equalsIgnoreCase("tablenumber")||
                     identArray[indx].equalsIgnoreCase("tablenumbers")||
                     identArray[indx].equalsIgnoreCase("tablenumbs")))
            {
                int tableNum = strTokens[0].trim().getIntValue();
                var value;
                var tableConfig;
                //value.append(tableNum);
                //tableNumbers.add(tableNum);

                for(int i=0; i<strTokens.size(); i++)
                {
                    if(strTokens[i].contains(":") && strTokens.size()>0)
                    {
                        //split string into two
                        StringArray tablesData;
                        tablesData.addTokens(strTokens[i], ":", "");
                        var tables;
                        for(int w=0; w<tablesData.size(); w++)
                        {
                            tables.append(tablesData[w]);
                            value.append(tablesData[w]);
                            //Logger::writeToLog(tablesData[w]);

                        }
                        //value.append(tables);
                        tableConfig.append(tables);
                    }
                    else
                    {
                        tableNumbers.add(strTokens[i].trim().getFloatValue());
                        value.append(strTokens[i].trim().getFloatValue());
                        tableConfig.append(strTokens[i].trim().getFloatValue());
                        //Logger::writeToLog(strTokens[i].trim());
                    }
                }

                cabbageIdentifiers.set(CabbageIDs::tableconfig, tableConfig);
                cabbageIdentifiers.set(CabbageIDs::tablenumber, value);
            }
            else if(identArray[indx].equalsIgnoreCase("popup"))
            {
                cabbageIdentifiers.set(CabbageIDs::popup, strTokens[0].trim().getIntValue());
            }

            else if(identArray[indx].equalsIgnoreCase("resizemode"))
            {
                var value;
                if(strTokens.size()>0)
                    for(int i=0; i<strTokens.size(); i++)
                    {
                        //add table modes to val array
                        value.append(strTokens[i].trim().getFloatValue());
                    }
                cabbageIdentifiers.set(CabbageIDs::resizemode, value);
            }

            else if(identArray[indx].equalsIgnoreCase("amprange"))
            {
                var value;
                if(strTokens.size()>0)
                    for(int i=0; i<strTokens.size(); i++)
                    {
                        //add table modes to val array
                        value.append(strTokens[i].trim().getFloatValue());
                    }
                cabbageIdentifiers.set(CabbageIDs::amprange, value);
            }

            else if(identArray[indx].equalsIgnoreCase("outlinethickness"))
            {
                //cabbageIdentifiers.set("line", strTokens[0].trim().getFloatValue());
                cabbageIdentifiers.set(CabbageIDs::outlinethickness, strTokens[0].trim().getFloatValue());
            }

            else if(identArray[indx].equalsIgnoreCase("linethickness"))
            {
                //cabbageIdentifiers.set("line", strTokens[0].trim().getFloatValue());
                cabbageIdentifiers.set(CabbageIDs::linethickness, strTokens[0].trim().getFloatValue());
            }
			
            else if(identArray[indx].equalsIgnoreCase("trackerthickness"))
            {
                //cabbageIdentifiers.set("line", strTokens[0].trim().getFloatValue());
                cabbageIdentifiers.set(CabbageIDs::trackerthickness, strTokens[0].trim().getFloatValue());
            }

            else if(identArray[indx].equalsIgnoreCase("value"))
            {
                cabbageIdentifiers.set(CabbageIDs::value, strTokens[0].trim().getFloatValue());
            }

            else if(identArray[indx].equalsIgnoreCase("scrollbars"))
            {
                cabbageIdentifiers.set(CabbageIDs::scrollbars, strTokens[0].trim().getIntValue());
            }

            else
            {
            }
            strTokens.clear();
        }
        else
        {

        }

        //}
    }
    return 1;//must add error checking to this...

}
//=========================================================================
//retrieve numerical attributes
float CabbageGUIClass::getNumProp(Identifier prop)
{
    var props = cabbageIdentifiers.getWithDefault(prop, -9999.f);
    if(props.size()>0)
        return props[0];
    else
        return cabbageIdentifiers.getWithDefault(prop, -9999.f);
}
//==================================================================
float CabbageGUIClass::getNumProp(Identifier prop, int index)
{
    //this method should never be called...
    jassert(1);
    return 0.f;
}

//================================================================================================
// these mthods can be used to find the values of indentifiers
// by passing a single line of text to them.
//================================================================================================
Rectangle<int> CabbageGUIClass::getBoundsFromText(String text)
{
    text = " "+text;
    String subString = text.substring(text.indexOfWholeWord("bounds"));
    subString = subString.substring(7, subString.indexOf(")"));
    StringArray strTokens;
    strTokens.addTokens(subString, ",()", "");
    return Rectangle<int>(strTokens[0].getIntValue(), strTokens[1].getIntValue(), strTokens[2].getIntValue(), strTokens[3].getIntValue());
}

//===================================================================
Colour CabbageGUIClass::getColourFromText(String text)
{
    StringArray strTokens;
    strTokens.addTokens(text, ",", "");
    Colour colour;
    if(strTokens.size()<2)
    {
        if(strTokens[0].trim()=="0")
            colour = Colours::white.withAlpha(1.f);
        else
            colour = Colours::findColourForName(strTokens[0].trim(), Colours::white);

    }
    else if(strTokens.size()==4)
        colour = Colour::fromRGBA (strTokens[0].getIntValue(),
                                   strTokens[1].getIntValue(),
                                   strTokens[2].getIntValue(),
                                   strTokens[3].getIntValue());
    else if(strTokens.size()==3)
        colour = Colour::fromRGB (strTokens[0].getIntValue(),
                                  strTokens[1].getIntValue(),
                                  strTokens[2].getIntValue());

    //Logger::writeToLog(text);
    //Logger::writeToLog(colour.toString());
    return colour;
}
//==================================================================
String CabbageGUIClass::getTextFromText(String text)
{
    text = " "+text;
    String subString = text.substring(text.indexOfWholeWord("text"));
    subString = subString.substring(5, subString.indexOf(")"));
    return subString;
}
//==================================================================
var CabbageGUIClass::getVarArrayFromText(String text)
{
    text = " "+text.replace("\"", "");
    String subString = text.substring(text.indexOfWholeWord("text"));
    subString = subString.substring(5, subString.indexOf(")"));
    StringArray array;
    var varArray;
    array.addTokens(subString, ",", "");
    for(int i=0; i<array.size(); i++)
        varArray.append(array[i].trim());
    return varArray;
}
//==================================================================
Point<int> CabbageGUIClass::getPosFromText(String text)
{
    text = " "+text;
    String subString = text.substring(text.indexOfWholeWord("pos"));
    subString = subString.substring(4, subString.indexOf(")"));
    StringArray strTokens;
    strTokens.addTokens(subString, ",()", "");
    return Point<int>(strTokens[0].getIntValue(), strTokens[1].getIntValue());
}
//==================================================================
Point<int> CabbageGUIClass::getSizeFromText(String text)
{
    text = " "+text;
    String subString = text.substring(text.indexOfWholeWord("size"));
    subString = subString.substring(5, subString.indexOf(")"));
    StringArray strTokens;
    strTokens.addTokens(subString, ",()", "");
    return Point<int>(strTokens[0].getIntValue(), strTokens[1].getIntValue());
}
//==================================================================
float CabbageGUIClass::getSkewFromText(String text)
{
    text = " "+text;
    String subString = text.substring(text.indexOfWholeWord("sliderskew"));
    subString = subString.substring(11, subString.indexOf(")"));
    return subString.getFloatValue();
}

//===================================================================
//set numerical attributes
void CabbageGUIClass::setNumProp(Identifier prop, float val)
{
    cabbageIdentifiers.set(prop, val);
}
//===================================================================
float CabbageGUIClass::getTableChannelValues(int index)
{
    if(index<tableChannelValues.size())
        return tableChannelValues.getReference(index);
    else
        return 0.f;
}
//===================================================================
void CabbageGUIClass::addTableChannelValues()
{
    tableChannelValues.add(-1.f);
}
//===================================================================
void CabbageGUIClass::setTableChannelValues(int index, float val)
{
    if(index<tableChannelValues.size())
        tableChannelValues.set(index, val);
}
//===================================================================
String CabbageGUIClass::getStringProp(Identifier prop, int index)
{
    jassert(1);
    /*
    		if(prop.equalsIgnoreCase("channel"))
    			return channels[index].trim();
    		else if(prop.equalsIgnoreCase("snapshotData"))
    			return snapshotData[index].trim();
    		else*/
    return String::empty;
}
//===================================================================
Rectangle<int> CabbageGUIClass::getComponentBounds()
{
    Rectangle<int> rect;
    rect.setBounds(getNumProp("left"), getNumProp("top"), getNumProp("width"), getNumProp("height"));
    return rect;
}
//===================================================================
String CabbageGUIClass::getStringProp(Identifier prop)
{
    var strings = cabbageIdentifiers.getWithDefault(prop, "");


    if(strings.size()>0)
    {
        //Logger::writeToLog(prop+":"+strings[0].toString());
        return strings[0].toString();
    }
    else
    {
        //Logger::writeToLog(prop+":"+strings.toString());
        //Logger::writeToLog(prop+":"+strings.toString());
        return strings.toString();
    }
    return String::empty;
}
//===================================================================
StringArray CabbageGUIClass::getStringArrayProp(Identifier prop)
{
    StringArray returnArray;
    var strings = cabbageIdentifiers.getWithDefault(prop, "");
    for(int i=0; i<strings.size(); i++)
        returnArray.add(strings[i].toString());
    return returnArray;
}
//===================================================================
String CabbageGUIClass::getStringArrayPropValue(Identifier prop, int index)
{
    StringArray returnArray;
    var strings = cabbageIdentifiers.getWithDefault(prop, "");
    //Logger::writeToLog(String(strings.size()));
    for(int i=0; i<strings.size(); i++)
        returnArray.add(strings[i].toString());
    if(isPositiveAndBelow(index,strings.size()))
        return returnArray[index];
    else
        return returnArray[strings.size()-1];
}
//===================================================================
void CabbageGUIClass::setStringArrayProp(Identifier prop, var value)
{
    cabbageIdentifiers.set(prop, value);
    //cabbageIdentifiers.set(prop, value);
}
//===================================================================
void CabbageGUIClass::setStringArrayPropValue(Identifier prop, int index, String value)
{
    var strings = cabbageIdentifiers.getWithDefault(prop, "");
    if(strings.size()>0)
        if(isPositiveAndBelow(index,strings.size()))
        {
            strings.getArray()->set(index, value);
            cabbageIdentifiers.set(prop, strings);
        }

}
//===================================================================
Array<int> CabbageGUIClass::getIntArrayProp(Identifier prop)
{
    Array<int> returnArray;
    var ints = cabbageIdentifiers.getWithDefault(prop, "");
    for(int i=0; i<ints.size(); i++)
        returnArray.add(ints[i]);
    return returnArray;
}
//===================================================================
var CabbageGUIClass::getVarArrayProp(Identifier prop)
{
    return cabbageIdentifiers.getWithDefault(prop, "");
}
//===================================================================
int CabbageGUIClass::getIntArrayPropValue(Identifier prop, int index)
{
    Array<int> returnArray;
    var ints = cabbageIdentifiers.getWithDefault(prop, "");
    for(int i=0; i<ints.size(); i++)
        returnArray.add(ints[i]);
    if(isPositiveAndBelow(index,ints.size()))
        return returnArray[index];
    else
        return returnArray[ints.size()-1];

    return 0;
}
//===================================================================
Array<float> CabbageGUIClass::getFloatArrayProp(Identifier prop)
{
    Array<float> returnArray;
    var ints = cabbageIdentifiers.getWithDefault(prop, "");
    for(int i=0; i<ints.size(); i++)
        returnArray.add(ints[i]);
    return returnArray;
}
//===================================================================
float CabbageGUIClass::getFloatArrayPropValue(Identifier prop, int index)
{
    Array<float> returnArray;
    var ints = cabbageIdentifiers.getWithDefault(prop, "");
    for(int i=0; i<ints.size(); i++)
        returnArray.add(ints[i]);
    if(isPositiveAndBelow(index,ints.size()))
        return returnArray[index];
    else
        return returnArray[ints.size()-1];
}
//===================================================================
void CabbageGUIClass::setStringProp(Identifier prop, int index, String value)
{
    jassert(1)
}
//===================================================================
void CabbageGUIClass::setStringProp(Identifier prop, String val)
{
    cabbageIdentifiers.set(prop, val);
}
//===================================================================
String CabbageGUIClass::getColourProp(Identifier prop)
{
    jassert(1)
    return String::empty;
}
//===================================================================
String CabbageGUIClass::getStringForIdentifier(var propsArray, String identifier, String type)
{
    String str;
    if(type=="number")
    {
        if(propsArray.size()>0)
        {
            for(int p=0; p<propsArray.size(); p++)
                str += propsArray[p].toString()+",";
            str= identifier+str.dropLastCharacters(1)+"), ";
            //Logger::writeToLog(str);
        }
        else
            str = identifier+propsArray.toString()+"), ";

        return str;
    }

    else if(type=="string")
    {
        if(propsArray.size()>0)
        {
            for(int p=0; p<propsArray.size(); p++)
                str += "\""+propsArray[p].toString()+"\",";
            str = identifier+str.dropLastCharacters(1)+"), ";
            //Logger::writeToLog(str);
        }
        else
            str = identifier+String("\"")+propsArray.toString()+"\"), ";

        return str;
    }
    else
        return "";
}
//===================================================================
String CabbageGUIClass::getCabbageCodeFromIdentifiers(NamedValueSet props)
{
    //Logger::writeToLog("::::getCabbageCodeFromIdentifiers::::");
    String line, widgetType;
    var propsArray;
    String temp="";
    String type;
    String colour, fontcolour;
    String left("0"), top("0"), width("100"), height("50"), colR, colG, colB, colA, min("0"), max("1"), skew("1"), drawmode, tablenumber,resizemode,
           incr("0.001"), slidervalue, value, maxx, maxy, minx, miny, valuex, valuey, channel, xchannel, ychannel,
           amprange;
    var rgbVals;
    //run through the complete list of identifiers and find type
    for(int i=0; i<props.size(); i++)
		 if(props.getName(i).toString().equalsIgnoreCase(CabbageIDs::type))
			 widgetType = props.getValueAt(i).toString();
			 
	//get default values for this type of widget
	CabbageGUIClass cAttr(widgetType, -99);
	
	//run through the complete list of identifiersand create code
	for(int i=0; i<props.size(); i++)
    {	
        String identifier = props.getName(i).toString();
        propsArray = props.getValueAt(i);
        if(props.getValueAt(i).toString()!="")
        {
			Logger::writeToLog(cAttr.getStringProp(props.getName(i).toString()+":"+props.getValueAt(i).toString()));
			if(props.getValueAt(i).isString() && 
				props.getValueAt(i).toString() == cAttr.getStringProp(props.getName(i).toString()))
				{
					//Logger::writeToLog("setting default values");
				}
			
            else if(props.getName(i).toString()=="top")
                top = props.getValueAt(i).toString();
            else if(props.getName(i).toString()=="left")
                left = props.getValueAt(i).toString();
            else if(props.getName(i).toString()=="width")
                width = props.getValueAt(i).toString();
            else if(props.getName(i).toString()=="height")
                height = props.getValueAt(i).toString();
            else if(identifier=="decimalplaces" ||
                    identifier=="range" ||
                    identifier=="name" ||
                    identifier=="scalex" ||
                    identifier=="scaley" ||
                    identifier=="mode" ||
                    identifier=="basetype" ||
                    identifier=="kind" ||
                    identifier=="visible" ||
					identifier=="trackerthickness" ||
                    identifier=="comborange")
            {
                //none of these identifiers need to be seen...
            }
            else if(props.getName(i).toString()=="colour" || props.getName(i).toString()=="fontcolour"
                    || props.getName(i).toString()=="trackercolour" || props.getName(i).toString()=="textcolour" ||
                    props.getName(i).toString()=="outlinecolour")
            {
                if(propsArray.size()>0)
                {
                    for(int y=0; y<propsArray.size(); y++)
                    {
                        rgbVals.append(propsArray[y]);
                    }
                    colour = colour << props.getName(i).toString() <<"("<<rgbVals[0].toString() << ", " << rgbVals[1].toString() << ", " << rgbVals[2].toString() << ", " << rgbVals[3].toString() << "), ";
                }
                else
                {
					Colour col = Colour::fromString(props.getValueAt(i).toString());
                    colour = colour << props.getName(i).toString() << "(" << (float)col.getRed() << ", " << (float)col.getGreen() << ", " << (float)col.getBlue() << ", " << (float)col.getAlpha() << "), ";			
				}
                rgbVals.resize(0);
            }

            else if((identifier=="popup") && (int(props.getValueAt(i))==0))
            {
                //only show if not set to default
            }
            else if((identifier=="midictrl") && (int(props.getValueAt(i))==-99))
            {
                //only show if not set to default
            }
            else if((identifier=="midichan") && (int(props.getValueAt(i))==-99))
            {
                //only show if not set to default
            }
            else if((identifier=="channel"))
            {
                channel = getStringForIdentifier(propsArray, "channel(", "string");
            }

            else if(identifier=="drawmode")
            {
                drawmode = getStringForIdentifier(propsArray, "drawmode(", "number");
            }

            else if((identifier=="amprange"))
            {
                amprange = getStringForIdentifier(propsArray, "amprange(", "number");
            }

            else if((identifier=="tablenumber"))
            {
                tablenumber = getStringForIdentifier(propsArray, "tablenumber(", "number");
            }

            else if((identifier=="resizemode"))
            {
                resizemode = getStringForIdentifier(propsArray, "resizemode(", "number");
            }

            else if((identifier=="xchannel"))
            {
                xchannel = props.getValueAt(i).toString();
            }
            else if((identifier=="ychannel"))
            {
                ychannel = props.getValueAt(i).toString();
            }


            //grab slider settings..
            else if(identifier=="min")
                min = props.getValueAt(i).toString();
            else if(identifier=="max")
                max = props.getValueAt(i).toString();
            else if(identifier=="value")
            {
                slidervalue = props.getValueAt(i).toString();
                value = "value("+slidervalue+"), ";
            }
            else if(identifier=="sliderskew")
                skew = props.getValueAt(i).toString();
            else if(identifier=="sliderincr")
                incr = props.getValueAt(i).toString();

            //grab xypad x-ranges...
            else if(identifier=="minx")
                minx = props.getValueAt(i).toString();
            else if(identifier=="maxx")
                maxx = props.getValueAt(i).toString();
            else if(identifier=="valuex")
                valuex = props.getValueAt(i).toString();

            //grab xypad y-ranges...
            else if(identifier=="miny")
                miny = props.getValueAt(i).toString();
            else if(identifier=="maxy")
                maxy = props.getValueAt(i).toString();
            else if(identifier=="valuey")
                valuey = props.getValueAt(i).toString();

            else if((identifier=="textbox") && (int(props.getValueAt(i))==0))
            {
                //only show if not set to default
            }
            else
            {
                //catch all other identifiers....
                line = line + identifier+String("(");

                if(propsArray.size()>0)
                    for(int y=0; y<propsArray.size(); y++)
                    {
                        if(propsArray[y].isString())
                            temp = temp+"\""+propsArray[y].toString()+"\", ";
                        else
                            temp = temp+propsArray[y].toString()+", ";
                    }
                else if(props.getValueAt(i).isString())
                    temp = temp+"\""+props.getValueAt(i).toString()+"\"";
                else
                    temp = temp+props.getValueAt(i).toString();

                line = line + temp + "), ";
                temp = "";
            }
        }
    }

    String completeLine;
    String range = "range("+min+", "+max+", "+slidervalue+", "+skew+", "+incr+"), ";
    String rangex = "rangex("+minx+", "+maxx+", "+valuex+"), ";
    String rangey = "rangey("+miny+", "+maxy+", "+valuey+"), ";
    String xypadChannels = "channel(\""+xchannel+"\", \""+ychannel+"\"), ";
    if(widgetType.contains("slider") || widgetType=="numberbox")
        completeLine = widgetType+" bounds("+left+", "+top+", "+width+", "+height+"), "+channel+range+line.replace(", )", ")")+ " "+colour;
    else if(widgetType=="xypad")
        completeLine = widgetType+" bounds("+left+", "+top+", "+width+", "+height+"), "+xypadChannels+rangex+rangey+line.replace(", )", ")")+ " "+colour;
    else
        completeLine = widgetType+" bounds("+left+", "+top+", "+width+", "+height+"), "+channel+line.replace(", )", ")")+ " "+tablenumber+drawmode+amprange+colour+resizemode+value;
    return completeLine;

}
