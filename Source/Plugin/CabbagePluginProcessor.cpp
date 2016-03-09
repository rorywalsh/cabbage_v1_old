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

#include "CabbagePluginProcessor.h"
#include "CabbagePluginEditor.h"
#include <iostream>


#define MAX_BUFFER_SIZE 1024
//Csound functions like to return 0 when everything is ok..
#define LOGGER 0

//these two lines may need to be copied to top part of csound.h
//#define int32 int
//#define uint32 unsigned int

CabbageLookAndFeel* lookAndFeel;
CabbageLookAndFeelBasic* lookAndFeelBasic;

//juce_ImplementSingleton (IdentArray);

char tmp_string[4096] = {0};


//==============================================================================
// There are two different CabbagePluginAudioProcessor constructors. One for the
// standalone application and the other for the plugin library
//==============================================================================
//#ifdef Cabbage_Build_Standalone
#if defined(Cabbage_Build_Standalone) || defined(CABBAGE_HOST)
//===========================================================
// STANDALONE - CONSTRUCTOR
//===========================================================
CabbagePluginAudioProcessor::CabbagePluginAudioProcessor(String inputfile, bool guiOnOff, float _scale)
    :backgroundThread ("Audio Recorder Thread"),
     activeWriter (nullptr),
     csoundStatus(false),
     csdFile(File(inputfile)),
     showMIDI(false),
     csCompileResult(1),
     changeMessageType(""),
     guiON(false),
     currentLine(-99),
     noSteps(0),
     noPatterns(0),
     timeCounter(0),
     beat(0),
     bpm(120),
     patMatrixActive(0),
     masterCounter(0),
     xyAutosCreated(false),
     updateTable(false),
     yieldCallbackBool(false),
     yieldCounter(10),
     isNativeThreadRunning(false),
     scoreEvents(),
     nativePluginEditor(false),
     averageSampleIndex(0),
     automationAmp(0),
     isAutomator(false),
     createLog(false),
     automationParamID(-1),
     debugMessage(""),
     guiRefreshRate(50),
     mouseY(0),
     mouseX(0),
     isWinXP(false),
     ksmpsOffset(0),
     breakCount(0),
     stopProcessing(false),
     firstTime(true),
     isMuted(false),
     isBypassed(false),
     vuCounter(0),
     updateFFTDisplay(false)
{
    codeEditor = nullptr;
    if(compileCsoundAndCreateGUI(false)==0)
    {
        if(!inputfile.equalsIgnoreCase(""))
            cUtils::debug("Csound coudln't compile your file:"+File(inputfile).getFullPathName());
    }
}
#else

//===========================================================
// PLUGIN - CONSTRUCTOR
//===========================================================
CabbagePluginAudioProcessor::CabbagePluginAudioProcessor(String sourcefile, Point<float> instrScale):
    backgroundThread ("Audio Recorder Thread"),
    activeWriter (nullptr),
    csoundStatus(false),
    showMIDI(false),
    csCompileResult(1),
    changeMessageType(""),
    guiON(false),
    currentLine(-99),
    noSteps(0),
    noPatterns(0),
    timeCounter(0),
    beat(0),
    bpm(120),
    patMatrixActive(0),
    masterCounter(0),
    xyAutosCreated(false),
    createLog(false),
    updateTable(false),
    yieldCallbackBool(false),
    yieldCounter(10),
    nativePluginEditor(false),
    averageSampleIndex(0),
    stopProcessing(false),
    firstTime(false),
    isMuted(false),
    isBypassed(false),
    vuCounter(0),
    scale(instrScale),
    updateFFTDisplay(false)
{
    //If a sourcefile is not given, Cabbage plugins always try to load a csd file with the same name as the plugin library.
    //Therefore we need to find the name of the library and append a '.csd' to it.

    if(!File(sourcefile).existsAsFile())
    {
#ifdef MACOSX
#ifdef CABBAGE_AU
        csdFile = File("~/Music/CabbageAudioUnit/startup.csd");

#else
        String osxCSD = File::getSpecialLocation(File::currentApplicationFile).getFullPathName()+String("/Contents/")+File::getSpecialLocation(File::currentApplicationFile).getFileName();
        File thisFile(osxCSD);
        Logger::writeToLog("MACOSX defined OK");
        csdFile = thisFile.withFileExtension(String(".csd")).getFullPathName();
#endif
        //cUtils::showMessage(csdFile.getFullPathName());
//#elseif AndroidBuild
//        File inFile(File::getSpecialLocation(File::currentApplicationFile));
//        ScopedPointer<InputStream> fileStream;
//        fileStream = File(inFile.getFullPathName()).createInputStream();
//        ZipFile zipFile (fileStream, false);
//
//        //sample files
//        String mkdir = "mkdir -p \""+String(getenv("EXTERNAL_STORAGE"))+String("/Cabbage\"");
//        String homeDir = String(getenv("EXTERNAL_STORAGE"))+String("/Cabbage/");
//
//        system(mkdir.toUTF8().getAddress());
//        ScopedPointer<InputStream> fileContents;

#else
        File thisFile(File::getSpecialLocation(File::currentExecutableFile));
        csdFile = thisFile.withFileExtension(String(".csd")).getFullPathName();
#endif

        //Logger::writeToLog(File::getSpecialLocation(File::currentExecutableFile).getFullPathName());
    }

    else
        csdFile = File(sourcefile);


    if(compileCsoundAndCreateGUI(true)==0)
    {
        suspendProcessing(true);
        cUtils::debug("Csound coudln't compile your file:"+csdFile.getFullPathName());

    }

}
#endif

//===========================================================
// PLUGIN - DESTRUCTOR
//===========================================================
CabbagePluginAudioProcessor::~CabbagePluginAudioProcessor()
{
    //deleteAndZero(cabbageCsoundEditor);

    deleteAndZero(lookAndFeel);
    deleteAndZero(lookAndFeelBasic);
    Logger::setCurrentLogger (nullptr);
    stopProcessing = true;
    removeAllChangeListeners();

#ifndef Cabbage_No_Csound

    xyAutomation.clear();

    if(csound)
    {
        this->getCallbackLock().enter();
        csound->DeleteChannelList(csoundChanList);
        Logger::writeToLog("about to cleanup Csound");
        //csoundDebugContinue(csound);
        //csound->Cleanup();
        //csound->SetHostImplementedMIDIIO(false);

        //csound->Reset();
        //csoundDebuggerClean(csound->GetCsound());

        csound = nullptr;
        Logger::writeToLog("Csound cleaned up");
    }

#endif
}

//============================================================================
//SET SCREEN WIDTH AND SCREEN HEIGHT MACROS
//============================================================================
void CabbagePluginAudioProcessor::setScreenMacros()
{
//android opens full screen by default.
#ifdef AndroidBuild
    Rectangle<int> rect(Desktop::getInstance().getDisplays().getMainDisplay().userArea);
    String screenWidth = "--omacro:SCREEN_WIDTH=\""+String(rect.getWidth()-60)+"\"";
    csound->SetOption(screenWidth.toUTF8().getAddress());
    String screenHeight = "--omacro:SCREEN_HEIGHT=\""+String(rect.getHeight()-60)+"\"";
    csound->SetOption(screenHeight.toUTF8().getAddress());
#else
    String width = "--omacro:SCREEN_WIDTH=\""+String(screenWidth)+"\"";
    csound->SetOption(width.toUTF8().getAddress());
    String height = "--omacro:SCREEN_HEIGHT=\""+String(screenHeight)+"\"";
    csound->SetOption(height.toUTF8().getAddress());
#endif
}
//============================================================================
//FIND MACROS AND ADD THEM TO SETUP OPTIONS
//============================================================================
void CabbagePluginAudioProcessor::addMacros(String csdText)
{
    StringArray csdArray;
    String macroName, macroText;

    csdArray.addLines(csdText);
    for(int i=0; i<csdArray.size(); i++)
    {
        if(csdArray[i].trim().substring(0, 7)=="#define")
        {
            StringArray tokens;
            tokens.addTokens(csdArray[i].replace("#", "").trim() ," ");
            macroName = tokens[1];
            tokens.remove(0);
            tokens.remove(0);
            macroText = "\\\"" + tokens.joinIntoString(" ").replace("\"", "\\\\\\\"")+"\\\"";
            String fullMacro = "--omacro:"+macroName+"="+macroText+"\"";
            csound->SetOption(fullMacro.toUTF8().getAddress());
        }

        if(csdArray[i].contains("</Cabbage>"))
            i=csdArray.size();
    }
}

void CabbagePluginAudioProcessor::initAllChannels()
{
    //init all channels with their init val, and set parameters
    for(int i=0; i<guiCtrls.size(); i++)
    {
        //		Logger::writeToLog(guiCtrls.getReference(i).getStringProp(CabbageIDs::channel)+": "+String(guiCtrls[i].getNumProp(CabbageIDs::value)));
        if(guiCtrls[i].getStringProp("channeltype")=="string")
            //deal with combobox strings..
            csound->SetChannel(guiCtrls[i].getStringProp(CabbageIDs::channel).toUTF8(), "");
        //									guiCtrls.getReference(i).getStringArrayPropValue("text", guiCtrls[i].getNumProp(CabbageIDs::value)-1).toUTF8().getAddress());
        else
        {
            if(guiCtrls[i].getStringProp(CabbageIDs::type)==CabbageIDs::hrange ||
                    guiCtrls[i].getStringProp(CabbageIDs::type)==CabbageIDs::vrange)
            {
                csound->SetChannel( guiCtrls[i].getStringArrayPropValue(CabbageIDs::channel, 0).toUTF8(), guiCtrls[i].getNumProp(CabbageIDs::minvalue));
                csound->SetChannel( guiCtrls[i].getStringArrayPropValue(CabbageIDs::channel, 1).toUTF8(), guiCtrls[i].getNumProp(CabbageIDs::maxvalue));
            }
            else
                csound->SetChannel( guiCtrls[i].getStringProp(CabbageIDs::channel).toUTF8(), guiCtrls[i].getNumProp(CabbageIDs::value));
        }

        if(guiCtrls[i].getStringProp(CabbageIDs::type)!="hrange" && guiCtrls.getReference(i).getStringProp(CabbageIDs::type)!="vrange")
        {
           
            messageQueue.addOutgoingChannelMessageToQueue(guiCtrls[i].getStringProp(CabbageIDs::channel),
                    guiCtrls[i].getNumProp(CabbageIDs::value), guiCtrls[i].getStringProp(CabbageIDs::type));
        }
    }

    //init all channels with their init val, and set parameters
    for(int i=0; i<guiLayoutCtrls.size(); i++)
    {
        if(guiLayoutCtrls[i].getStringProp(CabbageIDs::type).equalsIgnoreCase("texteditor"))
            csound->SetChannel(guiLayoutCtrls[i].getStringProp(CabbageIDs::channel).toUTF8(),
                               guiLayoutCtrls[i].getStringProp(CabbageIDs::text).toUTF8().getAddress());
        if(guiLayoutCtrls[i].getStringProp(CabbageIDs::identchannel).isNotEmpty())
            //deal with combobox strings..
            csound->SetChannel(guiLayoutCtrls[i].getStringProp(CabbageIDs::identchannel).toUTF8(), "");
    }
    this->updateCabbageControls();
}

