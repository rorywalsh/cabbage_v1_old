<Cabbage>
form size(400, 400), caption("Untitled"), pluginID("plu1")
csoundoutput bounds(10, 10, 380, 280)
</Cabbage>
<CsoundSynthesizer>
<CsOptions>
-n -d
</CsOptions>
<CsInstruments>
sr = 44100
ksmps = 64
nchnls = 2
0dbfs=1

instr 1
a1 vco2 1, 100, 10
k1 rms a1, 10
printk 1, k1
outs a1, a1
endin

</CsInstruments>  
<CsScore>
f1 0 1024 7 0 512 0 0 1 512 1
i1 0 3600
</CsScore>
</CsoundSynthesizer>