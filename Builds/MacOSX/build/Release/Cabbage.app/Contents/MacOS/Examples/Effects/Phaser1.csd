; phaser1.csd
; Written by Iain McCurdy, 2012.

<Cabbage>
form caption("phaser1") size(430, 90), pluginID("phs1")
image pos(0, 0), size(430, 90), colour(80,20,20), shape("rounded"), outlinecolour("white"), outlinethickness(4) 
label    bounds( 10, 15, 80, 12), text("INPUT");, fontcolour("white")
button   bounds( 10, 30, 80, 30), text("Live","Noise"), channel("input"), value(0)
rslider  bounds( 90, 11, 70, 70),  text("Frequency"), channel("freq"),     range(20.0, 5000, 160, 0.25), colour(160,40,40), textcolour("white"), trackercolour(white)
rslider  bounds(155, 11, 70, 70),  text("Feedback"),  channel("feedback"), range(-0.99, 0.99, 0.9),      colour(160,40,40), textcolour("white"), trackercolour(white)
rslider  bounds(220, 11, 70, 70), text("N.Ords."),   channel("ord"),      range(1, 256, 32, 0.5,1),     colour(160,40,40), textcolour("white"), trackercolour(white)
rslider  bounds(285, 11, 70, 70), text("Mix"),       channel("mix"),      range(0, 1.00, 1),            colour(160,40,40), textcolour("white"), trackercolour(white)
rslider  bounds(350, 11, 70, 70), text("Level"),     channel("level"),    range(0, 1.00, 0.7),          colour(160,40,40), textcolour("white"), trackercolour(white)
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
	gkfeedback	chnget	"feedback"				;
	gkord		chnget	"ord"					;
	gkmix		chnget	"mix"					;
	gklevel		chnget	"level"					;

	gkinput		chnget	"input"					;
	if gkinput==0 then
	 asigL,asigR	ins
	else
	 asigL	pinker
	 asigR	pinker
	endif

	kporttime	linseg	0,0.01,0.03				;CREATE A VARIABLE THAT WILL BE USED FOR PORTAMENTO TIME
	kfreq		portk	gkfreq, kporttime			;PORTAMENTO IS APPLIED TO 'SMOOTH' SLIDER MOVEMENT	
	kSwitch		changed	gkord					;GENERATE A MOMENTARY '1' PULSE IN OUTPUT 'kSwitch' IF ANY OF THE SCANNED INPUT VARIABLES CHANGE. (OUTPUT 'kSwitch' IS NORMALLY ZERO)
	if	kSwitch=1	then					;IF I-RATE VARIABLE CHANGE TRIGGER IS '1'...
		reinit	UPDATE						;BEGIN A REINITIALISATION PASS FROM LABEL 'UPDATE'
	endif								;END OF CONDITIONAL BRANCH
	UPDATE:								;BEGIN A REINITIALISATION PASS FROM HERE
	aphaserl	phaser1		asigL, kfreq, gkord, gkfeedback	;PHASER1 IS APPLIED TO THE LEFT CHANNEL
	aphaserr	phaser1		asigR, kfreq, gkord, gkfeedback	;PHASER1 IS APPLIED TO THE RIGHT CHANNEL
	rireturn							;RETURN FROM REINITIALISATION PASS TO PERFORMANCE TIME PASSES
	amixL		ntrpol	asigL,aphaserl,gkmix
	amixR		ntrpol	asigR,aphaserr,gkmix
			outs	amixL*gklevel, amixR*gklevel		;PHASER OUTPUT ARE SENT TO THE SPEAKERS
endin
		
</CsInstruments>

<CsScore>
i 1 0 [3600*24*7]
</CsScore>


</CsoundSynthesizer>



























