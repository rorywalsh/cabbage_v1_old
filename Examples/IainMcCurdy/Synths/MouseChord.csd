WRITTEN BY IAIN MCCURDY, 2010

Mouse Chord
-----------
This csd was originally written for use by someone who only had the use of a head-mouse for music performance.
If the mouse enters one of the coloured boxes the chord indicated by the Roman numeral is played. 
Continuous dynamic control is possible by moving the mouse within the box: the dynamic is lowest at the edges of each box and highest at the centre of each box.
Dynamics are implemented with a changing tone (low-pass filter) as well as changing amplitude.
The user can choose between three different sounds using the radio buttons within the GUI.
Key and tonality (major/minor) can also be selected from within the GUI.
No clicking is required enabling better timing in performance. The GUI window needs to be in focus for this to work.
The main opcode used in sensing mouse position is FLxyin.
Resizing the window by dragging the window edges will upset the functionality of this csd. 
If the window needs to be resized, for example to accomodate use on a netbook, this needs to be done within the code. There are two global variables in the header statement of the orchestra for rescaling the width and the height of the interface easily.
Professional head mouse or iris tracking software can be very expensive and requires practice. 
Camera Mouse is a good free option for Windows users. It can be found at www.cameramouse.org.
iNavigate is a free option for Mac OS 10.5 if the user wants to experiment with this approach.


<Cabbage>
form caption("Mouse Chord") size(1100, 800), pluginID("MChd")
image bounds( 50,200, 200, 200),     colour("Maroon"), shape("rounded"), outlinecolour("white"), line(1) 	; I
image bounds(300,200, 200, 200),     colour("Blue"),   shape("rounded"), outlinecolour("white"), line(1) 	; II
image bounds(550,200, 200, 200),     colour("Yellow"), shape("rounded"), outlinecolour("white"), line(1) 	; III
image bounds(800,200, 200, 200),     colour("Green"),  shape("rounded"), outlinecolour("white"), line(1) 	; IV
image bounds(150,550, 200, 200),     colour("Red"),    shape("rounded"), outlinecolour("white"), line(1) 	; V
image bounds(400,550, 200, 200),     colour("Purple"), shape("rounded"), outlinecolour("white"), line(1) 	; VI
image bounds(650,550, 200, 200),     colour("Tan"),    shape("rounded"), outlinecolour("white"), line(1) 	; VII
label bounds(132,150, 40, 50), text("I"), fontcolour("white")
label bounds(380,150, 45, 50), text("II"), fontcolour("white")
label bounds(615,150, 70, 50), text("III"), fontcolour("white")
label bounds(875,150, 60, 50), text("IV"), fontcolour("white")
label bounds(220,500, 60, 50), text("V"), fontcolour("white")
label bounds(470,500, 60, 50), text("VI"), fontcolour("white")
label bounds(708,500, 80, 50), text("VII"), fontcolour("white")

combobox bounds(  5, 20, 70, 20), channel("key"), value(12), text("B", "A#", "A", "G#", "G", "F#", "F", "E", "D#", "D", "C#", "C")
combobox bounds( 80, 20, 70, 20), channel("oct"), value(3), text("2", "1", "0", "-1", "-2")
combobox bounds(155, 20, 70, 20), channel("ton"), value(2), text("minor", "major")
label    bounds( 20,  5, 30, 11), text("Key"), fontcolour("white")
label    bounds( 85,  5, 50, 11), text("Octave"), fontcolour("white")
label    bounds(158,  5, 55, 11), text("Tonality"), fontcolour("white")

</Cabbage>


<CsoundSynthesizer>

<CsOptions>
-n -dm0
</CsOptions>

<CsInstruments>

sr	= 	44100
ksmps	= 	16
nchnls	= 	2
0dbfs	=	1	;MAXIMUM AMPLITUDE REGARDLESS OF BIT DEPTH


zakinit	4,10
gkRvbSze	init	0.85
gkRvbSnd	init	0.2
gisaw		ftgen	0,0,4096,11,80,1,0.9
giahh55 	ftgen	0,0,4096,10,0.667225,0.194524,0.098683,0.096875,0.021209,0.006311,0.002978,0.001013,0.001249,0.001446,0.002393,0.004826,0.018741,0.012161,0.010480,0.005261,0.004569,0.001376,0.001132,0.003605,0.001846,0.002757,0.005346,0.004712,0.004806,0.002357,0.001109,0.001302,0.001860,0.001054,0.001120,0.001642,0.002240,0.004382,0.005473,0.003755,0.002444,0.002088,0.001822,0.000946,0.000790,0.001222,0.001653,0.001374,0.001401,0.002118,0.002061,0.001470,0.001198,0.001635,0.002387,0.002248,0.001327,0.000951,0.000884,0.000844,0.000805,0.000667,0.000669,0.000701,0.000591,0.000445,0.000367,0.000314,0.000272,0.000225,0.000179,0.000152,0.000164,0.000169,0.000151,0.000144,0.000137,0.000121,0.000115,0.000123,0.000125,0.000116,0.000102,0.000095,0.000102,0.000120,0.000135,0.000132,0.000117,0.000100,0.000084,0.000074,0.000072,0.000075,0.000077,0.000079,0.000083,0.000083,0.000081,0.000082,0.000084,0.000081,0.000074,0.000063

