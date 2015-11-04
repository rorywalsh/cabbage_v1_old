Oscilliscope.csd
Using the gentable widget to create a simple oscilloscope.
Written by Iain McCurdy 2013

Source input options:
1	-	sine wave
2	-	vco (vco2 opcode)
3	-	buzz (gbuzz opcode)
4	-	FM synthesis. A basic modulator-carrier pair.
5	-	hsboscil (stack of octave spaced sines)
6	-	squarish (a hybrid square/sine wave created using GEN16) 
7	-	phase distortion synthesis (X). A method employed by Casio in the 1980s. (X axis distortion) 
8	-	phase distortion synthesis (Y). A method employed by Casio in the 1980s. (Y axis distortion) 
9	-	fof synthesis
10	-	bit depth reduction
11	-	sample rate hold

Use level to control the audio level. (Display level is unaffected by this control)
The display level can be controlled by the small vertical slider to the right of the display.
The phase of the display can be shifted using the small horizontal slider just beneath the waveform display.

<Cabbage>
form caption("Oscilloscope"), size(930, 450), pluginID("gn10"), colour( 20, 20, 20,150)

gentable bounds(  5,  5, 910, 240), identchannel("table1"), tablenumber(1), tablecolour("lightblue"), amprange(-1,1,1), zoom(-1)
hslider  bounds(  1,245, 920,  10), channel("phase"), range(0, 1, 0,1,0.001)
vslider  bounds(920,  0,  10, 252), channel("gain"), range(0, 1, 1,1,0.001)

label    bounds( 10, 258, 80, 11), text("Source")
combobox bounds( 10, 270, 80, 20), channel("source"), value(1), text("Sine","VCO","Buzz","FM","Hsboscil","Squarish","PD Half X","PD Half Y","FOF","Bit Depth","Sample Rate")
rslider  bounds( 20, 300, 60, 60), channel("level"), text("Level"), range(0, 1, 0.1,0.375,0.0001)