//============================================================================
//COMPILE CSOUND
//============================================================================
int CabbagePluginAudioProcessor::compileCsoundAndCreateGUI(bool isPlugin)
{
    initialiseWidgets(csdFile.loadFileAsString(), true);

    //File(csdFile.getFullPathName()).setAsCurrentWorkingDirectory();
    csdFile.setAsCurrentWorkingDirectory();


    StringArray tmpArray;
    CabbageGUIType cAttr;

    tmpArray.addLines(csdFile.loadFileAsString());
    for(int i=0; i<tmpArray.size() || tmpArray[i].contains("</Cabbage>"); i++)
        if(tmpArray[i].contains("logger("))
        {
            CabbageGUIType cAttr(tmpArray[i], -99);
            createLog = cAttr.getNumProp(CabbageIDs::logger);
            if(createLog)
            {
                String logFileName = csdFile.getParentDirectory().getFullPathName()+String("/")+csdFile.getFileNameWithoutExtension()+String("_Log.txt");
                logFile = File(logFileName);
                fileLogger = new FileLogger(logFile, String("Cabbage Log.."));
                Logger::setCurrentLogger(fileLogger);
            }
        }

    setOpcodeDirEnv();

#ifndef Cabbage_No_Csound
#if !defined(AndroidBuild)
    csound = new Csound();
#if !defined(Cabbage_Build_Standalone) && !defined(CABBAGE_HOST)
    cabbageCsoundEditor = nullptr;
#endif
#else
    csound = new AndroidCsound();
    //csound->setOpenSlCallbacks(); // for android audio to work
#endif

    csound->SetHostImplementedMIDIIO(true);
    csound->SetHostData(this);
    midiOutputBuffer.clear();
    csound->CreateMessageBuffer(0);
    csound->SetExternalMidiInOpenCallback(OpenMidiInputDevice);
    csound->SetExternalMidiReadCallback(ReadMidiData);
    csound->SetExternalMidiOutOpenCallback(OpenMidiOutputDevice);
    csound->SetExternalMidiWriteCallback(WriteMidiData);


    csound->SetIsGraphable(true);
    csound->SetMakeGraphCallback(makeGraphCallback);
    csound->SetDrawGraphCallback(drawGraphCallback);
    csound->SetKillGraphCallback(killGraphCallback);
    csound->SetExitGraphCallback(exitGraphCallback);


    csoundChanList = NULL;
    numCsoundChannels = 0;
    csndIndex = 32;
    startTimer(20);

    csoundParams = nullptr;
    csoundParams = new CSOUND_PARAMS();
#ifndef CABBAGE_HOST
    csoundParams->nchnls_override = this->getNumOutputChannels();
#endif

    csoundParams->sample_rate_override = this->getSampleRate();
    csoundParams->control_rate_override = cUtils::getKrFromFile(csdFile.getFullPathName(), (int)getSampleRate());


    csoundParams->displays = 0;
    csound->SetParams(csoundParams);
    csound->SetOption((char*)"-n");
    csound->SetOption((char*)"-d");
    if(isPlugin)
        csound->SetOption((char*)"--omacro:IS_A_PLUGIN=\"1\"");

#ifdef AndroidBuild
    csound->SetOption((char*)"--omacro:IS_ANDROID=\"1\"");
#endif
    setScreenMacros();
    addMacros(csdFile.loadFileAsString());
    csCompileResult = csound->Compile(const_cast<char*>(csdFile.getFullPathName().toUTF8().getAddress()));
    //csoundSetBreakpointCallback(csound->GetCsound(), breakpointCallback, (void*)this);
    csdFile.getParentDirectory().setAsCurrentWorkingDirectory();
    if(csCompileResult==OK)
    {
        initAllChannels();
        firstTime=false;
        guiRefreshRate = getCsoundKsmpsSize()*2;

        if(!isPlugin)
        {
            setPlayConfigDetails(getNumberCsoundOutChannels(),
                                 getNumberCsoundOutChannels(),
                                 getCsoundSamplingRate(),
                                 getCsoundKsmpsSize());
        }

        Logger::writeToLog("compiled Ok");
        keyboardState.allNotesOff(0);
        keyboardState.reset();

        //simple hack to allow tables to be set up correctly.
        csound->PerformKsmps();
        csound->SetScoreOffsetSeconds(0);
        csound->RewindScore();
        csdKsmps = csound->GetKsmps();

        if(csound->GetSpout()==nullptr);
        CSspout = csound->GetSpout();
        CSspin  = csound->GetSpin();
        cs_scale = csound->Get0dBFS();
        csndIndex = csound->GetKsmps();
        this->setLatencySamples(csound->GetKsmps());
        updateHostDisplay();
        csoundStatus = true;
        debugMessageArray.add(VERSION);
        debugMessageArray.add(String("\n"));
        char path[8192] = {0};

#ifdef WIN32

        csound->GetStringChannel("CSD_PATH", path);
        if(String(path).isNotEmpty())
            File(path).getParentDirectory().setAsCurrentWorkingDirectory();
        else
            csound->SetChannel("CSD_PATH", File(csdFile).getParentDirectory().getFullPathName().replace("\\", "\\\\").toUTF8().getAddress());
#else
        csound->GetStringChannel("CSD_PATH", path);
        if(String(path).isNotEmpty())
            File(path).getParentDirectory().setAsCurrentWorkingDirectory();
        else
            csound->SetChannel("CSD_PATH", File(csdFile).getParentDirectory().getFullPathName().toUTF8().getAddress());
#endif

        if(isPlugin)
            csound->SetChannel("IS_A_PLUGIN", 1.0);
        else
            csound->SetChannel("IS_A_PLUGIN", 0.0);

        if(isPlugin)
        {
            if (getPlayHead() != 0 && getPlayHead()->getCurrentPosition (hostInfo))
            {
                csound->SetChannel(CabbageIDs::hostbpm.toUTF8(), hostInfo.bpm);
                csound->SetChannel(CabbageIDs::timeinseconds.toUTF8(), hostInfo.timeInSeconds);
                csound->SetChannel(CabbageIDs::isplaying.toUTF8(), hostInfo.isPlaying);
                csound->SetChannel(CabbageIDs::isrecording.toUTF8(), hostInfo.isRecording);
                csound->SetChannel(CabbageIDs::hostppqpos.toUTF8(), hostInfo.ppqPosition);
                csound->SetChannel(CabbageIDs::timeinsamples.toUTF8(), hostInfo.timeInSamples);
                csound->SetChannel(CabbageIDs::timeSigDenom.toUTF8(), hostInfo.timeSigDenominator);
                csound->SetChannel(CabbageIDs::timeSigNum.toUTF8(), hostInfo.timeSigNumerator);
            }
        }
        cUtils::debug("Everything has been setup without an issue. ");

    }
    else
    {
        cUtils::debug("Csound couldn't compile your file");
#ifndef AndroidBuild
        String message= "Csound couldn't compile your file. Please check the Csound output console for more information\n\nYou can disable this warning from the Options->Preference menu.";
        if(getActiveEditor() && getPreference(appProperties, "DisableCompilerErrorWarning")==0)
            showMessage(message, &getActiveEditor()->getLookAndFeel());
#endif
        csoundStatus=false;
        return 0;
    }
#endif

    addWidgetsToEditor(true);

    return 1;
}
//============================================================================
//RECOMPILE CSOUND. THIS IS CALLED FROM THE PLUGIN HOST WHEN UDPATES ARE MADE ON THE FLY
//============================================================================
int CabbagePluginAudioProcessor::recompileCsound(File file)
{
#ifndef Cabbage_No_Csound

    stopProcessing = true;
    getCallbackLock().enter();
    numChannelsChanged();
    midiOutputBuffer.clear();
    //csound->DeleteChannelList(csoundChanList);

    //cUtils::debug(file.loadFileAsString());

    csound->DestroyMessageBuffer();
    //csound = nullptr;
    //csound = new Csound();
    csound->Reset();
    ksmpsOffset = 0;
    breakCount = 0;

    csound->SetHostImplementedMIDIIO(true);
    csound->SetHostData(this);
    csound->CreateMessageBuffer(0);
    csound->SetExternalMidiInOpenCallback(OpenMidiInputDevice);
    csound->SetExternalMidiReadCallback(ReadMidiData);
    csound->SetExternalMidiOutOpenCallback(OpenMidiOutputDevice);
    csound->SetExternalMidiWriteCallback(WriteMidiData);
    //csound->SetIsGraphable(0);

    csound->SetIsGraphable(true);
    csound->SetMakeGraphCallback(makeGraphCallback);
    csound->SetDrawGraphCallback(drawGraphCallback);
    csound->SetKillGraphCallback(killGraphCallback);
    csound->SetExitGraphCallback(exitGraphCallback);

    csoundParams = nullptr;
    csoundParams = new CSOUND_PARAMS();
#ifndef CABBAGE_HOST
    csoundParams->nchnls_override =2;
#endif
    csoundParams->displays = 0;
    //csoundParams->sample_rate_override = this->getSampleRate();
    //csoundParams->control_rate_override = cUtils::getKrFromFile(file.getFullPathName(), (int)getSampleRate());
    csound->SetParams(csoundParams);
    csound->SetOption((char*)"-n");
    //csound->SetOption((char*)"-d");

    setScreenMacros();

    addMacros(file.loadFileAsString());
    csound->SetHostImplementedMIDIIO(true);
    xyAutosCreated = false;
    numCsoundChannels = 0;


    CSspout = nullptr;
    CSspin = nullptr;
    //csound->Stop();


    csCompileResult = csound->Compile(const_cast<char*>(file.getFullPathName().toUTF8().getAddress()));
    file.getParentDirectory().setAsCurrentWorkingDirectory();
    
    initAllChannels();

#ifdef BUILD_DEBUGGER
    for(int i=0; i<breakpointInstruments.size(); i++)
    {
        getCallbackLock().enter();
        if(i==0)
            csoundDebuggerInit(csound->GetCsound());
        csoundSetBreakpointCallback(csound->GetCsound(), breakpointCallback, (void*)this);
        csoundSetInstrumentBreakpoint(csound->GetCsound(), breakpointInstruments[i], 0);
        getCallbackLock().exit();
    }
#endif

    StringArray lines, includeFiles;
    lines.addLines(file.loadFileAsString());

    for(int i=0; i<lines.size(); i++)
        if(lines[i].contains("include("))
        {
            CabbageGUIType cAttr(lines[i], -99);
            for(int y=0; y<cAttr.getStringArrayProp(CabbageIDs::include).size(); y++)
            {
                String infile = cAttr.getStringArrayPropValue(CabbageIDs::include, y);
                if(!File::isAbsolutePath(infile))
                {
                    String ifile = returnFullPathForFile(infile, file.getParentDirectory().getFullPathName());
                    includeFiles.add(ifile);
                }
                else
                    includeFiles.add(infile);
            }
            break;
        }

    includeFiles.removeDuplicates(0);


    if(csCompileResult==OK)
    {
        firstTime=false;
        keyboardState.allNotesOff(0);
        keyboardState.reset();

        //simple hack to allow tables to be set up correctly.
        csound->PerformKsmps();
        csound->SetScoreOffsetSeconds(0);
        csound->RewindScore();
        for(int i=0; i<includeFiles.size(); i++)
        {
            //	csound->CompileOrc(File(includeFiles[i]).loadFileAsString().toUTF8().getAddress());
            //	csound->InputMessage("i\"PROCESSOR\" 0 3600 1");
        }
        csndIndex = 0;
        CSspout = csound->GetSpout();
        CSspin  = csound->GetSpin();
        //csound->PerformKsmps();
        //csound->SetScoreOffsetSeconds(0);
        //csound->RewindScore();
        Logger::writeToLog("Csound compiled your file");
        //numCsoundChannels = csoundListChannels(csound->GetCsound(), &csoundChanList);
        cs_scale = csound->Get0dBFS();
        csoundStatus = true;

        //init all channels with their init val
        for(int i=0; i<guiCtrls.size(); i++)
        {
            messageQueue.addOutgoingChannelMessageToQueue(guiCtrls.getReference(i).getStringProp(CabbageIDs::channel),
                    guiCtrls.getReference(i).getNumProp(CabbageIDs::value), guiCtrls.getReference(i).getStringProp(CabbageIDs::type));
            csound->SetChannel( guiCtrls.getReference(i).getStringProp(CabbageIDs::channel).toUTF8(),
                                guiCtrls.getReference(i).getNumProp(CabbageIDs::value));
            
            
            this->updateCabbageControls();
        }

        debugMessageArray.add(CABBAGE_VERSION);
        debugMessageArray.add(String("\n"));
        //removeAllChangeListeners();
        getCallbackLock().exit();


#ifdef WIN32
        csound->SetChannel("CSD_PATH", file.getParentDirectory().getFullPathName().replace("\\", "\\\\").toUTF8().getAddress());
#else
        csound->SetChannel("CSD_PATH", file.getParentDirectory().getFullPathName().toUTF8().getAddress());
#endif

        csound->SetChannel("IS_A_PLUGIN", 0.0);

        stopProcessing = false;

        return csCompileResult;
    }
    else
    {
        Logger::writeToLog("Csound couldn't compile your file");
        csoundStatus=false;
#if !defined(AndroidBuild) && !defined(CABBAGE_AU)
        String message= "Csound couldn't compile your file. Please check the Csound output console for more information\n\nYou can disable this warning from the Options->Preference menu.";
        if(getActiveEditor() && getPreference(appProperties, "DisableCompilerErrorWarning")==0)
            showMessage(message, &getActiveEditor()->getLookAndFeel());
#endif
    }
    getCallbackLock().exit();

    return csCompileResult;
#endif
}
//===========================================================
// PARSE CSD FILE AND FILL GUI/GUI-LAYOUT VECTORs.
// NO JUCE WIDGETS GET CREATED IN THIS CLASS. ALL
// GUI OBJECTS ARE CREATED ON THE FLY IN THE CABBAGE PLUGIN
// EDITOR FROM INFORMATION HELD IN THE GUICONTROLS VECTOR
//===========================================================
//maybe this should only be done at the end of a k-rate cycle..
void CabbagePluginAudioProcessor::initialiseWidgets(String source, bool refresh)
{

    GUILayoutCtrlsArray layoutCtrlsArray;
    GUICtrlsArray ctrlsArray;

    if(refresh==true)
    {
        guiLayoutCtrls.clear();
        guiCtrls.clear();
        CabbagePluginAudioProcessorEditor* editor = dynamic_cast<CabbagePluginAudioProcessorEditor*>(this->getActiveEditor());
        if(editor)
        {
            editor->comps.clear(true);
            editor->layoutComps.clear(true);
            editor->subPatches.clear(true);
            editor->popupMenus.clear();
        }
    }

    widgetTypes.clear();
    indexOfLastGUICtrl = guiCtrls.size();
    indexOfLastLayoutCtrl = guiLayoutCtrls.size();

    String warningMessage;

    //cUtils::showMessage(scale);

    //setGuiEnabled((false));
    int guiID=0;
    StringArray csdText;


    String csdLine("");
    csdText.addLines(source.replace("\t", " "));
    bool multiComment = false;

    //csound->Message("\n===Cabbage Warnings===\n");
    int lineWhichCabbageSectionStarts = 0;
    int lineWhichCabbageSectionEnds = csdText.size();
    for(int i=0; i<csdText.size(); i++)
    {
        if(csdText[i].contains("<Cabbage>"))
            lineWhichCabbageSectionStarts=i;
        else if(csdText[i].contains("</Cabbage>"))
            lineWhichCabbageSectionEnds=i;
    }

    csdText.removeRange(0, lineWhichCabbageSectionStarts);
    csdText.removeRange(lineWhichCabbageSectionEnds+1, 99999);

    //cUtils::debug(csdText.size());

    for(int i=0; i<csdText.size(); i++)
    {
        String temp;
        //roll any lines with a \ into the next line and remove from csdText array...
        if(csdText[i].contains(" \\"))
        {
            temp = csdText[i+1];
            csdText.remove(i+1);
            csdText.set(i, csdText[i].replace(" \\", " ")+temp);
            //cUtils::debug(csdText[i]);
        }
    }

    for(int i=0; i<csdText.size(); i++)
    {

        int csdLineNumber=0;
#if (defined(Cabbage_Build_Standalone) || defined(CABBAGE_HOST)) && !defined(AndroidBuild)
        if(!refresh)
        {
            StringArray fullText;
            if(codeEditor)
                fullText.addLines(codeEditor->getAllText());
            else
                fullText.addLines(csdFile.loadFileAsString());
            for(int u=0; u<fullText.size(); u++)
                if(csdText[i].equalsIgnoreCase(fullText[u]))
                {
                    csdLineNumber = u;
                    //Logger::writeToLog("LineNumber:"+String(csdLineNumber)+"\n"+csdText[i]);
                    u=fullText.size();
                }
        }
#endif

        if(csdText[i].indexOfWholeWordIgnoreCase(String("</Cabbage>"))==-1)
        {
            if(csdText[i].trim().isNotEmpty())
            {
                csdLine = csdText[i];
                if(refresh)
                    csdLineNumber = i;
                //tidy up string
                csdLine = csdLine.trimStart();
                //csdLine = csdLine.removeCharacters(" \\");
                //csdLine = csdLine.removeCharacters(",\\");

                StringArray tokes;
                tokes.addTokens(csdLine.trimEnd(), ", ", "\"");

                if(tokes[0].containsIgnoreCase(String("/*")))
                {
                    multiComment = true;
                }
                if(tokes[0].containsIgnoreCase(String("*\\")))
                {
                    multiComment = false;
                }

                if(tokes[0].containsIgnoreCase(String(";")))
                {
                    //allows for single line comments
                }
                else if(tokes[0].containsIgnoreCase(String("}")))
                {
                    plantFlag = ""; //reset plantFlag when a closing bracket is found
                    presetFlag = "";
                }

                if(tokes[0].containsIgnoreCase("#define"))
                {
                    tokes.removeEmptyStrings();
                    if(tokes.size()>2)
                    {
                        if(tokes[1].contains("channel("))
                        {
                            CabbageGUIType cAttr(tokes.joinIntoString(" "), -1);
                            macroText.set("$"+tokes[2], " "+csdLine.substring(csdLine.indexOf(tokes[2])+tokes[2].length())+" ");
                            tokes.removeRange(0, 3);
                            String macroText = tokes.joinIntoString(" ");
                            String channel = cAttr.getStringProp(CabbageIDs::channel);
                            csound->SetChannel(channel.toUTF8().getAddress(), macroText.toUTF8().getAddress());
                        }
                        else
                            macroText.set("$"+tokes[1], " "+csdLine.substring(csdLine.indexOf(tokes[1])+tokes[1].length())+" ");
                    }
                }

                for(int cnt=0; cnt<macroText.size(); cnt++)
                {
                    if(csdLine.contains(macroText.getName(cnt).toString()))
                        csdLine = csdLine.replace(macroText.getName(cnt), macroText.getWithDefault(macroText.getName(cnt), "").toString()+" ");
                }

                if(!multiComment)
                    //check that the line of Cabbage code contains a valid widget name and
                    //populate the guiLayoutCtrls vector
                    if(layoutCtrlsArray.contains(tokes[0]))
                    {
                        CabbageGUIType cAttr(csdLine.trimEnd(), guiID);
                        cAttr.setStringProp(CabbageIDs::parentdir, getCsoundInputFile().getParentDirectory().getFullPathName());
#ifdef AndroidBuild
                        cAttr.scaleWidget(scale);
#endif
                        if(cAttr.getStringProp(CabbageIDs::type)=="form")
                        {
                            screenWidth = cAttr.getNumProp(CabbageIDs::width);
                            screenHeight = cAttr.getNumProp(CabbageIDs::height);
                        }


                        cAttr.setNumProp(CabbageIDs::lineNumber, csdLineNumber+lineWhichCabbageSectionStarts);

                        warningMessage = "";
                        warningMessage << "Line Number:" << csdLineNumber+1 << "\n" << cAttr.getWarningMessages();
                        if(cAttr.getWarningMessages().isNotEmpty())
                            csound->Message(warningMessage.toUTF8().getAddress());

                        if(cAttr.getNumProp(CabbageIDs::guirefresh)>1)
                            guiRefreshRate = cAttr.getNumProp(CabbageIDs::guirefresh);

                        csdLine = "";


                        //set up stuff for tables
                        if(tokes[0].equalsIgnoreCase(String("table")))
                        {
                            for(int i=0; i<cAttr.getStringArrayProp(CabbageIDs::channel).size(); i++)
                                cAttr.addTableChannelValues();

                        }

                        //set up plant flag if needed for other widgets
                        if(cAttr.getStringProp(String("plant")).isNotEmpty())
                        {
                            plantFlag = cAttr.getStringProp(String("plant"));
                        }
                        else if(cAttr.getStringProp(String("reltoplant")).equalsIgnoreCase(String("")))
                            cAttr.setStringProp(String("reltoplant"), plantFlag);

                        //if an array of objects is being set up...
                        if((cAttr.getStringArrayProp(CabbageIDs::identchannelarray).size()>0) &&
                                (cAttr.getStringArrayProp(CabbageIDs::channelarray).size()>0))
                        {
                            for(int i=0; i<cAttr.getStringArrayProp(CabbageIDs::channelarray).size(); i++)
                            {
                                CabbageGUIType copy(cAttr);
                                copy.setStringProp(CabbageIDs::channel, cAttr.getStringArrayProp(CabbageIDs::channelarray).getReference(i));
                                copy.setStringProp(CabbageIDs::identchannel, cAttr.getStringArrayProp(CabbageIDs::identchannelarray).getReference(i));
                                guiLayoutCtrls.add(copy);
                                widgetTypes.add("layout");
                                guiID++;
                            }
                        }
                        else
                        {
                            guiLayoutCtrls.add(cAttr);
                            widgetTypes.add("layout");
                            guiID++;
                        }

                        if(cAttr.getStringProp("type").containsIgnoreCase("form"))
                            if(cAttr.getStringProp("text").length()>2)
                                setPluginName(cAttr.getStringProp("text"));
                            else if(cAttr.getStringProp("caption").length()>2)
                                setPluginName(cAttr.getStringProp("caption"));
                            else setPluginName("Untitled Cabbage Patch!");

                        sendChangeMessage();
                        if(tokes[0].equalsIgnoreCase(String("hostbpm"))
                                ||tokes[0].equalsIgnoreCase(String("hosttime"))
                                ||tokes[0].equalsIgnoreCase(String("hostplaying"))
                                ||tokes[0].equalsIgnoreCase(String("hostppqpos"))
                                ||tokes[0].equalsIgnoreCase(String("hostrecording")))
                            startTimer(20);
                    }
                //check if line of Cabbage code contains a valid widget and populate the guiCtrls vector
                    else if(ctrlsArray.contains(tokes[0]))
                    {
                        CabbageGUIType cAttr(csdLine.trimEnd(), guiID);
                        cAttr.setStringProp(CabbageIDs::parentdir, getCsoundInputFile().getParentDirectory().getFullPathName());
#ifdef AndroidBuild
                        cAttr.scaleWidget(scale);
#endif


                        
                        warningMessage = "";
                        warningMessage << "Line Number:" << csdLineNumber+1 << "\n" << cAttr.getWarningMessages();
                        if(cAttr.getWarningMessages().isNotEmpty())
                            csound->Message(warningMessage.toUTF8().getAddress());

                        cAttr.setNumProp(CabbageIDs::lineNumber, csdLineNumber+lineWhichCabbageSectionStarts);
                        csdLine = "";

                        //attach widget to plant if need be
                        if(cAttr.getStringProp(String("reltoplant")).equalsIgnoreCase(String("")))
                        {
                            cAttr.setStringProp(String("reltoplant"), plantFlag);
                            if(cAttr.getStringProp("preset").length()<1)
                                cAttr.setStringProp(String("preset"), presetFlag.trim());
                        }

                        //xypad contain two control paramters, one for x axis and another for y. As such we add two
                        //to our contorl vector so that plugin hosts display two sliders. We name one of the xypad pads
                        // 'dummy' so that our editor doesn't display it. Our editor only needs to show one xypad.
                        if(tokes[0].equalsIgnoreCase(String("xypad")))
                        {
                            cAttr.setStringProp(CabbageIDs::xychannel, String("X"));
                            cAttr.setNumProp(CabbageIDs::range,  cAttr.getNumProp(CabbageIDs::rangex));
                            cAttr.setNumProp(CabbageIDs::min,  cAttr.getNumProp(CabbageIDs::minx));
                            cAttr.setNumProp(CabbageIDs::max,  cAttr.getNumProp(CabbageIDs::maxx));
                            cAttr.setNumProp(CabbageIDs::value, cAttr.getNumProp(CabbageIDs::valuex));
                            cAttr.setStringProp(CabbageIDs::channel, cAttr.getStringProp(CabbageIDs::xchannel));
                            guiCtrls.add(cAttr);
                            widgetTypes.add("interactive");
                            cAttr.setStringProp(CabbageIDs::xychannel, String("Y"));
                            cAttr.setNumProp(CabbageIDs::range,  cAttr.getNumProp(CabbageIDs::rangey));
                            cAttr.setNumProp(CabbageIDs::min,  cAttr.getNumProp(CabbageIDs::miny));
                            cAttr.setNumProp(CabbageIDs::max,  cAttr.getNumProp(CabbageIDs::maxy));
                            cAttr.setNumProp(CabbageIDs::value, cAttr.getNumProp(CabbageIDs::valuey));
                            cAttr.setStringProp(CabbageIDs::channel, cAttr.getStringProp(CabbageIDs::ychannel));
                            //append 'dummy' to name so the editor know not to display the
                            //second xypad
                            cAttr.setStringProp("name", cAttr.getStringProp(CabbageIDs::name)+String("dummy"));
                            guiCtrls.add(cAttr);
                            widgetTypes.add("interactive");
                            //widgetTypes.add("interactive");
                            guiID++;
                            startTimer(10);
                        }
                        else
                        {
                            //if an array of objects is to be set up enter.....
                            if(cAttr.getStringArrayProp(CabbageIDs::channelarray).size()>0)
                            {
                                for(int i=0; i<cAttr.getStringArrayProp(CabbageIDs::channelarray).size(); i++)
                                {
                                    CabbageGUIType copy = cAttr;
                                    copy.setStringProp(CabbageIDs::channel, cAttr.getStringArrayProp(CabbageIDs::channelarray).getReference(i));
                                    copy.setStringProp(CabbageIDs::identchannel, cAttr.getStringArrayProp(CabbageIDs::identchannelarray).getReference(i));
                                    //Logger::writeToLog(cAttr.getStringArrayProp(CabbageIDs::channelarray).getReference(i));
                                    guiCtrls.add(copy);
                                    widgetTypes.add("interactive");
                                    guiID++;
                                }
                            }
                            else
                            {
                                guiCtrls.add(cAttr);
                               
                                widgetTypes.add("interactive");
                                guiID++;
                                if(tokes[0].equalsIgnoreCase(String("hslider2")) || tokes[0].equalsIgnoreCase(String("vslider2")))
                                {
                                    cAttr.setStringProp(CabbageIDs::channel, cAttr.getStringArrayPropValue(CabbageIDs::channel, 1));
                                    cAttr.setStringProp("name", cAttr.getStringProp(CabbageIDs::name)+String("dummy"));
                                    guiCtrls.add(cAttr);
                                    widgetTypes.add("interactive");
                                    guiID++;
                                }
                                else if(tokes[0].equalsIgnoreCase(String("hrange")) || tokes[0].equalsIgnoreCase(String("vrange")))
                                {
                                    cAttr.setStringProp(CabbageIDs::channel, cAttr.getStringArrayPropValue(CabbageIDs::channel, 1));
                                    cAttr.setStringProp("name", cAttr.getStringProp(CabbageIDs::name)+String("dummy"));
                                    guiCtrls.add(cAttr);
                                    widgetTypes.add("interactive");
                                    guiID++;
                                }
                            }

                        }

                        //debugMessageArray.addArray(logGUIAttributes(cAttr, String("Interactive")));
                        //sendChangeMessage();
                    }

            }
        }
        else break;
    } //end of scan through entire csd text, control vectors are now populated

}

