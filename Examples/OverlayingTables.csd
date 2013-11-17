<Cabbage>
form size(600, 520), caption("Overlaying Tables"), pluginID("plu1")
table bounds(10, 10, 534, 142), channel("audioTrack"), tableNum(1), alpha(.6)
table bounds(10, 10, 534, 142), channel("ampTrack"), tableNum(2), colour("pink")
table bounds(10, 10, 534, 142), channel("panTrack"), tableNum(3), colour("red")
table bounds(10, 10, 534, 142), channel("revTrack"), tableNum(4), colour("cornflowerblue")

groupbox bounds(8, 157, 240, 100), text("Amplitude Envelop"), plant("ADSR11"){ 
rslider bounds(0, 30, 60, 60), text("A"), colour("pink"), channel("att1"), range(0.01, 1, .2)
rslider bounds(60, 30, 60, 60), text("D"), colour("pink"), channel("dec1"), range(0, 1, .6)
rslider bounds(120, 30, 60, 60), text("S"), colour("pink"), channel("sus1"), range(0, 1,.3)
rslider bounds(180, 30, 60, 60), text("R"), colour("pink"), channel("rel1"), range(0.01, 1, .4)
}

checkbox bounds(260, 200, 270, 25), channel("play"), text("Play [disables scrubber]")
hslider bounds(260, 157, 291, 40), channel("pos"), text("Position"), range(0, 1, 0)

groupbox bounds(8, 260, 240, 100), text("Pan amplitude"), plant("ADSR2"){ 
rslider bounds(0, 30, 60, 60), text("A"), colour("red"), channel("att2"), range(0.01,1, .6)
rslider bounds(60, 30, 60, 60), text("D"), colour("red"), channel("dec2"), range(0,1, .9)
rslider bounds(120, 30, 60, 60), text("S"), colour("red"), channel("sus2"), range(0,1,.2)
rslider bounds(180, 30, 60, 60), text("R"), colour("red"), channel("rel2"), range(0.01,3, .3)
}

groupbox bounds(8, 362, 240, 100), text("Rev amplitude"), plant("ADSR3"){ 
rslider bounds(0, 30, 60, 60), text("A"), colour("cornflowerblue"), channel("att3"), range(0.01,1, .5)
rslider bounds(60, 30, 60, 60), text("D"), colour("cornflowerblue"), channel("dec3"), range(0,1, .8)
rslider bounds(120, 30, 60, 60), text("S"), colour("cornflowerblue"), channel("sus3"), range(0,1,.8)
rslider bounds(180, 30, 60, 60), text("R"), colour("cornflowerblue"), channel("rel3"), range(0.01,3, .8)
}

</Cabbage>
<CsoundSynthesizer>
<CsOptions>        
-n -d -m0d
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
				;x  y  x                y  x                              y     x     y        x                   y
event "f", iFn, 0, iSize, 27,   0, 0, int(katt*(256)), 1, int(katt*(256))+int(kdec*256), ksus, 1024-int(krel*256), ksus,  1024-int(krel*256)+int(krel*256), 0
chnset kTrigTableUpdate, Schannel
endif

xout 1
endop
;===== end of UDOS ==========

instr 1  
k1 init 1
kTrigTableUpdate = -1
if(k1==1) then
chnset kTrigTableUpdate, "ampTrack"
chnset kTrigTableUpdate, "audioTrack"
chnset kTrigTableUpdate, "revTrack"
chnset kTrigTableUpdate, "panTrack"
k1 = 0
endif   
endin

instr 2  
kTrigTableUpdate = -1
kpos chnget "pos"
kchanged changed kpos
kplay chnget "play"
kamp chnget "amp"


k1 cabbageADSRTable "ampTrack", "att1", "dec1", "sus1", "rel1", 2, 1024 
k2 cabbageADSRTable "panTrack", "att2", "dec2", "sus2", "rel2", 3, 1024 
k3 cabbageADSRTable "revTrack", "att3", "dec3", "sus3", "rel3", 4, 1024 

kpos port kpos, .2
aFileIndex interp kpos
k3 downsamp aFileIndex
if(kplay==1) then
aFileIndex phasor 1/(ftlen(1)/sr)
k3 downsamp aFileIndex
;chnset k3, "pos"
print .1, 999
endif

a1 tab aFileIndex, 1, 1
a2 tab aFileIndex, 2, 1

chnset k3, "audioTrack"


outs a1*a2, a1*a2
endin

</CsInstruments>  
<CsScore>
f1 0 0 1 "vowgen.wav" 0 4 0
f2 0 1024 7 0 1024 1
f3 0 1024 7 0 1024 1
f4 0 1024 7 -1 1024 1
i1 0 3600
i2 1 1232
</CsScore>
</CsoundSynthesizer>