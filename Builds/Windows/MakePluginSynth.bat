
ECHO OFF
ECHO =============Setting Build flags===================
ECHO * You will need to edit this bat file in order to *
ECHO * setup the correct directories and provide paths *
ECHO * to the Csound library                           *
ECHO ===================================================
ECHO 
set JuceLibraryCode=C:\Users\Rory\Documents\SourceCode\cabbageaudio\cabbage\JuceLibraryCode
set PreProcessorDefs=-D__MINGW__=1 -D__MINGW_EXTENSION= -DCSOUND6 -D Cabbage_Plugin_Synth=1 -DUSE_DOUBLE=1 -DCabbage_GUI_Editor=1 -DJUCER_CODEBLOCKS_20734A5D=1
set CsoundIncludeDir=C:\Users\Rory\Documents\SourceCode\cabbageaudio\csound6-git\include
set ASIOSDK2Dir=C:\SDKs\ASIOSDK2.3\common
set VSTSDKDir=C:\SDKs\vstsdk2.4
set CabbageSourceDir=C:\Users\Rory\Documents\SourceCode\cabbageaudio\cabbage\Source
set CsoundLibrary=C:\Users\Rory\Documents\SourceCode\cabbageaudio\csound6-git\build\libcsound64.dll.a
set OUTPUT_TARGET=CabbagePluginSynth.dll
ECHO on

g++.exe -Wall -Wno-strict-aliasing -Wno-strict-overflow  -march=pentium4 -g -O0 -std=gnu++0x -mstackrealign %PreProcessorDefs%    -I. -I..\..\JuceLibraryCode -I%CsoundIncludeDir% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %JuceLibraryCode%\modules\juce_audio_plugin_client\utility\juce_PluginUtilities.cpp -o build\intermediate\Debug\juce_PluginUtilities.o

g++.exe -Wall -Wno-strict-aliasing -Wno-strict-overflow  -march=pentium4 -g -O0 -std=gnu++0x -mstackrealign %PreProcessorDefs%    -I. -I..\..\JuceLibraryCode -I%CsoundIncludeDir% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %JuceLibraryCode%\modules\juce_audio_plugin_client\VST\juce_VST_Wrapper.cpp -o build\intermediate\Debug\juce_VST_Wrapper.o

g++.exe -Wall -Wno-strict-aliasing -Wno-strict-overflow  -march=pentium4 -g -O0 -std=gnu++0x -mstackrealign %PreProcessorDefs% -I. -I..\..\JuceLibraryCode -I%CsoundIncludeDir% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %JuceLibraryCode%\modules\juce_audio_basics\juce_audio_basics.cpp -o build\intermediate\Debug\juce_audio_basics.o

g++.exe -Wall -Wno-strict-aliasing -Wno-strict-overflow  -march=pentium4 -g -O0 -std=gnu++0x -mstackrealign %PreProcessorDefs%    -I. -I..\..\JuceLibraryCode -I%CsoundIncludeDir% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %JuceLibraryCode%\modules\juce_audio_devices\juce_audio_devices.cpp -o build\intermediate\Debug\juce_audio_devices.o

g++.exe -Wall -Wno-strict-aliasing -Wno-strict-overflow  -march=pentium4 -g -O0 -std=gnu++0x -mstackrealign %PreProcessorDefs%    -I. -I..\..\JuceLibraryCode -I%CsoundIncludeDir% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %JuceLibraryCode%\modules\juce_audio_formats\juce_audio_formats.cpp -o build\intermediate\Debug\juce_audio_formats.o

g++.exe -Wall -Wno-strict-aliasing -Wno-strict-overflow  -march=pentium4 -g -O0 -std=gnu++0x -mstackrealign %PreProcessorDefs%    -I. -I..\..\JuceLibraryCode -I%CsoundIncludeDir% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %JuceLibraryCode%\modules\juce_audio_processors\juce_audio_processors.cpp -o build\intermediate\Debug\juce_audio_processors.o

g++.exe -Wall -Wno-strict-aliasing -Wno-strict-overflow  -march=pentium4 -g -O0 -std=gnu++0x -mstackrealign %PreProcessorDefs%    -I. -I..\..\JuceLibraryCode -I%CsoundIncludeDir% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %JuceLibraryCode%\modules\juce_audio_utils\juce_audio_utils.cpp -o build\intermediate\Debug\juce_audio_utils.o

g++.exe -Wall -Wno-strict-aliasing -Wno-strict-overflow  -march=pentium4 -g -O0 -std=gnu++0x -mstackrealign %PreProcessorDefs%    -I. -I..\..\JuceLibraryCode -I%CsoundIncludeDir% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %JuceLibraryCode%\modules\juce_core\juce_core.cpp -o build\intermediate\Debug\juce_core.o

