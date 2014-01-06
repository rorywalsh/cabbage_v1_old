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

#include "CabbageStandaloneDialog.h"



#define MAXBYTES 16777216

//==============================================================================
//  Somewhere in the codebase of your plugin, you need to implement this function
//  and make it create an instance of the filter subclass that you're building.
extern CabbagePluginAudioProcessor* JUCE_CALLTYPE createCabbagePluginFilter(String inputfile, bool guiOnOff, int plugType);


//==============================================================================
StandaloneFilterWindow::StandaloneFilterWindow (const String& title,
                                                const Colour& backgroundColour)
    : DocumentWindow (title, backgroundColour,
                      DocumentWindow::minimiseButton
                       | DocumentWindow::closeButton),
      optionsButton ("options"), 
	  isGUIOn(false), 
	  pipeOpenedOk(false), 
	  AudioEnabled(true), 
	  isAFileOpen(false),
	  standaloneMode(false),
	  updateEditorOutputConsole(false)
{	
	String defaultCSDFile = File(File::getSpecialLocation(File::currentExecutableFile)).withFileExtension(".csd").getFullPathName();
	consoleMessages = "";
	cabbageDance = 0;
	setTitleBarButtonsRequired (DocumentWindow::minimiseButton | DocumentWindow::closeButton, false);
    Component::addAndMakeVisible (&optionsButton);
    optionsButton.addListener (this);
	timerRunning = false;
	yAxis = 0;
	this->addKeyListener(this);
	optionsButton.setTriggeredOnMouseDown (true);
	bool alwaysontop = getPreference(appProperties, "SetAlwaysOnTop");
	setAlwaysOnTop(alwaysontop);
	this->setResizable(false, false);
	this->startTimer(true);
	lookAndFeel = new CabbageLookAndFeel();
	this->setLookAndFeel(lookAndFeel);
	oldLookAndFeel = new LookAndFeel_V1();
// MOD - Stefano Bonetti 
#ifdef Cabbage_Named_Pipe 
	ipConnection = new socketConnection(*this);
	pipeOpenedOk = ipConnection->createPipe(String("cabbage"));
	if(pipeOpenedOk) Logger::writeToLog(String("Namedpipe created ..."));
#endif 
// MOD - End

	JUCE_TRY
    {
        filter = createCabbagePluginFilter("", false, AUDIO_PLUGIN);
		filter->addChangeListener(this);
		filter->addActionListener(this);
		filter->sendChangeMessage();
		filter->createGUI("", true);		
    }
    JUCE_CATCH_ALL

    if (filter == nullptr)
    {
        jassertfalse    // Your filter didn't create correctly! In a standalone app that's not too great.
        JUCEApplication::quit();
    }

    filter->setPlayConfigDetails (JucePlugin_MaxNumInputChannels, 
                                          JucePlugin_MaxNumOutputChannels,
                                          44100, 512);

    PropertySet* const globalSettings = getGlobalSettings();

    deviceManager = new AudioDeviceManager();
	deviceManager->addAudioCallback (&player);
	deviceManager->addMidiInputCallback (String::empty, &player);

	ScopedPointer<XmlElement> savedState;
	if (globalSettings != nullptr)
    savedState = globalSettings->getXmlValue ("audioSetup");

    deviceManager->initialise (filter->getNumInputChannels(),
                               filter->getNumOutputChannels(), 0, false);
	//deviceManager->closeAudioDevice();
	filter->suspendProcessing(true);

	int runningCabbageIO = getPreference(appProperties, "UseCabbageIO");
    if(runningCabbageIO){	
		player.setProcessor (filter);    
		if (globalSettings != nullptr)
		{
			MemoryBlock data;

			if (data.fromBase64Encoding (globalSettings->getValue ("filterState"))
				 && data.getSize() > 0)
			{
				filter->setStateInformation (data.getData(), data.getSize());
			}
		}

		
	}
	else deviceManager->closeAudioDevice();

	setContentOwned (filter->createEditorIfNeeded(), true);

    const int x = globalSettings->getIntValue ("windowX", -100);
    const int y = globalSettings->getIntValue ("windowY", -100);

    if (x != -100 && y != -100)
        setBoundsConstrained (Rectangle<int> (x, y, getWidth(), getHeight()));
    else
		centreWithSize (getWidth(), getHeight());
		
	setPreference(appProperties, "AutoUpdate",0);
	
	//create editor but don't display it yet...
	cabbageCsoundEditor = new CodeWindow(csdFile.getFileName());
	cabbageCsoundEditor->setVisible(false);
	cabbageCsoundEditor->setSize(800, 500);
	//cabbageCsoundEditor->setFullScreen(true);
	cabbageCsoundEditor->addActionListener(this);
	cabbageCsoundEditor->setLookAndFeel(lookAndFeel);
	
	filter->codeEditor = cabbageCsoundEditor->textEditor;	
	
	//opens a default file that matches the name of the current executable
	//this can be used to create more 'standalone' like apps
	if(File(defaultCSDFile).existsAsFile()){
		standaloneMode = true;
		openFile(defaultCSDFile);
	}
	
	//filter->codeWindow = cabbageCsoundEditor->textEditor;
	//start timer for output message, and autoupdate if it's on
	startTimer(100);
}
//==============================================================================
// Destructor
//==============================================================================
StandaloneFilterWindow::~StandaloneFilterWindow()
{
//cabbageCsoundEditor.release();
//outputConsole.release();
#ifdef Cabbage_Named_Pipe 
// MOD - Stefano Bonetti
	if(ipConnection){ 
	ipConnection->disconnect();
	}
// MOD - End
#endif

    PropertySet* const globalSettings = getGlobalSettings();

    if (globalSettings != nullptr)
    {
        globalSettings->setValue ("windowX", getX());
        globalSettings->setValue ("windowY", getY());

        if (deviceManager != nullptr)
        {
            ScopedPointer<XmlElement> xml (deviceManager->createStateXml());
            globalSettings->setValue ("audioSetup", xml);
        }
    }

    deviceManager->removeMidiInputCallback (String::empty, &player);
    deviceManager->removeAudioCallback (&player);
    deviceManager = nullptr;

    if (globalSettings != nullptr && filter != nullptr)
    {
        MemoryBlock data;
        filter->getStateInformation (data);

        globalSettings->setValue ("filterState", data.toBase64Encoding());
    }
	filter->suspendProcessing(true);
    deleteFilter();
}

