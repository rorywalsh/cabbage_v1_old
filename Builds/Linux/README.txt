Build Cabbage under Linux
-------------------------

Requirements
============

The following are dependencies you may need:

sudo apt-get -y install libfreetype6-dev
sudo apt-get -y install libx11-dev
sudo apt-get -y install libxinerama-dev
sudo apt-get -y install libxcursor-dev
sudo apt-get -y install mesa-common-dev
sudo apt-get -y install libasound2-dev
sudo apt-get -y install freeglut3-dev
sudo apt-get -y install libxcomposite-dev
sudo apt-get install csound
sudo apt-get install libcsound64-dev
sudo apt-get install libcsnd-dev
sudo apt-get install libsndfile1
sudo apt-get install libsndfile1-dev
sudo apt-get install libjack-dev

Please make sure that the versoin of Csound you use is newer that 6.02, otherwise you will get bulid problems. You'll also need to have the VST SDK. 

If you are having problems viewing fonts you should get the following package:

sudo apt-get install msttcorefonts

Compile
=======

To build run the following command from the Linux directory:

./buildCabbage [custom paths]

Running this script without any input prameters will build with default paths, i.e:
Csound include: "/usr/local/include/csound 
Csound library: "/usr/local/lib
VST SDK: "~/SDKs/vstsdk2.4"

Or if you've downloaded the VST3 SDK you should do something like 'foo/bar/VST3\ SDK' so as to avoid problems with the space in the folder name. 
You can also simply rename the VST3 folder so that it doesn't contain and spaces. 

If you wish to use custom paths please pass them to the buildCabbage script in this order: 
"csound include" "csound library" "vst sdk" 

Examples usuage with custom paths:

./buildCabbage "/usr/local/csound" "/usr/lib" "~/VST_SDK"

buildCabbage will create a CabbageBuild direcory that will contain everything you need to run Cabbage. It will also install Cabbage to the system menu through the use of a .desktop file. Please run Cabbage from the system menu in order to ensure that all examples and documentation are found
when using Cabbage. Some users have reported issues when running Cabbage directly from the build directory. 