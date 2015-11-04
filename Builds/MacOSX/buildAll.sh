# The following command is needed to set up Csound so that it can be inside the bundle frameworks folder 
# install_name_tool -id @executable_path/../Frameworks/CsoundLib64.framework/Versions/6.0/CsoundLib64 \
# 	/Users/walshr/sourcecode/csound/build/CsoundLib64.framework/Versions/6.0/CsoundLib64

# install_name_tool -id @loader_path/../../libs/libsndfile.1.dylib \
#	/Users/walshr/sourcecode/csound/build/CsoundLib64.framework/libs/libsndfile.1.dylib

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
	echo "Removing old binaries"
	rm -rf ./build/Release/Cabbage.app
	rm -rf ./build/Release/CabbagePlugin.component
	rm -rf ./build/Release/CabbageStudio.app


	echo "Building Universal build"
	# The following command is needed to set up Csound so that it can be inside the bundle frameworks folder 
	# install_name_tool -id @executable_path/../Frameworks/CsoundLib64.framework/Versions/6.0/CsoundLib64 \
	# /Users/walshr/sourcecode/csound/build/CsoundLib64.framework/Versions/6.0/CsoundLib64

	xcodebuild -project Cabbage.xcodeproj/ ONLY_ACTIVE_ARCH=NO -configuration Release

	# The following command is needed to set up Csound so that it can be inside the bundle frameworks folder 
	# install_name_tool -id @loader_path/../Frameworks/CsoundLib64.framework/Versions/6.0/CsoundLib64 \
	# /Users/walshr/sourcecode/csound/build/CsoundLib64.framework/Versions/6.0/CsoundLib64
	
	xcodebuild -project CabbagePlugin.xcodeproj/ ONLY_ACTIVE_ARCH=NO -configuration Release GCC_PREPROCESSOR_DEFINITIONS="Cabbage_Plugin_Synth=1 USE_DOUBLE=1 CSOUND6=1 MACOSX=1"
	cp -rf ./build/Release/CabbagePlugin.component/ ./build/Release/Cabbage.app/Contents/CabbagePluginSynth.component
	rm -rf ./build/Release/CabbagePluginSynth.dat/CabbagePlugin.component	
	xcodebuild -project CabbagePlugin.xcodeproj/ -configuration Release ONLY_ACTIVE_ARCH=NO GCC_PREPROCESSOR_DEFINITIONS="MACOSX=1 USE_DOUBLE=1"
	cp -rf ./build/Release/CabbagePlugin.component/ ./build/Release/Cabbage.app/Contents/CabbagePluginEffect.component
	rm -rf ./build/Release/CabbagePluginEffect.component
	rm -rf ~/Library/Audio/Plug-Ins/VST/CabbagePlugin.vst

	# The following command is needed to set up Csound so that it can be inside the bundle frameworks folder 
	# install_name_tool -id @executable_path/../Frameworks/CsoundLib64.framework/Versions/6.0/CsoundLib64 \
	# /Users/walshr/sourcecode/csound/build/CsoundLib64.framework/Versions/6.0/CsoundLib64
	xcodebuild -project CabbageStudio.xcodeproj/ ONLY_ACTIVE_ARCH=NO -configuration Release

	echo "Bundling all files"	
	#mkdir ./build/Release/Cabbage.app/Contents/MacOS/Docs
	# mkdir ./build/Release/Cabbage.app/Contents/MacOS/Docs

	#mkdir ./build/Release/CabbageStudio.app/Contents/MacOS/Docs
	#mkdir ./build/Release/CabbageStudio.app/Contents/MacOS/Docs/_book

	cp -rf ../../Docs/_book ./build/Release/Cabbage.app/Contents/MacOS/Docs
	cp -rf ../../Docs/_book ./build/Release/CabbageStudio.app/Contents/MacOS/Docs

	cp -rf ../../Examples ./build/Release/Cabbage.app/Contents/MacOS/Examples
	cp -rf ../../Examples ./build/Release/CabbageStudio.app/Contents/MacOS/Examples
	
	cp opcodes.txt ./build/Release/Cabbage.app/Contents/MacOS/opcodes.txt 
	cp IntroScreen.csd ./build/Release/Cabbage.app/Contents/MacOS/IntroScreen.csd
	cp cabbage.png  ./build/Release/Cabbage.app/Contents/MacOS/cabbage.png 
	cp cabbageEarphones.png  ./build/Release/Cabbage.app/Contents/MacOS/cabbageEarphones.png 

	#packagesbuild ../../../CabbageOSXUniversal/CabbageOSXUniversal.pkgproj

	#cp -rf ./build/Release/Cabbage.app/ ./build/Release/Cabbage32.app/	
	#rm -rf 	cp -rf ./build/Release/Cabbage.app/

	# echo "Building All 64Bit"
	# xcodebuild -project Cabbage.xcodeproj/ ARCHS=x86_64 ONLY_ACTIVE_ARCH=NO -configuration Release
	# xcodebuild -project CabbagePlugin.xcodeproj/ ARCHS=-x86_64 ONLY_ACTIVE_ARCH=NO -configuration Release GCC_PREPROCESSOR_DEFINITIONS="Cabbage_Plugin_Synth=1 USE_DOUBLE=1 CSOUND6=1 MACOSX=1"
	# cp -rf ./build/Release/CabbagePlugin.component/ ./build/Release/Cabbage.app/Contents/CabbagePluginSynth.component
	# rm -rf ./build/Release/CabbagePluginSynth.dat/CabbagePlugin.component	
	# xcodebuild -project CabbagePlugin.xcodeproj/ -configuration Release ARCHS=x86_64 ONLY_ACTIVE_ARCH=NO GCC_PREPROCESSOR_DEFINITIONS="MACOSX=1 USE_DOUBLE=1"
	# cp -rf ./build/Release/CabbagePlugin.component/ ./build/Release/Cabbage.app/Contents/CabbagePluginEffect.component
	# rm -rf ./build/Release/CabbagePluginEffect.component
	# rm -rf ~/Library/Audio/Plug-Ins/VST/CabbagePlugin.vst

	# echo "Bundling all files"	
	# cp -rf ../../Docs ./build/Release/Cabbage.app/Contents/MacOS/Docs
	# cp -rf ../../Examples ./build/Release/Cabbage.app/Contents/MacOS/Examples
	# cp opcodes.txt ./build/Release/Cabbage.app/Contents/MacOS/opcodes.txt 

	# cp -rf ./build/Release/Cabbage.app/ ./build/Release/Cabbage64.app
	# rm -rf 	cp -rf ./build/Release/Cabbage.app/
