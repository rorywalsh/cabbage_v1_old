<Cabbage>
form caption("Grid Sequencer") size(615, 300), colour(255, 255, 255),pluginID("add1"), androidinfo("Grid sequencer, Rory Walsh 2015") 
rslider bounds(476, 10, 70, 64), channel("tempo"), range(1, 16, 4, 1, 1), text("Tempo"),  colour("white"), trackercolour("black") 
rslider bounds(540, 10, 71, 64), channel("octave"), range(-2, 2, 0, 1, 1),text("Octave"),  colour("white"), trackercolour("black")  
rslider bounds(476, 76, 71, 64), channel("duration"), range(.1, 5, 1),text("Duration"),  colour("white"), trackercolour("black")  
;image bounds(2, 0, 571, 254), colour("black")
;image bounds(10, 258, 383, 63), outlinecolour("white"), outlinethickness(2), colour(0,0,0,0)
;image bounds(16, 270, 35, 35), file("play.png"), identchannel("playImage")
;image bounds(2, 0, 571, 264), colour("black")
;gentable bounds(8, 6, 560, 245), tablenumber(10:11:12:13:14:15:16:17), active(1), tablebackgroundcolour(0,0,0,0), tablecolour("green"), zoom(-1), amprange(0, 1, -1, 1), identchannel("tables1")

image bounds(10, 10, 464, 276), colour("black")

groupbox bounds(38, 22, 409, 250), text("Delay Lines"), fontcolour("black"), visible(0), colour("white"), plant("delayPlant"), identchannel("delays"){
rslider bounds(14, 40, 95, 97), channel("del1"), range(0, 1, 0, 1, 0.001), textcolour("black"),  colour(55, 110, 182), text("D1"), trackercolour("black")
rslider bounds(110, 40, 95, 97), channel("del2"), range(0, 1, 0, 1, 0.001), textcolour("black"),  colour(55, 110, 182), text("D2"), trackercolour("black")
rslider bounds(207, 40, 95, 97), channel("del3"), range(0, 1, 0, 1, 0.001), textcolour("black"),  colour(55, 110, 182), text("D3"), trackercolour("black")
rslider bounds(301, 40, 95, 97), channel("del4"), range(0, 1, 0, 1, 0.001), textcolour("black"),  colour(55, 110, 182), text("D4"), trackercolour("black")
rslider bounds(16, 144, 95, 97), channel("del5"), range(0, 1, 0, 1, 0.001), textcolour("black"),  colour(55, 110, 182), text("D5"), trackercolour("black")
rslider bounds(111, 144, 95, 97), channel("del6"), range(0, 1, 0, 1, 0.001), textcolour("black"),  colour(55, 110, 182), text("D6"), trackercolour("black")
rslider bounds(207, 144, 95, 97), channel("del7"), range(0, 1, 0, 1, 0.001), textcolour("black"),  colour(55, 110, 182), text("D7"), trackercolour("black")
rslider bounds(299, 144, 95, 97), channel("del8"), range(0, 1, 0, 1, 0.001), textcolour("black"),  colour(55, 110, 182), text("D8"), trackercolour("black")
}