g++.exe -Wall -Wno-strict-aliasing -Wno-strict-overflow  -march=pentium4 -g -O0 -std=gnu++0x -mstackrealign %PreProcessorDefs%    -I. -I..\..\JuceLibraryCode -I%CsoundIncludeDir% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %JuceLibraryCode%\modules\juce_cryptography\juce_cryptography.cpp -o build\intermediate\Debug\juce_cryptography.o

g++.exe -Wall -Wno-strict-aliasing -Wno-strict-overflow  -march=pentium4 -g -O0 -std=gnu++0x -mstackrealign %PreProcessorDefs%    -I. -I..\..\JuceLibraryCode -I%CsoundIncludeDir% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %JuceLibraryCode%\modules\juce_data_structures\juce_data_structures.cpp -o build\intermediate\Debug\juce_data_structures.o

g++.exe -Wall -Wno-strict-aliasing -Wno-strict-overflow  -march=pentium4 -g -O0 -std=gnu++0x -mstackrealign %PreProcessorDefs%    -I. -I..\..\JuceLibraryCode -I%CsoundIncludeDir% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %JuceLibraryCode%\modules\juce_events\juce_events.cpp -o build\intermediate\Debug\juce_events.o

g++.exe -Wall -Wno-strict-aliasing -Wno-strict-overflow  -march=pentium4 -g -O0 -std=gnu++0x -mstackrealign %PreProcessorDefs%    -I. -I..\..\JuceLibraryCode -I%CsoundIncludeDir% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %JuceLibraryCode%\modules\juce_graphics\juce_graphics.cpp -o build\intermediate\Debug\juce_graphics.o

g++.exe -Wall -Wno-strict-aliasing -Wno-strict-overflow  -march=pentium4 -g -O0 -std=gnu++0x -mstackrealign %PreProcessorDefs%    -I. -I..\..\JuceLibraryCode -I%CsoundIncludeDir% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %JuceLibraryCode%\modules\juce_gui_basics\juce_gui_basics.cpp -o build\intermediate\Debug\juce_gui_basics.o

g++.exe -Wall -Wno-strict-aliasing -Wno-strict-overflow  -march=pentium4 -g -O0 -std=gnu++0x -mstackrealign %PreProcessorDefs%    -I. -I..\..\JuceLibraryCode -I%CsoundIncludeDir% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %JuceLibraryCode%\modules\juce_gui_extra\juce_gui_extra.cpp -o build\intermediate\Debug\juce_gui_extra.o

g++.exe -Wall -Wno-strict-aliasing -Wno-strict-overflow  -march=pentium4 -g -O0 -std=gnu++0x -mstackrealign %PreProcessorDefs%    -I. -I..\..\JuceLibraryCode -I%CsoundIncludeDir% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %CabbageSourceDir%\BinaryData.cpp -o build\intermediate\Debug\BinaryData.o

g++.exe -Wall -Wno-strict-aliasing -Wno-strict-overflow  -march=pentium4 -g -O0 -std=gnu++0x -mstackrealign %PreProcessorDefs%    -I. -I..\..\JuceLibraryCode -I%CsoundIncludeDir% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %CabbageSourceDir%\CabbageCallOutBox.cpp -o build\intermediate\Debug\CabbageCallOutBox.o

g++.exe -Wall -Wno-strict-aliasing -Wno-strict-overflow  -march=pentium4 -g -O0 -std=gnu++0x -mstackrealign %PreProcessorDefs%    -I. -I..\..\JuceLibraryCode -I%CsoundIncludeDir% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %CabbageSourceDir%\CabbageGUIClass.cpp -o build\intermediate\Debug\CabbageGUIClass.o

g++.exe -Wall -Wno-strict-aliasing -Wno-strict-overflow  -march=pentium4 -g -O0 -std=gnu++0x -mstackrealign %PreProcessorDefs%    -I. -I..\..\JuceLibraryCode -I%CsoundIncludeDir% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %CabbageSourceDir%\CabbageLookAndFeel.cpp -o build\intermediate\Debug\CabbageLookAndFeel.o

g++.exe -Wall -Wno-strict-aliasing -Wno-strict-overflow  -march=pentium4 -g -O0 -std=gnu++0x -mstackrealign %PreProcessorDefs%    -I. -I..\..\JuceLibraryCode -I%CsoundIncludeDir% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %CabbageSourceDir%\CabbageMainPanel.cpp -o build\intermediate\Debug\CabbageMainPanel.o

g++.exe -Wall -Wno-strict-aliasing -Wno-strict-overflow  -march=pentium4 -g -O0 -std=gnu++0x -mstackrealign %PreProcessorDefs%    -I. -I..\..\JuceLibraryCode -I%CsoundIncludeDir% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %CabbageSourceDir%\CabbagePropertiesDialog.cpp -o build\intermediate\Debug\CabbagePropertiesDialog.o

