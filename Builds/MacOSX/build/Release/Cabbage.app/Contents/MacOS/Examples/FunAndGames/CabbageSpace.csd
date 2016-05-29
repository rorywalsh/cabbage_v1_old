<Cabbage>
form size(600, 400), caption("Cabbage Space"), pluginID("plu1"), guirefresh(64)
image bounds(-100, -100, 1, 1), widgetarray("image", 300), shape("ellipse"), colour(255, 255, 255)
</Cabbage>
<CsoundSynthesizer>
<CsOptions>
-n -d -+rtmidi=NULL -M0 -m0d --midi-key=4
</CsOptions>
<CsInstruments>
sr = 44100 
ksmps = 64
nchnls = 2
0dbfs=1
  
instr 1; create stars on start
iCnt init 0
kCnt init 0 
iCntRows init 0
kMetro metro 20.016273

  until iCnt > 200 do
    iX random 0, 600
    iY random 0, 400
    S1 sprintfk "pos(%d, %d)", iX, iY
    S2 sprintfk "image_ident%d", iCnt+1
    chnset S1, S2
    iCnt=iCnt+1
    iCntRows = (iCnt%10==0 ? iCntRows+1 : iCntRows)
  enduntil 
  ;keep changing colour and size of start
  if kMetro==1 then
    kValue randh 255, kr, 2
    S1 sprintfk "colour(%d, %d, %d), size(%d, %d)", abs(kValue)%100, abs(kValue)%200, abs(kValue)%10, abs(kValue)/30, abs(kValue)/30
    S2 sprintfk "image_ident%d", kCnt+1
    chnset S1, S2
    kCnt = (kCnt>200 ? 0 : kCnt+1)  
  endif   
endin

instr 2; move ufo
kX randi 800, .5
kMove jspline 200, 0.1, 1
kTrig metro 20
  ;move spaceship around
  if kTrig==1 then
  S3 sprintfk "colour(%d, 200, 200), size(%d, %d), pos(%d, %d)", 255*kMove, abs(kMove)*.35, abs(kMove)*.15, abs(kX), 200+kMove
  chnset S3, "image_ident100" 
  endif
  
aCar oscil kMove*10, 10, -1
aMod oscil 1, aCar+kMove, -1
aMix = aMod*abs(kMove/200)
aOutL, aOutR reverbsc aMix, aMix, 0.2, 600  
outs aOutL, aOutR
endin

</CsInstruments>  
<CsScore> 
i1 0 1000
i2 0 1000
</CsScore>
</CsoundSynthesizer> 