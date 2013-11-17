<Cabbage>
form size(400, 300), caption("Untitled"), pluginID("plu1")
rslider bounds(10, 10, 70, 70), channel("vol"), range(0, 1, 0)
</Cabbage>
<CsoundSynthesizer>
<CsOptions>
-+rtmidi=alsa -m0d -+rtaudio=alsa -o dac:hw:1,0 -M hw:2,0,0 
</CsOptions>
<CsInstruments>
sr = 44100
ksmps = 64
nchnls = 2
0dbfs=1

opcode midiMonitor, k,o
kMidiSig = 0
kstatus, kchan, kdata1, kdata2 midiin
k1 changed kstatus
k2 changed kchan
k3 changed kdata1
k4 changed kdata2
if((k1==1)||(k2==1)||(k3==1)||(k4==1)) then
printks "Value:%d ChanNo:%d CtrlNo:%d\n" , 0, kdata2, kchan, kdata1
kMidiSig = 1
endif
xout kMidiSig
endop

instr 1
k1 ctrl7 1, 1, 0, 1
chnset k1, "vol"
a1 oscil 1, 400, 1
outs a1*k1, a1*k1
endin

</CsInstruments>  
<CsScore>
f1 0 1024 10 1
i1 0 10
</CsScore>
</CsoundSynthesizer>