<Cabbage>
form caption("Pitch Shifter") size(360, 120)
image bounds(0, 0, 360, 90), colour("black"), outline("white")
rslider bounds(5, 10, 70, 70), text("Mix"), channel("mix"), range(0, 1, 0.5)
rslider bounds(75, 10, 70, 70), text("Pitch"), channel("pitch"), range(-0.12, 0.12, 0.07)
rslider bounds(145, 10, 70, 70), text("Fine"), channel("fine"), range(-1, 1, 0)
rslider bounds(215, 10, 70, 70), text("Feedback"), channel("fback"), range(0, 1, 0)
rslider bounds(285, 10, 70, 70), text("Level"), channel("level"), range(0, 1, 1)
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

;Iain McCurdy
;http://iainmccurdy.org/csound.html
;Pitch shifting effect using pvs streaming FFT technology.


opcode	PitchShifter,a,akkkk
	ain,kmix,kpitch,kfine,kfback	xin			;READ IN INPUT ARGUMENTS
	iWet	ftgentmp	0,0,1024,-7,0,512,1,512,1	;RESCALING FUNCTION FOR WET LEVEL CONTROL
	iDry	ftgentmp	0,0,1024,-7,1,512,1,512,0	;RESCALING FUNCTION FOR DRY LEVEL CONTROL
	kWet	table	kmix, iWet, 1				;RESCALE WET LEVEL CONTROL ACCORDING TO FUNCTION TABLE iWet
	kDry	table	kmix, iDry, 1				;RESCALE DRY LEVEL CONTROL ACCORDING TO FUNCTION TABLE iWet
	aPS	init	0                                       ;INITIALIZE aOutL FOR FIRST PERFORMANCE TIME PASS
	kscal	=	octave(((kpitch*1000)/12)+kfine)	;DERIVE PITCH SCALING RATIO. NOTE THAT THE 'COARSE' PITCH DIAL BECOMES STEPPED IN SEMITONE INTERVALS
	fsig1 	pvsanal	ain+(aPS*kfback), 1024,256,1024,0	;PHASE VOCODE ANALYSE LEFT CHANNEL
	fsig2 	pvscale fsig1, kscal				;RESCALE PITCH
	aPS 	pvsynth fsig2					;RESYNTHESIZE FROM FSIG
	aout	sum 	ain*kDry, aPS*kWet			;REDEFINE GLOBAL AUDIO SIGNAL FROM MIX OF DRY AND WET SIGNALS
		xout	aout					;SEND AUDIO BACK TO CALLER INSTRUMENT
endop

instr 1
kmix chnget "mix"
kpitch chnget "pitch"
kfine chnget "fine"
kfback chnget "fback"
klevel chnget "level"
a1,a2	ins

;a1, a2 diskin2 "Guitar1.wav", 1, 0, 1
a1	PitchShifter	a1,kmix,kpitch*0.1,kfine/12,kfback	
a2	PitchShifter	a2,kmix,kpitch*0.1,kfine/12,kfback	
a1	=	a1 * klevel
a2	=	a2 * klevel
	outs	a1,a2
endin

</CsInstruments>
<CsScore>
i 1 0 [60*60*24*7]
</CsScore>
</CsoundSynthesizer>