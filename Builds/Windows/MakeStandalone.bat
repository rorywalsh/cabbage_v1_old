
ECHO OFF
ECHO =============Setting Build flags===================
ECHO * You will need to edit this bat file in order to *
ECHO * setup the correct directories and provide paths *
ECHO * to the Csound library                           *
ECHO ===================================================
ECHO 
set JuceLibraryCode=C:\Users\Rory\Documents\SourceCode\cabbageaudio\cabbage\JuceLibraryCode
set PreProcessorDefs=-D__MINGW__=1 -D__MINGW_EXTENSION= -DCabbage_Build_Standalone=1 -DUSE_DOUBLE=1 -DCabbage_GUI_Editor=1 -DJUCER_CODEBLOCKS_20734A5D=1
set CsoundIncludeDir=C:\Users\Rory\Documents\SourceCode\cabbageaudio\csound6-git\include
set ASIOSDK2Dir=C:\SDKs\ASIOSDK2.3\common
set VSTSDKDir=C:\SDKs\vstsdk2.4
set CsoundLibrary=C:\Users\Rory\Documents\SourceCode\cabbageaudio\csound6-git\build\libcsound64.dll.a
ECHO on

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

g++.exe -Wall -Wno-strict-aliasing -Wno-strict-overflow  -march=pentium4 -g -O0 -std=gnu++0x -mstackrealign %PreProcessorDefs%    -I. -I..\..\JuceLibraryCode -I%CsoundIncludeDir% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c C:\Users\Rory\Documents\SourceCode\cabbageaudio\cabbage\Source\BinaryData.cpp -o build\intermediate\Debug\BinaryData.o

g++.exe -Wall -Wno-strict-aliasing -Wno-strict-overflow  -march=pentium4 -g -O0 -std=gnu++0x -mstackrealign %PreProcessorDefs%    -I. -I..\..\JuceLibraryCode -I%CsoundIncludeDir% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c C:\Users\Rory\Documents\SourceCode\cabbageaudio\cabbage\Source\CabbageAudioDeviceSelectorComponent.cpp -o build\intermediate\Debug\CabbageAudioDeviceSelectorComponent.o

g++.exe -Wall -Wno-strict-aliasing -Wno-strict-overflow  -march=pentium4 -g -O0 -std=gnu++0x -mstackrealign %PreProcessorDefs%    -I. -I..\..\JuceLibraryCode -I%CsoundIncludeDir% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c C:\Users\Rory\Documents\SourceCode\cabbageaudio\cabbage\Source\CabbageCallOutBox.cpp -o build\intermediate\Debug\CabbageCallOutBox.o

g++.exe -Wall -Wno-strict-aliasing -Wno-strict-overflow  -march=pentium4 -g -O0 -std=gnu++0x -mstackrealign %PreProcessorDefs%    -I. -I..\..\JuceLibraryCode -I%CsoundIncludeDir% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c C:\Users\Rory\Documents\SourceCode\cabbageaudio\cabbage\Source\CabbageGUIClass.cpp -o build\intermediate\Debug\CabbageGUIClass.o

g++.exe -Wall -Wno-strict-aliasing -Wno-strict-overflow  -march=pentium4 -g -O0 -std=gnu++0x -mstackrealign %PreProcessorDefs%    -I. -I..\..\JuceLibraryCode -I%CsoundIncludeDir% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c C:\Users\Rory\Documents\SourceCode\cabbageaudio\cabbage\Source\CabbageLookAndFeel.cpp -o build\intermediate\Debug\CabbageLookAndFeel.o
               ^
g++.exe -Wall -Wno-strict-aliasing -Wno-strict-overflow  -march=pentium4 -g -O0 -std=gnu++0x -mstackrealign %PreProcessorDefs%    -I. -I..\..\JuceLibraryCode -I%CsoundIncludeDir% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c C:\Users\Rory\Documents\SourceCode\cabbageaudio\cabbage\Source\CabbageMainPanel.cpp -o build\intermediate\Debug\CabbageMainPanel.o

g++.exe -Wall -Wno-strict-aliasing -Wno-strict-overflow  -march=pentium4 -g -O0 -std=gnu++0x -mstackrealign %PreProcessorDefs%    -I. -I..\..\JuceLibraryCode -I%CsoundIncludeDir% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c C:\Users\Rory\Documents\SourceCode\cabbageaudio\cabbage\Source\CabbagePropertiesDialog.cpp -o build\intermediate\Debug\CabbagePropertiesDialog.o