//===========================================================================================
// graphing functions...
//===========================================================================================

void CabbagePluginAudioProcessor::makeGraphCallback(CSOUND *csound, WINDAT *windat, const char * /*name*/)
{
    CabbagePluginAudioProcessor *ud = (CabbagePluginAudioProcessor *) csoundGetHostData(csound);
    ud->windowIDs.addIfNotAlreadyThere(windat->windid);
    cUtils::debug("tableMin:"+String(windat->min));
    cUtils::debug("tableMa:"+String(windat->max));

    fftDisplay* fft = new fftDisplay(String(windat->caption),windat->windid, windat->oabsmax, windat->min, windat->max, windat->npts);

    bool addFFT = true;
    for(int i=0; i<ud->fftArrays.size(); i++)
    {
        if(ud->fftArrays[i]->windid==windat->windid)
            addFFT = false;
    }

    if(addFFT)
        ud->fftArrays.add(fft);

    //cUtils::debug(String(ud->fftArrays.size())+":"+String(windat->windid));
}

void CabbagePluginAudioProcessor::drawGraphCallback(CSOUND *csound, WINDAT *windat)
{
    CabbagePluginAudioProcessor *ud = (CabbagePluginAudioProcessor *) csoundGetHostData(csound);
    Array<float, CriticalSection> tablePoints = Array<float, CriticalSection>(&windat->fdata[0], windat->npts);
    ud->fftArrays.getUnchecked(windat->windid)->setPoints(tablePoints);
    ud->updateFFTDisplay = true;
}

