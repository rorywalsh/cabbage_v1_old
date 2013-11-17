<Cabbage>
form size(290, 350), caption("Overlaying Tables"), pluginID("plu1")

table bounds(10, 10, 271, 121), channel("liveWaveform"), tableNumbers(3), colour("orange", "red")
;table bounds(10, 10, 271, 121), channel("adsrTable1"), tableNum(2), colour("yellow")

groupbox bounds(10, 134, 271, 100), text("Harmonic amplitudes"), plant("ADSR2"){ 
rslider bounds(0, 30, 67, 60), text("1"), colour("red"), channel("hrm1"), range(0, 1, 1)
rslider bounds(67, 30, 67, 60), text("2"), colour("red"), channel("hrm2"), range(0, 1, 0)
rslider bounds(135, 30, 67, 60), text("3"), colour("red"), channel("hrm3"), range(0, 1, 0)
rslider bounds(203, 30, 67, 60), text("4"), colour("red"), channel("hrm4"), range(0, 1, 0)
}


keyboard bounds(10, 237, 270, 68)
</Cabbage>
<CsoundSynthesizer>
<CsOptions>        
-n -+rtmidi=null -M0 --midi-key-cps=4 --midi-velocity-amp=5
</CsOptions>
<CsInstruments>
sr = 44100
ksmps = 64
nchnls = 2    
0dbfs=1 

;global var we use to update our tables. 
giTrigTableUpdate init -1

;===== opcode for filling function tables with ADSR envelopes and updating them
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
if(kinit==1) then
;chnset giTrigTableUpdate, "adsrTable1"
kinit = 0
endif   
endin

;this instrument just displays the table
instr 1001  
kPos init 0

;k1 cabbageADSRTable "adsrTable1", "att1", "dec1", "sus1", "rel1", 2, 1024 
k2 cabbageADSRTable "liveWaveform", "att2", "dec2", "sus2", "rel2", 3, 1024 
;k3 cabbageADSRTable "adsrTable3", "att3", "dec3", "sus3", "rel3", 4, 1024 
endin


instr 1
kPos init 0
kHrm1 chnget "hrm1"
kHrm2 chnget "hrm2"
kHrm3 chnget "hrm3"
kHrm4 chnget "hrm4"

kenv1 madsr .1, .3, .6, 1
a1 oscil kenv1*kHrm1, p4, 100
a2 oscil kenv1*kHrm2, p4*2, 100
a3 oscil kenv1*kHrm3, p4*3, 100
a4 oscil kenv1*kHrm4, p4*4, 100

aSig = (a1+a2+a3+a4)/4

;event_i "f", 3, 0, 1024, 7, 0, 1024, 0


kSig downsamp aSig
kPos phasor p4
tablew kSig, kPos, 3, 1

k1 metro 5
if(k1==1) then	
	chnset giTrigTableUpdate, "liveWaveform"
endif



outs aSig, aSig
endin


</CsInstruments>  
<CsScore>
f100 0 1024 10 1

f2 0 1024 7 0 1024 0
f3 0 1024 7 0 1024 0
f4 0 1024 10 1 
i1000 0 3600
i1001 0 3600

</CsScore>
</CsoundSynthesizer>