g++.exe -Wall -Wno-strict-aliasing -Wno-strict-overflow  -march=pentium4 -g -O0 -std=gnu++0x -mstackrealign %PreProcessorDefs%    -I. -I..\..\JuceLibraryCode -I%CsoundIncludeDir% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c C:\Users\Rory\Documents\SourceCode\cabbageaudio\cabbage\Source\CabbageTable.cpp -o build\intermediate\Debug\CabbageTable.o

g++.exe -Wall -Wno-strict-aliasing -Wno-strict-overflow  -march=pentium4 -g -O0 -std=gnu++0x -mstackrealign %PreProcessorDefs%    -I. -I..\..\JuceLibraryCode -I%CsoundIncludeDir% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c C:\Users\Rory\Documents\SourceCode\cabbageaudio\cabbage\Source\ComponentLayoutEditor.cpp -o build\intermediate\Debug\ComponentLayoutEditor.o

g++.exe -Wall -Wno-strict-aliasing -Wno-strict-overflow  -march=pentium4 -g -O0 -std=gnu++0x -mstackrealign %PreProcessorDefs%    -I. -I..\..\JuceLibraryCode -I%CsoundIncludeDir% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c C:\Users\Rory\Documents\SourceCode\cabbageaudio\cabbage\Source\csPerfThread.cpp -o build\intermediate\Debug\csPerfThread.o

g++.exe -Wall -Wno-strict-aliasing -Wno-strict-overflow  -march=pentium4 -g -O0 -std=gnu++0x -mstackrealign %PreProcessorDefs%    -I. -I..\..\JuceLibraryCode -I%CsoundIncludeDir% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c C:\Users\Rory\Documents\SourceCode\cabbageaudio\cabbage\Source\DirectoryContentsComponent.cpp -o build\intermediate\Debug\DirectoryContentsComponent.o

g++.exe -Wall -Wno-strict-aliasing -Wno-strict-overflow  -march=pentium4 -g -O0 -std=gnu++0x -mstackrealign %PreProcessorDefs%    -I. -I..\..\JuceLibraryCode -I%CsoundIncludeDir% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c C:\Users\Rory\Documents\SourceCode\cabbageaudio\cabbage\Source\Editor\CodeEditor.cpp -o build\intermediate\Debug\CodeEditor.o

g++.exe -Wall -Wno-strict-aliasing -Wno-strict-overflow  -march=pentium4 -g -O0 -std=gnu++0x -mstackrealign %PreProcessorDefs%    -I. -I..\..\JuceLibraryCode -I%CsoundIncludeDir% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c C:\Users\Rory\Documents\SourceCode\cabbageaudio\cabbage\Source\Editor\CodeWindow.cpp -o build\intermediate\Debug\CodeWindow.o

g++.exe -Wall -Wno-strict-aliasing -Wno-strict-overflow  -march=pentium4 -g -O0 -std=gnu++0x -mstackrealign %PreProcessorDefs%    -I. -I..\..\JuceLibraryCode -I%CsoundIncludeDir% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c C:\Users\Rory\Documents\SourceCode\cabbageaudio\cabbage\Source\Editor\CommandManager.cpp -o build\intermediate\Debug\CommandManager.o

g++.exe -Wall -Wno-strict-aliasing -Wno-strict-overflow  -march=pentium4 -g -O0 -std=gnu++0x -mstackrealign %PreProcessorDefs%    -I. -I..\..\JuceLibraryCode -I%CsoundIncludeDir% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c C:\Users\Rory\Documents\SourceCode\cabbageaudio\cabbage\Source\Plugin\CabbagePluginEditor.cpp -o build\intermediate\Debug\CabbagePluginEditor.o

g++.exe -Wall -Wno-strict-aliasing -Wno-strict-overflow  -march=pentium4 -g -O0 -std=gnu++0x -mstackrealign %PreProcessorDefs%    -I. -I..\..\JuceLibraryCode -I%CsoundIncludeDir% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c C:\Users\Rory\Documents\SourceCode\cabbageaudio\cabbage\Source\Plugin\CabbagePluginProcessor.cpp -o build\intermediate\Debug\CabbagePluginProcessor.o

