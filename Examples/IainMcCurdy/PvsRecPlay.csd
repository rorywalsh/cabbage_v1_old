<Cabbage>
form caption("PVS Rec/Play") size(300, 430)
image pos(0, 0), size(300,110), colour("DarkOliveGreen"), shape("rounded"), outline("black"), line(4) 

groupbox bounds(10,  10, 70, 90), text("Record")
checkbox channel("Record"), bounds(20,25,50,70), value(0)

groupbox bounds(80,  10, 70, 90), text("Pause")
checkbox channel("Pause"), bounds(90,25,50,70), value(0)

groupbox bounds(150,  10, 70, 90), text("Play Loop")
checkbox channel("PlayLoop"), bounds(160,25,50,70), value(0)

groupbox bounds(220,  10, 70, 90), text("Play Once")
button channel("PlayOnce"), pos(230, 38), text(""), size(52, 52)

hslider bounds(10, 100, 280,50), channel("Speed"), range(0, 4, 1), text("Speed")
hslider bounds(10, 150, 280,50), channel("Pitch"), range(0.25, 4, 1), text("Pitch")
hslider bounds(10, 200, 280,50), channel("LoopBeg"), range(0, 1, 0), text("Loop Begin")
hslider bounds(10, 250, 280,50), channel("LoopEnd"), range(0, 1, 1), text("Loop End")
hslider bounds(10, 300, 280,50), channel("InGain"), range(0, 1, 1), text("Input Gain")
hslider bounds(10, 350, 280,50), channel("OutGain"), range(0, 1, 1), text("Output Gain")

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
gkRecDur	init	0			;DURATION OF THE MOST RECENTLY RECORDED BUFFER
gibuflen	init	60			;PVS BUFFER LENGTH

instr	1	;READ IN WIDGETS AND START AND STOP THE VARIOUS RECORDING AND PLAYBACK INSTRUMENTS
	gitablelen	=	ftlen(gistorageL)	;DERIVE TABLE LENGTH
	
	gkRecord	chnget	"Record"		;READ IN CABBAGE WIDGET CHANNELS
	gkPause		chnget	"Pause"
	gkPlayLoop	chnget	"PlayLoop"
	gkPlayOnce	chnget	"PlayOnce"
	gkPlayOnceTrig	changed	gkPlayOnce
	gkSpeed		chnget	"Speed"
	gkPitch		chnget	"Pitch"
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

