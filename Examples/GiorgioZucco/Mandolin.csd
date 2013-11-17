<Cabbage>
form caption("Mandolin") size(702, 215), colour("black"),pluginID("man1") 
rslider bounds(15, 10, 100, 100) channel("semi"), range(-12,12,-12), caption("Coarse"), colour("LightSkyBlue   ")
rslider bounds(120, 10, 100, 100) channel("size"), range(0.2,2,1.35), caption("Size"), colour("LightSkyBlue   ")
rslider bounds(225, 10, 100, 100) channel("detune"), range(.8,1,1), caption("Detune"), colour("LightSkyBlue   ")
rslider bounds(330, 10, 100, 100) channel("gain"), range(0.9,1,0.9), caption("Gain"), colour("LightSkyBlue   ")
rslider bounds(435, 10, 100, 100) channel("filter"), range(100,20000,12000), caption("Lowpass"), colour("LightSkyBlue   ")

rslider bounds(540,50, 60, 60), text("Rev level"), colour("LightSkyBlue   "), channel("level"), range(0,1,.8)
rslider bounds(610, 50, 60, 60), text("Rev size"), colour("LightSkyBlue   "), channel("revsize"), range(.1,.99,.7)
checkbox  bounds(550,10, 160, 20),channel("onoff"), text("Reverb On/Off"), value(0)
keyboard pos(10,120), size(680, 60)

</Cabbage>
<CsoundSynthesizer>
<CsOptions>
-d -n -+rtmidi=NULL -M0 
</CsOptions>
<CsInstruments>

sr = 44100
ksmps = 32
nchnls = 2
0dbfs = 1

;Author: Giorgio Zucco (2012)

ga1	init	0


instr	1
gksemi	chnget	"semi"
gksize	chnget	"size"
gkdetune	chnget	"detune"
gkgain	chnget	"gain"
gkfilt	chnget	"filter"
iamp	ampmidi	1
inote	notnum
kfreq	=	cpsmidinn(inote+int(gksemi))
amand	mandol	iamp,kfreq,.4,gkdetune,gkgain,gksize,2,60
afilt	clfilt	amand,gkfilt,0,20
aout	clip	afilt,0,0dbfs
kenv	mxadsr	.001,.8,.5,.2
outs	aout*kenv,aout*kenv
vincr	ga1,aout*kenv

endin

instr	10	;reverb
ktrig	chnget	"onoff"
klevel	chnget	"level"
ksize	chnget	"revsize"
if	ktrig	=	1	then
	
al,ar	reverbsc	ga1,ga1,ksize,12000,sr,0
asum1	dcblock2	al
asum2	dcblock2	ar
else
asum1	=	0
asum2	=	0
endif

outs	asum1*klevel,asum2*klevel
clear	ga1
endin


</CsInstruments>
<CsScore>
f2 0 8192 1 "mandpluk.aiff" 0 0 0

f0	36000

i10	0	36000
</CsScore>
</CsoundSynthesizer>
