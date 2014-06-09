
@ECHO OFF
ECHO =============Setting Build flags===================
ECHO * You will need to edit this bat file in order to *
ECHO * setup the correct directories and provide paths *
ECHO * to the Csound library                           *
ECHO ===================================================
ECHO 
set JUCE_LIBRARY_CODE=C:\Users\RoryWalsh\Documents\SourceCode\cabbageaudio\cabbage\JuceLibraryCode
set PREPROCESSOR_DEFS=-D__MINGW__=1 -DWIN32 -D__MINGW_EXTENSION=1 -DBUILD_DEBUGGER=1 -DCabbage_Build_Standalone=1 -DUSE_DOUBLE=1 -DCSOUND6=1 -DCabbage_GUI_Editor=1 -DJUCER_CODEBLOCKS_20734A5D=1
set CSOUND_INCLUDE_DIR=C:\Users\RoryWalsh\Documents\SourceCode\cabbageaudio\csound\include
set ASIOSDK2Dir=C:\SDKs\ASIOSDK2.3\common
set VSTSDKDir=C:\SDKs\vstsdk2.4
set CABBAGE_SOURCE_DIR=C:\Users\RoryWalsh\Documents\SourceCode\cabbageaudio\cabbage\Source
set CSOUND_LIBRARY=C:\Users\RoryWalsh\Documents\SourceCode\cabbageaudio\csound\build\libcsound64.dll.a
set COMPILER_FLAGS= -Wall -Wreorder -Wno-strict-aliasing -ggdb -Wno-strict-overflow -static-libgcc -static-libstdc++  -march=pentium4 -g -O0 -std=gnu++0x -mstackrealign
set OUTPUT_TARGET=Cabbage.exe

IF "%1"=="release" set COMPILER_FLAGS= -Wall -Wreorder -O0 -Wno-strict-aliasing -Wno-strict-overflow  -march=pentium4 -static-libgcc -static-libstdc++ -std=gnu++0x -mstackrealign

ECHO on

REM rm -rf build\intermediate\Debug
REM mkdir build\intermediate\Debug

g++.exe %COMPILER_FLAGS% %PREPROCESSOR_DEFS% -I. -I..\..\JuceLibraryCode -I%CSOUND_INCLUDE_DIR% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %JUCE_LIBRARY_CODE%\modules\juce_audio_basics\juce_audio_basics.cpp -o build\intermediate\Debug\juce_audio_basics.o

g++.exe %COMPILER_FLAGS% %PREPROCESSOR_DEFS%    -I. -I..\..\JuceLibraryCode -I%CSOUND_INCLUDE_DIR% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %JUCE_LIBRARY_CODE%\modules\juce_audio_devices\juce_audio_devices.cpp -o build\intermediate\Debug\juce_audio_devices.o

g++.exe %COMPILER_FLAGS% %PREPROCESSOR_DEFS%    -I. -I..\..\JuceLibraryCode -I%CSOUND_INCLUDE_DIR% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %JUCE_LIBRARY_CODE%\modules\juce_audio_formats\juce_audio_formats.cpp -o build\intermediate\Debug\juce_audio_formats.o

g++.exe %COMPILER_FLAGS% %PREPROCESSOR_DEFS%    -I. -I..\..\JuceLibraryCode -I%CSOUND_INCLUDE_DIR% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %JUCE_LIBRARY_CODE%\modules\juce_audio_processors\juce_audio_processors.cpp -o build\intermediate\Debug\juce_audio_processors.o

g++.exe %COMPILER_FLAGS% %PREPROCESSOR_DEFS%    -I. -I..\..\JuceLibraryCode -I%CSOUND_INCLUDE_DIR% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %JUCE_LIBRARY_CODE%\modules\juce_audio_utils\juce_audio_utils.cpp -o build\intermediate\Debug\juce_audio_utils.o

g++.exe %COMPILER_FLAGS% %PREPROCESSOR_DEFS%    -I. -I..\..\JuceLibraryCode -I%CSOUND_INCLUDE_DIR% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %JUCE_LIBRARY_CODE%\modules\juce_core\juce_core.cpp -o build\intermediate\Debug\juce_core.o

