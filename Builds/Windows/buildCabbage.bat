ECHO "Build script for Cabbage on Windows. See README.txt for more information"

make -f MakefileStandalone.mak CONFIG=Release clean_obj
make -f MakefileStandalone.mak CONFIG=Release

make -f MakefilePluginSynth.mak CONFIG=Release TARGET=CabbagePluginSynth.dat clean
make -f MakefilePluginSynth.mak CONFIG=Release TARGET=CabbagePluginSynth.dat

make -f MakefilePluginEffect.mak CONFIG=Release TARGET=CabbagePluginEffect.dat clean
make -f MakefilePluginEffect.mak CONFIG=Release TARGET=CabbagePluginEffect.dat