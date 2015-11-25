; Exciter.csd
; Written by Iain McCurdy, 2015.

<Cabbage>
form caption("Exciter") size(370, 100), pluginID("scho")
image           bounds(0, 0, 370, 100), colour("LightSlateGrey"), shape("rounded"), outlinecolour("white"), outlinethickness(6)
rslider  bounds( 10, 13, 75, 75), text("Frequency"), channel("freq"), range(20,20000,3000,0.5),  textcolour("white"), colour(37,59,59), trackercolour("Silver")
rslider  bounds( 80, 13, 75, 75), text("Ceiling"), channel("ceil"), range(20,20000,20000,0.5), textcolour("white"), colour(37,59,59), trackercolour("Silver")
rslider  bounds(150, 13, 75, 75), text("Harmonics"), channel("harms"), range(0.1,10.00,10),     textcolour("white"), colour(37,59,59), trackercolour("Silver")
rslider  bounds(220, 13, 75, 75), text("Blend"), channel("blend"), range(-10,10,10,0.5,0.0001),     textcolour("white"), colour(37,59,59), trackercolour("Silver")
rslider  bounds(290, 13, 75, 75), text("Level"), channel("level"), range(0, 1.00, 0.5),      textcolour("white"), colour(37,59,59), trackercolour("Silver")
</Cabbage>

<CsoundSynthesizer>

<CsOptions>
-d -n
</CsOptions>

<CsInstruments>
sr = 44100
ksmps = 32
nchnls = 2
0dbfs = 1



instr 1
 kporttime	linseg	0,0.001,0.05                                                     
 kfreq	chnget "freq"
 kceil	chnget "ceil"
 kharms chnget "harms"
 kblend	chnget "blend"
 klevel chnget "level"
 kmix  	chnget "mix"
 kmix	portk	kmix,kporttime
 klevel	portk	klevel,kporttime
 
 a1,a2	ins
 a1,a2	diskin2	"ClassicalGuitar.wav",1,0,1
 
 a1	exciter a1, kfreq, kceil, kharms, kblend
 a2	exciter a1, kfreq, kceil, kharms, kblend

 a1	=	a1 * klevel
 a2	=	a2 * klevel
 	outs	a1,a2
endin

</CsInstruments>

<CsScore>                                              
i 1 0 [60*60*24*7]
</CsScore>

</CsoundSynthesizer>                                                  