image bounds(26, 20, 432, 290), colour(255, 255, 255, 0), plant("gridPlant"), identchannel("grid") {
checkbox bounds(0, 0, 36, 32), channel("CH1"), text(""), corners(5)
checkbox bounds(36, 0, 36, 32), channel("CH2"), text(""), corners(5)
checkbox bounds(72, 0, 36, 32), channel("CH3"), text(""), corners(5)
checkbox bounds(108, 0, 36, 32), channel("CH4"), text(""), corners(5)
checkbox bounds(144, 0, 36, 32), channel("CH5"), text(""), corners(5)
checkbox bounds(180, 0, 36, 32), channel("CH6"), text(""), corners(5)
checkbox bounds(216, 0, 36, 32), channel("CH7"), text(""), corners(5)
checkbox bounds(252, 0, 36, 32), channel("CH8"), text(""), corners(5)
checkbox bounds(288, 0, 36, 32), channel("CH9"), text(""), corners(5)
checkbox bounds(324, 0, 36, 32), channel("CH10"), text(""), corners(5)
checkbox bounds(360, 0, 36, 32), channel("CH11"), text(""), corners(5)
checkbox bounds(396, 0, 36, 32), channel("CH12"), text(""), corners(5)
checkbox bounds(0, 217, 36, 32), channel("CL1"), text(""), corners(5)
checkbox bounds(36, 217, 36, 32), channel("CL2"), text(""), corners(5)
checkbox bounds(72, 217, 36, 32), channel("CL3"), text(""), corners(5)
checkbox bounds(108, 217, 36, 32), channel("CL4"), text(""), corners(5)
checkbox bounds(144, 217, 36, 32), channel("CL5"), text(""), corners(5)
checkbox bounds(180, 217, 36, 32), channel("CL6"), text(""), corners(5)
checkbox bounds(216, 217, 36, 32), channel("CL7"), text(""), corners(5)
checkbox bounds(252, 217, 36, 32), channel("CL8"), text(""), corners(5)
checkbox bounds(288, 217, 36, 32), channel("CL9"), text(""), corners(5)
checkbox bounds(324, 217, 36, 32), channel("CL10"), text(""), corners(5)
checkbox bounds(360, 217, 36, 32), channel("CL11"), text(""), corners(5)
checkbox bounds(396, 217, 36, 32), channel("CL12"), text(""), corners(5)
checkbox bounds(0, 186, 36, 32), channel("D1"), text(""), corners(5)
checkbox bounds(36, 186, 36, 32), channel("D2"), text(""), corners(5)
checkbox bounds(72, 186, 36, 32), channel("D3"), text(""), corners(5)
checkbox bounds(108, 186, 36, 32), channel("D4"), text(""), corners(5)
checkbox bounds(144, 186, 36, 32), channel("D5"), text(""), corners(5)
checkbox bounds(180, 186, 36, 32), channel("D6"), text(""), corners(5)
checkbox bounds(216, 186, 36, 32), channel("D7"), text(""), corners(5)
checkbox bounds(252, 186, 36, 32), channel("D8"), text(""), corners(5)
checkbox bounds(288, 186, 36, 32), channel("D9"), text(""), corners(5)
checkbox bounds(324, 186, 36, 32), channel("D10"), text(""), corners(5)
checkbox bounds(360, 186, 36, 32), channel("D11"), text(""), corners(5)
checkbox bounds(396, 186, 36, 32), channel("D12"), text(""), corners(5)
checkbox bounds(0, 155, 36, 32), channel("E1"), text(""), corners(5)
checkbox bounds(36, 155, 36, 32), channel("E2"), text(""), corners(5)
checkbox bounds(72, 155, 36, 32), channel("E3"), text(""), corners(5)
checkbox bounds(108, 155, 36, 32), channel("E4"), text(""), corners(5)
checkbox bounds(144, 155, 36, 32), channel("E5"), text(""), corners(5)
checkbox bounds(180, 155, 36, 32), channel("E6"), text(""), corners(5)
checkbox bounds(216, 155, 36, 32), channel("E7"), text(""), corners(5)
checkbox bounds(252, 155, 36, 32), channel("E8"), text(""), corners(5)
checkbox bounds(288, 155, 36, 32), channel("E9"), text(""), corners(5)
checkbox bounds(324, 155, 36, 32), channel("E10"), text(""), corners(5)
checkbox bounds(360, 155, 36, 32), channel("E11"), text(""), corners(5)
checkbox bounds(396, 155, 36, 32), channel("E12"), text(""), corners(5)
checkbox bounds(0, 124, 36, 32), channel("F1"), text(""), corners(5)
checkbox bounds(36, 124, 36, 32), channel("F2"), text(""), corners(5)
checkbox bounds(72, 124, 36, 32), channel("F3"), text(""), corners(5)
checkbox bounds(108, 124, 36, 32), channel("F4"), text(""), corners(5)
checkbox bounds(144, 124, 36, 32), channel("F5"), text(""), corners(5)
checkbox bounds(180, 124, 36, 32), channel("F6"), text(""), corners(5)
checkbox bounds(216, 124, 36, 32), channel("F7"), text(""), corners(5)
checkbox bounds(252, 124, 36, 32), channel("F8"), text(""), corners(5)
checkbox bounds(288, 124, 36, 32), channel("F9"), text(""), corners(5)
checkbox bounds(324, 124, 36, 32), channel("F10"), text(""), corners(5)
checkbox bounds(360, 124, 36, 32), channel("F11"), text(""), corners(5)
checkbox bounds(396, 124, 36, 32), channel("F12"), text(""), corners(5)
checkbox bounds(0, 93, 36, 32), channel("G1"), text(""), corners(5)
checkbox bounds(36, 93, 36, 32), channel("G2"), text(""), corners(5)
checkbox bounds(72, 93, 36, 32), channel("G3"), text(""), corners(5)
checkbox bounds(108, 93, 36, 32), channel("G4"), text(""), corners(5)
checkbox bounds(144, 93, 36, 32), channel("G5"), text(""), corners(5)
checkbox bounds(180, 93, 36, 32), channel("G6"), text(""), corners(5)
checkbox bounds(216, 93, 36, 32), channel("G7"), text(""), corners(5)
checkbox bounds(252, 93, 36, 32), channel("G8"), text(""), corners(5)
checkbox bounds(288, 93, 36, 32), channel("G9"), text(""), corners(5)
checkbox bounds(324, 93, 36, 32), channel("G10"), text(""), corners(5)
checkbox bounds(360, 93, 36, 32), channel("G11"), text(""), corners(5)
checkbox bounds(396, 93, 36, 32), channel("G12"), text(""), corners(5)
checkbox bounds(0, 62, 36, 32), channel( "A1"), text(""), corners(5)
checkbox bounds(36, 62, 36, 32), channel("A2"), text(""), corners(5)
checkbox bounds(72, 62, 36, 32), channel("A3"), text(""), corners(5)
checkbox bounds(108, 62, 36, 32), channel("A4"), text(""), corners(5)
checkbox bounds(144, 62, 36, 32), channel("A5"), text(""), corners(5)
checkbox bounds(180, 62, 36, 32), channel("A6"), text(""), corners(5)
checkbox bounds(216, 62, 36, 32), channel("A7"), text(""), corners(5)
checkbox bounds(252, 62, 36, 32), channel("A8"), text(""), corners(5)
checkbox bounds(288, 62, 36, 32), channel("A9"), text(""), corners(5)
checkbox bounds(324, 62, 36, 32), channel("A10"), text(""), corners(5)
checkbox bounds(360, 62, 36, 32), channel("A11"), text(""), corners(5)
checkbox bounds(396, 62, 36, 32), channel("A12"), text(""), corners(5)
checkbox bounds(0, 31, 36, 32), channel("B1"), text(""), corners(5)
checkbox bounds(36, 31, 36, 32), channel("B2"), text(""), corners(5)
checkbox bounds(72, 31, 36, 32), channel("B3"), text(""), corners(5)
checkbox bounds(108, 31, 36, 32), channel("B4"), text(""), corners(5)
checkbox bounds(144, 31, 36, 32), channel("B5"), text(""), corners(5)
checkbox bounds(180, 31, 36, 32), channel("B6"), text(""), corners(5)
checkbox bounds(216, 31, 36, 32), channel("B7"), text(""), corners(5)
checkbox bounds(252, 31, 36, 32), channel("B8"), text(""), corners(5)
checkbox bounds(288, 31, 36, 32), channel("B9"), text(""), corners(5)
checkbox bounds(324, 31, 36, 32), channel("B10"), text(""), corners(5)
checkbox bounds(360, 31, 36, 32), channel("B11"), text(""), corners(5)
checkbox bounds(396, 31, 36, 32), channel("B12"), text(""), corners(5)
checkbox bounds(0, 249, 35, 9), channel("stepPos1")   colour:0(50, 50, 50),
checkbox bounds(36, 249, 35, 9), channel("stepPos2")   colour:0(50, 50, 50) 
checkbox bounds(72, 249, 35, 9), channel("stepPos3")   colour:0(50, 50, 50) 
checkbox bounds(108, 249, 35, 9), channel("stepPos4")  colour:0(50, 50, 50) 
checkbox bounds(144, 249, 35, 9), channel("stepPos5")  colour:0(50, 50, 50) 
checkbox bounds(180, 249, 35, 9), channel("stepPos6")  colour:0(50, 50, 50) 
checkbox bounds(216, 249, 35, 9), channel("stepPos7")  colour:0(50, 50, 50) 
checkbox bounds(252, 249, 35, 9), channel("stepPos8")  colour:0(50, 50, 50) 
checkbox bounds(288, 249, 35, 9), channel("stepPos9")  colour:0(50, 50, 50) 
checkbox bounds(324, 249, 35, 9), channel("stepPos10") colour:0(50, 50, 50) 
checkbox bounds(360, 249, 35, 9), channel("stepPos11") colour:0(50, 50, 50) 
checkbox bounds(396, 249, 35, 9), channel("stepPos12") colour:0(50, 50, 50) 
}