g++.exe %COMPILER_FLAGS% %PREPROCESSOR_DEFS%    -I. -I..\..\JuceLibraryCode -I%CSOUND_INCLUDE_DIR% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %JUCE_LIBRARY_CODE%\modules\juce_cryptography\juce_cryptography.cpp -o build\intermediate\Debug\juce_cryptography.o

g++.exe %COMPILER_FLAGS% %PREPROCESSOR_DEFS%    -I. -I..\..\JuceLibraryCode -I%CSOUND_INCLUDE_DIR% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %JUCE_LIBRARY_CODE%\modules\juce_data_structures\juce_data_structures.cpp -o build\intermediate\Debug\juce_data_structures.o

g++.exe %COMPILER_FLAGS% %PREPROCESSOR_DEFS%    -I. -I..\..\JuceLibraryCode -I%CSOUND_INCLUDE_DIR% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %JUCE_LIBRARY_CODE%\modules\juce_events\juce_events.cpp -o build\intermediate\Debug\juce_events.o

g++.exe %COMPILER_FLAGS% %PREPROCESSOR_DEFS%    -I. -I..\..\JuceLibraryCode -I%CSOUND_INCLUDE_DIR% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %JUCE_LIBRARY_CODE%\modules\juce_graphics\juce_graphics.cpp -o build\intermediate\Debug\juce_graphics.o

g++.exe %COMPILER_FLAGS% %PREPROCESSOR_DEFS%    -I. -I..\..\JuceLibraryCode -I%CSOUND_INCLUDE_DIR% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %JUCE_LIBRARY_CODE%\modules\juce_gui_basics\juce_gui_basics.cpp -o build\intermediate\Debug\juce_gui_basics.o

g++.exe %COMPILER_FLAGS% %PREPROCESSOR_DEFS%    -I. -I..\..\JuceLibraryCode -I%CSOUND_INCLUDE_DIR% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %JUCE_LIBRARY_CODE%\modules\juce_gui_extra\juce_gui_extra.cpp -o build\intermediate\Debug\juce_gui_extra.o

g++.exe %COMPILER_FLAGS% %PREPROCESSOR_DEFS%    -I. -I..\..\JuceLibraryCode -I%CSOUND_INCLUDE_DIR% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %CABBAGE_SOURCE_DIR%\BinaryData.cpp -o build\intermediate\Debug\BinaryData.o

g++.exe %COMPILER_FLAGS% %PREPROCESSOR_DEFS%    -I. -I..\..\JuceLibraryCode -I%CSOUND_INCLUDE_DIR% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %CABBAGE_SOURCE_DIR%\CabbageAudioDeviceSelectorComponent.cpp -o build\intermediate\Debug\CabbageAudioDeviceSelectorComponent.o

g++.exe %COMPILER_FLAGS% %PREPROCESSOR_DEFS%    -I. -I..\..\JuceLibraryCode -I%CSOUND_INCLUDE_DIR% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %CABBAGE_SOURCE_DIR%\CabbageCallOutBox.cpp -o build\intermediate\Debug\CabbageCallOutBox.o

g++.exe %COMPILER_FLAGS% %PREPROCESSOR_DEFS%    -I. -I..\..\JuceLibraryCode -I%CSOUND_INCLUDE_DIR% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %CABBAGE_SOURCE_DIR%\CabbageGUIClass.cpp -o build\intermediate\Debug\CabbageGUIClass.o

g++.exe %COMPILER_FLAGS% %PREPROCESSOR_DEFS%    -I. -I..\..\JuceLibraryCode -I%CSOUND_INCLUDE_DIR% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %CABBAGE_SOURCE_DIR%\CabbageLookAndFeel.cpp -o build\intermediate\Debug\CabbageLookAndFeel.o
               ^
