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
								scaleX(1), 
								scaleY(1), 
								midiChan(-99), 
								midiCtrl(-99),
								width(0),
								height(0),
								left(0),
								top(0), 
								alpha(1),
								sliderRange(1),
								sliderIncr(.01),
								decimalPlaces(0),
								tabbed(0)
{
//Default values are assigned to all attributres 
//before parsing begins
	cabbageIdentifiers.set("scalex", 1);
	cabbageIdentifiers.set("scaley", 1);
	items.clear();
	items.add("");
	key.clear();
	min=0;
	max=1;
	sliderSkew = 1;
	value = 0;
	StringArray strTokens;
	strTokens.addTokens(compStr, " ", "\"");
	//changing case to lower to make syntax non case-sensitive

	//===============sliders==================//
	if(strTokens[0].trim() == "hslider"){
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
		cabbageIdentifiers.set(CabbageIDs::sliderskew, 1);
		cabbageIdentifiers.set(CabbageIDs::sliderincr, .0001);
		cabbageIdentifiers.set(CabbageIDs::midichan, -99);
		cabbageIdentifiers.set(CabbageIDs::midictrl, -99);
		//these don't appear in the props dialog
		cabbageIdentifiers.set(CabbageIDs::name, "hslider");
		cabbageIdentifiers.set(CabbageIDs::type, cabbageIdentifiers.getWithDefault("name", "").toString());
		cabbageIdentifiers.set(CabbageIDs::name, cabbageIdentifiers.getWithDefault("name", "").toString()+String(ID));
		cabbageIdentifiers.set(CabbageIDs::kind, "horizontal");
		cabbageIdentifiers.set(CabbageIDs::decimalplaces, 1);
		cabbageIdentifiers.set(CabbageIDs::identchannel, "");
		cabbageIdentifiers.set(CabbageIDs::visible, 1);
		

	}
    else if(strTokens[0].trim() == "vslider"){
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
		cabbageIdentifiers.set(CabbageIDs::sliderskew, 1);
		cabbageIdentifiers.set(CabbageIDs::sliderincr, .0001);
		cabbageIdentifiers.set(CabbageIDs::midichan, -99);
		cabbageIdentifiers.set(CabbageIDs::midictrl, -99);	
		//these don't appear in the props dialog
		cabbageIdentifiers.set(CabbageIDs::name, "vslider");
		cabbageIdentifiers.set(CabbageIDs::type, cabbageIdentifiers.getWithDefault("name", "").toString());
		cabbageIdentifiers.set(CabbageIDs::name, cabbageIdentifiers.getWithDefault("name", "").toString()+String(ID));
		cabbageIdentifiers.set(CabbageIDs::kind, "vertical");
		cabbageIdentifiers.set(CabbageIDs::decimalplaces, 1);	
		cabbageIdentifiers.set(CabbageIDs::identchannel, "");
		cabbageIdentifiers.set(CabbageIDs::visible, 1);

	}
    else if(strTokens[0].trim() == "rslider"){
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
		cabbageIdentifiers.set(CabbageIDs::text, "");
		cabbageIdentifiers.set(CabbageIDs::textbox, 0.f);
		cabbageIdentifiers.set(CabbageIDs::caption, "");
		cabbageIdentifiers.set(CabbageIDs::colour, Colours::whitesmoke.toString());
		cabbageIdentifiers.set(CabbageIDs::trackercolour, Colours::lime.toString());
		cabbageIdentifiers.set(CabbageIDs::fontcolour, CabbageUtils::getComponentFontColour().toString());	
		cabbageIdentifiers.set(CabbageIDs::sliderskew, 1);
		cabbageIdentifiers.set(CabbageIDs::sliderincr, .0001);
		cabbageIdentifiers.set(CabbageIDs::midichan, -99);
		cabbageIdentifiers.set(CabbageIDs::midictrl, -99);		
		//these don't appear in the props dialog
		cabbageIdentifiers.set(CabbageIDs::name, "rslider");
		cabbageIdentifiers.set(CabbageIDs::type, cabbageIdentifiers.getWithDefault("name", "").toString());
		cabbageIdentifiers.set(CabbageIDs::name, cabbageIdentifiers.getWithDefault("name", "").toString()+String(ID));
		cabbageIdentifiers.set(CabbageIDs::kind, "rotary");
		cabbageIdentifiers.set(CabbageIDs::decimalplaces, 1);			
		cabbageIdentifiers.set(CabbageIDs::identchannel, "");
		cabbageIdentifiers.set(CabbageIDs::visible, 1);
	}

    else if(strTokens[0].trim() == "source"){
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
    else if(strTokens[0].trim() == "infobutton"){
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
		cabbageIdentifiers.set(CabbageIDs::type, "infobutton");
		cabbageIdentifiers.set(CabbageIDs::name, "infobutton");
		cabbageIdentifiers.set(CabbageIDs::name, cabbageIdentifiers.getWithDefault("name", "").toString()+String(ID));
		cabbageIdentifiers.set(CabbageIDs::identchannel, "");
		cabbageIdentifiers.set(CabbageIDs::visible, 1);		
	}

	//===============directory list==================//
    else if(strTokens[0].trim() == "directorylist"){
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


    else if(strTokens[0].trim() == "patmatrix"){
	/*	  cabbageIdentifiers.set("basetype", "layout");
          top = 10;
          left = 10;
          width = 400;
          height = 240;
          channel = "";
          name = "patmatrix";
		  caption = "Pattern Matrix";
		  type = name;
		  noSteps = 8;
		  noPatterns=1;
		  rCtrls=0;
          //name.append(String(ID), 1024);
	
	 */ 
	 }

	//===============file button==================//
    else if(strTokens[0].trim() == "filebutton"){
		cabbageIdentifiers.set("basetype", "layout");
		top = 10;
		left = 10;
		width = 80;
		height = 40;
		var array;
		array.append("Open file");
		array.append("Open file");
		items.add("");
		items.add("");
		max = 1;
		min = 0;
		
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
		cabbageIdentifiers.set(CabbageIDs::type, "filebutton");
		cabbageIdentifiers.set(CabbageIDs::name, "filebutton");
		cabbageIdentifiers.set(CabbageIDs::mode, "open");		
		cabbageIdentifiers.set(CabbageIDs::identchannel, "");
		cabbageIdentifiers.set("name", cabbageIdentifiers.getWithDefault("name", "").toString()+String(ID));	
		var populate;
		populate.append("");
		populate.append("");
		cabbageIdentifiers.set("populate", populate);  
		cabbageIdentifiers.set(CabbageIDs::visible, 1);
	}

	//===============button==================//
    else if(strTokens[0].trim() == "button"){
		cabbageIdentifiers.set("basetype", "interactive");
		top = 10;
		left = 10;
		width = 80;
		height = 40;
		items.add("");
		items.add("");
		max = 1;
		min = 0;
		
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
		cabbageIdentifiers.set(CabbageIDs::value, 1);	
		cabbageIdentifiers.set(CabbageIDs::text, array);
		cabbageIdentifiers.set(CabbageIDs::caption, "");
		cabbageIdentifiers.set(CabbageIDs::colour, Colours::black.toString());	
		cabbageIdentifiers.set(CabbageIDs::fontcolour, Colours::white.toString());	
		cabbageIdentifiers.set(CabbageIDs::latched, 1);
		cabbageIdentifiers.set(CabbageIDs::type, "button");
		cabbageIdentifiers.set(CabbageIDs::name, "button");
		cabbageIdentifiers.set(CabbageIDs::name, cabbageIdentifiers.getWithDefault("name", "").toString()+String(ID));
		cabbageIdentifiers.set(CabbageIDs::identchannel, "");
		cabbageIdentifiers.set(CabbageIDs::visible, 1);
		  
	}

	//===============checkbox==================//
    else if(strTokens[0].trim() == "checkbox"){
		cabbageIdentifiers.set("basetype", "interactive");
		top = 10;
		left = 10;
		width = 100;
		height = 22;
		
		min = 0;
		max = 1;
		//default colour for toggles
		
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
		
		cabbageIdentifiers.set(CabbageIDs::colour, Colours::lime.toString());	
		cabbageIdentifiers.set(CabbageIDs::fontcolour, CabbageUtils::getComponentFontColour().toString());
 		cabbageIdentifiers.set(CabbageIDs::name, "checkbox");
		cabbageIdentifiers.set(CabbageIDs::name, cabbageIdentifiers.getWithDefault("name", "").toString()+String(ID));	
		cabbageIdentifiers.set(CabbageIDs::identchannel, "");
		cabbageIdentifiers.set(CabbageIDs::visible, 1);	  
	}
	//===============popupmenu==================//
    else if(strTokens[0].trim() == "popupmenu"){
		cabbageIdentifiers.set("basetype", "layout");
		items.clear();
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

		min=1;
		value = 1;

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
    else if(strTokens[0].trim() == "combobox"){
		cabbageIdentifiers.set("basetype", "interactive");
		items.clear();
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

		items.add("Item 1");
		items.add("Item 2");
		items.add("Item 3");
		items.add("Item 4");


		min=1;
		value = 1;

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
    else if(strTokens[0].trim() == "label"){
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
    else if(strTokens[0].trim() == "multitab"){
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
    else if(strTokens[0].trim() == "soundfiler"){
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
		cabbageIdentifiers.set(CabbageIDs::tablenumber, -1);
	}	
	//===============snapshot==================//	
    else if(strTokens[0].trim() == "snapshot"){
		cabbageIdentifiers.set("basetype", "layout");
		items.clear();
		top = 10;
		left = 10;
		width = 200;
		height = 20;
		items.add("One");
		items.add("Two");
		items.add("Three");
		items.add("Four");
		min=1;
		value=1;
		var val;
		val.append("One");
		val.append("Two");
		val.append("Three");
		val.append("Four");
		val.append("Five");

		cabbageIdentifiers.set(CabbageIDs::top, 10);
		cabbageIdentifiers.set(CabbageIDs::left, 10);
		cabbageIdentifiers.set(CabbageIDs::width, 200);
		cabbageIdentifiers.set(CabbageIDs::height, 20);	
		cabbageIdentifiers.set(CabbageIDs::text, val);
		cabbageIdentifiers.set(CabbageIDs::value, value);
		cabbageIdentifiers.set(CabbageIDs::fontcolour, Colours::white.toString());
		cabbageIdentifiers.set(CabbageIDs::colour, CabbageUtils::getComponentFontColour().toString());
		cabbageIdentifiers.set(CabbageIDs::type, "snapshot");
		cabbageIdentifiers.set(CabbageIDs::name, "snapshot");
		cabbageIdentifiers.set(CabbageIDs::name, cabbageIdentifiers.getWithDefault("name", "").toString()+String(ID));
		cabbageIdentifiers.set(CabbageIDs::identchannel, "");	
		cabbageIdentifiers.set(CabbageIDs::visible, 1);	  
	}
	//===============image==================//
    else if(strTokens[0].trim() == "image"){
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
    else if(strTokens[0].trim() == "groupbox"){
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
		cabbageIdentifiers.set(CabbageIDs::line, 1);
		cabbageIdentifiers.set(CabbageIDs::type, "groupbox");
		cabbageIdentifiers.set(CabbageIDs::name, "groupbox");
		cabbageIdentifiers.set(CabbageIDs::name, cabbageIdentifiers.getWithDefault("name", "").toString()+String(ID));		  
		cabbageIdentifiers.set(CabbageIDs::identchannel, "");  
		cabbageIdentifiers.set(CabbageIDs::visible, 1);
		
	}
	//===============line==================//
    else if(strTokens[0].trim() == "line"){
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
	//===============transport==================//
    else if(strTokens[0].trim() == "transport"){
		/*
		cabbageIdentifiers.set("basetype", "layout");
          top = 0;
          left = 0;
          width = 100;
          height = 40;
          name = "transport";
		  type = name;
          name.append(String(ID), 1024);
		   */
	}
	//===============csoundoutput==================//	
    else if(strTokens[0].trim() == "csoundoutput"){
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
		cabbageIdentifiers.set(CabbageIDs::fontcolour, Colours::white.toString());
		cabbageIdentifiers.set(CabbageIDs::name, "csoundoutput");
		cabbageIdentifiers.set(CabbageIDs::type, "csoundoutput");
		cabbageIdentifiers.set(CabbageIDs::name, cabbageIdentifiers.getWithDefault("name", "").toString()+String(ID));
		cabbageIdentifiers.set(CabbageIDs::identchannel, "");
		cabbageIdentifiers.set(CabbageIDs::visible, 1);
	}
	//===============vemeter==================//
    else if(strTokens[0].trim() == "vumeter"){
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
    else if(strTokens[0].trim() == "table"){
		cabbageIdentifiers.set("basetype", "layout");
		top = 10;
		left = 10;
		width = 400;
		height = 200;
		
		var tableColours;
		tableColours.append("white");
		tableColours.append("cornflowerblue");
		tableColours.append("yellow");
		
		cabbageIdentifiers.set(CabbageIDs::top, 10);
		cabbageIdentifiers.set(CabbageIDs::left, 10);
		cabbageIdentifiers.set(CabbageIDs::width, 400);
		cabbageIdentifiers.set(CabbageIDs::height, 200);	
		cabbageIdentifiers.set(CabbageIDs::tablenumber, 1);
		cabbageIdentifiers.set(CabbageIDs::drawmode, 0);
		cabbageIdentifiers.set(CabbageIDs::resizemode, 0);
		cabbageIdentifiers.set(CabbageIDs::readonly, 0);
		cabbageIdentifiers.set(CabbageIDs::tablecolour, tableColours);
		cabbageIdentifiers.set(CabbageIDs::amprange, 0);
		cabbageIdentifiers.set(CabbageIDs::type, "table");
		cabbageIdentifiers.set(CabbageIDs::name, cabbageIdentifiers.getWithDefault("name", "").toString()+String(ID));
		cabbageIdentifiers.set(CabbageIDs::identchannel, "");  
		cabbageIdentifiers.set(CabbageIDs::visible, 1);
	}
    else if(strTokens[0].trim() == "pvsview"){
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
    else if(strTokens[0].trim() == "xypad"){
		cabbageIdentifiers.set("basetype", "interactive");
		top = 10;
		left = 10;
		width = 200;
		height = 200;
		minX = 0;
		maxX = 200;
		minY = 0;
		maxY = 200;
		valueX = 0;
		decimalPlaces = 2;
		valueY = 0;
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
    else if(strTokens[0].trim() == "keyboard"){
		cabbageIdentifiers.set("basetype", "layout");
		top = 10;
		left = 10;
		width = 400;
		height = 100;
		value=60;
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
    else if(strTokens[0].trim() == "form"){
	    cabbageIdentifiers.set(CabbageIDs::basetype, "layout");
		cabbageIdentifiers.set(CabbageIDs::top, 10);
		cabbageIdentifiers.set(CabbageIDs::left, 10);
		cabbageIdentifiers.set(CabbageIDs::width, 600);
		cabbageIdentifiers.set(CabbageIDs::height, 300);	
		cabbageIdentifiers.set(CabbageIDs::caption, "");
		cabbageIdentifiers.set(CabbageIDs::name, "form");
		cabbageIdentifiers.set(CabbageIDs::type, "form");
		cabbageIdentifiers.set(CabbageIDs::guirefresh, 40);
		cabbageIdentifiers.set(CabbageIDs::identchannel, "");
		cabbageIdentifiers.set(CabbageIDs::visible, 1);
	}

    else if(strTokens[0].trim() == "hostbpm"){
		cabbageIdentifiers.set(CabbageIDs::basetype, "layout");
        cabbageIdentifiers.set(CabbageIDs::name, "hostbpm");
		cabbageIdentifiers.set(CabbageIDs::type, "hostbpm");
	}

    else if(strTokens[0].trim() == "hostppqpos"){
		cabbageIdentifiers.set(CabbageIDs::basetype, "layout");
        cabbageIdentifiers.set(CabbageIDs::name, "hostppqpos");
		cabbageIdentifiers.set(CabbageIDs::type, "hostppqpos");
	}

    else if(strTokens[0].trim() == "hostplaying"){
		cabbageIdentifiers.set(CabbageIDs::basetype, "layout");
        cabbageIdentifiers.set(CabbageIDs::name, "hostplaying");
		cabbageIdentifiers.set(CabbageIDs::type, "hostplaying");
	}

    else if(strTokens[0].trim() == "hostrecording"){
		cabbageIdentifiers.set(CabbageIDs::basetype, "layout");
        cabbageIdentifiers.set(CabbageIDs::name, "hostrecording");
		cabbageIdentifiers.set(CabbageIDs::type, "hostrecording");
	}

    else if(strTokens[0].trim() == "hosttime"){
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
	for(int indx=0;indx<identArray.size();indx++)
	{
		//check to see if identifier is part of input string..turn to lowercase first..
		//Logger::writeToLog("index:"+String(indx)+" arrayItem:"+identArray.getReference(indx)+" line:\n"+str);
		
		
		//shouldn't really be checking for index off, it must exist in it's full form or now!
		//OK for now, but might cause issues in the future..
		int identPos = str.toLowerCase().indexOf(identArray.getReference(indx));
          if(identPos!=-1){
			StringArray strTokens, fillStrTokens;
			fillStrTokens.addTokens(str, ", \t", "\"");
			typeOfWidget = fillStrTokens[0];
			
			String newString = str.substring(identPos+identArray.getReference(indx).length());
			
			String tstr = newString.substring(0, newString.indexOf(0, ")"));
			
			if(tstr.length()==0) return 0;
			
			strTokens.addTokens(tstr.removeCharacters("\t()\""), ",", "\"");
			
			//for(int i=0;i<strTokens.size();i++)
			//	showMessage(strTokens[i]);

			if(identArray[indx].equalsIgnoreCase("name(")){
					cabbageIdentifiers.set(CabbageIDs::name, strTokens[0].trim());
			} 
			else if(identArray[indx].equalsIgnoreCase("plant(")){
				cabbageIdentifiers.set(CabbageIDs::plant, strTokens[0].trim());
			} 
			else if(identArray[indx].equalsIgnoreCase("caption(")){
			cabbageIdentifiers.set(CabbageIDs::caption, strTokens[0].trim());
			} 
			
            else if(identArray[indx].equalsIgnoreCase(" channel(")||
				identArray[indx].equalsIgnoreCase(",channel(")||
				identArray[indx].equalsIgnoreCase(" chans(") ||
				identArray[indx].equalsIgnoreCase(" channels("))
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
						channels.clear();
						array.resize(0);					
						for(int u=0;u<strTokens.size();u++)
							{
							channels.add(strTokens[u].trim());
							array.append(strTokens[u].trim());
							}
						}
					else{ 
						if(strTokens.size()>1)
						for(int i=1;i<strTokens.size();i++)
							{
							channels.add(strTokens[i].trim());
							array.append(strTokens[i].trim());						
							}
						}
					cabbageIdentifiers.set(CabbageIDs::channel, array);	
				
				}

            else if(identArray[indx].equalsIgnoreCase(" colour(")||
				identArray[indx].equalsIgnoreCase(",colour(")||
				identArray[indx].equalsIgnoreCase("colour(")||
				identArray[indx].equalsIgnoreCase(" colours(")||
				identArray[indx].equalsIgnoreCase(", colours(")){
					cabbageIdentifiers.set(CabbageIDs::colour, getColourFromText(strTokens.joinIntoString(",")).toString());									
			}


            else if(identArray[indx].equalsIgnoreCase("fontcolour(")){
					cabbageIdentifiers.set(CabbageIDs::fontcolour, getColourFromText(strTokens.joinIntoString(",")).toString());			
			}

            else if(identArray[indx].equalsIgnoreCase("tablecolour(") ||
					identArray[indx].equalsIgnoreCase("tablecolours(")){
					var colours;
					for(int i=0;i<strTokens.size();i++)
						colours.append(strTokens[i].trim());						
					cabbageIdentifiers.set(CabbageIDs::tablecolour, colours);	
					}

            else if(identArray[indx].equalsIgnoreCase("trackercolour(")){
					cabbageIdentifiers.set(CabbageIDs::trackercolour, getColourFromText(strTokens.joinIntoString(",")).toString());				
			}

			else if(identArray[indx].equalsIgnoreCase("kind(")){
				cabbageIdentifiers.set(CabbageIDs::kind, strTokens[0].trim());	
			} 
			else if(identArray[indx].equalsIgnoreCase("file(")){
				cabbageIdentifiers.set(CabbageIDs::file, strTokens[0].trim());	
			} 
			else if(identArray[indx].equalsIgnoreCase("fillcolour(")){
					cabbageIdentifiers.set(CabbageIDs::fillcolour, getColourFromText(strTokens.joinIntoString(",")).toString());	

			}
			else if(identArray[indx].equalsIgnoreCase("shape(")){
				cabbageIdentifiers.set(CabbageIDs::shape, strTokens[0].trim());	
			} 
			else if(identArray[indx].equalsIgnoreCase("outlinecolour(")){
					cabbageIdentifiers.set(CabbageIDs::outlinecolour, getColourFromText(strTokens.joinIntoString(",")).toString());	
			}
			else if(identArray[indx].equalsIgnoreCase("textcolour(")){
				Colour textcolour;
					if(strTokens.size()<2)
						textcolour = Colours::findColourForName(strTokens[0].trim(), Colours::white);
					else if(strTokens.size()==4)
						textcolour = Colour::fromRGBA (strTokens[0].getIntValue(),
															strTokens[1].getIntValue(), 
															strTokens[2].getIntValue(),
															strTokens[3].getIntValue());
					else if(strTokens.size()==3)
						textcolour = Colour::fromRGB (strTokens[0].getIntValue(),
									strTokens[1].getIntValue(), 
									strTokens[2].getIntValue());
					cabbageIdentifiers.set(CabbageIDs::textcolour, textcolour.toString());	
			}
			
			//!!! This haven't been set for the generic data system
            else if(identArray[indx].equalsIgnoreCase("key(")){
				key.clear();
				if(strTokens.size()==1){
					key.add(strTokens[0].trim());
				}
				else if(strTokens.size()==2){
					key.add(strTokens[0].trim());
					key.add(strTokens[1].trim());
				}
			}

			else if(identArray[indx].equalsIgnoreCase("pluginid(")){
				cabbageIdentifiers.set(CabbageIDs::pluginid, strTokens[0].trim());
			}

            else if(identArray[indx].equalsIgnoreCase("items(")||
					identArray[indx].equalsIgnoreCase("text(")){
              //CabbageUtils::showMessage(typeOfWidget+":"+text);
			  var value;
			  for(int i= 0;i<(int)strTokens.size();i++)
					{
					//Logger::writeToLog(strTokens[i]);
					String test = strTokens[i]; 
					items.add(strTokens[i]);	
					value.append(strTokens[i].trim());
					//if((int)strTokens.size()<2) {
					//	items.add(strTokens[i]);
					//	value.append(strTokens[i].trim());
					//	i++;
					//	}
					comboRange = i+1;
					}
					//Logger::writeToLog(String(value.size())+typeOfWidget);
					if((value.size()==1) && ((typeOfWidget=="button") || (typeOfWidget=="filebutton"))){
						value.append(strTokens[0]);
						//Logger::writeToLog(strTokens[0].trim());
					}
						
			  cabbageIdentifiers.set(CabbageIDs::text, value);
			  cabbageIdentifiers.set(CabbageIDs::comborange, comboRange);
			}

			//!!! This haven't been set for the generic data system
			else if(identArray[indx].equalsIgnoreCase("instrs(")){
              items.clear();//clear any unwanted items
			  for(int i= 0;i<(int)strTokens.size();i++){
				String test = strTokens[i]; 
				items.add(strTokens[i]);	
				}
			}

			else if(identArray[indx].equalsIgnoreCase("preset(")){
				//CabbageUtils::showMessage("preset is no longer supported, see docs");
			}

			else if(identArray[indx].equalsIgnoreCase(" mode(") ||
					identArray[indx].equalsIgnoreCase(",mode(")){
				cabbageIdentifiers.set(CabbageIDs::mode, strTokens[0].trim());	
			}
			
			else if(identArray[indx].equalsIgnoreCase("native(")){
				cabbageIdentifiers.set("native", strTokens[0].trim());	
				//Logger::writeToLog(native);
			}
			
			else if(identArray[indx].equalsIgnoreCase("channeltype(")){
				cabbageIdentifiers.set(CabbageIDs::channeltype, strTokens[0].trim());	
			}

			else if(identArray[indx].equalsIgnoreCase("identchannel(")){
				cabbageIdentifiers.set(CabbageIDs::identchannel, strTokens[0].trim());	
			}
			
			else if(identArray[indx].equalsIgnoreCase("populate(")){
				cabbageIdentifiers.set(CabbageIDs::filetype, strTokens[0].trim());	
				if(strTokens.size()>1){
					cabbageIdentifiers.set(CabbageIDs::workingdir, strTokens[1].trim());	
				}
			}

			else if(identArray[indx].equalsIgnoreCase("author(")){
				cabbageIdentifiers.set(CabbageIDs::author, strTokens[0].trim());	
			}
			
			else if(identArray[indx].equalsIgnoreCase("align(")){
				cabbageIdentifiers.set(CabbageIDs::align, strTokens[0].trim());	
			}			

		
			else if(identArray[indx].equalsIgnoreCase("tabs(")){
				items.clear();
				var value;
				for(int i= 0;i<(int)strTokens.size();i++){
				items.add(strTokens[i]);	
				value.append(strTokens[i].trim());
				}
				cabbageIdentifiers.set(CabbageIDs::tabs, strTokens[0].trim());	
			}

			else if(identArray[indx].equalsIgnoreCase("tabpage(")){  
				cabbageIdentifiers.set(CabbageIDs::tabpage, strTokens[0].trim());	
			} 

			//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			//numeric paramters
			//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			
			else if(identArray[indx].equalsIgnoreCase("size(")){
				if(strTokens.size()>=2){
					//sizeText = identArray.getReference(indx)+tstr+")";
					width = strTokens[0].trim().getFloatValue(); 
					height = strTokens[1].trim().getFloatValue(); 
					cabbageIdentifiers.set(CabbageIDs::width, strTokens[0].trim().getFloatValue());					
					cabbageIdentifiers.set(CabbageIDs::height, strTokens[1].trim().getFloatValue());
				}
            }
			//!!! This haven't been set for the generic data system
			else if(identArray[indx].equalsIgnoreCase("scale(")){
				if(strTokens.size()<2){
					//debugMessage ="WARNING: Not enough paramters passed to scale(): usage scale(width, height\")";
				}
				else{
              scaleX = strTokens[0].trim().getFloatValue();  
			  scaleX = scaleX/width;
              scaleY = strTokens[1].trim().getFloatValue();  
			  scaleY = scaleY/height;
				}
            }
			else if(identArray[indx].equalsIgnoreCase("bounds(")){
				if(strTokens.size()<3){
					//debugMessage ="WARNING: Not enough paramters passed to bounds(): usage pos(top, left width, height\")";
				}
				else{
				  left = strTokens[0].trim().getFloatValue();  
				  top = strTokens[1].trim().getFloatValue();  
				  width = strTokens[2].trim().getFloatValue();  
				  if(strTokens.size()==3){
					  height = width;
					  cabbageIdentifiers.set(CabbageIDs::height, width);	
				  }
				  else{
				  height = strTokens[3].trim().getFloatValue(); 
				  cabbageIdentifiers.set(CabbageIDs::left, strTokens[0].trim().getFloatValue());					
				  cabbageIdentifiers.set(CabbageIDs::top, strTokens[1].trim().getFloatValue());
				  cabbageIdentifiers.set(CabbageIDs::width, strTokens[2].trim().getFloatValue());					
				  cabbageIdentifiers.set(CabbageIDs::height, strTokens[3].trim().getFloatValue());			  
				  }
				  
				}
            }
			//!!! This haven't been set for the generic data system
            else if(identArray[indx].equalsIgnoreCase("config(")){
				vuConfig.clear();
				 for(int i= 0;i<(int)strTokens.size();i++){
					 vuConfig.add(strTokens[i].trim().getFloatValue());
				 }
				}

            else if(identArray[indx].equalsIgnoreCase("pos(")){
				if(strTokens.size()<2){
					//debugMessage ="WARNING: Not enough paramters passed to pos(): usage pos(top, left\")";
				}
				else{
				  left = strTokens[0].trim().getFloatValue(); 
				  top = strTokens[1].trim().getFloatValue();
				  cabbageIdentifiers.set(CabbageIDs::left, strTokens[0].trim().getFloatValue());					
				  cabbageIdentifiers.set(CabbageIDs::top, strTokens[1].trim().getFloatValue());
				}
            }
			else if(identArray[indx].equalsIgnoreCase(" range(") ||
					identArray[indx].equalsIgnoreCase(",range(")){
				if(strTokens.size()<3){
					//debugMessage ="WARNING: Not enough paramters passed to range(): usage range(minx, max, value, incr\")";
				}
				else{	
				StringArray tempArray;
				//showMessage(str);
				//showMessage(strTokens[0]);
				min = strTokens[0].trim().getDoubleValue();// getFloatValue(); 
				max = strTokens[1].trim().getDoubleValue();//.getFloatValue(); 
				cabbageIdentifiers.set(CabbageIDs::min, strTokens[0].trim().getDoubleValue());	
				cabbageIdentifiers.set(CabbageIDs::max, strTokens[1].trim().getDoubleValue());	
				tempArray.add(strTokens[0].trim());
				tempArray.add(strTokens[1].trim());
				if(strTokens.size()>2){
				value = strTokens[2].trim().getDoubleValue();//.getFloatValue(); 
				cabbageIdentifiers.set(CabbageIDs::value, strTokens[2].trim().getDoubleValue());	
				tempArray.add(strTokens[2].trim());
				}
				else value = 0;				

				if(strTokens.size()>3){
				sliderSkew = strTokens[3].trim().getFloatValue();//.getFloatValue(); 
				cabbageIdentifiers.set(CabbageIDs::sliderskew, strTokens[3].trim().getDoubleValue());
				tempArray.add(strTokens[3].trim());
				}
				
				if(strTokens.size()>4){  
				sliderIncr = strTokens[4].trim().getDoubleValue();
				tempArray.add(strTokens[4].trim());
				cabbageIdentifiers.set(CabbageIDs::sliderincr, strTokens[4].trim().getDoubleValue());
				}
				
				sliderRange = max-min;				
				decimalPlaces = getNumberOfDecimalPlaces(tempArray);
				if(decimalPlaces<1 && max<=1)
					decimalPlaces = 2;
				else
					decimalPlaces = getNumberOfDecimalPlaces(tempArray);
				cabbageIdentifiers.set(CabbageIDs::decimalplaces, decimalPlaces);
				cabbageIdentifiers.set(CabbageIDs::range, sliderRange);

				}

			}
			else if(identArray[indx].equalsIgnoreCase("rangex("))
				{
				if(strTokens.size()<3)
					{
						//debugMessage ="WARNING: Not enough paramters passed to range(): usage range(minx, max, value\")";
					}
					else{
					minX = strTokens[0].removeCharacters("()").trim().getFloatValue();  				
					maxX = strTokens[1].removeCharacters("()").trim().getFloatValue();  
					valueX = strTokens[2].removeCharacters("()").trim().getFloatValue(); 
					
					cabbageIdentifiers.set(CabbageIDs::minx, strTokens[0].trim().getDoubleValue());			
					cabbageIdentifiers.set(CabbageIDs::maxx, strTokens[1].getDoubleValue());
					cabbageIdentifiers.set(CabbageIDs::valuex, strTokens[2].getDoubleValue());
					cabbageIdentifiers.set(CabbageIDs::rangex, maxX-minX);
					
					if(strTokens.size()==4)
					decimalPlaces = strTokens[3].trim().getFloatValue();
					xypadRangeX = maxX-minX;
					cabbageIdentifiers.set(CabbageIDs::decimalplaces, strTokens[3].trim().getDoubleValue());
					}
				}
			else if(identArray[indx].equalsIgnoreCase("rangey("))
				{
					if(strTokens.size()<3){
						//debugMessage ="WARNING: Not enough paramters passed to range(): usage range(minx, max, value\")";
					}
					else{
					minY = strTokens[0].trim().getFloatValue();  
					maxY = strTokens[1].trim().getFloatValue();  
					valueY = strTokens[2].trim().getFloatValue();				

					cabbageIdentifiers.set(CabbageIDs::miny, strTokens[0].trim().getDoubleValue());	
					cabbageIdentifiers.set(CabbageIDs::maxy, strTokens[1].trim().getDoubleValue());
					cabbageIdentifiers.set(CabbageIDs::valuey, strTokens[2].trim().getDoubleValue());
					cabbageIdentifiers.set(CabbageIDs::rangey, maxY-minY);
					

					if(strTokens.size()==4)
					decimalPlaces = strTokens[3].trim().getFloatValue();
					xypadRangeY = maxY-minY;
					cabbageIdentifiers.set(CabbageIDs::decimalplaces, strTokens[3].trim().getDoubleValue());
					}
				}
			else if(identArray[indx].equalsIgnoreCase("min("))
				{
				min = strTokens[0].removeCharacters("()").trim().getFloatValue();  
				cabbageIdentifiers.set(CabbageIDs::min, strTokens[0].trim().getFloatValue());	
				}
			else if(identArray[indx].equalsIgnoreCase("midictrl("))
				{
				if(strTokens.size()<2)
					{
					//debugMessage ="WARNING: Not enough paramters passed to midiCtrl(): usage midiCtrl(midiChan, midiCtrl\")";
					}
					else{
					midiChan = strTokens[0].trim().getFloatValue();  
					midiCtrl = strTokens[1].trim().getFloatValue();  
					cabbageIdentifiers.set(CabbageIDs::midichan, strTokens[0].trim().getFloatValue());	
					cabbageIdentifiers.set(CabbageIDs::midictrl, strTokens[0].trim().getFloatValue());	
					}
				}
            else if(identArray[indx].equalsIgnoreCase("max("))
				{
				max = strTokens[0].trim().getFloatValue();  
				cabbageIdentifiers.set(CabbageIDs::max, strTokens[0].trim().getFloatValue());	
				}

            else if(identArray[indx].equalsIgnoreCase("sliderincr("))
				{
				cabbageIdentifiers.set(CabbageIDs::sliderincr, strTokens[0].trim().getFloatValue());	
				}

            else if(identArray[indx].equalsIgnoreCase("sliderskew("))
				{
				cabbageIdentifiers.set(CabbageIDs::sliderskew, strTokens[0].trim().getFloatValue());	
				}

            else if(identArray[indx].equalsIgnoreCase("visible(")){ 
				cabbageIdentifiers.set(CabbageIDs::visible, strTokens[0].trim().getFloatValue());	
				}
			
            else if(identArray[indx].equalsIgnoreCase("tab("))
				{
				tabbed = strTokens[0].trim().getFloatValue();  
				cabbageIdentifiers.set(CabbageIDs::tabbed, strTokens[0].trim().getFloatValue());	
				}
            else if(identArray[indx].equalsIgnoreCase("latched("))
				{ 
				cabbageIdentifiers.set(CabbageIDs::latched, strTokens[0].trim().getFloatValue());	
				}
            else if(identArray[indx].equalsIgnoreCase("guirefresh("))
				{ 
				cabbageIdentifiers.set(CabbageIDs::guirefresh, strTokens[0].trim().getFloatValue());	
				}
            else if(identArray[indx].equalsIgnoreCase("textbox("))
				{
				cabbageIdentifiers.set(CabbageIDs::textbox, strTokens[0].trim().getFloatValue());				
				}
			
            else if(identArray[indx].equalsIgnoreCase("scrubberposition("))
				{
				cabbageIdentifiers.set(CabbageIDs::scrubberposition, strTokens[0].trim().getIntValue());		
				}			

            else if(identArray[indx].equalsIgnoreCase("logger("))
				{
				cabbageIdentifiers.set(CabbageIDs::logger, strTokens[0].trim().getIntValue());		
				}

            else if(identArray[indx].equalsIgnoreCase("child("))
				{
				cabbageIdentifiers.set(CabbageIDs::child, strTokens[0].trim().getIntValue());		
				}
			
            else if(identArray[indx].equalsIgnoreCase("show("))
				{
				cabbageIdentifiers.set(CabbageIDs::show, strTokens[0].trim().getIntValue());		
				}
			
            else if(identArray[indx].equalsIgnoreCase("zoom("))
				{
				cabbageIdentifiers.set(CabbageIDs::zoom, strTokens[0].trim().getFloatValue());		
				}	

            else if(identArray[indx].equalsIgnoreCase("readonly("))
				{
				cabbageIdentifiers.set(CabbageIDs::readonly, strTokens[0].trim().getFloatValue());		
				}
			
            else if(identArray[indx].equalsIgnoreCase("tablenum(")||
			(identArray[indx].equalsIgnoreCase("tablenumber("))||
			identArray[indx].equalsIgnoreCase("tablenumbers(")||
			identArray[indx].equalsIgnoreCase("tablenumbs(")){
				int tableNum = strTokens[0].trim().getIntValue();
				var value;
				value.append(tableNum);
				tableNumbers.add(tableNum);
				if(strTokens.size()>1)
					for(int i=1;i<strTokens.size();i++){
						tableNumbers.add(strTokens[i].trim().getFloatValue());
						value.append(strTokens[i].trim().getFloatValue());					
					}
				//Logger::writeToLog(value[0].toString());
				cabbageIdentifiers.set(CabbageIDs::tablenumber, value);
				//cabbageIdentifiers.set("tablenum", tableNum);
			}
            else if(identArray[indx].equalsIgnoreCase("popup(")){
				cabbageIdentifiers.set(CabbageIDs::popup, strTokens[0].trim().getIntValue());
			}

            else if(identArray[indx].equalsIgnoreCase("drawmode(")){
				var value;
				if(strTokens.size()>0)
					for(int i=0;i<strTokens.size();i++){
						//add table modes to val array
						value.append(strTokens[i].trim().getFloatValue());					
					}
				cabbageIdentifiers.set(CabbageIDs::drawmode, value);
			}

            else if(identArray[indx].equalsIgnoreCase("resizemode(")){
				var value;
				if(strTokens.size()>0)
					for(int i=0;i<strTokens.size();i++){
						//add table modes to val array
						value.append(strTokens[i].trim().getFloatValue());					
					}
				cabbageIdentifiers.set(CabbageIDs::resizemode, value);
			}			
			
            else if(identArray[indx].equalsIgnoreCase("amprange(")){
				var value;
				if(strTokens.size()>0)
					for(int i=0;i<strTokens.size();i++){
						//add table modes to val array
						value.append(strTokens[i].trim().getFloatValue());					
					}
				cabbageIdentifiers.set(CabbageIDs::amprange, value);
			}			

			else if(identArray[indx].equalsIgnoreCase(",line(")||
					identArray[indx].equalsIgnoreCase(" line(")){
					cabbageIdentifiers.set("line", strTokens[0].trim().getFloatValue());	
					cabbageIdentifiers.set(CabbageIDs::line, strTokens[0].trim().getFloatValue());  
					}
					
            else if(identArray[indx].equalsIgnoreCase("value(")){
				value = strTokens[0].trim().getFloatValue();  
				cabbageIdentifiers.set(CabbageIDs::value, strTokens[0].trim().getFloatValue());	
			} 
		
			//!!! This haven't been set for the generic data system			
			else if(identArray[indx].equalsIgnoreCase("rctrls(")){
				if(strTokens[0].trim().getFloatValue()>5)
				rCtrls = 5;
				else
				rCtrls = strTokens[0].trim().getFloatValue();  
			}
			else {
			}
			strTokens.clear();
          }
          else{
 
          }
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
		colour = Colours::findColourForName(strTokens[0].trim(), Colours::white);
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
	for(int i=0;i<array.size();i++)
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
String CabbageGUIClass::getPropsString()
{
/*	String type;
	return  << String(" bounds(") << String(left) << String(", ") << String(top) << String(", ") << String(width)
				<< String(", ") << String(height) << String("), channel(\"") << channel << String("), xyChannel(\"") << xyChannel << String("\"), value(")
				<< String(value) << String("), items(\"") << items[0].trim() << String("\", \"") << items[1].trim() << String("\")")
				<< String("), range(\"") << String(min) << String(", ") << String(max) << String(", ") << String(value) << String("\")");
*/
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
		return "";
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
	

	if(strings.size()>0){
		//Logger::writeToLog(prop+":"+strings[0].toString());
		return strings[0].toString();
	}
	else{
		//Logger::writeToLog(prop+":"+strings.toString());
		//Logger::writeToLog(prop+":"+strings.toString());
		return strings.toString();
	}
}
//===================================================================
StringArray CabbageGUIClass::getStringArrayProp(Identifier prop)
{
	StringArray returnArray;
	var strings = cabbageIdentifiers.getWithDefault(prop, "");
	for(int i=0;i<strings.size();i++)
		returnArray.add(strings[i].toString());
	return returnArray;
}
//===================================================================
String CabbageGUIClass::getStringArrayPropValue(Identifier prop, int index)
{
	StringArray returnArray;
	var strings = cabbageIdentifiers.getWithDefault(prop, "");
	for(int i=0;i<strings.size();i++)
		returnArray.add(strings[i].toString());
	if(isPositiveAndBelow(index,strings.size()))
	return returnArray[index];
	else
	return returnArray[strings.size()-1];		
}
//===================================================================
String CabbageGUIClass::setStringArrayProp(Identifier prop, var value)
{
	cabbageIdentifiers.set(prop, "");
	//cabbageIdentifiers.set(prop, value);
}
//===================================================================
String CabbageGUIClass::setStringArrayPropValue(Identifier prop, int index, String value)
{
	var strings = cabbageIdentifiers.getWithDefault(prop, "");
	if(strings.size()>0)
	if(isPositiveAndBelow(index,strings.size())){
	strings.getArray()->set(index, value);
	cabbageIdentifiers.set(prop, strings);
	}
	
}
//===================================================================
Array<int> CabbageGUIClass::getIntArrayProp(Identifier prop)
{
	Array<int> returnArray;
	var ints = cabbageIdentifiers.getWithDefault(prop, "");
	for(int i=0;i<ints.size();i++)
		returnArray.add(ints[i]);
	return returnArray;
}
//===================================================================
int CabbageGUIClass::getIntArrayPropValue(Identifier prop, int index)
{
	Array<int> returnArray;
	var ints = cabbageIdentifiers.getWithDefault(prop, "");
	for(int i=0;i<ints.size();i++)
		returnArray.add(ints[i]);
	if(isPositiveAndBelow(index,ints.size()))
	return returnArray[index];
	else
	return returnArray[ints.size()-1];
}
//===================================================================
Array<float> CabbageGUIClass::getFloatArrayProp(Identifier prop)
{
	Array<float> returnArray;
	var ints = cabbageIdentifiers.getWithDefault(prop, "");
	for(int i=0;i<ints.size();i++)
		returnArray.add(ints[i]);
	return returnArray;
}
//===================================================================
float CabbageGUIClass::getFloatArrayPropValue(Identifier prop, int index)
{
	Array<float> returnArray;
	var ints = cabbageIdentifiers.getWithDefault(prop, "");
	for(int i=0;i<ints.size();i++)
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
}
//===================================================================
String CabbageGUIClass::getStringForIdentifier(var propsArray, String identifier, String type)
{
	String str;
	if(type=="number"){
		if(propsArray.size()>0){
			for(int p=0;p<propsArray.size();p++)
			str += propsArray[p].toString()+",";						
			str= identifier+str.dropLastCharacters(1)+"), ";
			//Logger::writeToLog(str);
		}
		else
			str = identifier+propsArray.toString()+"), ";

	return str;
	}

	else if(type=="string"){
		if(propsArray.size()>0){
			for(int p=0;p<propsArray.size();p++)
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
	String line;
	var propsArray;
	String temp="";
	String type;
	String colour, fontcolour;
	String left, top, width, height, colR, colG, colB, colA, min, max, skew, drawmode, tablenumber,resizemode,
	incr, slidervalue, value, maxx, maxy, minx, miny, valuex, valuey, channel, xchannel, ychannel,
	amprange;
	var rgbVals;
	//run through the complete list of identifiers
	for(int i=0;i<props.size();i++){
		String identifier = props.getName(i).toString();
		propsArray = props.getValueAt(i);
		if(props.getValueAt(i).toString()!="")
			{
			if(props.getName(i).toString()=="top")
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
					identifier=="comborange")
			{
				//none of these identifiers need to be seen...
			}
			else if(props.getName(i).toString()=="colour" || props.getName(i).toString()=="fontcolour"
					|| props.getName(i).toString()=="trackercolour" || props.getName(i).toString()=="fill" ||
					props.getName(i).toString()=="outline")
			{
				if(propsArray.size()>0){
				for(int y=0;y<propsArray.size();y++){
					rgbVals.append(propsArray[y]);
				}			
				colour = colour << props.getName(i).toString() <<"("<<rgbVals[0].toString() << ", " << rgbVals[1].toString() << ", " << rgbVals[2].toString() << ", " << rgbVals[3].toString() << "), ";
				}
				else{
					Colour col = Colour::fromString(props.getValueAt(i).toString());
					colour = colour << props.getName(i).toString() << "(" << (float)col.getRed() << ", " << (float)col.getGreen() << ", " << (float)col.getBlue() << ", " << (float)col.getAlpha() << "), ";
				}
				rgbVals.resize(0);
			}

			else if((identifier=="popup") && (int(props.getValueAt(i))==0)){
			//only show if not set to default		
			}
			else if((identifier=="midictrl") && (int(props.getValueAt(i))==-99)){
			//only show if not set to default				
			}
			else if((identifier=="midichan") && (int(props.getValueAt(i))==-99)){
			//only show if not set to default				
			}
			else if((identifier=="channel")){
				channel = getStringForIdentifier(propsArray, "channel(", "string");
			}
			
			else if(identifier=="drawmode"){
				drawmode = getStringForIdentifier(propsArray, "drawmode(", "number");
			}

			else if((identifier=="amprange")){
				amprange = getStringForIdentifier(propsArray, "amprange(", "number");
			}
		
			else if((identifier=="tablenumber")){
				tablenumber = getStringForIdentifier(propsArray, "tablenumber(", "number");
			}					
			
			else if((identifier=="resizemode")){
				resizemode = getStringForIdentifier(propsArray, "resizemode(", "number");
			}	
			
			else if((identifier=="xchannel")){
			xchannel = props.getValueAt(i).toString();		
			}
			else if((identifier=="ychannel")){
			ychannel = props.getValueAt(i).toString();		
			}					
			
			
			//grab slider settings..
			else if(identifier=="min")
					min = props.getValueAt(i).toString();
			else if(identifier=="max")
					max = props.getValueAt(i).toString();
			else if(identifier=="value"){
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

			else if((identifier=="textbox") && (int(props.getValueAt(i))==0)){
			//only show if not set to default				
			}
			else if(identifier=="type")
				type = props.getValueAt(i).toString();
			else{
				//catch all other identifiers....
				line = line + identifier+String("(");
				
				if(propsArray.size()>0)
				for(int y=0;y<propsArray.size();y++){
					if(propsArray[y].isString())
					temp = temp+"\""+propsArray[y].toString()+"\", ";
					else
					temp = temp+propsArray[y].toString()+", ";
					}
				else	
						if(props.getValueAt(i).isString())							
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
	if((type == "rslider") || (type=="hslider") || (type=="vslider"))
	completeLine = type+" bounds("+left+", "+top+", "+width+", "+height+"), "+channel+range+line.replace(", )", ")")+ " "+colour;
	else if(type=="xypad")
	completeLine = type+" bounds("+left+", "+top+", "+width+", "+height+"), "+xypadChannels+rangex+rangey+line.replace(", )", ")")+ " "+colour;
	else
	completeLine = type+" bounds("+left+", "+top+", "+width+", "+height+"), "+channel+line.replace(", )", ")")+ " "+tablenumber+drawmode+amprange+colour+resizemode+value;
	return completeLine;
	
}