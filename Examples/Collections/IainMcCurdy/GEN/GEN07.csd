Demonstration of GEN07
Written by Iain McCurdy, 2014

GEN07 generates breakpoint functions by joining user-defined values using straight lines each of user-definable duration.

An audio test generator uses this function table as a repeating amplitude envelope. 
The offset value is subtracted so that the envelope can experience values of zero. 

<Cabbage>
form caption("GEN07"), size(245, 370), pluginID("gn07"), colour(13, 50, 67,50)

table bounds( 10,  5, 225, 120), channel("table"), tableNumbers(1), tablecolour("yellow"), identchannel("table"), amprange(0,1.1)

rslider bounds( 10,130, 50, 70), channel("val1"), text("Value.1"), textBox(1), range(0, 1, 0), colour(100,130,130,250), trackercolour("silver")
rslider bounds( 70,130, 50, 70), channel("val2"), text("Value.2"), textBox(1), range(0, 1, 1), colour(100,130,130,250), trackercolour("silver")
rslider bounds(130,130, 50, 70), channel("val3"), text("Value.3"), textBox(1), range(0, 1, 0), colour(100,130,130,250), trackercolour("silver")

rslider bounds( 40,205, 50, 70), channel("dur1"), text("Dur.1"), textBox(1), range(0, 4096, 96, 1, 1), colour(130,100,130,250), trackercolour("silver")
rslider bounds(100,205, 50, 70), channel("dur2"), text("Dur.2"), textBox(1), range(0, 4096, 4000, 1, 1), colour(130,100,130,250), trackercolour("silver")
              
line    bounds( 10,280,225,  2), colour("Grey")
checkbox bounds( 15,290,80, 17), channel("TestGen"), text("Test"),  value(1), colour("yellow"), shape("square")
rslider  bounds( 60,290, 70, 70), channel("speed"), text("Speed"), textBox(1), range(0.25, 8.00, 1,0.5,0.001),   colour(250,230,250,250), trackercolour("silver")
rslider  bounds(120,290, 70, 70), channel("freq"),  text("Freq."), textBox(1), range(50, 5000, 500,0.5,0.1),     colour(250,230,250,250), trackercolour("silver")
rslider  bounds(180,290, 70, 70), channel("lev"),   text("Level"), textBox(1), range(0, 1.00, 0.5),              colour(250,230,250,250), trackercolour("silver")

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

instr	1
	; read in widgets
	gkval1	chnget	"val1"
	gkval2	chnget	"val2"
	gkval3	chnget	"val3"

	gkdur1	chnget	"dur1"
	gkdur2	chnget	"dur2"
	
	; if any of the variables in the input list are changed, a momentary '1' trigger is generated at the output. This trigger is used to update function tables.
	ktrig	changed		gkval1,gkval2,gkval3,gkdur1,gkdur2
	if ktrig==1 then
	 reinit	UPDATE
	endif
	UPDATE:
	; Update function table
	  if	(i(gkdur1)+i(gkdur2))>4096 then				; if sum of segments exceeds table size...
	   idur1	=	i(gkdur1)* (4096/(i(gkdur1)+i(gkdur2)))	; ...scale segment durations down
	   idur2	=	i(gkdur2)* (4096/(i(gkdur1)+i(gkdur2)))
	   irem		=	0					; remainder duration of table
	  else								; if sum of segments is less than table size...
	   idur1	=	i(gkdur1)
	   idur2	=	i(gkdur2)
	   irem		=	4096 - (i(gkdur1) + i(gkdur2))		; remainder duration of table
	  endif
	gi1	ftgen	1, 0,   4096, -7, i(gkval1), idur1, i(gkval2), idur2, i(gkval3), irem, 0
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
; create the function table
f 1 0    4096 -16  1 0 0
; play instrument 1 for 1 hour
i 1 0 3600
</CsScore>

</CsoundSynthesizer>
