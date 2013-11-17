oscbnk_synth.csd

<Cabbage>
form caption("Oscillator Bank Synth") size(485, 370)

image bounds(  0,  0,485, 255), colour(200, 0,100,180), shape("sharp"), outline("white"), line(0)

;OSCILLATOR
groupbox bounds(0, 0, 285, 85), text("Oscillators"){
rslider bounds(  5, 25, 55, 55), fontcolour("white"), text("Amp."), channel("amp"), range(0, 3, 2), tracker("white")
rslider bounds( 50, 25, 55, 55), fontcolour("white"), text("N.Oscs."), channel("NOscs"), range(1, 100, 10, 1, 1), tracker("white")
rslider bounds( 95, 25, 55, 55), fontcolour("white"), text("Spread"), channel("fmd"), range(0, 1, 0.05,0.25,0.0001), tracker("white")
rslider bounds(140, 25, 55, 55), fontcolour("white"), text("Speed"), channel("mvt"), range(0, 100, 1, 0.5), tracker("white")
combobox bounds(195, 25, 80,20), channel("waveform"), value(1), text("saw", "square", "ahh", "organ", "trombone", "bassoon", "oboe", "clarinet", "eee", "ooh")
checkbox bounds(195, 50, 80,20), text("mono"), colour("yellow"), channel("mono"),  value(0)
rslider  bounds(250, 50, 30, 30), fontcolour("white"), text("Time"), channel("LegTim"), range(0, 3, 0.05, 0.5), tracker("white")
}

;AMPLITUDE ENVELOPE
groupbox bounds(285, 0, 200, 85), text("Amplitude Envelope"){
rslider bounds(290, 25, 55, 55), fontcolour("black"), text("Att."), channel("AAtt"), range(0, 8, 0.01,0.5), tracker("white")
rslider bounds(335, 25, 55, 55), fontcolour("black"), text("Dec."), channel("ADec"), range(0, 8, 0.01,0.5), tracker("white")
rslider bounds(380, 25, 55, 55), fontcolour("black"), text("Sus."), channel("ASus"), range(0, 1, 0.5, 0.5), tracker("white")
rslider bounds(425, 25, 55, 55), fontcolour("black"), text("Rel."), channel("ARel"), range(0.01, 8, 1.67, 0.5), tracker("white")
}

;FILTER
groupbox bounds(0, 85, 335, 85), text("Filter Envelope"){
rslider bounds(  5, 110, 55, 55), fontcolour("white"), text("Filter"), channel("cf"), range(0, 10, 2), tracker("white")
rslider bounds( 50, 110, 55, 55), fontcolour("white"), text("Res."), channel("res"), range(0, 1, 0), tracker("white")
rslider bounds( 95, 110, 55, 55), fontcolour("white"), text("Env."), channel("FEnvAmt"), range(0, 10, 4), tracker("white")
rslider bounds(140, 110, 55, 55), fontcolour("white"), text("Att."), channel("FAtt"), range(0, 8, 0.4,0.5), tracker("white")
rslider bounds(185, 110, 55, 55), fontcolour("white"), text("Dec."), channel("FDec"), range(0, 8, 1,0.5), tracker("white")
rslider bounds(230, 110, 55, 55), fontcolour("white"), text("Sus."), channel("FSus"), range(0, 10, 4, 0.5), tracker("white")
rslider bounds(275, 110, 55, 55), fontcolour("white"), text("Rel."), channel("FRel"), range(0.01, 8, 1.3, 0.5), tracker("white")
}

groupbox bounds(335, 85, 150, 85), text("-"){
}

