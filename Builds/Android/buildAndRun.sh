ant debug
adb install -r bin/DummyAndroid-debug.apk 
adb shell monkey -p com.yourcompany.dummyandroid -c android.intent.category.LAUNCHER 1