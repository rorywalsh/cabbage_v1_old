<Cabbage>
form caption("clip") size(325, 125)
image pos(5, 5), size(315, 90), colour("Red"), shape("rounded"), outline("white"), line(4) 
rslider bounds(15, 16, 70, 70), text("Limit"), channel("limit"), range(0.001, 1, 1, 0.5,0.001)
rslider bounds(80, 16, 70, 70), text("Argument"), channel("arg"), range(0, 1, 0.5)
combobox bounds(155, 22, 80,25), channel("method"), size(80,50), value(1), text("B.D.J.", "Sine", "Tanh")
rslider bounds(240, 16, 70, 70), text("Level"), channel("level"), range(0, 10, 0.7)
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
	gklimit		chnget	"limit"				;READ WIDGETS...
	gkmethod	chnget	"method"			;
	gkmethod	init	1
	gkmethod	init	i(gkmethod)-1
	gkarg		chnget	"arg"
	gklevel		chnget	"level"				;
	;asigL, asigR	diskin2	"Seashore.wav",1,0,1		;OPTIONALLY USE A BUILT-IN WAVEFORM FOR TESTING
	asigL, asigR	ins
	kSwitch		changed		gklimit, gkmethod, gkarg	;GENERATE A MOMENTARY '1' PULSE IN OUTPUT 'kSwitch' IF ANY OF THE SCANNED INPUT VARIABLES CHANGE. (OUTPUT 'kSwitch' IS NORMALLY ZERO)
	if	kSwitch=1	then	;IF A VARIABLE CHANGE INDICATOR IS RECEIVED...
		reinit	START		;...BEGIN A REINITIALISATION PASS FROM LABEL 'START' 
	endif				;END OF CONDITIONAL BRANCHING
	START:				;LABEL
	aL		clip 		asigL, i(gkmethod), i(gklimit), i(gkarg)
	aR		clip 		asigR, i(gkmethod), i(gklimit), i(gkarg)
	rireturn			;RETURN TO PERFORMANCE PASSES FROM INITIALISATION PASS
			outs		aL*gklevel*(1/(gklimit^0.5)), aR*gklevel*(1/(gklimit^0.5))		;pdclip OUTPUT ARE SENT TO THE SPEAKERS
endin
		
</CsInstruments>

<CsScore>
i 1 0 [3600*24*7]
</CsScore>


</CsoundSynthesizer>



























