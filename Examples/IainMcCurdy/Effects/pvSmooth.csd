<Cabbage>
form caption("pvsmooth") size(420, 120)
image bounds(0, 0, 420, 90), colour("Cream"), outline("silver"), line(4)
rslider bounds( 10, 8, 75, 75), text("Amp.Smooth"), channel("acf"), range(0, 1, 0),fontcolour("chocolate"),colour("chocolate")
rslider bounds( 90, 8, 75, 75), text("Frq.Smooth"), channel("fcf"), range(0, 1, 0),fontcolour("chocolate"),colour("chocolate")
rslider bounds(170, 8, 75, 75), text("Feedback"), channel("FB"), range(0, 1, 0),fontcolour("chocolate"),colour("chocolate")
rslider bounds(250, 8, 75, 75), text("Mix"), channel("mix"), range(0, 1, 1),fontcolour("chocolate"),colour("chocolate")
rslider bounds(330, 8, 75, 75), text("Level"), channel("lev"), range(0, 1, 0.5),fontcolour("chocolate"),colour("chocolate")
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
;Spectral smoothing effect.


opcode	pvsmooth_module,a,akkkkkiiii
	ain,kacf,kfcf,kfeedback,kmix,klev,iFFTsize,ioverlap,iwinsize,iwintype	xin
	f_FB		pvsinit iFFTsize,ioverlap,iwinsize,iwintype, 0			;INITIALISE FEEDBACK FSIG
	f_anal  	pvsanal	ain, iFFTsize, ioverlap, iwinsize, iwintype		;ANALYSE AUDIO INPUT SIGNAL AND OUTPUT AN FSIG
	f_mix		pvsmix	f_anal, f_FB						;MIX AUDIO INPUT WITH FEEDBACK SIGNAL
	f_smooth	pvsmooth	f_mix, kacf, kfcf					;BLUR AMPLITUDE AND FREQUENCY VALUES OF AN F-SIGNAL
	f_FB		pvsgain f_smooth, kfeedback 					;CREATE FEEDBACK F-SIGNAL FOR NEXT PASS
	aout		pvsynth f_smooth                      				;RESYNTHESIZE THE f-SIGNAL AS AN AUDIO SIGNAL
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
	kacf		chnget	"acf"
	kfcf		chnget	"fcf"
	kacf		=	1-kacf
	kfcf		=	1-kfcf
	kporttime	linseg	0,0.001,0.02
	kmix		chnget	"mix"
	klev		chnget	"lev"
	aoutL		pvsmooth_module	ainL,kacf,kfcf,kfeedback,kmix,klev,iFFTsize,ioverlap,iwinsize,iwintype
	aoutR		pvsmooth_module	ainR,kacf,kfcf,kfeedback,kmix,klev,iFFTsize,ioverlap,iwinsize,iwintype
			outs	aoutR,aoutR
endin

</CsInstruments>

<CsScore>
i 1 0 [60*60*24*7]
</CsScore>

</CsoundSynthesizer>