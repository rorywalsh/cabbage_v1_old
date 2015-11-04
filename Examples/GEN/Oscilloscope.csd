; Oscilloscope.csd
; Using the gentable widget to create a simple oscilloscope.
; Written by Iain McCurdy 2013
; 
; Source input options:
; 1	-	sine wave
; 2	-	vco (vco2 opcode)
; 3	-	buzz (gbuzz opcode)
; 4	-	FM synthesis. A basic modulator-carrier pair.
; 5	-	hsboscil (stack of octave spaced sines)
; 6	-	phase distortion synthesis (X). A method employed by Casio in the 1980s. (X axis distortion) 
; 7	-	phase distortion synthesis (Y). A method employed by Casio in the 1980s. (Y axis distortion) 
; 8	-	fof synthesis
; 9	-	bit depth reduction
; 10	-	sample rate hold
; 11	-	pdclip
; 12	-	powershape
; 13	-	clip
; 14	-	hilbert
; 
; Use level to control the audio level. (Display level is unaffected by this control)
; The display level can be controlled by the small vertical slider to the right of the display.
; The phase of the display can be shifted using the small horizontal slider just beneath the waveform display.

<Cabbage>
form caption("Oscilloscope"), size(930, 450), pluginID("osci"), colour( 20, 20, 20,150)

gentable bounds(  5,  5, 910, 240), identchannel("table1"), tablenumber(1), tablecolour("lightblue"), amprange(-1,1,1), zoom(-1)
hslider  bounds(  1,245, 920,  10), channel("phase"), range(0, 1, 0,1,0.001)
vslider  bounds(920,  0,  10, 252), channel("gain"), range(0, 1, 1,1,0.001)

label    bounds( 10, 258, 80, 11), text("Source")
combobox bounds(  5, 270, 90, 20), channel("source"), value(1), text("Sine","VCO","Buzz","FM","Hsboscil","PD Half X","PD Half Y","FOF","Bit Depth","S.R. Rate","pdclip","powershape","clip","hilbert")
rslider  bounds( 20, 300, 60, 60), channel("level"), text("Level"), range(0, 1, 0.1,0.375,0.0001)

