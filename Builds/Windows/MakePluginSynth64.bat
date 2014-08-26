
@ECHO OFF

ECHO =============Setting Build flags===================
ECHO * You will need to edit this bat file in order to *
ECHO * setup the correct directories and provide paths *
ECHO * to the Csound library                           *
ECHO ===================================================
ECHO 
set JUCE_LIBRARY_CODE=C:\Users\RoryWalsh\Documents\SourceCode\cabbageaudio\cabbage\JuceLibraryCode
set PREPROCESSOR_DEFS=-D__MINGW__=1 -DJUCE_MINGW -DWIN32 -D__MINGW_EXTENSION= -DCSOUND6=1 -D Cabbage_Plugin_Synth=1 -DUSE_DOUBLE=1 -DJUCER_CODEBLOCKS_20734A5D=1
set CSOUND_INCLUDE_DIR=C:\Users\RoryWalsh\Documents\SourceCode\cabbageaudio\csound64\csound6\include
set ASIOSDK2Dir=C:\SDKs\ASIOSDK2.3\common
set VSTSDKDir=C:\SDKs\vstsdk2.4
set CABBAGE_SOURCE_DIR=C:\Users\RoryWalsh\Documents\SourceCode\cabbageaudio\cabbage\Source
set CSOUND_LIBRARY=C:\Users\RoryWalsh\Documents\SourceCode\cabbageaudio\csound64\csound6\debug\libcsound64.dll.a
set COMPILER_FLAGS=-Wall -Wno-strict-aliasing -Wno-strict-overflow -march=x86-64 -g -ggdb -O0 -static-libgcc -static-libstdc++ -std=gnu++0x -mstackrealign
set OUTPUT_TARGET=CabbagePluginSynth.dll

IF "%1"=="release" set COMPILER_FLAGS= -Wall -Wreorder -Wno-strict-aliasing -static-libgcc -static-libstdc++ -Wno-strict-overflow  -march=x86-64 -O0 -std=gnu++0x -mstackrealign
IF "%1"=="release" set OUTPUT_TARGET=CabbagePluginSynth.dat

@ECHO on

rm -rf build\intermediate\Debug
mkdir build\intermediate\Debug

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

g++.exe %COMPILER_FLAGS% %PREPROCESSOR_DEFS%    -I. -I..\..\JuceLibraryCode -I%CSOUND_INCLUDE_DIR% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %CABBAGE_SOURCE_DIR%\CabbageGUIClass.cpp -o build\intermediate\Debug\CabbageGUIClass.o

g++.exe %COMPILER_FLAGS% %PREPROCESSOR_DEFS%    -I. -I..\..\JuceLibraryCode -I%CSOUND_INCLUDE_DIR% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %CABBAGE_SOURCE_DIR%\CabbageLookAndFeel.cpp -o build\intermediate\Debug\CabbageLookAndFeel.o

g++.exe %COMPILER_FLAGS% %PREPROCESSOR_DEFS%    -I. -I..\..\JuceLibraryCode -I%CSOUND_INCLUDE_DIR% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %CABBAGE_SOURCE_DIR%\CabbageMainPanel.cpp -o build\intermediate\Debug\CabbageMainPanel.o

g++.exe %COMPILER_FLAGS% %PREPROCESSOR_DEFS%    -I. -I..\..\JuceLibraryCode -I%CSOUND_INCLUDE_DIR% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %CABBAGE_SOURCE_DIR%\CabbageTable.cpp -o build\intermediate\Debug\CabbageTable.o

g++.exe %COMPILER_FLAGS% %PREPROCESSOR_DEFS%    -I. -I..\..\JuceLibraryCode -I%CSOUND_INCLUDE_DIR% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %CABBAGE_SOURCE_DIR%\Table.cpp -o build\intermediate\Debug\Table.o

g++.exe %COMPILER_FLAGS% %PREPROCESSOR_DEFS%    -I. -I..\..\JuceLibraryCode -I%CSOUND_INCLUDE_DIR% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %CABBAGE_SOURCE_DIR%\csPerfThread.cpp -o build\intermediate\Debug\csPerfThread.o

g++.exe %COMPILER_FLAGS% %PREPROCESSOR_DEFS%    -I. -I..\..\JuceLibraryCode -I%CSOUND_INCLUDE_DIR% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %CABBAGE_SOURCE_DIR%\DirectoryContentsComponent.cpp -o build\intermediate\Debug\DirectoryContentsComponent.o