//==============================================================================
// insane Cabbage dancing....
//==============================================================================
void StandaloneFilterWindow::timerCallback()
{   

	if(getPreference(appProperties, "AutoUpdate")){
		int64 diskTime = csdFile.getLastModificationTime().toMilliseconds();
		int64 tempTime = lastSaveTime.toMilliseconds();
		if(diskTime>tempTime){
			resetFilter(true);
			lastSaveTime = csdFile.getLastModificationTime();
		}
	}
	
	//cout << csdFile.getLastModificationTime().toString(true, true, false, false);
	if(cabbageDance){
	float moveY = sin(yAxis*2*3.14*20/100); 
	float moveX = sin(yAxis*2*3.14*10/100); 
	yAxis+=1;
	this->setTopLeftPosition(this->getScreenX()+(moveX*5), this->getScreenY()+(moveY*10));
	
	}
	

	if(outputConsole)
		outputConsole->setText(filter->getCsoundOutput());	
		
//	updateEditorOutputConsole=false;
//	}
}

bool StandaloneFilterWindow::keyPressed(const juce::KeyPress &key ,Component *)
{
Logger::writeToLog("MainForm:"+key.getTextDescription());	
return false;
}

//==============================================================================
// action Callback - updates instrument according to changes in source code
//==============================================================================
void StandaloneFilterWindow::actionListenerCallback (const String& message){
	if(message == "GUI Update, PropsPanel"){
	//if something changes in the properties panel we need to update our GUI so
	//that the changes are reflected in the on screen components 
	filter->createGUI(csdFile.loadFileAsString(), true);
	if(cabbageCsoundEditor){
		//cabbageCsoundEditor->csoundDoc.replaceAllContent(filter->getCsoundInputFile().loadFileAsString());
		cabbageCsoundEditor->textEditor->highlightLine(filter->getCurrentLineText());
		}
	}
	
	else if(message == "Score Updated"){
	if(cabbageCsoundEditor){
		cabbageCsoundEditor->csoundDoc.replaceAllContent(filter->getCsoundInputFile().loadFileAsString());
		cabbageCsoundEditor->textEditor->highlightLine(filter->getCurrentLineText());
		}
	}	
	
	else if(message == "GUI Updated, controls added, resized"){
	//no need to update our entire GUI here as only on-screen sizes change 
	setCurrentLine(filter->getCurrentLine()+1);
	if(cabbageCsoundEditor){
		//cabbageCsoundEditor->csoundDoc.replaceAllContent(filter->getCsoundInputFile().loadFileAsString());
		cabbageCsoundEditor->textEditor->highlightLine(filter->getCurrentLineText());
		}
	}

	else if(message == "GUI Updated, controls deleted"){
	//controls have already been deleted, we just need to update our file
	if(cabbageCsoundEditor)
	cabbageCsoundEditor->csoundDoc.replaceAllContent(csdFile.loadFileAsString());
	}

	else if(message.equalsIgnoreCase("fileSaved")){
	saveFile();
	}

	else if(message.contains("fileOpen")){
	openFile("");
	}

	else if(message.contains("audioSettings")){
	showAudioSettingsDialog();
	}
	
	else if(message.contains("fileSaveAs")){
	saveFileAs();
	}

	else if(message.contains("fileExportSynth")){
	exportPlugin(String("VSTi"), false);
	}

	else if(message.contains("fileExportEffect")){
	exportPlugin(String("VST"), false);
	}

	else if(message.contains("fileUpdateGUI")){
	filter->createGUI(cabbageCsoundEditor->getText(), true);
	csdFile.replaceWithText(cabbageCsoundEditor->getText()); 
	if(cabbageCsoundEditor){
		cabbageCsoundEditor->setName(csdFile.getFullPathName());
		}
	}

#ifdef Cabbage_Build_Standalone
	else if(message.contains("MENU COMMAND: manual update instrument"))
		resetFilter(true);

	else if(message.contains("MENU COMMAND: open instrument"))
		openFile("");

	else if(message.contains("MENU COMMAND: manual update GUI"))
		filter->createGUI(csdFile.loadFileAsString(), true);
		
	else if(message.contains("MENU COMMAND: toggle edit")){
		int val = getPreference(appProperties, "DisableGUIEditModeWarning");
		if(val)
			showMessage("", "Warning!! This feature is bleeding edge! (that's programmer speak for totally untested and likely to crash hard!). If you like to live on the edge, disable this warning under the 'Preferences' menu command and try 'Edit Mode' again, otherwise just let it be...", lookAndFeel, this);
		else{
			if(isAFileOpen == true)
			if(filter->isGuiEnabled()){
			((CabbagePluginAudioProcessorEditor*)filter->getActiveEditor())->setEditMode(false);
			filter->setGuiEnabled(false);
			}
			else{
			((CabbagePluginAudioProcessorEditor*)filter->getActiveEditor())->setEditMode(true);
			filter->setGuiEnabled(true);
			stopTimer();
			setPreference(appProperties, "AutoUpdate", 0);
			}
		}		
	}
		
	else if(message.contains("MENU COMMAND: suspend audio"))
        if(AudioEnabled){
			filter->suspendProcessing(true);
			AudioEnabled = false;
		}
		else{
			AudioEnabled = true;
			filter->suspendProcessing(false);
		}
else{}
#endif

}
//==============================================================================

//==============================================================================
// listener Callback - updates WinXound compiler output with Cabbage messages
//==============================================================================
void StandaloneFilterWindow::changeListenerCallback(juce::ChangeBroadcaster* /*source*/)
{
/*	
String text;
if(!cabbageCsoundEditor || !outputConsole){
	for(int i=0;i<filter->getDebugMessageArray().size();i++)
		  {
			  if(filter->getDebugMessageArray().getReference(i).length()>0)
			  {
				  text += String(filter->getDebugMessageArray().getReference(i).toUTF8());

			  }

		  }
	consoleMessages = consoleMessages+text+"\n";
	}
*/
updateEditorOutputConsole=true;
}
//==============================================================================
// Delete filter
//==============================================================================
void StandaloneFilterWindow::deleteFilter()
{
	player.setProcessor (nullptr);

    if (filter != nullptr && getContentComponent() != nullptr)
    {
        filter->editorBeingDeleted (dynamic_cast <AudioProcessorEditor*> (getContentComponent()));
        clearContentComponent();
    }

    filter = nullptr;
}

