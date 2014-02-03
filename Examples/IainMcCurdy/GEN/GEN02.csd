; GEN02
; demonstration of writing to GEN02 function tables and also a demonstration of Cabbage's 'table' widget
; the example creates an 8-note sequence of notes which ccan be played and looped in various ways 

; Note.1 - 8	the note numbers for each of the 8 notes in the sequence
; Vel.1 - 8	the velocities for each of the 8 notes in the sequence (essentially the starting value for the cutoff frequency of a resonant lowpass filter applied to each note)
; Speed.1 - 8	speed multipliers for each note in the sequence. Think of 1 as crotchets, 2 as quavers, 3 as triplet quavers and 4 as semiquavers
; Harm.		factor which controls a frequency shift applied to each note. Value here is arbitrary as freq shift is also dependent upon note number. Even number will produce harmonic results, odd numbers inharmonic results. Zero will result in no frequency shift.
; Filt.		shifts all filter cutoff envelopes up or down. (Zero = no shift). Value here is in octaves.
; Res.		resonance of the moogladder lowpass filter
; Dur.		duration of the filter envelope applied to each note (in seconds).
; Echo		amount of synth signal sent to the echo (delay) effect
; Repeats	number of echo/delay repeats. Actually the amount of feedback in the delay effect.
; Time		time spacing of echos. Related to tempo and numbers here are arbitary. 0 = 1/4 beat, 1 = 1/2 beat, 2 = 3/4 beat, 3 = 1 beat, 4 = 5/4 beats and so on
; Direction	three-way toggle switch to choose direction of the sequence looping: Forward, Forward/Backward or Backward
; Tempo		in beats per minute

<Cabbage>
form caption("GEN02"), size(410, 450), colour( 50, 50, 50),pluginID("gn02")
checkbox bounds(  0,  0,  10, 10), channel("Act1"),  value(0), colour("white"), shape("ellipse")
checkbox bounds( 50,  0,  10, 10), channel("Act2"),  value(0), colour("white"), shape("ellipse")
checkbox bounds(100,  0,  10, 10), channel("Act3"),  value(0), colour("white"), shape("ellipse")
checkbox bounds(150,  0,  10, 10), channel("Act4"),  value(0), colour("white"), shape("ellipse")
checkbox bounds(200,  0,  10, 10), channel("Act5"),  value(0), colour("white"), shape("ellipse")
checkbox bounds(250,  0,  10, 10), channel("Act6"),  value(0), colour("white"), shape("ellipse")
checkbox bounds(300,  0,  10, 10), channel("Act7"),  value(0), colour("white"), shape("ellipse")
checkbox bounds(350,  0,  10, 10), channel("Act8"),  value(0), colour("white"), shape("ellipse")
                                                                                  
table bounds(  5, 10, 400, 115), channel("notetable","amptable","speedtable"), tableNumbers(1,2,3), tablecolour("yellow","green","blue")

