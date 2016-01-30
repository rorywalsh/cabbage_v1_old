ECHO "Build script for Cabbage on Windows. See README.txt for more information"

CALL g++ csound.cpp -o csoundCabbage.exe -I ..\..\..\csound\include -L ..\..\..\csound\build -lcsound64

CALL mingw32-make -f MakefileStandalone.mak CONFIG=Release clean
CALL mingw32-make -f MakefileStandalone.mak CONFIG=Release -j6

CALL mingw32-make -f MakefilePluginEffect.mak CONFIG=Release clean
CALL mingw32-make -f MakefilePluginEffect.mak CONFIG=Release -j6
CALL mv build\CabbagePluginEffect.dll build\CabbagePluginEffect.dat 

CALL mingw32-make -f MakefilePluginSynth.mak CONFIG=Release clean
CALL mingw32-make -f MakefilePluginSynth.mak CONFIG=Release -j6
CALL mv build\CabbagePluginSynth.dll build\CabbagePluginSynth.dat

CALL mingw32-make -f MakefileHost.mak CONFIG=Release clean
CALL mingw32-make -f MakefileHost.mak CONFIG=Release -j6


CALL makensis Cabbage32Installer.nsi