gicos		ftgen	0,0,4096,11,1		;COSINE WAVE (USED BY THE LFOS)
gieqffn		ftgen	0,0,4097,7,-1,4096,1
gieqlfn		ftgen	0,0,4097,7,-1,4096,1
gieqqfn		ftgen	0,0,4097,7,-1,4096,1

;DEFINE CHORDS I-VII IN PCH FORMAT FOR MAJOR KEY
gichord1	ftgen	0,0,4,-2,8.00,8.04,8.07,9.00
gichord2	ftgen	0,0,4,-2,8.02,8.05,8.09,9.02
gichord3	ftgen	0,0,4,-2,8.04,8.07,8.11,9.04
gichord4	ftgen	0,0,4,-2,8.00,8.05,8.09,9.00
gichord5	ftgen	0,0,4,-2,7.11,8.02,8.07,8.11
gichord6	ftgen	0,0,4,-2,8.00,8.04,8.09,9.00
gichord7	ftgen	0,0,4,-2,8.02,8.05,8.11,9.02

;DEFINE CHORDS I-VII IN PCH FORMAT FOR MINOR KEY
gichord1m	ftgen	0,0,4,-2,8.00,8.03,8.07,9.00
gichord2m	ftgen	0,0,4,-2,8.02,8.05,8.08,9.02
gichord3m	ftgen	0,0,4,-2,8.03,8.07,8.11,9.03
gichord4m	ftgen	0,0,4,-2,8.05,8.08,8.00,9.05
gichord5m	ftgen	0,0,4,-2,8.07,8.11,8.02,9.07
gichord6m	ftgen	0,0,4,-2,8.08,8.00,8.03,9.08
gichord7m	ftgen	0,0,4,-2,8.02,8.05,8.11,9.02


instr	1

 gkMOUSE_X	chnget	"MOUSE_X"
 gkMOUSE_Y	chnget	"MOUSE_Y"
 gkton		chnget	"ton"
 gkkey		chnget	"key"
 gkkey		=	11 - (gkkey - 1)
 gkoct		chnget	"oct"
 gkoct		=	3-gkoct

#define CHORD_BOX(N'X'Y)
 #
 ix$N	=	$X
 iy$N	=	$Y
 if (gkMOUSE_X > ix$N)  && (gkMOUSE_X < (ix$N+200))  && (gkMOUSE_Y > iy$N)  && (gkMOUSE_Y < (iy$N+200)) then
  kinside$N	=	1
 else
  kinside$N	=	0
 endif
 ktrig$N	changed	kinside$N	;OUTPUT A MOMENTARY '1' VIA ktrig$N IF kinside$N CHANGES, I.E. IF THE MOUSE ENTERS OR LEAVES THIS CHORD BOX
 if	ktrig$N=1	then	;IF MOUSE HAS ENTERED OR LEFT THIS CHORD BOX (I.E. IF ktrig$N has taken a value of 1)...
 	if	kinside$N=1	then	;IF MOUSE IS IN THE BOX...
 		event	"i", 2, 0, 3600, $N	;PLAY A LONG NOTE INSTRUMENT $N
 	elseif	kinside$N=0	then	;IF MOUSE HAS JUST LEFT THE BOX...
 		turnoff2	2, 0, 1	;TURNOFF INSTRMENT $N ALLOWING RELEASE STAGES TO BE COMPLETED
 	endif				;END OF CONDITIONAL BRANCHING
 endif
 if kinside$N = 1 then
  kx$N	mirror	(gkMOUSE_X - ix$N) / 200, 0, 0.5 
  ky$N	mirror	(gkMOUSE_Y - iy$N) / 200, 0, 0.5 
  k$N	=	kx$N + ky$N
  zkw	k$N, $N-1	;WRITE LOCATION VALUE TO ZAK VARIABLE
 endif
 #

$CHORD_BOX(1'50'200)
$CHORD_BOX(2'300'200)
$CHORD_BOX(3'550'200)
$CHORD_BOX(4'800'200)
$CHORD_BOX(5'150'550)
$CHORD_BOX(6'400'550)
$CHORD_BOX(7'650'550)

endin

instr	2
	ipch2	table	1, gichord1 + p4 - 1	;PITCH OF FIRST NOTE OF CHORD
	ipch3	table	2, gichord1 + p4 - 1	;PITCH OF SECOND NOTE OF CHORD
	ipch4	table	3, gichord1 + p4 - 1	;PITCH OF THIRD NOTE OF CHORD
	aenv	expsegr	0.001,0.2,1,1,0.001	;AMPLITUDE ENVELOPE
	
	;DEFINE MACRO OF CODE TO PRODUCE EACH NOTE OF THE CHORD
