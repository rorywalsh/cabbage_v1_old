; Parp.csd
; Iain McCurdy, 2015

; Based on Perry Cook's waveguide model of someone struggling to play a trombone

; Please note that this is a rather temperamental opcode so some patience is needed if the instrument fails to sound.
; The values for 'Breath' (power) and 'Lip Tension' are most important in getting the instrument to sound.
; The actual values needed for these two parameters will also depend upon the note played.
; Lower notes tend to require a lower breath power and/or higher lip tension (TENSION:Offset).

; Sometimes it is necessary just to restart a note in order to get it to sound.

; In order to create a modulation of pitch and tone, tension can be randomly modulated using the controls 'Mod.Dep.' and 'Mod.Rate' 



<Cabbage>
#define	SLIDER_STYLE # trackercolour( 50, 50,155), fontcolour("black"), textcolour("black") #
#define	CHECKBOX_STYLE # colour(255,255,100) #

form caption("Parp") size(830,221), pluginID("Parp"), colour(170,170,170)

image      bounds(  2,  2,826,130), colour(220,220,220), outlinecolour("black"), outlinethickness(1)
rslider    bounds(  0, 25, 90, 90), textbox(1), text("Breath"),     channel("amp"),      range(0.001,20,1,0.5),        $SLIDER_STYLE
line       bounds( 85,  5,  2,126)
image      bounds( 80, 10,230,105), colour(0,0,0,0), plant("Tension") { 
label      bounds(  0,  0,230, 12), text("T E N S I O N"), fontcolour( 70, 70, 70)
rslider    bounds(  0, 15, 90, 90), textbox(1), text("Offset"),  channel("tens"),     range(0,5,0.75),                $SLIDER_STYLE
rslider    bounds( 70, 15, 90, 90), textbox(1), text("Mod.Dep"),  channel("TRndDep"),  range(0,0.8,0.1,0.5,0.001),              $SLIDER_STYLE
rslider    bounds(140, 15, 90, 90), textbox(1), text("Mod.Rate"),  channel("TRndRte"),  range(0.01,100,8,0.5,0.01),      $SLIDER_STYLE
}
line       bounds(305,  5,  2,126)

button     bounds(320, 60, 50, 20), text("Mono","Poly"), channel("MonoPoly"), value(1)
rslider    bounds(365, 25, 90, 90), textbox(1), text("Port.Time"), channel("PortTime"),  range(0, 5, 0.9), identchannel("PortTimeID"), visible(1), $SLIDER_STYLE

rslider    bounds(435, 25, 90, 90), textbox(1), text("Spread"),   channel("PanDep"),   range(0,1,1),                 $SLIDER_STYLE

image      bounds(515,  0,227,131), colour(0,0,0,0), plant("Reverb") {
label      bounds(  0, 10,227, 12), text("R E V E R B"), fontcolour( 70, 70, 70) 
line       bounds(  5,  5,  2,126)
rslider    bounds(  0, 25, 90, 90), textbox(1), text("Mix"),      channel("RvbMix"),   range(0,1,0.5),               $SLIDER_STYLE
rslider    bounds( 70, 25, 90, 90), textbox(1), text("Size"),     channel("RvbSize"),  range(0,1,0.005,0.5,0.001),   $SLIDER_STYLE
rslider    bounds(140, 25, 90, 90), textbox(1), text("Damp"),     channel("RvbDamp"),  range(0,1,1),                 $SLIDER_STYLE
line       bounds(225,  5,  2,126)
}

rslider    bounds(735, 25, 90, 90), textbox(1), text("Level"),    channel("outamp"),   range(0.001,100,3,0.5,0.001), $SLIDER_STYLE

keyboard   bounds(  2,134,826, 85)

</Cabbage>


<CsoundSynthesizer>

<CsoundSynthesizer>

<CsOptions>
-dm0 -n -+rtmidi=null -M0
</CsOptions>

<CsInstruments>

sr = 44100
ksmps = 16
nchnls = 2
0dbfs = 1

massign	0,2

