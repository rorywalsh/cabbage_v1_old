<Cabbage>
form caption("powershape") size(300, 120)
image pos(0, 0), size(300, 90), colour("Brown"), shape("rounded"), outline("white"), line(4) 
hslider bounds( 10,  0,280, 50), text("Amount"), channel("amount"), range(0.001, 10, 1, 0.5,0.001), colour("grey")
hslider bounds( 10, 40,280, 50), text("Level"), channel("level"), range(0, 50, 0.7, 0.5,0.001), colour("grey")  
}
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

instr	1
	gkshape		chnget	"amount"				;READ WIDGETS...
	gklevel		chnget	"level"					;
	;asigL, asigR	diskin2	"Seashore.wav",1,0,1			;USE SOUND FILE FOR TESTING
	asigL, asigR	ins
	ifullscale	=	0dbfs	;DEFINE FULLSCALE AMPLITUDE VALUE
	aL 		powershape 	asigL, gkshape, ifullscale	;CREATE POWERSHAPED SIGNAL
	aR 		powershape 	asigR, gkshape, ifullscale	;CREATE POWERSHAPED SIGNAL
			outs		aL*gklevel, aR*gklevel		;WAVESET OUTPUT ARE SENT TO THE SPEAKERS
endin
		
</CsInstruments>

<CsScore>
i 1 0 [3600*24*7]
</CsScore>


</CsoundSynthesizer>



























