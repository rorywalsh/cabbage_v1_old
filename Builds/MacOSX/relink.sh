#!/bin/sh

export SUPPORT_LIBS_DIR=/Users/walshr/sourcecode/cabbageaudio/cabbage/Builds/MacOSX/build/Release/Cabbage.app/Contents/Frameworks/CsoundLib64.framework/libs
export ORIGINAL_LIBS_DIR=/Users/walshr/sourcecode/cabbageaudio/csoundLibs
export FRAMEWORK_DIR=/Users/walshr/sourcecode/cabbageaudio/cabbage/Builds/MacOSX/build/Release/Cabbage.app/Contents/Frameworks/CsoundLib64.framework
export LOADER_PATH=@loader_path/../Frameworks/CsoundLib64.framework/libs
# echo "copying support libs..."
cp $ORIGINAL_LIBS_DIR/libfltk.1.3.dylib $SUPPORT_LIBS_DIR
cp $ORIGINAL_LIBS_DIR/libfltk_images.1.3.dylib $SUPPORT_LIBS_DIR
cp $ORIGINAL_LIBS_DIR/libfltk_forms.1.3.dylib $SUPPORT_LIBS_DIR
cp $ORIGINAL_LIBS_DIR/liblo.7.dylib $SUPPORT_LIBS_DIR
cp $ORIGINAL_LIBS_DIR/libsndfile.1.dylib $SUPPORT_LIBS_DIR
cp $ORIGINAL_LIBS_DIR/libportaudio.2.dylib $SUPPORT_LIBS_DIR
cp $ORIGINAL_LIBS_DIR/libportmidi.dylib $SUPPORT_LIBS_DIR
cp $ORIGINAL_LIBS_DIR/libpng16.16.dylib $SUPPORT_LIBS_DIR
cp $ORIGINAL_LIBS_DIR/libFLAC.8.2.0.dylib $SUPPORT_LIBS_DIR
cp $ORIGINAL_LIBS_DIR/libvorbisenc.2.dylib $SUPPORT_LIBS_DIR
cp $ORIGINAL_LIBS_DIR/libvorbis.0.dylib $SUPPORT_LIBS_DIR
cp $ORIGINAL_LIBS_DIR/libogg.0.dylib $SUPPORT_LIBS_DIR
cp $ORIGINAL_LIBS_DIR/libfluidsynth.1.dylib $SUPPORT_LIBS_DIR
cp $ORIGINAL_LIBS_DIR/libwiiuse.dylib $SUPPORT_LIBS_DIR
cp $ORIGINAL_LIBS_DIR/libluajit-5.1.2.0.2.dylib $SUPPORT_LIBS_DIR 

echo "\install_name_tool -id.....\n"

install_name_tool -id $LOADER_PATH/libfltk.1.3.dylib $SUPPORT_LIBS_DIR/libfltk.1.3.dylib
install_name_tool -id $LOADER_PATH/libfltk_images.1.3.dylib $SUPPORT_LIBS_DIR/libfltk_images.1.3.dylib
install_name_tool -id $LOADER_PATH/libfltk_forms.1.3.dylib $SUPPORT_LIBS_DIR/libfltk_forms.1.3.dylib
install_name_tool -id $LOADER_PATH/liblo.7.dylib $SUPPORT_LIBS_DIR/liblo.7.dylib
install_name_tool -id $LOADER_PATH/libsndfile.1.dylib $SUPPORT_LIBS_DIR/libsndfile.1.dylib
install_name_tool -id $LOADER_PATH/libportaudio.2.dylib $SUPPORT_LIBS_DIR/libportaudio.2.dylib
install_name_tool -id $LOADER_PATH/libportmidi.dylib $SUPPORT_LIBS_DIR/libportmidi.dylib
install_name_tool -id $LOADER_PATH/libpng16.16.dylib $SUPPORT_LIBS_DIR/libpng16.16.dylib
install_name_tool -id $LOADER_PATH/libFLAC.8.2.0.dylib $SUPPORT_LIBS_DIR/libFLAC.8.2.0.dylib
install_name_tool -id $LOADER_PATH/libvorbisenc.2.dylib $SUPPORT_LIBS_DIR/libvorbisenc.2.dylib
install_name_tool -id $LOADER_PATH/libvorbis.0.dylib $SUPPORT_LIBS_DIR/libvorbis.0.dylib
install_name_tool -id $LOADER_PATH/libogg.0.dylib $SUPPORT_LIBS_DIR/libogg.0.dylib
install_name_tool -id $LOADER_PATH/libfluidsynth.1.dylib $SUPPORT_LIBS_DIR/libfluidsynth.1.dylib
install_name_tool -id $LOADER_PATH/libwiiuse.dylib $SUPPORT_LIBS_DIR/libwiiuse.dylib
install_name_tool -id $LOADER_PATH/libluajit-5.1.2.0.2.dylib $SUPPORT_LIBS_DIR/libluajit-5.1.2.0.2.dylib