#define	NOTE(N)
	#
	
	if	i(gkton)=2	then	;IF TONALITY IS 'MAJOR'...
		ipch$N	table	$N-1, gichord1 + p4 - 1	;DEFINE PITCH (PCH FORMAT) FOR THIS NOTE (MAJOR)
	else				;OTHERWISE TONALITY MUST BE MINOR...
		ipch$N	table	$N-1, gichord1m + p4 - 1;DEFINE PITCH (PCH FORMAT) FOR THIS NOTE (MINOR)
	endif				;ENDO OF CONDITIONAL BRANCHING
	kcps$N		=	cpspch(ipch$N + (i(gkkey)*0.01) + i(gkoct))
 	;TWO ITERATIONS OF oscbnk ARE CREATED, ONE FOR EACH OF THE STEREO AUDIO CHANNELS. THE OUTPUTS WILL BE DIFFERENT AS THE RANDOM NUMBER GENERATORS WILL BE SEEDED BY THE SYSTEM CLOCK
 	;OUTPUT	OPCODE  CPS    | AMD  |    FMD       | PMD | OVERLAPS   | SEED  | L1MINF  | L1MAXF  | L2MINF  | L2MAXF  | LFOMODE | EQMINF  | EQMAXF | EQMINL | EQMAXL | EQMINQ | EQMAXQ  | EQMODE | KFN  | L1FN | L2FN | EQFFN  | EQLF   |  EQQFN |  TABL  | OUTFN
 	aL$N	oscbnk	kcps$N,   0,   0.005*kcps$N,    0,     10,        rnd(1),   0,         1,       0,        0,       238,      0,       8000,      1,       1,       1,       1,       -1, gisaw,  gicos, gicos, gieqffn, gieqlfn, gieqqfn
 	aR$N	oscbnk	kcps$N,   0,   0.005*kcps$N,    0,     10,        rnd(1),   0,        -1,       0,        0,       238,      0,       8000,      1,       1,       1,       1,       -1, gisaw,  gicos, gicos, gieqffn, gieqlfn, gieqqfn


	#
	;EXECUTE MACRO FOR EACH NOTE
	$NOTE(1)
	$NOTE(2)
	$NOTE(3)
	$NOTE(4)
	
	;SUM (MIX) THE FOUR NOTES
	aL	sum	aL1, aL2, aL3, aL4
	aR	sum	aR1, aR2, aR3, aR4

	kamp	zkr	p4 - 1		;READ AMPLITUDE FROM ZAK VARIABLE
	kamp	port	kamp, 0.05	;APPLY PORTAMENTO (TO PREVENT QUANTISATION / ZIPPER NOISE)
	aamp	interp	kamp		;INTERPOLATE TO CREATE AN AUDIO RATE VERSION OF THIS VARIABLE
	
	kcfoct	=	(8*kamp)+6	;DEFINE A FILTER CUTOFF FREQUENCY WHICH IS RELATED TO DISTANCE FROM THE CENTRE OF THE BOX
	
	aL	tonex	aL, cpsoct(kcfoct),2	;APPLY LOW PASS FILTERING (TONE CONTROL)
	aR	tonex	aR, cpsoct(kcfoct),2	;APPLY LOW PASS FILTERING (TONE CONTROL)
	
	aL	=	aL * aenv * aamp * 0.3	;SCALE AUDIO SIGNAL WITH AMPLITIUDE ENVELOPE
	aR	=	aR * aenv * aamp * 0.3
	outs	aL, aR
		zawm	aL * gkRvbSnd, 0                      ;SEND SOME OF THE AUDIO TO THE REVERB VIA ZAK PATCHING (LEFT CHANNEL) 
		zawm	aR * gkRvbSnd, 1                      ;SEND SOME OF THE AUDIO TO THE REVERB VIA ZAK PATCHING (RIGHT CHANNEL)


endin

instr	1000	;REVERB
	ainL	zar	0	;READ IN AUDIO FROM ZAK CHANNELS
	ainR	zar	1	;READ IN AUDIO FROM ZAK CHANNELS
	denorm		ainL, ainR		;...DENORMALIZE BOTH CHANNELS OF AUDIO SIGNAL
		arvbL, arvbR 	reverbsc 	ainL, ainR, gkRvbSze, 10000	;USE reverbsc OPCODE
	outs	arvbL, arvbR					;SEND AUDIO TO OUTPUTS
	zacl	0,3						;CLEAR ZAK AUDIO CHANNELS
endin


</CsInstruments>

<CsScore>
i 1 0 3600	;INSTRUMENT 1 PLAYS A NOTE FOR 1 HOUR (AND KEEPS REAL-TIME PERFORMANCE GOING)
i 1000 0 3600	; reverb
</CsScore>

</CsoundSynthesizer>