//==============================================================================
// Reset filter
//==============================================================================
void StandaloneFilterWindow::resetFilter(bool shouldResetFilter)
{
//first we check that the audio device is up and running ok
stopTimer();
filter->suspendProcessing(true);
if(shouldResetFilter){
	deleteFilter();
	deviceManager->closeAudioDevice();
	filter = createCabbagePluginFilter(csdFile.getFullPathName(), false, AUDIO_PLUGIN);	
	filter->addChangeListener(this);
	filter->addActionListener(this);
}
else{
filter->reCompileCsound(csdFile);	
}
//	filter->sendChangeMessage();
	filter->createGUI(csdFile.loadFileAsString(), true);

	setName(filter->getPluginName());

	int runningCabbageProcess = getPreference(appProperties, "UseCabbageIO");
    
	setContentOwned (filter->createEditorIfNeeded(), true);
    if(runningCabbageProcess){
		if (filter != nullptr)
		{
			if (deviceManager != nullptr){
				player.setProcessor (filter);
				deviceManager->restartLastAudioDevice();
			}
		}
		
	}
	else{
		filter->performEntireScore();
	}


    PropertySet* const globalSettings = getGlobalSettings();

    if (globalSettings != nullptr)
        globalSettings->removeValue ("filterState");


	startTimer(100);
	
	if(cabbageCsoundEditor){
	cabbageCsoundEditor->setName(csdFile.getFileName());
	cabbageCsoundEditor->setText(csdFile.loadFileAsString());
	cabbageCsoundEditor->textEditor->textChanged = false;
	filter->codeEditor = cabbageCsoundEditor->textEditor;
//	if(Component::getCurrentlyFocusedComponent()->getName().contains("Panel"))
//		Component::getCurrentlyFocusedComponent()->setWantsKeyboardFocus(false);
	}
	
	filter->suspendProcessing(false);
}

//==============================================================================
void StandaloneFilterWindow::saveState()
{
    PropertySet* const globalSettings = getGlobalSettings();

    FileChooser fc (TRANS("Save current state"),
                    globalSettings != nullptr ? File (globalSettings->getValue ("lastStateFile"))
                                              : File::nonexistent);

    if (fc.browseForFileToSave (true))
    {
        MemoryBlock data;
        filter->getStateInformation (data);

        if (! fc.getResult().replaceWithData (data.getData(), data.getSize()))
        {
            AlertWindow::showMessageBox (AlertWindow::WarningIcon,
                                         TRANS("Error whilst saving"),
                                         TRANS("Couldn't write to the specified file!"));
        }
    }
}

void StandaloneFilterWindow::loadState()
{
    PropertySet* const globalSettings = getGlobalSettings();

    FileChooser fc (TRANS("Load a saved state"),
                    globalSettings != nullptr ? File (globalSettings->getValue ("lastStateFile"))
                                              : File::nonexistent);

    if (fc.browseForFileToOpen())
    {
        MemoryBlock data;

        if (fc.getResult().loadFileAsData (data))
        {
            filter->setStateInformation (data.getData(), data.getSize());
        }
        else
        {
            AlertWindow::showMessageBox (AlertWindow::WarningIcon,
                                         TRANS("Error whilst loading"),
                                         TRANS("Couldn't read from the specified file!"));
        }
    }
}

//==============================================================================
PropertySet* StandaloneFilterWindow::getGlobalSettings()
{
    /* If you want this class to store the plugin's settings, you can set up an
       ApplicationProperties object and use this method as it is, or override this
       method to return your own custom PropertySet.

       If using this method without changing it, you'll probably need to call
       ApplicationProperties::setStorageParameters() in your plugin's constructor to
       tell it where to save the file.
    */
	getPreference(appProperties, "htmlHelp", "some directory");
	return appProperties->getUserSettings();
}

void StandaloneFilterWindow::showAudioSettingsDialog()
{
const int numIns = filter->getNumInputChannels() <= 0 ? JucePlugin_MaxNumInputChannels : filter->getNumInputChannels();
const int numOuts = filter->getNumOutputChannels() <= 0 ? JucePlugin_MaxNumOutputChannels : filter->getNumOutputChannels();

    CabbageAudioDeviceSelectorComponent selectorComp (*deviceManager,
                                               numIns, numIns, numOuts, numOuts,
                                               true, false, true, false);

    selectorComp.setSize (400, 250);
	setAlwaysOnTop(false);
	lookAndFeel->setColour(Label::textColourId, Colours::white);
	selectorComp.setLookAndFeel(lookAndFeel);
	Colour col(24, 24, 24);
	DialogWindow::showModalDialog(TRANS("Audio Settings"), &selectorComp, this, col, true, false, false);
	bool alwaysontop = getPreference(appProperties, "SetAlwaysOnTop");
	setAlwaysOnTop(alwaysontop);

}
//==============================================================================
void StandaloneFilterWindow::closeButtonPressed()
{
if(filter)
if(filter->hasTextChanged()){
	int result = showYesNoMessage("You would like to save your changes?", lookAndFeel, 1);
	if(result==0)saveFile();
	else if(result==1)
	JUCEApplication::quit();
	else
		return;
	}
		
JUCEApplication::quit();
}

void StandaloneFilterWindow::resized()
{
    DocumentWindow::resized();
    optionsButton.setBounds (8, 6, 60, getTitleBarHeight() - 8);
	
	/*
	if(filter){
	CabbagePluginAudioProcessorEditor* editor = dynamic_cast<CabbagePluginAudioProcessorEditor*>(filter->getActiveEditor());
	if(editor)
		editor->updateSize();
	}*/
}