void CabbagePluginAudioProcessor::killGraphCallback(CSOUND *csound, WINDAT *windat)
{
    CabbagePluginAudioProcessor *udata = (CabbagePluginAudioProcessor *) csoundGetHostData(csound);
    cUtils::debug("killGraphCallback");
}

int CabbagePluginAudioProcessor::exitGraphCallback(CSOUND *csound)
{
    CabbagePluginAudioProcessor *udata = (CabbagePluginAudioProcessor *) csoundGetHostData(csound);
    cUtils::debug("exitGraphCallback");
    return 0;
}

//===========================================================================================
// Create GUI. Must only be called after the widget abstractions have been created
//===========================================================================================
void CabbagePluginAudioProcessor::addWidgetsToEditor(bool refresh)
{
    if(this->createEditorIfNeeded())
    {
        CabbagePluginAudioProcessorEditor* editor = dynamic_cast<CabbagePluginAudioProcessorEditor*>(this->getActiveEditor());

        if(refresh)
        {
            editor->comps.clear();
            editor->layoutComps.clear();
            editor->repaint();
            //((CabbagePluginAudioProcessorEditor*)getActiveEditor())->setEditMode(false);
            //editor->setEditMode(false);
        }

        int layoutCtrlIndex=indexOfLastLayoutCtrl;
        int interactiveCtrlIndex=indexOfLastGUICtrl;

        //this ensures that widgets get added in order they appear
        //in text
        for(int i=0; i<getWidgetTypes().size(); i++)
        {
            if(getWidgetTypes()[i]=="layout")
            {
                editor->InsertGUIControls(getGUILayoutCtrls(layoutCtrlIndex));
                layoutCtrlIndex++;
            }
            else //interactive
            {
                editor->InsertGUIControls(getGUICtrls(interactiveCtrlIndex));
                interactiveCtrlIndex++;
            }
        }
//
//        for(int i=indexOfLastLayoutCtrl; i<guiLayoutCtrls.size(); i++)
//            editor->InsertGUIControls(guiLayoutCtrls[i]);
//        for(int i=indexOfLastGUICtrl; i<guiCtrls.size(); i++)
//            editor->InsertGUIControls(guiCtrls[i]);
#if defined(Cabbage_Build_Standalone) || defined(CABBAGE_HOST)
        if(!getPreference(appProperties, "ExternalEditor") && refresh)
            editor->setEditMode(isGuiEnabled());
#endif
    }
}


//===========================================================
// SHOW SOURCE EDITOR
//===========================================================
void CabbagePluginAudioProcessor::createAndShowSourceEditor(LookAndFeel* looky)
{
#if !defined(Cabbage_Build_Standalone) && !defined(CABBAGE_HOST) && !defined(AndroidBuild)
    if(!cabbageCsoundEditor)
    {
        cabbageCsoundEditor = new CodeWindow(csdFile.getFullPathName());
        cabbageCsoundEditor->setVisible(true);
        cabbageCsoundEditor->toFront(true);
        //setupWindowDimensions();
        cabbageCsoundEditor->addActionListener(this);
        cabbageCsoundEditor->setLookAndFeel(looky);
        //codeEditor->sendActionMessage("open file");
        cabbageCsoundEditor->textEditor->editor[0]->loadContent(csdFile.loadFileAsString());
        codeEditor = cabbageCsoundEditor->textEditor;

    }
#endif
}

