<Cabbage>
form caption("waveset") size(200, 120)
image pos(0, 0), size(210, 90), colour("Green"), shape("rounded"), outline("white"), line(4) 
rslider bounds(5, 10, 70, 70), text("Repeats"), channel("repeats"), range(1, 100, 1, 0.5, 1)
button channel("reset"), bounds(80, 15, 50,50), text("Reset")
rslider bounds(135, 10, 70, 70), text("Level"), channel("level"), range(0, 1, 0.7)
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
	krep		chnget	"repeats"				;READ WIDGETS...
	klevel		chnget	"level"					;
	kreset		chnget	"reset"					;
	ktrigger	changed	kreset
	if kreset==1 then
	 reinit UPDATE
	endif
	UPDATE:
	;asigL, asigR	diskin2	"Seashore.wav",1,0,1			;USE SOUND FILE FOR TESTING
	asigL, asigR	ins
	aL 		waveset 	asigL,krep,5*60*sr		;PASS THE AUDIO SIGNAL THROUGH waveset OPCODE. Input duration is defined in samples - in this example the expression given equats to a 5 minute buffer
	aR 		waveset 	asigR,krep,5*60*sr		;PASS THE AUDIO SIGNAL THROUGH waveset OPCODE. Input duration is defined in samples - in this example the expression given equats to a 5 minute buffer
	rireturn
			outs		aL*klevel, aR*klevel		;WAVESET OUTPUT ARE SENT TO THE SPEAKERS
endin
		
</CsInstruments>

<CsScore>
i 1 0 [3600*24*7]
</CsScore>


</CsoundSynthesizer>



