groupbox bounds( 100, 260, 270, 100), text("VCO"), plant("VCO"), visible(0), identchannel("2") {
label    bounds( 10,  38,110, 11), text("Waveform")
combobox bounds( 10,  50,110, 20), channel("wave"), value(1), text("Sawtooth","Square/PWM","Saw./Tri./Ramp")
rslider  bounds( 120,20, 80, 80), channel("pw"), text("Pulse Width"), textBox(1), range(0.01, 0.99, 0.5)
rslider  bounds( 190,20, 80, 80), channel("VCOharms"), text("N.Harms"), textBox(1), range(0, 0.5, 0.5,0.5,0.001)
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
groupbox bounds( 100,260, 80, 100), text("Squarish"), plant("squarish"), visible(0), identchannel("6") {
rslider  bounds(   0,20, 80, 80), channel("sqshape"), text("Shape"), textBox(1), range(2, 20.00, 10)
}
groupbox bounds( 100,260, 80, 100), text("PD Half X"), plant("pdhalfx"), visible(0), identchannel("7") {
rslider  bounds(   0,20, 80, 80), channel("PD_amount"), text("P.Distort"), textBox(1), range(-1, 1.00, 0,1,0.001)
}
groupbox bounds( 100,260, 80, 100), text("PD Half Y"), plant("pdhalfy"), visible(0), identchannel("8") {
rslider  bounds(   0,20, 80, 80), channel("PD_amountY"), text("P.Distort"), textBox(1), range(-1, 1.00, 0,1,0.001)
}
groupbox bounds( 100,260,380, 100), text("FOF"), plant("fof"), visible(0), identchannel("9") {
rslider  bounds(   0,20, 80, 80), channel("formant"), text("Formant"), textBox(1), range(50,10000,800,0.5,1)
rslider  bounds(  70,20, 80, 80), channel("FRatio"), text("Frq.Ratio"), textBox(1), range(1,32, 8,0.5,0.01)
checkbox  bounds( 150,45,100, 20), channel("FormOn"),  text("Formant"), radiogroup(1) , value(1)
checkbox  bounds( 150,65,100, 20), channel("RatioOn"), text("Frq.Ratio"),   radiogroup(1)
rslider  bounds( 230,20, 80, 80), channel("bandwidth"), text("Bandwidth"), textBox(1), range(1,1000, 50,0.5,1)
rslider  bounds( 300,20, 80, 80), channel("octdiv"), text("Oct.Div."), textBox(1), range(0,8, 0,1,0.01)
}
groupbox bounds( 100,260, 80, 100), text("Bit Depth"), plant("bitdepth"), visible(0), identchannel("10") {
rslider  bounds(   0,20, 80, 80), channel("bits"), text("Bits"), textBox(1), range(0, 16, 16,0.5,0.01)
}
groupbox bounds( 100,260, 80, 100), text("Samp. Rate"), plant("samplerate"), visible(0), identchannel("11") {
rslider  bounds(   0,20, 80, 80), channel("sr_hold"), text("SR Hold"), textBox(1), range(1, 100, 1,1,1)
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
			
giview	ftgen	1,0,1024,-10,0
giblank	ftgen	2,0,1024,2,0
gisine	ftgen	3,0,4096,10,1
gicos	ftgen	4,0,131072,9,1,1,90

giwfn	ftgen	5, 0, 131072, 10, 1			;A SINE WAVE
gioctfn	ftgen	6, 0, 1024, -19, 1, 0.5, 270, 0.5	;A HANNING-TYPE WINDOW
islope	=	10
gisqish	ftgen	7,0,4096,16, 0, 1024,-islope,1, 1024,islope,0, 1024,-islope,-1, 1024,islope,0
giexp	ftgen	0, 0, 1024, 19, 0.5, 0.5, 270, 0.5	;EXPONENTIAL CURVE

instr	1
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

 if changed(gksrc)==1 then
  Smsg	sprintfk	"visible(%d)",gksrc=2?1:0
  	chnset		Smsg,"2"
  Smsg	sprintfk	"visible(%d)",gksrc=3?1:0
  	chnset		Smsg,"3"
  Smsg	sprintfk	"visible(%d)",gksrc=4?1:0
  	chnset		Smsg,"4"
  Smsg	sprintfk	"visible(%d)",gksrc=5?1:0
  	chnset		Smsg,"5"
  Smsg	sprintfk	"visible(%d)",gksrc=6?1:0
  	chnset		Smsg,"6"
  Smsg	sprintfk	"visible(%d)",gksrc=7?1:0
  	chnset		Smsg,"7"
  Smsg	sprintfk	"visible(%d)",gksrc=8?1:0
  	chnset		Smsg,"8"
  Smsg	sprintfk	"visible(%d)",gksrc=9?1:0
  	chnset		Smsg,"9"
  Smsg	sprintfk	"visible(%d)",gksrc=10?1:0
  	chnset		Smsg,"10"
  Smsg	sprintfk	"visible(%d)",gksrc=11?1:0
  	chnset		Smsg,"11"
 endif
endin

instr	2
 icps	cpsmidi

 kporttime	linseg	0,0.001,0.05
 
 if i(gksrc)==1 then
  asig	poscil	1,icps
  


 elseif i(gksrc)==2 then
 
  gkpw	portk	gkpw,kporttime
  iwave	=	(i(gkwave)-1)*2
  kVCOharms	chnget	"VCOharms"
  if changed(kVCOharms)==1 then
   reinit RESTART_VCO
  endif
  RESTART_VCO:
  asig	vco2	0.8,icps,iwave,gkpw,0,i(kVCOharms)

 elseif i(gksrc)==3 then

  gkpow	portk	gkpow,kporttime
  asig		gbuzz		1, icps, gknp, gklp, gkpow, gicos

 elseif i(gksrc)==4 then

  gkFMratio	portk	gkFMratio,kporttime
  gkFMindex	portk	gkFMindex,kporttime
  asig foscili 1, icps, 1, gkFMratio, gkFMindex, gisine

 elseif i(gksrc)==5 then
  ktone	=	0
  if changed(gkoctcnt)==1 then
   reinit UPDATE
  endif
  UPDATE:
  kbrite	portk		gkbrite,kporttime
  asig 		hsboscil	1, 0, kbrite, icps, giwfn, gioctfn, i(gkoctcnt)

 elseif i(gksrc)==6 then

  kslope	chnget	"sqshape"
  if 	changed(kslope)==1 then
   reinit NEW_SQUARISH
  endif
  
  NEW_SQUARISH:
  islope	=	i(kslope)
  gisqish	ftgen	7,0,4096,16, 0, 1024,-islope,1, 1024,islope,0, 1024,-islope,-1, 1024,islope,0
  asig 		poscil	1, icps, gisqish

 elseif i(gksrc)==7 then
  asig 		poscil	1, icps, gisine
  kPD_amount	chnget	"PD_amount"
  kPD_amount	portk	kPD_amount,kporttime
  asig		pdhalf	asig,kPD_amount,1

 elseif i(gksrc)==8 then
  asig 		poscil	1, icps, gisine
  kPD_amount	chnget	"PD_amountY"
  kPD_amount	portk	kPD_amount,kporttime
  asig		pdhalfy	asig,kPD_amount,1

 elseif i(gksrc)==9 then
  kformant	chnget	"formant"
  kformant	portk	kformant,kporttime
  kbandwidth	chnget	"bandwidth"
  kbandwidth	portk	kbandwidth,kporttime
  koctdiv	chnget	"octdiv"
  koctdiv	portk	koctdiv,kporttime
  kFRatio	chnget	"FRatio"
  kFRatio	portk	kFRatio,kporttime
  kFormOn	chnget	"FormOn"
  kformant	=	(kFormOn==1?kformant:icps*kFRatio)
;  asig 	fof 	gkamp, kfund, kform,   gkoct,   gkband,    gkris, gkdur, gkdec, iolaps, ifna, ifnb, itotdur ;[, iphs] [, ifmode] [, iskip
  asig 		fof 	1,     icps, kformant,koctdiv,kbandwidth,  0.003, 0.1,   0.007,  3000, gisine, giexp, 3600 ;[, iphs] [, ifmode] [, iskip

 elseif i(gksrc)==10 then
  asig	poscil	1,icps
  kbits	chnget	"bits"
  kvalues		pow		2, kbits		;RAISES 2 TO THE POWER OF kbitdepth. THE OUTPUT VALUE REPRESENTS THE NUMBER OF POSSIBLE VALUES AT THAT PARTICULAR BIT DEPTH
  asig		=	(int((asig/0dbfs)*kvalues))/kvalues	;BIT DEPTH REDUCE AUDIO SIGNAL

 elseif i(gksrc)==11 then
  asig	poscil	1,icps
  ksr_hold	chnget	"sr_hold"
  asig		fold		asig,ksr_hold

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