button bounds(486, 208, 57, 26), channel("startStop"), text("Start", "Stop"), colour:0(0,0,0), colour:1("lime"), fontcolour:0("white"), fontcolour:1("black")
button bounds(486, 236, 57, 26), channel("gridDisplay"), colour:0("black"), colour:1(55, 110, 182) text("Grid"), value(1), radiogroup(99)
button bounds(546, 236, 57, 26), channel("delayDisplay"), colour:0("black"), colour:1(55, 110, 182), text("Delay"), radiogroup(99)
button bounds(546, 208, 57, 26), channel("random"), text("Random")
rslider bounds(540, 76, 71, 64), channel("randomWeight"), range(30, 100, 80, 1, 1),text("R-Weight"),  colour("white"), trackercolour("black")  
rslider bounds(476, 140, 71, 64), channel("synthGain"), range(0, 1, .5, 1, .001), text("Syn-Gain"),  colour("white"), trackercolour("black")  
rslider bounds(540, 140, 71, 64), channel("delayGain"), range(0, 1, .5, 1, .001), text("Del-Gain"),  colour("white"), trackercolour("black")  
</Cabbage>
<CsoundSynthesizer>
<CsOptions>
-+rtaudio=jack -odac;-d -n -+rtmidi=null -m0d
</CsOptions>
<CsInstruments>
sr = 44100
ksmps = 64
nchnls = 2
0dbfs=1
;RW 2014

