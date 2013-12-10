<Cabbage>
form caption("gbuzz Synth") size(875, 285), pluginID("wtsy")
image   bounds( 10,  6,365, 90), colour("DarkSlateGrey"), outline("white"), line(2), shape("sharp")	;main
image   bounds(380,  6,170, 90), colour("DarkSlateGrey"), outline("white"), line(2), shape("sharp")	;polyphony
image   bounds(555,  6,120, 90), colour("DarkSlateGrey"), outline("white"), line(2), shape("sharp")	;reverb
image   bounds(580,101,125, 90), colour("DarkSlateGrey"), outline("white"), line(2), shape("sharp")	;
image   bounds(680,  6,185, 90), colour("DarkSlateGrey"), outline("white"), line(2), shape("sharp")	;pitch bend
image   bounds( 10,101,305, 90), colour("DarkSlateGrey"), outline("white"), line(2), shape("sharp")	;multplier envelope
image   bounds(320,101,125, 90), colour("DarkSlateGrey"), outline("white"), line(2), shape("sharp")	;Low Cut
image   bounds(450,101,125, 90), colour("DarkSlateGrey"), outline("white"), line(2), shape("sharp")	;High Cut
image pos(0, 0), size(875, 285), colour("DarkSlateGrey"), outline("MediumSlateBlue"), line(3)

label   bounds(190, 11,100, 15), text("Main"), fontcolour(white)
label   bounds(415, 11,100, 15), text("Polyphony"), fontcolour(white)
label   bounds(590, 11,100, 15), text("Reverb"), fontcolour(white)
label   bounds( 93,106,160, 15), text("Multiplier Envelope"), fontcolour(white)
label   bounds(616,106,100, 15), text("Control"), fontcolour(white)
label   bounds(732, 11,100, 15), text("Pitch Bend"), fontcolour(white)

;MAIN
rslider  bounds( 10, 29, 62, 62), text("Level"),  colour(SlateGrey) channel("level"),range(0,20, 1,0.5,0.001), fontcolour(white)
rslider  bounds( 70, 29, 62, 62), text("Power"),  colour(SlateGrey) channel("mul"), range(0, 0.97, 0.6), fontcolour(white)
rslider  bounds(130, 29, 62, 62), text("Lowest"), colour(SlateGrey)  channel("lh"), range(1, 40, 1,1,1), fontcolour(white)
rslider  bounds(190, 29, 62, 62), text("Number"), colour(SlateGrey)  channel("nh"), range(1,200,80,1,1), fontcolour(white)
rslider  bounds(250, 29, 62, 62), text("Jitter"), colour(SlateGrey) channel("jitter"),range(0, 1, 0.4), fontcolour(white)
rslider  bounds(310, 29, 62, 62), text("Pan"),    colour(SlateGrey) channel("pan"),range(0, 1, 0.5), fontcolour(white)


;POLYPHONY
button   bounds(390, 30, 70, 25), text("mono", "poly"), channel("monopoly"), value(1), fontcolour("lime") 
rslider  bounds(490, 32, 60, 60), text("Leg.Time"), channel("LegTim"), range(0.01, 15, 0.05, 0.25, 0.00001), fontcolour(white) colour(SlateGrey)
label    bounds(417, 60, 90, 11), text("Mode"), fontcolour(white)
combobox bounds(390, 71, 90, 18), channel("PortMode"), value(1), text("Fixed", "Proportional")

;REVERB
rslider bounds(555, 29, 60, 60), text("Mix"), channel("RvbMix"), range(0, 1, 0.3), fontcolour(white) colour(SlateGrey)
rslider bounds(615, 29, 60, 60), text("Size"), channel("RvbSize"), range(0.3, 1, 0.7), fontcolour(white) colour(SlateGrey)

