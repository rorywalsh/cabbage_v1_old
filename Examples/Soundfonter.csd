<Cabbage>
form size(400, 240), caption("Sound font player")
filebutton bounds(24, 99, 150, 50), channel("soundfont"), items("Select sf2")
button bounds(204, 99, 150, 50), channel("load"), items("Load sf2")

keyboard bounds(16, 6, 368, 80)
</Cabbage>

<CsoundSynthesizer>

<CsOptions>
-d -n -+rtmidi=null -M0 --midi-velocity=4 --midi-key=5 
</CsOptions>

<CsInstruments>
sr = 44100
ksmps = 32
nchnls = 2
0dbfs  = 1

giengine fluidEngine
; soundfont path to manual/examples
;isfnum	 fluidLoad "07AcousticGuitar.sf2", giengine, 1
;	 fluidProgramSelect giengine, 1, isfnum, 0, 0
	
;gSname = "07AcousticGuitar.sf2"
;chn_S "soundfont", 3

instr 1	 
ikey	init p4
ivel	init p5
	fluidNote giengine, 1, ikey, ivel
endin

instr 1000
k1 chnget "load"
if(changed(k1)==1) then
event "i", 2000, 0, 10
endif
endin
 
instr 2000 
Sname chnget "soundfont"
prints Sname
isfnum	 fluidLoad "07AcousticGuitar.sf2", giengine, 1
	 fluidProgramSelect giengine, 1, isfnum, 0, 0
endin


instr 3000
imvol  init 7
asigl, asigr fluidOut giengine
       outs asigl*imvol, asigr*imvol
endin
endin
</CsInstruments>

<CsScore>
f0 3600
i1000 0 3600
i3000 1 3600
</CsScore>

</CsoundSynthesizer>