giCL ftgen 10, 0, 16, -2, 0 
giD ftgen 11, 0, 16, -2, 0 
giE ftgen 12, 0, 16, -2, 0 
giF ftgen 13, 0, 16, -2, 0 
giG ftgen 14, 0, 16, -2, 0 
giA ftgen 15, 0, 16, -2, 0 
giB ftgen 16, 0, 16, -2, 0 
giC ftgen 17, 0, 16, -2, 0 
giCH ftgen 10, 0, 16, -2, 0 
;---------------------------------------- 
;main instrument
;----------------------------------------
instr MissionCentral
  kTempo chnget "tempo"
  kTrig metro kTempo
  kIndex init 1
  kStep init 0
  iLen = ftlen(10)  
  kGo chnget "startStop"

  if changed:k(chnget:k("random"))==1 then
  	Smessage sprintfk {{i"RandomShuffle" 0 .1 "%s"}}, "CL"
  	scoreline Smessage, 1
  	Smessage sprintfk {{i"RandomShuffle" 0 .1 "%s"}}, "D"
  	scoreline Smessage, 1
  	Smessage sprintfk {{i"RandomShuffle" 0 .1 "%s"}}, "E"
  	scoreline Smessage, 1
  	Smessage sprintfk {{i"RandomShuffle" 0 .1 "%s"}}, "F"
  	scoreline Smessage, 1
  	Smessage sprintfk {{i"RandomShuffle" 0 .1 "%s"}}, "G"
  	scoreline Smessage, 1
  	Smessage sprintfk {{i"RandomShuffle" 0 .1 "%s"}}, "A"
  	scoreline Smessage, 1
  	Smessage sprintfk {{i"RandomShuffle" 0 .1 "%s"}}, "B"
  	scoreline Smessage, 1
  	Smessage sprintfk {{i"RandomShuffle" 0 .1 "%s"}}, "CH"
  	scoreline Smessage, 1
  endif

  if changed:k(chnget:k("delaysDisplay"))==1 || changed:k(chnget:k("gridDisplay"))==1 then
    if chnget:k("delayDisplay")==1 then
      chnset "visible(1)", "delays"
      chnset "visible(0)", "grid"
    else
      chnset "visible(1)", "grid"
      chnset "visible(0)", "delays" 
    endif
  endif

  if kTrig == 1 && kGo==1 then
  ;printks "Hello\n", 0
    ;call instrument to trigger notes at i-time
    ;event "i", "CheckAndEnableNotes", 0, 0.01, "D", giD
    event "i", 2, 0, 1, kIndex-1, 1    ;pattern 1    
  chnset kIndex==1 ? 1 : 0, "stepPos1"
  chnset kIndex==2 ? 1 : 0, "stepPos2"
  chnset kIndex==3 ? 1 : 0, "stepPos3"
  chnset kIndex==4 ? 1 : 0, "stepPos4"
  chnset kIndex==5 ? 1 : 0, "stepPos5"
  chnset kIndex==6 ? 1 : 0, "stepPos6"
  chnset kIndex==7 ? 1 : 0, "stepPos7"
  chnset kIndex==8 ? 1 : 0, "stepPos8"
  chnset kIndex==9 ? 1 : 0, "stepPos9"
  chnset kIndex==10 ? 1 : 0, "stepPos10"
  chnset kIndex==11 ? 1 : 0, "stepPos11"
  chnset kIndex==12 ? 1 : 0, "stepPos12"
  kIndex = kIndex==12 ? 1 : kIndex+1
  endif