void CabbagePluginAudioProcessor::openFile(LookAndFeel* looky)
{
    
    
    WildcardFileFilter wildcardFilter = WildcardFileFilter("*.csd", "", "File filter");
    Array<File> selectedFiles = cUtils::launchFileBrowser("Open a file",
                                wildcardFilter,
                                "*.csd",
                                1,
                                File("").getParentDirectory(),
                                true,
                                looky);

    if(selectedFiles.size())
    {
        csdFile = selectedFiles[0];
        stopProcessing = true;
        getCallbackLock().enter();
        //cabbageCsoundEditor->setText(csdFile.loadFileAsString(), csdFile.getFullPathName());
        initialiseWidgets(selectedFiles[0].loadFileAsString(), true);
        addWidgetsToEditor(true);
        getCallbackLock().exit();
        recompileCsound(csdFile);
    }
}
void CabbagePluginAudioProcessor::actionListenerCallback (const String& message)
{

#if !defined(Cabbage_Build_Standalone) && !defined(CABBAGE_HOST) && !defined(AndroidBuild)

    if(message=="open file")
    {
        WildcardFileFilter wildcardFilter = WildcardFileFilter("*.csd", "", "File filter");
        Array<File> selectedFiles = cUtils::launchFileBrowser("Open a file",
                                    wildcardFilter,
                                    "*.csd",
                                    1,
                                    File("").getParentDirectory(),
                                    false,
                                    &cabbageCsoundEditor->getLookAndFeel());

        if(selectedFiles.size())
        {
            csdFile = selectedFiles[0];
            cabbageCsoundEditor->setText(csdFile.loadFileAsString(), csdFile.getFullPathName());
            initialiseWidgets(csdFile.loadFileAsString(), true);
            addWidgetsToEditor(true);
            recompileCsound(csdFile);
        }
    }

    if(message=="closing editor")
    {
        delete cabbageCsoundEditor;
        cabbageCsoundEditor = nullptr;
    }

    if(message == "GUI Update, PropsPanel")
    {
        //if something changes in the properties panel we need to update our GUI so
        //that the changes are reflected in the on screen components
        initialiseWidgets(csdFile.loadFileAsString(), true);
        addWidgetsToEditor(true);


        if(cabbageCsoundEditor)
        {
            //cabbageCsoundEditor->csoundDoc.replaceAllContent(filter->getCsoundInputFile().loadFileAsString());
            cabbageCsoundEditor->textEditor->highlightLine(getCurrentLineText());
        }
    }

    else if(message == "Score Updated")
    {
        if(cabbageCsoundEditor)
        {
            //cabbageCsoundEditor->csoundDoc.replaceAllContent(filter->getCsoundInputFile().loadFileAsString());
            cabbageCsoundEditor->textEditor->highlightLine(getCurrentLineText());
        }
    }

    else if(message == "GUI Updated, controls added, resized")
    {
        //no need to update our entire GUI here as only on-screen sizes change
        setCurrentLine(getCurrentLine()+1);
        if(cabbageCsoundEditor)
        {
            //cabbageCsoundEditor->csoundDoc.replaceAllContent(filter->getCsoundInputFile().loadFileAsString());
            cabbageCsoundEditor->textEditor->highlightLine(getCurrentLineText());
        }
    }

    else if(message == "GUI Updated, controls deleted")
    {
        //controls have already been deleted, we just need to update our file
        if(cabbageCsoundEditor)
            cabbageCsoundEditor->csoundDoc.replaceAllContent(csdFile.loadFileAsString());
    }

    else if(message.equalsIgnoreCase("fileSaved"))
    {
        //saveFile();
        if(csdFile.hasWriteAccess())
        {
            csdFile.replaceWithText(cabbageCsoundEditor->getText());
        }

        initialiseWidgets(csdFile.loadFileAsString(), true);
        addWidgetsToEditor(true);
        recompileCsound(csdFile);

        if(isGuiEnabled())
        {
            startTimer(100);
            ((CabbagePluginAudioProcessorEditor*)getActiveEditor())->setEditMode(false);
            setGuiEnabled(false);
        }
    }


    else if(message.contains("ContinueDebug"))
    {
        continueCsoundDebug();
    }

    else if(message.contains("RemoveAllBreakpoints"))
    {
        cleanCsoundDebug();
    }

    else if(message.contains("NextDebug"))
    {
        nextCsoundDebug();
    }

    else if(message.contains("InstrumentBreakpoint"))
    {
        if(message.contains("Set"))
        {
            int instrNumber = message.substring(24).getIntValue();
            setCsoundInstrumentBreakpoint(instrNumber, 0);
        }
        else
        {
            int instrNumber = message.substring(27).getIntValue();
            removeCsoundInstrumentBreakpoint(instrNumber);

        }
    }

    else if(message.contains("fileUpdateGUI"))
    {
        initialiseWidgets(cabbageCsoundEditor->getText(), true);
        addWidgetsToEditor(true);
        csdFile.replaceWithText(cabbageCsoundEditor->getText());
        if(cabbageCsoundEditor)
        {
            cabbageCsoundEditor->setName(csdFile.getFullPathName());
        }
    }

    else if(message.contains("MENU COMMAND: toggle edit"))
    {
        if(isGuiEnabled())
        {
            ((CabbagePluginAudioProcessorEditor*)getActiveEditor())->setEditMode(false);
            setGuiEnabled(false);
            //filter->suspendProcessing(false);
        }
        else
        {
            ((CabbagePluginAudioProcessorEditor*)getActiveEditor())->setEditMode(true);
            setGuiEnabled(true);
            //filter->suspendProcessing(true);

            //stopTimer();
            //setPreference(appProperties, "ExternalEditor", 0);
        }
    }

#endif
}

//============================================================================
//dynamically remove components from editor window, used in EDIT mode
//============================================================================
void CabbagePluginAudioProcessor::removeGUIComponent(int index, String type)
{
    //remove component struct from our abstract vector
    CabbagePluginAudioProcessorEditor* editor = dynamic_cast<CabbagePluginAudioProcessorEditor*>(this->getActiveEditor());

    if(type=="interactive")
    {
        //remove GUI abstract structure from vector
        guiCtrls.remove(index);
    }
    else
    {
        //remove GUI abstract structure from vector
        guiLayoutCtrls.remove(index);
    }

    editor->updateLayoutEditorFrames();
    editor->repaint();
}

//============================================================================
// start/stop recording of file
//============================================================================
void CabbagePluginAudioProcessor::startRecording ()
{
    if(!isWinXP)
        if (sampleRate > 0)
        {
            // Create an OutputStream to write to our destination file...
            tempAudioFile.deleteFile();
            backgroundThread.startThread();
            ScopedPointer<FileOutputStream> fileStream (tempAudioFile.createOutputStream());

            if (fileStream != nullptr)
            {
                // Now create a WAV writer object that writes to our output stream...
                WavAudioFormat wavFormat;
                AudioFormatWriter* writer = wavFormat.createWriterFor(fileStream, sampleRate, 2, 16, StringPairArray(), 0);

                if (writer != nullptr)
                {
                    fileStream.release(); // (passes responsibility for deleting the stream to the writer object that is now using it)

                    // Now we'll create one of these helper objects which will act as a FIFO buffer, and will
                    // write the data to disk on our background thread.
                    threadedWriter = new AudioFormatWriter::ThreadedWriter (writer, backgroundThread, 32768);

                    // Reset our recording thumbnail
                    //thumbnail.reset (writer->getNumChannels(), writer->getSampleRate());
                    nextSampleNum = 0;

                    // And now, swap over our active writer pointer so that the audio callback will start using it..
                    const ScopedLock sl (writerLock);
                    activeWriter = threadedWriter;
                }
            }
        }
}

void CabbagePluginAudioProcessor::stopRecording()
{
    if(!isWinXP)
    {
        // First, clear this pointer to stop the audio callback from using our writer object..
        {
            const ScopedLock sl (writerLock);
            activeWriter = nullptr;
        }

        threadedWriter = nullptr;

        // Now we can delete the writer object. It's done in this order because the deletion could
        // take a little time while remaining data gets flushed to disk, so it's best to avoid blocking
        // the audio callback while this happens.
#if !defined(AndroidBuild)
        FileChooser fc("Save", tempAudioFile.getFullPathName(), "*.wav", UseNativeDialogue);
        //determine whether to poen file or directory
        if(fc.browseForFileToSave(true))
        {
            File selectedFile = fc.getResult();
            tempAudioFile.moveFileTo(selectedFile);
        }
#endif
    }
}


String CabbagePluginAudioProcessor::getCsoundOutput()
{
    const int messageCnt = csound->GetMessageCnt();

    if(messageCnt==0)
        return csoundOutput;

    for(int i=0; i<messageCnt; i++)
    {
        csoundOutput+=csound->GetFirstMessage();
        csound->PopFirstMessage();
    }

    return csoundOutput;
}

//===========================================================
// CALLBACKS FOR STANDALONE
//===========================================================
//#ifndef Cabbage_No_Csound
//void CabbagePluginAudioProcessor::messageCallback(CSOUND* csound, int /*attr*/,  const char* fmt, va_list args)
//{
//    if(fmt)
//    {
//        CabbagePluginAudioProcessor* ud = (CabbagePluginAudioProcessor *) csoundGetHostData(csound);
//        char msg[MAX_BUFFER_SIZE];
//        vsnprintf(msg, MAX_BUFFER_SIZE, fmt, args);
//
//        ud->debugMessage += String(msg); //We have to append the incoming msg
//		//const MessageManagerLock mmLock;
//        ud->csoundOutput += ud->debugMessage;
//        ud->debugMessageArray.add(ud->debugMessage);
//        if(ud->createLog)
//            Logger::writeToLog(String(msg).trim());
//        ud->sendChangeMessage();
//        ud->debugMessage = "";
//        ud = nullptr;
//
//    }
//}
//#endif

#if defined(BUILD_DEBUGGER) && !defined(Cabbage_No_Csound)

