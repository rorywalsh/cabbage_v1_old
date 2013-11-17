<Cabbage>
form caption("FOF Voice") size(510, 355)
xypad bounds(10, 10, 250, 210), channel("vowel", "oct"), text("X = Vowel 'AEIOU' : Y = Octave Division"), rangex(0, 1, 0.5), rangey(4, 0, 0)
rslider bounds(265,  10, 60, 60), text("Vib.Dep."), channel("vibdep"), range(0, 0.4, 0)
rslider bounds(325,  10, 60, 60), text("Mod.Delay"), channel("moddel"), range(0, 2, 0.3, 0.5)
rslider bounds(385,  10, 60, 60), text("Mod.Rise"), channel("modris"), range(0, 4, 2, 0.5)
rslider bounds(445,  10, 60, 60), text("Mod.Rate"), channel("modrte"), range(0.1, 8, 5, 0.5)
rslider bounds(265, 80, 60, 60), text("Trem.Dep."), channel("trmdep"), range(0, 1, 0)
rslider bounds(325, 80, 60, 60), text("Level"), channel("lev"), range(0, 1, 1)
combobox bounds(385, 80, 110,25), channel("voice"), value(1), text("Bass", "Tenor", "Countertenor", "Alto", "Soprano"), colour("black")

keyboard pos(10, 225), size(490, 100)

</Cabbage>

<CsoundSynthesizer>

<CsOptions>
-dm0 -n -+rtmidi=null -M0
</CsOptions>

<CsInstruments>
sr 	= 	44100
ksmps 	= 	32
nchnls 	= 	2
0dbfs	=	1

;Author: Iain McCurdy (2012)

gisine		ftgen	0, 0, 4096, 10, 1				;SINE WAVE
giexp		ftgen	0, 0, 1024, 19, 0.5, 0.5, 270, 0.5		;EXPONENTIAL CURVE USED TO DEFINE THE ENVELOPE SHAPE OF FOF PULSES

;FUNCTION TABLES STORING DATA FOR VARIOUS VOICE FORMANTS
;THE FIRST VALUE OF EACH TABLE DEFINES THE NUMBER OF DATA ELEMENTS IN THE TABLE
;THIS IS NEEDED BECAUSE TABLES SIZES MUST BE POWERS OF 2 TO FACILITATE INTERPOLATED TABLE READING (tablei) 
;BASS
giBF1		ftgen	0,  0, 8, -2,	4, 600,		400,	250,	350	;FREQ
giBF2		ftgen	0,  0, 8, -2,	4, 1040,	1620,	1750,	600	;FREQ
giBF3		ftgen	0,  0, 8, -2,	4, 2250,	2400,	2600,	2400	;FREQ
giBF4		ftgen	0,  0, 8, -2,	4, 2450,	2800,	3050,	2675	;FREQ
giBF5		ftgen	0,  0, 8, -2,	4, 2750,	3100,	3340,	2950	;FREQ
	        	
giBDb1		ftgen	0, 0, 8, -2,	4, 0,	0,	0,	0	;dB
giBDb2		ftgen	0, 0, 8, -2,	4, -7,	-12,	-30,	-20	;dB
giBDb3		ftgen	0, 0, 8, -2,	4, -9,	-9,	-16,	-32	;dB
giBDb4		ftgen	0, 0, 8, -2,	4, -9,	-12,	-22,	-28	;dB
giBDb5		ftgen	0, 0, 8, -2,	4, -20,	-18,	-28,	-36	;dB
	        	
giBBW1		ftgen	0, 0, 8, -2,	4, 60,	40,	60,	40	;BAND WIDTH
giBBW2		ftgen	0, 0, 8, -2,	4, 70,	80,	90,	80	;BAND WIDTH
giBBW3		ftgen	0, 0, 8, -2,	4, 110,	100,	100,	100	;BAND WIDTH
giBBW4		ftgen	0, 0, 8, -2,	4, 120,	120,	120,	120	;BAND WIDTH
giBBW5		ftgen	0, 0, 8, -2,	4, 130,	120,	120,	120	;BAND WIDTH
;TENOR  	
giTF1		ftgen	0, 0, 8, -2,	5, 650, 	400,	290,	400,	350	;FREQ
giTF2		ftgen	0, 0, 8, -2,	5, 1080, 	1700,   1870,	800,	600	;FREQ
giTF3		ftgen	0, 0, 8, -2,	5, 2650,	2600,   2800,	2600,	2700	;FREQ
giTF4		ftgen	0, 0, 8, -2,	5, 2900,	3200,   3250,	2800,	2900	;FREQ
giTF5		ftgen	0, 0, 8, -2,	5, 3250,	3580,   3540,	3000,	3300	;FREQ
	        	
