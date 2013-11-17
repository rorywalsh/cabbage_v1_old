<Cabbage>
form size(600, 320), caption("Untitled"), pluginID("plu1")
table bounds(10, 10, 534, 142), channel("table1"), tableNumber(1), colours("blue")
groupbox bounds(8, 157, 240, 100), text("ADSR amplitude"), plant("ADSR11"){ 
rslider bounds(0, 30, 60, 60), text("A"), colour("orange"), channel("att"), range(0.01, 1, .5)
rslider bounds(60, 30, 60, 60), text("D"), colour("orange"), channel("dec"), range(0, 1, .5)
rslider bounds(120, 30, 60, 60), text("S"), colour("orange"), channel("sus"), range(0, 1,.8)
rslider bounds(180, 30, 60, 60), text("R"), colour("orange"), channel("rel"), range(0.01, 1, .2)
}

checkbox bounds(260, 200, 270, 25), channel("play"), text("Play - disables scrubber -")
hslider bounds(260, 157, 291, 40), channel("pos"), text("Position"), range(0, 1, 0)

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
k1 init 1
k2 = -1
if(k1==1) then
chnset k2, "table1"
k1 = 0
endif   
endin

instr 2  
kpos chnget "pos"
kplay chnget "play"
kpos port kpos, .2
a3 interp kpos
k3 downsamp a3
if(kplay==1) then
a3 phasor 1/(ftlen(1)/sr)
k3 downsamp a3
chnset k3, "pos"
print .1, 999
endif

a1 tab a3, 1, 1

chnset k3, "table1"

outs a1, a1
endin

</CsInstruments>  
<CsScore>
;f1 0 1048576 7 1 1048576 1
f1 0 0 1 "vowgen.wav" 0 4 0
i1 0 3600
i2 1 1232
</CsScore>
</CsoundSynthesizer>