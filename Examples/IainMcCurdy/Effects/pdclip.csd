<Cabbage>
form caption("pdclip") size(315, 120)
label pos(0, -8), size(315, 90), colour(20,100,100,200), fontcolour(70,150,150,200), text("pdclip"), shape("rounded"), outline("white"), line(4) 
image pos(0, 0), size(315, 90), colour(10,100,200,200), shape("rounded"), outline("white"), line(4) 
rslider bounds(10, 11, 70, 70), text("Width"), channel("width"), range(0, 1, 0),fontcolour("black")
rslider bounds(75, 11, 70, 70), text("Centre"), channel("center"), range(-1, 1, 0),fontcolour("black")
combobox bounds(150, 20, 80,25), channel("bipolar"), size(80,50), value(2), text("Unipolar", "Bipolar")
rslider bounds(235, 11, 70, 70), text("Level"), channel("level"), range(0, 1, 0.7),fontcolour("black")
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
	gkwidth		chnget	"width"				;READ WIDGETS...
	gkcenter	chnget	"center"			;
	gkbipolar	chnget	"bipolar"			;
	gkbipolar	init	1
	gkbipolar	=	gkbipolar-1
	gklevel		chnget	"level"				;
	;asigL, asigR	diskin2	"Seashore.wav",1,0,1		;USE SOUND FILE FOR TESTING
	asigL, asigR	ins
	ifullscale	=	0dbfs	;DEFINE FULLSCALE AMPLITUDE VALUE
	kSwitch		changed		gkbipolar	;GENERATE A MOMENTARY '1' PULSE IN OUTPUT 'kSwitch' IF ANY OF THE SCANNED INPUT VARIABLES CHANGE. (OUTPUT 'kSwitch' IS NORMALLY ZERO)
	if	kSwitch=1	then	;IF A VARIABLE CHANGE INDICATOR IS RECEIVED...
		reinit	START		;...BEGIN A REINITIALISATION PASS FROM LABEL 'START' 
	endif				;END OF CONDITIONAL BRANCHING
	START:				;LABEL
	;CLIP THE AUDIO SIGNAL USING pdclip
	aL		pdclip		asigL, gkwidth, gkcenter, i(gkbipolar)	; [, ifullscale]]
	aR		pdclip		asigR, gkwidth, gkcenter, i(gkbipolar)	; [, ifullscale]]
	rireturn			;RETURN TO PERFORMANCE PASSES FROM INITIALISATION PASS
			outs		aL*gklevel, aR*gklevel		;pdclip OUTPUT ARE SENT TO THE SPEAKERS
endin
		
</CsInstruments>

<CsScore>
i 1 0 [3600*24*7]
</CsScore>


</CsoundSynthesizer>



