;FILTER LFO
groupbox bounds(  0,170, 280, 85), text("Filter LFO"){
combobox bounds( 15,205, 70, 20), channel("FlfoType"), value(2), text("sine", "splines", "S+H", "square")
rslider  bounds( 85,195, 55, 55), text("Rate"), channel("FRte"), range(0, 16, 4), fontcolour("white"), tracker("white")
rslider  bounds(130,195, 55, 55), text("Depth"), channel("FDep"), range(0, 4, 0), fontcolour("white"), tracker("white")
rslider  bounds(175,195, 55, 55), text("Delay"), channel("FDel"), range(0, 2, 0, 0.5), fontcolour("white"), tracker("white")
rslider  bounds(220,195, 55, 55), text("Rise"), channel("FRis"), range(0, 2, 0.1, 0.5), fontcolour("white"), tracker("white")

;EFFECTS
groupbox bounds(280,170, 110, 85), text("Reverb"){
rslider bounds(285, 195,  55, 55), fontcolour("white"), text("Mix"), channel("RvbMix"), range(0, 1, 0.2), tracker("white")
rslider bounds(330, 195,  55, 55), fontcolour("white"), text("Size"), channel("RvbSize"), range(0, 1, 0.2), tracker("white")
}

groupbox bounds(390,170, 95, 85), text("CPU Usage"){
}

keyboard bounds(0, 260, 480, 75)

image bounds(5, 343, 250, 25), colour(75, 85, 90, 100), plant("credit"){
label bounds(0.03, 0.2, .9, .6), text("Author: Iain McCurdy |2012|"), colour("white")
}

</Cabbage>

<CsoundSynthesizer>

<CsOptions>
-dm0 -n -+rtmidi=NULL -M0
</CsOptions>

<CsInstruments>

sr	=	44100
ksmps	=	64
nchnls	=	2
0dbfs	=	1
massign	0,1

;Author: Iain McCurdy (2012)


gisaw		ftgen	0,0,4096,11,80,1,0.9
gisquare	ftgen	0,0,4096,10, 1,0, 1/3,0, 1/5,0, 1/7,0, 1/9,0, 1/11,0, 1/13,0, 1/15,0, 1/17,0, 1/19,0, 1/21,0, 1/23,0, 1/25,0, 1/27,0, 1/29,0, 1/31,0, 1/33,0, 1/35,0, 1/37,0, 1/39
giahh		ftgen	0, 0, 4096, 10, 1, 0.615880,0.089582,0.201498,0.202694,0.249961,0.225852,0.199864,0.029378,0.012306,0.007803,0.007846,0.007454,0.008827,0.007466,0.007495,0.007979,0.017837,0.061000,0.059055,0.039181,0.038364,0.032715,0.048415,0.029643,0.010094
giorg		ftgen	0,0,4096,9, 1,1,rnd(1), 2,1/2,rnd(1), 4,1/4,rnd(1), 8,1/8,rnd(1), 16,1/16,rnd(1), 32,1/32,rnd(1), 64,1/64,rnd(1), 128,1/128,rnd(1)
gitromb		ftgen	0,0,4096,10,0.104682,0.077444,0.175816,0.178469,0.192922,0.167695,0.127283,0.094343,0.073856,0.045425,0.030985,0.027993,0.019936,0.013950,0.010611,0.007164,0.006458,0.003987,0.002664,0.002030,0.001435,0.001274,0.001036,0.000975,0.000876
;gitrpt		ftgen	0,0,4096,10,0.346469,0.852732,0.693941,0.494616,0.677656,0.590592,0.441108,0.626951,0.588338,0.416046,0.198161,0.107710,0.085130,0.075318,0.052485,0.050367,0.041515,0.039286,0.027532,0.023320,0.016384,0.012677,0.009971,0.006867,0.004900
gibassoon	ftgen	0,0,4096,10, 0.130954,0.157859,0.302897,0.190190,0.890855,0.427124,0.039988,0.105341,0.011869,0.102000,0.115308,0.039058,0.131398,0.081308,0.047672,0.026006,0.010927,0.034420,0.033017,0.014769,0.014178,0.023091,0.023960,0.013178,0.004946
gioboe		ftgen	0,0,4096,10, 0.258272,0.920123,0.458442,0.445704,0.088446,0.177257,0.063786,0.140382,0.031660,0.021808,0.048812,0.007823,0.004163,0.004775,0.002608,0.001464,0.001702,0.004168,0.003318,0.003878,0.002319,0.002505,0.002480,0.003416,0.002453
giclarinet	ftgen	0,0,4096,10, 0.820044,0.009787,0.242206,0.034475,0.058957,0.021330,0.028307,0.053692,0.006131,0.002513,0.002370,0.001698,0.001269,0.000933,0.002758,0.001387,0.000831,0.000953,0.000658,0.000892,0.000687,0.000479,0.000481,0.000310,0.000425
gieee 		ftgen 	0,0,4096,10, 0.430487,0.778043,0.090552,0.011467,0.030632,0.017911,0.006520,0.006820,0.007109,0.006437,0.008527,0.009083,0.016774,0.058922,0.070507,0.116728,0.045479,0.060107,0.069023,0.036487,0.047350,0.049526,0.036678,0.022550,0.022550
giooh 		ftgen 	0,0,4096,10, 0.110941,0.606890,0.046168,0.040510,0.031310,0.007323,0.003115,0.002749,0.002420,0.002253,0.002853,0.002337,0.002672,0.002061,0.002202,0.002410,0.002615,0.005079,0.003192,0.002699,0.003708,0.003501,0.002264,0.001713,0.001713

gicos		ftgen	0,0,4096,11,1		;COSINE WAVE (USED BY THE LFOS)
gieqffn		ftgen	0,0,4097,7,-1,4096,1
gieqlfn		ftgen	0,0,4097,7,-1,4096,1
gieqqfn		ftgen	0,0,4097,7,-1,4096,1

gasendL,gasendR	init	0


instr	1
	imono	chnget	"mono"
	gkNoteTrig	init	1	;at the beginning of a new note set note trigger flag to '1'
	icps		cpsmidi		;read in midi note pitch in cycles per second
	givel		veloc	0,1	;read in midi note velocity
	gkcps	init	icps		;update a global krate variable for note pitch

	if imono==0 then		;if we are *not* in legato mode...
	 aL,aR	subinstr	p1+1,icps	;call voice instrument as a subinstrument. Audio will be fed back to this instrument before being sent to the outputs.
		outs	aL,aR		;send audio to outputs
	else				;otherwise... (i.e. legato mode)
	 iactive	active p1+1	;check to see if these is already a note active...
	 if iactive==0 then		;...if not...
	  event_i	"i",p1+1,0,-1	;...start a new held note
	 endif
	endif
endin

instr	2
	imono	chnget	"mono"
	kLegTim	chnget	"LegTim"

	kporttime	linseg	0,0.001,1		;portamento time function rises quickly from zero to a held value
	kglisstime	=	kporttime*kLegTim	;scale portamento time function with value from GUI knob widget
	
	if imono==1 then				;if we are in legato mode...
	 kcps	portk	gkcps,kglisstime		;apply portamento smooth to changes in note pitch (this will only have an effect in 'legato' mode)
	 kactive	active	p1-1			;...check number of active midi notes (previous instrument)
	 if kactive==0 then				;if no midi notes are active...
	  turnoff					;... turn this instrument off
	 endif
	else						;otherwise... (polyphonic / non-legato mode)
	 kcps	=	p4		 		;pitch equal to the original note pitch
	endif

	;OSCILLATOR
	kamp		chnget	"amp"
	kNOscs		chnget	"NOscs"
	kfmd		chnget	"fmd"
	kmvt		chnget	"mvt"
	kwaveform	chnget	"waveform"
	kwaveform	init	1
	
	;AMPLITUDE ENVELOPE
	iAAtt		chnget	"AAtt"
	iADec		chnget	"ADec"
	iASus		chnget	"ASus"
	iARel		chnget	"ARel"
	aenv		linsegr	0.001,iAAtt+0.001,1,iADec+0.001,iASus,iARel+0.001,0.001	;AMPLITUDE ENVELOPE
	
	;FILTER AND FILTER ENVELOPE
	kcfoct		chnget	"cf"
	kres		chnget	"res"
	kFEnvAmt	chnget	"FEnvAmt"
	iFAtt		chnget	"FAtt"
	iFDec		chnget	"FDec"
	iFSus		chnget	"FSus"
	iFRel		chnget	"FRel"
	kFEnv		linsegr	0,iFAtt+0.001,1,iFDec+0.001,iFSus,iFRel+0.001,0
	kFEnv		=	kFEnv * kFEnvAmt

	;FILTER LFO
	kFlfoType	chnget	"FlfoType"				;
	kFRte		chnget	"FRte"					;
	kFDep		chnget	"FDep"					;
	iFDel		chnget	"FDel"					;
	iFRis		chnget	"FRis"					;
	kdepth		linseg		0,iFDel+0.001,0,iFRis+0.001,1	;DEPTH OF MODULATION ENVELOPE
	
	if kFlfoType==1 then
	 kFLFO		lfo		kdepth*kFDep,kFRte,0		;LFO (sine)
	elseif kFlfoType==2 then
	 kFLFO		jspline		kdepth*kFDep,kFRte,kFRte
	elseif kFlfoType==3 then
	 kFLFO		randomh		-kdepth*kFDep,kdepth*kFDep,kFRte
	 kFLFO		port		kFLFO,0.004			;smooth out the clicks
	else
	 kFLFO		lfo		kdepth*kFDep,kFRte,2		;LFO (bi-square)
	 kFLFO		port		kFLFO,0.004			;smooth out the clicks
	endif

	kCF		=	cpsoct(4+kFEnv+kcfoct+kFLFO)

	kCF		limit	kCF, 20, 5000
	  
	ivel	veloc	0,0.1					;READ IN NOTE VELOCITY	  
	  
	kwave	=	gisaw+kwaveform-1			;DERIVE FUNCTION TABLE NUMBER OF WAVEFORM FOR THE OSCILLATORS

	ktrig	changed	kNOscs					;IF ANY OF THE CONTROLS WHICH MUST BE ENTERED AS I-RATE CONTROLLERS HAVE CHANGED GENERATE A TRIGGER IMPULSE AT THE OUTPUT
	if ktrig==1 then					;IF ANY I-RATE CONTROL HAS CHANGED...
	  reinit	UPDATE					;BEGIN A RE-INITILISATION PASS FROM LABEL 'UPDATE'
	endif							
	UPDATE:							;BEGIN REINIT PASS FROM HERE
	kmvt	jspline	kmvt,kmvt,kmvt
	;TWO ITERATIONS OF oscbnk ARE CREATED, ONE FOR EACH OF THE STEREO AUDIO CHANNELS. THE OUTPUTS WILL BE DIFFERENT AS THE RANDOM NUMBER GENERATORS WILL BE SEEDED BY THE SYSTEM CLOCK
	;OUTPUT	OPCODE  CPS  | AMD  |    FMD     | PMD | OVERLAPS   | SEED | L1MINF  | L1MAXF  | L2MINF  | L2MAXF  | LFOMODE | EQMINF  | EQMAXF | EQMINL | EQMAXL | EQMINQ | EQMAXQ  | EQMODE | KFN  | L1FN | L2FN | EQFFN  | EQLF   |  EQQFN |  TABL  | OUTFN
	aL	oscbnk	kcps,   0,    kfmd*kcps,    0,  i(kNOscs),   rnd(1),    0,      kmvt,      0,        0,       238,      0,       8000,      1,       1,       1,       1,       -1,   kwave, gicos, gicos, gieqffn, gieqlfn, gieqqfn
	aR	oscbnk	kcps,   0,    kfmd*kcps,    0,  i(kNOscs),   rnd(1),    0,     -kmvt,      0,        0,       238,      0,       8000,      1,       1,       1,       1,       -1,   kwave, gicos, gicos, gieqffn, gieqlfn, gieqqfn
	rireturn						;RETURN FROM REINIT PASS
	;aL	moogladder	aL,kCF,kres			;USE 'moogvcf2' IF YOU NEED TO CONSERVE CPU RESOURCES
	;aR	moogladder	aR,kCF,kres
	;aL	moogvcf		aL,kCF,kres			;USE 'moogvcf2' IF YOU NEED TO CONSERVE CPU RESOURCES
	;aR	moogvcf		aR,kCF,kres

	aL 	bqrez 		aL, kCF, 1+kres*20
	aR 	bqrez 		aR, kCF, 1+kres*20
	
	aL	=	aL*aenv*ivel*kamp
	aR	=	aR*aenv*ivel*kamp
	
		outs		aL, aR	;SEND AUDIO TO THE OUTPUTS. RESCALE EACH CHANNEL WITH NOTE VELOCITY, AMPLITUDE ENVELOPE AND AMP CONTROL WIDGET.
	gkRvbMix	chnget	"RvbMix"
	gasendL	=	gasendL+(aL*gkRvbMix)
	gasendR	=	gasendR+(aR*gkRvbMix)
endin

instr	4	;reverb
	gkRvbSize	chnget	"RvbSize"
	if gkRvbMix==0 goto SKIP_REVERB
	aL,aR	reverbsc	gasendL,gasendR,gkRvbSize,12000
		outs	aL,aR
	SKIP_REVERB:
		clear	gasendL,gasendR
endin

</CsInstruments>

<CsScore>
f 0 [3600*24*7]
i 4 0 [3600*24*7]
</CsScore>

</CsoundSynthesizer>