groupbox bounds( 100, 260, 270, 100), text("VCO"), plant("VCO"), visible(0), identchannel("2") {
label    bounds( 10,  38,110, 11), text("Waveform")
combobox bounds( 10,  50,110, 20), channel("wave"), value(1), text("Sawtooth","Square/PWM","Saw./Tri./Ramp")
rslider  bounds( 120,20, 80, 80), channel("pw"), text("Pulse Width"), textBox(1), range(0.01, 0.99, 0.5)
rslider  bounds( 190,20, 80, 80), channel("VCOharms"), text("N.Harms"), textBox(1), range(0.01, 0.5, 0.5,0.5,0.001)
}
groupbox bounds( 100, 260, 200, 100), text("Buzz"), plant("buzz"), visible(0), identchannel("3") {
rslider  bounds(   0,  20,  80, 80), channel("lp"), text("Lowest"), textBox(1), range(1, 20, 1, 1,1)
rslider  bounds(  60,  20,  80, 80), channel("np"), text("Number"), textBox(1), range(1, 80, 10, 1,1)
rslider  bounds( 120,  20,  80, 80), channel("pow"), text("Power"), textBox(1), range(0, 2.00,0.5)
}
groupbox bounds( 100, 260, 150, 100), text("FM"), plant("FM"), visible(0), identchannel("4") {
rslider  bounds(   0,20, 80, 80), channel("FMratio"), text("Ratio"), textBox(1), range(0, 8.00, 1,0.5,0.01)
rslider  bounds(  70,20, 80, 80), channel("FMindex"), text("Index"), textBox(1), range(0,100.00, 1,0.5)
}
groupbox bounds( 100, 260, 150, 100), text("Hsboscil"), plant("hsboscil"), visible(0), identchannel("5") {
rslider  bounds(   0,20, 80, 80), channel("brite"), text("Brite"), textBox(1), range(-4, 4.00, 0)
rslider  bounds(  70,20, 80, 80), channel("octcnt"), text("Width"), textBox(1), range(2, 10.00, 3,1,1)
}
groupbox bounds( 100,260,235, 100), text("PD Half X"), plant("pdhalfx"), visible(0), identchannel("6") {
combobox bounds(   5, 30, 70, 20), channel("PDHalfIP"), value(1), text("Sine", "Saw", "Triangle","Square","User")
rslider  bounds(  70, 20, 80, 80), channel("PD_amount"), text("P.Distort"), textBox(1), range(-1, 1.00, 0,1,0.001)
combobox bounds(150, 30, 80, 20), channel("polarX"), value(1), text("Unipolar", "Bipolar")
}
groupbox bounds( 100,260,235, 100), text("PD Half Y"), plant("pdhalfy"), visible(0), identchannel("7") {
combobox bounds(   5, 30, 70, 20), channel("PDHalfyIP"), value(1), text("Sine", "Saw", "Triangle","Square","User")
rslider  bounds(  70,20, 80, 80), channel("PD_amountY"), text("P.Distort"), textBox(1), range(-1, 1.00, 0,1,0.001)
combobox bounds(150, 30, 80, 20), channel("polarY"), value(1), text("Unipolar", "Bipolar")
}
groupbox bounds( 100,260,450, 100), text("FOF"), plant("fof"), visible(0), identchannel("8") {
rslider  bounds(   0,20, 80, 80), channel("formant"), text("Formant"), textBox(1), range(50,10000,800,0.5,1)
rslider  bounds(  70,20, 80, 80), channel("FRatio"), text("Frq.Ratio"), textBox(1), range(1,32, 8,0.5,0.01)
checkbox  bounds( 150,45,100, 20), channel("FormOn"),  text("Formant"), radiogroup(1) , value(1)
checkbox  bounds( 150,65,100, 20), channel("RatioOn"), text("Frq.Ratio"),   radiogroup(1)
rslider  bounds( 230,20, 80, 80), channel("bandwidth"), text("Bandwidth"), textBox(1), range(1,1000, 50,0.5,1)
rslider  bounds( 300,20, 80, 80), channel("octdiv"), text("Oct.Div."), textBox(1), range(0,8, 0,1,0.01)
rslider  bounds( 370,20, 80, 80), channel("FOFGain"), text("Gain"), textBox(1), range(0.001,1,1,0.5,0.001)
}
groupbox bounds( 100,260, 80, 100), text("Bit Depth"), plant("bitdepth"), visible(0), identchannel("9") {
rslider  bounds(   0,20, 80, 80), channel("bits"), text("Bits"), textBox(1), range(0, 16, 16,0.5,0.01)
}
groupbox bounds( 100,260, 80, 100), text("Samp. Rate"), plant("samplerate"), visible(0), identchannel("10") {
rslider  bounds(   0,20, 80, 80), channel("sr_hold"), text("SR Hold"), textBox(1), range(1, 10000, 1,0.5,1)
}
groupbox bounds( 100,260,320, 100), text("pdclip"), plant("pdclip"), visible(0), identchannel("11") {
combobox     bounds(  5, 30, 70, 20), channel("PdclipIP"), value(1), text("Sine", "Phasor")
rslider      bounds( 70, 20, 80, 80), text("Width"),  channel("PdclipWidth"),  range(0, 1.00, 0), textBox(1)
rslider      bounds(140, 20, 80, 80), text("Centre"), channel("PdclipCenter"), range(-1.00, 1.00, 0), textBox(1) 
combobox     bounds(220, 30, 80, 20), channel("PdclipBipolar"), value(2), text("Unipolar", "Bipolar")
}
groupbox bounds( 100,260,180, 100), text("Powershape"), plant("powershape"), visible(0), identchannel("12") {
rslider      bounds(  0, 20, 80, 80), text("Amount"),  channel("PShapeAmt"),  range(0.1, 999, 1.00,0.25,0.01), textBox(1)
numberbox    bounds( 70, 22,100, 40), text("Amount [type]"),  channel("PShapeAmt"),  range(0.1, 999, 1,0.5,0.01)
}
groupbox bounds( 100,260,410, 100), text("Clip"), plant("clip"), visible(0), identchannel("13") {
rslider      bounds(  0, 20, 80, 80), text("Pre-Gain"),  channel("ClipGain"),  range(0.5,100.00, 1,0.5,0.01), textBox(1)
rslider      bounds( 70, 20, 80, 80), text("Limit"),  channel("ClipLimit"),  range(0.001, 2, 1, 0.5,0.001), textBox(1)
rslider      bounds(140, 20, 80, 80), text("Arg."),  channel("ClipArg"),  range(0, 1.00, 0.5), textBox(1)
combobox     bounds(220, 30, 80,20), channel("ClipMethod"), value(1), text("B.D.J.", "Sine", "Tanh")
checkbox  bounds( 310,45,100, 20), channel("ClipNorm"),  text("Normalise"), value(0)
}
groupbox bounds( 100,260,130, 100), text("Hilbert"), plant("hilbert"), visible(0), identchannel("14") {
checkbox  bounds( 10,35,120, 15), channel("hilbert_source"),  text("Source"),        value(1), radiogroup(2)
checkbox  bounds( 10,55,120, 15), channel("hilbert_sine"),    text("Sine Output"),   value(0), radiogroup(2)
checkbox  bounds( 10,75,120, 15), channel("hilbert_cosine"),  text("Cosine Output"), value(0), radiogroup(2)
}


