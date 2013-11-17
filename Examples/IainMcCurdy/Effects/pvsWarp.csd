<Cabbage>
form caption("pvsWarp") size(370, 120)
image pos(0, 0), size(370, 90), colour("DarkSlateGrey"), shape("rounded"), outline("white"), line(4) 
rslider bounds(10, 11, 70, 70), text("Scale"), channel("scal"), range(0.1, 10, 1, 0.5, 0.0001) , colour("LightSlateGrey"), fontcolour("white")
rslider bounds(80, 11, 70, 70), text("Shift"), channel("shift"), range(0, 5000, 1, 0.5), colour("LightSlateGrey"), fontcolour("white")
rslider bounds(150, 11, 70, 70), text("Feedback"), channel("FB"), range(0, 1, 0), colour("LightSlateGrey"), fontcolour("white")
rslider bounds(220, 11, 70, 70), text("Mix"), channel("mix"), range(0, 1, 1), colour("LightSlateGrey"), fontcolour("white")
rslider bounds(290, 11, 70, 70), text("Level"), channel("lev"), range(0, 1, 0.5), colour("LightSlateGrey"), fontcolour("white")
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


opcode	pvswarp_module,a,akkkkkiiii
	ain,kscal,kshift,kfeedback,kmix,klev,iFFTsize,ioverlap,iwinsize,iwintype	xin
	f_FB		pvsinit iFFTsize,ioverlap,iwinsize,iwintype, 0			;INITIALISE FEEDBACK FSIG
	f_anal  	pvsanal	ain, iFFTsize, ioverlap, iwinsize, iwintype		;ANALYSE AUDIO INPUT SIGNAL AND OUTPUT AN FSIG
	f_mix		pvsmix	f_anal, f_FB						;MIX AUDIO INPUT WITH FEEDBACK SIGNAL
	f_warp		pvswarp	f_mix, kscal, kshift					;BLUR AMPLITUDE AND FREQUENCY VALUES OF AN F-SIGNAL
	f_FB		pvsgain f_warp, kfeedback 					;CREATE FEEDBACK F-SIGNAL FOR NEXT PASS
	aout		pvsynth f_warp                      				;RESYNTHESIZE THE f-SIGNAL AS AN AUDIO SIGNAL
	amix		ntrpol		ain, aout, kmix					;CREATE DRY/WET MIX
			xout		amix*klev	
endop

instr	1
	ainL,ainR	ins
	kscal		chnget	"scal"
	kshift		chnget	"shift"
	kfeedback	chnget	"FB"
	kmix		chnget	"mix"
	klev		chnget	"lev"
	iFFTsize	=	1024
	ioverlap	=	256
	iwinsize	=	1024
	iwintype	=	1
	aoutL		pvswarp_module	ainL,kscal,kshift,kfeedback,kmix,klev,iFFTsize,ioverlap,iwinsize,iwintype
	aoutR		pvswarp_module	ainR,kscal,kshift,kfeedback,kmix,klev,iFFTsize,ioverlap,iwinsize,iwintype
			outs	aoutR,aoutR
endin

</CsInstruments>

<CsScore>
i 1 0 [60*60*24*7]
</CsScore>

</CsoundSynthesizer>