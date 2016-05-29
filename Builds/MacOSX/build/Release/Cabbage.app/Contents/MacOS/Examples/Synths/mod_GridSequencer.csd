<Cabbage>
form caption("Grid Sequencer") size(580, 370), colour("black"),pluginID("add1"), guirefresh(1) 
gentable bounds(10, 10, 560, 245), tablenumber(10:11:12:13:14:15:16:17), active(1), tablebackgroundcolour(0,0,0,0), tablecolour("green"), zoom(-1), amprange(0, 1, -1, 1), identchannel("tables1")
rslider bounds(320, 260, 71, 62), channel("tempo"), range(1, 16, 4, 1, 1), text("Tempo"),  colour("green"), 
rslider bounds(252, 260, 71, 62), channel("octave"), range(-2, 2, 0, 1, 1),text("Octave"),  colour("green"), 
rslider bounds(184, 260, 71, 62), channel("duration"), range(.1, 5, 1),text("Duration"),  colour("green"), 
image bounds(-100, 9, 35, 240), colour(0,0,0,0), outlinethickness(2), outlinecolour("white"), identchannel("scrubber")
image bounds(10, 258, 383, 63), outlinecolour("white"), outlinethickness(2), colour(0,0,0,0)
image bounds(16, 270, 35, 35), file("play.png"), identchannel("playImage")
button bounds(16, 262, 40, 35), channel("startStop"), text("", ""), colour:0(0,0,0,0), colour:1(0,0,0,0)
checkbox bounds(64, 268, 120, 20), channel("midiOut"), text("Output to Midi"), radiogroup(100), value(1), colour:1(0, 200, 38)
checkbox bounds(64, 288, 120, 20), channel("synthOut"), text("Output to Synth"), radiogroup(100), colour:1(0, 200, 38)
</Cabbage>
<CsoundSynthesizer>
<CsOptions>
-d -n -+rtmidi=null -m0d -Q0
</CsOptions>
<CsInstruments>
sr = 44100
ksmps = 32
nchnls = 2
0dbfs=1
;RW 2014

;----------------------------------------
;main instrument
;----------------------------------------
instr 1
  kTempo chnget "tempo"
  k1 metro kTempo
  kStep init 0
  iLen = ftlen(10)  
  kGo chnget "startStop"

  if k1 == 1 && kGo==1 then
    ;call instrument to trigger notes at i-time
    event "i", 2, 0, 1, kStep+0, 1    ;pattern 1    
	;updates scrubber position
    Sposition sprintfk "pos(%d, 9)", kStep*(560/iLen)+10
    kStep = (kStep>14 ? 0 : kStep+1)
    chnset Sposition, "scrubber"  
  endif

  ;toggle play/puase 
  if changed:k(kGo)==1 then
    if kGo==1 then
      chnset "file(\"pause.png\")", "playImage"
    else
      chnset "file(\"play.png\")", "playImage"
      ;hide scrubber when not playing
      chnset "pos(-100, 9)", "scrubber" 
    endif
  endif

endin 

;---------------------------------------
; trigger notes 
;---------------------------------------
instr 2
  iRow = 0
  iTables[] genarray 0, 7
  iOct chnget "octave"
  iDur chnget "duration"
  until iRow>7 do
    iAmp tab_i p4, iTables[iRow]+10 
    if iAmp == 1 then
      iNote tab_i 7-iRow, 5
;      print iNote
      event_i "i", 3, 0, 1, iNote+iOct*12
    endif
    iRow+=1
  enduntil    
endin

;---------------------------------------
; play notes 
;---------------------------------------
instr 3
iSynth chnget "synthOut"
iMidiOut chnget "midiOut"
	if iSynth == 1 then
  ;simple synth for testing purpose
	  a1 expseg 0.01, .01, .25, p3, 0.01
	  a2 oscili a1, cpsmidinn(p4), -1
	  outs a2, a2 
	else
	  noteondur 1, p4, 120, p3
	endif
endin


</CsInstruments>
<CsScore>
;8 tables of 16 samples
f10 0 16 -2 0 
f11 0 16 -2 0
f12 0 16 -2 0
f13 0 16 -2 0
f14 0 16 -2 0  
f15 0 16 -2 0 
f16 0 16 -2 0 
f17 0 16 -2 0 

;main instrument
i1 2 1000
</CsScore>
</CsoundSynthesizer>