keyboard bounds(  5,365,920, 80)
</Cabbage>
                    
<CsoundSynthesizer>

<CsOptions>   
-dm0 -n -+rtmidi=null -M0
</CsOptions>

<CsInstruments>

sr 		= 	44100	; SAMPLE RATE
ksmps 		= 	32	; NUMBER OF AUDIO SAMPLES IN EACH CONTROL CYCLE
nchnls 		= 	2	; NUMBER OF CHANNELS (1=MONO)
0dbfs		=	1	; MAXIMUM AMPLITUDE

massign	0,2
			
giview	ftgen	1,0,1024,10,0
giblank	ftgen	2,0,1024,2,0
gisine	ftgen	3,0,4096,10,1
gicos	ftgen	4,0,131072,9,1,1,90
giuser	ftgen	0,0,4097,10,1,1,0,1,0,0,1,0,0,0,0,0.1

giwfn	ftgen	5, 0, 131072, 10, 1			;A SINE WAVE
gioctfn	ftgen	6, 0, 1024, -19, 1, 0.5, 270, 0.5	;A HANNING-TYPE WINDOW

giexp	ftgen	0, 0, 1024, 19, 0.5, 0.5, 270, 0.5	;EXPONENTIAL CURVE


instr	1
 kporttime	linseg	0,0.001,0.05
 
 gksrc	chnget	"source"

 gkwave	chnget	"wave"
 gkpw	chnget	"pw"

 gklp	chnget	"lp"
 gknp	chnget	"np"
 gkpow	chnget	"pow"
 gklev	chnget	"level"

 gkFMratio	chnget	"FMratio"
 gkFMindex	chnget	"FMindex"
 gkbrite	chnget	"brite"
 gkoctcnt	chnget	"octcnt"

 gkPDHalfIP	chnget	"PDHalfIP"

 gkPDHalfyIP	chnget	"PDHalfyIP"

 gkformant	chnget	"formant"
 gkformant	portk	gkformant,kporttime
 gkbandwidth	chnget	"bandwidth"
 gkbandwidth	portk	gkbandwidth,kporttime
 gkoctdiv	chnget	"octdiv"
 gkoctdiv	portk	gkoctdiv,kporttime
 gkFRatio	chnget	"FRatio"
 gkFRatio	portk	gkFRatio,kporttime
 gkFormOn	chnget	"FormOn"
 gkFOFGain	chnget	"FOFGain"
 gkFOFGain	portk	gkFOFGain,kporttime

 gkbits	chnget	"bits"
 gkbits	portk	gkbits,kporttime

 gksr_hold	chnget	"sr_hold"
 gksr_hold	portk	gksr_hold,kporttime

 gkPdclipIP		chnget	"PdclipIP"
 gkPdclipWidth		chnget	"PdclipWidth"
 gkPdclipWidth		portk	gkPdclipWidth,kporttime
 gkPdclipCenter		chnget	"PdclipCenter"
 gkPdclipCenter		portk	gkPdclipCenter,kporttime
 gkPdclipBipolar	chnget	"PdclipBipolar"

 gkPShapeAmt	chnget	"PShapeAmt"
 gkPShapeAmt	portk	gkPShapeAmt,kporttime

 gkClipLimit	chnget	"ClipLimit"
 gkClipMethod	chnget	"ClipMethod"
 gkClipArg	chnget	"ClipArg"
 gkClipGain	chnget	"ClipGain"
 gkClipGain	portk	gkClipGain,kporttime
 gkClipNorm	chnget	"ClipNorm"

 if changed(gksrc)==1 then
  Smsg	sprintfk	"visible(%d)", gksrc == 2 ? 1 : 0
  	chnset		Smsg,"2"
 Smsg	sprintfk	"visible(%d)", gksrc == 3 ? 1 : 0
  	chnset		Smsg,"3"
 Smsg	sprintfk	"visible(%d)", gksrc == 4 ? 1 : 0
  	chnset		Smsg,"4"
 Smsg	sprintfk	"visible(%d)", gksrc == 5 ? 1 : 0
  	chnset		Smsg,"5"
 Smsg	sprintfk	"visible(%d)", gksrc == 6 ? 1 : 0
  	chnset		Smsg,"6"
 Smsg	sprintfk	"visible(%d)", gksrc == 7 ? 1 : 0
  	chnset		Smsg,"7"
 Smsg	sprintfk	"visible(%d)", gksrc == 8 ? 1 : 0
  	chnset		Smsg,"8"
 Smsg	sprintfk	"visible(%d)", gksrc == 9 ? 1 : 0
  	chnset		Smsg,"9"
 Smsg	sprintfk	"visible(%d)", gksrc == 10 ? 1 : 0
  	chnset		Smsg,"10"
 Smsg	sprintfk	"visible(%d)", gksrc == 11 ? 1 : 0
  	chnset		Smsg,"11"
 Smsg	sprintfk	"visible(%d)", gksrc == 12 ? 1 : 0
  	chnset		Smsg,"12"
 Smsg	sprintfk	"visible(%d)", gksrc == 13 ? 1 : 0
  	chnset		Smsg,"13"
 Smsg	sprintfk	"visible(%d)", gksrc == 14 ? 1 : 0
  	chnset		Smsg,"14"
 endif
  
