<Cabbage>
form caption("Time-domain Freq-shift") size(360, 120)
image bounds( 0,  0, 360, 90), colour("darkslategrey"), outline("white"), line(4)
rslider bounds(  5, 10, 70, 70), text("Mix"), channel("mix"), range(0, 1, 0.5)
rslider bounds( 75, 10, 70, 70), text("Freq."), channel("freq"), range(-1000, 1000, -50)
rslider bounds(145, 10, 70, 70), text("Mult"), channel("mult"), range(-1, 1, 0.1)
rslider bounds(215, 10, 70, 70), text("Feedback"), channel("fback"), range(0, .75, .6)
rslider bounds(285, 10, 70, 70), text("Level"), channel("level"), range(0, 1, 1)

</Cabbage>
<CsoundSynthesizer>
<CsOptions>
-d -n
</CsOptions>
<CsInstruments>
sr = 44100
ksmps = 32
nchnls = 2
0dbfs = 1

;Iain McCurdy, 2012
;http://iainmccurdy.org/csound.html
;Frequency shifter effect based around the hilbert filter.

opcode	FreqShifter,a,akkkk
	adry,kmix,kfreq,kmult,kfback	xin			;READ IN INPUT ARGUMENTS
	iWet	ftgentmp	0,0,1024,-7,0,512,1,512,1	;RESCALING FUNCTION FOR WET LEVEL CONTROL
	iDry	ftgentmp	0,0,1024,-7,1,512,1,512,0	;RESCALING FUNCTION FOR DRY LEVEL CONTROL
	isine	ftgentmp	0,0,4096,10,1			;A SINE WAVE SHAPE
	kWet	table	kmix, iWet, 1				;RESCALE WET LEVEL CONTROL ACCORDING TO FUNCTION TABLE giWet
	kDry	table	kmix, iDry, 1				;RESCALE DRY LEVEL CONTROL ACCORDING TO FUNCTION TABLE giWet
	aFS	init	0					;INITILISE FEEDBACK SIGNAL (FOR FIRST K-PASS)
	ain	=	adry + (aFS * kfback)			;ADD FEEDBACK SIGNAL TO INPUT (AMOUNT OF FEEDBACK CONTROLLED BY 'Feedback Gain' SLIDER)
	areal, aimag hilbert ain				;HILBERT OPCODE OUTPUTS TWO PHASE SHIFTED SIGNALS, EACH 90 OUT OF PHASE WITH EACH OTHER
	kporttime	linseg	0,0.001,0.02
	kfshift	portk	kfreq*kmult, kporttime
	;QUADRATURE OSCILLATORS. I.E. 90 OUT OF PHASE WITH RESPECT TO EACH OTHER
	;OUTUTS	OPCODE	AMPLITUDE | FREQ. | FUNCTION_TABLE | INITIAL_PHASE (OPTIONAL;DEFAULTS TO ZERO)
	asin 	oscili       1,    kfshift,     isine,          0
	acos 	oscili       1,    kfshift,     isine,          0.25	
	;RING MODULATE EACH SIGNAL USING THE QUADRATURE OSCILLATORS AS MODULATORS
	amod1	=		areal * acos
	amod2	=		aimag * asin	
	;UPSHIFTING OUTPUT
	aFS	= (amod1 - amod2)
	aout	sum	aFS*kWet, adry*kDry		;CREATE WET/DRY MIX
		xout	aout				;SEND AUDIO BACK TO CALLER INSTRUMENT
endop

instr 1
kmix chnget "mix"
kfreq chnget "freq"
kmult chnget "mult"
kfback chnget "fback"
klevel chnget "level"
a1,a2	ins
a1	FreqShifter	a1,kmix,kfreq,kmult,kfback	
a2	FreqShifter	a2,kmix,kfreq,kmult,kfback	
a1	=	a1 * klevel
a2	=	a2 * klevel
	outs	a1,a2
endin

</CsInstruments>
<CsScore>
i 1 0 [60*60*24*7]
</CsScore>
</CsoundSynthesizer>