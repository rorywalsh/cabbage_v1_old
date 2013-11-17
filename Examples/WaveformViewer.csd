<Cabbage>
form size(450, 545), caption("Waveform Viewer"), pluginID("plu1")
table bounds(10, 10, 426, 200), channel("table1"), tableNumbers(1), colour("blue")


groupbox bounds(13, 212, 420, 300), colour(160, 160, 160, 20), fontcolour("yellow"), preset("eq"), text("Harmonics"), plant("filters1"){
vslider bounds(.0, .1, .1, .8), text("1"), channel("hrm1"), range(0, 1, 0), textbox(1), tracker(0, 0, 255, 10), colour("yellow")
vslider bounds(.1, .1, .1, .8), text("2"), channel("hrm2"), range(0, 1, 0), textbox(1), tracker(0, 0, 255, 40), colour("yellow")
vslider bounds(.2, .1, .1, .8), text("3"), channel("hrm3"), range(0, 1, 0), textbox(1), tracker(0, 0, 255, 60), colour("yellow")
vslider bounds(.3, .1, .1, .8), text("4"), channel("hrm4"), range(0, 1, 0), textbox(1), tracker(0, 0, 255, 100), colour("yellow")
vslider bounds(.4, .1, .1, .8), text("5"), channel("hrm5"), range(0, 1, 0), textbox(1), tracker(0, 0, 255, 130), colour("yellow")
vslider bounds(.5, .1, .1, .8), text("6"), channel("hrm6"), range(0, 1, 0), textbox(1), tracker(0, 0, 255, 150), colour("yellow")
vslider bounds(.6, .1, .1, .8), text("7"), channel("hrm7"), range(0, 1, 0), textbox(1), tracker(0, 0, 255, 190), colour("yellow")
vslider bounds(.7, .1, .1, .8), text("8"), channel("hrm8"), range(0, 1, 0), textbox(1), tracker(0, 0, 255, 210), colour("yellow")
vslider bounds(.8, .1, .1, .8), text("9"), channel("hrm9"), range(0, 1, 0), textbox(1), tracker(0, 0, 255, 240), colour("yellow")
vslider bounds(.9, .1, .1, .8), text("10"), channel("hrm10"), range(0, 1, 0), textbox(1), tracker(0, 0, 255, 255), colour("yellow")
}

checkbox bounds(15, 520, 80, 20), channel("auto"), text("Automate"), colour("yellow")
</Cabbage>
<CsoundSynthesizer>
<CsOptions>        
-n -d 
</CsOptions>
<CsInstruments>
sr = 44100
ksmps = 64
nchnls = 2    
0dbfs = 1

instr 1  
;define a variable to update tables
kTriggerTableUpdate = -1

;get data from Cabbage
kHrm1 chnget "hrm1"
kHrm2 chnget "hrm2"
kHrm3 chnget "hrm3"
kHrm4 chnget "hrm4"
kHrm5 chnget "hrm5"
kHrm6 chnget "hrm6"
kHrm7 chnget "hrm7"
kHrm8 chnget "hrm8"
kHrm9 chnget "hrm9"
kHrm10 chnget "hrm10"

kAuto chnget "auto"
kTurnonAuto changed kAuto
if(kAuto==1) then
kHrm1 oscil 1, .1, 2
chnset abs(kHrm1), "hrm1"
kHrm2 oscil 1, .1, 2, .1
chnset abs(kHrm2), "hrm2"
kHrm3 oscil 1, .1, 2, .2
chnset abs(kHrm3), "hrm3"
kHrm4 oscil 1, .1, 2, .3
chnset abs(kHrm4), "hrm4"
kHrm5 oscil 1, .1, 2, .4
chnset abs(kHrm5), "hrm5"
kHrm6 oscil 1, .1, 2, .5
chnset abs(kHrm6), "hrm6"
kHrm7 oscil 1, .1, 2, .6
chnset abs(kHrm7), "hrm7"
kHrm8 oscil 1, .1, 2, .7
chnset abs(kHrm8), "hrm8"
kHrm9 oscil 1, .1, 2, .8
chnset abs(kHrm9), "hrm9"
kHrm10 oscil 1, .1, 2, .9
chnset abs(kHrm10), "hrm10"
endif


;harmonics 
a1 oscil kHrm1, 200, 2
a2 oscil kHrm2, 200*2, 2
a3 oscil kHrm3, 200*3, 2
a4 oscil kHrm4, 200*4, 2
a5 oscil kHrm5, 200*5, 2
a6 oscil kHrm6, 200*6, 2
a7 oscil kHrm7, 200*7, 2
a8 oscil kHrm8, 200*8, 2
a9 oscil kHrm9, 200*9, 2
a10 oscil kHrm10, 200*10, 2

aMix = (a1+a2+a3+a4+a5+a6+a7+a8+a9+a10)
outs aMix/10, aMix/10
;write signal to table
;phasor is set to the lowest frequency
aPos phasor 200		
tablew aMix, aPos, 1, 1 
kUpdate metro 10

if(kUpdate==1) then
;printk .1, 99
	chnset kTriggerTableUpdate, "table1"
endif

endin

</CsInstruments>  
<CsScore>
f1 0 [2^16] 7 0 [2^16] 0
;f1 0 [2^16] 7 1 1 -1 1022 -1 
f2 0 1024 10 1
;f1 0 0 1 "vowgen.wav" 0 4 0
i1 0 3600
;i2 1 1232
</CsScore>
</CsoundSynthesizer>