//==============================================================================
// Button clicked method
//==============================================================================
void StandaloneFilterWindow::buttonClicked (Button*)
{
    if (filter == nullptr)
        return;

	int examplesOffset = 4000;
	String test;
	PopupMenu subMenu, m;
	m.setLookAndFeel(lookAndFeel);
	subMenu.setLookAndFeel(lookAndFeel);
	PopupMenu recentFilesMenu;
	RecentlyOpenedFilesList recentFiles;
	Array<File> exampleFiles;
	recentFiles.restoreFromString (appProperties->getUserSettings()->getValue ("recentlyOpenedFiles"));
	
	standaloneMode=false;
	isAFileOpen = true;
	if(!standaloneMode){
		m.addItem(1, String("Open Cabbage Instrument | Ctrl+o"));
	
		recentFiles.createPopupMenuItems (recentFilesMenu, 9000, true, true);
		m.addSubMenu ("Open recent file", recentFilesMenu);	
		
		String examplesDir = appProperties->getUserSettings()->getValue("ExamplesDir", "");	
		if(!File(examplesDir).exists()){
		#if defined(LINUX) || defined(MACOSX)
			examplesDir = File::getSpecialLocation(File::currentExecutableFile).getParentDirectory().getFullPathName()+"/Examples";
		#else
			examplesDir = File::getSpecialLocation(File::currentExecutableFile).getParentDirectory().getFullPathName()+"\\Examples";
		#endif
		}
			
		addFilesToPopupMenu(subMenu, exampleFiles, examplesDir, "*.csd", examplesOffset); 
		m.addSubMenu(String("Examples"), subMenu);

		subMenu.clear();
		
		subMenu.addItem(30, String("Effect"));
		subMenu.addItem(31, String("Instrument"));
		m.addSubMenu(String("New Cabbage..."), subMenu);

		m.addItem(2, String("View Source Editor"));
		m.addItem(3, String("View Csound output"));
		m.addSeparator();
	}

		if(!getPreference(appProperties, "AudioEnabled"))
		m.addItem(400, String("Audio Enabled | Ctrl+m"), true, false);
		else
		m.addItem(400, String("Audio Enabled | Ctrl+m"), true, true);


//	if(getPreference(appProperties, "CabbageEnabled"))
//	m.addItem(400, "Audio Enabled | Ctrl+m", true, true); 
//	else
//	m.addItem(400, "Audio Enabled | Ctrl+m", true, false); 
	
    m.addItem(4, TRANS("Audio Settings..."));
    m.addSeparator();
	if(!standaloneMode){	
		m.addItem(100, String("Toggle Edit-mode"));
		m.addSeparator();
		subMenu.clear();
		subMenu.addItem(15, TRANS("Plugin Synth"));
		subMenu.addItem(16, TRANS("Plugin Effect"));
		m.addSubMenu(TRANS("Export..."), subMenu);
#ifndef MACOSX
		subMenu.clear();
		subMenu.addItem(5, TRANS("Plugin Synth"));
		subMenu.addItem(6, TRANS("Plugin Effect"));
		m.addSubMenu(TRANS("Export As..."), subMenu);
#endif
		m.addSeparator();
	}

	if(!standaloneMode){
		m.addItem(8, String("Rebuild Instrument | Ctrl+b"));
		m.addItem(9, String("Rebuild GUI | Ctrl+u"));
	/*
	if(filter->getMidiDebug())
    m.addItem(9, TRANS("Show MIDI Debug Information"), true, true);
	else
	m.addItem(9, TRANS("Show MIDI Debug Information"));
	*/
		subMenu.clear();
		subMenu.addItem(11, TRANS("Effects"));
		subMenu.addItem(12, TRANS("Synths"));
		//m.addSubMenu(TRANS("Batch Convert"), subMenu);
		m.addSeparator();

/*
	m.addSeparator();
	if(cabbageDance)
	m.addItem(99, String("Cabbage Dance"), true, true);
	else
	m.addItem(99, String("Cabbage Dance"));
*/
		subMenu.clear();
		
		if(getPreference(appProperties, "SetAlwaysOnTop"))
		subMenu.addItem(7, String("Always on Top"), true, true);
		else
		subMenu.addItem(7, String("Always on Top"), true, false);
		//preferences....
		subMenu.addItem(203, "Set Cabbage Plant Directory");
		subMenu.addItem(200, "Set Csound Manual Directory");
		subMenu.addItem(205, "Set Examples Directory");
		if(!getPreference(appProperties, "DisablePluginInfo"))
		subMenu.addItem(201, String("Disable Export Plugin Info"), true, false);
		else
		subMenu.addItem(201, String("Disable Export Plugin Info"), true, true);


		if(!getPreference(appProperties, "AutoUpdate"))
		subMenu.addItem(299, String("Auto-update"), true, false);
		else
		subMenu.addItem(299, String("Auto-update"), true, true);

		if(!getPreference(appProperties, "DisableGUIEditModeWarning"))
		subMenu.addItem(202, String("Disable GUI Edit Mode warning"), true, true);
		else
		subMenu.addItem(202, String("Disable GUI Edit Mode warning"), true, false);

		if(!getPreference(appProperties, "EnablePopupDisplay"))
		subMenu.addItem(207, String("Enable opcode popup help display"), true, false);
		else
		subMenu.addItem(207, String("Enable opcode popup help display"), true, true);


		if(!getPreference(appProperties, "UseCabbageIO"))
		subMenu.addItem(204, String("Use Cabbage IO"), true, false);
		else
		subMenu.addItem(204, String("Use Cabbage IO"), true, true);
#ifndef LINUX
		if(!getPreference(appProperties, "Using Cabbage-Csound"))
		subMenu.addItem(206, String("Using Cabbage-Csound"), true, false);
		else
		subMenu.addItem(206, String("Using Cabbage-Csound"), true, true);
#endif
		m.addSubMenu("Preferences", subMenu);
		
		m.addItem(2000, "About");
		//m.addItem(2345, "Test button");
	}

	int options = m.showAt (&optionsButton);
 
	//----- open file ------
	if(options==1){
		openFile("");
	}
	else if(options==2345){
//	filter->removeGUIComponent(2);
	}
	else if((options>=examplesOffset) && (options<=examplesOffset+exampleFiles.size())){
		openFile(exampleFiles[options-examplesOffset].getFullPathName());	
	}

	else if(options>=9000){
		openFile(recentFiles.getFile(options-9000).getFullPathName());
	}
	else if(options==2000){
		String credits = "				Rory Walsh, Copyright (2008)\n\n";
		credits.append("\t\t\t\tLook and Feel Developer:\n", 2056);
		credits.append("\t\t\t\t\tDamien Rennick\n\n", 2056);
		credits.append("\t\t\t\tCabbage Farmers:\n", 2056);
		credits.append("\t\t\t\t\tIain McCurdy\n", 2056);
		credits.append("\t\t\t\t\tGiorgio Zucco\n", 2056);
		credits.append("\t\t\t\t\tNil Geisweiller\n", 2056);
		credits.append("\t\t\t\t\tDave Philips\n", 2056);
		credits.append("\t\t\t\t\tEamon Brady\n\n", 2056);
		credits.append("\t\t\t\tUsers Forum:\n", 2056);
		credits.append("\t\t\t\t\twww.thecabbagefoundation.org", 2056);
		String title(CABBAGE_VERSION);
		showMessage("			"+title, credits, lookAndFeel, this);
		
	}
	//----- view text editor ------
	else if(options==2){
		if(csdFile.getFileName().length()>0){
			if(!cabbageCsoundEditor){
			cabbageCsoundEditor = new CodeWindow(csdFile.getFileName());
			cabbageCsoundEditor->setVisible(false);
#ifndef MACOSX
			cabbageCsoundEditor->setFullScreen(true);
#endif
			cabbageCsoundEditor->addActionListener(this);
			cabbageCsoundEditor->setLookAndFeel(lookAndFeel);
			filter->codeEditor = cabbageCsoundEditor->textEditor;
			}
			//cabbageCsoundEditor->setText(csdFile.loadFileAsString());
			this->toBehind(cabbageCsoundEditor);
			cabbageCsoundEditor->setVisible(true);
#ifndef MACOSX
			cabbageCsoundEditor->setFullScreen(true);
#endif
			cabbageCsoundEditor->toFront(true);
			
			
			if(!outputConsole){
			outputConsole = new CsoundMessageConsole("Csound Output Messages", 
														Colours::black, 
														getPosition().getY()+getHeight(),
														getPosition().getX());												
			outputConsole->setLookAndFeel(lookAndFeel);
			outputConsole->setText(filter->getCsoundOutput());
			outputConsole->setAlwaysOnTop(true);
			outputConsole->toFront(true);
			outputConsole->setVisible(true);
			}
		}
		else showMessage("Please open or create a file first", lookAndFeel);
	}
	//-------Csound output console-----
	else if(options==3){
	if(csdFile.getFullPathName().length()>0){
		if(!outputConsole){
		outputConsole = new CsoundMessageConsole("Csound Output Messages", 
													Colours::black, 
													getPosition().getY()+getHeight(),
													getPosition().getX());												
		outputConsole->setLookAndFeel(lookAndFeel);
		outputConsole->setText(filter->getCsoundOutput());
		outputConsole->setAlwaysOnTop(true);
		outputConsole->toFront(true);
		outputConsole->setVisible(true);
		}
		else{
			outputConsole->setAlwaysOnTop(true);
			outputConsole->toFront(true);
			outputConsole->setVisible(true);
			}
		}
		else showMessage("Please open or create a file first", lookAndFeel);
	}
	//----- new effect ------
	else if(options==30){
	if(!cabbageCsoundEditor){
		cabbageCsoundEditor = new CodeWindow("");
		cabbageCsoundEditor->addActionListener(this);
		cabbageCsoundEditor->setLookAndFeel(lookAndFeel);
		filter->codeEditor = cabbageCsoundEditor->textEditor;
		}
		cabbageCsoundEditor->setVisible(true);
		cabbageCsoundEditor->newFile("effect");
		saveFileAs();
		//cabbageCsoundEditor->csoundEditor->textEditor->grabKeyboardFocus();
		isAFileOpen = true;
	}
	//----- new instrument ------
	else if(options==31){
	if(!cabbageCsoundEditor){
		cabbageCsoundEditor = new CodeWindow("");
		cabbageCsoundEditor->setLookAndFeel(lookAndFeel);
		cabbageCsoundEditor->addActionListener(this);
		filter->codeEditor = cabbageCsoundEditor->textEditor;
		}
	cabbageCsoundEditor->setVisible(true);
	cabbageCsoundEditor->newFile("instrument");
	saveFileAs();
	//cabbageCsoundEditor->csoundEditor->textEditor->grabKeyboardFocus();
	isAFileOpen = true;
	}
	//----- audio settings ------
   	else if(options==4){
        showAudioSettingsDialog();
		int val = getPreference(appProperties, "UseCabbageIO");
		if(!val)
		resetFilter(true);
	}

	//suspend audio
   	else if(options==400){
        if(getPreference(appProperties, "AudioEnabled")){
			filter->suspendProcessing(true);
			setPreference(appProperties, "AudioEnabled", 0);
		}
		else{
			AudioEnabled = true;
			filter->suspendProcessing(false);
			setPreference(appProperties, "AudioEnabled", 1);
		}
	}

	//----- export ------
	else if(options==15)
	exportPlugin(String("VSTi"), false);
	
	else if(options==16)
	exportPlugin(String("VST"), false);


	//----- export as ------
	else if(options==5)
	exportPlugin(String("VSTi"), true);
	
	else if(options==6)
	exportPlugin(String("VST"), true);

	//----- always on top  ------
	else if(options==7)

	if(getPreference(appProperties, "SetAlwaysOnTop")){
		this->setAlwaysOnTop(false);
		setPreference(appProperties, "SetAlwaysOnTop", 0);
	}
	else{
		this->setAlwaysOnTop(true);
		setPreference(appProperties, "SetAlwaysOnTop", 1);
	}
	
	//----- update instrument  ------
    else if(options==8){
        resetFilter(false);
		//filter->reCompileCsound();
	}
	//----- update GUI only -----
	else if(options==9)
	filter->createGUI(csdFile.loadFileAsString(), true);

	//----- batch process ------
	else if(options==11)
		BatchProcess(String("VST"));

	else if(options==12)
		BatchProcess(String("VSTi"));

	//----- auto-update file when saved remotely ------
	else if(options==299){
		int val = getPreference(appProperties, "AutoUpdate");
		if(val==0){
			setPreference(appProperties, "AutoUpdate", 1);
			startTimer(100);	
			}
		else{
			setPreference(appProperties, "AutoUpdate", 0);
			stopTimer();
		}
		}
/*
	//------- cabbage dance ------
	else if(options==99){
		if(!cabbageDance){
		startTimer(20);
		cabbageDance = true;
		}
		else{
		stopTimer();
		cabbageDance = false;
		}
	}
	*/
	//------- preference Csound manual dir ------
	else if(options==200){
		String dir = getPreference(appProperties, "CsoundHelpDir", "");
		FileChooser browser(String("Please select the Csound manual directory...\n(Windows: Program Files\\Csound)\n(OSX /Library/Frameworks/CsoundLib64.framework/Versions/6.0/Resources/Manual)"), File(dir), String("*.csd"));
		if(browser.browseForDirectory()){
			setPreference(appProperties, "CsoundHelpDir", browser.getResult().getFullPathName());
		}	
	}
	//------- preference Csound manual dir ------
	else if(options==203){
		String dir = getPreference(appProperties, "PlantFileDir", "");
		FileChooser browser(String("Please select your Plant file directory..."), File(dir), String("*.csd"));
		if(browser.browseForDirectory()){
			setPreference(appProperties, "PlantFileDir", browser.getResult().getFullPathName());
		}	
	}
	//--------preference Csound IO
	else if(options==204){
		int val = getPreference(appProperties, "UseCabbageIO");
		if(val==0) 
			setPreference(appProperties, "UseCabbageIO", 1);
		else
			setPreference(appProperties, "UseCabbageIO", 0);
	}
	
	//--------using Cabbage-Csound
	else if(options==206){
		String homeFolder = File::getSpecialLocation(File::currentExecutableFile).getParentDirectory().getFullPathName();
		int val = getPreference(appProperties, "Using Cabbage-Csound");
		if(val==0){ 
			setPreference(appProperties, "Using Cabbage-Csoun", 1);
			String homeFolder = File::getSpecialLocation(File::currentExecutableFile).getParentDirectory().getFullPathName();
			String pluginFolder = homeFolder+"\\Disabled_CsoundPlugins";
			String csoundDLL = homeFolder+"\\Disabled_csound64.dll";
			if(!File(pluginFolder).moveFileTo(File(homeFolder+"\\CsoundPlugins")))
				showMessage("Could not find Csound plugins folder?", &getLookAndFeel());
			if(!File(csoundDLL).moveFileTo(File(homeFolder+"\\csound64.dll")))
				showMessage("Could not find Csound library dll?", &getLookAndFeel());			
		}
		else{
			setPreference(appProperties, "Using Cabbage-Csound", 0);
			String pluginFolder = homeFolder+"\\CsoundPlugins";
			String csoundDLL = homeFolder+"\\csound64.dll";
			if(!File(pluginFolder).moveFileTo(File(homeFolder+"\\Disabled_CsoundPlugins")))
				showMessage("Could not find Csound plugins folder?", &getLookAndFeel());
			if(!File(csoundDLL).moveFileTo(File(homeFolder+"\\Disabled_csound64.dll")))
				showMessage("Could not find Csound library dll?", &getLookAndFeel());
			
		}
	}	
	
	//------- preference Examples dir ------
	else if(options==205){
		String dir = getPreference(appProperties, "ExamplesDir", "");
		FileChooser browser(String("Please select your Examples directory..."), File(dir), String("*.csd"));
		if(browser.browseForDirectory()){
			setPreference(appProperties, "ExamplesDir", browser.getResult().getFullPathName());
		}	
	}
	
	//------- preference plugin info ------
	else if(options==201){
		int val = getPreference(appProperties, "DisablePluginInfo");
		if(val==0)
			appProperties->getUserSettings()->setValue("DisablePluginInfo", var(1));
		else
			appProperties->getUserSettings()->setValue("DisablePluginInfo", var(0));
	}
	
	//------- preference popup display ------
	else if(options==207){
		int val = getPreference(appProperties, "EnablePopupDisplay");
		if(val==0)
			appProperties->getUserSettings()->setValue("EnablePopupDisplay", var(1));
		else
			appProperties->getUserSettings()->setValue("EnablePopupDisplay", var(0));
	}
	
	//------- preference disable gui edit warning ------
	else if(options==202){
		int val = getPreference(appProperties, "DisableGUIEditModeWarning");
		if(val==0) 
			setPreference(appProperties, "DisableGUIEditModeWarning", 1);
		else
			setPreference(appProperties, "DisableGUIEditModeWarning", 0);
	}
	//------- enable GUI edito mode------
	else if(options==100){
		int val = getPreference(appProperties, "DisableGUIEditModeWarning");
		if(val)
			showMessage("", "Warning!! This feature is bleeding edge! (that's programmer speak for totally untested and likely to crash hard!). If you like to live on the edge, disable this warning under the 'Preferences' menu command and try 'Edit Mode' again, otherwise just let it be...", lookAndFeel);
		else{
			openTextEditor();
	if(isAFileOpen == true)
		if(filter->isGuiEnabled()){
		startTimer(100);
		((CabbagePluginAudioProcessorEditor*)filter->getActiveEditor())->setEditMode(false);
		filter->setGuiEnabled(false);
		}
		else{
		((CabbagePluginAudioProcessorEditor*)filter->getActiveEditor())->setEditMode(true);
		filter->setGuiEnabled(true);
		stopTimer();
		setPreference(appProperties, "AutoUpdate", 0);
		}
	else showMessage("", "Open or create a file first", &getLookAndFeel(), this);
		}
	}
	repaint();
}

