ECHO "Build script for Cabbage on Windows. See README.txt for more information"

CALL mingw32-make -f MakefileStandalone64.mak CONFIG=Release clean
CALL mingw32-make -f MakefileStandalone64.mak CONFIG=Release
CALL mingw32-make -f MakefilePluginEffect64.mak CONFIG=Release clean
CALL mingw32-make -f MakefilePluginEffect64.mak CONFIG=Release
CALL mingw32-make -f MakefilePluginSynth64.mak CONFIG=Release clean
CALL mingw32-make -f MakefilePluginSynth64.mak CONFIG=Release
CALL mv build\CabbagePluginEffect.dll build\CabbagePluginEffect.dat 
CALL mv build\CabbagePluginSynth.dll build\CabbagePluginSynth.dat