DiskinFilePlayer.csd

Plays a user selected sound file using diskin2 opcode. 
This file player is best suited for the playback of very long sound files and is less well suited for polyphonic playback.

The sound file can be played back using the Play/Stop button (and the 'Transpose' / 'Speed' buttons to implement pitch/speed change)
 or it can be played back using the MIDI keyboard.

Note that for 'reverse' to be effective either 'loop' needs to be active or inskip needs to be something other than zero

<Cabbage>
form caption("Diskin File Player") size(675, 160), pluginID("DkPl")
image                    bounds(  0,  0,675, 160), colour( 70, 30, 30), outlinecolour("White"), line(3)	; main panel colouration    

filebutton bounds(  5,  5, 80, 25), text("Open File","Open File"), fontcolour("white") channel("filename"), shape("ellipse")
checkbox   bounds(  5, 40, 95, 25), channel("PlayStop"), text("Play/Stop"), colour("lime"), fontcolour("white")
checkbox   bounds(105,  5,100, 15), channel("loop"), text("Loop On/Off"), colour("yellow"), fontcolour("white")
checkbox   bounds(105, 22,100, 15), channel("reverse"), text("Reverse"), colour("yellow"), fontcolour("white")
label      bounds(118, 40, 75, 12), text("Interpolation"), fontcolour("white")
combobox   bounds(105, 53,100, 20), channel("interp"), items("No interp.", "Linear", "Cubic", "Point Sinc"), value(3), fontcolour("white")
rslider    bounds(215,  5, 70, 70), channel("transpose"), range(-24, 24, 0,1,1),            colour( 90, 50, 50), trackercolour("silver"), text("Transpose"), fontcolour("white")
rslider    bounds(280,  5, 70, 70), channel("speed"),     range( -4,  4.00, 1),             colour( 90, 50, 50), trackercolour("silver"), text("Speed"), fontcolour("white")
rslider    bounds(345,  5, 70, 70), channel("inskip"),    range(  0,  1.00, 0),             colour( 90, 50, 50), trackercolour("silver"), text("In Skip"), fontcolour("white")
rslider    bounds(410,  5, 70, 70), channel("AttTim"),    range(0, 5, 0, 0.5, 0.001),       colour( 90, 50, 50), trackercolour("silver"), text("Att.Tim"),   fontcolour("white")
rslider    bounds(475,  5, 70, 70), channel("RelTim"),    range(0.01, 5, 0.05, 0.5, 0.001), colour( 90, 50, 50), trackercolour("silver"), text("Rel.Tim"),   fontcolour("white")
rslider    bounds(540,  5, 70, 70), channel("MidiRef"),   range(0,127,60, 1, 1),            colour( 90, 50, 50), trackercolour("silver"), text("MIDI Ref."), fontcolour("white")
rslider    bounds(605,  5, 70, 70), channel("level"),     range(  0,  3.00, 1, 0.5),        colour( 90, 50, 50), trackercolour("silver"), text("Level"), fontcolour("white")

keyboard bounds( 5, 80, 665, 75)
</Cabbage>

<CsoundSynthesizer>

<CsOptions>
-n -+rtmidi=NULL -M0
</CsOptions>

<CsInstruments>

sr = 44100
ksmps = 64
nchnls = 2
0dbfs=1

massign	0,3	; all midi notes on all channels sent to instrument 3

giInterpArr[] array 1, 2, 4, 8
gSfilepath	init	""

instr	1
 gkPlayStop	chnget	"PlayStop"		; read in widgets
 gkloop		chnget	"loop"
 gktranspose	chnget	"transpose"
 gkspeed	chnget	"speed"
 gkinterp	chnget	"interp"
 gkreverse	chnget	"reverse"
 gklevel	chnget	"level"
 gSfilepath	chnget	"filename"		; read in file path string from filebutton widget 
 ktrig		trigger	gkPlayStop,0.5,0	; if play/stop button toggles from low (0) to high (1) generate a '1' trigger
 schedkwhen	ktrig,0,0,2,0,-1		; start instrument 2
 
 ktrig1	changed	gktranspose			; if 'transpose' button is changed generate a '1' trigger
 ktrig2	changed	gkspeed				; if 'speed' button is changed generate a '1' trigger
 
 if ktrig1==1 then				; if transpose control has been changed...
  chnset	semitone(gktranspose),"speed"	; set speed according to transpose value
 elseif ktrig2==1&&gkspeed>=0 then		; if speed control has been changed...
  chnset	log2(gkspeed)*12,"transpose"	; set transpose control according to speed value
 endif
endin

