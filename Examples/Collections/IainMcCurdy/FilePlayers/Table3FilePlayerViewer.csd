Table3FilePlayer.csd

Load a user selected sound file into a GEN 01 function table and plays it back using loscil3. 
This file player is suited for polyphonic playback and is less well suited for the playback of very long sound files .


<Cabbage>
form caption("Table3 File Player") size(575,340), colour(0,0,0) pluginID("T3Pl")
image                    bounds(  0,  0,575,340), colour(30, 30, 70), outlinecolour("White"), line(3), shape("sharp")	; main panel colouration    

soundfiler bounds(  5,  5,565,175), channel("beg","len"), identchannel("filer1"),  colour(0, 255, 255, 255), fontcolour(160, 160, 160, 255), 

filebutton bounds(  5,190, 80, 25), text("Open File","Open File"), fontcolour("white") channel("filename"), shape("ellipse")
checkbox   bounds(  5,220, 95, 25), channel("PlayStop"), text("Play/Stop"), colour("yellow")

label      bounds(133,184, 80, 8), text("L   O   O   P"), fontcolour("white")

groupbox   bounds(100,200,100, 50), plant("looping"), text("Looping Mode"), fontcolour("white"){
combobox   bounds( 10, 25, 80, 20), channel("mode"), items("Forward", "Backward", "Fwd./Bwd."), value(1), fontcolour("white")
}

line       bounds(210,190,  2, 65), colour("Grey")

label      bounds(247,184, 80, 8), text("S   P   E   E   D"), fontcolour("white")
rslider    bounds(215,195, 60, 60), channel("transpose"), range(-24, 24, 0,1,1),            colour(50, 50, 90), text("Transpose"), fontcolour("white"), trackercolour(210,210,250)
rslider    bounds(270,195, 60, 60), channel("speed"),     range(0, 4.00, 1, 0.5, 0.001),               colour(50, 50, 90), text("Speed"),     fontcolour("white"), trackercolour(210,210,250)

line       bounds(330,190,  2, 65), colour("Grey")

label      bounds(348,184,120, 8), text("E   N   V   E   L   O   P   E"), fontcolour("white")
rslider    bounds(335,195, 60, 60), channel("AttTim"),    range(0, 5, 0.01, 0.5, 0.001),       colour(50, 50, 90), text("Att.Tim"),   fontcolour("white"), trackercolour(210,210,250)
rslider    bounds(390,195, 60, 60), channel("RelTim"),    range(0.01, 5, 0.05, 0.5, 0.001), colour(50, 50, 90), text("Rel.Tim"),   fontcolour("white"), trackercolour(210,210,250)
line       bounds(450,190,  2, 65), colour("Grey")

label      bounds(472,184, 80, 8), text("C   O   N   T   R   O   L"), fontcolour("white")
rslider    bounds(455,195, 60, 60), channel("MidiRef"),   range(0,127,60, 1, 1),            colour(50, 50, 90), text("MIDI Ref."), fontcolour("white"), trackercolour(210,210,250)
rslider    bounds(510,195, 60, 60), channel("level"),     range(  0,  3.00, 1, 0.5),        colour(50, 50, 90), text("Level"),     fontcolour("white"), trackercolour(210,210,250)

keyboard bounds(5,260, 565, 75)
</Cabbage>

<CsoundSynthesizer>

<CsOptions>
-n -+rtmidi=NULL -M0 -dm0
</CsOptions>

<CsInstruments>

sr = 44100
ksmps = 64
nchnls = 2
0dbfs = 1

		massign	0,3
gichans		init	0
giFileLen	init	0
giReady		init	0
gSfilepath	init	""
gitri		ftgen	0,0,131072,7,0,131072/2,1,131072/2,0

instr	1
 gkloop		chnget	"loop"
 gkLoopStart	chnget	"beg"
 gkLoopEnd	chnget	"len"
 gkPlayStop	chnget	"PlayStop"
 gktranspose	chnget	"transpose"
 gkspeed	chnget	"speed"
 gklevel	chnget	"level"
 gkmode		chnget	"mode"
 
 gSfilepath	chnget	"filename"
 kNewFileTrg	changed	gSfilepath		; if a new file is loaded generate a trigger
 if kNewFileTrg==1 then				; if a new file has been loaded...
  event	"i",99,0,0				; call instrument to update sample storage function table 
 endif  
 
 ktrig	trigger	gkPlayStop,0.5,0		; if play button changes to 'play', generate a trigger
 schedkwhen	ktrig,0,0,2,0,-1		; start instr 2 playing a held note

 ktrig1	changed	gktranspose			; if 'transpose' button is changed generate a '1' trigger
 ktrig2	changed	gkspeed				; if 'speed' button is changed generate a '1' trigger
 
 if ktrig1==1 then				; if transpose control has been changed...
  chnset	semitone(gktranspose),"speed"	; set speed according to transpose value
 elseif ktrig2==1 then		; if speed control has been changed...
  chnset	log2(abs(gkspeed))*12,"transpose"	; set transpose control according to speed value
 endif

endin

instr	99	; load sound file
 gichans	filenchnls	gSfilepath			; derive the number of channels (mono=1,stereo=2) in the sound file
 gitableL	ftgen	1,0,0,1,gSfilepath,0,0,1
 giFileSamps	=		nsamp(gitableL)			; derive the file duration in samples
 if gichans==2 then
  gitableR	ftgen	2,0,0,1,gSfilepath,0,0,2
 endif
 giReady 	=	1					; if no string has yet been loaded giReady will be zero
 kzero		init	0
 
 Smessage sprintfk "file(%s)", gSfilepath
 chnset Smessage, "filer1"	