echo "\install_name_tool -change.....\n"

# change deps for libsndfile
export OLD_VORBISENC_LIB=$ORIGINAL_LIBS_DIR/libvorbisenc.2.dylib
export NEW_VORBISENC_LIB=@loader_path/../Frameworks/CsoundLib64.framework/libs/libvorbisenc.2.dylib
install_name_tool -change $OLD_VORBISENC_LIB $NEW_VORBISENC_LIB $SUPPORT_LIBS_DIR/libsndfile.1.dylib

export OLD_VORBIS_LIB=$ORIGINAL_LIBS_DIR/libvorbis.0.dylib
export NEW_VORBIS_LIB=@loader_path/../Frameworks/CsoundLib64.framework/libs/libvorbis.0.dylib
install_name_tool -change $OLD_VORBIS_LIB $NEW_VORBIS_LIB $SUPPORT_LIBS_DIR/libsndfile.1.dylib
install_name_tool -change $OLD_VORBIS_LIB $NEW_VORBIS_LIB $SUPPORT_LIBS_DIR/libvorbisenc.2.dylib

export OLD_OGG_LIB=$ORIGINAL_LIBS_DIR/libogg.0.dylib
export NEW_OGG_LIB=@loader_path/../Frameworks/CsoundLib64.framework/libs/libogg.0.dylib
install_name_tool -change $OLD_OGG_LIB $NEW_OGG_LIB $SUPPORT_LIBS_DIR/libsndfile.1.dylib
install_name_tool -change $OLD_OGG_LIB $NEW_OGG_LIB $SUPPORT_LIBS_DIR/libvorbis.0.dylib
install_name_tool -change $OLD_OGG_LIB $NEW_OGG_LIB $SUPPORT_LIBS_DIR/libvorbisenc.2.dylib

export OLD_FLAC_LIB=$ORIGINAL_LIBS_DIR/libFLAC.8.2.0.dylib
export NEW_FLAC_LIB=@loader_path/../Frameworks/CsoundLib64.framework/libs/libFLAC.8.2.0.dylib
install_name_tool -change $OLD_FLAC_LIB $NEW_FLAC_LIB $SUPPORT_LIBS_DIR/libsndfile.1.dylib
install_name_tool -change $OLD_OGG_LIB $NEW_OGG_LIB $SUPPORT_LIBS_DIR/libFLAC.8.2.0.dylib

install_name_tool -change $ORIGINAL_LIBS_DIR/libfltk.1.3.dylib @loader_path/../Frameworks/CsoundLib64.framework/libs/libfltk.1.3.dylib  $SUPPORT_LIBS_DIR/libfltk_images.1.3.dylib
install_name_tool -change $ORIGINAL_LIBS_DIR/libfltk.1.3.dylib @loader_path/../Frameworks/CsoundLib64.framework/libs/libfltk.1.3.dylib  $SUPPORT_LIBS_DIR/libfltk_forms.1.3.dylib
install_name_tool -change $ORIGINAL_LIBS_DIR/libpng16.16.dylib @loader_path/../Frameworks/CsoundLib64.framework/libs/libpng16.16.dylib  $SUPPORT_LIBS_DIR/libfltk_images.1.3.dylib

