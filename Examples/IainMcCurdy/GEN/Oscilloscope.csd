Oscilliscope
How to use the table widget to create a simple oscilloscope

<Cabbage>
form caption("Oscilloscope"), size(840, 450), pluginID("gn10"), colour( 20, 20, 20,150)

table bounds(  5,  5, 830, 250), identchannel("table1"), tableNumbers(1), tablecolour("LightBlue"), amprange(-1,1)

label    bounds( 10, 258, 80, 11), text("Source")
combobox bounds( 10, 270, 80, 20), channel("source"), value(1), text("Sine","VCO","Buzz","FM","Hsboscil")
rslider  bounds( 20, 300, 60, 60), channel("level"), text("Level"), range(0, 1, 0.1)

groupbox bounds( 100, 260, 200, 100), text("VCO"), plant("VCO") {
label    bounds( 10,  38,110, 11), text("Waveform")
combobox bounds( 10,  50,110, 20), channel("wave"), value(1), text("Sawtooth","Square/PWM","Saw./Tri./Ramp")
rslider  bounds( 120,20, 80, 80), channel("pw"), text("Pulse Width"), textBox(1), range(0.01, 0.99, 0.5)
}
groupbox bounds( 310, 260, 200, 100), text("Buzz"), plant("buzz") {
rslider  bounds(   0,  20,  80, 80), channel("lp"), text("Lowest"), textBox(1), range(1, 20, 1, 1,1)
rslider  bounds(  60,  20,  80, 80), channel("np"), text("Number"), textBox(1), range(1, 80, 10, 1,1)
rslider  bounds( 120,  20,  80, 80), channel("pow"), text("Power"), textBox(1), range(0, 2.00,0.5)
}
groupbox bounds( 520, 260, 150, 100), text("FM"), plant("FM") {
rslider  bounds(   0,20, 80, 80), channel("FMratio"), text("Ratio"), textBox(1), range(0.5, 8.00, 1)
rslider  bounds(  70,20, 80, 80), channel("FMindex"), text("Index"), textBox(1), range(0,100.00, 1,0.5)
}
groupbox bounds( 680, 260, 150, 100), text("Hsboscil"), plant("hsboscil") {
rslider  bounds(   0,20, 80, 80), channel("brite"), text("Brite"), textBox(1), range(-4, 4.00, 0)
rslider  bounds(  70,20, 80, 80), channel("octcnt"), text("Width"), textBox(1), range(2, 10.00, 3,1,1)
}

keyboard bounds(  5,365,830, 80)
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
			
giview	ftgen	1,0, 4096,-2, 0	;
giblank	ftgen	2,0, 4096,-2,0
gisine	ftgen	3,0,4096,10,1
gicos	ftgen	0,0,131072,9,1,1,90

giwfn	ftgen	0, 0, 131072, 10, 1			;A SINE WAVE
gioctfn	ftgen	0, 0, 1024, -19, 1, 0.5, 270, 0.5	;A HANNING-TYPE WINDOW

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
endin

instr	2
 icps	cpsmidi

 kporttime	linseg	0,0.001,0.05
 
 if i(gksrc)==1 then
  asig	poscil	1,icps

 elseif i(gksrc)==2 then
 
  gkpw	portk	gkpw,kporttime
  iwave	=	(i(gkwave)-1)*2
  asig	vco2	0.8,icps,iwave,gkpw

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
  asig 		hsboscil	1, 0, gkbrite, icps, giwfn, gioctfn, i(gkoctcnt)
  rireturn
 endif

 aptr	phasor	icps
 
 	tablew	asig,aptr,giview,1

 if metro(10)==1 then
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
