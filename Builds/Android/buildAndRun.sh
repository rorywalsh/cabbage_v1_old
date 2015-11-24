ant debug
adb install -r bin/CabbageAndroid-debug.apk 
adb shell monkey -p com.yourcompany.cabbageandroid -c android.intent.category.LAUNCHER 1