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

#ifndef __PLUGINPROCESSOR_H_FE85D052__
#define __PLUGINPROCESSOR_H_FE85D052__

#include "../../JuceLibraryCode/JuceHeader.h"
#include "../CabbageUtils.h"
#include "../CabbageGUIClass.h"
#include "../XYPadAutomation.h"
#include "../CabbageMessageSystem.h"
#include "../Soundfiler.h"
#include "../Editor/CodeWindow.h"
#include "../Editor/CodeEditor.h"
//#include "CabbageGenericAudioProcessorEditor.h"
#include "../CabbageLookAndFeel.h"


#ifndef Cabbage_No_Csound
#ifdef AndroidBuild
#include "AndroidCsound.hpp"
#else
#include <csound.hpp>
#endif
#endif

#include "csdl.h"


#ifndef Cabbage_Build_Standalone
class CodeWindow;
#endif

#ifdef Cabbage64Bit
#define CABBAGE_VERSION "Cabbage(64bit) v0.5.15"
#else
#define CABBAGE_VERSION "Cabbage(32bit) v0.5.15"
#endif

#define AUDIO_PLUGIN 1
#define EXTERNAL_PLUGIN 2
#define AUTOMATION_PLUGIN 3

//#if defined(Cabbage_Build_Standalone) || defined(CABBAGE_HOST)
//class CsoundCodeEditor;
//#endif

#if defined(BUILD_DEBUGGER) && !defined(Cabbage_No_Csound)
#include <csdebug.h>
#endif