//==============================================================================
// open text editor
//==============================================================================
void StandaloneFilterWindow::openTextEditor()
{
		if(csdFile.getFileName().length()>0){
			cabbageCsoundEditor->setText(filter->getCsoundInputFileText());
			this->toBehind(cabbageCsoundEditor);
			cabbageCsoundEditor->setVisible(true);
#ifndef MACOSX
//			cabbageCsoundEditor->setFullScreen(true);
#endif
			cabbageCsoundEditor->toFront(true);			
			
			if(!outputConsole){
			outputConsole = new CsoundMessageConsole("Csound Output Messages", 
														Colours::black, 
														getPosition().getY()+getHeight(),
														getPosition().getX());												
			outputConsole->setLookAndFeel(lookAndFeel);
			outputConsole->setText(filter->getCsoundOutput());
			outputConsole->setAlwaysOnTop(true);
			outputConsole->toFront(true);
			outputConsole->setVisible(true);
			}
		}
		else showMessage("Please open or create a file first", lookAndFeel);	
}

//==========================================================================


//==============================================================================
// open/save/save as methods
//==============================================================================
void StandaloneFilterWindow::openFile(String _csdfile)
{
if(_csdfile.length()>4){
	csdFile = File(_csdfile);
	originalCsdFile = csdFile;
	lastSaveTime = csdFile.getLastModificationTime();
	csdFile.setAsCurrentWorkingDirectory();
	resetFilter(true);
}	
else{	
#ifdef MACOSX
		FileChooser openFC(String("Open a Cabbage .csd file..."), File::nonexistent, String("*.csd;*.vst"));
		if(openFC.browseForFileToOpen()){
			csdFile = openFC.getResult();
			originalCsdFile = openFC.getResult();
			lastSaveTime = csdFile.getLastModificationTime();
			csdFile.setAsCurrentWorkingDirectory();
			if(csdFile.getFileExtension()==(".vst")){
				String csd = csdFile.getFullPathName();
				csd << "/Contents/" << csdFile.getFileNameWithoutExtension() << ".csd";
				csdFile = File(csd);
			}
			
			cabbageCsoundEditor->setText(csdFile.loadFileAsString());
			cabbageCsoundEditor->textEditor->setAllText(csdFile.loadFileAsString());
			isAFileOpen = true;
			resetFilter(true);
		}			
#else
		FileChooser openFC(String("Open a Cabbage .csd file..."), File::nonexistent, String("*.csd"));
		this->setAlwaysOnTop(false);
		if(openFC.browseForFileToOpen()){
			csdFile = openFC.getResult();
			csdFile.getParentDirectory().setAsCurrentWorkingDirectory();
			lastSaveTime = csdFile.getLastModificationTime();
			resetFilter(true);
			cabbageCsoundEditor->setText(csdFile.loadFileAsString());
			cabbageCsoundEditor->textEditor->setAllText(csdFile.loadFileAsString());
			isAFileOpen = true;
		}
		if(getPreference(appProperties, "SetAlwaysOnTop"))
			setAlwaysOnTop((true));
		else
			setAlwaysOnTop(false);
#endif
	}

}

