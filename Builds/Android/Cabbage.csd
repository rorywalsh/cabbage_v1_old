<Cabbage>
form size(600, 400), caption("Android Synth"), pluginID("plu1")
;keyboard bounds(8, 72, 583, 259)

;hslider bounds(8, 10, 586, 50), channel("hslider"), range(0, 1, .5), colour("white")
button bounds(49, 59, 496, 231), channel("but1"), text("Start me", "Stop me"), value(0)
</Cabbage>
<CsoundSynthesizer>
<CsOptions>
-n -d
</CsOptions>
<CsInstruments>
sr = 22050
ksmps = 256
nchnls = 2
0dbfs=1


instr 1
a1 init 0
k1 chnget "but1"
if(k1==1) then
a1 buzz 1, 400, 1, 1
else
a1 = 0
endif

outs a1, a1
endin

</CsInstruments>  
<CsScore>
f1 0 1024 10 1
i1 0 1000
</CsScore>
</CsoundSynthesizer>