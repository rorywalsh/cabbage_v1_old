<Cabbage>
form caption("pvsBuffer") size(510, 120)
image pos(0, 0), size(510, 90), line(1), colour("firebrick"), shape("rounded"), outline("white"), line(4) 
rslider bounds(10, 11, 70, 70), text("Speed"), channel("speed"), range(0, 4, 1, 0.5, 0.0001)
rslider bounds(80, 11, 70, 70), text("Buf. Size"), channel("buflen"), range(0.1, 8, 1, 0.5)
rslider bounds(150, 11, 70, 70), text("Semitones"), channel("semis"), range(-24, 24, 0, 1, 1)
rslider bounds(220, 11, 70, 70), text("Cents"), channel("cents"), range(-100, 100, 0, 1, 1)
rslider bounds(290, 11, 70, 70), text("Feedback"), channel("FB"), range(0, 1, 0)
rslider bounds(360, 11, 70, 70), text("Mix"), channel("mix"), range(0, 1, 1)
rslider bounds(430, 11, 70, 70), text("Level"), channel("lev"), range(0, 1, 0.5)
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


opcode	pvsbuffer_module,a,akkkkkkiiii
	ain,kspeed,kbuflen,kscale,kfeedback,kmix,klev,iFFTsize,ioverlap,iwinsize,iwintype	xin
	kPhOffset	=	0
	ktrig		changed		kbuflen
	ibuflen	init	1
	kspeed	init	1
	kscale	init	1
	if ktrig==1 then
	 reinit	UPDATE
	endif
	UPDATE:
	ibuflen		=	i(kbuflen)
	iphasor		ftgen		0, 0, 65536, 7, 0, 65536, 1			;WAVE SHAPE FOR A MOVING PHASE POINTER
	aread 		osciliktp 	kspeed/ibuflen, iphasor, kPhOffset		;CREATE MOVING POINTER TO READ FROM BUFFER
	kread		downsamp	aread
	kread		=		kread * ibuflen					;RESCALE READ POINTER WITH PHASOR RANGE SLIDER
	aFB		init		0
	f_anal 		pvsanal		ain+aFB, iFFTsize, ioverlap, iwinsize, iwintype	;ANALYSE THE AUDIO SIGNAL THAT WAS CREATED IN INSTRUMENT 1. OUTPUT AN F-SIGNAL.
	ibuffer,ktime  	pvsbuffer   	f_anal, ibuflen					;BUFFER FSIG
	rireturn
	khandle		init 		ibuffer						;INITIALISE HANDLE TO BUFFER
	f_buf  		pvsbufread  	kread , khandle					;READ BUFFER
	f_scale		pvscale 	f_buf, kscale					;RESCALE FREQUENCIES
	aresyn 		pvsynth  	f_scale			                   	;RESYNTHESIZE THE f-SIGNAL AS AN AUDIO SIGNAL	
	aFB		=		aresyn*kfeedback
	amix		ntrpol		ain, aresyn, kmix				;CREATE DRY/WET MIX
			xout		amix*klev	
endop

instr	1
	ainL,ainR	ins
	kspeed		chnget	"speed"
	kbuflen		chnget	"buflen"
	ksemis		chnget	"semis"
	kcents		chnget	"cents"
	ksemis		init	0
	kcents		init	0
	kscale		=	semitone(ksemis)*cent(kcents)
	kscale		init	1
	kbuflen		init	1
	kspeed		init	1
	kmix		chnget	"mix"
	kfeedback	chnget	"FB"
	klev		chnget	"lev"
	kmix		init	1
	kfeedback	init	0
	klev		init	0.5
	iFFTsize	=	1024
	ioverlap	=	256
	iwinsize	=	1024
	iwintype	=	1
	aoutL		pvsbuffer_module	ainL,kspeed,kbuflen,kscale,kfeedback,kmix,klev,iFFTsize,ioverlap,iwinsize,iwintype
	aoutR		pvsbuffer_module	ainR,kspeed,kbuflen,kscale,kfeedback,kmix,klev,iFFTsize,ioverlap,iwinsize,iwintype
			outs	aoutL,aoutR
endin

</CsInstruments>
<CsScore>
i 1 0.1 [60*60*24*7]
</CsScore>
</CsoundSynthesizer>