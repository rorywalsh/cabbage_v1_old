ECHO "Build script for Cabbage on Windows. See README.txt for more information"

CALL g++ csound.cpp -o csoundCabbage.exe -I ..\..\..\csound64\include -L ..\..\..\csound64\build -lcsound64

CALL mingw32-make -f MakefileStandalone64.mak CONFIG=Release clean
CALL mingw32-make -f MakefileStandalone64.mak -j8 CONFIG=Release

CALL mingw32-make -f MakefilePluginEffect64.mak CONFIG=Release clean
CALL mingw32-make -f MakefilePluginEffect64.mak -j8 CONFIG=Release
CALL mv build64\CabbagePluginEffect.dll build64\CabbagePluginEffect.dat 

CALL mingw32-make -f MakefilePluginSynth64.mak CONFIG=Release clean
CALL mingw32-make -f MakefilePluginSynth64.mak -j8 CONFIG=Release
CALL mv build64\CabbagePluginSynth.dll build64\CabbagePluginSynth.dat

CALL mingw32-make -f MakefileHost64.mak CONFIG=Release clean
CALL mingw32-make -f MakefileHost64.mak -j8 CONFIG=Release

CALL makensis Cabbage64Installer.nsi