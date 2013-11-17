<Cabbage>
form caption("pvscale Pitch Shifter") size(360, 120)
rslider bounds(10, 10, 70, 70), text("Semitones"), channel("semis"), range(-24, 24, 7, 1, 1)
rslider bounds(80, 10, 70, 70), text("Cents"), channel("cents"), range(-100, 100, 0, 1, 1)
rslider bounds(150, 10, 70, 70), text("Feedback"), channel("FB"), range(0, 1, 0)
rslider bounds(220, 10, 70, 70), text("Mix"), channel("mix"), range(0, 1, 0.5)
rslider bounds(290, 10, 70, 70), text("Level"), channel("lev"), range(0, 1, 0.5, 0.5)
</Cabbage>
<CsoundSynthesizer>
<CsOptions>
-d -n
</CsOptions>
<CsInstruments>

sr 		= 	44100
ksmps 		= 	32
nchnls 		= 	2
0dbfs		=	1	;MAXIMUM AMPLITUDE

;Iain McCurdy
;http://iainmccurdy.org/csound.html
;Pitch shifting effect using pvs scale opcode.


opcode	pvscale_module,a,akkkkiiii
	ain,kscale,kfeedback,kmix,klev,iFFTsize,ioverlap,iwinsize,iwintype	xin
	aFB		init	0
	;f_FB		pvsinit iFFTsize,ioverlap,iwinsize,iwintype, 0			;INITIALISE FEEDBACK FSIG
	f_anal  	pvsanal	ain+aFB, iFFTsize, ioverlap, iwinsize, iwintype		;ANALYSE AUDIO INPUT SIGNAL AND OUTPUT AN FSIG
	;f_mix		pvsmix	f_anal, f_FB						;MIX AUDIO INPUT WITH FEEDBACK SIGNAL
	f_scale		pvscale f_anal, kscale						;RESCALE FREQUENCIES
	;f_FB		pvsgain f_scale, kfeedback 					;CREATE FEEDBACK F-SIGNAL FOR NEXT PASS
	aout		pvsynth f_scale                      				;RESYNTHESIZE THE f-SIGNAL AS AN AUDIO SIGNAL
	aFB		=	aout*kfeedback
	amix		ntrpol		ain, aout, kmix					;CREATE DRY/WET MIX
			xout		amix*klev	
endop

instr	1
	ainL,ainR	ins
	kmix	chnget	"mix"
	kFB	chnget	"FB"

	iFFTsize	=	1024
	ioverlap	=	256
	iwinsize	=	1024
	iwintype	=	1
	
	kfeedback	chnget	"FB"
	ksemis		chnget	"semis"
	kcents		chnget	"cents"
	kmix		chnget	"mix"
	klev		chnget	"lev"
	kporttime	linseg	0,0.001,0.02
	kscale		=	semitone(ksemis)*cent(kcents)
	aoutL		pvscale_module	ainL,kscale,kfeedback,kmix,klev,iFFTsize,ioverlap,iwinsize,iwintype
	aoutR		pvscale_module	ainR,kscale,kfeedback,kmix,klev,iFFTsize,ioverlap,iwinsize,iwintype
			outs	aoutR,aoutR
endin

</CsInstruments>

<CsScore>
i 1 0 [60*60*24*7]
</CsScore>

</CsoundSynthesizer>