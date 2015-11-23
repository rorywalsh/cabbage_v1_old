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

#ifndef __CabbUtilities_h__
#define __CabbUtilities_h__

#include <string>
#include <time.h>

#include "../JuceLibraryCode/JuceHeader.h"

#ifndef Cabbage_Plugin_Host
#include "BinaryData.h"
#endif

#include <assert.h>

using namespace std;

#define svgRSliderDiameter 1000

#define svgVSliderWidth 300
#define svgVSliderHeight 1000

#define svgVSliderThumbHeight 1000
#define svgVSliderThumbWidth 1000

#define svgHSliderThumbWidth 1000
#define svgHSliderThumbHeight 1000

#define svgHSliderHeight 300
#define svgHSliderWidth 1000

#define svgButtonWidth 1000
#define svgButtonHeight 500

#define svgGroupboxWidth 1000
#define svgGroupboxHeight 800

#define OK 0

class KeyboardShortcutKeys
{
public:
    KeyboardShortcutKeys(XmlElement* xml):xmlData(xml)
    {

    }

    ~KeyboardShortcutKeys() {}

    String getKeyPress(String name)
    {
        for(int i=0; xmlData->getNumAttributes(); i++)
        {
            if(xmlData->getAttributeName(i)==name)
                return xmlData->getAttributeValue(i);
        }
    }

    int keyCode;
    ModifierKeys mods;
    ScopedPointer<XmlElement> xmlData;

};

class CabbageTimer : public Timer,
    public ActionBroadcaster
{
public:
    CabbageTimer(): time(0), go(false)
    {
    };
    ~CabbageTimer() {};

    void timerCallback()
    {
        time++;
        if(time>seconds)
        {
            sendActionMessage(event);
            stopTimer();
        }
    }

    bool startTimedEvent(int _seconds, String _event)
    {
        time=0;
        seconds = _seconds,
        event =_event;
        startTimer(200);
        return true;
    }



    bool go;
    int time;
    int seconds;
    String event;
};


//simple component class for popup displays
class PopupText : public Component,
    public Timer
{
public:
    PopupText():timerCount(0) {}
    ~PopupText() {}

    void setText(String input)
    {
        Font font = Font ("Verdana", 11.5, 1);
        textWidth = font.getStringWidth(input);
        setSize(textWidth, 15);
        text = input;
        timerCount = 0;
        startTimer(20);
    }

    void timerCallback()
    {
        timerCount++;
        if(timerCount>30)
        {
            stopTimer();
            setVisible(false);
        }
    }

    void paint(Graphics &g)
    {
        g.fillAll(Colours::whitesmoke);
        g.setColour(Colours::black);

        g.drawFittedText (text,
                          0, 0, textWidth, getHeight(),
                          Justification::centredLeft, 1);
    }

private:
    float textWidth;
    int timerCount;
    String text;

};

//===========================================================================================
//some utility functions used across classes...
//===========================================================================================
class cUtils
{
public:
    cUtils() {};
    ~cUtils() {};


//===========================================================================================
    string juce2Str(juce::String inStr)
    {
        string str(inStr.toUTF8());
        return str;
    }

//===========================================================================================
    String str2Juce(std::string inStr)
    {
        String str(inStr.c_str());
        return str;
    }

    float cabbageABS(float in)
    {
        if(in<0)
            return in*-1;
        else return in;
    }
//
    static void debug(String message)
    {
#ifdef DEBUG
        Logger::writeToLog(message);
#endif
    }

    static void debug(float value)
    {
#ifdef DEBUG
        Logger::writeToLog(String(value));
#endif
    }

    static void debug(String message, double value)
    {
#ifdef DEBUG
        Logger::writeToLog(message+":"+String(value));
#endif
    }

    static void debug(float val, String value)
    {
#ifdef DEBUG
        Logger::writeToLog(String(val)+":"+value);
#endif
    }

    static void debug(float val, float value)
    {
#ifdef DEBUG
        Logger::writeToLog(String(val)+":"+String(value));
#endif
    }

    static void debug(String message, String value)
    {
#ifdef DEBUG
        Logger::writeToLog(message+":"+value);
#endif
    }
//===========================================================================================
    static void showMessage(String message)
    {
        AlertWindow::showMessageBoxAsync (AlertWindow::WarningIcon,
                                          "Cabbage Message",
                                          message,
                                          "Ok");

    }

    static void showMessage(String title, String message)
    {
        AlertWindow::showMessageBoxAsync (AlertWindow::WarningIcon,
                                          title,
                                          message,
                                          "Ok");

    }

    static bool compDouble(double x, int y)
    {
        if( x > (double)y-0.0001 && x < (double)y+0.0001)
        {
            // They are almost equal
            return true;
        }
        else
        {
            // They aren't equal at all
            return false;
        }
    }

    static double roundToMultiple(double x, double multiple)
    {
        return round(x / multiple) * multiple;
    }

    static double roundToPrec(double x, int prec)
    {
        double power = 1.0;
        int i;

        if (prec > 0)
            for (i = 0; i < prec; i++)
                power *= 10.0;
        else if (prec < 0)
            for (i = 0; i < prec; i++)
                power /= 10.0;

        if (x > 0)
            x = floor(x * power + 0.5) / power;
        else if (x < 0)
            x = ceil(x * power - 0.5) / power;

        if (x == -0)
            x = 0;

        return x;
    }
//===========================================================================================
    static const Colour getRandomColour()
    {
        const Colour colour = Colour(Random::getSystemRandom().nextInt(255),
                                     Random::getSystemRandom().nextInt(255),
                                     Random::getSystemRandom().nextInt(255));
        return colour;
    }

//===========================================================================================
    static void showMessageWithLocation(String message)
    {
        File thisFile(File::getSpecialLocation(File::currentApplicationFile));
        AlertWindow alert(thisFile.getFullPathName() , message, AlertWindow::WarningIcon);
        alert.showMessageBoxAsync(AlertWindow::WarningIcon, thisFile.getFullPathName() , message, "Ok");
    }
//===========================================================================================
    static void showMessage(double num)
    {
        String str(num);
        File thisFile(File::getSpecialLocation(File::currentApplicationFile));
        AlertWindow alert(thisFile.getFullPathName(), str, AlertWindow::WarningIcon);
        alert.showMessageBoxAsync(AlertWindow::WarningIcon, thisFile.getFullPathName(), str, "Ok");
    }
//===========================================================================================
    static void showMessage(String title, String message, LookAndFeel* feel, Component* mainWindow)
    {
        if(title.length()<1)title="Cabbage Message";
        mainWindow->setAlwaysOnTop(false);
//	mainWindow->toBack();
        AlertWindow alert(title, message, AlertWindow::WarningIcon);
        alert.setLookAndFeel(feel);
        alert.setAlwaysOnTop(true);
        alert.addButton("Ok", 1);
#if !defined(AndroidBuild)
        alert.runModalLoop();
#else
        alert.showMessageBoxAsync(AlertWindow::WarningIcon, "Cabbage Message" , message, "Ok");
#endif
        mainWindow->setAlwaysOnTop(true);
    }

//===========================================================================================
    static void showMessage(String message, LookAndFeel* feel)
    {
        AlertWindow alert("Cabbage Message" , message, AlertWindow::WarningIcon);
        alert.setLookAndFeel(feel);
        alert.addButton("Ok", 1);
#if !defined(AndroidBuild)
        alert.runModalLoop();
#else
        alert.showMessageBoxAsync(AlertWindow::WarningIcon, "Cabbage Message" , message, "Ok");
#endif
    }

