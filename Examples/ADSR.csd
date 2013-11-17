<Cabbage>
form size(660, 650), caption("ADSR"), pluginID("plu1")
table bounds(10, 6, 240, 92), channel("table1"), tableNumber(1)
table bounds(255, 6, 239, 93), channel("table2"), tableNumber(2)
                   
keyboard bounds(10, 205, 484, 86)bounds 

groupbox bounds(10, 100, 240, 100), text("ADSR amplitude"), plant("ADSR1"), colour(17, 24, 21, 255){ 
rslider bounds(0, 30, 60, 60), text("A"), colour("cornflowerblue"), channel("att"), range(0.01,3, .5)
rslider bounds(60, 30, 60, 60), text("D"), colour("cornflowerblue"), channel("dec"), range(0,1, .5)
rslider bounds(120, 30, 60, 60), text("S"), colour("cornflowerblue"), channel("sus"), range(0,1,.5)
rslider bounds(180, 30, 60, 60), text("R"), colour("cornflowerblue"), channel("rel"), range(0.01,3, 1)
}
     

groupbox bounds(255, 100, 240, 100), text("Band Boost/Cut"), colour(17, 24, 21, 255), plant("BOOST"){ 
rslider bounds(5, 30, 60, 60), text("CF"), colour("cornflowerblue"), channel("cf"), range(0, 5000, 1)
rslider bounds(65, 30, 60, 60), text("BW"), colour("cornflowerblue"), channel("bw"), range(0, 1000, 1)
hslider bounds(129, 30, 103, 30), channel("boost"), text(""), range(-1, 1, 0), colour("white")
checkbox bounds(135, 60, 110, 17), channel("enableFilter"), text("Enable")
}
  
image bounds(5, 4, 494, 293), colour(252, 250, 250, 50), line(0)
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

massign 0,1000
 

instr 1000
iatt chnget "att"
idec chnget "dec"
isus chnget "sus"
irel chnget "rel"
kenv madsr iatt, idec, isus, irel
asig vco2 p5/4, p4
aflt lpf18 asig, abs(kenv)*2000, .5, 0
outs aflt, aflt

endin


instr 500
kgo = -1
katt chnget "att"
kdec chnget "dec"
ksus chnget "sus"
krel chnget "rel"

kchanged changed katt+kdec+ksus+krel
if(kchanged==1) then						;x  y  x                y  x                              y     x     y        x                   y
event "f", 1, 0, 1024, 27,   0, 0, int(katt*(256)), 1, int(katt*(256))+int(kdec*256), ksus, 1024-int(krel*256), ksus,  1024-int(krel*256)+int(krel*256), 0
chnset kgo, "table1"
endif
endin

opcode cabbageADSRTable, k, SSSSSii
Schannel, Satt, Sdec, Ssus, Srel, iFn, iSize xin
kTrigTableUpdate = -1
katt chnget Satt
kdec chnget Sdec
ksus chnget Ssus
krel chnget Srel

kchanged changed katt+kdec+ksus+krel
if(kchanged==1) then						;x  y  x                y  x                              y     x     y        x                   y
event "f", iFn, 0, iSize, 27, 0, 0, int(katt*(256)), 1, int(katt*(256))+int(kdec*256), ksus, 1024-int(krel*512), ksus,  1024-int(krel*512)+int(krel*512), 0
chnset kTrigTableUpdate, Schannel
endif

xout 1
endop


instr 400
kgo = -1
kbw chnget "bw"
kbw = (kbw/1000)*1024
kcf chnget "cf"
kboost chnget "boost"
kboost = ((kboost+1)/2)+.01
kcf = (kcf/5000)*1024
konoff chnget "checkbox"
kchangedCheck changed konoff
if(kchangedCheck==1) then
printk 0, 99
endif

kchanged changed kbw+kcf+kboost
if(kchanged==1) then
if((kcf-(kbw/2))>0) then				
event "f", 2, 0, 1024, 5, .5, kcf-(kbw/2), .5, kbw/2, kboost, kbw/2, .5, 1024-(kcf-(kbw/2)), .5
chnset kgo, "table2"
endif
endif

a1 randi .5, 10000
a2 reson a1, kcf, kbw, 1
outs a2*konoff, a2*konoff

endin


</CsInstruments>  
<CsScore>
f1 0 1024 7 0 1000 0
f2 0 1024 7 1 1024 1
;f1 0 0 1 "test.wav" 0 4 0
i500 0 3600
i400 0 3600
</CsScore>
</CsoundSynthesizer>