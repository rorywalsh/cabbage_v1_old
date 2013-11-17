<Cabbage>
form caption("hsboscil Synth") size(500, 370)
image pos(0, 0), size(500, 80), colour("Blue"), shape("rounded"), outline("white"), line(4), caption("amplitude")
rslider bounds(5, 10, 60, 60), text("Amp."), channel("amp"), range(0, 1, 0.3)
rslider bounds(70, 10, 60, 60), text("Att."), channel("AAtt"), range(0, 8, 0.01,0.5)
rslider bounds(120, 10, 60, 60), text("Dec."), channel("ADec"), range(0, 8, 0.01,0.5)
rslider bounds(170, 10, 60, 60), text("Sus."), channel("ASus"), range(0, 1, 0.5, 0.5)
rslider bounds(220, 10, 60, 60), text("Rel."), channel("ARel"), range(0.01, 8, 0.01, 0.5)
rslider bounds(285, 10, 60, 60), text("Rate"), channel("ARte"), range(0, 16, 4)
rslider bounds(335, 10, 60, 60), text("Depth"), channel("ADep"), range(0, 1, 0)
rslider bounds(385, 10, 60, 60), text("Delay"), channel("ADel"), range(0, 2, 0, 0.5)
rslider bounds(435, 10, 60, 60), text("Rise"), channel("ARis"), range(0, 2, 0.1, 0.5)

image pos(0, 85), size(390, 145), colour("Blue"), shape("rounded"), outline("white"), line(4) 
rslider bounds(5, 95, 60, 60), text("Brite"), channel("brite"), range(-6, 6, 2)
rslider bounds(5, 160, 60, 60), text("Vel."), channel("BVelDep"), range(0, 6, 3)
rslider bounds(70, 95, 60, 60), text("Att."), channel("BAtt"), range(0, 8, 0.01,0.5)
rslider bounds(120, 95, 60, 60), text("Att.Lev."), channel("BAttLev"), range(-6, 6, 1.7)
rslider bounds(170, 95, 60, 60), text("Dec."), channel("BDec"), range(0, 8, 0.1,0.5)
rslider bounds(220, 95, 60, 60), text("Sus."), channel("BSus"), range(-6, 6, 0)
rslider bounds(270, 95, 60, 60), text("Rel."), channel("BRel"), range(0, 8, 0.01,0.5)
rslider bounds(320, 95, 60, 60), text("Rel.Lev."), channel("BRelLev"), range(-4, 4, 0)
rslider bounds(320, 160, 60, 60), text("Oct.Cnt."), channel("octcnt"), range(2, 10, 3, 1, 1)
rslider bounds(70, 160, 60, 60), text("Rate"), channel("BRte"), range(0, 16, 2)
rslider bounds(120, 160, 60, 60), text("Depth"), channel("BDep"), range(0, 6, 1)
rslider bounds(170, 160, 60, 60), text("Delay"), channel("BDel"), range(0, 2, 0.5, 0.5)
rslider bounds(220, 160, 60, 60), text("Rise"), channel("BRis"), range(0, 4, 1.5, 0.5)

image pos(395, 85), size(105, 145), colour("Blue"), shape("rounded"), outline("white"), line(4) 
rslider bounds(420,  95, 60, 60), text("Tone"), channel("tone"), range(-1, 1, 0)
rslider bounds(420, 160, 60, 60), text("Risset"), channel("TRate"), range(-3, 3, 0,1,0.001)

keyboard pos(0, 240), size(500, 100)
}
</Cabbage>

/*
PANEL 1
-------
Amp	-	amplitude control
Att.	-	amplitude envelope attack time
Dec.	-	amplitude envelope decay time
Sus.	-	amplitude envelope sustain level
Rel.	-	amplitude envelope release time
Rate	-	rate of amplitude LFO (tremolo)
Depth	-	depth of amplitude LFO
Delay	-	delay time before amplitude LFO begins
Rise	-	time over which amplitude LFO depth rises from zero to its prescribed value

PANEL 2
-------
Brite	-	manual brightness control
Vel.	-	amount of velocity to brightness control
Att.	-	brightness envelope attack time
Att.Lev	-	brightness envelope attack level
Dec.	-	brightness envelope decay time
Sus.	-	brightness envelope sustain level
Rel.	-	brightness envelope release time
Rel.Lev	-	brightness envelope release level
Oct.Cnt.-	Number of octave spaced partials in a tone
Rate	-	rate of brightness LFO
Depth	-	depth of brightness LFO
Delay	-	delay time before brightness LFO begins
Rise	-	time over which brightness LFO depth rises from zero to its prescribed value

PANEL 3 (small panel)
---------------------
Tone	-	Transposition of a tone (-1 semitone to +1 semitone)
LFO Rate-	Rate of an LFO (sawtooth shape) that modulates 'Tone' (see above)

*/

<CsoundSynthesizer>

<CsOptions>
-dm0 -n -+rtmidi=null -M0
</CsOptions>

<CsInstruments>

sr 		= 	44100	;SAMPLE RATE
ksmps 		= 	16	;NUMBER OF AUDIO SAMPLES IN EACH CONTROL CYCLE
nchnls 		= 	2	;NUMBER OF CHANNELS (2=STEREO)
0dbfs		=	1

