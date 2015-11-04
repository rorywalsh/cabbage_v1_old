; HarpFilter.csd

; Creates a stack of waveguides simulating a resonating bank of strings

; Tunings of strings are not controlled individually but are instead defined using several global controls.

; Frequency		-	Base frequency of the stack of waveguide filters
; Cutoff		-	Cutoff frequency of a 1st order lowpass filter within the feedback loop of each waveguide unit
; Spacing		-	The spacing method used between adjacent waveguide filters: Geometric or Arithmetic
; Interval		-	Interval factor between adjacent filters.
;				 If 'Spacing' is geometric then Interval is applied geometrically, each time multiplying it to the previous frequency to derive the next.
;				 In this mode the value Interval actually defines an interval in semitones so an interval of 12 will produce a ratio of 2
;				 e.g. if base frequency is 200 and interval is 12, the sequence is 200,400,800,1600 
;				 If 'Spacing' is 'Arithmetic' then this is applied arithmetically each time adding base_frequency to the frequency of the previous filter to derive the frequency of the next.
;				 e.g. if base frequency is 200, interval is 1, the sequence is 200,400,600,800 etc... i.e. harmonic
; Number		-	The number of waveguides to be created
; Lowest		-	The Lowest filter in the sequence. i.e. shift the stacks up in steps as this is increased.
; Reflect		-	If activated, additional waveguide filters are created at frequencies reflected beneath the base frequency according to the geometric or arithmetric rules. Activating 'Reflect' will double the number of filters used.
; Double Strength	-	activates a second, in series, interation of the entire waveguide stack. This, in essence, increases the strength of the waveguide filtering.
; Width			-	offsets the frequencies of the left and right channels to imbue added stereo width
; Random		-	range of random offsets added to waveguide frequencies
; Lowcut		-	Inserts a 24dB/oct highpass filter after each waveguide, the cutoff of which corresponds to the cutoff of that filter.
; Port.			-	Portamento time applied to changes made to frequency for each waveguide (and by extension also changes made to 'Interval'). Large portamento times are possible thereby permitting slow morphs. 
; Feedback		-	feedback ratio of each waveguide unit.
;				 negative feedback will shift the fundemental down one octave and only odd harmonics will be preset
; Attack		-	Attack time of each new note played 
; Decay			-	Decay time of the input sound once a note is released
; Release		-	Release time of the outputs of the waveguides after a note is released
; Level			-	Output amplitude control

<Cabbage>
form caption("Harp Filter") size(1010,205), pluginID("HaFi")
image             bounds(  0,  0,1010,125), colour("silver"), shape("sharp")

checkbox  bounds(  7,  5, 70, 12), text("Keyboard"), channel("input"), fontcolour("black")
combobox  bounds(  7, 20, 65, 16), text("Live","Dust","Noise"), channel("InSigMode"), value(1)  
label     bounds( 10, 42, 66, 13), text("Frequency"), fontcolour("black")
rslider   bounds( 22, 58, 42, 42),  channel("freq"),      fontcolour("black"), textcolour("black"), trackercolour("DarkSlateGrey"), range(1,5000,150,0.5,0.01)
numberbox bounds( 13,100, 60, 17),  channel("freq"),     colour("white"), fontcolour("black"), textcolour("black"), trackercolour("DarkSlateGrey"), range(1,5000,150,0.5,0.01)

image    bounds( 85,  5, 75,115), outlinethickness(1), line(1), outlinecolour("darkslategrey"), shape("sharp")
label    bounds( 90,  8, 65, 11), text("Cutoff"), fontcolour("black")
combobox bounds( 90, 20, 65, 16), text("Fixed","Ratio"), channel("CutoffMode"), value(1)  
rslider  bounds( 85, 40, 75, 75), text("Hertz"),      channel("cutoff"),      textbox(1), fontcolour("black"), textcolour("black"), trackercolour("DarkSlateGrey"), range(20,20000,8000,0.5,1), identchannel("cutoff_ident")
rslider  bounds( 85, 40, 75, 75), text("Ratio"),      channel("CutoffRatio"), textbox(1), fontcolour("black"), textcolour("black"), trackercolour("DarkSlateGrey"), range(1,60,21,1,1), identchannel("CutoffRatio_ident")

