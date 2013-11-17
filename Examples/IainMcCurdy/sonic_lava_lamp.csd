<Cabbage>
form caption("Sonic Lava Lamp") size(516, 460)
xypad bounds(5, 0, 250, 210), channel("LFOspeed", "LFO_OS"), rangex(0, 1, 1), rangey(5, 11, 8), xyautoindex(0), rangex(1), rangey(6),  colour(255, 0, 0, 255), fontcolour(100, 149, 237, 255), 
xypad bounds(255, 0, 250, 210), channel("LFOshape", "LFOdepth"), rangex(0, 1, 0), rangey(0.25, 4, 1), xyautoindex(0), rangex(1), rangey(3.75),  colour(0, 255, 0, 255), fontcolour(100, 149, 237, 255), 
xypad bounds(5, 210, 250, 210), channel("pan", "amp"), rangex(0, 1, 0.5), rangey(0, 1, 0.5), xyautoindex(0), rangex(1), rangey(1),  colour(255, 255, 0, 255), fontcolour(100, 149, 237, 255), 
xypad bounds(255, 210, 250, 210), channel("DelTim", "DelSnd"), rangex(0, 1, 0.5), rangey(0, 1, 0.5), xyautoindex(0), rangex(1), rangey(1),  colour(0, 255, 255, 255), fontcolour(100, 149, 237, 255), 
image bounds(5, 425, 405, 20), colour(75, 85, 90, 100), plant("credit"), line(0){
label bounds(0.03, 0.1, .6, .7), text("Author: Iain McCurdy |2012|"), colour("white")
}
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
giSawUp		ftgen	0, 0, 4096, -7, -1, 4096, 1
giSawDn		ftgen	0, 0, 4096, -7, 1, 4096, -1
giSq		ftgen	0, 0, 4096, -7, 1, 2048, 1, 0, -1, 2048, -1
giMorfTab	ftgen	0, 0, 4096, -7, 1, 2048, 1, 0, -1, 2048, -1
giTabs4Morf	ftgen	0, 0, 4, -2, giSawUp, giSawDn, giSq, giSq
giAmpCurve	ftgen	0, 0, 16, -16,   1, 6, 0, 1,   5, -4, 0.2,   5, 0, 0.2

opcode	PingPongDelay, aa,aakk
	ainL,ainR,ktime,kfeedback	xin
	atime		interp	ktime					;INTERPOLATED A-RATE VERSION OF DELAY TIME
	aL_OS		vdelay	ainL,(atime*1000)/2,(10*1000)/2	;DELAYED OFFSET OF LEFT CHANNEL (FIRST 'PING')
	;LEFT CHANNEL
	abuf		delayr	10					;ESTABLISH DELAY BUFFER
	aDelL		deltapi	atime					;TAP BUFFER
			delayw	aL_OS+(aDelL*kfeedback)			;WRITE INPUT AUDIO INTO BUFFER
	;RIGHT CHANNEL
	abuf		delayr	10					;ESTABLISH DELAY BUFFER
	aDelR		deltapi	atime					;TAP BUFFER
			delayw	ainR+(aDelR*kfeedback)			;WRITE INPUT AUDIO INTO BUFFER
			xout	aDelL+aL_OS,aDelR
endop
instr	1
	kporttime	linseg	0,0.01,0.03
	kLFOspeed	chnget	"LFOspeed"
	kLFOspeed	expcurve	kLFOspeed,6
	kLFOspeed	scale	kLFOspeed, 40,0.05
	kLFO_OS		chnget	"LFO_OS"
	kLFOshape	chnget	"LFOshape"
	kLFOdepth	chnget	"LFOdepth"
	kamp		chnget	"amp"
	kpan		chnget	"pan"
	kDelTim		chnget	"DelTim"
	kDelTim		expcurve	kDelTim,6
	kDelTim		scale	kDelTim, 2,0.5
	kDelSnd		chnget	"DelSnd"
	kDelSnd		portk	kDelSnd,kporttime*3
	kLFOdepth	portk	kLFOdepth,kporttime
	kLFO_OS		portk	kLFO_OS,kporttime
	kLFOshape	portk	kLFOshape,kporttime
	kamp		portk	kamp,kporttime
	kamp		expcurve	kamp,2
	kpan		portk	kpan,kporttime
			ftmorf	kLFOshape*3, giTabs4Morf, giMorfTab
	aoctLFO		oscili	kLFOdepth,kLFOspeed,giMorfTab
	aoct		=	aoctLFO+kLFO_OS
	koct		downsamp	aoct
	kAmpCurve	tablei	koct,giAmpCurve
	kcps		=	cpsoct(koct)
	acps		interp	kcps
	asig		oscili	kAmpCurve/3,acps,gisine
	;asig		vco2	kAmpCurve/3,kcps,0
	;asig		vco2	kAmpCurve/3,kcps,2,0.5

	kcf		limit	cpsoct(kLFO_OS+kLFOdepth), 20, sr/3
	asig		butlp	asig, kcf
	aL,aR		pan2	asig,kpan
	aRvbL,aRvbR	reverbsc	aL,aR,0.8,4000
	aL		sum	(aL*kamp),(aRvbL*0.1)
	aR		sum	(aR*kamp),(aRvbR*0.1)
	kDelTim		portk	kDelTim,kporttime*10
	aDlyL,aDlyR	PingPongDelay	aL*kDelSnd,aR*kDelSnd,2*kDelTim,0.6
	aL		sum	aL,(aDlyL*0.5)
	aR		sum	aR,(aDlyR*0.5)

			outs	aL, aR
endin

</CsInstruments>

<CsScore>
i 1 0 [3600*24*7]
</CsScore>

</CsoundSynthesizer>
