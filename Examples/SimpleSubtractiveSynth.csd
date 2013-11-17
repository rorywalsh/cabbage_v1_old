<Cabbage> 
form caption("Subtractive Synth") size(450, 270), colour("black")
groupbox text("Filter Controls"), bounds(10, 1, 430, 130), fontcolour("white")
rslider bounds(30, 30, 90, 90), channel("cf"), range(1, 10000, 2000), tracker("yellow"), text("Centre Frequency"), colour("cornflowerblue") 
rslider bounds(130, 30, 90, 90) channel("res"), range(0, 1, .1)), tracker("yellow"), text("Resonance"), colour("cornflowerblue") 
rslider bounds(230, 30, 90, 90) channel("lfo_rate"), range(0, 10, 0), tracker("yellow"), text("LFO Rate"), colour("cornflowerblue") 
rslider bounds(330, 30, 90, 90) channel("lfo_depth"), range(0, 1000, 0), tracker("yellow"),, text("LFO Depth"), colour("cornflowerblue") 
keyboard bounds(10, 140, 430, 100) 
</Cabbage> 
<CsoundSynthesizer> 
<CsOptions> 
-d -n -+rtmidi=null -M0 -b1024 --midi-key-cps=4 --midi-velocity-amp=5 
</CsOptions> 
<CsInstruments> 
sr = 44100 
ksmps = 32 
nchnls = 2 
0dbfs = 1

instr 1 
;access data from named channels
kcf chnget "cf" 
kres chnget "res" 
klforate chnget "lfo_rate"  
klfodepth chnget "lfo_depth" 
aenv madsr .1, 0.4, .6, 1 
asig vco2 p5, p4 
klfo lfo klfodepth, klforate, 5 
aflt moogladder asig, kcf+klfo, kres 
outs aflt*aenv, aflt*aenv
endin 

</CsInstruments> 
<CsScore> 
f1 0 1024 10 1 
f0 3600 
</CsScore> 
</CsoundSynthesizer> 