;PITCH BEND
rslider  bounds(680, 29, 60, 60), fontcolour("white"), text("< Bend"),    channel("BendDown"),    range(-1,0, 0, 1,0.001), colour(SlateGrey)
rslider  bounds(740, 29, 60, 60), fontcolour("white"), text("Bend >"),    channel("BendUp"),      range(0, 1, 0, 1,0.001), colour(SlateGrey)
rslider  bounds(800, 29, 60, 60), fontcolour("white"), text("Bend Rng."), channel("BendRange"),   range(1, 24, 12, 1,1),   colour(SlateGrey)

;MULTIPLIER ENVELOPE
rslider  bounds( 10,124, 62, 62), text("Att"),  colour(SlateGrey) channel("MAtt"),range(0, 5.000, 0.01, 0.375,0.0001), fontcolour(white)
rslider  bounds( 70,124, 62, 62), text("Lev"),  colour(SlateGrey) channel("MLev"),range(0, 1.000, 0), fontcolour(white)
rslider  bounds(130,124, 62, 62), text("Dec"),  colour(SlateGrey) channel("MDec"),range(0, 5.000, 0.1,  0.375,0.0001), fontcolour(white)
rslider  bounds(190,124, 62, 62), text("Sus"),  colour(SlateGrey) channel("MSus"),range(0, 1.000, 0), fontcolour(white)
rslider  bounds(250,124, 62, 62), text("Rel"),  colour(SlateGrey) channel("MRel"),range(0, 5.000, 0.1,  0.375,0.0001), fontcolour(white)

;LOW CUT
checkbox bounds(350,107, 70, 12), text("Low Cut") channel("LowCutOnOff"), FontColour(white), colour("lime")
rslider  bounds(320,124, 62, 62), text("Low Cut"),  colour(SlateGrey) channel("LowCut"),range(0, 30.00, 0,1,0.0011), fontcolour(white)
rslider  bounds(380,124, 62, 62), text("Lo Poles"),  colour(SlateGrey) channel("LowPoles"),range(2, 30, 2,1,1), fontcolour(white)
;HIGH CUT
checkbox bounds(478,107, 70, 12), text("High Cut") channel("HighCutOnOff"), FontColour(white), colour("lime")
rslider  bounds(452,124, 62, 62), text("High Cut"),  colour(SlateGrey) channel("HighCut"),range(1, 100.00, 100,1,0.001), fontcolour(white)
rslider  bounds(512,124, 62, 62), text("Hi Poles"),  colour(SlateGrey) channel("HighPoles"),range(2, 30, 2,1,1), fontcolour(white)

rslider  bounds(582,124, 62, 62), text("Mod."),  colour(SlateGrey) channel("mod"),range(0, 1.00, 0, 1, 0.0001), fontcolour(white)
rslider  bounds(642,124, 62, 62), text("Noise"),  colour(SlateGrey) channel("NoiseAmp"),range(0, 5.00, 0, 1, 0.0001), fontcolour(white)

keyboard pos(10, 195), size(855, 80)

</Cabbage>

<CsoundSynthesizer>

<CsOptions>
-dm0 -n -+rtmidi=null -M0
</CsOptions>

<CsInstruments>

sr 		= 	44100
ksmps 		= 	16
nchnls 		= 	2
0dbfs		=	1	;MAXIMUM AMPLITUDE
seed	0
massign	0,2

;Author: Iain McCurdy (2012)

gisine	ftgen	0,0,131072,10,1			;A SINE WAVE. USED BY THE LFOs.
gicos	ftgen	0,0,131072,9,1,1,90		;FUNCTION TABLE THAT STORES A SINGLE CYCLE OF A COSINE WAVE

gasendL,gasendR	init	0

;FUNCTION TABLE USED TO RE-MAP THE RELATIONSHIP BETWEEN VELOCITY AND ATTACK TIME 
giattscl	ftgen	0,0,128,-16,2,128,-10,0.005
giNAttScl	ftgen	0,0,128,-16,8,128,-4,0.25


