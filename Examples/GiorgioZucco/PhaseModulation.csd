<Cabbage>
form caption("Phase modulation synthesizer") size(552, 330), colour("black"),pluginID("sca1") 
rslider bounds(15, 10, 100, 100) channel("mod"), range(0,10,1), caption("Modulation"), colour("white"), midictrl(1, 1)
rslider bounds(120, 10, 100, 100) channel("semi"), range(-24,12,-12), caption("Coarse"), colour("white")
rslider bounds(225, 10, 100, 100) channel("detune"), range(-2,2,0), caption("Detune"), colour("white")
rslider bounds(330, 10, 100, 100) channel("index"), range(0,2,0), caption("Index"), colour("white")
rslider bounds(435, 10, 100, 100) channel("fback"), range(0, .1,0), caption("Feedback"), colour("white")

groupbox bounds(15, 120, 240, 100), text("ADSR amplitude"), plant("ADSR"){ 
rslider bounds(.0, .3, .6, .6), text("A"), colour("orange"), channel("att"), range(0.01,3, .5)
rslider bounds(.25, .3, .6, .6), text("D"), colour("orange"), channel("dec"), range(0,1, .5)
rslider bounds(.5, .3, .6, .6), text("S"), colour("orange"), channel("sus"), range(0,1,.8)
rslider bounds(.75, .3, .6, .6), text("R"), colour("orange"), channel("rel"), range(0.01,3, .2)
}
groupbox bounds(260, 120, 280, 100), text("ADSR index modulation"), plant("indexmod"){ 
rslider bounds(.0, .3, .6, .6), text("A"), colour("orange"), channel("att"), range(0.01,3, .5)
rslider bounds(.2, .3, .6, .6), text("D"), colour("orange"), channel("dec"), range(0,1, .5)
rslider bounds(.4, .3, .6, .6), text("S"), colour("orange"), channel("sus"), range(0,1,.8)
rslider bounds(.6, .3, .6, .6), text("R"), colour("orange"), channel("rel"), range(0.01,3, .2)
rslider bounds(.8, .3, .6, .6), text("Spread"),channel("spread"), range(0, 1, .5), colour("white")
}


keyboard pos(10, 240), size(530, 60)
</Cabbage>
<CsoundSynthesizer>
<CsOptions>
-d -n -+rtmidi=null -M0 -b1024  -m0d
</CsOptions>
<CsInstruments>

sr = 44100
ksmps = 64
nchnls = 2
0dbfs = 1

;Author: Giorgio Zucco (2012)


instr	1

;channel
kmodfreq	chnget	"mod"
ksemi1	chnget	"semi"
kdetune	chnget	"detune"
kindex	chnget	"index"
kfback	chnget	"fback"
kspread chnget "spread"
iatt	chnget	"att"
idec	chnget	"dec"
isus	chnget	"sus"
irel	chnget	"rel"
iatt2	chnget	"att2"
idec2	chnget	"dec2"
isus2	chnget	"sus2"
irel2	chnget	"rel2"
klfoamp	chnget	"lfoamp"
klforate	chnget	"lforate"
klfoamp2	chnget	"lfoamp2"	;lfo x filter
klforate2	chnget	"lforate2"
;midi
imidinn notnum
iamp	ampmidi	1
kbend	pchbend 0,2  ;pitch bend
;lfo
klfo	lfo	klfoamp,klforate,2
kfreq1 =	cpsmidinn(imidinn+kbend+int(ksemi1))+klfo ;controllo midi
kfreq2 =	cpsmidinn(imidinn+kbend+kdetune+int(ksemi1))+klfo

kindexenv	mxadsr	iatt2,idec2,isus2,irel2	
amod	poscil	kindex*kindexenv,kmodfreq*kfreq1,1	;oscillatore modulante
aphase1	phasor	kfreq1	;generatore di frequenza per tablei
aphase2	phasor	kfreq2	;generatore di frequenza per tablei
acarrier1	init	0	;inizializzo il carrier per il feedback
acarrier2	init	0
acarrier1	tablei	aphase1+amod+(acarrier1*kfback),1,1,0,1	;car+mod+feedbk
acarrier2	tablei	aphase2+amod+(acarrier2*kfback),1,1,0,1

;master
aL	clip	acarrier1,0,0dbfs
aR	clip	acarrier2,0,0dbfs


aoutL = ((aL * kspread) + (aR * (1 - kspread))) *.5  
aoutR = ((aL * (1-kspread)) + (aR * kspread))   *.5

kadsr	mxadsr	iatt,idec,isus,irel

outs	(aoutL*iamp)*kadsr,(aoutR*iamp)*kadsr

endin



</CsInstruments>
<CsScore>
f1	0	16384	10	1

;f0 3600
i1	0	36000
</CsScore>
</CsoundSynthesizer>