endin 

;---------------------------------------
; update tables if button is pressed
;---------------------------------------
instr UpdateTablesWhenGridIsPressed
  iTable = p4
  SInputChannel init p5

  Schannel sprintfk "%s%d", SInputChannel, 1
  if changed:k(chnget:k(Schannel))==1 then
    tabw chnget:k(Schannel)==1 ? 1 : 0, 0, iTable
  endif
  Schannel sprintfk "%s%d", SInputChannel, 2
  if changed:k(chnget:k(Schannel))==1 then
    tabw chnget:k(Schannel)==1 ? 1 : 0, 1, iTable
  endif
  Schannel sprintfk "%s%d", SInputChannel, 3
  if changed:k(chnget:k(Schannel))==1 then
    tabw chnget:k(Schannel)==1 ? 1 : 0, 2, iTable
  endif
  Schannel sprintfk "%s%d", SInputChannel, 4
  if changed:k(chnget:k(Schannel))==1 then
    tabw chnget:k(Schannel)==1 ? 1 : 0, 3, iTable
  endif
  Schannel sprintfk "%s%d", SInputChannel, 5
  if changed:k(chnget:k(Schannel))==1 then
    tabw chnget:k(Schannel)==1 ? 1 : 0, 4, iTable
  endif
  Schannel sprintfk "%s%d", SInputChannel, 6
  if changed:k(chnget:k(Schannel))==1 then
    tabw chnget:k(Schannel)==1 ? 1 : 0, 5, iTable
  endif
  Schannel sprintfk "%s%d", SInputChannel, 7
  if changed:k(chnget:k(Schannel))==1 then
    tabw chnget:k(Schannel)==1 ? 1 : 0, 6, iTable
  endif
  Schannel sprintfk "%s%d", SInputChannel, 8
  if changed:k(chnget:k(Schannel))==1 then
    tabw chnget:k(Schannel)==1 ? 1 : 0, 7, iTable
  endif
  Schannel sprintfk "%s%d", SInputChannel, 9
  if changed:k(chnget:k(Schannel))==1 then
    tabw chnget:k(Schannel)==1 ? 1 : 0, 8, iTable
  endif
  Schannel sprintfk "%s%d", SInputChannel, 10
  if changed:k(chnget:k(Schannel))==1 then
    tabw chnget:k(Schannel)==1 ? 1 : 0, 9, iTable
  endif
  Schannel sprintfk "%s%d", SInputChannel, 11
  if changed:k(chnget:k(Schannel))==1 then
    tabw chnget:k(Schannel)==1 ? 1 : 0, 10, iTable
  endif
  Schannel sprintfk "%s%d", SInputChannel, 12
  if changed:k(chnget:k(Schannel))==1 then
    tabw chnget:k(Schannel)==1 ? 1 : 0, 11, iTable
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
    ;print iTables[iRow]+10
    if iAmp == 1 then
      iNote tab_i 7-iRow, 5
;      print iNote
      event_i "i", 3, 0, chnget:i("duration"), iNote+iOct*12
    endif
    iRow+=1
  enduntil    
endin

;---------------------------------------
; play notes 
;---------------------------------------
instr 3
iSynth = 1
a1 expon .1, p3, 0.001
a2 oscili a1, cpsmidinn(p4), -1
;outs a2, a2 
chnmix a2, "mainOuts"
endin

;---------------------------------------
; simple delay lines 
;---------------------------------------
instr MixerAndEffects
aMix chnget "mainOuts"

aDelay1 comb aMix, 4, .25
aDelay2 comb aMix, 4, .33

aDelay3 comb aMix, 4, .64
aDelay4 comb aMix, 4, .5

aDelay5 comb aMix, 4, .75
aDelay6 comb aMix, 10, .91

aDelay7 comb aMix, 12, 1.8
aDelay8 comb aMix, 12, 1.17

aDelay9 comb aMix, 15, 2.15
aDelay10 comb aMix, 15, 2.74

aDelay11 comb aMix, 20, 3.25
aDelay12 comb aMix, 20, 3.57

aDelay13 comb aMix, 30, 4.5
aDelay14 comb aMix, 30, 4.82