    static void showMessage(String title, String message, LookAndFeel* feel)
    {
        AlertWindow alert(title, message, AlertWindow::WarningIcon);
        alert.setLookAndFeel(feel);
        //alert.showMessageBoxAsync(AlertWindow::WarningIcon, "Cabbage Message" , message, "Ok");
        alert.addButton("Ok", 1);
#if !defined(AndroidBuild)
        alert.runModalLoop();
#else
        alert.showMessageBoxAsync(AlertWindow::WarningIcon, "Cabbage Message" , message, "Ok");
#endif
    }

//===========================================================================================
    static int showYesNoMessage(String message, LookAndFeel* feel, int cancel=0)
    {
        AlertWindow alert("Cabbage Message", message, AlertWindow::WarningIcon, 0);
        alert.setLookAndFeel(feel);
        alert.addButton("Yes", 0);
        alert.addButton("No", 1);
        if(cancel==1)
            alert.addButton("Cancel", 2);
#if !defined(AndroidBuild)
        int result = alert.runModalLoop();
#else
        int result = alert.showYesNoCancelBox(AlertWindow::QuestionIcon, "Warning", message, "Yes", "No", "Cancel", nullptr, nullptr);
#endif
        return result;
    }
//===========================================================================================
    StringArray CreateStringArray(std::string str)
    {
        StringArray strArray;
        //std::string str = sourceEditorComponent->textEditor->getText().toUTF8();
        int cnt=0;
        for(int i=0; i<(int)str.length(); i++)
        {
            long pos = (int)str.find("\n", 0);
            if(pos!=std::string::npos)
            {
                strArray.add(str2Juce(str.substr(0, pos+1)));
                str.erase(0, pos+1);
                cnt++;
            }
        }
        //add the very last line which won't have a "\n"...
        strArray.add(str2Juce(str.substr(0, 100)));
        return strArray;
    }
//==========================================================================================
    static Array<File> launchFileBrowser(String title, WildcardFileFilter filter, String fileType, int mode, File initialDir, bool useNative, LookAndFeel *look)
    {
        const bool warnAboutOverwrite = true;
        Array<File> results;
#if !defined(AndroidBuild)
        //if set to open or browse for files
        if(mode==1)
        {
#ifndef Cabbage_Build_Standalone
            //in plugin mode it's best to use Cabbage file browser instead of a system one
            useNative=false;
#endif

            if(useNative==false)
            {
                FileBrowserComponent browserComponent ( FileBrowserComponent::openMode|
                                                        FileBrowserComponent::canSelectFiles,
                                                        initialDir,
                                                        &filter,
                                                        nullptr);

                FileChooserDialogBox box (title, String::empty,
                                          browserComponent, warnAboutOverwrite,
                                          cUtils::getDarkerBackgroundSkin());

                box.setLookAndFeel(look);

                if (box.show())
                {
                    for (int i = 0; i < browserComponent.getNumSelectedFiles(); ++i)
                        results.add (browserComponent.getSelectedFile (i));
                }
            }
            else
            {
                FileChooser openFC(title, initialDir, fileType, true);
                if(openFC.browseForFileToOpen())
                    results = openFC.getResults();
            }
        }

        //set to save files
        if(mode==0)
        {
            if(useNative==false)
            {
                FileBrowserComponent browserComponent ( FileBrowserComponent::saveMode|
                                                        FileBrowserComponent::canSelectFiles,
                                                        initialDir,
                                                        &filter,
                                                        nullptr);

                FileChooserDialogBox box (title, String::empty,
                                          browserComponent, warnAboutOverwrite,
                                          cUtils::getDarkerBackgroundSkin());

                box.setLookAndFeel(look);

                if (box.show())
                {
                    for (int i = 0; i < browserComponent.getNumSelectedFiles(); ++i)
                        results.add (browserComponent.getSelectedFile (i));
                }
            }
            else
            {
                FileChooser saveFC(String("Save as..."), File::nonexistent, String(""), true);
                if(saveFC.browseForFileToSave(true))
                    results = saveFC.getResults();
            }
        }

        //browse for directory
        if(mode==2)
        {
            if(useNative==false)
            {
                FileBrowserComponent browserComponent ( FileBrowserComponent::openMode,
                                                        initialDir,
                                                        &filter,
                                                        nullptr);

                FileChooserDialogBox box (title, String::empty,
                                          browserComponent, warnAboutOverwrite,
                                          cUtils::getDarkerBackgroundSkin());

                box.setLookAndFeel(look);

                if (box.show())
                {
                    for (int i = 0; i < browserComponent.getNumSelectedFiles(); ++i)
                        results.add (browserComponent.getSelectedFile (i));
                }
            }
            else
            {
                FileChooser fc(title, File::nonexistent, String(""), true);
                if(fc.browseForDirectory())
                    results.add(fc.getResult());
            }
        }
#endif
        return results;
    }
//==========================================================================================
    static String cabbageString (String input, Font font, float availableWidth)
    {
        //This method returns a truncated string if it is too big to fit into its available
        //space.  Dots are added at the end to signify that it has been truncated.

        String newStr;
        float stringWidth = font.getStringWidthFloat (input);
        int numChars = input.length();
        float charWidth = stringWidth / numChars;

        if (stringWidth > availableWidth)
        {
            int numCharsToInclude = ((availableWidth / charWidth)+0.5) - 2;
            newStr = input.substring (0, numCharsToInclude);
            newStr << "..";
            return newStr;
        }
        else
            return input;
    }