//==============================================================================
// CabbagePluginAudioProcessor definition
//==============================================================================
class CabbagePluginAudioProcessor  : public AudioProcessor,
    public cUtils,
    public ChangeBroadcaster,
    public Timer,
    public ActionBroadcaster,
    public ChangeListener,
    public ActionListener
{
    //==============================================================================
    File csdFile;
    int masterCounter;
    String filename;
    String pluginName;
    bool csoundStatus;
    int csCompileResult;
    void timerCallback();
    String csoundOutput;
    String debuggerMessage;
    void changeListenerCallback(ChangeBroadcaster *source);
    String changeMessageType;
    bool guiON;
    int currentLine;
    bool xyAutosCreated;
    bool updateTable;
    Array<int> tableNumbers;
    AudioPlayHead::CurrentPositionInfo hostInfo;

    NamedValueSet macroText;

    StringArray socketChannelIdentifiers;
    NamedValueSet socketChannelValues;
    String socketAddress;
    int scoketPort;

    ScopedPointer<FileLogger> fileLogger;
    bool createLog;

    //ExamplePacketListener listener;
    //UdpListeningReceiveSocket socket;
    //ScopedPointer<OscThread> oscThread;

    File logFile;
    bool isAutomator;
    bool isWinXP;
    bool isNativeThreadRunning;
    String csoundDebuggerOutput;
    float rmsLeft, rmsRight;

    //============== Csound related variables/methods ==============================
#ifndef Cabbage_No_Csound
    ScopedPointer<CSOUND_PARAMS> csoundParams;
    //ScopedPointer<CsoundPerformanceThread> csoundPerfThread;
    PVSDATEXT* dataout;
    MYFLT cs_scale;
#if !defined(AndroidBuild)
    ScopedPointer<Csound> csound;                           //Csound instance
#else
    ScopedPointer<AndroidCsound> csound;
#endif
    MYFLT *CSspin, *CSspout;        //Csound audio IO pointers
    int csndIndex;                          //Csound sample counter
    int csdKsmps;
    int CSCompResult;                       //result of Csound performKsmps
    controlChannelInfo_s* csoundChanList;
    int numCsoundChannels;          //number of Csound channels
    static void messageCallback(CSOUND *csound, int attr, const char *fmt, va_list args);  //message callback function
#if defined(BUILD_DEBUGGER) && !defined(Cabbage_No_Csound)
    static void breakpointCallback(CSOUND *csound, debug_bkpt_info_t *bkpt_info, void *udata);
#endif
    //Csound API functions for deailing with midi input
    static int OpenMidiInputDevice(CSOUND * csnd, void **userData, const char *devName);
    static int OpenMidiOutputDevice(CSOUND * csnd, void **userData, const char *devName);
    static int ReadMidiData(CSOUND *csound, void *userData, unsigned char *mbuf, int nbytes);
    static int WriteMidiData(CSOUND *csound, void *userData, const unsigned char *mbuf, int nbytes);
#endif
    static void YieldCallback(void* data);
    void updateCabbageControls();
    void sendOutgoingMessagesToCsound();
    int ksmpsOffset;
    bool CS_DEBUG_MODE;
    int pos;


    StringArray debugInfo;
    StringArray widgetTypes;
    //basic classes that hold all information regarding GUI objects
    //guiLayoutControls are not used to send data to Csound, and don't show
    //as parameters in a host, guiCtrls do show are parameters, and can send
    //channel messages to Csound.
    Array<CabbageGUIClass, CriticalSection> guiLayoutCtrls;
    Array<CabbageGUIClass, CriticalSection> guiCtrls;
    String plantFlag, presetFlag;
    String debugMessage;
    StringArray debugMessageArray;
    String currentLineText;
    bool editorReOpened;
    OwnedArray<XYPadAutomation, CriticalSection> xyAutomation;
    void updateGUIControlsKsmps(int speed);
    int guiRefreshRate;
#ifdef Cabbage_No_Csound
    std::vector<float> temp;
#else
    std::vector<MYFLT> temp;
#endif
    TimeSliceThread backgroundThread; // the thread that will write our audio data to disk
    ScopedPointer<AudioFormatWriter::ThreadedWriter> threadedWriter; // the FIFO used to buffer the incoming data
    int64 nextSampleNum;
    double sampleRate;

    File tempAudioFile;
    CriticalSection writerLock;
    AudioFormatWriter::ThreadedWriter* volatile activeWriter;
    bool firstTime, isBypassed, isMuted;


public:

    //------------------------------- interprocess comms -------------------------------
    void appendMessage (const String& message)
    {
        cUtils::debug(message);
    }


    class CabbageInterprocessConnection  : public InterprocessConnection
    {
    public:
        CabbageInterprocessConnection (CabbagePluginAudioProcessor& owner_)
            : InterprocessConnection (true),
              owner (owner_)
        {
            static int totalConnections = 0;
            ourNumber = ++totalConnections;
        }

        void connectionMade()
        {
            const String message = "Connection #" + String (ourNumber) + " - connection started";
            owner.csound->Message(message.toUTF8());
        }

        void connectionLost()
        {
            const String message = "Connection #" + String (ourNumber) + " - connection lost";
            owner.csound->Message(message.toUTF8());
        }

        void messageReceived (const MemoryBlock& message);

    private:
        CabbagePluginAudioProcessor& owner;
        int ourNumber;
    };

    class CabbageInterprocessConnectionServer   : public InterprocessConnectionServer
    {
    public:
        CabbageInterprocessConnectionServer (CabbagePluginAudioProcessor& owner_)
            : owner (owner_)
        {
        }

        InterprocessConnection* createConnectionObject()
        {
            CabbageInterprocessConnection* newConnection = new CabbageInterprocessConnection (owner);

            owner.activeConnections.add (newConnection);
            return newConnection;
        }

    private:
        CabbagePluginAudioProcessor& owner;
    };

    OwnedArray <CabbageInterprocessConnection, CriticalSection> activeConnections;
    ScopedPointer<CabbageInterprocessConnectionServer> server;
    void openInterprocess (bool asSocket, bool asSender, String address, int port);

    void closeInterprocess()
    {
        server->stop();
        activeConnections.clear();

    }


    //--------------------------------------------------------------

    bool isFirstTime()
    {
        return firstTime;
    };
    String changeMessage;
    Array<int> dirtyControls;
    bool CSOUND_DEBUG_MODE;
    void setCsoundInstrumentBreakpoint(int instr, int line);
    void removeCsoundInstrumentBreakpoint(int instr);
    void continueCsoundDebug();
    void nextCsoundDebug();
    void cleanCsoundDebug();
    void createAndShowSourceEditor(LookAndFeel* looky);
    void actionListenerCallback (const String& message);



    int getNumberCsoundOutChannels()
    {
        return csound->GetNchnls();
    }

    int getNumberCsoundInChannels()
    {
        return csound->GetNchnls();
    }

    int getCsoundSamplingRate()
    {
        return csound->GetSr();
    }

    int getCsoundKsmpsSize()
    {
        return csound->GetKsmps();
    }

    void shouldBypass(bool val)
    {
        const ScopedLock sl (getCallbackLock());
        isBypassed = val;
    }

    StringArray getWidgetTypes()
    {
        return widgetTypes;
    }

    void shouldMute(bool val)
    {
        const ScopedLock sl (getCallbackLock());
        isMuted = val;
    }
    //==============================================================================

#if defined(Cabbage_Build_Standalone) || (CABBAGE_HOST)
    CabbagePluginAudioProcessor(String inputfile, bool guiOnOff, int pluginType);
#else
    CabbagePluginAudioProcessor();
#endif
    ~CabbagePluginAudioProcessor();

//#if defined(Cabbage_Build_Standalone) || defined(CABBAGE_HOST)
    CsoundCodeEditor* codeEditor;
//#else

#if !defined(Cabbage_Build_Standalone) && !defined(CABBAGE_HOST)
    CodeWindow* cabbageCsoundEditor;
#endif


    bool compiledOk()
    {
        return csCompileResult;
    }

    bool haveXYAutosBeenCreated()
    {
        return xyAutosCreated;
    }

    void setHaveXYAutoBeenCreated(bool val)
    {
        xyAutosCreated = val;
    }

    double getTailLengthSeconds(void) const
    {
        return 1;
    }

    int performEntireScore();
    void startRecording();
    void stopRecording();
    int reCompileCsound(File file);
    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock);
    void releaseResources();
    virtual void processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages);
    //==============================================================================
    AudioProcessorEditor* createEditor();
    bool hasEditor() const;
    //==============================================================================
    const String getName() const;
    int getNumParameters();
    float getParameter (int index);
    void setParameter (int index, float newValue);
    const String getParameterName (int index);
    const String getParameterText (int index);
    const String getInputChannelName (int channelIndex) const;
    const String getOutputChannelName (int channelIndex) const;
    bool isInputChannelStereoPair (int index) const;
    bool isOutputChannelStereoPair (int index) const;
    bool acceptsMidi() const;
    bool producesMidi() const;
    //Array<int> pField4, pField5, pField6, pField7;
    int noSteps;
    int noPatterns;
    int timeCounter;
    int beat;
    int patMatrixActive;
    float bpm;
    //==============================================================================
    int getNumPrograms();
    int getCurrentProgram();
    void setCurrentProgram (int index);
    const String getProgramName (int index);
    void changeProgramName (int index, const String& newName);
    //==============================================================================
    void getStateInformation (MemoryBlock& destData);
    void setStateInformation (const void* data, int sizeInBytes);
    StringArray getTableStatement(int tableNum);
    const Array<double, CriticalSection> getTable(int tableNum);
    const Array<float, CriticalSection> getTableFloats(int tableNum);
    void createGUI(String source, bool refresh);
    int checkTable(int tableNum);
    MidiKeyboardState keyboardState;
    //midiBuffers
    MidiBuffer midiBuffer;
    MidiBuffer midiOutputBuffer;
    MidiBuffer ccBuffer;
    bool showMIDI;
    bool yieldCallbackBool;
    int yieldCounter, vuCounter;
    bool nativePluginEditor;
    CabbageMessageQueue messageQueue;
    StringArray scoreEvents;
    int averageSampleIndex;
    bool stopProcessing;
    float outputNo1;
    int pluginType;
    float automationAmp;
    int automationParamID;
    int mouseX, mouseY;
    int breakCount;
    Array<int> breakpointInstruments;


    //==============================================================================
    File getCsoundInputFile()
    {
        return csdFile;
    }

    inline String getCsoundInputFileText()
    {
        String ret="";
#if (defined(Cabbage_Build_Standalone) || defined(CABBAGE_HOST)) && !defined(AndroidBuild)
        if(codeEditor)
            ret = codeEditor->getAllText();
        else
            ret = csdFile.loadFileAsString();
#endif
        return ret;
    }

    void updateCsoundFile(String text)
    {
        //csdFile.replaceWithText(text);
#if (defined(Cabbage_Build_Standalone) || defined(CABBAGE_HOST)) && !defined(AndroidBuild)
        codeEditor->setAllText(text);
#endif
    }

    int saveEditorFiles()
    {
#if defined(Cabbage_Build_Standalone) || defined(CABBAGE_HOST)
        if(codeEditor)
            return codeEditor->saveAllFiles();
#endif
    }

    void saveText()
    {
#if defined(Cabbage_Build_Standalone) || defined(CABBAGE_HOST)
        codeEditor->textChanged = false;
#endif
    }

    void setOpcodeDirEnv()
    {
#ifdef WIN32
        //showMessage(getenv("OPCODE6DIR64"));
        String opcodeDir = File::getSpecialLocation(File::currentExecutableFile).getParentDirectory().getFullPathName()+"\\CsoundPlugins";
        if(!File(opcodeDir).exists())
            opcodeDir = String(getenv("CABBAGE_OPCODE_PATH"));

        Logger::writeToLog("\n================================\nCabbage opcode plugins are located at:"+opcodeDir);
        //showMessage(opcodeDir);
        if(File(opcodeDir).exists())
        {
            String env = "OPCODE6DIR64="+opcodeDir;
            _putenv(env.toUTF8().getAddress());
            Logger::writeToLog("Current opcodeDir is:"+String(getenv("OPCODE6DIR64")));
        }
#endif
    }

    void highlightLine(String text)
    {
#if (defined(Cabbage_Build_Standalone) || defined(CABBAGE_HOST)) && !defined(AndroidBuild)
        codeEditor->highlightLine(text);
#endif
    }

    String getDebugMessage()
    {
        return debugMessage;
    }

    void addDebugMessage(String message)
    {
        debugMessageArray.add(message);;
    }

    StringArray getDebugMessageArray()
    {
        return debugMessageArray;
    }

    void clearDebugMessageArray()
    {
        debugMessageArray.clear();
    }

    int getCompileStatus()
    {
        return csCompileResult;
    }

    void clearDebugMessage()
    {
        debugMessage="";
    }

    void setPluginName(String name)
    {
        pluginName = name;
    }

    String getPluginName()
    {
        return pluginName;
    }

    Array<Array <float > > tableArrays;

    Array<float> getTableArray(int index)
    {
        return tableArrays[index];
    }

    void setMidiDebug(bool val)
    {
        showMIDI=val;
    }
    bool getMidiDebug()
    {
        return showMIDI;
    }
    //======== log information about GUI controls ===============
    StringArray logGUIAttributes(CabbageGUIClass cAttr, String type)
    {

        StringArray arr;
        /*
        arr.add(String("----------- ")+type+String(" -----------"));
        arr.add(String("Name:")+cAttr.getStringProp("name")+String(", Type:")+cAttr.getStringProp("type")+String(", Caption:")+cAttr.getStringProp("caption")+String(", RelToPlant:")+cAttr.getStringProp("reltoplant")+String(", Plant:")+cAttr.getStringProp("plant"));
        arr.add(String("PosX:")+String(cAttr.getNumProp("left"))+String(", PosY:")+String(cAttr.getNumProp("top"))+String(", Width:")+String(cAttr.getNumProp("width"))+String(", Height:")+String(cAttr.getNumProp("height")));
        */
        arr.add(String("   "));
        //Logger::writeToLog(String("----------- ")+type+String(" -----------"));
        //Logger::writeToLog(String("Name:")+cAttr.getStringProp("name")+String(", Type:")+cAttr.getStringProp("type")+String(", Caption:")+cAttr.getStringProp("caption")+String(", RelToPlant:")+cAttr.getStringProp("reltoplant")+String(", Plant:")+cAttr.getStringProp("plant"));
        //Logger::writeToLog(String("PosX:")+String(cAttr.getNumProp("left"))+String(", PosY:")+String(cAttr.getNumProp("top"))+String(", Width:")+String(cAttr.getNumProp("width"))+String(", Height:")+String(cAttr.getNumProp("height")));
        //Logger::writeToLog("   ");
        return arr;
    }
    inline bool getCsoundStatus()
    {
        return csoundStatus;
    }

    //===========================================================
    inline int getGUICtrlsSize()
    {
        return (int)guiCtrls.size();
    }

    inline int getGUILayoutCtrlsSize()
    {
        return (int)guiLayoutCtrls.size();
    }

    inline CabbageGUIClass &getGUILayoutCtrls(int index)
    {
        return guiLayoutCtrls.getReference(index);
    }

    inline CabbageGUIClass &getGUICtrls(int index)
    {
        return guiCtrls.getReference(index);
    }

    inline String getChangeMessageType()
    {
        return changeMessageType;
    }

    String getCsoundOutput();

    inline String getDebuggerOutput()
    {
        return csoundDebuggerOutput;
    }

    inline void setChangeMessageType(String text)
    {
        changeMessageType = text;
    }

    inline int getCurrentLine()
    {
        return currentLine;
    }

    inline void setCurrentLine(int line)
    {
        currentLine = line;
    }

    inline void setCurrentLineText(String lineText)
    {
        currentLineText = lineText;
    }

    String getCurrentLineText()
    {
        return currentLineText;
    }

    void removeGUIComponent(int index, String type);

