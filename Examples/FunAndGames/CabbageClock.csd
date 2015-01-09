<Cabbage>
form size(400, 400), caption("Cabbage Clock"), pluginID("plu1"), guirefresh(10)
image bounds(10, 10, 380, 380), shape("ellipse"), colour(0,0,0,0), outlinecolour("white"), outlinethickness(2)
image bounds(350, 180, 40, 20), colour("green"), shape("rounded")
image bounds(10, 180, 40, 20), colour("green"), shape("rounded")
image bounds(190, 350, 20, 40), colour("green"), shape("rounded")
image bounds(190, 10, 20, 40), colour("green"), shape("rounded")
image bounds(200, 180, 160, 20), colour("white"), identchannel("secondhand")
image bounds(200, 180, 140, 20), colour("cornflowerblue"), identchannel("minutehand"), 
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
 kSecond, kMinute init 0
 if metro(20)==1 then
 	SIdent sprintfk "rotate(%f, 0, 10)", (kSecond/60)*(2*3.14159)
 	chnset SIdent, "secondhand"
 	kSecond = (kSecond<60 ? kSecond+1 : 0)
 	kMinute = (kSecond==46 ? kMinute+1 : kMinute)
 	if changed(kMinute)==1 then
 		SIdent sprintfk "rotate(%f, 0, 10)", (kMinute/60)*(2*3.14159)
 		chnset SIdent, "minutehand"
  	endif
 endif
endin


</CsInstruments>  
<CsScore>
f1 0 2048 10 1 
i1 0 1000
</CsScore>
</CsoundSynthesizer>