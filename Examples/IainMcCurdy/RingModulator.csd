<Cabbage>
form caption("Ring Modulator") size(300, 120)
image pos(0, 0), size(300, 90), colour("black"), shape("rounded"), outline("lime"), line(2) 
rslider bounds(10, 11, 70, 70), text("Mix"), channel("mix"), range(0, 1, 1)
rslider bounds(80, 11, 70, 70), text("Freq."), channel("freq"), range(10, 5000, 800, 0.5)
rslider bounds(150, 11, 70, 70), text("Env."), channel("env"), range(0, 1, 0.5)
rslider bounds(220, 11, 70, 70), text("Level"), channel("level"), range(0, 1, 1)
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

;Author: Iain McCurdy (2012)
;http://iainmccurdy.org/csound.html


opcode	RingModulator,a,akkk
	ain,kmix,kfreq,kenv	xin					;READ IN INPUT ARGUMENTS
	iWet	ftgentmp	0,0,1024,-7,0,512,1,512,1		;RESCALING FUNCTION FOR WET LEVEL CONTROL
	iDry	ftgentmp	0,0,1024,-7,1,512,1,512,0		;RESCALING FUNCTION FOR DRY LEVEL CONTROL
	isine	ftgentmp	0,0,4096,10,1				;SINE WAVE
	kWet	table	kmix, iWet, 1					;RESCALE WET LEVEL CONTROL ACCORDING TO FUNCTION TABLE iWet
	kDry	table	kmix, iDry, 1					;RESCALE DRY LEVEL CONTROL ACCORDING TO FUNCTION TABLE iDry
	kporttime	linseg	0,0.001,0.02				;PORTAMENTO VARIABLE
	kModFrq	portk	kfreq, kporttime				;SMOOTH VARIABLE CHANGES
	aFollow		follow2		ain, 0.01, 0.1			;AMPLITUDE FOLLOWING AUDIO SIGNAL
	kFollow		downsamp	aFollow
	kFollow	logcurve	kFollow/0dbfs,20
	kModFrq	=	kModFrq + (cpsoct(kFollow*kenv*30))     	;CREATE A LEFT CHANNEL MODULATING FREQUENCY BASE ON THE STATIC VALUE CREATED BY kfreq AND THE AMOUNT OF DYNAMIC ENVELOPE FOLLOWING GOVERNED BY kenv
	aMod	poscil	1, kModFrq, isine  				;CREATE RING MODULATING SIGNAL
	aout	sum	ain*kDry, ain*aMod*kWet				;MIX DRY AND WET SIGNALS
		xout	aout						;SEND AUDIO BACK TO CALLER INSTRUMENT
endop

instr 1
kmix chnget "mix"
kfreq chnget "freq"
kenv chnget "env"
klevel chnget "level"
a1,a2	ins
a1	RingModulator	a1,kmix,kfreq,kenv	
a2	RingModulator	a2,kmix,kfreq,kenv	
a1	=	a1 * klevel
a2	=	a2 * klevel
	outs	a1,a2
endin

</CsInstruments>

<CsScore>
i 1 0 [60*60*24*7]
</CsScore>

</CsoundSynthesizer>