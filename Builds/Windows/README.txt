Build Cabbage on Windows
-------------------------

If Csound is not installed in the usual place, i.e.,
"C:\Program Files (x86)\Csound" you will need to hack the makefiles so
that the compiler can find the Csound header files. You'll also need to pass
the location of the ASIO SDK and the VST SDK. You'll see how this is done in the 
relevent makefiles. 

Compile
=======

To build, run the following command from the Windows directory:

./buildCabbage

This should create a build directory with the Cabbage binary, as well
as two plugin libraries. If you move the Cabbage binary to another
location please make sure to also move the plugin libs, as they MUST
reside in the same folder as the main Cabbage binary.
