<Cabbage>
form caption("wguide1") size(355, 120)
image pos(0, 0), size(355, 90), colour("Orange"), shape("rounded"), outline("white"), line(4) 
rslider bounds(10, 11, 70, 70), text("Frequency"), fontcolour("black"), channel("freq"), tracker("black"), range(20, 8000, 160, 0.25)
rslider bounds(75, 11, 70, 70), text("Cutoff"), fontcolour("black"),channel("cutoff"), tracker("black"), range(20,20000,8000,0.25)
rslider bounds(140, 11, 70, 70), text("Feedback"), fontcolour("black"),channel("feedback"), tracker("black"), range(-0.999, 0.999, 0.8)
rslider bounds(205, 11, 70, 70), text("Mix"), fontcolour("black"), channel("mix"), tracker("black"), range(0, 1, 0.7)
rslider bounds(270, 11, 70, 70), text("Level"), fontcolour("black"), channel("level"), tracker("black"), range(0, 1, 0.7)
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

;Author: Iain McCurdy (2012)

instr	1
	gkfreq		chnget	"freq"					;READ WIDGETS...
	afreq		interp	gkfreq
	gkcutoff	chnget	"cutoff"				;
	gkfeedback	chnget	"feedback"				;
	gkmix		chnget	"mix"					;
	gklevel		chnget	"level"					;
	;asigL, asigR	diskin2	"Seashore.wav",1,0,1			;USE SOUND FILE FOR TESTING
	asigL, asigR	ins
	kporttime	linseg	0,0.01,0.03				;CREATE A VARIABLE THAT WILL BE USED FOR PORTAMENTO TIME
	aresL 		wguide1 asigL, afreq, gkcutoff, gkfeedback
	aresR 		wguide1 asigR, afreq, gkcutoff, gkfeedback
	amixL		ntrpol	asigL,aresL,gkmix
	amixR		ntrpol	asigR,aresR,gkmix

			outs	amixL*gklevel, amixR*gklevel		;WGUIDE1 OUTPUTS ARE SENT OUT
endin
		
</CsInstruments>

<CsScore>
i 1 0 [3600*24*7]
</CsScore>


</CsoundSynthesizer>



