opcode	lineto2,k,kk
 kinput,ktime	xin
 ktrig	changed	kinput,ktime				; reset trigger
 if ktrig==1 then					; if new note has been received or if portamento time has been changed...
  reinit RESTART
 endif
 RESTART:						; restart 'linseg' envelope
 if i(ktime)==0 then					; 'linseg' fails if duration is zero...
  koutput	=	i(kinput)			; ...in which case output simply equals input
 else
  koutput	linseg	i(koutput),i(ktime),i(kinput)	; linseg envelope from old value to new value
 endif
 rireturn
 		xout	koutput
endop

; FUNCTION TABLES MAP NOTE NUMBERS TO BREATH POWER (AMPSCL) AND OUTPUT LEVEL IN AN ATTEMPT TO PROVIDE A MORE PLAYABLE INSTRUMENT
giAmpScl	ftgen	0,0,128,-25, 0,0.001, 30,0.01, 36,0.04, 48,0.1, 60,0.2, 72,0.4, 84,1.5, 96,4, 108,10, 128,20
giLevScl	ftgen	0,0,128,-25, 0,10, 30,6, 36,4, 48,1.6, 60,1, 72,0.6, 84,0.15, 96,0.05, 108,0.02, 128,0.01

; EACH NOTE PLAYED MAPS TO A DIFFERENT (RANDOM BUT REPEATABLE) PANNING LOCATION
giPanDist	ftgen	0,0,128,21,1,1
gasendL,gasendR	init	0			; REVERB SEND VARIABLE

instr	1
 gkamp    	chnget	"amp"			; READ IN WIDGETS
 gknum    	chnget	"num"	
 gktens   	chnget	"tens" 	
 gkoutamp 	chnget	"outamp" 	
 gkTRndDep	chnget	"TRndDep"	
 gkTRndRte	chnget	"TRndRte"	
 gkPanDep	chnget	"PanDep"
 gkRvbMix	chnget	"RvbMix"
 gkRvbSize	chnget	"RvbSize"
 gkRvbDamp	chnget	"RvbDamp"
 gkMonoPoly	chnget	"MonoPoly"
 gkPortTime	chnget	"PortTime"
 
endin

	givfn	ftgenonce	0, 0, 256, 10, 1	; VIBRATO WAVEFORM (A SINE WAVE) - (VIBRATO IS NOT USED)


instr	2					; RESPONDS TO MIDI NOTES PLAYED
 inum	notnum					; READ IN NOTE NUMBER
 gknum	=	inum				; ASSIGN TO K-RATE GLOBAL VARIABLE FOR NOTE NUMBER (NEEDED IN MONOPHONIC LEGATO MODE)
 iactive	active	3			; SENSE NUMBER OF ACTIVE NOTES IN SOUNDING INSTRUMENT
 if i(gkMonoPoly)==0&&iactive==0 then		; IF MONOPHNIC/LEGATO MODE AND THIS IS THE FIRST NOTE PLAYED...
  event_i	"i",3,0,-1			; START SOUNDING INSTRUMENT WITH A HELD NOTE
 elseif i(gkMonoPoly)==1 then			; OTHERWISE IF POLYPHONIC MODE
  aL,aR	subinstr	3, inum			; CALL SOUNDING INSTRUMENT AS A SUBINSTRUMENT
 endif						; END OF CONDITIONAL BRANCHING
endin