void StandaloneFilterWindow::saveFile()
{
if(csdFile.hasWriteAccess())
csdFile.replaceWithText(cabbageCsoundEditor->getText());
resetFilter(false);
//cabbageCsoundEditor->grabKeyboardFocus();
//cabbageCsoundEditor->csoundEditor->grabKeyboardFocus();
RecentlyOpenedFilesList recentFiles;
recentFiles.restoreFromString (appProperties->getUserSettings()->getValue ("recentlyOpenedFiles"));
																			recentFiles.addFile (csdFile);
appProperties->getUserSettings()->setValue ("recentlyOpenedFiles", 
											recentFiles.toString());	
		
filter->saveText();
}

void StandaloneFilterWindow::saveFileAs()
{
FileChooser saveFC(String("Save Cabbage file as..."), File::nonexistent, String("*.csd"));
	this->setAlwaysOnTop(false);
	if(saveFC.browseForFileToSave(true)){
		csdFile = saveFC.getResult().withFileExtension(String(".csd"));
		csdFile.replaceWithText(cabbageCsoundEditor->getText());
		cabbageCsoundEditor->setText(csdFile.loadFileAsString());
		resetFilter(true);
	}
	if(getPreference(appProperties, "SetAlwaysOnTop"))
		setAlwaysOnTop(true);
	else
		setAlwaysOnTop(false);

RecentlyOpenedFilesList recentFiles;
recentFiles.restoreFromString (appProperties->getUserSettings()->getValue ("recentlyOpenedFiles"));
																			recentFiles.addFile (csdFile);
appProperties->getUserSettings()->setValue ("recentlyOpenedFiles", 
											recentFiles.toString());	
filter->saveText();

}
//==============================================================================
// Export plugin method
//==============================================================================
int StandaloneFilterWindow::exportPlugin(String type, bool saveAs)
{
File dll;
File loc_csdFile;
#ifndef LINUX
File thisFile(File::getSpecialLocation(File::currentApplicationFile));
#else
File thisFile(File::getSpecialLocation(File::currentExecutableFile));
#endif

if(!csdFile.exists()){
					showMessage("", "You need to open a Cabbage instrument before you can export one as a plugin!", lookAndFeel, this);
					return 0;
				}
#ifdef LINUX
	FileChooser saveFC(String("Save as..."), File::nonexistent, String(""));
	String VST;
	Logger::writeToLog(currentApplicationDirectory);
	if (saveFC.browseForFileToSave(true)){
		if(type.contains("VSTi"))
			VST = currentApplicationDirectory + String("/CabbagePluginSynth.so");
		else if(type.contains(String("VST")))
			VST = currentApplicationDirectory + String("/CabbagePluginEffect.so");
		else if(type.contains(String("AU"))){
			showMessage("", "This feature only works on computers running OSX", lookAndFeel, this);
		}
		//Logger::writeToLog(VST);
		//showMessage(VST);
		File VSTData(VST);
		if(!VSTData.exists()){
		this->setMinimised(true);
		showMessage("", VST+" cannot be found?", lookAndFeel, this);
		}
		
		else{
			File dll(saveFC.getResult().withFileExtension(".so").getFullPathName());
			Logger::writeToLog(dll.getFullPathName());
			if(!VSTData.copyFileTo(dll))	showMessage("", "Can not move lib", lookAndFeel, this);
			File loc_csdFile(saveFC.getResult().withFileExtension(".csd").getFullPathName());
			loc_csdFile.replaceWithText(csdFile.loadFileAsString());
		}
	}
#elif WIN32
	FileChooser saveFC(String("Save plugin as..."), File::nonexistent, String("*.dll"));
	String VST;
		if(type.contains("VSTi"))
			VST = thisFile.getParentDirectory().getFullPathName() + String("\\CabbagePluginSynth.dat");
		else if(type.contains(String("VST")))
			VST = thisFile.getParentDirectory().getFullPathName() + String("\\CabbagePluginEffect.dat");

		File VSTData(VST);

		if(!VSTData.exists()){
			showMessage("", "Cabbage cannot find the plugin libraries. Make sure that Cabbage is situated in the same directory as CabbagePluginSynth.dat and CabbagePluginEffect.dat", oldLookAndFeel, this);
			return 0;
		}
		else{
			if(saveAs){
			if (saveFC.browseForFileToSave(true)){
			dll = saveFC.getResult().withFileExtension(".dll").getFullPathName();
			loc_csdFile = saveFC.getResult().withFileExtension(".csd").getFullPathName();
			}
			else
				return 0;
			}
			else{
			dll = csdFile.withFileExtension(".dll").getFullPathName();
			loc_csdFile = csdFile.withFileExtension(".csd").getFullPathName();
			
			}
			//showMessage(dll.getFullPathName());
			if(!VSTData.copyFileTo(dll))	
				showMessage("", "Problem moving plugin lib, make sure it's not currently open in your plugin host!", lookAndFeel, this);
			
			loc_csdFile.replaceWithText(csdFile.loadFileAsString());
			setUniquePluginID(dll, loc_csdFile, false);
			String info;
			info = String("Your plugin has been created. It's called:\n\n")+dll.getFullPathName()+String("\n\nIn order to modify this plugin you only have to edit the associated .csd file. You do not need to export every time you make changes.\n\nTo turn off this notice visit 'Preferences' in the main 'options' menu");
			
			int val = getPreference(appProperties, "DisablePluginInfo");
			if(!val)
			showMessage(info, lookAndFeel);
			
#ifdef Cabbage_Named_Pipe
			sendMessageToWinXound("CABBAGE_PLUGIN_FILE_UPDATE", csdFile.getFullPathName()+String("|")+loc_csdFile.getFullPathName());
			csdFile = loc_csdFile;	
			cabbageCsoundEditor->setName(csdFile.getFullPathName());
			sendMessageToWinXound("CABBAGE_SHOW_MESSAGE|Info", "WinXound has been updated\nyour .csd file");
#endif
		}

#endif
	
#if MACOSX
	
	FileChooser saveFC(String("Save as..."), File::nonexistent, String(".vst"));
	String VST;
	if (saveFC.browseForFileToSave(true)){
		//showMessage("name of file is:");
		//showMessage(saveFC.getResult().withFileExtension(".vst").getFullPathName());
				
		if(type.contains("VSTi"))
			//VST = thisFile.getParentDirectory().getFullPathName() + String("//CabbagePluginSynth.dat");
			VST = thisFile.getFullPathName()+"/Contents/CabbagePluginSynth.component";
		else if(type.contains(String("VST")))
			//VST = thisFile.getParentDirectory().getFullPathName() + String("//CabbagePluginEffect.dat");
			VST = thisFile.getFullPathName()+"/Contents/CabbagePluginEffect.component";
		else if(type.contains(String("AU"))){
			showMessage("this feature is coming soon");
			//VST = thisFile.getParentDirectory().getFullPathName() + String("\\CabbageVSTfx.component");
		}

		//create a copy of the data package and write it to the new location given by user
		File VSTData(VST);
		if(!VSTData.exists()){
			showMessage("Cabbage cannot find the plugin libraries. Make sure that Cabbage is situated in the same directory as CabbagePluginSynth.dat and CabbagePluginEffect.dat", lookAndFeel);
			return 0;
		}
		
		
		String plugType = ".vst";
//		if(type.contains(String("AU")))
//			plugType = String(".component");
//		else plugType = String(".vst");
		
		File dll(saveFC.getResult().withFileExtension(plugType).getFullPathName());
		
		VSTData.copyFileTo(dll);
		//showMessage("copied");
		
		
		
		File pl(dll.getFullPathName()+String("/Contents/Info.plist"));
		String newPList = pl.loadFileAsString();
		//write our identifiers to the plist file 
		newPList = newPList.replace("CabbagePlugin", saveFC.getResult().getFileNameWithoutExtension());
		newPList = newPList.replace("NewProject", saveFC.getResult().getFileNameWithoutExtension());
		//write plist file
		pl.replaceWithText(newPList);

		
		
		File loc_csdFile(dll.getFullPathName()+String("/Contents/")+saveFC.getResult().getFileNameWithoutExtension()+String(".csd"));		
		loc_csdFile.replaceWithText(csdFile.loadFileAsString());		
		//showMessage(loc_csdFile.getFullPathName());
		//showMessage(csdFile.loadFileAsString());
		csdFile = loc_csdFile;		
		
		//showMessage(VST+String("/Contents/MacOS/")+saveFC.getResult().getFileNameWithoutExtension());
		//if plugin already exists there is no point in rewriting the binaries
		if(!File(saveFC.getResult().withFileExtension(".vst").getFullPathName()+("/Contents/MacOS/")+saveFC.getResult().getFileNameWithoutExtension()).exists()){		
		File bin(dll.getFullPathName()+String("/Contents/MacOS/CabbagePlugin"));
			//if(bin.exists())showMessage("binary exists");
		
				
			File pluginBinary(dll.getFullPathName()+String("/Contents/MacOS/")+saveFC.getResult().getFileNameWithoutExtension());
			
			bin.moveFileTo(pluginBinary);				
			//else
			//showMessage("could not copy library binary file");
		
		setUniquePluginID(pluginBinary, loc_csdFile, true);
		}
		
		String info;
		info = String("Your plugin has been created. It's called:\n\n")+dll.getFullPathName()+String("\n\nIn order to modify this plugin you only have to edit the current .csd file. You do not need to export every time you make changes. To open the current csd file with Cabbage in another session, go to 'Open Cabbage Instrument' and select the .vst file. Cabbage will the load the associated .csd file. \n\nTo turn off this notice visit 'Preferences' in the main 'options' menu");
		
		int val = appProperties->getUserSettings()->getValue("DisablePluginInfo", var(0)).getFloatValue();
		if(!val)
			showMessage(info, lookAndFeel);		
		

	}
	
#endif		
	
	return 0;	
}

