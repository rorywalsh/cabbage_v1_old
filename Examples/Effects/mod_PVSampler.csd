<Cabbage>
form caption("PVS Sampler") size(280, 270), pluginID("pvrp")

image bounds(4, 120, 269, 105), colour(0, 0,0,0), outlinethickness(1) 
label bounds(4, 127, 269, 16), align("centre"), text("Transport Controls")

label    bounds(13, 204, 50, 14), text("Record") 
label    bounds(80, 202, 50, 14), text("Pause") 
label    bounds(148, 202, 50, 14), text("Loop") 
label    bounds(210, 202, 60, 14), text("Oneshot") 


checkbox bounds(13, 152, 50, 50), channel("Record"), value(0), colour:0(0,0,0,0),  colour:1(0,0,0,0)
checkbox bounds(80, 152, 50, 50), channel("Pause"), value(0), colour:0(0,0,0,0),  colour:1(0,0,0,0)
checkbox bounds(148, 152, 50, 50), channel("PlayLoop"), value(0), colour:0(0,0,0,0),  colour:1(0,0,0,0)
checkbox bounds(216, 152, 50, 50), channel("PlayOnce"), value(0), colour:0(0,0,0,0),  colour:1(0,0,0,0)

image bounds(13, 152, 50, 50), colour(0,0,0,0), identchannel("recordButton")
image bounds(13, 152, 50, 50), file("record.png"), value(0), shape("square")

image bounds(80, 152, 50, 50), colour(0,0,0,0), identchannel("pauseButton")
image bounds(82, 155, 46, 45), file("pause.png"), value(0), shape("square")

image bounds(148, 152, 50, 50), colour(0,0,0,0), identchannel("loopButton")
image bounds(152, 155, 44, 45), file("loop.png"), value(0), shape("square")

image bounds(216, 152, 50, 50), colour(0,0,0,0), identchannel("playButton")
image bounds(216, 155, 46, 45), file("play.png"), value(0), shape("square")


rslider bounds(4, 2, 70, 60), text("Speed"), channel("Speed"), range(-4.00, 4.00, 1) ;, text("Speed")
rslider bounds(70, 2, 70, 60), text("Pitch"), channel("Pitch"), range(0.25, 4.00, 1)  ;, text("Pitch")
hslider2 bounds(4, 55, 268, 68), text("Loop Points"), channel("LoopBeg","LoopEnd"), range(0, 1, 0), min(0),max(1)   ;, text("Loop Begin")
rslider bounds(136, 2, 70, 60), text("Input Gain"), channel("InGain"), range(0, 1, 1) ;, text("Input Gain")
rslider bounds(202, 2, 70, 60), text("Output Gain"), channel("OutGain"), range(0, 1, 1)  ;, text("Output Gain")


</Cabbage>

<CsoundSynthesizer>

<CsOptions>
-d -n
</CsOptions>

<CsInstruments>

sr  =   44100 
ksmps   =   32
nchnls  =   2
0dbfs = 1

;Author: Iain McCurdy (2012)

gistorageL  ftgen 0,0,1048576,-7,0  ;AUDIO DATA STORAGE SPACE (ABOUT 23 SECONDS)
gistorageR  ftgen 0,0,1048576,-7,0  ;AUDIO DATA STORAGE SPACE (ABOUT 23 SECONDS)
gkRecDur  init  0     ;DURATION OF THE MOST RECENTLY RECORDED BUFFER
gibuflen  init  60      ;PVS BUFFER LENGTH

instr 1 ;READ IN WIDGETS AND START AND STOP THE VARIOUS RECORDING AND PLAYBACK INSTRUMENTS
  gitablelen  = ftlen(gistorageL) ;DERIVE TABLE LENGTH
  
  gkRecord  chnget  "Record"    ;READ IN CABBAGE WIDGET CHANNELS
  gkPause   chnget  "Pause"
  gkPlayLoop  chnget  "PlayLoop"
  gkPlayOnce  chnget  "PlayOnce"
  gkPlayOnceTrig  changed gkPlayOnce
  gkSpeed   chnget  "Speed"
  gkPitch   chnget  "Pitch"
  gkLoopBeg chnget  "LoopBeg"
  gkLoopEnd chnget  "LoopEnd"
  gkInGain  chnget  "InGain"
  gkOutGain chnget  "OutGain"

  ;RW hack to show state of record icons
  if changed:k(gkRecord)==1 then
  	if gkRecord==1 then
  	chnset "colour(255, 0, 0)", "recordButton"
  	else
  	chnset "colour(0, 0, 0, 0)", "recordButton"
  	endif
  endif

  if changed:k(gkPause)==1 then
  	if gkPause==1 then
  	chnset "colour(0, 118, 38)", "pauseButton"
  	else
  	chnset "colour(0, 0, 0, 0)", "pauseButton"
  	endif
  endif    	

  if changed:k(gkPlayLoop)==1 then
  	if gkPlayLoop==1 then
  	chnset "colour(0, 118, 38)", "loopButton"
  	else
  	chnset "colour(0, 0, 0, 0)", "loopButton"
  	endif
  endif
  
  if changed:k(gkPlayOnce)==1 then
  	if gkPlayOnce==1 then
  	chnset "colour(0, 118, 38)", "playButton"
  	else
  	chnset "colour(0, 0, 0, 0)", "playButton"
  	endif
  endif  
  