g++.exe %COMPILER_FLAGS% %PREPROCESSOR_DEFS%    -I. -I..\..\JuceLibraryCode -I%CSOUND_INCLUDE_DIR% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %CABBAGE_SOURCE_DIR%\CabbageMainPanel.cpp -o build\intermediate\Debug\CabbageMainPanel.o

g++.exe %COMPILER_FLAGS% %PREPROCESSOR_DEFS%    -I. -I..\..\JuceLibraryCode -I%CSOUND_INCLUDE_DIR% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %CABBAGE_SOURCE_DIR%\CabbagePropertiesDialog.cpp -o build\intermediate\Debug\CabbagePropertiesDialog.o

g++.exe %COMPILER_FLAGS% %PREPROCESSOR_DEFS%    -I. -I..\..\JuceLibraryCode -I%CSOUND_INCLUDE_DIR% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %CABBAGE_SOURCE_DIR%\CabbageTable.cpp -o build\intermediate\Debug\CabbageTable.o

g++.exe %COMPILER_FLAGS% %PREPROCESSOR_DEFS%    -I. -I..\..\JuceLibraryCode -I%CSOUND_INCLUDE_DIR% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %CABBAGE_SOURCE_DIR%\Table.cpp -o build\intermediate\Debug\Table.o

g++.exe %COMPILER_FLAGS% %PREPROCESSOR_DEFS%    -I. -I..\..\JuceLibraryCode -I%CSOUND_INCLUDE_DIR% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %CABBAGE_SOURCE_DIR%\ComponentLayoutEditor.cpp -o build\intermediate\Debug\ComponentLayoutEditor.o

g++.exe %COMPILER_FLAGS% %PREPROCESSOR_DEFS%    -I. -I..\..\JuceLibraryCode -I%CSOUND_INCLUDE_DIR% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %CABBAGE_SOURCE_DIR%\csPerfThread.cpp -o build\intermediate\Debug\csPerfThread.o

g++.exe %COMPILER_FLAGS% %PREPROCESSOR_DEFS%    -I. -I..\..\JuceLibraryCode -I%CSOUND_INCLUDE_DIR% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %CABBAGE_SOURCE_DIR%\DirectoryContentsComponent.cpp -o build\intermediate\Debug\DirectoryContentsComponent.o

g++.exe %COMPILER_FLAGS% %PREPROCESSOR_DEFS%    -I. -I..\..\JuceLibraryCode -I%CSOUND_INCLUDE_DIR% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %CABBAGE_SOURCE_DIR%\Editor\CodeEditor.cpp -o build\intermediate\Debug\CodeEditor.o

g++.exe %COMPILER_FLAGS% %PREPROCESSOR_DEFS%    -I. -I..\..\JuceLibraryCode -I%CSOUND_INCLUDE_DIR% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %CABBAGE_SOURCE_DIR%\Editor\CodeWindow.cpp -o build\intermediate\Debug\CodeWindow.o

g++.exe %COMPILER_FLAGS% %PREPROCESSOR_DEFS%    -I. -I..\..\JuceLibraryCode -I%CSOUND_INCLUDE_DIR% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %CABBAGE_SOURCE_DIR%\Editor\SplitComponent.cpp -o build\intermediate\Debug\SplitComponent.o

g++.exe %COMPILER_FLAGS% %PREPROCESSOR_DEFS%    -I. -I..\..\JuceLibraryCode -I%CSOUND_INCLUDE_DIR% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %CABBAGE_SOURCE_DIR%\Editor\CommandManager.cpp -o build\intermediate\Debug\CommandManager.o

g++.exe %COMPILER_FLAGS% %PREPROCESSOR_DEFS%    -I. -I..\..\JuceLibraryCode -I%CSOUND_INCLUDE_DIR% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %CABBAGE_SOURCE_DIR%\Plugin\CabbagePluginEditor.cpp -o build\intermediate\Debug\CabbagePluginEditor.o

g++.exe %COMPILER_FLAGS% %PREPROCESSOR_DEFS%    -I. -I..\..\JuceLibraryCode -I%CSOUND_INCLUDE_DIR% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %CABBAGE_SOURCE_DIR%\Plugin\CabbagePluginProcessor.cpp -o build\intermediate\Debug\CabbagePluginProcessor.o

