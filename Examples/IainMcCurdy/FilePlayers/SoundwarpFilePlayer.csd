SoundwarpFilePlayer.csd

<Cabbage>
form caption("Soundwarp File Player") size(800,160), colour(0,0,0) pluginID("SWPl")
image                       bounds(  0,  0,800,160), colour( 30, 90, 60), outlinecolour("White"), line(3)	; main panel colouration    

filebutton bounds(  5, 10, 80, 25), text("Open File","Open File"), fontcolour("white") channel("filename"), shape("ellipse")
checkbox   bounds(  5, 38, 95, 17), channel("PlayStop"), text("Play/Stop"), fontcolour("white")
checkbox   bounds(  5, 58,100, 17), channel("freeze"), text("Freeze"), colour("LightBlue"), fontcolour("white")

label      bounds(285,  4, 65, 8), text("G   R   A   I   N   S"), fontcolour("white")
rslider    bounds( 90, 15, 60, 60), channel("overlap"),     range( 1, 100, 20, 1,1),             colour( 50,110, 80), text("Overlaps"),     fontcolour("white"), trackercolour(150,210,180)
rslider    bounds(145, 15, 60, 60), channel("grsize"),      range( 1, 40000, 4000, 0.5,1),       colour( 50,110, 80), text("Size"),         fontcolour("white"), trackercolour(150,210,180)
rslider    bounds(200, 15, 60, 60), channel("grsizeOS"),    range( 0, 2.00,   0.5,  0.5),        colour( 50,110, 80), text("Size OS"),      fontcolour("white"), trackercolour(150,210,180)
rslider    bounds(255, 15, 60, 60), channel("transpose"),   range(-48, 48, 0,1,0.001),               colour( 50,110, 80), text("Transpose"),    fontcolour("white"), trackercolour(150,210,180)

label      bounds(337, 18, 35, 10), text("M o d e"), fontcolour("white")
combobox   bounds(318, 28, 74, 17), channel("mode"), items("Speed", "Pointer"), value(1), fontcolour("white")
label      bounds(334, 47, 40, 10), text("S h a p e"), fontcolour("white")
combobox   bounds(318, 57, 74, 17), channel("shape"), items("Hanning", "Perc.1", "Perc.2", "Gate", "Rev.Perc.1", "Rev.Perc.2"), value(1), fontcolour("white")

rslider    bounds(395, 15, 60, 60), channel("speed"),       range( 0.00, 5.00, 1,0.5,0.001),           colour( 50,110, 80), text("Speed"),        fontcolour("white"), trackercolour(150,210,180), visible(1), identchannel("SpeedID")
rslider    bounds(395, 15, 60, 60), channel("ptr"),         range(     0,1.00, 0.5,1,0.001),            colour( 50,110, 80), text("Pointer"),      fontcolour("white"), trackercolour(150,210,180), visible(0), identchannel("PtrID")
rslider    bounds(450, 15, 60, 60), channel("ptrOS"),       range(     0, 1.00, 0),              colour( 50,110, 80), text("Ptr.OS"),       fontcolour("white"), trackercolour(150,210,180), visible(0), identchannel("PtrOSID")

rslider    bounds(505, 15, 60, 60), channel("inskip"),      range(     0, 1.00, 0),              colour( 50,110, 80), text("inskip"),       fontcolour("white"), trackercolour(150,210,180)

line       bounds(559, 10,  2, 65), colour("Grey")

label      bounds(576,  4, 90, 8), text("E   N   V   E   L   O   P   E"), fontcolour("white")
rslider    bounds(565, 15, 60, 60), channel("AttTim"),    range(0, 5.00, 0.01, 0.5, 0.001),        colour( 50,110, 80), text("Att.Tim"),   fontcolour("white"),trackercolour(150,210,180)
rslider    bounds(620, 15, 60, 60), channel("RelTim"),    range(0.01, 5, 0.05, 0.5, 0.001), colour( 50,110, 80), text("Rel.Tim"),   fontcolour("white"),trackercolour(150,210,180)

line       bounds(681, 10,  2, 65), colour("Grey")

