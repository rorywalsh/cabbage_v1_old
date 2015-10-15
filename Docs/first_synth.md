# Your first Cabbage instrument
The following section illustrates the steps involved in building a simple Cabbage instrument. It is assumed that the user has some prior knowledge of Csound. For a list of Csound tutorials and resources please visit the [Csound Homepage](www.csound.com). Each Cabbage instrument is defined in a simple text file with a .csd extension. The syntax used to create GUI widgets is quite straightforward and should be provided within special xml-style tags <Cabbage> and </Cabbage> which can appear either above or below Csound's own <CsoundSynthesizer> tags. Each line of Cabbage specific code relates to one GUI widget only. The attributes of each widget are set using different identifiers such as colour(), channel(), size() etc. Where identifiers are not used, Cabbage will use the default values. Long lines can be broken up with a '\' placed at the end of a line. 

Each and every Cabbage widget has 4 common parameters: position on screen(x, y) and size(width, height). Apart from position and size all other parameters are optional and if left out default values will be assigned. To set widget parameters you will need to use an appropriate identifier after the widget name. More information on the various widgets and identifiers available in Cabbage can be found in the Cabbage Widgets section of these docs.

## A basic Cabbage synthesiser

Code to create the most basic of Cabbage synthesisers is presented below. This instrument uses the MIDI interop command line flags to pipe MIDI data directly to p-fields in instrument 1. In this case all MIDI pitch data is being written directly to a special variable named p4, and all MIDI amplitude data is written to p5. MIDI data being sent on channel 1 will cause instrument 1 to play. Data being sent on channel 2 will cause instrument 2 to play, and so on. If you wish to assign an instrument to a unique MIDI channel you can use the 'massign' opcode. 

![](images/simpleSynthExample.png)


![](images/smallLogo.PNG)
You'll notice that a '-n' and '-d' are passed to Csound in the CsOptions section. -n stops Csound from writing audio to disk. The '-d' prevents any FLTK widgets from displaying. These flags are now set by defaul in Cabbage so you no longer need to pass them in the CsOpyions. You will also notice that our instrument is stereo. All Cabbage instruments are stereo. 
