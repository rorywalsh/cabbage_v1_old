<Cabbage>
form caption("Table File Player") size(340, 155), colour(0,0,0) pluginID("FPly")
filebutton bounds(  5,  5, 80, 25), text("Open","Open"), fontcolour("white") channel("filename"), shape("ellipse")
checkbox   bounds(  5, 35, 95, 25), channel("PlayStop"), text("Play/Stop"), fontcolour("white")

groupbox   bounds(100,  5,100, 45), plant("looping"), text("Looping Mode"){
combobox   bounds( 10, 20, 80, 20), channel("loop"), items("None", "Forward", "Fwd./Bwd."), value(1), fontcolour("white")
}

rslider    bounds(205,  0, 70, 70), channel("transpose"), range(-48, 48, 0,1,1), colour(0,0,0)), trackercolour("silver"), text("Transpose"), fontcolour("white")
rslider    bounds(270,  0, 70, 70), channel("level"), range(  0,  3.00, 1, 0.5), colour(0,0,0), trackercolour("silver"), text("Level"), fontcolour("white")
keyboard bounds(5, 75, 330, 75)
</Cabbage>

<CsoundSynthesizer>

<CsOptions>
-n -+rtmidi=NULL -M0
</CsOptions>

<CsInstruments>

sr = 44100
ksmps = 64
nchnls = 2
0dbfs=1
massign	0,3

gitable	ftgen	1,0,4096,10,1

instr	1
gkPlayStop	chnget	"PlayStop"
gkloop		chnget	"loop"
gktranspose	chnget	"transpose"
gklevel		chnget	"level"
ktrig	trigger	gkPlayStop,0.5,0
schedkwhen	ktrig,0,0,4,0,-1

Sfilepath	chnget	"filename"
;ktrig2		changed	Sfilepath
;kval		strtodk Sfilepath
;ktrig2		changed	kval
;schedkwhen	ktrig2,0,0,2,0,0
endin

instr	2
Sfilepath	chnget	"filename"
;iLen strlen Sfilepath
;if iLen>0 then
; gitable	ftgen	1,0,0,1,Sfilepath,0,0,0
;endif
endin

instr	3	;midi
icps	cpsmidi
iamp	ampmidi	1
iratio	=	icps/cpsoct(8)
ichans	=	ftchnls(gitable)
if ichans==1 then
 a1	loscil3	gklevel*iamp,iratio,gitable,1,i(gkloop)-1,0,nsamp(gitable)
	outs	a1,a1
elseif ichans==2 then
 a1,a2	loscil3	gklevel*iamp,iratio,gitable,1,i(gkloop)-1,0,nsamp(gitable)
	outs	a1,a2
endif

endin

instr	4
if gkPlayStop==0 then
 turnoff
endif

ichans	=	ftchnls(gitable)
if ichans==1 then
 a1	loscil3	gklevel,semitone(gktranspose),gitable,1,i(gkloop)-1,0,nsamp(gitable)
	outs	a1,a1
elseif ichans==2 then
 a1,a2	loscil3	gklevel,semitone(gktranspose),gitable,1,i(gkloop)-1,0,nsamp(gitable)
	outs	a1,a2
endif
endin

</CsInstruments>  

<CsScore>
i 1 0 10000
</CsScore>

</CsoundSynthesizer>