;MACRO THAT DEFINES THE CODED NEEDED TO RECORD A SINGLE CHANNEL PVS BUFFER
#define	REC_BUF(CHAN)
	#
	iFFTsize	=	1024
	ioverlap	=	256
	iwinsize	=	1024
	iwintype	=	1
	;kPhOffset	=	0
	f_anal$CHAN	pvsanal		ain$CHAN, iFFTsize, ioverlap, iwinsize, iwintype	;ANALYSE THE LEFT CHANNEL AUDIO. OUTPUT AN F-SIGNAL.
	ibuf$CHAN,ktime	pvsbuffer   	f_anal$CHAN, gibuflen					;BUFFER FSIG
	gkhandle$CHAN	init 		ibuf$CHAN						;INITIALISE HANDLE TO BUFFER
	#
	;EXPAND BUFFER TWICE, ONCE FOR EACH STEREO CHANNEL
	$REC_BUF(L)
	$REC_BUF(R)
	gkRecDur	timeinsts	;DURATION OF CURRENT RECORDING
	if gkRecDur>=gibuflen then	;IF BUFFER IS FULL (I.E. DO NOT OVERWRITE THE BEGINNING OF THE BUFFER
	 turnoff			;TURN OFF THIS INSTRUMENT
	endif				;ENDO OF THIS CONDITIONAL BRANCH
	SKIP_RECORD:			;JUMP TO HERE WHEN 'PAUSE' BUTTON IS ACTIVE
endin

instr	PlayLoop
	if gkPlayLoop==0 then					;IF 'PLAY LOOPED' BUTTON IS INACTIVE...
	 turnoff						;TURN THIS INSTRUMENT OFF
	endif							;END OF THIS CONDITIONAL BRANCH
	if	gkPause=1	goto SKIP_PLAY_LOOP		;IF PAUSE BUTTON IS ACTIVATED SKIP PLAYBACK CODE
	kporttime	linseg	0,0.001,0.05			;PORTAMENTO TIME RAMPS UP RAPIDLY TO A HELD VALUE
	kLoopBeg	portk	gkLoopBeg, kporttime		;APPLY PORTAMENTO SMOOTHING TO CHANGES OF LOOP BEGIN SLIDER
	kLoopEnd	portk	gkLoopEnd, kporttime		;APPLY PORTAMENTO SMOOTHING TO CHANGES OF LOOP END SLIDER
	kLoopBeg	=	kLoopBeg * gkRecDur		;RESCALE gkLoopBeg (RANGE 0-1) TO BE WITHIN THE RANGE 0-FILE_LENGTH.
	kLoopEnd	=	kLoopEnd * gkRecDur		;RESCALE gkLoopEnd (RANGE 0-1) TO BE WITHIN THE RANGE 0-FILE_LENGTH.
	kLoopLen	=	kLoopEnd - kLoopBeg		;DERIVE LOOP LENGTH FROM LOOP START AND END POINTS
	kPlayPhasFrq	divz	gkSpeed, kLoopLen, 0.00001	;SAFELY DIVIDE, PROVIDING ALTERNATIVE VALUE IN CASE DENOMINATOR IS ZERO 
	kPlayNdx	phasor	kPlayPhasFrq			;DEFINE PHASOR POINTER FOR BUFFER READ INDEX
	kLoopBeg	=	(kLoopBeg < kLoopEnd ? kLoopBeg : kLoopEnd)	;CHECK IF LOOP-BEGINNING AND LOOP-END SLIDERS HAVE BEEN REVERSED
	kPlayNdx	=	(kPlayNdx*kLoopLen) + kLoopBeg	;RESCALE INDEX POINTER ACCORDING TO LOOP LENGTH AND LOOP BEGINING

	f_bufL 		pvsbufread  	kPlayNdx , gkhandleL	;READ BUFFER
	f_scaleL	pvscale 	f_bufL, gkPitch		;RESCALE FREQUENCIES
	aL 		pvsynth  	f_scaleL               	;RESYNTHESIZE THE f-SIGNAL AS AN AUDIO SIGNAL	

	f_bufR 		pvsbufread  	kPlayNdx , gkhandleR	;READ BUFFER
	f_scaleR	pvscale 	f_bufR, gkPitch		;RESCALE FREQUENCIES
	aR 		pvsynth  	f_scaleR               	;RESYNTHESIZE THE f-SIGNAL AS AN AUDIO SIGNAL	

	    		outs	aL*gkOutGain,aR*gkOutGain	;SEND AUDIO TO OUTPUTS
	SKIP_PLAY_LOOP:						;JUMP TO HERE WHEN 'PAUSE' BUTTON IS ACTIVE
endin

instr	PlayOnce
	if	gkPause=1	goto SKIP_PLAY_ONCE		;IF PAUSE BUTTON IS ACTIVATED SKIP PLAYBACK
	
	kPlayOnceNdx	init	0				;INITIALISE PLAYBACK POINTER
	if kPlayOnceNdx<=gkRecDur then				;IF PLAYBACK IS NOT YET COMPLETED THEN CONTINUE PLAYBACK
	 kLoopBeg	=	gkLoopBeg * gkRecDur		;RESCALE gkLoopBeg (RANGE 0-1) TO BE WITHIN THE RANGE 0-FILE_LENGTH.
	 kLoopEnd	=	gkLoopEnd * gkRecDur		;RESCALE gkLoopEnd (RANGE 0-1) TO BE WITHIN THE RANGE 0-FILE_LENGTH.
	 kPlayOnceNdx	line	0,1,1				;CREATE A MOVING POINTER
	 if kLoopEnd>kLoopBeg then				;IF LOOP END SLIDER IS AT A LATER POSITION TO LOOP BEGIN SLIDER...
	  kPlayOnceNdx	=	(kPlayOnceNdx*gkSpeed)+kLoopBeg	;RESCALE MOVING POINTER VALUE ACCORDING TO LOOP BEGIN POSITION AND SPEED SLIDER SETTING
	  if kPlayOnceNdx>=kLoopEnd then			;IF PLAY INDEX IS EQUAL TO OR GREATER THAN THE DURATION OF THE RECORDED BUFFER (STOP PLAYBACK)...
	   turnoff						;TURN THIS INSTRUMENT OFF
	  endif							;END OF CONDITIONAL BRANCH
	 else							;OTHERWISE (I.E. LOOP BEGIN SLIDER IS AT A LATER POSITION THAT LOOP END)
	  kPlayOnceNdx	=	kLoopBeg-(kPlayOnceNdx*gkSpeed)	;RESCALE MOVING POINTER VALUE ACCORDING TO LOOP BEGIN POSITION AND SPEED SLIDER SETTING
	  if kPlayOnceNdx<=kLoopEnd then			;IF PLAY POINTER HAS REACH THE BEGINNING OF THE PRESCRIBED CHUNK BETWEEN LOOP BEGIN AND LOOP END (STOP PLAYBACK)...
	   turnoff						;TURN THIS INSTRUMENT OFF
	  endif							;END OF CONDITIONAL BRANCH
	 endif							;END OF CONDITIONAL BRANCH
	endif
	f_bufL 		pvsbufread  	kPlayOnceNdx , gkhandleL	;READ BUFFER
	f_scaleL	pvscale 	f_bufL, gkPitch			;RESCALE FREQUENCIES
	aL 		pvsynth  	f_scaleL               		;RESYNTHESIZE THE f-SIGNAL AS AN AUDIO SIGNAL	

	f_bufR 		pvsbufread  	kPlayOnceNdx , gkhandleR	;READ BUFFER
	f_scaleR	pvscale 	f_bufR, gkPitch			;RESCALE FREQUENCIES
	aR 		pvsynth  	f_scaleR                	;RESYNTHESIZE THE f-SIGNAL AS AN AUDIO SIGNAL	

	  	outs		aL*gkOutGain,aR*gkOutGain		;SEND AUDIO TO OUTPUT
	;else
	; turnoff
	;endif							;END OF CONDITIONAL BRANCH

	SKIP_PLAY_ONCE:
endin

</CsInstruments>

<CsScore>
i 1 0 [3600*24*7]
</CsScore>

</CsoundSynthesizer>