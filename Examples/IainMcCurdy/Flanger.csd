<Cabbage>
form caption("Flanger") size(380, 120)
image pos(10, 0), size(360, 90), line(1), colour("darkgreen"), shape("rounded"), outline("white"), line(4) 
rslider bounds(15, 10, 70, 70), text("Rate"), channel("rate"), range(0, 1, 0.1)
rslider bounds(85, 10, 70, 70), text("Depth"), channel("depth"), range(0, 1, 0.5)
rslider bounds(155, 10, 70, 70), text("Delay"), channel("delay"), range(0, 1, 0.1)
rslider bounds(225, 10, 70, 70), text("Feedback"), channel("fback"), range(-1, 1, 0.9)
rslider bounds(295, 10, 70, 70), text("Level"), channel("level"), range(0, 1, 1)

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

opcode	Flanger,a,akkkk
	ain,krate,kdepth,kdelay,kfback	xin					;READ IN INPUT ARGUMENTS
	krate		expcurve	krate,50				;CREATE AN EXPONENTIAL REMAPPING OF krate
	krate		scale		krate,14,0.001				;RESCALE VALUE	
	kdelay		expcurve	kdelay,200				;CREATE AN EXPONENTIAL REMAPPING OF kdelay
	kdelay		scale		kdelay,0.1,0.00015			;RESCALE VALUE	
	ilfoshape	ftgentmp	0, 0, 131072, 19, 0.5, 1, 180, 1	;U-SHAPE PARABOLA FOR LFO
	kporttime	linseg		0, 0.001, 0.1 				;USE OF AN ENVELOPE VALUE THAT QUICKLY RAMPS UP FROM ZERON TO THE REQUIRED VALUE PREVENTS VARIABLES GLIDING TO THEIR REQUIRED VALUES EACH TIME THE INSTRUMENT IS STARTED
	kdlt		portk		kdelay, kporttime 			;PORTAMENTO IS APPLIED TO A VARIABLE. A NEW VARIABLE 'kdlt' IS CREATED.
	adlt		interp		kdlt					;A NEW A-RATE VARIABLE 'adlt' IS CREATED BY INTERPOLATING THE K-RATE VARIABLE 'kdlt'
	kdep		portk		kdepth*0.01, kporttime 			;PORTAMENTO IS APPLIED TO A VARIABLE. A NEW VARIABLE 'kdep' IS CREATED.
	amod		oscili		kdep, krate, ilfoshape			;OSCILLATOR THAT MAKES USE OF THE POSITIVE DOMAIN ONLY U-SHAPE PARABOLA WITH FUNCTION TABLE NUMBER ilfoshape
	adlt		sum		adlt, amod				;STATIC DELAY TIME AND MODULATING DELAY TIME ARE SUMMED
	adelsig		flanger 	ain, adlt, kfback , 1.2			;FLANGER SIGNAL CREATED
	aout		sum		ain*0.5, adelsig*0.5			;CREATE DRY/WET MIX 
			xout		aout					;SEND AUDIO BACK TO CALLER INSTRUMENT
endop

instr 1
krate chnget "rate"
kdepth chnget "depth"
kdelay chnget "delay"
kfback chnget "fback"
klevel chnget "level"
a1,a2	ins
a1	Flanger	a1,krate,kdepth,kdelay,kfback	
a2	Flanger	a2,krate,kdepth,kdelay,kfback	
a1	=	a1 * klevel
a2	=	a2 * klevel
	outs	a1,a2
endin

</CsInstruments>

<CsScore>
i 1 0 [60*60*24*7]
</CsScore>

</CsoundSynthesizer>