opcode	FreqShifter,a,aki
	ain,kfshift,ifn	xin					;READ IN INPUT ARGUMENTS
	areal, aimag hilbert ain				;HILBERT OPCODE OUTPUTS TWO PHASE SHIFTED SIGNALS, EACH 90 OUT OF PHASE WITH EACH OTHER
	asin 	oscili       1,    kfshift,     ifn,          0
	acos 	oscili       1,    kfshift,     ifn,          0.25	
	;RING MODULATE EACH SIGNAL USING THE QUADRATURE OSCILLATORS AS MODULATORS
	amod1	=		areal * acos
	amod2	=		aimag * asin	
	;UPSHIFTING OUTPUT
	aFS	= (amod1 - amod2)
		xout	aFS				;SEND AUDIO BACK TO CALLER INSTRUMENT
endop

opcode	SsplinePort,k,KkkO												;DEFINE OPCODE
	knum,kporttime,kcurve,iopt	xin										;READ IN INPUT ARGUMENTS
	kout	init	i(knum)												;INITIALISE TO OUTPUT VALUE (PORTAMENTO APPLIED VALUE)
	ktrig	changed	knum												;...GENERATE A TRIGGER IS A NEW NOTE NUMBER IS GENERATED (FROM INSTR. 1)
	 if ktrig=1 then												;IF A NEW (LEGATO) NOTE HAS BEEN PRESSED 
	  reinit	S_CURVE												;BEGIN A REINITIALISATION PASS FROM LABEL
	 endif														;END OF CONDITIONAL BRANCH
	 S_CURVE:													;A LABEL. REINITIALISATION BEGINS FROM HERE.
	if iopt!=0 then													;IF ABSOLUTE/PROPORTIONAL SWITCH IS ON... (I.E. PROPORTIONAL)
	 idiff	=	1+abs(i(knum)-i(kout))									;ABSOLUTE DIFFERENCE BETWEEN OLD NOTE AND NEW NOTE IN STEPS (+ 1)
	 kporttime	=	kporttime*idiff										;SCALE PORTAMENTO TIME ACCORDING TO THE NOTE GAP
	endif														;END OF CONDITIONAL BRANCH
	imid	=	i(kout)+((i(knum)-i(kout))/2)								;SPLINE MID POINT VALUE
	isspline	ftgentmp	0,0,4096,-16,i(kout),4096*0.5,i(kcurve),imid,(4096/2)-1,-i(kcurve),i(knum)	;GENERATE 'S' SPLINE
	kspd	=	i(kporttime)/kporttime										;POINTER SPEED AS A RATIO (WITH REFERENCE TO THE ORIGINAL DURATION)
	kptr	init	0												;POINTER INITIAL VALUE	
	kout	tablei	kptr,isspline											;READ VALUE FROM TABLE
	kptr	limit	kptr+((ftlen(isspline)/(i(kporttime)*kr))*kspd), 0, ftlen(isspline)-1				;INCREMENT THE POINTER BY THE REQUIRED NUMBER OF TABLE POINTS IN ONE CONTROL CYCLE AND LIMIT IT BETWEEN FIRST AND LAST TABLE POINT - FINAL VALUE WILL BE HELD IF POINTER ATTEMPTS TO EXCEED TABLE DURATION
	rireturn													;RETURN FROM REINITIALISATION PASS
	 		xout	kout                                                                                    ;SEND PORTAMENTOED VALUES BACK TO CALLER INSTRUMENT
endop

