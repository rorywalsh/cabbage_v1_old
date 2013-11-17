<Cabbage>
form size(305, 158), caption("MIDI Arpeggiator"), colour(30, 30, 30)
groupbox bounds(9, 5, 281, 113), text("Controls"), colour("black")
rslider bounds(20, 33, 67, 71), text("Transpose"), range(0, 11, 0), channel("range")
rslider bounds(91, 33, 67, 71), text("Tempo"), range(0, 11, 0), channel("tempo")

combobox bounds(161, 33, 122, 72), caption("Chord"), channel("combobox"), items("Major", "Minor", "7th")
</Cabbage>
<CsoundSynthesizer>
<CsOptions>
-n -d -+rtmidi=NULL -M0 -Q0 --midi-key=4 --midi-velocity-amp=5
</CsOptions>
<CsInstruments>
sr = 44100
ksmps = 64
nchnls = 2
0dbfs=1

;trigger instrument 1000 with incoming midi
massign 0, 1000

instr 1000
;retrieve channel data from Cabbage
kTransp chnget "transpose"
iChord chnget "chord"
kTempo chnget "tempo"
kTrigger metro kTempo;			
kIndex init 0

;if a type of chord is selected, proceed
if(iChord>0) then
	if(kTrigger==1) then
	;read through table of notes
	kNote tab kIndex, iChord
	;start instrument one each time we enter this test
	event "i", 1, 0, 1/kTempo, p4+kNote+kTransp
	;increment out table index
	kIndex = (kIndex>3 ? 0 : kIndex+1); 
	endif
endif

endin

instr 1	
;output MIDI notes
noteondur 1, p4, 120, p3
endin

</CsInstruments>  
<CsScore>
;tables containing chord types, maj, min, 7th
f1 0 4 -2 0 4 7 4 
f1 0 4 -2 0 3 7 3
f1 0 4 -2 0 4 7 11 
f0 3600
</CsScore>
</CsoundSynthesizer>