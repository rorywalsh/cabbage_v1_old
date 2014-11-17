<Cabbage>
form caption("Mouse Sense") size(785, 400), pluginID("SMo1"), guirefresh(10)

rslider bounds( 45,  5,100, 30), text("MOUSE X"),		fontcolour("white"), textbox(1),		channel("X"), 	range(0, 800, 0)
rslider bounds(140,  5,100, 30), text("MOUSE Y"),		fontcolour("white"), textbox(1),		channel("Y"), 	range(0, 400, 0)
rslider bounds(  5, 50,100, 30), text("MOUSE LEFT"),		fontcolour("white"), textbox(1),		channel("LEFT"), 	range(0, 1, 0,1,1)
rslider bounds( 95, 50,100, 30), text("MOUSE MIDDLE"),		fontcolour("white"), textbox(1),		channel("MIDDLE"), 	range(0, 1, 0,1,1)
rslider bounds(185, 50,100, 30), text("MOUSE RIGHT"),		fontcolour("white"), textbox(1),		channel("RIGHT"), 	range(0, 1, 0,1,1)

;numberbox bounds( 45,  5, 50, 15), text("MOUSE X"),		fontcolour("white"), textbox(1),		channel("X"), 	range(0, 800, 0,1,0.001)

textbox bounds(300, 10,475,350), file("MouseSenseInfo.txt")

</Cabbage>

<CsoundSynthesizer>

<CsOptions>
-d -n
</CsOptions>

<CsInstruments>

sr 		= 	44100	;SAMPLE RATE
ksmps 		= 	32	;NUMBER OF AUDIO SAMPLES IN EACH CONTROL CYCLE
nchnls 		= 	2	;NUMBER OF CHANNELS (2=STEREO)
0dbfs		=	1

instr	1
 kMOUSE_X		chnget	"MOUSE_X"
 kMOUSE_Y		chnget	"MOUSE_Y"
 kMOUSE_DOWN_LEFT	chnget	"MOUSE_DOWN_LEFT"
 kMOUSE_DOWN_MIDDLE	chnget	"MOUSE_DOWN_MIDDLE"
 kMOUSE_DOWN_RIGHT	chnget	"MOUSE_DOWN_RIGHT"

 if changed(kMOUSE_X)==1 then
  chnset	kMOUSE_X, "X"
 endif

 if changed(kMOUSE_Y)==1 then
  chnset	kMOUSE_Y, "Y"
 endif

 if changed(kMOUSE_DOWN_LEFT)==1 then
  chnset	kMOUSE_DOWN_LEFT, "LEFT"
 endif

 if changed(kMOUSE_DOWN_MIDDLE)==1 then
  chnset	kMOUSE_DOWN_MIDDLE, "MIDDLE"
 endif

 if changed(kMOUSE_DOWN_RIGHT)==1 then
  chnset	kMOUSE_DOWN_RIGHT, "RIGHT"
 endif

endin
		
</CsInstruments>

<CsScore>
i 1 0 [3600*24*7]
</CsScore>

</CsoundSynthesizer>