    static int getNchnlsFromFile(String csdText)
    {
        StringArray array;
        array.addLines(csdText);
        //cUtils::showMessage(file.loadFileAsString());

        for(int i=0; i<array.size(); i++)
        {
            //cUtils::debug(array.joinIntoString(" "));
            cUtils::debug(array[i]);
            if(array[i].contains("nchnls") && array[i].contains("="))
            {
                String channels = array[i].substring(array[i].indexOf("=")+1, (array[i].contains(";") ? array[i].indexOf(";") : 100));
                return channels.trim().getIntValue();
            }
        }
        return 2;
    }

    static float getKrFromFile(String csdFile, float sr)
    {
        StringArray array;
        array.addLines(File(csdFile).loadFileAsString());

        for(int i=0; i<array.size(); i++)
        {
            //cUtils::debug(array.joinIntoString(" "));
            if(array[i].contains("kr") && array[i].contains("="))
            {
                String kr = array[i].substring(array[i].indexOf("=")+1, (array[i].contains(";") ? array[i].indexOf(";") : 100));
                return kr.trim().getIntValue();
            }
        }

        for(int i=0; i<array.size(); i++)
        {
            //cUtils::debug(array[i]);
            if(array[i].contains("ksmps") && array[i].contains("="))
            {
                String ksmps = array[i].substring(array[i].indexOf("=")+1, (array[i].contains(";") ? array[i].indexOf(";") : 100));
                return sr/(ksmps.trim().getDoubleValue());
            }
        }

        return sr/64.f;
    }

    static int getNumberOfDecimalPlaces(StringArray array)
    {
        int longest=0;
        int index = 0;
        for(int i=0; i<array.size(); i++)
        {
            if(array[i].length()>longest)
            {
                longest = array[i].length();
                index = i;
            }
        }

        if(array[index].indexOf(".")>=0)
        {
            String subTemp = array[index].substring(array[index].indexOf("."), 10);
            return subTemp.length()-1;
        }
        else return 0;


    }

//========= Normal font for components ===============================================
    static Font getComponentFont()
    {
        Font font = Font ("Verdana", 11.5, 1);
        return font;
    }

//======= For slider values etc ======================================================
    static Font getValueFont()
    {
        Font font = Font ("Helvetica", 11.5, 1);
        return font;
    }
//======= get minMax numerical value from stringarray ================================
    static Range<float> getMinMax(StringArray array)
    {
        if(array.size()==0)
            jassert(1);
        float min = array[0].getFloatValue();
        float max = array[0].getFloatValue();
        for(int i=1; i<array.size(); i++)
        {
            if(array[i].getFloatValue()>max)
                max = array[i].getFloatValue();
            if(array[i].getFloatValue()<min)
                min = array[i].getFloatValue();
        }

        return Range<float>(min, max);

    }
//======= For spectrograms and tables etc ============================================
    static Font getSmallerValueFont()
    {
        Font font = Font ("Helvetica", 10, 0);
        return font;
    }

//======= For titles etc ============================================================
    static Font getTitleFont()
    {
        Font font = Font ("Helvetica", 12.5, 1);
        return font;
    }

//======= Title font colour =========================================================
    static Colour getTitleFontColour()
    {
        Colour cl = Colour::fromRGBA (160, 160, 160, 255);
        return cl;
    }

//====== Font Colour ================================================================
    static Colour getComponentFontColour()
    {
        Colour cl = Colour::fromRGBA (160, 160, 160, 255);
        return cl;
    }

//====== for group components etc =================================================
    static Colour getComponentSkin()
    {
        //Colour skin = Colour::fromRGBA (45, 55, 60, 255);
        Colour skin = Colour::fromRGBA (75, 85, 90, 100); //some transparency
        return skin;
    }

//====== border colour ============================================================
    static const Colour getBorderColour()
    {
        return getComponentFontColour().withMultipliedAlpha(0.2);
    }

//====== border width ============================================================
    static const float getBorderWidth()
    {
        return 1.0f;
    }

//======== for the main background =========================================================
    static Colour getBackgroundSkin()
    {
        Colour skin = Colour::fromRGBA (5, 15, 20, 255);
        return skin;
    }

//======= method for retrieve the string values of rectangles..
    static	String getBoundsString(juce::Rectangle<int> currentBounds)
    {
        return "bounds(" + String(currentBounds.getX()) + String(", ") + String(currentBounds.getY()) + String(", ") + String(currentBounds.getWidth()) + String(", ")
               + String(currentBounds.getHeight()) + String(")");
    }


//======= method for replacing the contents of an identifier with new values..
    static String replaceIdentifier(String line, String identifier, String updatedIdentifier)
    {
        if(identifier.length()<2)
            return line;
        if(updatedIdentifier.length()<2)
            return line;

        int startPos = line.indexOf(identifier);
        if(startPos==-1)
            return "";

        String firstSection = line.substring(0, line.indexOf(identifier));
        line = line.substring(line.indexOf(identifier));
        String secondSection = line.substring(line.indexOf(")")+1);

        return firstSection+updatedIdentifier+secondSection;
    }

//======= for darker backgrounds ==========================================================
    static Colour getDarkerBackgroundSkin()
    {
        Colour skin = getBackgroundSkin().darker(0.4);
        return skin;
    }

//==========================================================================================
    void cabbageSleep(unsigned int mseconds)
    {
        clock_t goal = mseconds + clock();
        while (goal > clock());
    }

//==========================================================================================
    Array<float> getAmpRangeArray(Array<float> ranges, int tableNumber)
    {
        Array<float> ampRange;

        for(int i=2; i<ranges.size(); i+=4)
            if(ranges[i]==tableNumber || ranges[i]==-1)
            {
                for(int y = i-2; y<=i+1; y++)
                {
                    ampRange.add(ranges[y]);
                    cUtils::debug(ranges[y]);
                }
            }

        return ampRange;

    }

//======== Check if NaN ====================================================================
    static bool isNumber(double x)
    {
        // This looks like it should always be true,
        // but it's false if x is a NaN.
        return (x == x);
    }

//==========================================================================================
    static void addCustomPlantsToMenu (PopupMenu& m, Array<File> &plantFiles, String userDir)
    {
        int menuSize = m.getNumItems();

        PopupMenu menu;
        PopupMenu subMenu;
        int fileCnt=0;

        if(File(userDir).exists())
        {
            FileSearchPath filePaths(userDir);
            //cUtils::showMessage(appProperties->getUserSettings()->getValue("CabbageFilePaths"))

            //add all files in root of specifed directories
            for(int i=0; i<filePaths.getNumPaths(); i++)
            {
                File plantDir(filePaths[i]);
                plantDir.findChildFiles(plantFiles, File::findFiles, false, "*.plant");

            }

            for (int i = 0; i < plantFiles.size(); ++i)
                m.addItem (i+100, plantFiles[i].getFileNameWithoutExtension());


            //fileCnt = cabbageFiles.size();

            //increment menu size and serach recursively through all subfolders in specified dirs
            for(int i=0; i<filePaths.getNumPaths(); i++)
            {
                Array<File> subFolders;
                File searchDir(filePaths[i]);
                subFolders.add(searchDir);
                searchDir.findChildFiles(subFolders, File::findDirectories, true);

                //remove parent dirs from array
                for(int p=0; p<filePaths.getNumPaths(); p++)
                    subFolders.removeAllInstancesOf(filePaths[p]);

                PopupMenu subMenu;
                for (int subs = 0; subs < subFolders.size(); subs++)
                {
                    cUtils::debug(subFolders[subs].getFullPathName());
                    fileCnt = plantFiles.size();
                    subFolders[subs].findChildFiles(plantFiles, File::findFiles, false, "*.plants");
                    subMenu.clear();

                    for (int fileIndex=fileCnt+1; fileIndex < plantFiles.size(); fileIndex++)
                        subMenu.addItem (fileIndex+100, plantFiles[fileIndex].getFileNameWithoutExtension());


                    m.addSubMenu(subFolders[subs].getFileNameWithoutExtension(), subMenu);
                }

                subMenu.clear();
            }


            m.addSeparator();
            //m.setLookAndFeel(&this->getLookAndFeel());
        }
    }