instr	1
	kporttime	linseg	0,0.001,0.05

	gkmul		chnget	"mul"
	gklh		chnget	"lh"
	gknh		chnget	"nh"
	gkjitter	chnget		"jitter"
	
	gkmonopoly	chnget	"monopoly"
	gkLegTim	chnget	"LegTim"
	gkPortMode	chnget	"PortMode"
	
	gkpan		chnget	"pan"
	gklevel		chnget	"level"
	gkRvbMix	chnget	"RvbMix"
	gkRvbSize	chnget	"RvbSize"

	gkMAtt	chnget	"MAtt"
	gkMLev	chnget	"MLev"
	gkMDec	chnget	"MDec"
	gkMSus	chnget	"MSus"
	gkMRel	chnget	"MRel"

	gkLowCutOnOff	chnget	"LowCutOnOff"
	gkLowCut	chnget	"LowCut"
	gkLowPoles	chnget	"LowPoles"
	gkHighCutOnOff	chnget	"HighCutOnOff"
	gkHighCut	chnget	"HighCut"
	gkHighPoles	chnget	"HighPoles"
	
	gkmod		chnget	"mod"
	gkVibDep	=	0.2
	gkTremDep	=	0.3
	gkToneDep	=	1
	gkNoiseAmp	chnget	"NoiseAmp"
	
	gkBendDown	chnget	"BendDown"
	gkBendUp	chnget	"BendUp"
	gkBendRange	chnget	"BendRange"
	gkPchBend	portk	(gkBendDown+gkBendUp)*gkBendRange, kporttime
endin

instr	2	;triggered via MIDI
	gkNoteTrig	init	1	;at the beginning of a new note set note trigger flag to '1'
	inum		notnum		;read in midi note number
	givel		veloc	0,1	;read in midi note velocity
	gknum	=	inum		;update a global krate variable for note pitch

	if i(gkmonopoly)==0 then		;if we are *not* in legato mode...
	 inum	notnum						; read midi note number (0 - 127)
	 	event_i	"i",p1+1+(inum*0.001),0,-1,inum		; call sound producing instr
	 krel	release						; release flag (1 when note is released, 0 otherwise)
	 if krel==1 then					; when note is released...
	  turnoff2	p1+1+(inum*0.001),4,1			; turn off the called instrument
	 endif							; end of conditional
	else				;otherwise... (i.e. legato mode)
	 iactive	active p1+1	;check to see if there is already a note active...
	 if iactive==0 then		;...if not...
	  event_i	"i",p1+1,0,-1	;...start a new held note
	 endif
	endif
endin

