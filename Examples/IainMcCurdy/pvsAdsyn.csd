<Cabbage>
form caption("pvsadsyn") size(440, 120)
image pos(0, 0), size(440, 90), colour("darkslategrey"), shape("rounded"), outline("white"), line(4) 
rslider bounds(10, 11, 70, 70), text("Freq.Mod."), channel("fmod"), range(0.25, 4, 1)
rslider bounds(80, 11, 70, 70), text("Num.Oscillators"), channel("noscs"), range(1, 1024, 256,1,1)
rslider bounds(150, 11, 70, 70), text("Bin Offset"), channel("binoffset"), range(0, 256, 1,1,1)
rslider bounds(220, 11, 70, 70), text("Bin Incr."), channel("binincr"), range(1, 32, 1,1,1)
rslider bounds(290, 11, 70, 70), text("Mix"), channel("mix"), range(0, 1, 1)
rslider bounds(360, 11, 70, 70), text("Level"), channel("lev"), range(0, 1, 0.5)
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


opcode	pvsadsyn_module,a,akkkkkkiiii
	ain,kfmod,knoscs,kbinoffset,kbinincr,kmix,klev,iFFTsize,ioverlap,iwinsize,iwintype	xin
	f_FB		pvsinit iFFTsize,ioverlap,iwinsize,iwintype, 0			;INITIALISE FEEDBACK FSIG
	f_anal  	pvsanal	ain, iFFTsize, ioverlap, iwinsize, iwintype		;ANALYSE AUDIO INPUT SIGNAL AND OUTPUT AN FSIG
	knossc	init	256
	ktrig		changed	knoscs,kbinoffset,kbinincr
	if ktrig==1 then
	 reinit	UPDATE
	endif
	UPDATE:
	inoscs	init	i(knoscs)	
	ibinoffset	init	i(kbinoffset)
	ibinincr	init	i(kbinincr)
	inoscs	limit	inoscs, 1, (((iFFTsize*0.5)+1)-ibinoffset)/ibinincr
	aresyn 		pvsadsyn f_anal, inoscs, kfmod , i(kbinoffset), i(kbinincr)	;RESYNTHESIZE FROM THE fsig USING pvsadsyn
	rireturn
	amix		ntrpol		ain, aresyn, kmix					;CREATE DRY/WET MIX
			xout		amix*klev	
endop

instr	1
	ainL,ainR	ins
	kmix		chnget	"mix"
	kfmod		chnget	"fmod"
	knoscs		chnget	"noscs"
	kbinoffset	chnget	"binoffset"
	kbinincr	chnget	"binincr"
	klev		chnget	"lev"
	iFFTsize	=	1024
	ioverlap	=	256
	iwinsize	=	1024
	iwintype	=	1
	aoutL		pvsadsyn_module	ainL,kfmod,knoscs,kbinoffset,kbinincr,kmix,klev,iFFTsize,ioverlap,iwinsize,iwintype
	aoutR		pvsadsyn_module	ainR,kfmod,knoscs,kbinoffset,kbinincr,kmix,klev,iFFTsize,ioverlap,iwinsize,iwintype
			outs	aoutR,aoutR
endin

</CsInstruments>

<CsScore>
i 1 0 [60*60*24*7]
</CsScore>

</CsoundSynthesizer>