void CabbagePluginAudioProcessor::breakpointCallback(CSOUND *csound, debug_bkpt_info_t *bkpt_info, void *userdata)
{
#ifdef BUILD_DEBUGGER
    CabbagePluginAudioProcessor* ud = (CabbagePluginAudioProcessor *) userdata;
    ud->getCallbackLock().enter();
    int ksmpOffset = ud->ksmpsOffset;;

    //code based on Andres implementation in CsoundQT...

    debug_instr_t *debug_instr = bkpt_info->breakpointInstr;


    // Copy variable list
    debug_variable_t* vp = bkpt_info->instrVarList;


    if(ud->breakCount==0)
        ud->breakCount= debug_instr->kcounter;
    else ud->breakCount++;
    String output;

    output << "\nBreakpoint at instr " << debug_instr->p1 << "\tNumber of k-cycles into performance: " << ud->breakCount << "\n------------------------------------------------------";;

    while (vp)
    {
        output << " \n";
        if (vp->name[0] != '#')
        {
            output << " VarName:"<< vp->name << "\t";;
            if (strcmp(vp->typeName, "i") == 0 || strcmp(vp->typeName, "k") == 0)
            {
                output << " value=" << *((MYFLT *) vp->data);
            }
            else if(strcmp(vp->typeName, "S") == 0)
            {
                output << " value=" << (char*)(vp->data);
            }
            else if (strcmp(vp->typeName, "a") == 0)
            {
                MYFLT *data = (MYFLT *) vp->data;
                output << " value="<< *data;// << *(data + 1) << *(data + 2)<< *(data + 3);

            }
            output << " varType[" << vp->typeName << "]";

        }
        vp = vp->next;
    }

    ud->csoundDebuggerOutput = output;
    csoundDebugFreeVariables(csound, vp);
    //csoundDebugFreeInstrInstances(csound, debug_instr);
    ud->getCallbackLock().exit();

#endif
}
#endif
//==============================================================================
#if defined(Cabbage_Build_Standalone) || defined(CABBAGE_HOST)
CabbagePluginAudioProcessor* JUCE_CALLTYPE createCabbagePluginFilter(String inputfile, bool guiOnOff, int pluginType)
{
    return new CabbagePluginAudioProcessor(inputfile, false, pluginType);
}
#elif defined(AndroidBuild)
AudioProcessor* JUCE_CALLTYPE createPluginFilter(String file, Point<float> scale)
{
    return new CabbagePluginAudioProcessor(file, scale);
}
#else
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CabbagePluginAudioProcessor();
}
#endif

//==========================================================================
//action listener. Listen to messages being sent form xypad automations
//==========================================================================
void CabbagePluginAudioProcessor::changeListenerCallback(ChangeBroadcaster *source)
{
    float xVal, yVal;
    //is message coming from an xypad
    XYPadAutomation* xyPad = dynamic_cast< XYPadAutomation*>(source);
    if(xyPad)
    {
#ifdef Cabbage_Build_Standalone
        setParameterNotifyingHost(xyPad->paramIndex, xyPad->getXValue());
        setParameterNotifyingHost(xyPad->paramIndex+1, xyPad->getYValue());
        sendChangeMessage();
#else
        if(xyPad->getMinimumXValue()>=0)
            xVal = (xyPad->getXValue()/xyPad->getXRange())+(fabs(xyPad->getMinimumXValue())/xyPad->getXRange());
        else
            xVal = (xyPad->getXValue()/xyPad->getXRange())-(fabs(xyPad->getMinimumXValue())/xyPad->getXRange());

        if(xyPad->getMinimumYValue()<=0)
            yVal = (xyPad->getYValue()/xyPad->getYRange())+(fabs(xyPad->getMinimumYValue())/xyPad->getYRange());
        else
            yVal = (xyPad->getYValue()/xyPad->getYRange())-(fabs(xyPad->getMinimumYValue())/xyPad->getYRange());

        setParameterNotifyingHost(xyPad->paramIndex, xVal);
        setParameterNotifyingHost(xyPad->paramIndex+1, yVal);
        sendChangeMessage();
#endif
    }


}

//==============================================================================
// getTable data from Csound so table editor can draw table
//==============================================================================
StringArray CabbagePluginAudioProcessor::getTableStatement(int tableNum)
{
    StringArray fdata;
    fdata.add(String::empty);
    if(csCompileResult==OK)
    {
        MYFLT* argsPtr, *temp;
        int noOfArgs = csoundGetTableArgs(csound->GetCsound(), &argsPtr, tableNum);
        if(noOfArgs!=-1)
        {
            int tableSize = csound->GetTable(temp, tableNum);
            fdata.add(String(tableNum));
            fdata.add("0");
            fdata.add(String(tableSize));
            if(noOfArgs==0)
                fdata.add(String(1));
            else
                for(int i=0; i<noOfArgs; i++)
                {
                    fdata.add(String(argsPtr[i]));
                }
        }
    }
    return fdata;
}
//==============================================================================
fftDisplay* CabbagePluginAudioProcessor::getFFTTable(int tableNum)
{
    if(fftArrays[tableNum])
        return fftArrays[tableNum];
    else
        return new fftDisplay("", -1, 0, 0, 0, 0);
}


const Array<float, CriticalSection> CabbagePluginAudioProcessor::getTableFloats(int tableNum)
{
    Array<float, CriticalSection> points;
    if(csCompileResult==OK)
    {
        points.clear();

        int tableSize=0;
#ifndef Cabbage_No_Csound

        tableSize = csound->TableLength(tableNum);
        temp.clear();
        //not good if table size is -1!
        if(tableSize<0)
            return points;

        temp.reserve(tableSize);
        csound->TableCopyOut(tableNum, &temp[0]);
#else
        float *temp;
#endif
        if(tableSize>0)
            points = Array<float, CriticalSection>(&temp[0], tableSize);
    }
    return points;
}

int CabbagePluginAudioProcessor::checkTable(int tableNum)
{
#ifndef Cabbage_No_Csound
    return  csound->TableLength(tableNum);
#else
    return -1;
#endif
}

//=================================================================================
// Get and Set Parameter methods, called by our editor, and the plugin host...
//=================================================================================
float CabbagePluginAudioProcessor::getParameter (int index)
{
    if(isPositiveAndBelow(index, getGUICtrlsSize()))
    {
        float range = getGUICtrls(index).getNumProp(CabbageIDs::range);
        float min = getGUICtrls(index).getNumProp(CabbageIDs::min);
        //Logger::writeToLog("parameterGet-"+String(index)+String("-Min:")+String(min)+" Range:"+String(range)+ " Val:"+String(getGUICtrls(index).getNumProp(CabbageIDs::value)));
        //Logger::writeToLog("parameterGet:"+String(index)+String(":")+String(guiCtrls[index].getNumProp(CabbageIDs::value)));

        // this gets called at any time by our host or out GUI editor
        if(index<(int)guiCtrls.size()) //make sure index isn't out of range
        {
#ifndef Cabbage_Build_Standalone
            float val = (getGUICtrls(index).getNumProp(CabbageIDs::value)/range)-(min/range);
            if(getGUICtrls(index).getStringProp(CabbageIDs::type)==CabbageIDs::combobox)
            {
                val = jmap(getGUICtrls(index).getNumProp(CabbageIDs::value), 0.f, getGUICtrls(index).getNumProp(CabbageIDs::comborange), 0.f, 1.f);
                return val;
            }
            else if(getGUICtrls(index).getStringProp(CabbageIDs::type)==CabbageIDs::checkbox ||
                    getGUICtrls(index).getStringProp(CabbageIDs::type)==CabbageIDs::button)
                return getGUICtrls(index).getNumProp(CabbageIDs::value);
            else
                return (getGUICtrls(index).getNumProp(CabbageIDs::value)/range)-(min/range);
#else
            return guiCtrls[index].getNumProp(CabbageIDs::value);
#endif
        }
        else
            return 0.0f;
    }
    else
    {
        cUtils::debug("getParameter(): Parameter index out of bounds");
        return 0.0f;
    }
}
//-------------------------------------------------------------------------------------------
void CabbagePluginAudioProcessor::setParameter (int index, float newValue)
{
    if(isPositiveAndBelow(index, getGUICtrlsSize()))
    {
        String stringMessage;
#ifndef Cabbage_No_Csound
        float range, min, comboRange;
        //add index of control that was changed to dirty control vector, unless it's a combobox.
#ifdef Cabbage_Build_Standalone
        if(!getGUICtrls(index).getStringProp("filetype").contains("snaps"))
            dirtyControls.addIfNotAlreadyThere(index);
#else

        dirtyControls.addIfNotAlreadyThere(index);
#endif

        if(index<(int)guiCtrls.size())//make sure index isn't out of range
        {
#ifndef Cabbage_Build_Standalone
            //scaling in here because incoming values in plugin mode range from 0-1
            range = getGUICtrls(index).getNumProp(CabbageIDs::range);
            comboRange = getGUICtrls(index).getNumProp(CabbageIDs::comborange);
            min = getGUICtrls(index).getNumProp(CabbageIDs::min);

            if(getGUICtrls(index).getStringProp(CabbageIDs::type)==CabbageIDs::xypad)
                newValue = (jmax(0.f, newValue)*range)+min;
            else if(getGUICtrls(index).getStringProp(CabbageIDs::type)==CabbageIDs::combobox)//combo box value need to be rounded...
                newValue = jmap(newValue, 0.f, 1.f, 0.f, comboRange);
            else if(getGUICtrls(index).getStringProp(CabbageIDs::type)==CabbageIDs::checkbox ||
                    getGUICtrls(index).getStringProp(CabbageIDs::type)==CabbageIDs::button)
                range=1;
            else
                newValue = (newValue*range)+min;



#endif
            if(getGUICtrls(index).getStringProp(CabbageIDs::type)==CabbageIDs::combobox &&
                    getGUICtrls(index).getStringProp(CabbageIDs::channeltype)==CabbageIDs::stringchannel)
            {
                stringMessage = getGUICtrls(index).getStringArrayPropValue(CabbageIDs::text, newValue-1);
                messageQueue.addOutgoingChannelMessageToQueue(guiCtrls.getReference(index).getStringProp(CabbageIDs::channel),
                        stringMessage, CabbageIDs::stringchannel);
            }
            else
            {
                messageQueue.addOutgoingChannelMessageToQueue(guiCtrls.getReference(index).getStringProp(CabbageIDs::channel),
                        newValue, guiCtrls.getReference(index).getStringProp(CabbageIDs::type));

            }
            //guiCtrls.getReference(index).setNumProp(CabbageIDs::value, newValue);
        }
#endif
    }
    //updateCabbageControls();
}