image    bounds(165,  5,405,115), outlinethickness(1), line(1), outlinecolour("darkslategrey"), shape("sharp")
label    bounds(180,  8, 80, 11), text("Spacing"), fontcolour("black")
combobox bounds(180, 20, 80, 16), text("Geometric","Arithmetic"), channel("type"), value(2)  
checkbox  bounds(280, 20, 55, 12), text("Reflect"),      channel("dual"),  fontcolour("black")
checkbox  bounds(350, 20,120, 12), text("Double Strength"),      channel("DoubleStrength"),  fontcolour("black")
rslider   bounds(180, 40, 75, 75), text("Interval"),  channel("interval"),   textbox(1), fontcolour("black"), textcolour("black"), trackercolour("DarkSlateGrey"), range(-12,12,0.25)
numberbox bounds(190,100, 55, 16),                    channel("interval"),   range(-24,24,0.25,1,0.0001), colour("white"), fontcolour("black")
rslider  bounds(250, 40, 75, 75), text("Number"),     channel("max"),         textbox(1), fontcolour("black"), textcolour("black"), trackercolour("DarkSlateGrey"), range(1,100,11,1,1)
rslider  bounds(310, 40, 75, 75), text("Lowest"),     channel("min"),      textbox(1), fontcolour("black"), textcolour("black"), trackercolour("DarkSlateGrey"), range(1,100,1,1,1)
rslider  bounds(370, 41, 75, 75), text("Width"),      channel("StWidth"),   range(-0.5, 0.5, 0, 1,0.001), textbox(1), fontcolour("black"), textcolour("black"), trackercolour("DarkSlateGrey")
rslider  bounds(430, 41, 75, 75), text("Random"),     channel("RndFactor"),   range(0, 5, 0, 0.5,0.001), textbox(1), fontcolour("black"), textcolour("black"), trackercolour("DarkSlateGrey")
rslider  bounds(490, 41, 75, 75), text("Tune"),       channel("Tune"),       range(-100,100, 0, 1,1), textbox(1), fontcolour("black"), textcolour("black"), trackercolour("DarkSlateGrey")

checkbox bounds(590, 20, 70, 12), text("Low Cut"),     channel("LowCut"),  fontcolour("black")
rslider  bounds(570, 40, 75, 75), text("Port."),   channel("Portamento"), textbox(1), fontcolour("black"), textcolour("black"), trackercolour("DarkSlateGrey"), range(0,99,0,0.5,0.01)
numberbox bounds(640, 55, 55, 35), text("Feedback"),  channel("feedback"), range(-0.99999,0.99999,0.99,1,0.00001), colour("white"), fontcolour("black"), textcolour("black")
rslider  bounds(690, 40, 75, 75), text("Attack"), channel("Att"),         textbox(1), fontcolour("black"), textcolour("black"), trackercolour("DarkSlateGrey"), range(0.05,10,0.05,0.5)
rslider  bounds(750, 40, 75, 75), text("Decay"),  channel("Dec"),         textbox(1), fontcolour("black"), textcolour("black"), trackercolour("DarkSlateGrey"), range(0.05,2,0.05,0.5)
rslider  bounds(810, 40, 75, 75), text("Release"),channel("Rel"),         textbox(1), fontcolour("black"), textcolour("black"), trackercolour("DarkSlateGrey"), range(0.05,20,15,0.5)
rslider  bounds(870, 40, 75, 75), text("Mix"),    channel("Mix"),         textbox(1), fontcolour("black"), textcolour("black"), trackercolour("DarkSlateGrey"), range(0,1,1,0.5,0.001)
rslider  bounds(930, 40, 75, 75), text("Level"),  channel("amp"),         textbox(1), fontcolour("black"), textcolour("black"), trackercolour("DarkSlateGrey"), range(0,2,0.7,0.5,0.001)

keyboard bounds(  0,125,1010, 80)

</Cabbage>

<CsoundSynthesizer>

<CsOptions>
-dm0 -n -+rtmidi=null -M0
</CsOptions>

<CsInstruments>

sr 		= 	44100	;SAMPLE RATE
ksmps 		= 	32	;NUMBER OF AUDIO SAMPLES IN EACH CONTROL CYCLE
nchnls 		= 	2	;NUMBER OF CHANNELS (2=STEREO)
0dbfs		=	1                                     
		massign	0,2

giTriDist	ftgen	0,0,1024,21,3,1

