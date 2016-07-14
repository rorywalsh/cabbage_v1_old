ECHO "Build script for Cabbage on Windows. See README.txt for more information"

CALL mingw32-make -f MakefileStandaloneCanonical64.mak CONFIG=Release clean
CALL mingw32-make -f MakefileStandaloneCanonical64.mak CONFIG=Release -j6

CALL mingw32-make -f MakefilePluginEffectCanonical64.mak CONFIG=Release clean
CALL mingw32-make -f MakefilePluginEffectCanonical64.mak CONFIG=Release -j6
CALL mv buildCanon64\CabbagePluginEffect.dll build\CabbagePluginEffect.dat 

CALL mingw32-make -f MakefilePluginSynthCanonical64.mak CONFIG=Release clean
CALL mingw32-make -f MakefilePluginSynthCanonical64.mak CONFIG=Release -j6
CALL mv buildCanon64\CabbagePluginSynth.dll build\CabbagePluginSynth.dat

CALL mingw32-make -f MakefileHostCanonical64.mak CONFIG=Release clean
CALL mingw32-make -f MakefileHostCanonical64.mak CONFIG=Release -j6


CALL "C:\Program Files (x86)\NSIS\makensis" Cabbage64InstallerCanonical.nsi