rslider bounds(  5,130, 50, 50), channel("note1"), text("Note.1"), range(0, 108, 48, 1, 1),colour("yellow")  
rslider bounds( 55,130, 50, 50), channel("note2"), text("Note.2"), range(0, 108, 50, 1, 1),colour("yellow")
rslider bounds(105,130, 50, 50), channel("note3"), text("Note.3"), range(0, 108, 46, 1, 1),colour("yellow")
rslider bounds(155,130, 50, 50), channel("note4"), text("Note.4"), range(0, 108, 48, 1, 1),colour("yellow")
rslider bounds(205,130, 50, 50), channel("note5"), text("Note.5"), range(0, 108, 54, 1, 1),colour("yellow")
rslider bounds(255,130, 50, 50), channel("note6"), text("Note.6"), range(0, 108, 53, 1, 1),colour("yellow")
rslider bounds(305,130, 50, 50), channel("note7"), text("Note.7"), range(0, 108, 51, 1, 1),colour("yellow")
rslider bounds(355,130, 50, 50), channel("note8"), text("Note.8"), range(0, 108, 42, 1, 1),colour("yellow")
rslider bounds(  5,190, 50, 50), channel("amp1"), text("Vel.1"), range(0, 1.00, 0.8),colour("green")
rslider bounds( 55,190, 50, 50), channel("amp2"), text("Vel.2"), range(0, 1.00, 0.37),colour("green")
rslider bounds(105,190, 50, 50), channel("amp3"), text("Vel.3"), range(0, 1.00, 0.4),colour("green")
rslider bounds(155,190, 50, 50), channel("amp4"), text("Vel.4"), range(0, 1.00, 0.7),colour("green")
rslider bounds(205,190, 50, 50), channel("amp5"), text("Vel.5"), range(0, 1.00, 0.4),colour("green")
rslider bounds(255,190, 50, 50), channel("amp6"), text("Vel.6"), range(0, 1.00, 0.8),colour("green")
rslider bounds(305,190, 50, 50), channel("amp7"), text("Vel.7"), range(0, 1.00, 0.45),colour("green")
rslider bounds(355,190, 50, 50), channel("amp8"), text("Vel.8"), range(0, 1.00, 0.5),colour("green")
rslider bounds(  5,250, 50, 50), channel("speed1"), text("Speed.1"), range(1, 4, 2,1,1),colour("blue")
rslider bounds( 55,250, 50, 50), channel("speed2"), text("Speed.2"), range(1, 4, 2,1,1),colour("blue")
rslider bounds(105,250, 50, 50), channel("speed3"), text("Speed.3"), range(1, 4, 2,1,1),colour("blue")
rslider bounds(155,250, 50, 50), channel("speed4"), text("Speed.4"), range(1, 4, 1,1,1),colour("blue")
rslider bounds(205,250, 50, 50), channel("speed5"), text("Speed.5"), range(1, 4, 1,1,1),colour("blue")
rslider bounds(255,250, 50, 50), channel("speed6"), text("Speed.6"), range(1, 4, 2,1,1),colour("blue")
rslider bounds(305,250, 50, 50), channel("speed7"), text("Speed.7"), range(1, 4, 1,1,1),colour("blue")
rslider bounds(355,250, 50, 50), channel("speed8"), text("Speed.8"), range(1, 4, 1,1,1),colour("blue")
rslider bounds( 15,310, 50, 55), channel("fshift"), text("Harm."), range(0, 32, 0,1,1), colour("red")
rslider bounds( 70,310, 50, 55), channel("filt"), text("Filt."), range(-4.00, 4.00, 0), colour("red")
rslider bounds(125,310, 50, 55), channel("res"), text("Res."), range(0, 0.99, 0.7), colour("red")
rslider bounds(180,310, 50, 55), channel("dur"), text("Dur."), range(0.10, 4, 1,0.5), colour("red")
rslider bounds(235,310, 50, 55), channel("echo"), text("Echo"), range(0, 1.00, 0.3), colour("pink")
rslider bounds(290,310, 50, 55), channel("rpts"), text("Repeats"), range(0, 1.00, 0.4), colour("pink")
rslider bounds(345,310, 50, 55), channel("time"), text("Time"), range(0, 7, 3,1,1), colour("pink")
rslider bounds(345,370, 50, 55), channel("amp"), text("Level"), range(0, 1.00, 0.6), colour("orange")
vslider bounds( 25,357,50,  60), text(""), channel("dir"), fontcolour("black"), range(0, 2, 1,1,1), colour("white"), trackercolour("black")
label    bounds( 50,370, 55, 12), text("Bwd"),  FontColour("white")
label    bounds( 50,386, 55, 12), text("Fwd/Bwd"),  FontColour("white")
label    bounds( 50,403, 55, 12), text("Fwd"),  FontColour("white")
hslider bounds(105,370, 240, 45), channel("tempo"),range(10,500,150, 1, 1), colour(150,150,150), trackercolour("black")
label    bounds(205,402, 55, 14), text("Tempo"),  FontColour("white")
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

gisine		ftgen	0,0,4096,10,1

; tables for note numbers, velocities and speeds
ginotes		ftgen	1,0, 8,-2, 0
giamps		ftgen	2,0, 8,-2, 0
gispeeds	ftgen	3,0, 8,-2, 0

/* UDOs */
opcode	scale_i, i, iii					; i-rate version of the 'scale' opcode
	ival,imax,imin	xin
	ival		=	(ival*(imax-imin))+imin
			xout	ival
endop

opcode	FreqShifter,a,aki					; frequency shifter
	ain,kfshift,ifn	xin					;READ IN INPUT ARGUMENTS
	areal, aimag hilbert ain				;HILBERT OPCODE OUTPUTS TWO PHASE SHIFTED SIGNALS, EACH 90 OUT OF PHASE WITH EACH OTHER
	asin 	oscili       1,    kfshift,     ifn,          0
	acos 	oscili       1,    kfshift,     ifn,          0.25	
	;RING MODULATE EACH SIGNAL USING THE QUADRATURE OSCILLATORS AS MODULATORS
	amod1	=		areal * acos
	amod2	=		aimag * asin	
	;UPSHIFTING OUTPUT
	aFS	= (amod1 - amod2)
		xout	aFS				;SEND AUDIO BACK TO CALLER INSTRUMENT
endop