endin





instr	2
 icps	cpsmidi
 inum	notnum
  
 kporttime	linseg	0,0.001,0.05
 
 ; SINE
 if i(gksrc)==1 then
  asig	poscil	1,icps

 ; VCO
 elseif i(gksrc)==2 then
  gkpw	portk	gkpw,kporttime
  iwave	=	(i(gkwave)-1)*2
  kVCOharms	chnget	"VCOharms"
  if changed(kVCOharms)==1 then
   reinit RESTART_VCO
  endif
  RESTART_VCO:
  asig	vco2	0.8,icps,iwave,gkpw,0,i(kVCOharms)

 ; GBUZZ
 elseif i(gksrc)==3 then
  gkpow	portk	gkpow,kporttime
  asig		gbuzz		1, icps, gknp, gklp, gkpow, gicos, 0.5

 ; FM
 elseif i(gksrc)==4 then
  gkFMratio	portk	gkFMratio,kporttime
  gkFMindex	portk	gkFMindex,kporttime
  asig foscili 1, icps, 1, gkFMratio, gkFMindex, gisine

 ; HSBOSCIL
 elseif i(gksrc)==5 then
  ktone	=	0
  if changed(gkoctcnt)==1 then
   reinit UPDATE
  endif
  UPDATE:
  kbrite	portk		gkbrite,kporttime
  asig 		hsboscil	1, 0, kbrite, icps, giwfn, gioctfn, i(gkoctcnt)

 ; PDHALF(X)
 elseif i(gksrc)==6 then
  kPD_amount	chnget	"PD_amount"
  kPD_amount	portk	kPD_amount,kporttime
  ipolarX	chnget	"polarX"
  aphsr	phasor	icps
  aphsr	pdhalf	aphsr,kPD_amount,ipolarX-1
  imaxh	=  		sr / (2 * 440.0 * exp(log(2.0) * (inum - 69) / 12))	; author: Istvan Varga
  kswitch	changed	gkPDHalfIP
  if kswitch==1 then
   reinit PDHALF_INIT
  endif
  PDHALF_INIT:
  if i(gkPDHalfIP)==1 then 						; sine
   asig   	tablei	aphsr,gisine,1
  elseif i(gkPDHalfIP)==2 then 						; saw
   isaw		ftgentmp	0,0,1024,7,1,1024,-1
   isawBL	ftgentmp	0, 0, 1024, -30, isaw, 1, imaxh	
   asig   	tablei	aphsr,isawBL,1
   asig		*=	0.8
  elseif i(gkPDHalfIP)==3 then						; tri
   itri		ftgentmp	0,0,1024,7,0,256,1,512,-1,256,0
   itriBL	ftgentmp	0, 0, 1024, -30, itri, 1, imaxh	
   asig   	tablei	aphsr,itriBL,1
  elseif i(gkPDHalfIP)==4 then						; square
   isq		ftgentmp	0,0,1024,7,1,512,1,0,-1,512,-1
   isqBL	ftgentmp	0, 0, 1024, -30, isq, 1, imaxh	
   asig   	tablei	aphsr,isqBL,1
   asig		*=	0.8
  elseif i(gkPDHalfIP)==5 then 						; user definable waveform (see instr 0)
   asig   	tablei	aphsr,giuser,1
  endif

 ; PDHALFY
 elseif i(gksrc)==7 then
  kPD_amount	chnget	"PD_amountY"
  kPD_amount	portk	kPD_amount,kporttime
  ipolarY	chnget	"polarY"
  aphsr	phasor	icps
  aphsr	pdhalfy	aphsr,kPD_amount,ipolarY-1
  imaxh	=  		sr / (2 * 440.0 * exp(log(2.0) * (inum - 69) / 12))	; author: Istvan Varga
  kswitch	changed	gkPDHalfyIP
  if kswitch==1 then
   reinit PDHALFY_INIT
  endif
  PDHALFY_INIT:
  if i(gkPDHalfyIP)==1 then 						; sine
   asig   	tablei	aphsr,gisine,1
  elseif i(gkPDHalfyIP)==2 then 						; saw
   isaw		ftgentmp	0,0,1024,7,1,1024,-1
   isawBL	ftgentmp	0, 0, 1024, -30, isaw, 1, imaxh	
   asig   	tablei	aphsr,isawBL,1
   asig		*=	0.8
  elseif i(gkPDHalfyIP)==3 then						; tri
   itri		ftgentmp	0,0,1024,7,0,256,1,512,-1,256,0
   itriBL	ftgentmp	0, 0, 1024, -30, itri, 1, imaxh	
   asig   	tablei	aphsr,itriBL,1
  elseif i(gkPDHalfyIP)==4 then						; square
   isq		ftgentmp	0,0,1024,7,1,512,1,0,-1,512,-1
   isqBL	ftgentmp	0, 0, 1024, -30, isq, 1, imaxh	
   asig   	tablei	aphsr,isqBL,1
   asig		*=	0.8
  elseif i(gkPDHalfyIP)==5 then 					; user definable waveform (see instr 0)
   asig   	tablei	aphsr,giuser,1
  endif
 
 ; FOF
 elseif i(gksrc)==8 then
  kformant	=	(gkFormOn==1?gkformant:icps*gkFRatio)
