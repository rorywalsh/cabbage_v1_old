if [ $1 == "standalone" ]
then
	echo "Building Standalone Only"	
	rm -rf ./build/Debug/Cabbage.app
	xcodebuild -project Cabbage.xcodeproj/
fi

if [ $1 == "bundle" ]
then
	echo "Bundling all files"	
	cp -rf ../../Docs ./build/Debug/Cabbage.app/Contents/MacOS/Docs
	cp -rf ../../Examples ./build/Debug/Cabbage.app/Contents/MacOS/Examples
	cp opcodes.txt ./build/Debug/Cabbage.app/Contents/MacOS/opcodes.txt 
fi

if [ $1 == "plugins" ]
then
	echo "Building Plugins Only"	
	xcodebuild -project CabbagePlugin.xcodeproj/ GCC_PREPROCESSOR_DEFINITIONS="Cabbage_Plugin_Synth=1 USE_DOUBLE=1 MACOSX=1 CSOUND6=1 Debug=1"
	rm -rf ./build/Debug/Cabbage.app/Contents/CabbagePluginSynth.component
	cp -rf ./build/Debug/CabbagePlugin.component/ ./build/Debug/Cabbage.app/Contents/CabbagePluginSynth.component
	rm -rf ./build/Debug/CabbagePluginSynth.dat/CabbagePlugin.component

	xcodebuild -project CabbagePlugin.xcodeproj/ GCC_PREPROCESSOR_DEFINITIONS="MACOSX=1 Debug=1 USE_DOUBLE=1"
	rm -rf ./build/Debug/Cabbage.app/Contents/CabbagePluginEffect.component
	cp -rf ./build/Debug/CabbagePlugin.component/ ./build/Debug/Cabbage.app/Contents/CabbagePluginEffect.component
	rm -rf ./build/Debug/CabbagePluginEffect.component
	rm -rf ~/Library/Audio/Plug-Ins/VST/CabbagePlugin.vst
fi

if [ $1 == "csound plugins" ]
then
	echo "Building Csound Plugins Only"	
	xcodebuild -project CabbagePluginCsound.xcodeproj/ GCC_PREPROCESSOR_DEFINITIONS="Cabbage_Plugin_Synth=1 USE_DOUBLE=1 MACOSX=1 CSOUND6=1 Debug=1"
	rm -rf ./build/Debug/Cabbage.app/Contents/CabbageCsoundPluginSynth.component
	cp -rf ./build/Debug/CabbagePlugin.component/ ./build/Debug/Cabbage.app/Contents/CabbageCsoundPluginSynth.component


	xcodebuild -project CabbagePluginCsound.xcodeproj/ GCC_PREPROCESSOR_DEFINITIONS="MACOSX=1 Debug=1 USE_DOUBLE=1"
	rm -rf ./build/Debug/Cabbage.app/Contents/CabbageCsoundPluginEffect.component
	cp -rf ./build/Debug/CabbagePlugin.component/ ./build/Debug/Cabbage.app/Contents/CabbageCsoundPluginEffect.component
	rm -rf ./build/Debug/CabbagePluginEffect.component
	rm -rf ~/Library/Audio/Plug-Ins/VST/CabbagePlugin.vst
fi

if [ $1 == "all" ]
then
	echo "Building All"
	xcodebuild -project Cabbage.xcodeproj/

	xcodebuild -project CabbagePlugin.xcodeproj/ GCC_PREPROCESSOR_DEFINITIONS="Cabbage_Plugin_Synth=1 USE_DOUBLE=1 CSOUND6=1 MACOSX=1 Debug=1"
	cp -rf ./build/Debug/CabbagePlugin.component/ ./build/Debug/Cabbage.app/Contents/CabbagePluginSynth.component
	rm -rf ./build/Debug/CabbagePluginSynth.dat/CabbagePlugin.component
	
	xcodebuild -project CabbagePlugin.xcodeproj/ GCC_PREPROCESSOR_DEFINITIONS="MACOSX=1 Debug=1 USE_DOUBLE=1"
	cp -rf ./build/Debug/CabbagePlugin.component/ ./build/Debug/Cabbage.app/Contents/CabbagePluginEffect.component
	rm -rf ./build/Debug/CabbagePluginEffect.component
	rm -rf ~/Library/Audio/Plug-Ins/VST/CabbagePlugin.vst

	echo "Bundling all files"	
	cp -rf ../../Docs ./build/Debug/Cabbage.app/Contents/MacOS/Docs
	cp -rf ../../Examples ./build/Debug/Cabbage.app/Contents/MacOS/Examples
	cp opcodes.txt ./build/Debug/Cabbage.app/Contents/MacOS/opcodes.txt 	
fi