;A RECURSIVE UDO IS USED TO CREATE THE STACK OF WGUIDE1S
opcode	wguide1stack, a, akkkkkkkkii					;OPCODE DEFINITION
ain,kfreq,kRndFactor,kcutoff,kLowCut,kfeedback,kint,kPortTime,ktype,icount,imax	xin		;INPUT ARG NAMES
	amix	=	0
	iRnd	trirand	1
	kRnd	=	iRnd * kRndFactor
	if ktype==0 then						;IF GEOMETRIC MODE HAS BEEN CHOSEN...
	 kfreq2	=	kfreq*semitone(kint*(icount-1) + kRnd)			;DEFINE FREQUENCY FOR THIS WGUIDE1 ACCORDING TO THE BASE FREQUENCY, INTERVAL AND THE COUNTER (LOCATION IN SERIES)
	else								;OTHERWISE MUST BE ARITHMETIC MODE
	 kfreq2	=	(kfreq+(kfreq*(icount-1)*kint)) * semitone(kRnd)			;DEFINE FREQUENCY FOR THIS WGUIDE1 ACCORDING TO THE BASE FREQUENCY, INTERVAL AND THE COUNTER (LOCATION IN SERIES)
	endif								;END OF CONDITIONAL
	if abs(kfreq2)>sr/3||abs(kfreq2)<20 then			;IF FREQUENCY IS OUTSIDE OF A SENSIBLE RANGE JUMP THE CREATION OF THE WGUIDE1 ALTOGETHER
	 asig	=	0
	else
	 kfreq2	portk	kfreq2,kPortTime
	 asig	wguide1 ain, kfreq2, kcutoff, kfeedback			;CREATE THE WGUIDE1 SIGNAL
	 if kLowCut==1 then
	  asig	buthp	asig,kfreq2
	  asig	buthp	asig,kfreq2	  
	 endif
	endif
	if icount<imax then						;IF THERE ARE STILL MORE WGUIDES TO CREATE IN THE STACK...
	  amix		wguide1stack	ain,kfreq,kRndFactor,kcutoff,kLowCut,kfeedback,kint,kPortTime,ktype,icount+1,imax	;CALL THE UDO AGAIN
	endif								;END OF CONDITIONAL
	skip:								;LABEL - SKIP TO HERE IF THE FREQUENCY WAS OUT OF RANGE
			xout		asig + amix			;SEND MIX OF ALL AUDIO BACK TO CALLER INSTRUMENT
endop									;END OF UDO


instr	1
	gkInSigMode		chnget	"InSigMode"
	if gkInSigMode==1 then
	 gasigL,gasigR	ins
	elseif gkInSigMode==2 then
	 gasigL	dust2	0.5,10
	 gasigR	dust2	0.5,10
	else
	 gasigL	pinkish	0.2
	 gasigR	pinkish	0.2
	endif
	
	kporttime	linseg		0,0.001,0.03
	
	gkfreq		chnget		"freq"
	;gkfreq		portk		gkfreq,kporttime
	gkCutoffMode	chnget		"CutoffMode"
	gkcutoff	chnget		"cutoff"
	gkCutoffRatio	chnget		"CutoffRatio"
	gkfeedback	chnget		"feedback"
	gkfeedback	portk		gkfeedback,kporttime
	gkinterval	chnget		"interval"
	;gkinterval	portk		gkinterval,kporttime
	gkmax		chnget		"max"
	gkmin		chnget		"min"
	ktype		chnget		"type"
	ktype		init		2
	gktype		=		ktype - 1	; COMBOBOX TO 0-1
	gkinput		chnget		"input"
	gkAtt		chnget		"Att"
	gkDec		chnget		"Dec"
	gkRel		chnget		"Rel"
	gkMix		chnget		"Mix"
	gkamp		chnget		"amp"
	gkPortamento	chnget		"Portamento"
	gkdual		chnget		"dual"
	gkLowCut	chnget		"LowCut"
	gkStWidth	chnget		"StWidth"
	gkRndFactor	chnget		"RndFactor"
	gkTune		chnget		"Tune"
	gkTune		*=		0.01				; CONVERT FROM CENTS TO SEMITONES
	gkLDiff		=		semitone(-gkStWidth+gkTune)
	gkRDiff		=		semitone(gkStWidth+gkTune)	
    gkDoubleStrength	chnget	"DoubleStrength"
	if changed(gkCutoffMode)==1 then
	 if gkCutoffMode==1 then
	  chnset	"visible(1)","cutoff_ident"
	  chnset	"visible(0)","CutoffRatio_ident"
	 else
	  chnset	"visible(0)","cutoff_ident"
	  chnset	"visible(1)","CutoffRatio_ident"
	 endif   
	endif

	event_i	"i",2,0.001,-1
	ktrig	trigger	gkinput,0.5,2
	if ktrig=1 then
	 if gkinput==0 then
	  event	"i",2,0,-1
	 endif
	endif
endin
		