g++.exe -Wall -Wno-strict-aliasing -Wno-strict-overflow  -march=pentium4 -g -O0 -std=gnu++0x -mstackrealign %PreProcessorDefs%    -I. -I..\..\JuceLibraryCode -I%CsoundIncludeDir% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %CabbageSourceDir%\CabbageTable.cpp -o build\intermediate\Debug\CabbageTable.o

g++.exe -Wall -Wno-strict-aliasing -Wno-strict-overflow  -march=pentium4 -g -O0 -std=gnu++0x -mstackrealign %PreProcessorDefs%    -I. -I..\..\JuceLibraryCode -I%CsoundIncludeDir% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %CabbageSourceDir%\csPerfThread.cpp -o build\intermediate\Debug\csPerfThread.o

g++.exe -Wall -Wno-strict-aliasing -Wno-strict-overflow  -march=pentium4 -g -O0 -std=gnu++0x -mstackrealign %PreProcessorDefs%    -I. -I..\..\JuceLibraryCode -I%CsoundIncludeDir% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %CabbageSourceDir%\DirectoryContentsComponent.cpp -o build\intermediate\Debug\DirectoryContentsComponent.o

g++.exe -Wall -Wno-strict-aliasing -Wno-strict-overflow  -march=pentium4 -g -O0 -std=gnu++0x -mstackrealign %PreProcessorDefs%    -I. -I..\..\JuceLibraryCode -I%CsoundIncludeDir% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %CabbageSourceDir%\Plugin\CabbagePluginProcessor.cpp -o build\intermediate\Debug\CabbagePluginProcessor.o

g++.exe -Wall -Wno-strict-aliasing -Wno-strict-overflow  -march=pentium4 -g -O0 -std=gnu++0x -mstackrealign %PreProcessorDefs%    -I. -I..\..\JuceLibraryCode -I%CsoundIncludeDir% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %CabbageSourceDir%\Soundfiler.cpp -o build\intermediate\Debug\Soundfiler.o

g++.exe -Wall -Wno-strict-aliasing -Wno-strict-overflow  -march=pentium4 -g -O0 -std=gnu++0x -mstackrealign %PreProcessorDefs%    -I. -I..\..\JuceLibraryCode -I%CsoundIncludeDir% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %CabbageSourceDir%\XYPad.cpp -o build\intermediate\Debug\XYPad.o

g++.exe -Wall -Wno-strict-aliasing -Wno-strict-overflow  -march=pentium4 -g -O0 -std=gnu++0x -mstackrealign %PreProcessorDefs%    -I. -I..\..\JuceLibraryCode -I%CsoundIncludeDir% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %CabbageSourceDir%\XYPadAutomation.cpp -o build\intermediate\Debug\XYPadAutomation.o

g++.exe -shared -Wl,--dll -o build\%OUTPUT_TARGET% build\intermediate\Debug\juce_audio_basics.o build\intermediate\Debug\juce_audio_devices.o build\intermediate\Debug\juce_audio_formats.o build\intermediate\Debug\juce_audio_processors.o build\intermediate\Debug\juce_audio_utils.o build\intermediate\Debug\juce_core.o build\intermediate\Debug\juce_cryptography.o build\intermediate\Debug\juce_data_structures.o build\intermediate\Debug\juce_events.o build\intermediate\Debug\juce_graphics.o build\intermediate\Debug\juce_gui_basics.o build\intermediate\Debug\juce_gui_extra.o build\intermediate\Debug\juce_PluginUtilities.o build\intermediate\Debug\juce_VST_Wrapper.o build\intermediate\Debug\BinaryData.o build\intermediate\Debug\CabbageCallOutBox.o build\intermediate\Debug\CabbageGUIClass.o build\intermediate\Debug\CabbageLookAndFeel.o build\intermediate\Debug\CabbageMainPanel.o build\intermediate\Debug\CabbagePropertiesDialog.o build\intermediate\Debug\CabbageTable.o build\intermediate\Debug\csPerfThread.o build\intermediate\Debug\DirectoryContentsComponent.o build\intermediate\Debug\CodeWindow.o build\intermediate\Debug\CommandManager.o build\intermediate\Debug\CabbagePluginProcessor.o build\intermediate\Debug\Soundfiler.o build\intermediate\Debug\XYPad.o build\intermediate\Debug\XYPadAutomation.o -lgdi32 -luser32 -lkernel32 -lcomctl32 -lcomdlg32 -limm32 -lole32 -loleaut32 -lrpcrt4 -lshlwapi -luuid -lversion -lwininet -lwinmm -lws2_32 -lwsock32 %CsoundLibrary%  -mwindows cabbage.res