//==============================================================================
// Set unique plugin ID for each plugin based on the file name 
//==============================================================================
int StandaloneFilterWindow::setUniquePluginID(File binFile, File csdFile, bool AU){
	String newID;
	StringArray csdText;
	csdText.addLines(csdFile.loadFileAsString());
	//read contents of csd file to find pluginID
	for(int i=0;i<csdText.size();i++)
    {
		StringArray tokes;
		tokes.addTokens(csdText[i].trimEnd(), ", ", "\"");
		if(tokes[0].equalsIgnoreCase(String("form"))){
			CabbageGUIClass cAttr(csdText[i].trimEnd(), 0);		
			if(cAttr.getStringProp("pluginID").length()!=4){
				showMessage(String("Your plugin ID is not the right size. It MUST be 4 characters long. Some hosts may not be able to load your plugin"), lookAndFeel);
				return 0;
			}
			else{
				newID = cAttr.getStringProp("pluginID");
				i = csdText.size();
			}			
		}
	}
	
	size_t file_size;
	const char *pluginID;
	//if(!AU)
		pluginID = "YROR";
	//else
	//	pluginID = "RORY";
	
	
	long loc;
	//showMessage(binFile.getFullPathName(), lookAndFeel);
	fstream mFile(binFile.getFullPathName().toUTF8(), ios_base::in | ios_base::out | ios_base::binary);
	if(mFile.is_open())
	{
		mFile.seekg (0, ios::end);
		file_size = mFile.tellg();
		unsigned char* buffer = (unsigned char*)malloc(sizeof(unsigned char)*file_size);
		//set plugin ID, do this a few times in case the plugin ID appear in more than one place.
		for(int r=0;r<10;r++){
			mFile.seekg (0, ios::beg);
			
			mFile.read((char*)&buffer[0], file_size);
			loc = cabbageFindPluginID(buffer, file_size, pluginID);
			if (loc < 0)
				//showMessage(String("Internel Cabbage Error: The pluginID was not found"));
				break;
			else {
				//showMessage("The plugin ID was found!");
				mFile.seekg (loc, ios::beg);	
				mFile.write(newID.toUTF8(), 4);	
			}
		}
		
		//set plugin name based on .csd file
		const char *pluginName = "CabbageEffectNam";
		String plugLibName = csdFile.getFileNameWithoutExtension();
		if(plugLibName.length()<16)
			for(int y=plugLibName.length();y<16;y++)
				plugLibName.append(String(" "), 1);
		
		mFile.seekg (0, ios::beg);
		buffer = (unsigned char*)malloc(sizeof(unsigned char)*file_size);
		mFile.read((char*)&buffer[0], file_size);
		loc = cabbageFindPluginID(buffer, file_size, pluginName);
		if (loc < 0)
			showMessage(String("Plugin name could not be set?!?"), lookAndFeel);
		else {
			//showMessage("plugin name set!");
			mFile.seekg (loc, ios::beg);	
			mFile.write(csdFile.getFileNameWithoutExtension().toUTF8(), 16);	
		}
		//#endif
		
	}
else
	showMessage("File could not be opened", lookAndFeel);
	
mFile.close();
return 1;
}