;  asig 	fof 	gkamp, kfund, kform,   gkoct,   gkband,    gkris, gkdur, gkdec, iolaps, ifna, ifnb, itotdur ;[, iphs] [, ifmode] [, iskip
  asig 		fof 	gkFOFGain,     icps, kformant,gkoctdiv,gkbandwidth,  0.003, 0.1,   0.007,  3000, gisine, giexp, 3600 ;[, iphs] [, ifmode] [, iskip
  
 ; BIT DEPTH
 elseif i(gksrc)==9 then
  asig	poscil	1,icps
  kvalues		pow		2, gkbits		;RAISES 2 TO THE POWER OF kbitdepth. THE OUTPUT VALUE REPRESENTS THE NUMBER OF POSSIBLE VALUES AT THAT PARTICULAR BIT DEPTH
  asig		=	(int((asig/0dbfs)*kvalues))/kvalues	;BIT DEPTH REDUCE AUDIO SIGNAL

 ; SAMPLE RATE 
 elseif i(gksrc)==10 then
  icps	=	int(icps)
  asig	poscil	1,icps
  asig		fold		asig,gksr_hold/icps	;gksr_hold

 ; PDCLIP
 elseif i(gksrc)==11 then
  kSwitch		changed		gkPdclipBipolar,gkPdclipIP	;GENERATE A MOMENTARY '1' PULSE IN OUTPUT 'kSwitch' IF ANY OF THE SCANNED INPUT VARIABLES CHANGE. (OUTPUT 'kSwitch' IS NORMALLY ZERO)
  if	kSwitch=1	then	;IF A VARIABLE CHANGE INDICATOR IS RECEIVED...
  	reinit	PDCLIP_INIT	;...BEGIN A REINITIALISATION PASS FROM LABEL 'START' 
  endif				;END OF CONDITIONAL BRANCHING
  PDCLIP_INIT:				;LABEL
  if i(gkPdclipIP)==1 then
   asig		poscil	1,icps
  else
   asig		phasor	icps
  endif
  ;CLIP THE AUDIO SIGNAL USING pdclip
  asig		pdclip		asig, gkPdclipWidth, gkPdclipCenter, i(gkPdclipBipolar)-1	; [, ifullscale]]

 ; POWERSHAPE
 elseif i(gksrc)==12 then
  asig		poscil	1,icps
  ifullscale	=	0dbfs	;DEFINE FULLSCALE AMPLITUDE VALUE
  asig 		powershape 	asig, gkPShapeAmt, ifullscale	;CREATE POWERSHAPED SIGNAL

 ; CLIP
 elseif i(gksrc)==13 then
  kSwitch		changed		gkClipLimit, gkClipMethod, gkClipArg, gkClipNorm	;GENERATE A MOMENTARY '1' PULSE IN OUTPUT 'kSwitch' IF ANY OF THE SCANNED INPUT VARIABLES CHANGE. (OUTPUT 'kSwitch' IS NORMALLY ZERO)
  if	kSwitch=1	then	;IF A VARIABLE CHANGE INDICATOR IS RECEIVED...
   reinit	CLIP_INIT		;...BEGIN A REINITIALISATION PASS FROM LABEL 'START' 
  endif				;END OF CONDITIONAL BRANCHING
  CLIP_INIT:				;LABEL
  asig		poscil	1,icps
  asig		clip 		asig * gkClipGain, i(gkClipMethod), i(gkClipLimit), i(gkClipArg)
  if i(gkClipNorm)==1 then		; IF NORMALISE IS ON...
   asig	=	asig * (1/i(gkClipLimit))
  endif

 ; HILBERT
 elseif i(gksrc)==14 then
  khilbert_sine		chnget	"hilbert_sine"
  khilbert_cosine	chnget	"hilbert_cosine"
  asig		poscil	1,icps
  aCosOut, aSineOut hilbert asig
  khilbert_sine	chnget	"hilbert_sine"
  if khilbert_sine==1 then
   asig	=	aSineOut
  elseif khilbert_cosine==1 then
   asig	=	aCosOut
  endif				; (otherwise asig = asig)
 endif


 
 aptr	phasor	icps
 
 kphase	chnget	"phase"
 kgain	chnget	"gain"
 	tablew	asig*kgain,aptr+kphase,giview,1,0,1

 if metro(icps/5)==1 then
  	chnset	"tablenumber(1)", "table1"	; update table display	
 endif
 
 	outs	asig*gklev ,asig*gklev

 krelease	release
 if trigger(krelease,0.5,0)==1 then
  event	"i",3,0,0.1
 endif
endin

instr	3	; erase table
	tablecopy	giview,giblank
  	chnset	"tablenumber(1)", "table1"	; update table display	
	turnoff
endin

</CsInstruments>

<CsScore>
i 1 0 [3600*24*7]
</CsScore>

</CsoundSynthesizer>
