ECHO "Build script for Cabbage on Windows. See README.txt for more information"

CALL MakeStandalone64.bat release
CALL MakePluginEffect64.bat release
CALL MakePluginSynth64.bat release