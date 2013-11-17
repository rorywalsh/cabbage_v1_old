<Cabbage>
form caption("Tab.Rec/Play") size(300, 380)

groupbox bounds(10,  10, 70, 90), text("Record")
checkbox channel("Record"), bounds(20,25,50,70), value(0)

groupbox bounds(80,  10, 70, 90), text("Pause"){
checkbox channel("Pause"), bounds(90,25,50,70), value(0)

groupbox bounds(150,  10, 70, 90), text("Play Loop"){
checkbox channel("PlayLoop"), bounds(160,25,50,70), value(0)

groupbox bounds(220,  10, 70, 90), text("Play Once")
button channel("PlayOnce"), pos(230, 38), text(""), size(52, 52)

hslider bounds(10, 120, 280,50), channel("Speed"), range(0, 2, 1), text("Speed")
hslider bounds(10, 160, 280,50), channel("LoopBeg"), range(0, 1, 0), text("Loop Begin")
hslider bounds(10, 200, 280,50), channel("LoopEnd"), range(0, 1, 1), text("Loop End")
hslider bounds(10, 240, 280,50), channel("InGain"), range(0, 1, 1), text("Input Gain")
hslider bounds(10, 280, 280,50), channel("OutGain"), range(0, 1, 1), text("Output Gain")

</Cabbage>

<CsoundSynthesizer>

<CsOptions>
-d -n
</CsOptions>

<CsInstruments>

sr 	= 	44100	
ksmps 	= 	32
nchnls 	= 	2
0dbfs	=	1

;Author: Iain McCurdy (2012)

gistorageL	ftgen	0,0,1048576,-7,0	;AUDIO DATA STORAGE SPACE (ABOUT 23 SECONDS)
gistorageR	ftgen	0,0,1048576,-7,0	;AUDIO DATA STORAGE SPACE (ABOUT 23 SECONDS)
gkRecDur	init	0

instr	1
	gitablelen	=	ftlen(gistorageL)	;DERIVE TABLE LENGTH
	
	gkRecord	chnget	"Record"		;READ IN CABBAGE WIDGET CHANNELS
	gkPause		chnget	"Pause"
	gkPlayLoop	chnget	"PlayLoop"
	gkPlayOnce	chnget	"PlayOnce"
	gkPlayOnceTrig	changed	gkPlayOnce
	gkSpeed		chnget	"Speed"
	gkLoopBeg	chnget	"LoopBeg"
	gkLoopEnd	chnget	"LoopEnd"
	gkInGain	chnget	"InGain"
	gkOutGain	chnget	"OutGain"

	kswitch	changed	gkRecord,gkPlayOnce,gkPlayLoop		;IF EITHER 'PLAY ONCE', 'PLAY LOOP' OR 'RECORD' ARE CHANGED, GENERATE A MOMENTARY '1'
	if kswitch==1 then					;IF SWITCH CHANGE TRIGGER IS '1'
	 reinit RESTART						;BEGIN A REINITIALISATION PASS
	endif							;END OF CONDITIONAL BRANCH
	RESTART:						;A LABEL. BEGIN REINITIALISATION PASS FROM HERE
	
	if i(gkRecord)=1 then		;IF RECORD BUTTON IS ON...
		turnon "Record"		;TURN ON RECORD INSTRUMENT
	elseif i(gkRecord)=0 then	;OR ELSE IF RECORD BUTTON IS OFF...
		turnoff2 "Record",0,0	;TURN OFF RECORD INSTRUMENT
	endif				;END OF THIS CONDITIONAL BRANCH

	if i(gkPlayLoop)=1 then		;IF 'PLAY LOOP' BUTTON IS ON...
		turnon "PlayLoop"	;TURN ON PLAY LOOP INSTRUMENT
	endif				;END OF THIS CONDITIONAL BRANCH

	if i(gkPlayOnceTrig)=1 then	;IF 'PLAY ONCE' BUTTON IS ON...
	;	turnon "PlayOnce"	;TURN ON PLAY ONCE INSTRUMENT
	event_i "i","PlayOnce",0,300	;TURN ON PLAY ONCE INSTRUMENT
	endif				;END OF THIS CONDITIONAL BRANCH

	rireturn
endin

instr	Record
	if	gkPause=1	goto SKIP_RECORD		;IF PAUSE BUTTON IS ACTIVATED TEMPORARILY SKIP RECORDING PROCESS

	    ainL,ainR	ins					;READ AUDIO FROM LIVE INPUT CHANNEL 1
	    aRecNdx	line		0,gitablelen/sr,1	;CREATE A POINTER FOR WRITING TO TABLE - FREQUENCY OF POINTER IS DEPENDENT UPON TABLE LENGTH AND SAMPLE RATE
	    aRecNdx	=		aRecNdx*gitablelen	;RESCALE POINTER ACCORDING TO LENGTH OF FUNCTION TABLE 
	    gkRecDur	downsamp	aRecNdx			;CREATE A K-RATE GLOBAL VARIABLE THAT WILL BE USED BY THE 'PLAYBACK' INSTRUMENT TO DETERMINE THE LENGTH OF RECORDED DATA			
	  		tablew		ainL*gkInGain,  aRecNdx, gistorageL;WRITE AUDIO TO AUDIO STORAGE TABLE
	  		tablew		ainR*gkInGain,  aRecNdx, gistorageR;WRITE AUDIO TO AUDIO STORAGE TABLE
	    if	gkRecDur>=gitablelen	then			;IF MAXIMUM RECORD TIME IS REACHED...
		kRecord=0
	    endif						;END OF CONDITIONAL BRANCH
	SKIP_RECORD:
endin

instr	PlayLoop
	if gkPlayLoop==0 then
	 turnoff
	endif
	if	gkPause=1	goto SKIP_PLAY_LOOP			;IF PAUSE BUTTON IS ACTIVATED SKIP ALL RECORDING AND PLAYBACK...


	kporttime	linseg	0,0.001,0.05			;PORTAMENTO TIME RAMPS UP RAPIDLY TO A HELD VALUE
	kLoopBeg	portk	gkLoopBeg, kporttime		;APPLY PORTAMENTO SMOOTHING TO CHANGES OF LOOP BEGIN SLIDER
	kLoopEnd	portk	gkLoopEnd, kporttime		;APPLY PORTAMENTO SMOOTHING TO CHANGES OF LOOP END SLIDER
	kLoopBeg	=	kLoopBeg * gkRecDur		;RESCALE gkLoopBeg (RANGE 0-1) TO BE WITHIN THE RANGE 0-FILE_LENGTH. NEW OUTPUT VARIABLE kLoopBeg.
	kLoopEnd	=	kLoopEnd * gkRecDur		;RESCALE gkLoopEnd (RANGE 0-1) TO BE WITHIN THE RANGE 0-FILE_LENGTH. NEW OUTPUT VARIABLE kLoopEnd.
	kLoopLen	=	kLoopEnd - kLoopBeg		;DERIVE LOOP LENGTH FROM LOOP START AND END POINTS
	kPlayPhasFrq	divz	gkSpeed,    (kLoopLen/sr),       0.00001	;SAFELY DIVIDE, PROVIDING ALTERNATIVE VALUE INCASE DENOMINATOR IS ZERO 
	aPlayNdx		phasor	kPlayPhasFrq				;DEFINE PHASOR POINTER FOR TABLE INDEX
	kLoopBeg	=	(kLoopBeg < kLoopEnd ? kLoopBeg : kLoopEnd)	;CHECK IF LOOP-BEGINNING AND LOOP-END SLIDERS HAVE BEEN REVERSED
	aLoopLen	interp	abs(kLoopLen)
	aLoopBeg	interp	kLoopBeg
	aPlayNdx	=	(aPlayNdx*aLoopLen) + aLoopBeg	;RESCALE INDEX POINTER ACCORDING TO LOOP LENGTH AND LOOP BEGINING
	aL		tablei	aPlayNdx,    gistorageL		;READ AUDIO FROM AUDIO STORAGE FUNCTION TABLE
	aR		tablei	aPlayNdx,    gistorageR		;READ AUDIO FROM AUDIO STORAGE FUNCTION TABLE
	    		outs	aL*gkOutGain,aR*gkOutGain		;SEND AUDIO TO OUTPUTS

	SKIP_PLAY_LOOP:
endin

instr	PlayOnce
	if	gkPause=1	goto SKIP_PLAY_ONCE			;IF PAUSE BUTTON IS ACTIVATED SKIP ALL RECORDING AND PLAYBACK...

	kPlayOnceNdx	init	0
	;kPlayOnceTrig	changed	kPlayOnce
	if kPlayOnceNdx<=gkRecDur then				;IF PLAYBACK IS NOT YET COMPLETED THEN...
	 kLoopBeg	=	gkLoopBeg * gkRecDur		;RESCALE gkLoopBeg (RANGE 0-1) TO BE WITHIN THE RANGE 0-FILE_LENGTH. NEW OUTPUT VARIABLE kLoopBeg.
	 kLoopEnd	=	gkLoopEnd * gkRecDur		;RESCALE gkLoopEnd (RANGE 0-1) TO BE WITHIN THE RANGE 0-FILE_LENGTH. NEW OUTPUT VARIABLE kLoopEnd.
	 if kLoopEnd>kLoopBeg then				;IF LOOP END SLIDER IS AT A LATER POSITION TO LOOP BEGIN SLIDER...
	  aPlayOnceNdx	line	0,1,1				;CREATE A MOVING POINTER
	  aPlayOnceNdx	=	(aPlayOnceNdx*gkSpeed*sr)+kLoopBeg;RESCALE MOVING POINTER VALUE ACCORDING TO LOOP BEGIN POSITION AND SPEED SLIDER SETTING
	  kPlayOnceNdx	downsamp	aPlayOnceNdx		;CREATE kndx, A K-RATE VERSION OF andx. THIS WILL BE USED TO CHECK IF PLAYBACK OF THE DESIRED CHUNK OF AUDIO HAS COMPLETED.
	  if kPlayOnceNdx>=kLoopEnd then
	   turnoff
	  endif
	 else						;OTHERWISE (I.E. LOOP BEGIN SLIDER IS AT A LATER POSITION THAT LOOP END)
	  aPlayOnceNdx	line	0,1,1			;CREATE A NEGATIVE MOVING POINTER
	  aPlayOnceNdx	=	kLoopBeg-(aPlayOnceNdx*gkSpeed*sr);RESCALE MOVING POINTER VALUE ACCORDING TO LOOP BEGIN POSITION AND SPEED SLIDER SETTING
	  kPlayOnceNdx	downsamp	aPlayOnceNdx	;CREATE kndx, A K-RATE VERSION OF andx
	  if kPlayOnceNdx<=kLoopEnd then
	   turnoff
	  endif
	 endif						;END OF CONDITIONAL BRANCH
	 ;OUT 	OPCODE 		INDEX        | FUNCTION_TABLE
	 aL	tablei		aPlayOnceNdx,    gistorageL	;READ AUDIO FROM AUDIO STORAGE FUNCTION TABLE
	 aR	tablei		aPlayOnceNdx,    gistorageR	;READ AUDIO FROM AUDIO STORAGE FUNCTION TABLE
	  	outs		aL*gkOutGain,aR*gkOutGain		;SEND AUDIO TO OUTPUT
	else
	 turnoff
	endif							;END OF CONDITIONAL BRANCH

	SKIP_PLAY_ONCE:
endin

</CsInstruments>

<CsScore>
i 1 0 [3600*24*7]
</CsScore>

</CsoundSynthesizer>