    static void addFilesToPopupMenu(PopupMenu &m, Array<File> &filesArray, String dir, String ext, int indexOffset)
    {
        File searchDir(dir);
        Array<File> subFolders;
        Array<File> files;
        subFolders.add(searchDir);
        int noOfFiles=0, fileCnt;
        searchDir.findChildFiles(subFolders, File::findDirectories, true);
        String pathSlash;
#if defined(LINUX) || defined(MACOSX)
        pathSlash = "/";
        dir = dir+ "/";
#else
        pathSlash = "\\";
        dir = dir+ "\\";
#endif

        PopupMenu subMenu;
//grab all file in sub-folders
        for (int i = 1; i < subFolders.size(); i++)
        {
            if(!subFolders[i].containsSubDirectories())
            {
                subFolders[i].findChildFiles(filesArray, File::findFiles, false, ext);

                subMenu.clear();
                for (fileCnt = noOfFiles; fileCnt < filesArray.size(); fileCnt++)
                {
                    subMenu.addItem (fileCnt + indexOffset, filesArray[fileCnt].getFileNameWithoutExtension());
                }
                noOfFiles = fileCnt;
                if(noOfFiles>0)
                    m.addSubMenu(subFolders[i].getFullPathName().replace(dir, "").replace(pathSlash, "-"), subMenu);
            }
        }
        subMenu.clear();
    }

//======================================================================================
    static String setDecimalPlaces(const double& x, const int& numDecimals)
    {
        int y=x;
        double z=x-y;
        double m=pow((double)10.f, (double)numDecimals);
        double q=z*m;
        double r=roundToIntAccurate(q);

        return String(static_cast<double>(y)+(1.0/m)*r);
    }

//======================================================================================
    static int getPreference(ApplicationProperties* appPrefs, String pref)
    {
        return appPrefs->getUserSettings()->getValue(pref, var(0)).getFloatValue();
    }