label      bounds(700,  4, 80, 8), text("C   O   N   T   R   O   L"), fontcolour("white")
rslider    bounds(685, 15, 60, 60), channel("MidiRef"),   range(0,127,60, 1, 1),            colour( 50,110, 80), text("MIDI Ref."), fontcolour("white"),trackercolour(150,210,180)
rslider    bounds(740, 15, 60, 60), channel("level"),     range(  0,  3.00, 1, 0.5),        colour( 50,110, 80), text("Level"),     fontcolour("white"),trackercolour(150,210,180)

keyboard bounds(5, 80,790, 75)
</Cabbage>

<CsoundSynthesizer>

<CsOptions>
-n -+rtmidi=NULL -M0 -dm0
</CsOptions>

<CsInstruments>

sr = 44100
ksmps = 64
nchnls = 2
0dbfs=1

massign	0,3
gichans		init	0
giFileLen	init	0
giReady		init	0
gSfilepath	init	""

; WINDOWING FUNCTIONS USED TO DYNAMICALLY SHAPE THE GRAINS
; NUM | INIT_TIME | SIZE | GEN_ROUTINE | PARTIAL_NUM | STRENGTH | PHASE
; GRAIN ENVELOPE WINDOW FUNCTION TABLES:
giwfn1	ftgen	0,  0, 131072,  20,   1, 1 					; HANNING
giwfn2	ftgen	0,  0, 131072,  7,    0, 3072,   1, 128000,    0		; PERCUSSIVE - STRAIGHT SEGMENTS
giwfn3	ftgen	0,  0, 131072,  5, .001, 3072,   1, 128000, .001		; PERCUSSIVE - EXPONENTIAL SEGMENTS
giwfn4	ftgen	0,  0, 131072,  7,    0, 1536,   1, 128000,    1, 1536, 0	; GATE - WITH DE-CLICKING RAMP UP AND RAMP DOWN SEGMENTS
giwfn5	ftgen	0,  0, 131072,  7,    0, 128000, 1, 3072,      0		; REVERSE PERCUSSIVE - STRAIGHT SEGMENTS
giwfn6	ftgen	0,  0, 131072,  5, .001, 128000, 1, 3072,   .001		; REVERSE PERCUSSIVE - EXPONENTIAL SEGMENTS

instr	1
 gkloop		chnget	"loop"
 gkPlayStop	chnget	"PlayStop"
 gkfreeze	chnget	"freeze"
 gkfreeze	=	1-gkfreeze
 gktranspose	chnget	"transpose"
 gkoverlap	chnget	"overlap"
 gkgrsize	chnget	"grsize"
 gkgrsizeOS	chnget	"grsizeOS"
 gkshape	chnget	"shape"
 gkmode		chnget	"mode"
 gkmode		init	1
 gkspeed	chnget	"speed"
 gkptr		chnget	"ptr"
 gkptrOS	chnget	"ptrOS"
 gkinskip	chnget	"inskip"
 gklevel	chnget	"level"
 
 
 ; SHOW OR HIDE WIDGETS -------------------------------------
 kchange	changed	gkmode
 if(kchange==1) then
	String1 sprintfk "visible(%d)", 1 - (gkmode - 1)
	chnset String1, "SpeedID"
	String2 sprintfk "visible(%d)", gkmode - 1
	chnset String2, "PtrID"
	chnset String2, "PtrOSID"
endif
; -----------------------------------------------------------

 gSfilepath	chnget	"filename"
 kNewFileTrg	changed	gSfilepath		; if a new file is loaded generate a trigger
 if kNewFileTrg==1 then				; if a new file has been loaded...
  event	"i",99,0,0				; call instrument to update sample storage function table 
 endif  

 ktrig	trigger	gkPlayStop,0.5,0
 schedkwhen	ktrig,0,0,2,0,-1
endin



instr	99	; load sound file
 gichans	filenchnls	gSfilepath			; derive the number of channels (mono=1,stereo=2) in the sound file
 giFileLen	filelen		gSfilepath			; derive the number of channels (mono=1,stereo=2) in the sound file
 gitableL	ftgen	1,0,0,1,gSfilepath,0,0,1
 if gichans==2 then
  gitableR	ftgen	2,0,0,1,gSfilepath,0,0,2
 endif
 giReady 	=	1					; if no string has yet been loaded giReady will be zero
endin