#define TURN_ON(NAME)
  #
  i$NAME  nstrnum "$NAME"
  kOnTrig$NAME  trigger gk$NAME,0.5,0
  if kOnTrig$NAME==1 then   ;IF BUTTON IS TURNED ON...
   event  "i",i$NAME,0,3600
  endif
  #
  
  $TURN_ON(Record)
  $TURN_ON(PlayOnce)
  $TURN_ON(PlayLoop)
endin

instr Record
  if gkRecord==0 then   ;IF BUTTON IS TURNED ON...
   turnoff
  endif
  
  if  gkPause=1 goto SKIP_RECORD    ;IF PAUSE BUTTON IS ACTIVATED TEMPORARILY SKIP RECORDING PROCESS
  ainL,ainR ins         ;READ AUDIO FROM LIVE INPUT CHANNEL 1

;MACRO THAT DEFINES THE CODED NEEDED TO RECORD A SINGLE CHANNEL PVS BUFFER
#define REC_BUF(CHAN)
  #
  iFFTsize  = 1024
  ioverlap  = 256
  iwinsize  = 1024
  iwintype  = 1
  ;kPhOffset  = 0
  f_anal$CHAN pvsanal   ain$CHAN, iFFTsize, ioverlap, iwinsize, iwintype  ;ANALYSE THE LEFT CHANNEL AUDIO. OUTPUT AN F-SIGNAL.
  ibuf$CHAN,ktime pvsbuffer     f_anal$CHAN, gibuflen         ;BUFFER FSIG
  gkhandle$CHAN init    ibuf$CHAN           ;INITIALISE HANDLE TO BUFFER
  #
  ;EXPAND BUFFER TWICE, ONCE FOR EACH STEREO CHANNEL
  $REC_BUF(L)
  $REC_BUF(R)
  gkRecDur  timeinsts ;DURATION OF CURRENT RECORDING
  if gkRecDur>=gibuflen then  ;IF BUFFER IS FULL (I.E. DO NOT OVERWRITE THE BEGINNING OF THE BUFFER
   turnoff      ;TURN OFF THIS INSTRUMENT
  endif       ;ENDO OF THIS CONDITIONAL BRANCH
  SKIP_RECORD:      ;JUMP TO HERE WHEN 'PAUSE' BUTTON IS ACTIVE
endin

instr PlayLoop
  if gkPlayLoop==0 then   ;IF BUTTON IS TURNED ON...
   turnoff
  endif

  if gkPlayLoop==0 then         ;IF 'PLAY LOOPED' BUTTON IS INACTIVE...
   turnoff            ;TURN THIS INSTRUMENT OFF
  endif             ;END OF THIS CONDITIONAL BRANCH
  if  gkPause=1 goto SKIP_PLAY_LOOP   ;IF PAUSE BUTTON IS ACTIVATED SKIP PLAYBACK CODE
  kporttime linseg  0,0.001,0.05      ;PORTAMENTO TIME RAMPS UP RAPIDLY TO A HELD VALUE
  kLoopBeg  portk gkLoopBeg, kporttime    ;APPLY PORTAMENTO SMOOTHING TO CHANGES OF LOOP BEGIN SLIDER
  kLoopEnd  portk gkLoopEnd, kporttime    ;APPLY PORTAMENTO SMOOTHING TO CHANGES OF LOOP END SLIDER
  kLoopBeg  = kLoopBeg * gkRecDur   ;RESCALE gkLoopBeg (RANGE 0-1) TO BE WITHIN THE RANGE 0-FILE_LENGTH.
  kLoopEnd  = kLoopEnd * gkRecDur   ;RESCALE gkLoopEnd (RANGE 0-1) TO BE WITHIN THE RANGE 0-FILE_LENGTH.
  kLoopLen  = abs(kLoopEnd - kLoopBeg)  ;DERIVE LOOP LENGTH FROM LOOP START AND END POINTS
  kPlayPhasFrq  divz  gkSpeed, kLoopLen, 0.00001  ;SAFELY DIVIDE, PROVIDING ALTERNATIVE VALUE IN CASE DENOMINATOR IS ZERO 
  kPlayNdx  phasor  kPlayPhasFrq      ;DEFINE PHASOR POINTER FOR BUFFER READ INDEX
  ;kLoopBeg = (kLoopBeg < kLoopEnd ? kLoopBeg : kLoopEnd) ;CHECK IF LOOP-BEGINNING AND LOOP-END SLIDERS HAVE BEEN REVERSED
  kPlayNdx  = (kPlayNdx*kLoopLen) + kLoopBeg  ;RESCALE INDEX POINTER ACCORDING TO LOOP LENGTH AND LOOP BEGINING

  f_bufL    pvsbufread    kPlayNdx , gkhandleL  ;READ BUFFER
  f_scaleL  pvscale   f_bufL, gkPitch   ;RESCALE FREQUENCIES
  aL    pvsynth   f_scaleL                ;RESYNTHESIZE THE f-SIGNAL AS AN AUDIO SIGNAL 

  f_bufR    pvsbufread    kPlayNdx , gkhandleR  ;READ BUFFER
  f_scaleR  pvscale   f_bufR, gkPitch   ;RESCALE FREQUENCIES
  aR    pvsynth   f_scaleR                ;RESYNTHESIZE THE f-SIGNAL AS AN AUDIO SIGNAL 

          outs  aL*gkOutGain,aR*gkOutGain ;SEND AUDIO TO OUTPUTS
  SKIP_PLAY_LOOP:           ;JUMP TO HERE WHEN 'PAUSE' BUTTON IS ACTIVE
endin

instr PlayOnce
  if gkPlayOnce==0 then   ;IF BUTTON IS TURNED ON...
   turnoff
  endif

  if  gkPause=1 goto SKIP_PLAY_ONCE   ;IF PAUSE BUTTON IS ACTIVATED SKIP PLAYBACK
  
  kPlayOnceNdx  init  0       ;INITIALISE PLAYBACK POINTER
  if kPlayOnceNdx<=gkRecDur then        ;IF PLAYBACK IS NOT YET COMPLETED THEN CONTINUE PLAYBACK
   kLoopBeg = gkLoopBeg * gkRecDur    ;RESCALE gkLoopBeg (RANGE 0-1) TO BE WITHIN THE RANGE 0-FILE_LENGTH.
   kLoopEnd = gkLoopEnd * gkRecDur    ;RESCALE gkLoopEnd (RANGE 0-1) TO BE WITHIN THE RANGE 0-FILE_LENGTH.
   kPlayOnceNdx line  0,1,1       ;CREATE A MOVING POINTER
   if kLoopEnd>kLoopBeg then        ;IF LOOP END SLIDER IS AT A LATER POSITION TO LOOP BEGIN SLIDER...
    kPlayOnceNdx  = (kPlayOnceNdx*gkSpeed)+kLoopBeg ;RESCALE MOVING POINTER VALUE ACCORDING TO LOOP BEGIN POSITION AND SPEED SLIDER SETTING
    if kPlayOnceNdx>=kLoopEnd then      ;IF PLAY INDEX IS EQUAL TO OR GREATER THAN THE DURATION OF THE RECORDED BUFFER (STOP PLAYBACK)...
     turnoff            ;TURN THIS INSTRUMENT OFF
    endif             ;END OF CONDITIONAL BRANCH
   else             ;OTHERWISE (I.E. LOOP BEGIN SLIDER IS AT A LATER POSITION THAT LOOP END)
    kPlayOnceNdx  = kLoopBeg-(kPlayOnceNdx*gkSpeed) ;RESCALE MOVING POINTER VALUE ACCORDING TO LOOP BEGIN POSITION AND SPEED SLIDER SETTING
    if kPlayOnceNdx<=kLoopEnd then      ;IF PLAY POINTER HAS REACHED THE BEGINNING OF THE PRESCRIBED CHUNK BETWEEN LOOP BEGIN AND LOOP END (STOP PLAYBACK)...
     turnoff            ;TURN THIS INSTRUMENT OFF
    endif             ;END OF CONDITIONAL BRANCH
   endif              ;END OF CONDITIONAL BRANCH
  endif
  f_bufL    pvsbufread    kPlayOnceNdx , gkhandleL  ;READ BUFFER
  f_scaleL  pvscale   f_bufL, gkPitch     ;RESCALE FREQUENCIES
  aL    pvsynth   f_scaleL                  ;RESYNTHESIZE THE f-SIGNAL AS AN AUDIO SIGNAL 

  f_bufR    pvsbufread    kPlayOnceNdx , gkhandleR  ;READ BUFFER
  f_scaleR  pvscale   f_bufR, gkPitch     ;RESCALE FREQUENCIES
  aR    pvsynth   f_scaleR                  ;RESYNTHESIZE THE f-SIGNAL AS AN AUDIO SIGNAL 

      outs    aL*gkOutGain,aR*gkOutGain   ;SEND AUDIO TO OUTPUT
  ;else
  ; turnoff
  ;endif              ;END OF CONDITIONAL BRANCH

  SKIP_PLAY_ONCE:
  krelease  release
  if krelease==1 then
   chnset 1-krelease,"PlayOnce"
  endif
endin

</CsInstruments>

<CsScore>
i 1 0 [3600*24*7]
</CsScore>

</CsoundSynthesizer>