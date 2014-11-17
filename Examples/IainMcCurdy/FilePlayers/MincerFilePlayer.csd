; MincerFilePlayer.csd
; Written by Iain McCurdy, 2014


; Mouse Scrubber activates by right-clicking and dragging over the waveform.

<Cabbage>
form caption("Mincer File Player") size(570,340), colour(0,0,0) pluginID("Minc"), guirefresh(10) 
image bounds(  0,  0,570,340), colour( 50,100,100), outlinecolour("White"), line(3), shape("sharp")

soundfiler bounds(  5,  5,560,150), channel("beg","len"), channel("pos1","end1"), identchannel("filer1"),  colour(0, 255, 255, 255), fontcolour(160, 160, 160, 255), 

hslider    bounds(  0,150,570, 30), channel("pointer"), range( 0,  1.00, 0.1), colour( 40, 80, 80),  trackercolour("white"), fontcolour("white")
label      bounds(241,172, 88, 12), text("Manual Pointer"), fontcolour("white")

filebutton bounds(  5,190, 80, 25), text("Open File","Open File"), fontcolour("white"), channel("filename"), shape("ellipse")

checkbox bounds(  5,223,120, 12), text("Manual Pointer"), channel("r1"), fontcolour("white"), colour(yellow)
checkbox bounds(  5,237,120, 12), text("Mouse Scrubber"), channel("r2"), fontcolour("white"), colour(yellow), value(1) 

checkbox   bounds(125,223, 60, 12), channel("lock"), text("Lock"), colour("yellow"), fontcolour("white"), value(1)

label      bounds(105,188, 48, 12), text("FFT Size"), fontcolour("white")
combobox   bounds( 95,200, 70, 20), channel("FFTSize"), items("32768", "16384", "8192", "4096", "2048", "1024", "512", "256", "128", "64", "32", "16", "8", "4"), value(5), fontcolour("white")

rslider    bounds(175,185, 70, 70), channel("transpose"), range(-48, 48, 0,1,1),            colour( 40, 80, 80)), trackercolour("white"), text("Transpose"), fontcolour("white")
rslider    bounds(240,185, 70, 70), channel("portamento"),range(0, 0.500,0.05),         colour( 40, 80, 80)), trackercolour("white"), text("Port.Time"), fontcolour("white")
rslider    bounds(305,185, 70, 70), channel("AttTim"),    range(0, 5, 0, 0.5, 0.001),       colour( 40, 80, 80),  trackercolour("white"), text("Att.Tim"),   fontcolour("white")
rslider    bounds(370,185, 70, 70), channel("RelTim"),    range(0.01, 5, 0.05, 0.5, 0.001), colour( 40, 80, 80),  trackercolour("white"), text("Rel.Tim"),   fontcolour("white")
rslider    bounds(435,185, 70, 70), channel("MidiRef"),   range(0,127,60, 1, 1),            colour( 40, 80, 80),  trackercolour("white"), text("MIDI Ref."), fontcolour("white")
rslider    bounds(500,185, 70, 70), channel("level"),     range(  0,  3.00, 1, 0.5),        colour( 40, 80, 80),  trackercolour("white"), text("Level"),     fontcolour("white")

keyboard bounds( 5,260, 560, 75)
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



opcode	CabbageRadio2,k,SS			; change opcode name and number is string variable inputs
S1,S2	xin					; add string inputs for the required number of inputs 
kon		=	1
koff		=	0
ksum		=	0
ktrigsum	=	0
#define READ_CHANGES(NAME)	#
k$NAME	chnget	$NAME
ksum	=	ksum + k$NAME			; sum of values
kon$NAME	trigger	k$NAME,0.5,0		; trigger that a checkbox has been turned on
ktrigsum	=	ktrigsum + kon$NAME#	; sum of all on triggers

#define WRITE_CHANGES(NAME'COUNT)	#
if kon$NAME!=1 then
 chnset	koff,$NAME
else
 kval	=	$COUNT
endif#

#define PREVENT_ALL_OFF(NAME)	#
koff$NAME	trigger	k$NAME,0.5,1		; trigger that a checkbox has been turned on
if koff$NAME==1&&ksum==0 then
 chnset	kon,$NAME
endif#

$READ_CHANGES(S1) 
$READ_CHANGES(S2)				; add macro expansions for the required number of radio buttons

