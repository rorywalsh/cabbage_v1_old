<Cabbage>
form size(745, 340), caption("3 Carrier FM Synthesiser"), pluginID("rfm1")

image bounds(5, 5, 240, 80), colour(0, 0, 0, 0), outlinecolour("white"), outlinethickness(2), plant("5RSliders1"){ 
label bounds(0, 5, 240, 12), align("centre"), text("Carrier One"),
rslider bounds(.0, .3, .6, .6), text("Att."), channel("att1"), range(0.001, 3, .001)
rslider bounds(.2, .3, .6, .6), text("Dec."), channel("dec1"), range(0, 2, 0)
rslider bounds(.4, .3, .6, .6), text("Sus."), channel("sus1"), range(0, 1, 1)
rslider bounds(.59, .3, .6, .6), text("Rel."), channel("rel1"), range(0, 3, 0)
rslider bounds(.78, .3, .6, .6), text("Oct."), channel("oct1"), range(1, 10, 1, 1, 1)
}

image bounds(250, 5, 240, 80), colour(0, 0, 0, 0), outlinecolour("white"), outlinethickness(2), plant("5RSliders2"){ 
label bounds(0, 5, 240, 12), align("centre"),  text("Carrier Two"),
rslider bounds(.0, .3, .6, .6), text("Att."), channel("att2"), range(0.001, 3, .001)
rslider bounds(.2, .3, .6, .6), text("Dec."), channel("dec2"), range(0, 2, 0)
rslider bounds(.4, .3, .6, .6), text("Sus."), channel("sus2"), range(0, 1, 1)
rslider bounds(.59, .3, .6, .6), text("Rel."), channel("rel2"), range(0 ,3, 0)
rslider bounds(.78, .3, .6, .6), text("Oct."), channel("oct2"), range(1, 10, 2, 1, 1)
}

image bounds(495, 5, 240, 80),colour(0, 0, 0, 0), outlinecolour("white"), outlinethickness(2), plant("5RSliders3"){ 
label bounds(0, 5, 240, 12), align("centre"),  text("Carrier Three"), 
rslider bounds(.0, .3, .6, .6), text("Att."), channel("att3"), range(0.001, 3, .001)
rslider bounds(.2, .3, .6, .6), text("Dec."), channel("dec3"), range(0, 2, 0)
rslider bounds(.4, .3, .6, .6), text("Sus."), channel("sus3"), range(0, 1, 1)
rslider bounds(.59, .3, .6, .6), text("Rel."), channel("rel3"), range(0, 3, 0)
rslider bounds(.78, .3, .6, .6), text("Oct."), channel("oct3"), range(1, 10, 4, 1, 1)
}

image bounds(5, 90, 240, 80), plant("5RSliders4"), colour(0, 0, 0, 0), outlinecolour("white"), outlinethickness(2){ 
label bounds(0, 5, 240, 12), align("centre"),  text("Modulator"),
rslider bounds(.0, .3, .6, .6), text("Att."), channel("att4"), range(0.001, 3, .001)
rslider bounds(.2, .3, .6, .6), text("Dec."), channel("dec4"), range(0, 2, 0)
rslider bounds(.4, .3, .6, .6), text("Sus."), channel("sus4"), range(0, 1, 1)
rslider bounds(.59, .3, .6, .6), text("Rel."), channel("rel4"), range(0, 3, 0)
rslider bounds(.78, .3, .6, .6), text("Dev."), channel("dev"), range(0, 2000, 2000)
}

image bounds(250, 90, 240, 80),  plant("4rSliders"), colour(0, 0, 0, 0), outlinecolour("white"), outlinethickness(2),{ 
label bounds(0, 5, 240, 12), align("centre"), text("Automated Parametric EQ"),
rslider bounds(.0, .3, .6, .6), text("Range"), channel("range"), range(0, 2000, 1000)
rslider bounds(.2, .3, .6, .6), text("Q"), channel("q"), range(1, 20, 5)
rslider bounds(.4, .3, .6, .6), text("Freq"), channel("LFOFreq"), range(0, 10, 1)
combobox bounds(.61, .35, .35, .2), channel("waveType"), items("Sine", "Wobble 1", "Wobble 2", "Wobble 3", "Wobble 4", "Wobble 5", "Wobble 6",  "Rand")
checkbox bounds(.63, .65, .35, .2), text("LFO On"), colour:1("white"), channel("lfoOn")
}

image bounds(495, 90, 170, 80), plant("presence"), colour(0, 0, 0, 0), outlinecolour("white"), outlinethickness(2),{ 
label bounds(0, 5, 170, 12), align("centre"), text("Presence -k-rate-"),   
rslider bounds(.01, .3, .6, .6), text("Depth"), colour("white"), channel("depth"), range(0, 10, 0.2)
rslider bounds(.32, .3, .6, .6), text("Rate"), colour("white"), channel("rate"), range(0, 10, 0.2)
rslider bounds(.64, .3, .6, .6), text("Diff"), colour("white"), channel("diff"), range(0, 1, .8)
}

image bounds(670, 90, 65, 80), colour(0, 0, 0, 0), outlinecolour("white"), outlinethickness(2), plant("carriers"){ 
label bounds(0, 5, 65, 12), align("centre"), text("Carriers"), 
rslider bounds(0.17, .33, .7, .7), channel("numCars"), colour("white"), trackercolour(0, 118, 36), range(1, 3, 3, 1, 1)
}
checkbox bounds(10, 286, 100, 20), text("Polyphonic"), channel("poly")

keyboard bounds(10, 180, 725, 100), value(16)

