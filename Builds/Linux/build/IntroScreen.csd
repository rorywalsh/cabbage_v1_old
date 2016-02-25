<Cabbage>
form size(700, 550), caption("Welcome to Cabbage"), pluginID("plu1"), colour(58, 110, 182)
image bounds(0, 0, 700, 500), file("cabbage.png"), identchannel("cabbage")
image bounds(0, 0, 700, 500), file("cabbageEarphones.png"), identchannel("phones") 
label bounds(40, 480, 600, 30), text("Press the Options button to begin..."), align("centre"), fontcolour("white"), identchannel("text")
</Cabbage>
<CsoundSynthesizer>
<CsOptions>
-m0d
</CsOptions>
<CsInstruments>
sr = 44100
ksmps = 64
nchnls = 2
0dbfs=1


instr 1
kCounter line 0, 100000, 100000
kAmp linseg 1, 3, 3, 1, 2
kFreq linseg .5, p3, 10, 1, 1
kOsc oscil kAmp, kFreq

kRGBLine linseg 0, .5, 255, 1, 255
kFloatLine linseg 0, .5, 1, 1, 1

if metro(16)==1 then
	Smessage sprintfk "size(%d, %d), alpha(%d)", (700+kOsc*2), (500+kOsc*2), kFloatLine
	chnset Smessage, "phones"
endif		
endin

</CsInstruments>  
<CsScore>
f1 0 1024 10 1
f2 0 1024 10 1 1 .4 .3 .2
i1 0 5000
</CsScore>
</CsoundSynthesizer>