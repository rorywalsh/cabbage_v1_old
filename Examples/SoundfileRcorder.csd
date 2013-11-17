<Cabbage>
form size(600, 320), caption("Untitled"), pluginID("plu1")
table bounds(10, 10, 234, 142), channel("table1"), tableNumbers(1), colour("cornflowerblue")
groupbox bounds(11, 154, 271, 100), text("Harmonic amplitudes"), plant("ADSR2"){ 
rslider bounds(0, 30, 67, 60), text("1"), colour("red"), channel("hrm1"), range(0, 1, 1)
rslider bounds(67, 30, 67, 60), text("2"), colour("red"), channel("hrm2"), range(0, 1, 0)
rslider bounds(135, 30, 67, 60), text("3"), colour("red"), channel("hrm3"), range(0, 1, 0)
rslider bounds(203, 30, 67, 60), text("4"), colour("red"), channel("hrm4"), range(0, 1, 0)
}


checkbox bounds(246, 52, 70, 25), channel("record"), text("Record"), colour("red"), value(1)
hslider bounds(246, 10, 291, 40), channel("pos"), text("Position"), range(0, 1, 0)

</Cabbage>
<CsoundSynthesizer>
<CsOptions>        
-n -d 
</CsOptions>
<CsInstruments>
sr = 44100
ksmps = 64
nchnls = 2    


instr 1  
kHrm1 chnget "hrm1"
kHrm2 chnget "hrm2"
kHrm3 chnget "hrm3"
kHrm4 chnget "hrm4"


a1 oscil kHrm1*5000, 200, 2
a2 oscil kHrm2*5000, 200*2, 2
a3 oscil kHrm3*5000, 200*3, 2
a4 oscil kHrm4*5000, 200*4, 2


kPlay chnget "record"

if(kPlay == 1) then
a3 phasor 200
endif


tablew (a1+a2+a3+a4)/4, a3, 1, 1 
k3 = -1
k1 metro 20
if(k1==1) then
chnset k3, "table1"
endif
;outs a1, a1
endin

</CsInstruments>  
<CsScore>
;f1 0 1048576 7 1 1048576 1
f1 0 [2^16] 10 1 
f2 0 1024 10 1
;f1 0 0 1 "vowgen.wav" 0 4 0
i1 0 3600
;i2 1 1232
</CsScore>
</CsoundSynthesizer>