    static String getPreference(ApplicationProperties* appPrefs, String pref, String value)
    {
        return appPrefs->getUserSettings()->getValue(pref, "");
    }

//================================================================================
    static void toggleOnOffPreference(ApplicationProperties* appPrefs, String pref)
    {
        if(getPreference(appPrefs, pref)==0)
            appPrefs->getUserSettings()->setValue(pref, var(1));
        else
            appPrefs->getUserSettings()->setValue(pref, var(0));
    }
//====================================================================================================
    static void setPreference(ApplicationProperties* appPrefs, String pref, int value)
    {
        appPrefs->getUserSettings()->setValue(pref, var(value));
    }

//====================================================================================
    static Image getSVGImageFor(String path, String type, AffineTransform affine)
    {

        String svgFileName;
        Image svgImg;
        if(type.contains("button"))
        {
            svgFileName = path+"/"+String(type)+".svg";
            if(File(svgFileName).existsAsFile())
                svgImg = Image(Image::ARGB, svgButtonWidth, svgButtonHeight, true);//default button size 100px X 50px
        }

        else if(type.contains("slider"))
        {
            svgFileName = path+"/"+String(type)+".svg";
            if(File(svgFileName).existsAsFile())
                svgImg = Image(Image::ARGB, svgRSliderDiameter, svgRSliderDiameter, true);//default rotary slider size 150px X 150px
        }

        else if(type.contains("groupbox"))
        {
            svgFileName = path+"/"+String(type)+".svg";
            if(File(svgFileName).existsAsFile())
                svgImg = Image(Image::ARGB, svgGroupboxWidth, svgGroupboxHeight, true);//default rotary slider size 150px X 150px
        }


        File svgFile(svgFileName);
        ScopedPointer<XmlElement> svg (XmlDocument::parse(svgFile.loadFileAsString()));
        if (svgFile.exists())
        {
            if(svg == nullptr)
                Logger::writeToLog("couldn't parse svg, might not exist");
            ScopedPointer<Drawable> drawable;

            Graphics graph(svgImg);
            if (svg != nullptr)
            {
                drawable = Drawable::createFromSVG (*svg);
                drawable->draw(graph, 1.f, affine);
                return svgImg;
            }
        }

        return svgImg;
    }

//========= Text button image ========================================================
    static Image drawTextButtonImage(float width, float height, bool isButtonDown, Colour colour, String svgPath)
    {
        Image img = Image(Image::ARGB, width, height, true);
        Graphics g (img);
        float opacity;

        //if alpha is full draw invible button
        if(colour.getAlpha()==0x00)
        {
            g.fillAll(Colours::transparentBlack);
            return img;
        }


        if(getSVGImageFor(svgPath, "button_background", AffineTransform::identity).isValid())
        {
            //----- If "off"
            if (isButtonDown == false)
                g.drawImage(getSVGImageFor(svgPath, "button_background", AffineTransform::identity), 0, 0, width, height, 0, 0, svgButtonWidth, svgButtonHeight, false);
            else
                g.drawImage(getSVGImageFor(svgPath, "button_background", AffineTransform::identity), 1, 1, width-2, height-2, 0, 0, svgButtonWidth, svgButtonHeight, false);
        }
        else
        {

            //----- Outline
            g.setColour (Colour::fromRGBA (10, 10, 10, 255));
            g.fillRoundedRectangle (0, 0, width*0.95, height*0.95, height*0.1);

            //----- If "off"
            if (isButtonDown == false)
            {
                //----- Shadow
                for (float i=0.01; i<0.05; i+=0.01)
                {
                    g.setColour (Colour::fromRGBA (0, 0, 0, 255/(i*100)));
                    g.fillRoundedRectangle (width*i, height*i,
                                            width*0.95, height*0.95, height*0.1);
                    opacity = 0.3;
                }
            }
            else
                opacity = 0.1;

            //----- Filling in the button
            //Colour bg1 = Colour::fromRGBA (25, 25, 28, 255);
            //Colour bg2 = Colour::fromRGBA (15, 15, 18, 255);
            Colour bg1 = colour;
            Colour bg2 = colour.darker();

            ColourGradient cg = ColourGradient (bg1, 0, 0, bg2, width*0.5, height*0.5, false);
            g.setGradientFill (cg);
            g.fillRoundedRectangle (width*0.01, height*0.01, width*0.93, height*0.93, height*0.1);

            //----- For emphasising the top and left edges to give the illusion that light is shining on them
            ColourGradient edgeHighlight = ColourGradient (Colours::whitesmoke, 0, 0,
                                           Colours::transparentWhite, 0, height*0.1, false);
            g.setGradientFill (edgeHighlight);
            g.setOpacity (opacity);
            g.fillRoundedRectangle (0, 0, width*0.95, height*0.95, height*0.1);

            ColourGradient edgeHighlight2 = ColourGradient (Colours::whitesmoke, 0, 0,
                                            Colours::transparentWhite, height*0.1, 0, false);
            g.setGradientFill (edgeHighlight2);
            g.setOpacity (opacity);
            g.fillRoundedRectangle (0, 0, width*0.95, height*0.95, height*0.1);
        }
        return img;
    }

//====================================================================================================
    static Image drawToggleImage (float width, float height, bool isToggleOn, Colour colour, bool isRect, String svgPath)
    {
        Image img = Image(Image::ARGB, width, height, true);
        Graphics g (img);
        float opacity = 0;

        //if alpha is full draw invible button
        if(colour.getAlpha()==0x00)
        {
            g.fillAll(Colours::transparentBlack);
            return img;
        }

        if (isRect)   //if rectangular toggle
        {
            g.setColour (Colour::fromRGBA (10, 10, 10, 255));
            g.fillRoundedRectangle (0, 0, width*0.95, height*0.95, height*0.1);

            if (isToggleOn == true)
            {
                g.setColour (colour);
                g.fillRoundedRectangle (width*0.01, height*0.01, width*0.93, height*0.93, height*0.1);
                opacity = 0.4;
            }
            else   //off
            {
                // Shadow
                for (float i=0.01; i<0.05; i+=0.01)
                {
                    g.setColour (Colour::fromRGBA (0, 0, 0, 255/(i*100)));
                    g.fillRoundedRectangle (width*i, height*i,
                                            width*0.95, height*0.95, height*0.1);
                }
                // Filling in the button
                Colour bg1 = Colour::fromRGBA (25, 25, 28, 255);
                Colour bg2 = Colour::fromRGBA (15, 15, 18, 255);
                ColourGradient cg = ColourGradient (bg1, 0, 0, bg2, width*0.5, height*0.5, false);
                g.setGradientFill (cg);
                g.fillRoundedRectangle (width*0.01, height*0.01, width*0.93, height*0.93, height*0.1);
                opacity = 0.2;
            }

            // For emphasising the top and left edges to give the illusion that light is shining on them
            ColourGradient edgeHighlight = ColourGradient (Colours::whitesmoke, 0, 0,
                                           Colours::transparentWhite, 0, height*0.1, false);
            g.setGradientFill (edgeHighlight);
            g.setOpacity (opacity);
            g.fillRoundedRectangle (0, 0, width*0.95, height*0.95, height*0.1);

            ColourGradient edgeHighlight2 = ColourGradient (Colours::whitesmoke, 0, 0,
                                            Colours::transparentWhite, height*0.1, 0, false);
            g.setGradientFill (edgeHighlight2);
            g.setOpacity (opacity);
            g.fillRoundedRectangle (0, 0, width*0.95, height*0.95, height*0.1);
        }
        else   //else if round toggle
        {
            //base
            ColourGradient base = ColourGradient (Colours::white, width*-0.3, height*-0.3, Colours::black,
                                                  width*0.8, height*0.8, false);
            g.setGradientFill(base);
            g.fillEllipse (0, 0, width, height);

            g.setColour(Colours::black);
            g.fillEllipse(width*0.09, height*0.09, width*0.82, height*0.82);

            Colour outline = Colour::fromRGB(70, 70, 70);

            g.setColour(outline.withAlpha(colour.getAlpha()));
            g.fillEllipse(width*0.04, height*0.04, width*0.92, height*0.92);

            if (isToggleOn)   //on
            {
//                ColourGradient cg = ColourGradient(colour.withSaturation(0.2), width*0.4, height*0.4, colour,
//                                                   width*0.8, height*0.8, true);

                //g.setGradientFill (cg);
                g.setColour(colour);
                g.fillEllipse(width*0.09, height*0.09, width*0.82, height*0.82);
            }
            else   //off
            {
                g.setColour(Colours::black);
                g.fillEllipse(width*0.09, height*0.09, width*0.82, height*0.82);

                Colour bg1 = Colour::fromRGBA (25, 25, 28, 255);
                Colour bg2 = Colour::fromRGBA (15, 15, 18, 255);
                ColourGradient cg = ColourGradient (bg1, 0, 0, bg2, width*0.5, height*0.5, false);


                //ColourGradient cg = ColourGradient (Colours::white, width*0.4, height*0.4, colour.darker(0.9), width*0.3, height*0.3, true);
                g.setGradientFill (cg);
                g.setOpacity(0.4);
                g.fillEllipse(width*0.1, height*0.1, width*0.8, height*0.8);
            }
        }
        return img;
    }

//====================================================================================================
    static void setPreference(ApplicationProperties* appPrefs, String pref, String value)
    {
        appPrefs->getUserSettings()->setValue(pref, var(value));
    }
//====================================================================================================
    static void drawSphericalThumb (Graphics& g, const float x, const float y,
                                    const float w, const float h, const Colour& colour,
                                    const float outlineThickness)
    {
        ColourGradient cg = ColourGradient (Colours::white, 0, 0, colour, w/2, h/2, false);
        cg.addColour (0.4, Colours::white.overlaidWith (colour));
        g.setGradientFill (cg);
        g.fillEllipse (x, y, w, h);
        g.setOpacity(.4);
        g.fillEllipse (x+1, y+1, w, h);
    }
//====================================================================================================
    static void drawGlassPointer (Graphics& g, float x, float y, float diameter,
                                  const Colour& colour, float outlineThickness, int direction)
    {
        if (diameter <= outlineThickness)
            return;

        Path p;

        p.startNewSubPath (x + diameter * 0.5f, y);
        p.lineTo (x + diameter*.9f, y + diameter * 0.6f);
        //    p.lineTo (x + diameter, y + diameter);
        p.lineTo (diameter*.1f+x, y + diameter*0.6f);
        //    p.lineTo (x, y + diameter * 0.6f);
        p.closeSubPath();

        p.applyTransform(AffineTransform::rotation (direction * (float_Pi * 0.5f), x + diameter * 0.5f, y + diameter * 0.5f));

        {
            ColourGradient cg (Colours::white.overlaidWith (colour.withMultipliedAlpha (0.7f)), 0, y,
                               Colours::white.overlaidWith (colour.withMultipliedAlpha (0.3f)), 0, y + diameter, false);

            cg.addColour (0.4, Colours::white.overlaidWith (colour));

            g.setGradientFill (cg);
            g.fillPath (p);
        }

        ColourGradient cg (Colours::transparentBlack,
                           x + diameter * 0.5f, y + diameter * 0.5f,
                           Colours::black.withAlpha (0.5f * outlineThickness * colour.getFloatAlpha()),
                           x - diameter * 0.2f, y + diameter * 0.5f, true);

        cg.addColour (0.5, Colours::transparentBlack);
        cg.addColour (0.7, Colours::black.withAlpha (0.07f * outlineThickness));

        g.setGradientFill (cg);
        g.fillPath (p);

        g.setColour (Colours::black.withAlpha (0.5f * colour.getFloatAlpha()));
        g.strokePath (p, PathStrokeType (outlineThickness));
    }
//=======================================================================================
//draw buttons for transport controls
    static Image drawSoundfilerButton(String type, String colour)
    {

        Image img = Image(Image::ARGB, 100, 100, true);
        Graphics g (img);
        g.fillAll(Colour(50, 50, 50));
        if (type == "play_normal")
        {
            Path p;
            p.addTriangle(img.getWidth()*0.3f, img.getHeight()*0.2f, img.getWidth()*0.7f, img.getHeight()*0.5f,
                          img.getWidth()*0.3f, img.getHeight()*0.8f);
            g.setColour(Colour::fromString(colour));
            g.fillPath(p);

            return img;
        }
        else if (type == "play_hover")
        {
            Path p;
            p.addTriangle(img.getWidth()*0.3f, img.getHeight()*0.2f, img.getWidth()*0.7f, img.getHeight()*0.5f,
                          img.getWidth()*0.3f, img.getHeight()*0.8f);
            g.setColour(Colour::fromString(colour));
            g.fillPath(p);

            ColourGradient shadow = ColourGradient (Colour::fromRGBA(50, 50, 50, 50), 0, 0,
                                                    Colour::fromRGBA(10, 10, 10, 100), img.getWidth(), img.getHeight(), false);
            g.setGradientFill(shadow);
            g.drawRoundedRectangle(0.5f, 0.5f, img.getWidth()-2.5f, img.getHeight()-2.5f, jmin(img.getWidth()/20.0f,
                                   img.getHeight()/20.0f), 2.5f);

            return img;
        }
        else if (type == "play_down")
        {
            //g.setColour(cUtils::getDarkerBackgroundSkin().darker(0.9f));
            g.setColour(Colours::black.withAlpha(0.4f));
            g.fillRoundedRectangle(0, 0, img.getWidth(), img.getHeight(), jmin(img.getWidth()/20.0f,
                                   img.getHeight()/20.0f));

            g.setColour(Colour::fromString(colour).withBrightness(5.0f));
            g.fillRoundedRectangle(img.getWidth()*0.3f, img.getHeight()*0.2f,
                                   img.getWidth()*0.2f, img.getHeight()*0.6f, img.getWidth()/20.0f);
            g.fillRoundedRectangle(img.getWidth()*0.6f, img.getHeight()*0.2f,
                                   img.getWidth()*0.2f, img.getHeight()*0.6f, img.getWidth()/20.0f);

            //3D outline
            ColourGradient shadow = ColourGradient (Colours::black, 0, 0,
                                                    Colour::fromRGBA(50, 50, 50, 50), img.getWidth(), img.getHeight(), false);
            g.setGradientFill(shadow);
            g.drawRoundedRectangle(1.5f, 1.5f, img.getWidth()-2.0f, img.getHeight()-2.0f, jmin(img.getWidth()/20.0f,
                                   img.getHeight()/20.0f), 2.0f);

            return img;
        }
        else if (type == "skip_end_normal")
        {
            Path p;
            p.addTriangle(img.getWidth()*0.2f, img.getHeight()*0.2f, img.getWidth()*0.6f, img.getHeight()*0.5f,
                          img.getWidth()*0.2f, img.getHeight()*0.8f);
            g.setColour(cUtils::getComponentFontColour());
            g.fillPath(p);
            g.fillRoundedRectangle(img.getWidth()*0.7f, img.getHeight()*0.2f,
                                   img.getWidth()*0.1f, img.getHeight()*0.6f, img.getWidth()/20.0f);
            return img;
        }
        else if (type == "skip_end_hover")
        {
            Path p;
            p.addTriangle(img.getWidth()*0.2f, img.getHeight()*0.2f, img.getWidth()*0.6f, img.getHeight()*0.5f,
                          img.getWidth()*0.2f, img.getHeight()*0.8f);
            g.setColour(cUtils::getComponentFontColour());
            g.fillPath(p);
            g.fillRoundedRectangle(img.getWidth()*0.7f, img.getHeight()*0.2f,
                                   img.getWidth()*0.1f, img.getHeight()*0.6f, img.getWidth()/20.0f);

            //3D outline
            ColourGradient shadow = ColourGradient (Colour::fromRGBA(50, 50, 50, 50), 0, 0,
                                                    Colour::fromRGBA(10, 10, 10, 100), img.getWidth(), img.getHeight(), false);
            g.setGradientFill(shadow);
            g.drawRoundedRectangle(0.5f, 0.5f, img.getWidth()-2.5f, img.getHeight()-2.5f, jmin(img.getWidth()/20.0f,
                                   img.getHeight()/20.0f), 2.5f);

            return img;
        }
        else if (type == "skip_end_down")
        {
            //g.setColour(cUtils::getDarkerBackgroundSkin().darker(0.9f));
            g.setColour(Colours::black.withAlpha(0.4f));
            g.fillRoundedRectangle(0, 0, img.getWidth(), img.getHeight(), jmin(img.getWidth()/20.0f,
                                   img.getHeight()/20.0f));

            Path p;
            p.addTriangle(img.getWidth()*0.2f, img.getHeight()*0.2f, img.getWidth()*0.6f, img.getHeight()*0.5f,
                          img.getWidth()*0.2f, img.getHeight()*0.8f);
            g.setColour(Colours::cornflowerblue.withBrightness(5.0f));
            g.fillPath(p);
            g.fillRoundedRectangle(img.getWidth()*0.7f, img.getHeight()*0.2f,
                                   img.getWidth()*0.1f, img.getHeight()*0.6f, img.getWidth()/20.0f);

            //3D outline
            ColourGradient shadow = ColourGradient (Colours::black, 0, 0,
                                                    Colour::fromRGBA(50, 50, 50, 50), img.getWidth(), img.getHeight(), false);
            g.setGradientFill(shadow);
            g.drawRoundedRectangle(1.5f, 1.5f, img.getWidth()-2.0f, img.getHeight()-2.0f, jmin(img.getWidth()/20.0f,
                                   img.getHeight()/20.0f), 2.0f);

            return img;
        }
        else if (type == "skip_start_normal")
        {
            Path p;
            p.addTriangle(img.getWidth()*0.4f, img.getHeight()*0.5f, img.getWidth()*0.8f, img.getHeight()*0.2f,
                          img.getWidth()*0.8f, img.getHeight()*0.8f);
            g.setColour(Colour::fromString(colour));
            g.fillPath(p);
            g.fillRoundedRectangle(img.getWidth()*0.2f, img.getHeight()*0.2f,
                                   img.getWidth()*0.1f, img.getHeight()*0.6f, img.getWidth()/20.0f);
            return img;
        }
        else if (type == "skip_start_hover")
        {
            Path p;
            p.addTriangle(img.getWidth()*0.4f, img.getHeight()*0.5f, img.getWidth()*0.8f, img.getHeight()*0.2f,
                          img.getWidth()*0.8f, img.getHeight()*0.8f);
            g.setColour(Colour::fromString(colour).withBrightness(.5));
            g.fillPath(p);
            g.fillRoundedRectangle(img.getWidth()*0.2f, img.getHeight()*0.2f,
                                   img.getWidth()*0.1f, img.getHeight()*0.6f, img.getWidth()/20.0f);

            //3D outline
            ColourGradient shadow = ColourGradient (Colour::fromRGBA(50, 50, 50, 50), 0, 0,
                                                    Colour::fromRGBA(10, 10, 10, 100), img.getWidth(), img.getHeight(), false);
            g.setGradientFill(shadow);
            g.drawRoundedRectangle(0.5f, 0.5f, img.getWidth()-2.5f, img.getHeight()-2.5f, jmin(img.getWidth()/20.0f,
                                   img.getHeight()/20.0f), 2.5f);
            return img;
        }
        else if (type == "skip_start_down")
        {
            //g.setColour(cUtils::getDarkerBackgroundSkin().darker(0.9f));
            g.setColour(Colours::black.withAlpha(0.4f));
            g.fillRoundedRectangle(0, 0, img.getWidth(), img.getHeight(), jmin(img.getWidth()/20.0f,
                                   img.getHeight()/20.0f));

            Path p;
            p.addTriangle(img.getWidth()*0.4f, img.getHeight()*0.5f, img.getWidth()*0.8f, img.getHeight()*0.2f,
                          img.getWidth()*0.8f, img.getHeight()*0.8f);
            g.setColour(Colour::fromString(colour).withBrightness(5.0f));
            g.fillPath(p);
            g.fillRoundedRectangle(img.getWidth()*0.2f, img.getHeight()*0.2f,
                                   img.getWidth()*0.1f, img.getHeight()*0.6f, img.getWidth()/20.0f);

            //3D outline
            ColourGradient shadow = ColourGradient (Colours::black, 0, 0,
                                                    Colour::fromRGBA(50, 50, 50, 50), img.getWidth(), img.getHeight(), false);
            g.setGradientFill(shadow);
            g.drawRoundedRectangle(1.5f, 1.5f, img.getWidth()-2.0f, img.getHeight()-2.0f, jmin(img.getWidth()/20.0f,
                                   img.getHeight()/20.0f), 2.0f);

            return img;
        }

        else return img;
    }
//====================================================================================================