giTDb1		ftgen	0, 0, 8, -2,	5, 0,	0,	0,	0,	0	;dB
giTDb2		ftgen	0, 0, 8, -2,	5, -6,	-14,	-15,	-10,	-20	;dB
giTDb3		ftgen	0, 0, 8, -2,	5, -7,	-12,	-18,	-12,	-17	;dB
giTDb4		ftgen	0, 0, 8, -2,	5, -8,	-14,	-20,	-12,	-14	;dB
giTDb5		ftgen	0, 0, 8, -2,	5, -22,	-20,	-30,	-26,	-26	;dB
	        	
giTBW1		ftgen	0, 0, 8, -2,	5, 80,	70,	40,	40,	40	;BAND WIDTH
giTBW2		ftgen	0, 0, 8, -2,	5, 90,	80,	90,	80,	60	;BAND WIDTH
giTBW3		ftgen	0, 0, 8, -2,	5, 120,	100,	100,	100,	100	;BAND WIDTH
giTBW4		ftgen	0, 0, 8, -2,	5, 130,	120,	120,	120,	120	;BAND WIDTH                                         
giTBW5		ftgen	0, 0, 8, -2,	5, 140,	120,	120,	120,	120	;BAND WIDTH
;COUNTER TENOR
giCTF1		ftgen	0, 0, 8, -2,	5, 660,	440,	270,	430,	370		;FREQ
giCTF2		ftgen	0, 0, 8, -2,	5, 1120,	1800,	1850,	820,	630	;FREQ
giCTF3		ftgen	0, 0, 8, -2,	5, 2750,	2700,	2900,	2700,	2750	;FREQ
giCTF4		ftgen	0, 0, 8, -2,	5, 3000,	3000,	3350,	3000,	3000	;FREQ
giCTF5		ftgen	0, 0, 8, -2,	5, 3350,	3300,	3590,	3300,	3400	;FREQ
	        	
giTBDb1		ftgen	0, 0, 8, -2,	5, 0,	0,	0,	0,	0	;dB
giTBDb2		ftgen	0, 0, 8, -2,	5, -6,	-14,	-24,	-10,	-20	;dB
giTBDb3		ftgen	0, 0, 8, -2,	5, -23,	-18,	-24,	-26,	-23	;dB
giTBDb4		ftgen	0, 0, 8, -2,	5, -24,	-20,	-36,	-22,	-30	;dB
giTBDb5		ftgen	0, 0, 8, -2,	5, -38,	-20,	-36,	-34,	-30	;dB
	        	
giTBW1		ftgen	0, 0, 8, -2,	5, 80,	70,	40,	40,	40	;BAND WIDTH
giTBW2		ftgen	0, 0, 8, -2,	5, 90,	80,	90,	80,	60	;BAND WIDTH
giTBW3		ftgen	0, 0, 8, -2,	5, 120,	100,	100,	100,	100	;BAND WIDTH
giTBW4		ftgen	0, 0, 8, -2,	5, 130,	120,	120,	120,	120	;BAND WIDTH
giTBW5		ftgen	0, 0, 8, -2,	5, 140,	120,	120,	120,	120	;BAND WIDTH
;ALTO   	
giAF1		ftgen	0, 0, 8, -2,	5, 800,	400,	350,	450,	325		;FREQ
giAF2		ftgen	0, 0, 8, -2,	5, 1150,	1600,	1700,	800,	700	;FREQ
giAF3		ftgen	0, 0, 8, -2,	5, 2800,	2700,	2700,	2830,	2530	;FREQ
giAF4		ftgen	0, 0, 8, -2,	5, 3500,	3300,	3700,	3500,	2500	;FREQ
giAF5		ftgen	0, 0, 8, -2,	5, 4950,	4950,	4950,	4950,	4950	;FREQ
	        	