//==============================================================================
//this method gets called after a performKsmps() to update our GUI controls
//with messages from Csound. For instance, a user might wish to change the value
//of a GUI slider from Csound by using a chnset opcode. The speed at which this is
//updated can be teaked, so as not to hog resources...
void CabbagePluginAudioProcessor::updateCabbageControls()
{
#ifndef Cabbage_No_Csound
    bool shouldUpdate = false;
    String channelMessage;
    if(csCompileResult==OK)
    {
        //update all control widgets
        const int guiCtrls_count = guiCtrls.size();
        for(int index=0; index<guiCtrls_count; ++index)
        {
            CabbageGUIType &guiCtrl = guiCtrls.getReference(index);
            if(guiCtrl.getStringProp(CabbageIDs::channeltype).equalsIgnoreCase(CabbageIDs::stringchannel) && guiCtrl.getStringProp(CabbageIDs::type)=="combobox")
            {
                csound->GetStringChannel(guiCtrl.getStringProp(CabbageIDs::channel).toUTF8().getAddress(), tmp_string);
                channelMessage = String(tmp_string);
                if(channelMessage!=guiCtrl.getStringProp(CabbageIDs::currenttext))
                {
                    guiCtrl.setStringProp(CabbageIDs::currenttext, channelMessage);
                    StringArray items = guiCtrl.getStringArrayProp(CabbageIDs::text);
                    int itemIndex = items.indexOf(channelMessage);
                    guiCtrl.setNumProp(CabbageIDs::value, itemIndex+1);
                    shouldUpdate = true;
                }
            }
            else
            {
                float value = csound->GetChannel(guiCtrl.getStringProp(CabbageIDs::channel).getCharPointer());
                //cUtils::debug(guiCtrl.getStringProp(CabbageIDs::channel));
                if(value!=guiCtrl.getNumProp(CabbageIDs::value))
                {
                    //Logger::writeToLog("Channel:"+guiCtrls[index].getStringProp(CabbageIDs::channel));
                    //Logger::writeToLog("value:"+String(value));
                    guiCtrl.setNumProp(CabbageIDs::value, value);
                    dirtyControls.addIfNotAlreadyThere(index);
                    shouldUpdate = true;
                }
            }

            if(guiCtrl.getStringProp(CabbageIDs::identchannel).isNotEmpty())
            {
                //if controls has an identifier channel send data from Csound to control
                csound->GetStringChannel(guiCtrl.getStringProp(CabbageIDs::identchannel).toUTF8().getAddress(), tmp_string);
                channelMessage = String(tmp_string);
                if(channelMessage.isNotEmpty())
                {
                    guiCtrl.setStringProp(CabbageIDs::identchannelmessage, channelMessage.trim());
                    guiCtrl.parse(guiCtrl.getStringProp(CabbageIDs::type)+" "+channelMessage, "");
                    dirtyControls.addIfNotAlreadyThere(index);
                    shouldUpdate = true;
                }
                //else
                //guiCtrls.getReference(index).setStringProp(CabbageIDs::identchannelmessage, "");
                //zero channel message so that we don't keep sending the same string
                csound->SetChannel(guiCtrl.getStringProp(CabbageIDs::identchannel).toUTF8().getAddress(), "");
            }
        }

        //update all layout control widgets
        //currently this is only needed for table widgets as other layout controls
        //don't use channel messages...
        const int guiLayoutCtrls_count = getGUILayoutCtrlsSize();
        for(int index=0; index<guiLayoutCtrls_count; ++index)
        {
            CabbageGUIType &guiLayoutCtrl = guiLayoutCtrls.getReference(index);
            if(guiLayoutCtrl.getStringProp(CabbageIDs::type)==CabbageIDs::table)
            {
                for(int y=0; y<guiLayoutCtrl.getStringArrayProp(CabbageIDs::channel).size(); ++y)
                {
                    //String test = getGUILayoutCtrls(index).getStringArrayPropValue(CabbageIDs::channel, y);
                    float value = csound->GetChannel(guiLayoutCtrl.getStringArrayPropValue(CabbageIDs::channel, y).getCharPointer());
                    guiLayoutCtrl.setTableChannelValues(y, value);
                    shouldUpdate=true;
                }
            }

            if(guiLayoutCtrl.getStringProp(CabbageIDs::identchannel).isNotEmpty())
            {
                //guiLayoutCtrls[index].getStringProp(CabbageIDs::identchannel).toUTF8().getAddress()
                csound->GetStringChannel(guiLayoutCtrl.getStringProp(CabbageIDs::identchannel).toUTF8().getAddress(), tmp_string);
                channelMessage = String(tmp_string);
                //Logger::writeToLog(guiLayoutCtrls[index].getStringProp(CabbageIDs::identchannel));
                if(channelMessage.isNotEmpty())
                {

                    guiLayoutCtrl.parse(guiLayoutCtrl.getStringProp(CabbageIDs::type)+" "+channelMessage, channelMessage);
                    //cUtils::debug(channelMessage);
                    guiLayoutCtrl.setStringProp(CabbageIDs::identchannelmessage,channelMessage.trim());
                    shouldUpdate=true;
                }
                //else
                //	guiLayoutCtrls.getReference(index).setStringProp(CabbageIDs::identchannelmessage, "");
                //zero channel message so that we don't keep sending the same string
                csound->SetChannel(guiLayoutCtrl.getStringProp(CabbageIDs::identchannel).toUTF8().getAddress(), "");
            }
        }
        if(shouldUpdate || shouldUpdateFFTDisplay())
            sendChangeMessage();
    }
#endif
}


//==============================================================================
//this method only gets called when it's safe to do so, i.e., between calls to performKsmps()
//this method sends any channel messages that are in the queue to from Cabbage to Csound
void CabbagePluginAudioProcessor::sendOutgoingMessagesToCsound()
{
#ifndef Cabbage_No_Csound
    if(csCompileResult==OK)
    {
#ifndef Cabbage_Build_Standalone
        if (getPlayHead() != 0 && getPlayHead()->getCurrentPosition (hostInfo))
        {
            csound->SetChannel(CabbageIDs::hostbpm.toUTF8(), hostInfo.bpm);
            csound->SetChannel(CabbageIDs::timeinseconds.toUTF8(), hostInfo.timeInSeconds);
            csound->SetChannel(CabbageIDs::isplaying.toUTF8(), hostInfo.isPlaying);
            csound->SetChannel(CabbageIDs::isrecording.toUTF8(), hostInfo.isRecording);
            csound->SetChannel(CabbageIDs::hostppqpos.toUTF8(), hostInfo.ppqPosition);
            csound->SetChannel(CabbageIDs::timeinsamples.toUTF8(), hostInfo.timeInSamples);
            csound->SetChannel(CabbageIDs::timeSigDenom.toUTF8(), hostInfo.timeSigDenominator);
            csound->SetChannel(CabbageIDs::timeSigNum.toUTF8(), hostInfo.timeSigNumerator);

        }
#endif
        const int message_count = messageQueue.getNumberOfOutgoingChannelMessagesInQueue();
        const int scoreEvents_count = scoreEvents.size();
        for(int i=0; i<message_count; i++)
        {
            const CabbageChannelMessage &message = messageQueue.getOutgoingChannelMessageFromQueue(i);
            //Logger::writeToLog("MessageType:"+messageQueue.getOutgoingChannelMessageFromQueue(i).type);
            if(message.type=="directoryList")
            {
                for(int y=0; y<scoreEvents_count; ++y)
                    csound->InputMessage(scoreEvents[y].toUTF8());
                //scoreEvents.clear();
            }
            //update Csound function tables with values from table widget
            else if(message.type=="updateTable")
            {
                //Logger::writeToLog(messageQueue.getOutgoingChannelMessageFromQueue(i).fStatement.toUTF8());
                csound->InputMessage(message.fStatement.getCharPointer());
            }
            //catch string messags
            else if(message.type==CabbageIDs::stringchannel)
            {
                //Logger::writeToLog(messageQueue.getOutgoingChannelMessageFromQueue(i).channelName);
                //Logger::writeToLog(messageQueue.getOutgoingChannelMessageFromQueue(i).stringVal);
                csound->SetChannel(message.channelName.getCharPointer(),
                                   message.stringVal.toUTF8().getAddress());
            }
            else
                csound->SetChannel(message.channelName.getCharPointer(),
                                   message.value);
        }

        messageQueue.flushOutgoingChannelMessages();
    }

#endif
}


//========================================================================
// Standard plugin methods, getName, getNumParameters, setParamterName, get ProgramName, etc....
//==============================================================================
const String CabbagePluginAudioProcessor::getName() const
{
    return pluginName;
}

int CabbagePluginAudioProcessor::getNumParameters()
{
    return guiCtrls.size();
}

const String CabbagePluginAudioProcessor::getParameterName (int index)
{
    if(index<(int)guiCtrls.size())//make sure index isn't out of range
        return guiCtrls.getReference(index).getStringProp(CabbageIDs::channel);
    else return String::empty;
}

const String CabbagePluginAudioProcessor::getParameterText (int index)
{
    if(index<(int)guiCtrls.size())//make sure index isn't out of range
        return String (guiCtrls.getReference(index).getNumProp(CabbageIDs::value), 2);
    else return String::empty;
}

const String CabbagePluginAudioProcessor::getInputChannelName (int channelIndex) const
{
    if(channelIndex<(int)guiCtrls.size())//make sure index isn't out of range
        return String (channelIndex + 1);
    else return String::empty;
}

const String CabbagePluginAudioProcessor::getOutputChannelName (int channelIndex) const
{
    if(channelIndex<(int)guiCtrls.size())//make sure index isn't out of range
        return String (channelIndex + 1);
    else return String::empty;
}

bool CabbagePluginAudioProcessor::isInputChannelStereoPair (int /*index*/) const
{
    return true;
}

bool CabbagePluginAudioProcessor::isOutputChannelStereoPair (int /*index*/) const
{
    return true;
}

bool CabbagePluginAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool CabbagePluginAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

void CabbagePluginAudioProcessor::setGuiEnabled(bool val)
{
#if (defined(Cabbage_Build_Standalone) || defined(CABBAGE_HOST)) && !defined(AndroidBuild)
    guiON = val;
    CabbagePluginAudioProcessorEditor* editor = dynamic_cast< CabbagePluginAudioProcessorEditor*>(this->getActiveEditor());
    if(editor)
    {
#if !defined(CABBAGE_HOST)
        if(val==false)
            if(getPreference(appProperties, "ExternalEditor"))
                csdFile.replaceWithText(codeEditor->getAllText());
#endif

    }
#endif
}

int CabbagePluginAudioProcessor::getNumPrograms()
{
    //seems that we need to report at least one program for some hosts to work properly
    return 1;
}

int CabbagePluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void CabbagePluginAudioProcessor::setCurrentProgram (int /*index*/)
{
}

const String CabbagePluginAudioProcessor::getProgramName (int /*index*/)
{
    return String::empty;
}

void CabbagePluginAudioProcessor::changeProgramName (int /*index*/, const String& /*newName*/)
{
}

//==============================================================================
void CabbagePluginAudioProcessor::prepareToPlay (double sampRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    //showMessage(String(this->getNumOutputChannels()));
    keyboardState.reset();
    sampleRate = sampRate;
}
//==============================================================================
void CabbagePluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    keyboardState.reset();
}
//==============================================================================
//this following callback only runs in plugin mode, and only when one of the
//host widgets are being used
void CabbagePluginAudioProcessor::timerCallback()
{
#ifndef Cabbage_No_Csound
    for(int y=0; y<xyAutomation.size(); y++)
    {
        if(xyAutomation[y])
            xyAutomation[y]->update();
    }
#endif
}