#ifndef Cabbage_No_Csound
    Csound* getCsound()
    {
        return csound;
    }

    CSOUND* getCsoundStruct()
    {
        return csound->GetCsound();
    }

    MYFLT getCSScale()
    {
        return cs_scale;
    }

    PVSDATEXT* getPVSDataOut()
    {
        return dataout;
    }

#endif
    void addLayoutCtrl(CabbageGUIClass cAttr)
    {
        guiLayoutCtrls.add(cAttr);
    }

    void addGUICtrl(CabbageGUIClass cAttr)
    {
        guiCtrls.add(cAttr);
    }


    bool isGuiEnabled()
    {
        return guiON;
    }

    void setGuiEnabled(bool val);

    void addXYAutomater(XYPadAutomation* xyAuto)
    {
        xyAutomation.add(xyAuto);
    }

    XYPadAutomation* getXYAutomater(int index)
    {
        return xyAutomation[index];
    }

    int getXYAutomaterSize()
    {
        return xyAutomation.size();
    }

    void removeXYAutomaters()
    {
        xyAutomation.clear();
    }

    bool silenceInProducesSilenceOut() const
    {
        return true;
    }



    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbagePluginAudioProcessor);

};

//special auotmation only plugin type. Does not output any audio.
#ifdef Cabbage_Host
class CabbagePluginAutomationProcessor : public CabbagePluginAudioProcessor
{
public:
    CabbagePluginAutomationProcessor(AudioProcessor* filter, String inputfile, int pluginType):
        CabbagePluginAudioProcessor(inputfile, false, pluginType)
    {}
    ~CabbagePluginAutomationProcessor() {}


};
#endif
#endif  // __PLUGINPROCESSOR_H_FE85D052__
