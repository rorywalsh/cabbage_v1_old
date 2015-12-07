#!/bin/bash

if [ $1 = "release" ]; then
ant release
adb install -r bin/Cabbage-release.apk 
adb shell monkey -p com.yourcompany.cabbage -c android.intent.category.LAUNCHER 1
else
ant debug
adb install -r bin/Cabbage-debug.apk 
adb shell monkey -p com.yourcompany.cabbage -c android.intent.category.LAUNCHER 1
fi