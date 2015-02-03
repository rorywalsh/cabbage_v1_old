Demonstration of GEN07
Written by Iain McCurdy, 2014

GEN07 generates breakpoint functions by joining user-defined values using straight lines each of user-definable duration.

An audio test generator uses this function table as a repeating amplitude envelope. 
The offset value is subtracted so that the envelope can experience values of zero. 

<Cabbage>
form caption("GEN07"), size(250, 375), pluginID("gn07"), colour(13, 50, 67,50)

gentable bounds( 15,  5, 225, 120), tablenumbers(1), tablecolour("blue"), tablebackgroundcolour("white"), tablegridcolour(230,230,230), identchannel("table"), amprange(0,1,1), zoom(-1)
label bounds(0,1,15,9), text("1")
label bounds(0,13,15,9), text("0.9")
label bounds(0,25,15,9), text("0.8")
label bounds(0,37,15,9), text("0.7")
label bounds(0,49,15,9), text("0.6")
label bounds(0,61,15,9), text("0.5")
label bounds(0,73,15,9), text("0.4")
label bounds(0,85,15,9), text("0.3")
label bounds(0,97,15,9), text("0.2")
label bounds(0,109,15,9), text("0.1")
label bounds(0,121,15,9), text("0")
label bounds(31,126,15,9), text("0.1")
label bounds(54,126,15,9), text("0.2")
label bounds(77,126,15,9), text("0.3")
label bounds(99,126,15,9), text("0.4")
label bounds(122,126,15,9), text("0.5")
label bounds(145,126,15,9), text("0.6")
label bounds(167,126,15,9), text("0.7")
label bounds(189,126,15,9), text("0.8")
label bounds(212,126,15,9), text("0.9")
label bounds(232,126,15,9), text("1")

rslider bounds( 10,135, 50, 70), channel("val1"), text("Value.1"), textBox(1), range(0, 1, 0), colour(100,130,130,250), trackercolour("silver")
rslider bounds( 70,135, 50, 70), channel("val2"), text("Value.2"), textBox(1), range(0, 1, 1), colour(100,130,130,250), trackercolour("silver")
rslider bounds(130,135, 50, 70), channel("val3"), text("Value.3"), textBox(1), range(0, 1, 0), colour(100,130,130,250), trackercolour("silver")

rslider bounds( 40,210, 50, 70), channel("dur1"), text("Dur.1"), textBox(1), range(0, 1, 0.2, 1, 0.001), colour(130,100,130,250), trackercolour("silver")
rslider bounds(100,210, 50, 70), channel("dur2"), text("Dur.2"), textBox(1), range(0, 1, 0.8, 1, 0.001), colour(130,100,130,250), trackercolour("silver")
              
line     bounds( 10,285,225,  2), colour("Grey")
checkbox bounds( 15,295,80, 17), channel("TestGen"), text("Test"),  value(1), colour("yellow"), shape("square")
rslider  bounds( 60,295, 70, 70), channel("speed"), text("Speed"), textBox(1), range(0.25, 8.00, 1,0.5,0.001),   colour(250,230,250,250), trackercolour("silver")
rslider  bounds(120,295, 70, 70), channel("freq"),  text("Freq."), textBox(1), range(50, 5000, 500,0.5,0.1),     colour(250,230,250,250), trackercolour("silver")
rslider  bounds(180,295, 70, 70), channel("lev"),   text("Level"), textBox(1), range(0, 1.00, 0.5),              colour(250,230,250,250), trackercolour("silver")

</Cabbage>
                    
<CsoundSynthesizer>

<CsOptions>   
-dm0 -n -+rtmidi=null -M0
</CsOptions>

<CsInstruments>

sr 		= 	44100	;SAMPLE RATE
ksmps 		= 	32	;NUMBER OF AUDIO SAMPLES IN EACH CONTROL CYCLE
nchnls 		= 	2	;NUMBER OF CHANNELS (1=MONO)
0dbfs		=	1	;MAXIMUM AMPLITUDE

giTabLen	=	1024

instr	1
	; read in widgets
	gkval1	chnget	"val1"
	gkval2	chnget	"val2"
	gkval3	chnget	"val3"

	gkdur1	chnget	"dur1"
	gkdur2	chnget	"dur2"
	
	gkdur1	*=	giTabLen
	gkdur2	*=	giTabLen
	
	; if any of the variables in the input list are changed, a momentary '1' trigger is generated at the output. This trigger is used to update function tables.
	ktrig	changed		gkval1,gkval2,gkval3,gkdur1,gkdur2
	if ktrig==1 then
	 reinit	UPDATE
	endif
	UPDATE:
	; Update function table
	  if	(i(gkdur1)+i(gkdur2))>giTabLen then				; if sum of segments exceeds table size...
	   idur1	=	i(gkdur1)* (giTabLen/(i(gkdur1)+i(gkdur2)))	; ...scale segment durations down
	   idur2	=	i(gkdur2)* (giTabLen/(i(gkdur1)+i(gkdur2)))
	   irem		=	0					; remainder duration of table
	  else								; if sum of segments is less than table size...
	   idur1	=	i(gkdur1)
	   idur2	=	i(gkdur2)
	   irem		=	giTabLen - (i(gkdur1) + i(gkdur2))		; remainder duration of table
	  endif
	gi1	ftgen	1, 0,   giTabLen, -7, i(gkval1), idur1, i(gkval2), idur2, i(gkval3), irem, 0
	chnset	"tablenumber(1)", "table"	; update table display	
	rireturn
	

	kTestGen	chnget	"TestGen"				; test generator on/off
	kspeed	chnget	"speed"
	kamp	chnget	"lev"
	kfreq	chnget	"freq"
	aphasor	phasor	kspeed
	aenv	tablei	aphasor,gi1,1
	asig	vco2	0.4*kamp*kTestGen,kfreq,4,0.5			; triangle audio wave
	asig	=	asig * aenv					; 
		outs	asig, asig
endin

</CsInstruments>

<CsScore>
; play instrument 1 for 1 hour
i 1 0 3600
</CsScore>

</CsoundSynthesizer>