//==============================================================================
void CabbagePluginAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    float** audioBuffers = buffer.getArrayOfWritePointers();
    const int numSamples = buffer.getNumSamples();

	int output_channel_count = getNumOutputChannels();

    float samp;

    if(stopProcessing || isGuiEnabled())
    {
        buffer.clear();
    }
    else
    {
#ifndef Cabbage_No_Csound
        //if no inputs are used clear buffer in case it's not empty..
        if(getNumInputChannels()==0)
            buffer.clear();

        if(csCompileResult==OK)
        {
            if(isBypassed==true)
                return;



            keyboardState.processNextMidiBuffer (midiMessages, 0, numSamples, true);
            midiBuffer = midiMessages;
            //midiBuffer.clear();

#if JucePlugin_ProducesMidiOutput
            if(!midiOutputBuffer.isEmpty())
            {
                midiMessages.swapWith(midiOutputBuffer);
            }
            else
                midiMessages.clear();
#endif

            const CriticalSection &callback_lock = getCallbackLock();

            for(int i=0; i<numSamples; i++, ++csndIndex)
            {
                if(csndIndex == csdKsmps)
                {
                    callback_lock.enter();
                    //slow down calls to these functions, no need for them to be firing at k-rate
                    if (guiRefreshRate < yieldCounter)
                    {
                        yieldCounter = 0;
                        sendOutgoingMessagesToCsound();
                        updateCabbageControls();
                    }
                    else
                        ++yieldCounter;

                    csCompileResult = csound->PerformKsmps();

                    if(csCompileResult!=OK)
                        stopProcessing = true;
                    else
                        ++ksmpsOffset;

                    callback_lock.exit();
                    csndIndex = 0;
                }
                if(csCompileResult==OK)
                {
                    pos = csndIndex * output_channel_count;
                    for(int channel = 0; channel < output_channel_count; ++channel)
                    {
                        float *&current_buffer = audioBuffers[channel];
                        samp = *current_buffer * cs_scale;
                        CSspin[pos] = samp;
                        *current_buffer = (CSspout[pos] / cs_scale);
                        ++current_buffer;
                        ++pos;

                    }

                }
                else
                    buffer.clear();


            }

            if (activeWriter != 0 && !isWinXP)
                activeWriter->write (buffer.getArrayOfReadPointers(), numSamples);

            if(isMuted)
                buffer.clear();

            rmsLeft = buffer.getRMSLevel(0, 0, numSamples);
            rmsRight = buffer.getRMSLevel(1, 0, numSamples);

        }//if not compiled just mute output
        else
        {
            for(int channel = 0; channel < output_channel_count; ++channel)
            {
                for(int i=0; i<numSamples; ++i, ++csndIndex)
                {
                    audioBuffers[channel][i]=0;
                }
            }
        }



#endif
    }

    vuCounter = (vuCounter>5) ? 0 : vuCounter+1;
    if(vuCounter==0)
        sendActionMessage("rmsValues "+String(rmsLeft)+" "+String(rmsRight));
}


//==============================================================================
// MIDI functions
//==============================================================================

#ifndef Cabbage_No_Csound
int CabbagePluginAudioProcessor::OpenMidiInputDevice(CSOUND * csound, void **userData, const char* /*devName*/)
{
    *userData = csoundGetHostData(csound);
    if(!userData)
        cout << "\n\ncan't open midi in\n\n";
    return 0;
}

//==============================================================================
// Reads MIDI input data from host, gets called every time there is MIDI input to our plugin
//==============================================================================
int CabbagePluginAudioProcessor::ReadMidiData(CSOUND* /*csound*/, void *userData,
        unsigned char *mbuf, int nbytes)
{
    CabbagePluginAudioProcessor *midiData = (CabbagePluginAudioProcessor *)userData;
    if(!userData)
    {
        cout << "\nInvalid";
        return 0;
    }

    int cnt=0;

    if(!midiData->midiBuffer.isEmpty() && cnt <= (nbytes - 3))
    {
        MidiMessage message(0xf4, 0, 0, 0);
        MidiBuffer::Iterator i (midiData->midiBuffer);
        int messageFrameRelativeTothisProcess;
        while (i.getNextEvent (message, messageFrameRelativeTothisProcess))
        {
            const uint8* data = message.getRawData();
            *mbuf++ = *data++;
            *mbuf++ = *data++;
            *mbuf++ = *data++;

            if(message.isProgramChange() || message.isChannelPressure())
                cnt+=2;
            else
                cnt+=3;
        }
        midiData->midiBuffer.clear();
    }
    return cnt;

}

//==============================================================================
// Opens MIDI output device, adding -QN to your CsOptions will causes this method to be called
// as soon as your plugin loads
//==============================================================================
int CabbagePluginAudioProcessor::OpenMidiOutputDevice(CSOUND * csound, void **userData, const char* /*devName*/)
{
#if !defined(AndroidBuild)
    *userData = csoundGetHostData(csound);
    if(!userData)
        Logger::writeToLog("\n\ncan't open midi out\n\n");
#endif
    return 0;
}

//==============================================================================
// Write MIDI data to plugin's MIDI output. Each time Csound outputs a midi message this
// method should be called. Note: you must have -Q set in your CsOptions
//==============================================================================
int CabbagePluginAudioProcessor::WriteMidiData(CSOUND* /*csound*/, void *_userData,
        const unsigned char *mbuf, int nbytes)
{
    CabbagePluginAudioProcessor *userData = (CabbagePluginAudioProcessor *)_userData;
    if(!userData)
    {
        Logger::writeToLog("\n\nInvalid");
        return 0;
    }

    MidiMessage message(mbuf, nbytes, 0);
    //Logger::writeToLog(String(message.getNoteNumber()));
    userData->midiOutputBuffer.addEvent(message, 0);
    return nbytes;
}

#endif
//==============================================================================
bool CabbagePluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* CabbagePluginAudioProcessor::createEditor()
{
    return new CabbagePluginAudioProcessorEditor (this);
}

//==============================================================================
void CabbagePluginAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // Here's an example of how you can use XML to make it easy and more robust:
    // Create an outer XML element..
    XmlElement xml ("CABBAGE_PLUGIN_SETTINGS");
    cUtils::debug(guiCtrls.size());
    for(int i=0; i<guiCtrls.size(); i++)
    {
        xml.setAttribute(guiCtrls[i].getStringProp(CabbageIDs::channel),getParameter(i));
    }

    for(int i=0; i<guiLayoutCtrls.size(); i++)
        if(guiLayoutCtrls[i].getStringProp(CabbageIDs::type)==CabbageIDs::filebutton)
        {
            //save filebutton last opened file...
            char string[4096] = {0};
            csound->GetStringChannel(guiLayoutCtrls[i].getStringProp(CabbageIDs::channel).toUTF8().getAddress(), string);
            xml.setAttribute ("filebutton_"+guiLayoutCtrls[i].getStringProp(CabbageIDs::channel), String(string));
        }
        else if(guiLayoutCtrls[i].getStringProp(CabbageIDs::type)==CabbageIDs::sourcebutton)
        {
            xml.setAttribute("sourcebutton", csdFile.getFullPathName());
        }
        else if(guiLayoutCtrls[i].getStringProp(CabbageIDs::type)==CabbageIDs::texteditor)
        {
            xml.setAttribute(guiLayoutCtrls[i].getStringProp(CabbageIDs::name), guiLayoutCtrls[i].getStringProp(CabbageIDs::text));
        }

#ifdef CABBAGE_AU
    xml.setAttribute("sourcefile", csdFile.getFullPathName());
#endif
    // then use this helper function to stuff it into the binary blob and return it..
    copyXmlToBinary (xml, destData);

}

void CabbagePluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    // This getXmlFromBinary() helper function retrieves our XML from the binary blob..
    ScopedPointer<XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState != nullptr)
    {
#ifdef CABBAGE_AU
        for(int i=0; i<xmlState->getNumAttributes(); i++)
        {
            if(xmlState->getAttributeName(i).contains("sourcefile"))
            {
                //showMessage(xmlState->getAttributeValue(i));
                csdFile = File(xmlState->getAttributeValue(i));
                initialiseWidgets(csdFile.loadFileAsString(), true);
                addWidgetsToEditor(true);
                recompileCsound(csdFile);
                updateHostDisplay();
            }
            else if(xmlState->getAttributeName(i).contains("texteditor"))
            {
                String channel = xmlState->getAttributeName(i).substring(11);
                guiLayoutCtrls[i].setStringProp(CabbageIDs::text, xmlState->getAttributeValue(i));
            }
        }
#endif
        // make sure that it's actually our type of XML object..
        if (xmlState->hasTagName ("CABBAGE_PLUGIN_SETTINGS"))
        {
            for(int i=0; i<this->getNumParameters(); i++)
            {
                this->setParameter(i, (float)xmlState->getDoubleAttribute(guiCtrls[i].getStringProp(CabbageIDs::channel)));
            }


            for(int i=0; i<xmlState->getNumAttributes(); i++)
            {
                //if the instrument uses a filebutton, then retreive the last known
                //file that was loaded with it
                if(xmlState->getAttributeName(i).contains("filebutton_"))
                {
                    String channel = xmlState->getAttributeName(i).substring(11);
                    //Logger::writeToLog(xmlState->getAttributeValue(i));
                    csound->SetChannel(channel.toUTF8().getAddress(), xmlState->getAttributeValue(i).toUTF8().getAddress());
                }
                else  if(xmlState->getAttributeName(i).contains("sourcebutton"))
                {
                    //showMessage(xmlState->getAttributeValue(i));
                    csdFile = File(xmlState->getAttributeValue(i));
                    initialiseWidgets(csdFile.loadFileAsString(), true);
                    addWidgetsToEditor(true);
                    recompileCsound(csdFile);
                }
            }
        }
    }
}
//==============================================================================
// breakpoint functions
//==============================================================================
void CabbagePluginAudioProcessor::setCsoundInstrumentBreakpoint(int instr, int line=0)
{
#if defined(BUILD_DEBUGGER) && !defined(Cabbage_No_Csound)
    getCallbackLock().enter();
    if(breakpointInstruments.size()==0)
        csoundDebuggerInit(csound->GetCsound());
    csoundSetBreakpointCallback(csound->GetCsound(), breakpointCallback, (void*)this);
    csoundSetInstrumentBreakpoint(csound->GetCsound(), instr, 0);
    breakpointInstruments.add(instr);
    getCallbackLock().exit();
#endif
}

void CabbagePluginAudioProcessor::removeCsoundInstrumentBreakpoint(int instr)
{
#if defined(BUILD_DEBUGGER) && !defined(Cabbage_No_Csound)
    getCallbackLock().enter();
    breakpointInstruments.removeAllInstancesOf(instr);
    if(breakpointInstruments.size()==0)
        breakCount=0;
    csoundDebuggerInit(csound->GetCsound());
    csoundRemoveInstrumentBreakpoint(csound->GetCsound(), instr);
    getCallbackLock().exit();
#endif
}

void CabbagePluginAudioProcessor::continueCsoundDebug()
{
#if defined(BUILD_DEBUGGER) && !defined(Cabbage_No_Csound)

    if(breakpointInstruments.size()>0)
    {
        getCallbackLock().enter();
        csoundDebugContinue(csound->GetCsound());
        getCallbackLock().exit();
    }

#endif
}

void CabbagePluginAudioProcessor::nextCsoundDebug()
{
#if defined(BUILD_DEBUGGER) && !defined(Cabbage_No_Csound)
    //not yet implemented....
    if(breakpointInstruments.size()>0)
    {
        getCallbackLock().enter();
        //csoundDebugNext(csound->GetCsound());
        getCallbackLock().exit();
    }

#endif
}

void CabbagePluginAudioProcessor::cleanCsoundDebug()
{
#if defined(BUILD_DEBUGGER) && !defined(Cabbage_No_Csound)
    csoundClearBreakpoints(csound->GetCsound());
#endif
}
