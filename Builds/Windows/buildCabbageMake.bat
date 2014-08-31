ECHO "Build script for Cabbage on Windows. See README.txt for more information"

CALL mingw32-make -f MakefileStandalone.mak CONFIG=Release clean
CALL mingw32-make -f MakefileStandalone.mak CONFIG=Release
CALL mingw32-make -f MakefilePluginEffect.mak CONFIG=Release clean
CALL mingw32-make -f MakefilePluginEffect.mak CONFIG=Release
CALL mingw32-make -f MakefilePluginSynth.mak CONFIG=Release clean
CALL mingw32-make -f MakefilePluginSynth.mak CONFIG=Release
CALL mv build\CabbagePluginEffect.dll build\CabbagePluginEffect.dat 
CALL mv build\CabbagePluginSynth.dll build\CabbagePluginSynth.dat