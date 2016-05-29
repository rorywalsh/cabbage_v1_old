; LiveBufferDisplay.csd
; Written by Iain McCurdy, 2014

<Cabbage>
form caption("Live Buffer Display"), size(610, 245), pluginID("LiBT"), colour("DarkSLateGrey"), guirefresh(64)
gentable      bounds(  5,  5,600,120), tablenumber(1), tablecolour("lime"), amprange(-1,1,1), identchannel(table), zoom(-1)

hslider    bounds(  0,120, 610, 35), channel("ptr"),    range(0, 1.00, 1)
label      bounds(205,150,200,  12), text("Pointer"), fontcolour("white")

checkbox bounds(  5,150, 70, 20), channel("freeze"), text("Freeze"), fontcolour("white")

rslider    bounds( 20,180, 60, 60), channel("InGain"), text("Input Gain"), textcolour("white"), range(0, 8.00, 1)
rslider    bounds( 80,180, 60, 60), channel("DryGain"), text("Dry Gain"), textcolour("white"), range(0, 8.00, 1)
rslider    bounds(140,180, 60, 60), channel("FreezeGain"), text("Frze Gain"), textcolour("white"), range(0, 8.00, 1)

</Cabbage>
                    
<CsoundSynthesizer>

<CsOptions>   
-dm0 -n
</CsOptions>

<CsInstruments>

sr 		= 	44100	; SAMPLE RATE
ksmps 		= 	64	; NUMBER OF AUDIO SAMPLES IN EACH CONTROL CYCLE
nchnls 		= 	2	; NUMBER OF CHANNELS
0dbfs		=	1	; MAXIMUM AMPLITUDE
			
giDispBuffer	ftgen	1,0, 131072/ksmps,10, 0	; define live audio buffer table for display
giAudBuffer	ftgen	2,0, 131072,10, 0	; define live audio buffer table

instr	1
	aL,aR	ins
	kfreeze	chnget	"freeze"

	koffset	init	0	
	kInGain	chnget	"InGain"
	aL	*=	kInGain
	aR	*=	kInGain
	kDryGain	chnget	"DryGain"
	kFreezeGain	chnget	"FreezeGain"
		
	if kfreeze==0 then

	
	 aptr	phasor	sr/ftlen(giAudBuffer)
		tablew	aL,aptr,giAudBuffer,1

	 iDispTabLen	=	ftlen(giDispBuffer)				; table length
	 aDispTabLen	init	iDispTabLen					; table length (as an a-rate variable)

	 koffset	wrap	koffset+1,0,iDispTabLen
	 
	 	tablew	aL,aDispTabLen,giDispBuffer			; write current audio live sample to table
	 
	 ; SHUNT ENTIRE TABLE CONTENTS ONE STEP TO THE RIGHT
	 kcount	=	0
	 loop:
	 kval	table	kcount+1,giDispBuffer
	 	tablew	kval,kcount,giDispBuffer
	 loop_lt	kcount,1,iDispTabLen,loop	

	 if metro(16)==1 then
       	  chnset	"tablenumber(1)", "table"	; update table display	
	 endif
	 
	 		outs	aL*kDryGain,aR*kDryGain
	 
	else	
	
	 kptr	chnget	"ptr"
	 kporttime linseg 0,0.001,0.01
	 kptr portk kptr,kporttime
	 kptr	wrap	kptr+(koffset/iDispTabLen),0,1
	 aptr	interp	kptr*(ftlen(giAudBuffer)/sr)
	 asig 	mincer 	aptr, 1, 1, giAudBuffer, 0, 1024
	 	outs	asig*kFreezeGain,asig*kFreezeGain

	endif

endin

</CsInstruments>

<CsScore>
i 1 0 [3600*24*7]
</CsScore>

</CsoundSynthesizer>