    static const DrawablePath* createPlayButtonPath(int buttonSize, Colour col)
    {
        Path playPath;
        playPath.addTriangle(0, 0, buttonSize, buttonSize/2, 0, buttonSize);
        DrawablePath* playImage = new DrawablePath();
        playImage->setFill(col);
        playImage->setPath(playPath);
        return playImage;
    }

    static const DrawablePath* createPauseButtonPath(int buttonSize)
    {
        Path pausePath;
        pausePath.addRectangle(0, 0, buttonSize*.4, buttonSize);
        pausePath.addRectangle(buttonSize*.5, 0, buttonSize*.4, buttonSize);
        DrawablePath* pauseImage = new DrawablePath();;
        pauseImage->setPath(pausePath);
        return pauseImage;
    }

    static const DrawablePath* createZoomInButtonPath(int buttonSize)
    {
        Path zoomInPath;
        //zoomInPath.addEllipse(-100, -100, buttonSize+100, buttonSize+100);
        zoomInPath.addRectangle(0, buttonSize/3, buttonSize, buttonSize/3);
        zoomInPath.addRectangle(buttonSize/3, 0, buttonSize/3, buttonSize);
        DrawablePath* zoomImage = new DrawablePath();
        zoomImage->setFill(Colours::green.darker(.9f));
        //zoomImage->setStrokeFill(Colours::green.darker(.9f));
        //zoomImage->setStrokeThickness(4);
        zoomImage->setPath(zoomInPath);
        return zoomImage;
    }

