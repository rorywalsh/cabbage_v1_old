Build Cabbage on Windows
-------------------------
To build for Windows you will need to have MINGW, MSYS and Csound installed. If Csound is not installed in the usual place, i.e., "C:\Program Files (x86)\Csound" you will need to hack the .bat files so that the compiler can find the Csound header files. You'll also need to pass the location of the ASIO SDK and the VST SDK. You'll see how this is done in the .bat files.

Why are there both .bat files and Makefiles included?
=====================================================
The makefiles will build Cabbage on your PC, but the resulting binaries will not run on other machines(I'm not sure why, feel free to investigate). The .bat files will build binaries that can be used on other Windows machines. 

Building
========

To build, run the following command from the Windows directory:

./buildCabbage

This should create a build directory with the Cabbage binary, as well as two plugin libraries. If you move the Cabbage binary to another location please make sure to also move the plugin libs, as they MUST reside in the same folder as the main Cabbage binary. You will need place linshdfile-1.dll into the same directory as Cabbage in order for it to run. 