giADb1		ftgen	0, 0, 8, -2,	5, 0,	0,	0,	0,	0	;dB
giADb2		ftgen	0, 0, 8, -2,	5, -4,	-24,	-20,	-9,	-12	;dB
giADb3		ftgen	0, 0, 8, -2,	5, -20,	-30,	-30,	-16,	-30	;dB
giADb4		ftgen	0, 0, 8, -2,	5, -36,	-35,	-36,	-28,	-40	;dB
giADb5		ftgen	0, 0, 8, -2,	5, -60,	-60,	-60,	-55,	-64	;dB
	        	
giABW1		ftgen	0, 0, 8, -2,	5, 50,	60,	50,	70,	50	;BAND WIDTH
giABW2		ftgen	0, 0, 8, -2,	5, 60,	80,	100,	80,	60	;BAND WIDTH
giABW3		ftgen	0, 0, 8, -2,	5, 170,	120,	120,	100,	170	;BAND WIDTH
giABW4		ftgen	0, 0, 8, -2,	5, 180,	150,	150,	130,	180	;BAND WIDTH
giABW5		ftgen	0, 0, 8, -2,	5, 200,	200,	200,	135,	200	;BAND WIDTH
;SOPRANO
giSF1		ftgen	0, 0, 8, -2,	5, 800,	350,	270,	450,	325		;FREQ
giSF2		ftgen	0, 0, 8, -2,	5, 1150,	2000,	2140,	800,	700	;FREQ
giSF3		ftgen	0, 0, 8, -2,	5, 2900,	2800,	2950,	2830,	2700	;FREQ
giSF4		ftgen	0, 0, 8, -2,	5, 3900,	3600,	3900,	3800,	3800	;FREQ
giSF5		ftgen	0, 0, 8, -2,	5, 4950,	4950,	4950,	4950,	4950	;FREQ
	        	
giSDb1		ftgen	0, 0, 8, -2,	5, 0,	0,	0,	0,	0	;dB
giSDb2		ftgen	0, 0, 8, -2,	5, -6,	-20,	-12,	-11,	-16	;dB
giSDb3		ftgen	0, 0, 8, -2,	5, -32,	-15,	-26,	-22,	-35	;dB
giSDb4		ftgen	0, 0, 8, -2,	5, -20,	-40,	-26,	-22,	-40	;dB
giSDb5		ftgen	0, 0, 8, -2,	5, -50,	-56,	-44,	-50,	-60	;dB
	        	
giSBW1		ftgen	0, 0, 8, -2,	5, 80,	60,	60,	70,	50	;BAND WIDTH
giSBW2		ftgen	0, 0, 8, -2,	5, 90,	90,	90,	80,	60	;BAND WIDTH
giSBW3		ftgen	0, 0, 8, -2,	5, 120,	100,	100,	100,	170	;BAND WIDTH
giSBW4		ftgen	0, 0, 8, -2,	5, 130,	150,	120,	130,	180	;BAND WIDTH
giSBW5		ftgen	0, 0, 8, -2,	5, 140,	200,	120,	135,	200	;BAND WIDTH

opcode 		fofx5, a, kkki
	kfund,kvowel,koct,ivoice	xin
		
	ivoice		limit		ivoice,0,4	; PROTECT AGAINST OUT OF RANGE VALUES FOR ivoice
	;CREATE A MACRO FOR EACH FORMANT TO REDUCE CODE REPETITION