;Author: Iain McCurdy (2012)

giwfn	ftgen	0, 0, 131072, 10, 1			;A SINE WAVE
gioctfn	ftgen	0, 0, 1024, -19, 1, 0.5, 270, 0.5	;A HANNING-TYPE WINDOW

instr	1
	kporttime	linseg	0,0.001,0.01				;PORTAMENTO TIME VALUE RISES QUICKLY FROM ZERO TO A HELD VALUE 
	kamp		chnget	"amp"					;READ WIDGETS...
	iAAtt		chnget	"AAtt"					;
	iADec		chnget	"ADec"					;
	iASus		chnget	"ASus"					;
	iARel		chnget	"ARel"					;
	aenv		linsegr	0.001,iAAtt+0.001,1,iADec+0.001,iASus,iARel+0.001,0.001	;AMPLITUDE ENVELOPE
	iADel		chnget	"ADel"					;
	iARis		chnget	"ARis"					;
	kADep		chnget	"ADep"					;
	kARte		chnget	"ARte"					;
	ktone		chnget	"tone"					;READ WIDGETS...
	ktone		portk	ktone,kporttime				;
	kbrite		chnget	"brite"					;
	kbrite		portk	kbrite,kporttime
	koctcnt		chnget	"octcnt"				;
	iBVelDep	chnget	"BVelDep"				;

	iBAtt		chnget	"BAtt"					;
	iBAttLev	chnget	"BAttLev"				;
	iBDec		chnget	"BDec"					;
	iBSus		chnget	"BSus"					;
	iBRel		chnget	"BRel"					;
	iBRelLev	chnget	"BRelLev"				;
	kBEnv		linsegr	0,iBAtt+0.001,iBAttLev,iBDec+0.001,iBSus,iBRel+0.001,iBRelLev	;'BRIGHTNESS' ENVELOPE
	iBDel		chnget	"BDel"					;
	iBRis		chnget	"BRis"					;
	kBDep		chnget	"BDep"					;
	kBRte		chnget	"BRte"					;

	kTRate		chnget	"TRate"					;

	ibasfreq	cpsmidi						;READ MIDI NOTE PITCH

	ktrig		changed		koctcnt				;IF 'NUMBER OF OCTAVES' AS DEFINED BY WIDGET CHANGES GENERATE A TRIGGER
	if ktrig==1 then 						;IF 'NUMBER OF OCTAVES' CHANGE TRIGGER HAS BEEN GENERATED...
	 reinit UPDATE							;BEGIN A REINITIALISATION PASS FROM LABEL 'UPDATE'
	endif								;END OF CONDITIONAL
	UPDATE:								;LABEL 'UPDATE'. REINITIALISATION PASS BEGINS FROM HERE

	;AMPLITUDE LFO
	kdepth		linseg		0,iADel+0.001,0,iARis+0.001,1	;DEPTH OF MODULATION ENVELOPE
	kALFO		lfo		(kdepth*kADep)/2,kARte,0	;LFO
	kALFO		=		(kALFO+0.5) + (kADep * 0.5)	;OFFSET AND RESCALE LFO FUNCTION

	;VELOCITY TO AMPLITUDE
	iVelAmp		veloc		0.2,1				;READ IN MIDI NOTE VELOCITY AS A VALUE WITHIN THE RANGE 0.2 TO 1

	;BRIGHTNESS LFO
	kdepth		linseg		0,iBDel+0.001,0,iBRis+0.001,1	;DEPTH OF MODULATION ENVELOPE
	kBLFO		lfo		kdepth*kBDep,kBRte,0		;LFO

	;VELOCITY TO BRIGHTNESS
	iBVel		ampmidi		iBVelDep			;MIDI NOTE VELOCITY USED TO DERIVE A VALUE THAT WILL INFLUENCE 'BRIGHTNESS'

	kTLFO	lfo	1,kTRate,4					;'RISSET GLISSANDO' FUNCTION
	
	aL 		hsboscil	kamp*kALFO*iVelAmp, ktone+kTLFO, kbrite+kBEnv+kBLFO-iBVel, ibasfreq, giwfn, gioctfn, i(koctcnt), rnd(1)	;CREATE AN hsboscil TONE FOR THE LEFT CHANNEL. RANDOM INITIAL PHASE CREATE STEREO EFFECT.
	aR 		hsboscil	kamp*kALFO*iVelAmp, ktone+kTLFO, kbrite+kBEnv+kBLFO-iBVel, ibasfreq, giwfn, gioctfn, i(koctcnt), rnd(1)	;CREATE AN hsboscil TONE FOR THE RIGHT CHANNEL. RANDOM INITIAL PHASE CREATE STEREO EFFECT.
	aL		=		aL*aenv				;APPLY AMPLITUDE ENVELOPE TO AUDIO OUTPUT OF hsboscil
	aR		=		aR*aenv				;APPLY AMPLITUDE ENVELOPE TO AUDIO OUTPUT OF hsboscil
	rireturn
			outs		aL,aR			;SEND AUDIO TO OUTPUTS
endin
		
</CsInstruments>

<CsScore>
f 0 [3600*24*7]
</CsScore>

</CsoundSynthesizer>