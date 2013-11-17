<Cabbage>
form caption("pvsBlur"), size(190, 110)
rslider bounds(10, 10, 60, 60), text("Blur Time") channel("blurtime"), range(0, 2, 0.5)
rslider bounds(70, 10, 60, 60), text("Mix"), channel("mix"), range(0, 1, 0.5)
rslider bounds(130, 10, 60, 60), text("Level"), channel("lev"), range(0, 1, 0.5, 0.5)
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

;Author: Iain McCurdy (2012)
;http://iainmccurdy.org/csound.html


opcode	pvsblur_module,a,akkkkiiii
	ain,kblurtime,kfeedback,kmix,klev,iFFTsize,ioverlap,iwinsize,iwintype	xin
	f_FB		pvsinit iFFTsize,ioverlap,iwinsize,iwintype, 0			;INITIALISE FEEDBACK FSIG
	f_anal  	pvsanal	ain, iFFTsize, ioverlap, iwinsize, iwintype		;ANALYSE AUDIO INPUT SIGNAL AND OUTPUT AN FSIG
	f_mix		pvsmix	f_anal, f_FB						;MIX AUDIO INPUT WITH FEEDBACK SIGNAL
	f_blur		pvsblur	f_mix, kblurtime, 2					;BLUR AMPLITUDE AND FREQUENCY VALUES OF AN F-SIGNAL
	f_FB		pvsgain f_blur, kfeedback 					;CREATE FEEDBACK F-SIGNAL FOR NEXT PASS
	aout		pvsynth f_blur                      				;RESYNTHESIZE THE f-SIGNAL AS AN AUDIO SIGNAL
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
	kblurtime	chnget	"blurtime"
	kporttime	linseg	0,0.001,0.02
	kblurtime	portk	kblurtime,kporttime
	kmix		chnget	"mix"
	klev		chnget	"lev"
	aoutL		pvsblur_module	ainL,kblurtime,kfeedback,kmix,klev,iFFTsize,ioverlap,iwinsize,iwintype
	aoutR		pvsblur_module	ainR,kblurtime,kfeedback,kmix,klev,iFFTsize,ioverlap,iwinsize,iwintype
			outs	aoutR,aoutR
endin

</CsInstruments>

<CsScore>
i 1 0 [60*60*24*7]
</CsScore>

</CsoundSynthesizer>