endin




instr	2	; sample triggered by 'play/stop' button
 if gkPlayStop==0 then
  turnoff
 endif

 if giReady = 1 then						; i.e. if a file has been loaded
  iAttTim	chnget	"AttTim"				; read in widgets
  iRelTim	chnget	"RelTim"
  if iAttTim>0 then						; is amplitude envelope attack time is greater than zero...
   kenv	linsegr	0,iAttTim,1,iRelTim,0				; create an amplitude envelope with an attack, a sustain and a release segment (senses realtime release)
  else
   kenv	linsegr	1,iRelTim,0					; create an amplitude envelope with a sustain and a release segment (senses realtime release)
  endif
  kenv	expcurve	kenv,8					; remap amplitude value with a more natural curve
  aenv	interp		kenv					; interpolate and create a-rate envelope
  kporttime	linseg	0,0.001,0.05				; portamento time function. (Rises quickly from zero to a held value.)
  kspeed	portk	gkspeed,kporttime			; apply portamento smoothing to changes in speed
  klevel	portk	gklevel,kporttime			; apply portamento smoothing to changes

  if (gkLoopEnd==0 && gkLoopStart==0) then			; if loop positions haven't yet been set
   gkLoopEnd	=	giFileSamps
  endif
  if gkLoopEnd==0 kgoto SKIP					; if loop duration is zero don't produce any audio

  krate		=	kspeed * sr / gkLoopEnd
  arate		interp	krate

  if gkmode==1 then
   aphasor	phasor	arate
  elseif gkmode==2 then
   aphasor	phasor	-arate
  else
   aphasor	poscil	1,-arate*0.5,gitri
  endif
  
  aLoopStart	interp	gkLoopStart
  aLoopEnd	interp	gkLoopEnd
  aphasor	=	(aphasor*aLoopEnd)+aLoopStart
  
  if gichans==1 then						; if mono...
   a1	table3	aphasor, gitableL
 	outs	a1*aenv*klevel, a1*aenv*klevel			; send mono audio to both outputs 
  elseif gichans==2 then					; otherwise, if stereo...
   a1	table3	aphasor, gitableL
   a2	table3	aphasor, gitableR
 	outs	a1*aenv*klevel, a2*aenv*klevel			; send stereo signal to outputs
  endif               
  
  SKIP:								; jump to here if loop duration was zero
  
 endif

 ; print scrubber
 if(metro(20)==1) then
  kscrubber	downsamp	aphasor
  Smessage sprintfk "scrubberposition(%d)", kscrubber
  chnset Smessage, "filer1"
 endif

endin

instr	3	; sample triggered by midi note
 icps	cpsmidi							; read in midi note data as cycles per second
 iamp	ampmidi	1						; read in midi velocity (as a value within the range 0 - 1)
 iMidiRef	chnget	"MidiRef"

 if giReady = 1 then						; i.e. if a file has been loaded
  iAttTim	chnget	"AttTim"				; read in widgets
  iRelTim	chnget	"RelTim"
  if iAttTim>0 then						; is amplitude envelope attack time is greater than zero...
   kenv	linsegr	0,iAttTim,1,iRelTim,0				; create an amplitude envelope with an attack, a sustain and a release segment (senses realtime release)
  else
   kenv	linsegr	1,iRelTim,0					; create an amplitude envelope with a sustain and a release segment (senses realtime release)
  endif
  kenv	expcurve	kenv,8					; remap amplitude value with a more natural curve
  aenv	interp		kenv					; interpolate and create a-rate envelope
  kporttime	linseg	0,0.001,0.05				; portamento time function. (Rises quickly from zero to a held value.)
  ispeed	=	icps/cpsmidinn(iMidiRef)		; derive playback speed from note played in relation to a reference note (MIDI note 60 / middle C)
  klevel	portk	gklevel,kporttime			; apply portamento smoothing to changes in level


  if (gkLoopEnd==0 && gkLoopStart==0) then			; if loop positions haven't yet been set
   gkLoopEnd	=	giFileSamps
  endif
  if gkLoopEnd==0 kgoto SKIP					; if loop duration is zero don't produce any audio
    
  krate		=	ispeed * sr / gkLoopEnd
  arate		interp	krate

  if gkmode==1 then
   aphasor	phasor	arate
  elseif gkmode==2 then
   aphasor	phasor	-arate
  else
   aphasor	poscil	1,-arate*0.5,gitri
  endif
  
  aLoopStart	interp	gkLoopStart
  aLoopEnd	interp	gkLoopEnd
  aphasor	=	(aphasor*aLoopEnd)+aLoopStart

  if gichans==1 then						; if mono...
   a1	table3	aphasor, gitableL
 	outs	a1*aenv*klevel, a1*aenv*klevel			; send mono audio to both outputs 
  elseif gichans==2 then					; otherwise, if stereo...
   a1	table3	aphasor, gitableL
   a2	table3	aphasor, gitableR
 	outs	a1*aenv*klevel, a2*aenv*klevel			; send stereo signal to outputs
  endif               
  SKIP:								; jump to here if loop duration was zero

 endif

  if(metro(20)==1) then
   kscrubber	downsamp	aphasor
   Smessage sprintfk "scrubberposition(%d)", kscrubber
   chnset Smessage, "filer1"
  endif

endin
 
</CsInstruments>  

<CsScore>
i 1 0 [60*60*24*7]
</CsScore>

</CsoundSynthesizer>