if ktrigsum>0 then
 $WRITE_CHANGES(S1'1)
 $WRITE_CHANGES(S2'2)				; add macro expansions for the required number of radio buttons
endif

$PREVENT_ALL_OFF(S1)
$PREVENT_ALL_OFF(S2)				; add macro expansions for the required number of radio buttons

kval	=	(ksum=0?0:kval)
	xout	kval
endop



massign	0,3

gichans		init	0		; 
giReady		init	0		; flag to indicate function table readiness

giFFTSizes[]	array	32768, 16384, 8192, 4096, 2048, 1024, 512, 256, 128, 64, 32, 16, 8, 4	; an array is used to store FFT window sizes

gSfilepath	init	""

instr	1
 ; derive a value for upper/lower row by using the two switches (r1 and r2) as radio buttons
 gkmode		CabbageRadio2	"r1","r2"	; call UDO
 gkmode		init	2
 kporttime	chnget	"portamento"	
 krampup	linseg	0,0.001,1
 gkloop		chnget	"loop"
 gklevel	chnget	"level"
 gklock		chnget	"lock"
 gkfreeze	chnget	"freeze"
 gkfreeze	=	1-gkfreeze
 gkFFTSize	chnget	"FFTSize"
 gSfilepath	chnget	"filename"
 kNewFileTrg	changed	gSfilepath		; if a new file is loaded generate a trigger

 gkMOUSE_DOWN_RIGHT	chnget	"MOUSE_DOWN_RIGHT"	; Read in mouse left click status
 kStartScrub		trigger	gkMOUSE_DOWN_RIGHT,0.5,0
 
 if gkMOUSE_DOWN_RIGHT==1 && gkmode==2 then
  kMOUSE_X	chnget	"MOUSE_X"
  kMOUSE_Y	chnget	"MOUSE_Y"
  if kStartScrub==1 then 
   reinit RAMP_FUNC
  endif
  RAMP_FUNC:
  krampup	linseg	0,0.001,1
  rireturn
  kMOUSE_X	portk	(kMOUSE_X - 5) / 555, kporttime*krampup
  kMOUSE_Y	=	1 - ((kMOUSE_Y - 5) / 145)
  gapointer	interp	kMOUSE_X
  gktranspose	=	(kMOUSE_Y * 48) - 24
  schedkwhen	kStartScrub,0,0,2,0,-1
 else
  kpointer	chnget	"pointer"
  kpointer	portk	kpointer,kporttime*krampup
  gapointer	interp	kpointer
  gktranspose	chnget	"transpose"
 endif
 
 if kNewFileTrg==1 then				; if a new file has been loaded...
  event	"i",99,0,0				; call instrument to update sample storage function table 
 endif  
 
 kStartManual	trigger	gkmode,1.5,1
 printk2	kStartManual
 schedkwhen	kStartManual,0,0,2,0,-1
endin

instr	99	; load sound file
 gichans	filenchnls	gSfilepath			; derive the number of channels (mono=1,stereo=2) in the sound file
 gitableL	ftgen	1,0,0,1,gSfilepath,0,0,1
 if gichans==2 then
  gitableR	ftgen	2,0,0,1,gSfilepath,0,0,2
 endif
 giReady 	=	1					; if no string has yet been loaded giReady will be zero

 Smessage sprintfk "file(%s)", gSfilepath			; print sound file image to fileplayer
 chnset Smessage, "filer1"	
endin

instr	2	; non-midi
 if gkmode!=1 && gkMOUSE_DOWN_RIGHT!=1 then
  turnoff
 endif
 if giReady = 1 then				; i.e. if a file has been loaded
  aenv	linsegr	0,0.01,1,0.01,0			; simple de-click envelope
  
  kporttime	linseg	0,0.001,0.01
  ktranspose	portk	gktranspose,kporttime
  
  ktrig	changed		gkFFTSize
  if ktrig==1 then
   reinit RESTART
  endif
  RESTART:
  iFileLen	=	ftlen(gitableL)/sr
  if gichans=1 then
   a1	mincer		gapointer*iFileLen, gklevel, semitone(ktranspose), gitableL, gklock, giFFTSizes[i(gkFFTSize)-1]
  	outs	a1*aenv,a1*aenv
  elseif gichans=2 then
   a1	mincer		gapointer*iFileLen, gklevel, semitone(ktranspose), gitableL, gklock, giFFTSizes[i(gkFFTSize)-1]
   a2	mincer		gapointer*iFileLen, gklevel, semitone(ktranspose), gitableR, gklock, giFFTSizes[i(gkFFTSize)-1]
  	outs	a1*aenv,a2*aenv
 endif
endif
endin

instr	3	; midi triggered instrument
 if giReady = 1 then						; i.e. if a file has been loaded
  icps	cpsmidi							; read in midi note data as cycles per second
  iamp	ampmidi	1						; read in midi velocity (as a value within the range 0 - 1)
  iMidiRef	chnget	"MidiRef"				; MIDI unison reference note
  iFrqRatio		=	icps/cpsmidinn(iMidiRef)	; derive playback speed from note played in relation to a reference note (MIDI note 60 / middle C)
 
  iAttTim	chnget	"AttTim"		; read in amplitude envelope attack time widget
  iRelTim	chnget	"RelTim"		; read in amplitude envelope attack time widget
  if iAttTim>0 then				; 
   kenv	linsegr	0,iAttTim,1,iRelTim,0
  else								
   kenv	linsegr	1,iRelTim,0			; attack time is zero so ignore this segment of the envelope (a segment of duration zero is not permitted
  endif
  kenv	expcurve	kenv,8			; remap amplitude value with a more natural curve
  aenv	interp		kenv			; interpolate and create a-rate envelope
  
  ktrig	changed		gkFFTSize
  if ktrig==1 then
   reinit RESTART
  endif
  RESTART:
  iFileLen	=	ftlen(gitableL)/sr
  if gichans=1 then
   a1	mincer		gapointer*iFileLen, gklevel, iFrqRatio, gitableL, gklock, giFFTSizes[i(gkFFTSize)-1]
  	outs	a1*aenv,a1*aenv
  elseif gichans=2 then
   a1	mincer		gapointer*iFileLen, gklevel, iFrqRatio, gitableL, gklock, giFFTSizes[i(gkFFTSize)-1]
   a2	mincer		gapointer*iFileLen, gklevel, iFrqRatio, gitableR, gklock, giFFTSizes[i(gkFFTSize)-1]
  	outs	a1*aenv,a2*aenv
  endif
 endif
endin

</CsInstruments>  

<CsScore>
i 1 0 10000
</CsScore>

</CsoundSynthesizer>