g++.exe %COMPILER_FLAGS% %PREPROCESSOR_DEFS%    -I. -I..\..\JuceLibraryCode -I%CSOUND_INCLUDE_DIR% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %CABBAGE_SOURCE_DIR%\Soundfiler.cpp -o build\intermediate\Debug\Soundfiler.o

g++.exe %COMPILER_FLAGS% %PREPROCESSOR_DEFS%    -I. -I..\..\JuceLibraryCode -I%CSOUND_INCLUDE_DIR% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %CABBAGE_SOURCE_DIR%\Standalone\StandaloneFilterApp.cpp -o build\intermediate\Debug\StandaloneFilterApp.o

g++.exe %COMPILER_FLAGS% %PREPROCESSOR_DEFS%    -I. -I..\..\JuceLibraryCode -I%CSOUND_INCLUDE_DIR% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %CABBAGE_SOURCE_DIR%\XYPad.cpp -o build\intermediate\Debug\XYPad.o

g++.exe %COMPILER_FLAGS% %PREPROCESSOR_DEFS%    -I. -I..\..\JuceLibraryCode -I%CSOUND_INCLUDE_DIR% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %CABBAGE_SOURCE_DIR%\XYPadAutomation.cpp -o build\intermediate\Debug\XYPadAutomation.o

g++.exe %COMPILER_FLAGS% %PREPROCESSOR_DEFS%    -I. -I..\..\JuceLibraryCode -I%CSOUND_INCLUDE_DIR% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %CABBAGE_SOURCE_DIR%\Standalone\CabbageStandaloneDialog.cpp -o build\intermediate\Debug\CabbageStandaloneDialog.o

g++.exe  -o build\%OUTPUT_TARGET% build\intermediate\Debug\juce_audio_basics.o build\intermediate\Debug\juce_audio_devices.o build\intermediate\Debug\juce_audio_formats.o build\intermediate\Debug\juce_audio_processors.o build\intermediate\Debug\juce_audio_utils.o build\intermediate\Debug\juce_core.o build\intermediate\Debug\juce_cryptography.o build\intermediate\Debug\juce_data_structures.o build\intermediate\Debug\juce_events.o build\intermediate\Debug\juce_graphics.o build\intermediate\Debug\juce_gui_basics.o build\intermediate\Debug\juce_gui_extra.o build\intermediate\Debug\BinaryData.o build\intermediate\Debug\CabbageAudioDeviceSelectorComponent.o build\intermediate\Debug\Table.o build\intermediate\Debug\CabbageCallOutBox.o build\intermediate\Debug\CabbageGUIClass.o build\intermediate\Debug\CabbageLookAndFeel.o build\intermediate\Debug\CabbageMainPanel.o build\intermediate\Debug\CabbagePropertiesDialog.o build\intermediate\Debug\CabbageTable.o build\intermediate\Debug\ComponentLayoutEditor.o build\intermediate\Debug\csPerfThread.o build\intermediate\Debug\DirectoryContentsComponent.o build\intermediate\Debug\CodeEditor.o build\intermediate\Debug\CodeWindow.o build\intermediate\Debug\CommandManager.o build\intermediate\Debug\CabbagePluginEditor.o build\intermediate\Debug\SplitComponent.o build\intermediate\Debug\CabbagePluginProcessor.o build\intermediate\Debug\Soundfiler.o build\intermediate\Debug\CabbageStandaloneDialog.o build\intermediate\Debug\StandaloneFilterApp.o build\intermediate\Debug\XYPad.o build\intermediate\Debug\XYPadAutomation.o -lgdi32 -luser32 -lkernel32 -lcomctl32 -lcomdlg32 -limm32 -lole32 -loleaut32 -lrpcrt4 -lshlwapi -luuid -lversion -lwininet -lwinmm -lws2_32 -lwsock32 %CSOUND_LIBRARY%  -mwindows cabbage.res