instr	2
 if gkPlayStop==0 then				; if play/stop is off (stop)...
  turnoff					; turn off this instrument
 endif						
 iStrLen 	strlen gSfilepath		; derive string length
 if iStrLen > 0 then				; if string length is greater than zero (i.e. a file has been selected) then...
  iAttTim	chnget	"AttTim"		; read in amplitude envelope attack time widget
  iRelTim	chnget	"RelTim"		; read in amplitude envelope attack time widget
  if iAttTim>0 then				; 
   kenv	linsegr	0,iAttTim,1,iRelTim,0
  else								
   kenv	linsegr	1,iRelTim,0			; attack time is zero so ignore this segment of the envelope (a segment of duration zero is not permitted
  endif
  kenv	expcurve	kenv,8			; remap amplitude value with a more natural curve
  aenv	interp		kenv			; interpolate and create a-rate envelope
  iFileLen	filelen	gSfilepath		; derive chosen sound file length
  iNChns	filenchnls	gSfilepath	; derive the number of channels (mono=1 / stereo=2) from the chosen  sound file
  iinskip	chnget	"inskip"		; read in inskip widget
  iloop	chnget	"loop"				; read in 'loop mode' widget
  ktrig	changed	gkloop,gkinterp			; if loop setting or interpolation mode setting
  if ktrig==1 then				; if loop setting has been changed...
   reinit RESTART				; reinitialise from label RESTART
  endif						
  RESTART:					
  kporttime	linseg	0,0.001,0.05		; portamento time function. (Rises quickly from zero to a held value.)
  kspeed	portk	gkspeed,kporttime	; apply portamento smoothing to changes in speed
  if iNChns==2 then				; if stereo...
   a1,a2	diskin2	gSfilepath,kspeed*(1-(gkreverse*2)),iinskip*iFileLen,i(gkloop),0,giInterpArr[i(gkinterp)-1]	; use stereo diskin2
  	outs	a1*gklevel*aenv, a2*gklevel*aenv	; send audio to outputs
  elseif iNChns==1 then				; if mono
   a1	diskin2	gSfilepath,kspeed*(1-(gkreverse*2)),iinskip*iFileLen,i(gkloop),0,giInterpArr[i(gkinterp)-1]	; use mono diskin2
  	outs	a1*gklevel*aenv, a1*gklevel*aenv	; send audio to outputs
  endif
 endif
endin


instr	3
 icps	cpsmidi							; read in midi note data as cycles per second
 iamp	ampmidi	1						; read in midi velocity (as a value within the range 0 - 1)
 iStrLen 	strlen	gSfilepath				; derive string length
 if iStrLen > 0 then						; if string length is greater than zero (i.e. a file has been selected) then...
  iMidiRef	chnget	"MidiRef"				; MIDI unison reference note
  iinskip	chnget	"inskip"				; read in inskip widget
  iloop		chnget	"loop"					; read in 'loop mode' widget
  iAttTim	chnget	"AttTim"				; read in amplitude envelope attack time widget
  iRelTim	chnget	"RelTim"				; read in amplitude envelope attack time widget
  if iAttTim>0 then						; 
   kenv	linsegr	0,iAttTim,1,iRelTim,0
  else								
   kenv	linsegr	1,iRelTim,0					; attack time is zero so ignore this segment of the envelope (a segment of duration zero is not permitted
  endif
  kenv	expcurve	kenv,8					; remap amplitude value with a more natural curve
  aenv	interp		kenv					; interpolate and create a-rate envelope
  iFileLen	filelen	gSfilepath				; derive chosen sound file length
  ispeed		=	icps/cpsmidinn(iMidiRef)	; derive playback speed from note played in relation to a reference note (MIDI note 60 / middle C)
  iNChns	filenchnls	gSfilepath			; derive the number of channels (mono=1 / stereo=2) from the chosen  sound file
  if iNChns==2 then						; if stereo...
   a1,a2	diskin2	gSfilepath,ispeed*(1-(gkreverse*2)),iinskip*iFileLen,i(gkloop),0,giInterpArr[i(gkinterp)-1]	; use stereo diskin2
  	outs	a1*gklevel*aenv, a2*gklevel*aenv				; send audio to outputs
  elseif iNChns==1 then						; if mono
   a1		diskin2	gSfilepath,ispeed*(1-(gkreverse*2)),iinskip*iFileLen,i(gkloop),0,giInterpArr[i(gkinterp)-1]	; use mono diskin2
  	outs	a1*gklevel*aenv, a1*gklevel*aenv				; send audio to outputs
  endif
 endif
endin

</CsInstruments>  

<CsScore>
i 1 0 10000
</CsScore>

</CsoundSynthesizer>