instr	3	;gbuzz instrument. MIDI notes are directed here.
	kporttime	linseg	0,0.001,1		;portamento time function rises quickly from zero to a held value
	kglisstime	=	kporttime*gkLegTim	;scale portamento time function with value from GUI knob widget

	/* MODULATION */
	krate	randomi	3,4,1,1
	if gkNoteTrig==1 then
	 reinit RESTART_MOD_ENV
	endif
	RESTART_MOD_ENV:
	kModRise	linseg	0,1.5,1
	kmod	lfo	gkmod*kModRise,krate,0
	rireturn
	
	if i(gkmonopoly)==1 then			;if we are in legato mode...

	 knum	SsplinePort	gknum,kglisstime,1,i(gkPortMode)-1

	 kactive	active	p1-1			;...check number of active midi notes (previous instrument)
	 if kactive==0 then				;if no midi notes are active...
	  turnoff					;... turn this instrument off
	 endif
	else						;otherwise... (polyphonic / non-legato mode)
	 knum	=	p4		 		;pitch equal to the original note pitch
	endif
	ivel	init	givel
	
	kporttime	linseg		0,0.001,0.02		;CREATE A FUNCTION  THAT RISES RAPIDLY FROM ZERO TO A FIXED VALUE THAT WILL BE USED FOR PORTAMENTO TIME 
	
	;------------------------------------------------------------------------------------------------------------
	;PITCH JITTER (THIS WILL BE USED TO ADD HUMAN-PLAYER REALISM)
	;------------------------------------------------------------------------------------------------------------
	;				AMP | MIN_FREQ. | MAX_FREQ
	kPitchJit	jitter		0.05*gkjitter*4,     1,         20

	;------------------------------------------------------------------------------------------------------------
	;AMPLITUDE JITTER (THIS WILL BE USED TO ADD HUMAN-PLAYER REALISM)
	;------------------------------------------------------------------------------------------------------------
	;				AMP | MIN_FREQ. | MAX_FREQ
	kAmpJit		jitter		0.1*gkjitter*4,     0.2,        1
	kAmpJit		=		kAmpJit+1			;OFFSET SO IT MODULATES ABOUT '1' INSTEAD OF ABOUT ZERO
	
	knum		=		knum+kPitchJit			;DERIVE K-RATE NOTE NUMBER VALUE INCORPORATING PITCH BEND, VIBRATO, AND PITCH JITTER	

	/* OSCILLATOR */
	kmul		portk		gkmul, kporttime*0.5
	kMulEnv		linsegr		0, i(gkMAtt)+0.0001, i(gkMLev), i(gkMDec)+0.0001, i(gkMSus), i(gkMRel)+0.0001, 0
	kmul		=		kmul+kMulEnv+(kmod*0.3)
	kmul		limit		kmul,0,0.9

	knum	=	knum + gkPchBend + (kmod*gkVibDep)
	
	asig		gbuzz		(kAmpJit*0.1)*(1+(kmod*gkTremDep*0.9)), cpsmidinn(knum), gknh, gklh, kmul, gicos

	/* FREQUENCY SHIFTER */
	;aFS		FreqShifter	asig,cpsmidinn(knum)*8,gisine
	;kscale		expcurve	kmul,1000
	;asig		sum		asig,aFS*kscale*5
	
	/* NOISE */
	kNoiseAmp	expcurve	kmul,40
	kNoiseAmp	scale		kNoiseAmp,2,0.1
	anoise		gauss		kNoiseAmp*gkNoiseAmp
	anoise		butlp		anoise,1000
	asig		=		asig * (1+anoise)
	
	/* LOW CUT / HIGH CUT FILTERS */
	;FILTER
	if gkLowCutOnOff=1 then
	 kLowCut	portk	gkLowCut,kporttime*5
	 kLowCut	limit	cpsmidinn(knum)*kLowCut,20,sr/2
	 ktrig		changed	gkLowPoles
	 if ktrig=1 then
	  reinit	RESTART_LOWCUT
	 endif
	 RESTART_LOWCUT:
	 asig		clfilt	asig,kLowCut,1,i(gkLowPoles)
	endif
	if gkHighCutOnOff=1 then
	 kHighCut	portk	gkHighCut,kporttime*5
	 kHighCut	limit	cpsmidinn(knum)*gkHighCut,20,sr/2
	 ktrig		changed	gkHighPoles
	 if ktrig=1 then
	  reinit	RESTART_HIGHCUT
	 endif
	 RESTART_HIGHCUT:
	 asig		clfilt	asig,kHighCut,0,i(gkHighPoles)
	 rireturn
	endif

	;asig	pinkish	0.2

	iatt		=		0.05
	aenv		linsegr		0,iatt,1,0.05,0			;AMPLITUDE ENVELOPE
	asig		=		asig * aenv
	aL,aR		pan2		asig*gklevel,gkpan		;scale amplitude level and create stereo panned signal
			outs		aL*(1-gkRvbSize), aR*(1-gkRvbSize)		;SEND AUDIO TO THE OUTPUTS
	gasendL		=		gasendL+aL*gkRvbMix
	gasendR		=		gasendR+aR*gkRvbMix
	gkNoteTrig	=	0					;reset new-note trigger (in case it was '1')
endin

instr	5	;reverb
	if gkRvbMix==0 kgoto SKIP_REVERB
	aL,aR	reverbsc	gasendL,gasendR,gkRvbSize,12000
		outs		aL,aR
		clear		gasendL,gasendR
	SKIP_REVERB:
endin

</CsInstruments>

<CsScore>
;i "UpdateTableNumbers" 0 3600
i 1 0 3600			;reverb
i 5 0 3600			;reverb
</CsScore>

</CsoundSynthesizer>