instr	1						; update function tables and send to Cabbage table display
; define a macro to update a function table and its display (using a macro avoids code repetition)
#define	AttributeSet(NAME'NUM'MIN'MAX)
	#
	k$NAME1	chnget	"$NAME1"	; read in widgets
	k$NAME2	chnget	"$NAME2"
	k$NAME3	chnget	"$NAME3"
	k$NAME4	chnget	"$NAME4"
	k$NAME5	chnget	"$NAME5"
	k$NAME6	chnget	"$NAME6"
	k$NAME7	chnget	"$NAME7"
	k$NAME8	chnget	"$NAME8"
	ktrig	changed	k$NAME1,k$NAME2,k$NAME3,k$NAME4,k$NAME5,k$NAME6,k$NAME7,k$NAME8	; if any of the input args change, generate a trigger
	if ktrig==1 then       								; if trigger created...
	 reinit Update$NAME								; reinit from label
	endif                                                         
	Update$NAME:									; label
	 i$NAME1	init	(i(k$NAME1)-($MIN))/($MAX-$MIN)				; rescale widget variable to be within the range 0 - 1 (needed for well displayed tables)
	 i$NAME2	init	(i(k$NAME2)-($MIN))/($MAX-$MIN)
	 i$NAME3	init	(i(k$NAME3)-($MIN))/($MAX-$MIN)
	 i$NAME4	init	(i(k$NAME4)-($MIN))/($MAX-$MIN)
	 i$NAME5	init	(i(k$NAME5)-($MIN))/($MAX-$MIN)
	 i$NAME6	init	(i(k$NAME6)-($MIN))/($MAX-$MIN)
	 i$NAME7	init	(i(k$NAME7)-($MIN))/($MAX-$MIN)
	 i$NAME8	init	(i(k$NAME8)-($MIN))/($MAX-$MIN)
	 gi$NAMEs	ftgen	$NUM,0, 8,-2, i$NAME1, i$NAME2, i$NAME3, i$NAME4, i$NAME5, i$NAME6, i$NAME7, i$NAME8	; update function table
	 ktrigr	=	-1								; table update trigger
	 chnset	ktrigr, "$NAMEtable"							; update table display
	rireturn									; return from reinit
	#
	$AttributeSet(note'1'0'108)							; expand macro (args: name label, table number, minimum and maximum) 
	$AttributeSet(amp'2'0'1)
	$AttributeSet(speed'3'1'4)
endin
                  
instr	2							; play note sequence
	gktempo	chnget	"tempo"					; read tempo from widget
	gkspeed	init	1					; initialise speed value (may be changed later in instrument 3)
	gindx	init	0					; initialise index to the start of the sequence
	ginotecount	init	0				; initialise note counter (counts all the notes that have occured without wrapping)
	ktrig	metro	(gktempo*gkspeed)/60			; metronome
	schedkwhen	ktrig,0,0,3,0,-1			; trigger instr 3
	event_i "i",4,0,3600					; start sound producing instrument (stays on always)
endin

instr	3
	inote	table	gindx,ginotes				; read note number from table (range: 0 - 1)
	inote	scale_i	inote,108,0				; rescale to the range -1 to 127
	gknote	init	inote             			; set global krate variable for note number
	iamp	table	gindx,giamps				; read amplitude from table (range: 0 - 1)
	gkamp	init	iamp					; set global krate variable for amplitude
	ispeed	table	gindx,gispeeds        			; read speed from table (range: 0 - 1)
	ispeed	scale_i	ispeed,4,1				; rescale to the range 1 - 4
	gkspeed	init	ispeed					; set global krate variable for speed
	idur	chnget	"dur"					; read envelope duration from table
	gkcf	expseg	inote+(60*iamp),idur,inote,1,inote	; create filter cutoff envelope
	kfilt	chnget	"filt"					; read filter envelope shift from widget
	gkcf	limit	cpsmidinn(gkcf+(12*kfilt)),20,20000	; convert envelope from note number to CPS, shift up or down and limit to prevent out of range values
	idir	chnget	"dir"					; read sequence direction from widget

	if(gindx==0) then					; set progress indicators
	 chnset	0,"Act8"					; set indicator 'off'
	 chnset	1,"Act1"					; set indicator 'on'
	 chnset	0,"Act2"					; set indicator 'off'
	elseif(gindx==1) then
	 chnset	0,"Act1"					; set indicator 'off'
	 chnset	1,"Act2"                                        ; set indicator 'on' 
	 chnset	0,"Act3"                                        ; set indicator 'off'
	elseif(gindx==2) then
	 chnset	0,"Act2"					; set indicator 'off'
	 chnset	1,"Act3"                                        ; set indicator 'on' 
	 chnset	0,"Act4"                                        ; set indicator 'off'
	elseif(gindx==3) then
	 chnset	0,"Act3"					; set indicator 'off'
	 chnset	1,"Act4"                                        ; set indicator 'on' 
	 chnset	0,"Act5"                                        ; set indicator 'off'
	elseif(gindx==4) then
	 chnset	0,"Act4"					; set indicator 'off'
	 chnset	1,"Act5"                                        ; set indicator 'on' 
	 chnset	0,"Act6"                                        ; set indicator 'off'
	elseif(gindx==5) then
	 chnset	0,"Act5"					; set indicator 'off'
	 chnset	1,"Act6"                                        ; set indicator 'on' 
	 chnset	0,"Act7"                                        ; set indicator 'off'
	elseif(gindx==6) then
	 chnset	0,"Act6"					; set indicator 'off'
	 chnset	1,"Act7"                                        ; set indicator 'on' 
	 chnset	0,"Act8"                                        ; set indicator 'off'
	elseif(gindx==7) then
	 chnset	0,"Act7"					; set indicator 'off'
	 chnset	1,"Act8"                                        ; set indicator 'on' 
	 chnset	0,"Act1"                                        ; set indicator 'off'
	endif	  

	/* PRINT SCRUBBER (Doesn't work on OSX)*/
	ktrig	init	gindx/ftlen(ginotes)
	chnset	ktrig,"notetable"
	
	/* SHIFT INDEX FOR NEXT NOTE */
	if(idir==0) then					; FWD
	 ginotecount	=	ginotecount+1			; increment note index
	 gindx	wrap	ginotecount,0,ftlen(ginotes)		; wrap out of range values
	 gindx	=	int(gindx)
	elseif(idir==1) then					; FWD/BWD
	 ginotecount	=	ginotecount+1			; increment note index 
	 gindx	mirror	ginotecount,0,ftlen(ginotes)-1		; mirror out of range values
	 gindx	=	int(gindx)
	else							; BWD
	 ginotecount	=	ginotecount-1			; decrement note index 
	 gindx	wrap	ginotecount,-0.5,ftlen(ginotes)-0.5	; wrap out of range values
	 gindx	=	int(gindx)
	endif
endin

instr	4
	kres		chnget	"res"				; read in widgets...
	klev		chnget	"amp"
	kecho		chnget	"echo"
	krpts		chnget	"rpts"
	kfshift		chnget	"fshift"
	kporttime	linseg	0,0.001,1			; portamento time ramps up quickly from zero, holds at '1'                                 
	knote	portk	gknote,kporttime*0.001			; portamento smoothing to note number changes         
	kcf	portk	gkcf,kporttime*0.001			; portamento smoothing to filter cutoff frequency (prevents clicks resulting from discontinuities)
	kamp	portk	gkamp,kporttime				; portamento smoothing to amplitude (prevents clicks resulting from discontinuities)
	a1	vco2	gkamp*klev,cpsmidinn(knote),0,0.5	; VCO audio signal generator
	a1	moogladder	a1,kcf,kres			; moogladder lowpass filter                                                                                                                                                                                                                
	a1	FreqShifter	a1,cpsmidinn(knote)*kfshift*0.5,gisine	; frequency shift applied to audio signal (using a UDO: see above). Frequency is a function of note number of the sequence and the on-screen control 'harm.'
	idry	ftgen	0,0,1024,7,1,512,1,512,0		; table used to shape amplitude control of the 'dry' signal level
	kdry	table	kecho,idry,1				; read 'dry' signal level
	a1	=	a1 * klev * kdry			; scale audio signal with 'Level' and 'Echo' controls
	aL	delay			a1,0.002		; slightly delay audio signal (used to create a stereo effect)
		outs	aL,a1					; send audio to outputs (left channel slightly delayed)
	
	/* DELAY EFFECT */
	ktime	chnget	"time"					; read delay time from widget (arbitrary value)
	itimes	ftgen	0,0,8, -2, 0.25, 0.5, 0.75, 1, 1.25, 1.5, 1.75, 2	; table of delay time multipliers
	kmult	table	ktime,itimes				; read delay time multiplier
	ktempo	portk	gktempo,kporttime			; long portamento smoothing to changes in tempo
	kmult	portk	kmult,kporttime*0.001			; very short portamento smoothing to changes to delay time using the 'Time' widget
	kdeltim	=	(60/ktempo)*kmult			; calculate delay time
	adeltim	interp	kdeltim					; convert to a-rate with interpolation  
	abuf	delayr	(60*2)/10				; create an audio delay buffer
	atap	deltapi	adeltim					; tap delay buffer
		delayw	(a1*kecho)+(atap*krpts)			; write audio into delay buffer. Add in a bit of feedback
	aR	delay	atap,0.002				; slightly delay audio signal (used to create a stereo effect)
		outs	atap,aR					; send audio to outputs (right channel slightly delayed)
endin                                                                                                                     


</CsInstruments>

<CsScore>
i 1 0 [3600*24*7]		; instrument to update function tables and table displays
i 2 0.001 [3600*24*7]		; instrument to play note sequence
</CsScore>                            

</CsoundSynthesizer>