instr	2
	/* MIDI AND GUI INTEROPERABILITY */
	iMIDIflag	=	0			; IF MIDI ACTIVATED = 1, NON-MIDI = 0
	mididefault	1, iMIDIflag			; IF NOTE IS MIDI ACTIVATED REPLACE iMIDIflag WITH '1'

	if iMIDIflag==1 then				; IF THIS IS A MIDI ACTIVATED NOTE...
	 inum	notnum
	 ivel	veloc	0,1
	 p1	=	p1 + (rnd(1000)*0.0001)
	 if gkinput==0 then
	  turnoff
	 endif
	 icps	cpsmidi					; READ MIDI PITCH VALUES - THIS VALUE CAN BE MAPPED TO GRAIN DENSITY AND/OR PITCH DEPENDING ON THE SETTING OF THE MIDI MAPPING SWITCHES
	 kfreq	init		icps
	else
	 kfreq	=		gkfreq
	endif						; END OF THIS CONDITIONAL BRANCH

	if trigger:k(gkinput,0.5,0)==1&&iMIDIflag==0 then
	 turnoff
	endif
	
	kRelease	release
	
	/* INPUT SIGNAL ENVELOPE */
	aenv		linsegr	0,i(gkAtt),1,i(gkDec),0
	asigL	=	gasigL*aenv
	asigR	=	gasigR*aenv
	
	/* DERIVE LOWPASS FILETR CUTOFF DEPENDING UPON MODE SELECTION */
	if gkCutoffMode==2 then
	 kcutoff	limit	gkCutoffRatio*kfreq,20,sr/2
	else
	 kcutoff	=	gkcutoff
	endif	

	/*PORTAMENTO TIME FUNCTION*/
	kPortTime	linseg	0,0.001,1
	kPortTime	*=	gkPortamento

	kchange		changed		gkmax,gkmin		;IF VALUATORS THAT DEFINE THE NUMBER OF WGUIDE1S NEEDED OR THE START POINT IN THE SERIES CHANGE GENERATE A MOMENTARY IMPULSE
	if kchange==1 then					;IF NUMBER OF WGUIDE1S NEEDED OR THE START POINT IN THE SERIES HAS CHANGED...
	  reinit	update					;REINITIALISE THE STACK CREATION
	endif			
	update:							;REINIT FROM HERE
	;CALL THE UDO. (ONCE FOR EACH CHANNEL.)
	aresL		wguide1stack	asigL, kfreq*gkLDiff, gkRndFactor, kcutoff, gkLowCut, gkfeedback, gkinterval, kPortTime, gktype, i(gkmin), i(gkmax)+i(gkmin)-1
	aresR		wguide1stack	asigR, kfreq*gkRDiff, gkRndFactor, kcutoff, gkLowCut, gkfeedback, gkinterval, kPortTime, gktype, i(gkmin), i(gkmax)+i(gkmin)-1
	if gkDoubleStrength==1 then
	 aresL		wguide1stack	aresL*0.03, kfreq*gkLDiff, gkRndFactor, kcutoff, gkLowCut, gkfeedback, gkinterval, kPortTime, gktype, i(gkmin), i(gkmax)+i(gkmin)-1
	 aresR		wguide1stack	aresR*0.03, kfreq*gkRDiff, gkRndFactor, kcutoff, gkLowCut, gkfeedback, gkinterval, kPortTime, gktype, i(gkmin), i(gkmax)+i(gkmin)-1
	endif
	if gkdual==1 then	;DUAL DIRECTION WGUIDE1S SELECTED (NOTE NEGATIVE 'kinterval'
	 if i(gkmin)==1 then	;DON'T DOUBLE UP FUNDEMENTAL IF 'Lowest' IS '1'
	  imin	=	i(gkmin)+1
	  imax	=	i(gkmax)+i(gkmin)-2
	 else
	  imin	=	i(gkmin)
	  imax	=	i(gkmax)+i(gkmin)-1
	 endif
	 if gkmin==1&&gkmax==1 kgoto skip	;IF 'Num.wguides' AND 'Lowest' ARE BOTH '1', DON'T CREATE ANY REFLECTED WGUIDE1S AT ALL 	
	 aresL	wguide1stack	asigL, kfreq*gkLDiff, gkRndFactor, kcutoff, gkLowCut, gkfeedback, -gkinterval, kPortTime, gktype, imin, imax
	 aresR	wguide1stack	asigR, kfreq*gkRDiff, gkRndFactor, kcutoff, gkLowCut, gkfeedback, -gkinterval, kPortTime, gktype, imin, imax
	 skip:
	endif
	aresL	dcblock2	aresL
	aresR	dcblock2	aresR
	rireturn						;RETURN FROM REINIT
	

	/* EXTEND RELEASE */
	kenv		linsegr	1,i(gkRel),0
	aresL		=	aresL * kenv
	aresR		=	aresR * kenv
	ktime		timeinsts
	krms		rms	aresL,3
	if krms<0.00001&&ktime>0.2&&iMIDIflag==1 then
	 turnoff2	p1,4,0
	endif

	/* WET_DRY MIX */
	aOutL	ntrpol	asigL, aresL, gkMix
	aOutR	ntrpol	asigR, aresR, gkMix
	
			outs	aOutL*gkamp, aOutR*gkamp	;SEND wguide OUTPUT TO THE AUDIO OUTPUTS AND SCALE USING THE FLTK SLIDER VARIABLE gkamp
endin

</CsInstruments>

<CsScore>
i 1 0 [3600*24*7]
</CsScore>

</CsoundSynthesizer>