//==============================================================================
// Batch process multiple csd files to convert them to plugins libs. 
//==============================================================================
void StandaloneFilterWindow::BatchProcess(String type){
File thisFile(File::getSpecialLocation(File::currentApplicationFile));
#ifdef WIN32  
FileChooser saveFC(String("Select files..."), File::nonexistent, String(""));
String VST;
	if (saveFC.browseForMultipleFilesToOpen()){
		if(type.contains("VSTi"))
			VST = thisFile.getParentDirectory().getFullPathName() + String("\\CabbagePluginSynth.dat");
		else if(type.contains(String("VST")))
			VST = thisFile.getParentDirectory().getFullPathName() + String("\\CabbagePluginEffect.dat");
		else if(type.contains(String("AU"))){
			showMessage("This feature only works on computers running OSX");
			}
	//showMessage(VST);
	File VSTData(VST);
	if(!VSTData.exists())  
		showMessage("problem with plugin lib");
	else{
		for(int i=0;i<saveFC.getResults().size();i++){
			File dll(saveFC.getResults().getReference(i).withFileExtension(".dll").getFullPathName());
		//showMessage(dll.getFullPathName());
		if(!VSTData.copyFileTo(dll))	
			showMessage("problem moving plugin lib");
		else{
		//File loc_csdFile(saveFC.getResults().getReference(i).withFileExtension(".csd").getFullPathName());
		//loc_csdFile.replaceWithText(csdFile.loadFileAsString());
		}
		}
		showMessage("Batch Convertion Complete");
	}
	}
#endif
}
