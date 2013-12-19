Build Cabbage under OSX
-------------------------

Requirements
============

You will need to have Csound6 installed. You'll also need to have the VST SDK. 
If you place the VSTSDK in a directory called SDKs in your home diectory, i.e, ~SDKs
then you will not have to edit the Cabbage Xcode projects. If you place the SDK in 
another directory you will need to edit the include paths in the Cabbage and Cabbage
Plugin Xcode Projects. 

Building
========

To build the entire Cabbage package you only need to call the buildAll.sh script contained
in the MacOSX folder of Cabbage source. You may need to change the permissions of the build 
script before calling it. If you cd to the MacOSX folder you should then do the following:

chmod 777 buildAll.sh
./buildAll.sh all

This should create a Debug build directory with a Cabbage bundle. The bundle will contain
the required plugins libraries, examples, docs, and everything else you need in order
to start using Cabbage. 

Building directly in XCode
==========================

Cabbage and the Cabbage dummy plugins can be build directly in Xcode by opening and building 
Cabbage.xcodeproj and CabbagePlugin.xcodeproj. You will need to build two versions of the Cabbage
plugin project. One with the PreProcessor directive "Cabbage_Plugin_Synth=1" and one without. You will
also need to rename the resulting .component files, renaming them CabbagePluginSynth.component, and
CabbagePluginEffect.componenet. You then need to place them in the contents directory of the Cabbage 
bundle. The buildAll.sh script automates all this..