    static const DrawablePath* createZoomOutButtonPath(int buttonSize)
    {
        Path zoomInPath;
        //zoomInPath.addEllipse(-100, -100, buttonSize+100, buttonSize+100);
        zoomInPath.addRectangle(0, buttonSize/3, buttonSize, buttonSize/3);
        DrawablePath* zoomImage = new DrawablePath();
        zoomImage->setFill(Colours::green.darker(.9f));
        //zoomImage->setStrokeFill(Colours::green.darker(.9f));
        //zoomImage->setStrokeThickness(4);
        zoomImage->setPath(zoomInPath);
        return zoomImage;
    }

    static const DrawablePath* createLoopButtonPath()
    {
        Path path;
        path.startNewSubPath(60, 0);
        path.lineTo(100, 0);
        path.closeSubPath();
        path.startNewSubPath(100, 0);
        path.lineTo(100, 80);
        path.closeSubPath();
        path.startNewSubPath(100, 80);
        path.lineTo(120, 80);
        path.lineTo(100, 100);
        path.lineTo(80, 80);
        path.closeSubPath();

        path.startNewSubPath(60, 100);
        path.lineTo(20, 100);
        path.closeSubPath();
        path.startNewSubPath(20, 100);
        path.lineTo(20, 20);
        path.closeSubPath();
        path.startNewSubPath(20, 20);
        path.lineTo(0, 20);
        path.lineTo(20, 0);
        path.lineTo(40, 20);
        path.closeSubPath();
        AffineTransform transform(AffineTransform::identity);
        path.applyTransform(transform.rotated(float_Pi/2.f));
        DrawablePath* image = new DrawablePath();
        image->setFill(Colours::white);
        image->setStrokeThickness(25.f);
        image->setStrokeFill(Colours::green.darker(.9f));
        image->setPath(path);
        return image;
    }