fi

if [ $1 == "debug" ]
then
	echo "Building Debug version"
	xcodebuild -project Cabbage.xcodeproj/ ONLY_ACTIVE_ARCH=NO -configuration Debug
	xcodebuild -project CabbagePlugin.xcodeproj/ ONLY_ACTIVE_ARCH=NO -configuration Debug GCC_PREPROCESSOR_DEFINITIONS="Cabbage_Plugin_Synth=1 USE_DOUBLE=1 CSOUND6=1 MACOSX=1"
	cp -rf ./build/Debug/CabbagePlugin.component/ ./build/Debug/Cabbage.app/Contents/CabbagePluginSynth.component
	rm -rf ./build/Debug/CabbagePluginSynth.dat/CabbagePlugin.component	
	xcodebuild -project CabbagePlugin.xcodeproj/ -configuration Debug ONLY_ACTIVE_ARCH=NO GCC_PREPROCESSOR_DEFINITIONS="MACOSX=1 USE_DOUBLE=1"
	cp -rf ./build/Debug/CabbagePlugin.component/ ./build/Debug/Cabbage.app/Contents/CabbagePluginEffect.component
	rm -rf ./build/Debug/CabbagePluginEffect.component
	rm -rf ~/Library/Audio/Plug-Ins/VST/CabbagePlugin.vst

	echo "Bundling all files"	
	cp -rf ../../Docs ./build/Debug/Cabbage.app/Contents/MacOS/Docs
	cp -rf ../../Examples ./build/Debug/Cabbage.app/Contents/MacOS/Examples
	cp opcodes.txt ./build/Debug/Cabbage.app/Contents/MacOS/opcodes.txt 

	cp -rf ./build/Debug/Cabbage.app/ ./build/Debug/Cabbage32.app	
	rm -rf 	cp -rf ./build/Debug/Cabbage.app/

fi

if [ $1 == "10_6" ]
then
	echo "Building All 32Bit"
	xcodebuild -project Cabbage10_6.xcodeproj/ ONLY_ACTIVE_ARCH=NO 
	xcodebuild -project CabbagePlugin10_6.xcodeproj/ ARCHS=i386 ONLY_ACTIVE_ARCH=NO GCC_PREPROCESSOR_DEFINITIONS="Cabbage_Plugin_Synth=1 USE_DOUBLE=1 CSOUND6=1 MACOSX=1"
	cp -rf ./build/Debug/CabbagePlugin.component/ ./build/Debug/Cabbage.app/Contents/CabbagePluginSynth.component
	rm -rf ./build/Debug/CabbagePluginSynth.dat/CabbagePlugin.component	
	xcodebuild -project CabbagePlugin10_6.xcodeproj/ ONLY_ACTIVE_ARCH=NO GCC_PREPROCESSOR_DEFINITIONS="MACOSX=1 USE_DOUBLE=1"
	cp -rf ./build/Debug/CabbagePlugin.component/ ./build/Debug/Cabbage.app/Contents/CabbagePluginEffect.component
	rm -rf ./build/Debug/CabbagePluginEffect.component
	rm -rf ~/Library/Audio/Plug-Ins/VST/CabbagePlugin.vst

	echo "Bundling all files"	
	cp -rf ../../Docs ./build/Debug/Cabbage.app/Contents/MacOS/Docs
	cp -rf ../../Examples ./ß/Debug/Cabbage.app/Contents/MacOS/Examples
	cp opcodes.txt ./build/Debug/Cabbage.app/Contents/MacOS/opcodes.txt 

fi