install_name_tool -change $ORIGINAL_LIBS_DIR/libportaudio.2.dylib @loader_path/libportaudio.2.dylib $SUPPORT_LIBS_DIR/libfluidsynth.1.dylib

# install name changes for libs under framework, luajit not included here
install_name_tool -change $ORIGINAL_LIBS_DIR/libsndfile.1.dylib 		@exectuble_path/../Frameworks/CsoundLib64.framework/libs/libsndfile.1.dylib 		$FRAMEWORK_DIR/CsoundLib64
install_name_tool -change $ORIGINAL_LIBS_DIR/libsndfile.1.dylib 		@loader_path/../Frameworks/CsoundLib64.framework/libs/libsndfile.1.dylib 		$FRAMEWORK_DIR/Opcodes64/libstdutil.dylib
install_name_tool -change $ORIGINAL_LIBS_DIR/libfltk.1.3.dylib 			@loader_path/../Frameworks/CsoundLib64.framework/libs/libfltk.1.3.dylib 		$FRAMEWORK_DIR/Opcodes64/libwidgets.dylib
install_name_tool -change $ORIGINAL_LIBS_DIR/libfltk_images.1.3.dylib 	@loader_path/../Frameworks/CsoundLib64.framework/libs/libfltk_images.1.3.dylib 	$FRAMEWORK_DIR/Opcodes64/libwidgets.dylib
install_name_tool -change $ORIGINAL_LIBS_DIR/libfltk_forms.1.3.dylib 	@loader_path/../Frameworks/CsoundLib64.framework/libs/libfltk_forms.1.3.dylib 	$FRAMEWORK_DIR/Opcodes64/libwidgets.dylib
install_name_tool -change $ORIGINAL_LIBS_DIR/libfltk.1.3.dylib 			@loader_path/../Frameworks/CsoundLib64.framework/ibs/libfltk.1.3.dylib 			$FRAMEWORK_DIR/Opcodes64/libvirtual.dylib
install_name_tool -change $ORIGINAL_LIBS_DIR/libfltk_images.1.3.dylib 	@loader_path/../Frameworks/CsoundLib64.framework/libs/libfltk_images.1.3.dylib 	$FRAMEWORK_DIR/Opcodes64/libvirtual.dylib
install_name_tool -change $ORIGINAL_LIBS_DIR/liblo.7.dylib 				@loader_path/../Frameworks/CsoundLib64.framework/libs/liblo.7.dylib 			$FRAMEWORK_DIR/Opcodes64/libosc.dylib
install_name_tool -change $ORIGINAL_LIBS_DIR/libportaudio.2.dylib 		@loader_path/../Frameworks/CsoundLib64.framework/libs/libportaudio.2.dylib 		$FRAMEWORK_DIR/Opcodes64/librtpa.dylib
install_name_tool -change $ORIGINAL_LIBS_DIR/libwiiuse.dylib 			@loader_path/../Frameworks/CsoundLib64.framework/libs/libwiiuse.dylib 			$FRAMEWORK_DIR/Opcodes64/libwiimote.dylib
install_name_tool -change $ORIGINAL_LIBS_DIR/libpng16.16.dylib 			@loader_path/../Frameworks/CsoundLib64.framework/libs/libpng16.16.dylib 		$FRAMEWORK_DIR/Opcodes64/libimage.dylib
install_name_tool -change $ORIGINAL_LIBS_DIR/libfluidsynth.1.dylib 		@loader_path/../Frameworks/CsoundLib64.framework/libs/libfluidsynth.1.dylib 	$FRAMEWORK_DIR/Opcodes64/libfluidOpcodes.dylib

#install_name_tool -change libportmidi.dylib @loader_path/../../../../libs/libportmidi.dylib $FRAMEWORK64_DIR/Resources/Opcodes64/libpmidi.dylib