aDelay15 comb aMix, 30, 5.15
aDelay16 comb aMix, 30, 5.20

aMixL1 = (aDelay1*chnget:k("del1")+aDelay3*chnget:k("del2")+aDelay1*chnget:k("del3")+aDelay3*chnget:k("del4"))*.25
aMixR1 = (aDelay2*chnget:k("del1")+aDelay4*chnget:k("del2")+aDelay1*chnget:k("del3")+aDelay3*chnget:k("del4"))*.25
aMixL2 = (aDelay9*chnget:k("del5")+aDelay12*chnget:k("del6")+aDelay13*chnget:k("del7")+aDelay15*chnget:k("del8"))*.25
aMixR2 = (aDelay10*chnget:k("del5")+aDelay13*chnget:k("del6")+aDelay14*chnget:k("del7")+aDelay15*chnget:k("del8"))*.25

aMixDownL = ((aMixL1+aMixL1)/2)*chnget:k("delayGain")
aMixDownR = ((aMixR2+aMixR2)/2)*chnget:k("delayGain")
aMix = aMix*chnget:k("synthGain")
outs (aMix+aMixDownL)/2, (aMix+aMixDownR)/2
chnclear "mainOuts"
endin

;---------------------------------------
; random shuffler 
;---------------------------------------
instr RandomShuffle
  prints p4
  SInputChannel init p4
  iWeight chnget "randomWeight"

  Schannel sprintfk "%s%d", SInputChannel, 1
  iVol1 random -100, 100
  chnset iVol1>iWeight ? 1 : 0, Schannel

  Schannel sprintfk "%s%d", SInputChannel, 2  
  iVol2 random -100, 100
  chnset iVol2>iWeight ? 1 : 0, Schannel

  Schannel sprintfk "%s%d", SInputChannel, 3
  iVol3 random -100, 100
  chnset iVol3>iWeight ? 1 : 0, Schannel

  Schannel sprintfk "%s%d", SInputChannel, 4
  iVol4 random -100, 100
  chnset iVol4>iWeight ? 1 : 0, Schannel

  Schannel sprintfk "%s%d", SInputChannel, 5
  iVol5 random -100, 100
  chnset iVol5>iWeight ? 1 : 0, Schannel

  Schannel sprintfk "%s%d", SInputChannel, 6
  iVol6 random -100, 100
  chnset iVol6>iWeight ? 1 : 0, Schannel

  Schannel sprintfk "%s%d", SInputChannel, 7
  iVol7 random -100, 100
  chnset iVol7>iWeight ? 1 : 0, Schannel

  Schannel sprintfk "%s%d", SInputChannel, 8
  iVol8 random -100, 100
  chnset iVol8>iWeight ? 1 : 0, Schannel

  Schannel sprintfk "%s%d", SInputChannel, 9
  iVol9 random -100, 100
  chnset iVol9>iWeight ? 1 : 0, Schannel

  Schannel sprintfk "%s%d", SInputChannel, 10
  iVol10 random -100, 100
  chnset iVol10>iWeight ? 1 : 0, Schannel

  Schannel sprintfk "%s%d", SInputChannel, 11
  iVol11 random -100, 100
  chnset iVol11>iWeight ? 1 : 0, Schannel

  Schannel sprintfk "%s%d", SInputChannel, 12
  iVol12 random -100, 100
  chnset iVol12>iWeight ? 1 : 0, Schannel
 
endin

</CsInstruments>
<CsScore>


;8 tables of 16 samples
f5 0 8 -2 72 71 69 67 65 64 62 60

f0 3600
;i"Transport" 0 3600
;i"UpdateTables" 0 3600
;main instrument
i"UpdateTablesWhenGridIsPressed" 0 3600 10 "CL"
i"UpdateTablesWhenGridIsPressed" 0 3600 11 "D"
i"UpdateTablesWhenGridIsPressed" 0 3600 12 "E"
i"UpdateTablesWhenGridIsPressed" 0 3600 13 "F"
i"UpdateTablesWhenGridIsPressed" 0 3600 14 "G"
i"UpdateTablesWhenGridIsPressed" 0 3600 15 "A"
i"UpdateTablesWhenGridIsPressed" 0 3600 16 "B"
i"UpdateTablesWhenGridIsPressed" 0 3600 17 "CH"

i"MissionCentral" 0 1000
i"MixerAndEffects" 0 1000
</CsScore>
</CsoundSynthesizer>