#define	FORMANT(N)
	#
	invals	table	0, giBF1+(ivoice*15)+$N-1					;NUMBER OF DATA ELEMENTS IN EACH TABLE
	invals		=		invals-1								;
	k$Nform 	tablei		1+(kvowel*invals), giBF1+(ivoice*15)+$N-1	;READ FORMANT FREQUENCY FROM TABLE
	k$Ndb 		tablei		1+(kvowel*invals), giBDb1+(ivoice*15)+$N-1	;READ DECIBEL VALUE FROM TABLE
	k$Namp		=		ampdb(k$Ndb)					;CONVERT TO AN AMPLITUDE VALUE                                                
	k$Nband 	tablei		1+(kvowel*invals), giBBW1+(ivoice*15)+$N-1	;READ BANDWIDTH FROM TABLE
	#
	;EXECUTE MACRO MULTIPLE TIMES
	$FORMANT(1)                                                                                      
	$FORMANT(2)                                                                                      
	$FORMANT(3)                                                                                        
	$FORMANT(4)
	$FORMANT(5)
	;======================================================================================================================================================================
	iris		=		0.003	;GRAIN PULSE RISE TIME
	idur		=		0.02	;GRAIN PULSE DURATION
	idec		=		0.007	;GRAIN PULSE DECAY
	iolaps		=		14850	;MAXIMUM NUMBER OF OVERLAPS (OVERESTIMATE)
	ifna		=		gisine	;FUNCTION TABLE FOR AUDIO CONTAINED WITHIN FOF GRAINS
	ifnb		=		giexp	;FUNCTION TABLE THAT DEFINES THE ATTACK AND DECAY SHAPES OF EACH FOF GRAIN
	itotdur		=		3600	;TOTAL MAXIMUM DURATION OF A NOTE (OVERESTIMATE)
	;FOF===================================================================================================================================================================
	a1 		fof 		k1amp, kfund, k1form, koct, k1band, iris, idur, idec, iolaps, ifna, ifnb, itotdur
	a2 		fof 		k2amp, kfund, k2form, koct, k2band, iris, idur, idec, iolaps, ifna, ifnb, itotdur
	a3 		fof 		k3amp, kfund, k3form, koct, k3band, iris, idur, idec, iolaps, ifna, ifnb, itotdur
	a4 		fof 		k4amp, kfund, k4form, koct, k4band, iris, idur, idec, iolaps, ifna, ifnb, itotdur
	a5 		fof 		k5amp, kfund, k5form, koct, k5band, iris, idur, idec, iolaps, ifna, ifnb, itotdur
	;======================================================================================================================================================================

	;OUT===================================================================================================================================================================
	asig		=		(a1+a2+a3+a4+a5)/5	;MIX THE FIVE FOF STREAMS AND REDUCE AMPLITUDE FIVE-FOLD
			xout		asig			;SEND AUDIO BACK TO CALLER INSTRUMENT
endop

instr	1
	kvibdep		chnget	"vibdep"				;READ WIDGETS...
	imoddel		chnget	"moddel"				;
	imodris		chnget	"modris"				;
	kmodrte		chnget	"modrte"				;
	ktrmdep		chnget	"trmdep"				;
	klevel		chnget	"lev"					;
	kvowel		chnget	"vowel"					;
	koct		chnget	"oct"					;
	ivoice		chnget	"voice"					;

	icps		cpsmidi						;INTERPRET MIDI NOTE AS A FREQUENCY VALUE
	ivel		ampmidi	1					;INTERPRET MIDI VELOCITY AS A VALUE BETWEEN 0 AND 1

	;VIBRATO (PITCH MODULATION)
	kmodenv	linseg	0,0.001+imoddel,0,0.001+imodris,1		;MODULATION DEPTH ENVELOPE - MODULATION CAN BE DELAYED BY THE FIRST ENVELOPE SEGEMENT AND THE RISE TIME IS DEFINED BY THE DURATION OF THE SECOND SEGMENT
	kvib	lfo	kvibdep*kmodenv,kmodrte,0			;VIBRATO FUNCTION

	;TREMOLO (AMPLITUDE MODULATION)
	ktrem		lfo		kmodenv*(ktrmdep/2),kmodrte,0	;TREMOLO LFO FUNCTION
	ktrem		=		(ktrem+0.5) + (ktrmdep * 0.5)	;OFFSET AND RESCALE TREMOLO FUNCTION ACCORDING TO TREMOLO DEPTH WIDGET SETTING 

	kporttime	linseg	0,0.001,0.02				;PORTAMENTO FUNCTION RISES QUICKLY TO A HELD VALUE
	kvowel	portk	kvowel,kporttime				;APPLY PORTAMENTO SMOOTHING TO VOWEL PARAMETER

	asig	fofx5	icps*semitone(kvib), kvowel, koct, ivoice-1	;CALL fofx5 UDO

	kenv	linsegr	0,0.3*(1-ivel)+0.01,1,0.1,0			;AMPLITUDE ENVELOPE - ATTACK TIME IS INFLUENCED BY KEY VELOCITY
	asig	=	asig*kenv*ktrem*ivel*klevel			;APPLY AMPLITUDE CONTROLS: ENVELOPE, TREMOLO, KEY VELOCITY AND LEVEL
		outs	asig,asig					;SEND AUDIO TO OUTPUT
endin

</CsInstruments>

<CsScore>
f 0 [3600*24*7]
</CsScore>

</CsoundSynthesizer>