instr	3					; SOUNDING INSTRUMENT
 if active:k(2)==0 then				; IF ALL MIDI NOTES (INSTR 2 RESPONDS TO MIDI) ARE RELEASED...
  turnoff					; TURN THIS INSTRUMENT OFF (ALLOW RELEASE SEGMENTS TO COMPLETE)
 endif						; END OF CONDITIONAL BRANCHING
 kPortTime	linseg	0,0.001,1		; PORTAMENTO FUNCTION RAMPS UP RAPIDLY FROM ZERO TO A HELD VALUE OF '1'

 if release:k()==1 kgoto SKIP			; IF NOTE IS IN RELEASE STAGE DO NOT READ IN ANY NEW NOTE VALUES
 if i(gkMonoPoly)==0 then			; IF MONOPHONIC/LEGATO MODE...
  knum		lineto2	gknum,kPortTime*gkPortTime	; ...GLIDE TO NEW NOTE VALUES (NOTE NUMBER IS DEFINED BY NEWEST MIDI NOTE PLAYED)
 elseif i(gkMonoPoly)==1 then			; OTHERWISE (POLYPHONIC MODE)
  knum		init	p4			; NOTE IS JUST DEFINED BY p4 VALUE SENT BY THE SUBINSTR OPCODE IN INSTR 2
 endif						; END OF CONDITIONAL BRANCHING
 SKIP:						; JUMP TO HERE IF IN RELEASE STAGE
 
 ktensrnd	rspline	-gkTRndDep, gkTRndDep, gkTRndRte, gkTRndRte*3	; RANDOM SPLINE FUNCTION USED TO MODULATE LIP TENSION PARAMETER
 
 kAmpScl	tablei	knum,giAmpScl		; READ AMP (BREATH POWER) SCALING VALUE ACCORDING NOTE NUMBER
 asig		wgbrass gkamp*kAmpScl, cpsmidinn(knum), gktens+ktensrnd, 0.01, 0, 0, givfn, 1	; GENERATE A WGBRASS TONE
 kLevScl	tablei	knum,giLevScl		; READ OUTPUT LEVEL SCALING VALUE ACCORDING NOTE NUMBER
 asig		*=	kLevScl			; APPLY LEVEL SCALING VALUE
 
 kPan		tablei	knum,giPanDist		; READ IN PAN POSITION FOR THE NOTE PLAYED FROM RANDOM DISTRIBUTION TABLE
 aL,aR		pan2	asig,(kPan*gkPanDep) + (0.5*(1-gkPanDep)) 	; CREATE STEREO (PANNED) SIGNALS
 
 gkwidth	=		1				; SHORT DELAY STEREO WIDENING AMOUNT
 kRndDelL	rspline		0, 0.04*gkwidth, 0.1, 0.2	; SPLINIC RANDOM DELAY TIME (LEFT CHANNEL)
 kRndDelR	rspline		0, 0.04*gkwidth, 0.1, 0.2	; SPLINIC RANDOM DELAY TIME (RIGHT CHANNEL)
 kRndDelL	limit		kRndDelL, 0, 0.04		; CONSTRAIN POSSIBLE OUT OF RANGE VALUE
 kRndDelR	limit		kRndDelR, 0, 0.04
 aRndDelL	interp		kRndDelL			; CONVERT TO A-RATE WITH LINER INTERPOLATION
 aRndDelR	interp		kRndDelR
 aL		vdelay		aL, 0.1+(aRndDelL*1000), 0.1 + 40	; CREATE SHORT DELAY VERSIONS OF BOTH CHANNELS WITH DIFFERENT DELAY TIMES
 aR		vdelay		aR, 0.1+(aRndDelR*1000), 0.1 + 40
 
 kenv		linsegr	1,0.1,0					; ANTI-CLICK ENVELOPE
 aL		*=	kenv					; APPLY ENVELOPE
 aR		*=	kenv
 gasendL	+=	aL					; MIX IN BOTH CHANNELS TO SEND SIGNAL FOR INSTR 99 (REVERB AND OUTPUT)
 gasendR	+=	aR
endin

instr	99							; REVERB AND OUTPUT
	aL,aR		freeverb	gasendL,gasendR,gkRvbSize,gkRvbDamp	; FREEVERBED SIGNALS
	aL		ntrpol		gasendL/2,aL,gkRvbMix	; MIX DRY/WET
	aR		ntrpol		gasendR/2,aR,gkRvbMix
			outs		aL*gkoutamp, aR*gkoutamp	; SEND DRY/WET MIX TO OUTPUTS
			clear		gasendL,gasendR			; CLEAR AUDIO SEND SIGNALS
endin

</CsInstruments>  

<CsScore>
i 1  0 [3600*24*7]	; READ IN WIDGETS
i 99 0 [3600*24*7]	; REVERB AND OUTPUT
</CsScore>

</CsoundSynthesizer>