instr	2	; triggered by 'play/stop' button
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

  kspeed	portk	gkspeed, kporttime
  kptr		portk	gkptr, kporttime
  
  ktranspose	portk	gktranspose,kporttime*3			; 
  
  ktrig	changed	gkshape,gkoverlap,gkgrsize,gkgrsizeOS,gkmode,gkinskip
  if ktrig==1 then
   reinit	UPDATE
  endif
  UPDATE:
  
  iwfn		=	i(gkshape) + giwfn1 - 1
  imode		=	i(gkmode) - 1
    
  if imode==0 then
   kwarp	=	1/(kspeed*gkfreeze)
  else
   kwarp	=	giFileLen * (kptr - i(gkinskip))
   kptrOS	random	0, (giFileLen - kwarp) * gkptrOS
   kwarp	=	kwarp + kptrOS
  endif

  apch		interp	semitone(ktranspose)
  ibeg		=	i(gkinskip) * giFileLen
  klevel	portk	gklevel/(i(gkoverlap)^0.25), kporttime		; apply portamento smoothing to changes in level
  
  if gichans==1 then						; if mono...   
   a1	sndwarp		klevel, kwarp, apch, gitableL, ibeg, i(gkgrsize), i(gkgrsize) * i(gkgrsizeOS), i(gkoverlap), iwfn, imode
 	outs	a1*aenv,a1*aenv					; send mono audio to both outputs 
  elseif gichans==2 then						; otherwise, if stereo...
   a1	sndwarp		klevel, kwarp, apch, gitableL, ibeg, i(gkgrsize), i(gkgrsize) * i(gkgrsizeOS), i(gkoverlap), iwfn, imode
   a2	sndwarp		klevel, kwarp, apch, gitableR, ibeg, i(gkgrsize), i(gkgrsize) * i(gkgrsizeOS), i(gkoverlap), iwfn, imode
 	outs	a1*aenv,a2*aenv					; send stereo signal to outputs
  endif
  rireturn
 endif
endin

instr	3
 icps	cpsmidi							; read in midi note data as cycles per second
 iamp	ampmidi	1						; read in midi velocity (as a value within the range 0 - 1)
 iAttTim	chnget	"AttTim"				; read in widgets
 iRelTim	chnget	"RelTim"
 iMidiRef	chnget	"MidiRef"
 iFrqRatio		=	icps/cpsmidinn(iMidiRef)	; derive playback speed from note played in relation to a reference note (MIDI note 60 / middle C)

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

  kprate	portk	gkspeed,kporttime
  
  ktranspose	portk	gktranspose,kporttime			; 
  
  ktrig	changed	gkshape,gkoverlap,gkgrsize,gkgrsizeOS,gkmode,gkinskip
  if ktrig==1 then
   reinit	UPDATE
  endif
  UPDATE:
  
  iwfn		=	i(gkshape) + giwfn1 - 1
  imode		=	i(gkmode) - 1
  
  if imode==0 then
   kwarp	=	1/(gkspeed*gkfreeze)
  else
   kwarp	=	giFileLen * (gkptr - i(gkinskip))
   kptrOS	random	0, (giFileLen - kwarp) * gkptrOS
   kwarp	=	kwarp + kptrOS
  endif  
  apch		interp	semitone(ktranspose)
  ibeg		=	i(gkinskip) * giFileLen
  klevel	portk	gklevel/(i(gkoverlap)^0.25), kporttime		; apply portamento smoothing to changes in level

  if gichans==1 then						; if mono...
   a1	sndwarp		klevel, kwarp, iFrqRatio, gitableL, ibeg, i(gkgrsize), i(gkgrsize) * i(gkgrsizeOS), i(gkoverlap), iwfn, imode
 	outs	a1*aenv,a1*aenv					; send mono audio to both outputs 
  elseif gichans==2 then						; otherwise, if stereo...
   a1	sndwarp		klevel, kwarp, iFrqRatio, gitableL, ibeg, i(gkgrsize), i(gkgrsize) * i(gkgrsizeOS), i(gkoverlap), iwfn, imode
   a2	sndwarp		klevel, kwarp, iFrqRatio, gitableR, ibeg, i(gkgrsize), i(gkgrsize) * i(gkgrsizeOS), i(gkoverlap), iwfn, imode
 	outs	a1*aenv,a2*aenv					; send stereo signal to outputs
  endif
  rireturn
 endif
endin

</CsInstruments>  

<CsScore>
i 1 0 [60*60*24*7]
</CsScore>

</CsoundSynthesizer>
