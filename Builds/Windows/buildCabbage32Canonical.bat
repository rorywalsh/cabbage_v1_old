ECHO "Build script for Cabbage on Windows. See README.txt for more information"

CALL g++ csound.cpp -o csoundCabbage.exe -I ..\..\..\csound\include -L ..\..\..\csound\build -lcsound64

CALL mingw32-make -f MakefileStandaloneCanonical.mak CONFIG=Release clean
CALL mingw32-make -f MakefileStandaloneCanonical.mak CONFIG=Release -j6

CALL mingw32-make -f MakefilePluginEffectCanonical.mak CONFIG=Release clean
CALL mingw32-make -f MakefilePluginEffectCanonical.mak CONFIG=Release -j6
CALL mv buildCanon\CabbagePluginEffect.dll build\CabbagePluginEffect.dat 

CALL mingw32-make -f MakefilePluginSynthCanonical.mak CONFIG=Release clean
CALL mingw32-make -f MakefilePluginSynthCanonical.mak CONFIG=Release -j6
CALL mv buildCanon\CabbagePluginSynth.dll build\CabbagePluginSynth.dat

CALL mingw32-make -f MakefileHostCanonical.mak CONFIG=Release clean
CALL mingw32-make -f MakefileHostCanonical.mak CONFIG=Release -j6


CALL makensis Cabbage32InstallerCanonical.nsi