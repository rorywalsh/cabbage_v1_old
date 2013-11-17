<Cabbage>
form caption("pvsBlur"), size(290, 145) colour("DarkGrey")
hslider bounds( 10,  3,280, 40), text("Blur Time"), channel("blurtime"), range(0, 2, 0.5), fontcolour("white")
hslider bounds( 10, 43,280, 40), text("Mix"), channel("mix"), range(0, 1, 0.5)    , fontcolour("white")
hslider bounds( 10, 83,280, 40), text("Level"), channel("lev"), range(0, 1, 0.5, 0.5)    , fontcolour("white")
</Cabbage>
<CsoundSynthesizer>
<CsOptions>
-d -n
</CsOptions>
<CsInstruments>
sr 		= 	44100
ksmps 		= 	64
nchnls 		= 	2
0dbfs		=	1	;MAXIMUM AMPLITUDE

;Author: Iain McCurdy (2012)
;http://iainmccurdy.org/csound.html


opcode	pvsblur_module,a,akkkiiii
	ain,kblurtime,kmix,klev,iFFTsize,ioverlap,iwinsize,iwintype	xin
	f_anal  	pvsanal	ain, iFFTsize, ioverlap, iwinsize, iwintype		;ANALYSE AUDIO INPUT SIGNAL AND OUTPUT AN FSIG
	f_blur		pvsblur	f_anal, kblurtime, 2					;BLUR AMPLITUDE AND FREQUENCY VALUES OF AN F-SIGNAL
	aout		pvsynth f_blur                      				;RESYNTHESIZE THE f-SIGNAL AS AN AUDIO SIGNAL
	amix		ntrpol		ain, aout, kmix					;CREATE DRY/WET MIX
			xout		amix*klev	
endop

instr	1
	kblurtime	chnget	"blurtime"
	kmix		chnget	"mix"
	klev		chnget	"lev"

	ainL,ainR	ins
	;ainL,ainR	diskin	"808loop.wav",1,0,1	;USE FOR TESTING

	iFFTsize	=	1024
	ioverlap	=	512
	iwinsize	=	1024
	iwintype	=	1
	
	kporttime	linseg	0,0.001,0.02
	kblurtime	portk	kblurtime,kporttime
	aoutL		pvsblur_module	ainL,kblurtime,kmix,klev,iFFTsize,ioverlap,iwinsize,iwintype
	aoutR		pvsblur_module	ainR,kblurtime,kmix,klev,iFFTsize,ioverlap,iwinsize,iwintype
			outs	aoutR,aoutR
endin

</CsInstruments>

<CsScore>
i 1 0 [60*60*24*7]
</CsScore>

</CsoundSynthesizer>