<Cabbage>
form caption("Test Syntheszer") size(550, 369), colour("black"),pluginID("buz1") 
rslider bounds(98, 50, 100, 100) channel("spread"), range(0, 1, .5), caption("Stereo image"), colour("SteelBlue ")
     
groupbox bounds(16, 162, 240, 100), plant("ADSR"), line(1), text("ADSR amplitude"),  colour(75, 85, 90, 100), fontcolour(160, 160, 160, 255), 
rslider bounds(0, 30, 60, 60), text("A"), colour("orange"), channel("att"), range(0.01,3, .5)
rslider bounds(60, 30, 60, 60), text("D"), colour("orange"), channel("dec"), range(0,1, .5)
rslider bounds(120, 30, 60, 60), text("S"), colour("orange"), channel("sus"), range(0,1,.8)
rslider bounds(180, 30, 60, 60), text("R"), colour("orange"), channel("rel"), range(0.01,3, .2)
} 
groupbox bounds(260, 162, 130, 100), plant("lfo"), line(1), text("LFO"),  colour(75, 85, 90, 100), fontcolour(160, 160, 160, 255), 
rslider bounds(5, 30, 60, 60), text("Lfo Amp"), colour("SteelBlue "), channel("lfoamp"), range(0,100, 0)
rslider bounds(65, 30, 60, 60), text("Lfo Rate"), colour("SteelBlue "), channel("lforate"), range(0,20, 0)
}
groupbox bounds(202, 18, 130, 100), text("LFO filter"), plant("lfofilter"){ 
rslider bounds(5, 30, 60, 60), text("Lfo Amp"), colour("SteelBlue "), channel("lfoamp2"), range(0,1000, 0)
rslider bounds(65, 30, 60, 60), text("Lfo Rate"), colour("SteelBlue "), channel("lforate2"), range(0,10, 0)
} 
 
keyboard bounds(20, 276, 361, 60)
       
rslider bounds(16, 16, 70, 70), channel("rslider"), range(0, 10000, 0, 1, 0.0001), text("Attack"), textbox("1"),  colour(0, 154, 255, 255), trackercolour(255, 255, 0, 255), fontcolour(160, 160, 160, 255), 
combobox bounds(98, 18, 100, 30), channel("combobox"), text("Welcome", "To", "The", "Second", "Csound", "Conference"),  colour(46, 20, 20, 255), fontcolour(255, 255, 0, 255), value(1), 
</Cabbage> 
<CsoundSynthesizer>
<CsOptions>
-d -n -+rtmidi=null -M0 -b1024 
</CsOptions>
<CsInstruments>
; Initialize the global variables. 
sr = 44100
ksmps = 32
nchnls = 2
0dbfs = 1

;Author: Giorgio Zucco (2012)


instr	1

;channel
kdetune	chnget	"detune"
ksemi	chnget	"semi"
kcut	chnget	"cutoff"
kharm	chnget	"harm"
kspread chnget "spread"
iatt	chnget	"att"
idec	chnget	"dec"
isus	chnget	"sus"
irel	chnget	"rel"
klfoamp	chnget	"lfoamp"
klforate	chnget	"lforate"
klfoamp2	chnget	"lfoamp2"	;lfo x filter
klforate2	chnget	"lforate2"
;midi
imidinn notnum
iamp	ampmidi	1
kbend	pchbend 0,2  ;pitch bend
kfreq1 = cpsmidinn(imidinn+kbend+int(ksemi)) 
kfreq2 = cpsmidinn(imidinn+kbend+int(ksemi)+kdetune) 
;lfo
klfo	poscil	klfoamp,klforate,1	
asig1	buzz	iamp,(kfreq1+klfo),int(kharm),1
asig2	buzz	iamp,(kfreq2+klfo),int(kharm),1
;filter	
klfofilter	lfo	klfoamp2,klforate2,3
aout1	fofilter	asig1,kcut+klfofilter,0.007,0.04
aout2	fofilter	asig2,kcut+klfofilter,0.007,0.04
aout1x	balance	aout1,asig1
aout2x	balance	aout2,asig2

;master
aL	clip	aout1x,0,0dbfs
aR	clip	aout2x,0,0dbfs


aoutL = ((aL * kspread) + (aR * (1 - kspread))) *.5
aoutR = ((aL * (1-kspread)) + (aR * kspread))  *.5 

kadsr mxadsr	iatt,idec,isus,irel

outs	aoutL*kadsr,aoutR*kadsr

endin



</CsInstruments>
<CsScore>
f1	0	16384	10	1
f2	0	1024	10	1
i1	0	36000
</CsScore>
</CsoundSynthesizer>
