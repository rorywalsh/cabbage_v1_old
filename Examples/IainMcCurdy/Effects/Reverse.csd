<Cabbage>
form caption("Reverse") size(255, 135)
image bounds(5, 5, 245, 95), colour("darkslategrey"), shape("rounded"), outline("white"), line(6)
rslider bounds( 10, 15,  75, 75), text("Time"), channel("time"), range(0.1, 4, 1, 0.5)
rslider bounds( 80, 15, 75, 75), text("Level"), channel("level"), range(0, 1, 1, 0.5)
checkbox bounds(160, 35, 100, 25), channel("reverse"),  value(1), text("Reverse"), shape("ellipse")
}
</Cabbage>
<CsoundSynthesizer>
<CsOptions>
-d -n
</CsOptions>
<CsInstruments>
sr = 44100
ksmps = 32
nchnls = 2
0dbfs = 1

;Author: Iain McCurdy (2012)
;http://iainmccurdy.org/csound.html

opcode	Reverse, a, aK				;nb. CAPITAL K CREATE A K-RATE VARIABLE THAT HAS A USEFUL VALUE ALSO AT I-TIME
	ain,ktime	xin			;READ IN INPUT ARGUMENTS
	;four windowing envelopes. An appropriate one will be chosen based on the reversed chunk duration
	ienv1	ftgentmp			0, 0, 131072, 7, 0, 1024,           1, 131072-(1024*2),            1,  1024,      0	;for longest chunk times
	ienv2	ftgentmp			0, 0, 131072, 7, 0, 4096,           1, 131072-(4096*2),           1,  4096,       0
	ienv3	ftgentmp			0, 0, 131072, 7, 0,16384,           1, 131072-(16384*2),          1, 16384,       0
	ienv4	ftgentmp			0, 0, 131072, 7, 0,32768,           1, 131072-(32768*2),          1, 32768,       0	;for shortest chunk times
	ktrig	changed	ktime			;IF ktime CONTROL IS MOVED GENERATE A MOMENTARY '1' IMPULSE
	if ktrig=1 then				;IF A TRIGGER HAS BEEN GENERATED IN THE LINE ABOVE...
		reinit	UPDATE			;...BEGIN A REINITILISATION PASS FROM LABEL 'UPDATE'
	endif					;END OF CONDITIONAL BRANCH
	UPDATE:					;LABEL CALLED 'UPDATE'
	itime	=	i(ktime)		;CREATE AN I-TIME VERSION OF ktime
	aptr	phasor	2/itime			;CREATE A MOVING PHASOR THAT WITH BE USED TO TAP THE DELAY BUFFER
	if itime<0.2 then			;if chunk time is less than 0.2... (very short) 
	 aenv	table3	aptr,ienv4,1		;create amplitude envelope
	elseif itime<0.4 then
	 aenv	table3	aptr,ienv3,1
	elseif itime<2 then
	 aenv	table3	aptr,ienv2,1
	else
	 aenv	table3	aptr,ienv1,1
	endif
	aptr	=	aptr*itime		;SCALE PHASOR ACCORDING TO THE LENGTH OF THE DELAY TIME CHOSEN BY THE USER
 	abuffer	delayr	itime+.01		;CREATE A DELAY BUFFER
	atap	deltap3	aptr			;READ AUDIO FROM A TAP WITHIN THE DELAY BUFFER
		delayw	ain			;WRITE AUDIO INTO DELAY BUFFER
	rireturn				;RETURN FROM REINITIALISATION PASS
	xout	atap*aenv			;SEND AUDIO BACK TO CALLER INSTRUMENT. APPLY AMPLITUDE ENVELOPE TO PREVENT CLICKS.
endop


instr 1
ktime chnget "time"
klevel chnget "level"
kreverse chnget "reverse"
a1,a2	ins
;a1,a2	diskin2	"SynthPad.wav",1,0,1
arev1	Reverse	a1,ktime	
arev2	Reverse	a2,ktime
kreverse	port	kreverse,0.5	;reverse switch will smoothly crossfade between forward and backward modes
a1	ntrpol	a1,arev1,kreverse
a2	ntrpol	a2,arev2,kreverse
a1	=	a1 * klevel
a2	=	a2 * klevel
	outs	a1,a2
endin

</CsInstruments>

<CsScore>
i 1 0 [60*60*24*7]
</CsScore>

</CsoundSynthesizer>