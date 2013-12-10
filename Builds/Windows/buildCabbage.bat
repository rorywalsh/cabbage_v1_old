ECHO "Build script for Cabbage on Windows. See README.txt for more information"

CALL MakeStandalone.bat release
CALL MakePluginEffect.bat release
CALL MakePluginSynth.bat release