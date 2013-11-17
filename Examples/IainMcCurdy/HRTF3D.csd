<Cabbage>
form caption("HRTF 3D") size(385, 250)
image pos(0, 0), size(365, 230), colour("Orange"), shape("rounded"), outline("white"), line(4) 
xypad bounds(10, 10, 250, 210), channel("Az", "Elev"), text("X=Azimuth : Y=Elevation"), rangex(-180,  180,   0), rangey(-40, 90, 0)
combobox bounds(265, 10, 90,25), channel("type"), value(2), text("hrtfmove", "hrtfmove2")
hslider bounds(265,  40, 90,55), channel("L_R_In"), range(0, 1, 0), caption("L/R In")
combobox bounds(265,100, 90,60), channel("overlap"), value(3), text("1", "2", "4", "8", "16", "32", "64"), caption("Overlap")
hslider bounds(265, 165, 90, 55), channel("radius"), range(1, 30, 9), caption("Radius")

}
</Cabbage>

<CsoundSynthesizer>

<CsOptions>
-d -n
</CsOptions>

<CsInstruments>

sr 		= 	44100	;SAMPLE RATE
ksmps 		= 	32	;NUMBER OF AUDIO SAMPLES IN EACH CONTROL CYCLE
nchnls 		= 	2	;NUMBER OF CHANNELS (2=STEREO)
0dbfs		=	1

;Author: Iain McCurdy (2012)

instr	1
	kAz		chnget		"Az"			;READ WIDGETS...
	kElev		chnget		"Elev"			;
	ktype		chnget		"type"			;
	kL_R_In		chnget		"L_R_In"		;
	koverlap	chnget		"overlap"		;
	kradius		chnget		"radius"		;
	aL,aR		ins					;READ STEREO AUDIO INPUT
	asrc		ntrpol		aL,aR,kL_R_In
	;asrc		diskin2		"bounce.wav",1,0,1	;USE SOUND FILE FOR TESTING
	kporttime	linseg		0,0.001,0.02		;PORTAMENTO TIME RISES UP QUICKLY FROM ZERO TO A HELD VALUE
	kAz		portk		kAz,kporttime		;SMOOTH RT CHANGES OF AZIMUTH
	kElev		portk		kElev,kporttime		;SMOOTH RT CHANGES OF ELEVATION
	kswitch		changed		koverlap,kradius	;IF 'OVERLAP' OR 'RADIUS' CHANGE, GENERATE A TRIGGER
	if kswitch==1 then					;IF A CHANGE TRIGGER HAS BEEN GENERATED...
	 reinit	update						;REINITIALISE FROM LABEL 'update'
	endif							;END OF CONDITIONAL BRANCH
	update:							;A LABEL. REINITIALISATION BEGINS FROM HERE
	ioverlap	=	2 ^ (i(koverlap) - 1)		;DERIVE OVERLAP VALUE (1,2,4,8 etc.) FROM COMBOBOX INDEX VALUES (1,2,3,4 etc.)
	if ktype==1 then					;IF 'hrtfmove' SELECTED USING COMBOBOX...
	 aleft, aright	hrtfmove 	asrc, kAz, kElev, "hrtf-44100-left.dat","hrtf-44100-right.dat",ioverlap, i(kradius);, isr]   	;APPLY hrtfmove2 OPCODE TO AUDIO SOURCE FROM INSTR 1
	else							;IF 'hrtfmove2' SELECTED USING COMBOBOX...
	 aleft, aright	hrtfmove2 	asrc, kAz, kElev, "hrtf-44100-left.dat","hrtf-44100-right.dat",ioverlap, i(kradius);, isr]   	;APPLY hrtfmove2 OPCODE TO AUDIO SOURCE FROM INSTR 1
	endif							;END OF CONDITIONAL BRANCH
	rireturn
			outs		aleft, aright		;SEND AUDIO TO OUTPUTS
endin
		
</CsInstruments>

<CsScore>
i 1 0 [3600*24*7]
</CsScore>

</CsoundSynthesizer>