    static const DrawablePath* createStopButtonPath(int buttonSize, Colour col)
    {
        Path stopPath;
        stopPath.addRectangle(0, 0, buttonSize, buttonSize);
        DrawablePath* stopImage = new DrawablePath();
        stopImage->setPath(stopPath);
        stopImage->setFill(col);
        return stopImage;
    }

    static const DrawablePath* createRecordButtonPath(int buttonSize, Colour col)
    {
        Path stopPath;
        stopPath.addEllipse(0, 0, buttonSize, buttonSize);
        DrawablePath* stopImage = new DrawablePath();
        stopImage->setPath(stopPath);
        stopImage->setFill(col);
        return stopImage;
    }

    static const DrawablePath* createDisabledStopButtonPath(float buttonSize, Colour col1, Colour col2)
    {
        Path stopPath;
        stopPath.addRectangle(0, 0, buttonSize, buttonSize);
        stopPath.addLineSegment(Line<float>(0.f, 0.f, buttonSize, buttonSize), 1.f);
        stopPath.addLineSegment(Line<float>(buttonSize, 0.f, 0.f, buttonSize), 1.f);
        DrawablePath* stopImage = new DrawablePath();
        stopImage->setFill(Colours::red);
        stopImage->setStrokeFill(col2);
        stopImage->setPath(stopPath);
        return stopImage;
    }

    static const DrawablePath* createEnvelopeButtonPath(Colour col)
    {
        Path path;
        path.addEllipse(-10, 70, 25, 25);
        path.startNewSubPath(10, 70);
        path.lineTo(45, 10);
        path.closeSubPath();
        path.addEllipse(35, 25, 25, 25);
        path.startNewSubPath(40, 10);
        path.lineTo(70, 80);
        path.closeSubPath();
        path.addEllipse(65, 75, 25, 25);
        path.startNewSubPath(70, 80);
        path.lineTo(120, 10);
        path.closeSubPath();
        path.addEllipse(115, 5, 25, 25);

        DrawablePath* image = new DrawablePath();
        image->setFill(Colours::white);
        image->setStrokeThickness(25.f);
        image->setStrokeFill(col);
        image->setPath(path);
        return image;
    }

    static const DrawablePath* createOpenButtonPath(int buttonSize)
    {
        Path openPath;
        openPath.startNewSubPath(4, 2);
        openPath.lineTo(18, 2);
        openPath.lineTo(18, 5);
        openPath.lineTo(22, 5);
        openPath.lineTo(22, buttonSize-5);
        openPath.lineTo(4, buttonSize-5);
        openPath.lineTo(4, 2);
        //openPath.startNewSubPath(8, 7);
        //openPath.lineTo(24, 7);
        openPath.closeSubPath();
        DrawablePath* openImage = new DrawablePath();;
        openImage->setPath(openPath);
        openImage->setFill(Colours::white);
        openImage->setStrokeFill(Colours::green.darker(.9f));
        openImage->setStrokeThickness(4);
        return openImage;
    }

    static void drawBypassIcon(Graphics& g, Rectangle<float> rect, bool isActive)
    {
        const float x = rect.getX();
        const float y = rect.getY();
        const float w = rect.getWidth()-5.f;
        const float h = rect.getHeight();
        const float d = 5;
        g.setColour(isActive ? Colours::cornflowerblue : Colours::lime);
        Path p;
        p.startNewSubPath(x+5, y+h/2.f+d/2.f);
        g.drawEllipse(x, y+h/2.f, d, d, 1);
        g.drawEllipse(x+w, y+h/2.f, d, d, 1.f);

        if(!isActive)
        {
            p.lineTo(x+w, y+h/2.f+d/2.f);
        }
        else
        {
            p.addArc(x+w, y+h/2.f+d/2.f, 5, 5, 3.14, 3.14);
        }

        p.closeSubPath();
        g.strokePath(p, PathStrokeType(1));
    }
//====================================================================================================
    static String returnFullPathForFile(String file, String fullPath)
    {
        String pic;
        if(file.isNotEmpty())
        {
#ifdef MACOSX
#ifndef Cabbage_Build_Standalone
            pic.append(String("/Contents/")+file, 1024);
#else
            pic = fullPath+String("//")+file;
#endif
#endif
#ifdef LINUX
            pic = fullPath+String("/")+file;;
#endif
#ifdef WIN32
            pic = fullPath+String("\\")+file;;
#endif
#ifdef AndroidBuild
            String homeDir = String(getenv("EXTERNAL_STORAGE"))+String("/Cabbage/");
            pic = homeDir+String("/")+file;;
#endif
            return pic;
        }
        else return "";
    }

    //===========================================================================================

};

#endif
