<Cabbage>
form size(290, 550), caption("Overlaying Tables"), pluginID("plu1")

table bounds(10, 10, 271, 121), channel("adsrTable1", "adsrTable2", "adsrTable3"), tableNumbers(2, 3, 4), colour("yellow", "red", "blue")
;table bounds(10, 10, 271, 121), channel("adsrTable1"), tableNum(2), colour("yellow")

groupbox bounds(10, 130, 271, 100), text("Harmonic 1 Amplitude Envelop"), plant("ADSR11"){ 
rslider bounds(0, 28, 59, 57), text("A"), colour("orange"), channel("att1"), range(0.001, 1, .2)
rslider bounds(67, 28, 59, 57), text("D"), colour("orange"), channel("dec1"), range(0, 1, .6)
rslider bounds(135, 28, 59, 57), text("S"), colour("orange"), channel("sus1"), range(0, 1,.3)
rslider bounds(203, 28, 59, 57), text("R"), colour("orange"), channel("rel1"), range(0.01, 3, .4)
}

groupbox bounds(10, 235, 271, 100), text("Harmonic 2 Amplitude amplitude"), plant("ADSR2"){ 
rslider bounds(0, 30, 67, 60), text("A"), colour("red"), channel("att2"), range(0.001, 1, .6)
rslider bounds(67, 30, 67, 60), text("D"), colour("red"), channel("dec2"), range(0,1, .9)
rslider bounds(135, 30, 67, 60), text("S"), colour("red"), channel("sus2"), range(0,1,.2)
rslider bounds(203, 30, 67, 60), text("R"), colour("red"), channel("rel2"), range(0.01,3, .3)
}

groupbox bounds(10, 340, 271, 100), text("Harmonic 3 Amplitude amplitude"), plant("ADSR3"){ 
rslider bounds(0, 30, 67, 60), text("A"), colour("cornflowerblue"), channel("att3"), range(0.001, 1, .5)
rslider bounds(67, 30, 67, 60), text("D"), colour("cornflowerblue"), channel("dec3"), range(0,1, .8)
rslider bounds(135, 30, 67, 60), text("S"), colour("cornflowerblue"), channel("sus3"), range(0,1,.8)
rslider bounds(203, 30, 67, 60), text("R"), colour("cornflowerblue"), channel("rel3"), range(0.01,3, .8)
}

keyboard bounds(11, 450, 270, 68)
</Cabbage>
<CsoundSynthesizer>
<CsOptions>        
-n -+rtmidi=null -M0 -m0d --midi-key-cps=4 --midi-velocity-amp=5
</CsOptions>
<CsInstruments>
sr = 44100
ksmps = 64
nchnls = 2    
0dbfs=1 



;===== opcode for filling function tables with ADSR envelopes
opcode cabbageADSRTable, k, SSSSSii
Schannel, Satt, Sdec, Ssus, Srel, iFn, iSize xin
kTrigTableUpdate = -1
katt chnget Satt
kdec chnget Sdec
ksus chnget Ssus
krel chnget Srel

kchanged changed katt+kdec+ksus+krel
if(kchanged==1) then	
iQuarter = iSize/4

				;x  y  x                y  x                              y     x     y        x                   y
event "f", iFn, 0, iSize, 27,   0, 0, int(katt*(iQuarter)), 1, int(katt*(iQuarter))+int(kdec*iQuarter), ksus, iSize-int(krel*iQuarter), ksus,  iSize-int(krel*iQuarter)+int(krel*iQuarter), 0
chnset kTrigTableUpdate, Schannel
endif

xout 1
endop

opcode cabbageADSR, k, SSSS
Satt, Sdec, Ssus, Srel xin
iatt chnget Satt
idec chnget Sdec
isus chnget Ssus
irel chnget Srel

kenv madsr iatt, idec, isus, irel
xout kenv 
endop
;===== end of UDOS ==========

;this instrument initialises each table
instr 1000  
kinit init 1
kTrigTableUpdate = -1
if(kinit==1) then
chnset kTrigTableUpdate, "adsrTable1"
chnset kTrigTableUpdate, "adsrTable2"
chnset kTrigTableUpdate, "adsrTable3"
kinit = 0
endif   
endin


;this instrument just displays the table
instr 1001  
kPos init 0
kTrigTableUpdate = -1
k1 cabbageADSRTable "adsrTable1", "att1", "dec1", "sus1", "rel1", 2, 1024 
k2 cabbageADSRTable "adsrTable2", "att2", "dec2", "sus2", "rel2", 3, 1024 
k3 cabbageADSRTable "adsrTable3", "att3", "dec3", "sus3", "rel3", 4, 1024 
endin

instr 1
;I just use a plain oldadsr to create the envelopes
kenv1 cabbageADSR "att1", "dec1", "sus1", "rel1"
kenv2 cabbageADSR "att2", "dec2", "sus2", "rel2"
kenv3 cabbageADSR "att3", "dec3", "sus3", "rel3"


a1 oscil kenv1/5, p4, 100
a2 oscil kenv2/5, p4*2.01, 100
a3 oscil kenv3/5, p4*3.02, 100
outs a1+a2+a3, a1+a2+a3

endin

</CsInstruments>  
<CsScore>
f100 0 1024 10 1


f2 0 1024 10 1
f3 0 1024 10 1 1
f4 0 1024 10 1 1 1
i1000 0 3600
i1001 0 3600

</CsScore>
</CsoundSynthesizer>