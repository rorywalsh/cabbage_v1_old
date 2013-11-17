<Cabbage>
form caption("Lowpass Filter") size(370, 120), pluginID("LPFl")
image pos(0, 0), size(370, 90), colour("yellow"), shape("rounded"), outline("white"), line(4) 
rslider bounds( 10, 11, 70, 70), text("Freq."),colour("orange"), 		fontcolour("black"), 		channel("cf"), 		range(20, 20000, 20000, 0.333)
rslider bounds( 75, 11, 70, 70), text("Res."),colour("orange"), 		fontcolour("black"), 		channel("res"),		range( 0,     1, 0)
rslider bounds(140, 11, 70, 70), text("Mix"),colour("orange"),		fontcolour("black"), 		channel("mix"), 	range(0, 1, 1)
button  bounds(215,  10, 80, 20), text("24dB/oct", "12dB/oct"), channel("steepness"), value(0)
label   bounds(225,  30, 80, 12), text("Steepness"), FontColour("black")
checkbox bounds(215, 50, 80, 20), channel("ResType"), FontColour("black"),  value(0), text("Resonant"), shape("rounded")
rslider bounds(295, 11, 70, 70), text("Level"),colour("orange"),		fontcolour("black"), 		channel("level"), 	range(0, 1, 1)
}
image bounds(5, 95, 205, 20), colour(75, 85, 90, 100), plant("credit"), line(0){
label bounds(0.03, 0.1, .95, .7), text("Author: Iain McCurdy |2012|"), colour("white")
</Cabbage>

<CsoundSynthesizer>

<CsOptions>
-d -n
</CsOptions>

<CsInstruments>

sr 		= 	44100	;SAMPLE RATE
ksmps 		= 	32	;NUMBER OF AUDIO SAMPLES IN EACH CONTROL CYCLE
nchnls 		= 	2	;NUMBER OF CHANNELS (2=STEREO)
0dbfs		=	1

;Author: Iain McCurdy (2012)

instr	1
	kcf		chnget	"cf"				;
	kres		chnget	"res"				;
	kmix		chnget	"mix"				;
	ksteepness	chnget	"steepness"			;
	kResType	chnget	"ResType"			;
	klevel		chnget	"level"				;
	kporttime	linseg	0,0.001,0.02
	kcf	portk	kcf,kporttime
	aL,aR	ins	
	if ksteepness==0&&kResType!=1 then
	 aFiltL	tone	aL,kcf
	 aFiltR	tone	aR,kcf
        elseif ksteepness==1&&kResType!=1 then
	 aFiltL	butlp	aL,kcf
	 aFiltR	butlp	aR,kcf
        elseif kResType==1 then
	 aFiltL	moogladder	aL,kcf,kres
	 aFiltR	moogladder	aR,kcf,kres        
	endif
	aL	ntrpol	aL,aFiltL,kmix
	aR	ntrpol	aR,aFiltR,kmix
		outs	aL*klevel,aR*klevel
endin
		
</CsInstruments>

<CsScore>
i 1 0 [3600*24*7]
</CsScore>

</CsoundSynthesizer>