g++.exe -Wall -Wno-strict-aliasing -Wno-strict-overflow  -march=pentium4 -g -O0 -std=gnu++0x -mstackrealign %PreProcessorDefs%    -I. -I..\..\JuceLibraryCode -I%CsoundIncludeDir% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c C:\Users\Rory\Documents\SourceCode\cabbageaudio\cabbage\Source\Soundfiler.cpp -o build\intermediate\Debug\Soundfiler.o

g++.exe -Wall -Wno-strict-aliasing -Wno-strict-overflow  -march=pentium4 -g -O0 -std=gnu++0x -mstackrealign %PreProcessorDefs%    -I. -I..\..\JuceLibraryCode -I%CsoundIncludeDir% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c C:\Users\Rory\Documents\SourceCode\cabbageaudio\cabbage\Source\Standalone\CabbageStandaloneDialog.cpp -o build\intermediate\Debug\CabbageStandaloneDialog.o

g++.exe -Wall -Wno-strict-aliasing -Wno-strict-overflow  -march=pentium4 -g -O0 -std=gnu++0x -mstackrealign %PreProcessorDefs%    -I. -I..\..\JuceLibraryCode -I%CsoundIncludeDir% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c C:\Users\Rory\Documents\SourceCode\cabbageaudio\cabbage\Source\Standalone\StandaloneFilterApp.cpp -o build\intermediate\Debug\StandaloneFilterApp.o

g++.exe -Wall -Wno-strict-aliasing -Wno-strict-overflow  -march=pentium4 -g -O0 -std=gnu++0x -mstackrealign %PreProcessorDefs%    -I. -I..\..\JuceLibraryCode -I%CsoundIncludeDir% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c C:\Users\Rory\Documents\SourceCode\cabbageaudio\cabbage\Source\XYPad.cpp -o build\intermediate\Debug\XYPad.o

g++.exe -Wall -Wno-strict-aliasing -Wno-strict-overflow  -march=pentium4 -g -O0 -std=gnu++0x -mstackrealign %PreProcessorDefs%    -I. -I..\..\JuceLibraryCode -I%CsoundIncludeDir% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c C:\Users\Rory\Documents\SourceCode\cabbageaudio\cabbage\Source\XYPadAutomation.cpp -o build\intermediate\Debug\XYPadAutomation.o

g++.exe  -o build\Cabbage.exe build\intermediate\Debug\juce_audio_basics.o build\intermediate\Debug\juce_audio_devices.o build\intermediate\Debug\juce_audio_formats.o build\intermediate\Debug\juce_audio_processors.o build\intermediate\Debug\juce_audio_utils.o build\intermediate\Debug\juce_core.o build\intermediate\Debug\juce_cryptography.o build\intermediate\Debug\juce_data_structures.o build\intermediate\Debug\juce_events.o build\intermediate\Debug\juce_graphics.o build\intermediate\Debug\juce_gui_basics.o build\intermediate\Debug\juce_gui_extra.o build\intermediate\Debug\BinaryData.o build\intermediate\Debug\CabbageAudioDeviceSelectorComponent.o build\intermediate\Debug\CabbageCallOutBox.o build\intermediate\Debug\CabbageGUIClass.o build\intermediate\Debug\CabbageLookAndFeel.o build\intermediate\Debug\CabbageMainPanel.o build\intermediate\Debug\CabbagePropertiesDialog.o build\intermediate\Debug\CabbageTable.o build\intermediate\Debug\ComponentLayoutEditor.o build\intermediate\Debug\csPerfThread.o build\intermediate\Debug\DirectoryContentsComponent.o build\intermediate\Debug\CodeEditor.o build\intermediate\Debug\CodeWindow.o build\intermediate\Debug\CommandManager.o build\intermediate\Debug\CabbagePluginEditor.o build\intermediate\Debug\CabbagePluginProcessor.o build\intermediate\Debug\Soundfiler.o build\intermediate\Debug\CabbageStandaloneDialog.o build\intermediate\Debug\StandaloneFilterApp.o build\intermediate\Debug\XYPad.o build\intermediate\Debug\XYPadAutomation.o -lgdi32 -luser32 -lkernel32 -lcomctl32 -lcomdlg32 -limm32 -lole32 -loleaut32 -lrpcrt4 -lshlwapi -luuid -lversion -lwininet -lwinmm -lws2_32 -lwsock32 %CsoundLibrary%  -mwindows cabbage.res