g++.exe %COMPILER_FLAGS% %PREPROCESSOR_DEFS%    -I. -I..\..\JuceLibraryCode -I%CSOUND_INCLUDE_DIR% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %CABBAGE_SOURCE_DIR%\Plugin\CabbagePluginProcessor.cpp -o build\intermediate\Debug\CabbagePluginProcessor.o

g++.exe %COMPILER_FLAGS% %PREPROCESSOR_DEFS%    -I. -I..\..\JuceLibraryCode -I%CSOUND_INCLUDE_DIR% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %CABBAGE_SOURCE_DIR%\Plugin\CabbagePluginEditor.cpp -o build\intermediate\Debug\CabbagePluginEditor.o

g++.exe %COMPILER_FLAGS% %PREPROCESSOR_DEFS%    -I. -I..\..\JuceLibraryCode -I%CSOUND_INCLUDE_DIR% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %CABBAGE_SOURCE_DIR%\Soundfiler.cpp -o build\intermediate\Debug\Soundfiler.o

g++.exe %COMPILER_FLAGS% %PREPROCESSOR_DEFS%    -I. -I..\..\JuceLibraryCode -I%CSOUND_INCLUDE_DIR% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %CABBAGE_SOURCE_DIR%\XYPad.cpp -o build\intermediate\Debug\XYPad.o

g++.exe %COMPILER_FLAGS% %PREPROCESSOR_DEFS%    -I. -I..\..\JuceLibraryCode -I%CSOUND_INCLUDE_DIR% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %CABBAGE_SOURCE_DIR%\XYPadAutomation.cpp -o build\intermediate\Debug\XYPadAutomation.o

g++.exe %COMPILER_FLAGS% %PREPROCESSOR_DEFS%    -I. -I..\..\JuceLibraryCode -I%CSOUND_INCLUDE_DIR% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %JUCE_LIBRARY_CODE%\modules\juce_audio_plugin_client\utility\juce_PluginUtilities.cpp -o build\intermediate\Debug\juce_PluginUtilities.o

g++.exe %COMPILER_FLAGS% %PREPROCESSOR_DEFS%    -I. -I..\..\JuceLibraryCode -I%CSOUND_INCLUDE_DIR% -I%ASIOSDK2Dir% -I%VSTSDKDir%  -c %JUCE_LIBRARY_CODE%\modules\juce_audio_plugin_client\VST\juce_VST_Wrapper.cpp -o build\intermediate\Debug\juce_VST_Wrapper.o

g++.exe -shared -Wl,--dll -o build\%OUTPUT_TARGET% build\intermediate\Debug\juce_audio_basics.o build\intermediate\Debug\juce_audio_devices.o build\intermediate\Debug\juce_audio_formats.o build\intermediate\Debug\juce_audio_processors.o build\intermediate\Debug\juce_audio_utils.o build\intermediate\Debug\juce_core.o build\intermediate\Debug\juce_cryptography.o build\intermediate\Debug\juce_data_structures.o build\intermediate\Debug\juce_events.o build\intermediate\Debug\juce_graphics.o build\intermediate\Debug\juce_gui_basics.o build\intermediate\Debug\Table.o build\intermediate\Debug\juce_gui_extra.o build\intermediate\Debug\juce_PluginUtilities.o build\intermediate\Debug\juce_VST_Wrapper.o build\intermediate\Debug\BinaryData.o build\intermediate\Debug\CabbagePluginEditor.o build\intermediate\Debug\CabbageGUIClass.o build\intermediate\Debug\CabbageLookAndFeel.o build\intermediate\Debug\CabbageMainPanel.o build\intermediate\Debug\CabbageTable.o build\intermediate\Debug\csPerfThread.o build\intermediate\Debug\DirectoryContentsComponent.o  build\intermediate\Debug\CabbagePluginProcessor.o build\intermediate\Debug\Soundfiler.o build\intermediate\Debug\XYPad.o build\intermediate\Debug\XYPadAutomation.o -lgdi32 -luser32 -lkernel32 -lcomctl32 -lcomdlg32 -limm32 -lole32 -loleaut32 -lrpcrt4 -lshlwapi -luuid -lversion -lwininet -lwinmm -lws2_32 -lwsock32 %CSOUND_LIBRARY%  -mwindows