</Cabbage>
<CsoundSynthesizer>
<CsOptions>
-n -d -+rtmidi=NULL -M0 --midi-key-cps=5 --midi-velocity-amp=4
</CsOptions>
<CsInstruments>
sr = 44100
ksmps = 64
nchnls = 2
0dbfs=1

gamix init 0
gabal init 0
gaPresL init 0
gaPresR init 0

instr 1
iatt1 chnget "att1"
idec1 chnget "dec1"
isus1 chnget "sus1"
irel1 chnget "rel1"
ioct1 chnget "oct1"

iatt2 chnget "att2"
idec2 chnget "dec2"
isus2 chnget "sus2"
irel2 chnget "rel2"
ioct2 chnget "oct2"

iatt3 chnget "att3"
idec3 chnget "dec3"
isus3 chnget "sus3"
irel3 chnget "rel3"
ioct3 chnget "oct3"

iatt4 chnget "att4"
idec4 chnget "dec4"
isus4 chnget "sus4"
irel4 chnget "rel4"
idev chnget "dev"

kLFO chnget "lfoOn"
kNumCars chnget "numCars"

if(kLFO==0) then
	turnoff2 1000, 0, 0
	event_i "i", 1000, 0, 3600
endif

kCarEnv1 madsr iatt1, idec1, isus1, irel1
kCarEnv2 madsr iatt2, idec2, isus2, irel2
kCarEnv3 madsr iatt3, idec3, isus3, irel3

kModEnv madsr iatt4, idec4, isus4, irel4
aModulator oscil idev*kModEnv, p5/2, 1 

;set number of carriers
if(kNumCars==1) then
aCar1 oscil (kCarEnv1*p4)/5, p5+(aModulator*(ioct1)), 1
aCar2 = 0
aCar3 = 0
elseif(kNumCars==2) then
aCar1 oscil (kCarEnv1*p4)/5, p5+(aModulator*(ioct1)), 1
aCar2 oscil (kCarEnv2*p4)/5, p5+(aModulator*(ioct2)), 1
aCar3 = 0
elseif(kNumCars==3) then
aCar1 oscil (kCarEnv1*p4)/5, p5+(aModulator*(ioct1)), 1
aCar2 oscil (kCarEnv2*p4)/5, p5+(aModulator*(ioct2)), 1
aCar3 oscil (kCarEnv3*p4)/5, p5+(aModulator*(ioct3)), 1
endif
gamix = aCar1+aCar2+aCar3

kPoly chnget "poly"
if(kPoly==1) then
outs gamix/3, gamix/3
endif

endin

;filter
instr 1000
kRange chnget "range"
kQ chnget "q"
klfoFreq chnget "LFOFreq"
kType chnget "waveType"
kType = (kType <1 ? 1 : kType)

if(kType<8) then
	k1 oscilikt kRange, klfoFreq, kType
elseif(kType=8) then
	k1 randi kRange, klfoFreq
endif

aflt butterbp gamix, 50+abs(k1), abs(k1)/kQ
abal balance aflt, gamix 
gabal = abal

endin

instr	1500
kdepth chnget "depth"
krate chnget "rate"
kdiff chnget "diff"

asigr = gabal+gamix
asigl = gabal+gamix
;Based on UDO by Giorgio Zucco (2012)
k1ch  randi       kdepth/2,krate,1
ar1   vdelay asigr,kdepth/2+k1ch,10
k2ch  randi       kdepth/2,krate*0.9,.2
ar2   vdelay asigr,kdepth/2+k2ch,10
k3ch  randi       kdepth/2,krate*1.1,.2
ar3   vdelay asigr,kdepth/2+k3ch,10
k4ch  randi       kdepth/2,krate*1.3,.1
ar4   vdelay asigr,kdepth/2+k4ch,10

kdepthl = kdepth*kdiff
krate1 = krate*kdiff

k1chl  randi       kdepthl/2,krate1,1
ar1l   vdelay asigl,kdepthl/2+k1chl,10
k2chl  randi       kdepthl/2,krate1*0.95,.2
ar2l   vdelay asigl,kdepthl/2+k2chl,10
k3chl  randi       kdepthl/2,krate1*1.15,.2
ar3l   vdelay asigl,kdepthl/2+k3chl,10
k4chl  randi       kdepthl/2,krate1*1.35,.1
ar4l   vdelay asigl,kdepthl/2+k4chl,10


aL   =    (asigr+ar1+ar2+ar3+ar4)/2
aR  =    (asigl+ar1l+ar2l+ar3l+ar4l)/2

gaPresL = aL
gaPresR = aR

endin


;little trick to make it monophonic
instr 2000
kPoly chnget "poly"
if(kPoly==1) then
outs (gabal+gaPresL)/3, (gabal+gaPresR)/3
endif
outs (gabal+gamix+gaPresL)/3, (gabal+gamix+gaPresR)/3

;outs gamix, gamix
gamix = 0
gabal = 0
endin
</CsInstruments>  
<CsScore>
f1 0 4096 10 1 0.5 .25 .17
f2 0 4096 10 1 .5 .25 1
f3 0 4096 10 1 -1 .2 -.01 0 .2
f4 0 4096 10 1 .1  0 .4  0 .2 .01 0 .2
f5 0 4096 10 0  1 -.2  -.01 0 .2
f6 0 4096 10 1  1.01 .32  .01 1 .2
f7 0 4096 10 1 .3 .5 1 .06 1 1 .2

i1500 0 3000
i2000 0 3000

</CsScore>
</CsoundSynthesizer>