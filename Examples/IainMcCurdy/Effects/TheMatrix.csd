TheMatrix.csd


<Cabbage>
form caption("The Matrix") size(350, 500)
vslider bounds( 15,  5, 70, 480), text("Speed"), channel("ndx"), range(0, 4095, 2730,1,1), TextBox(1)
rslider bounds( 15,455, 70,  30), channel("speed"), range(-8, 8, 1), text("speed"), TextBox(1)
label   bounds( 90,  16,175, 39), text("Matrix"), FontColour( 75, 75, 75)
label   bounds(105,  18,175, 42), text("Matrix"), FontColour(125,125,125)
label   bounds(120,  20,175, 45), text("Matrix"), FontColour(255,255,255)

groupbox bounds( 90, 90,100, 45), line(0), {
checkbox bounds(100,100, 90, 25), colour("red"), text("DANGER!"), shape("circle"), channel("danger"), value(1)

}
</Cabbage>

<CsoundSynthesizer>

<CsOptions>
-dm0 -n
</CsOptions>

<CsInstruments>
sr 	= 	44100
ksmps 	= 	64
nchnls 	= 	2
0dbfs	=	1
seed	0

;Author: Iain McCurdy (2012)

gifunc	ftgen	0,0,4096,-16, -8,1365,-4, 0,1365,0, 1,1365,4, 8

instr	1
	kndx	chnget	"ndx"
	kspeed	table	kndx,gifunc
		chnset	kspeed,"speed"
	kdanger	lfo	1,1,3
		chnset	kdanger,"danger"

	ain		inch	1
	kPhOffset	=	0
	ibuflen		=	30
	aread 		phasor 		kspeed/ibuflen
	kread		downsamp	aread
	kread		=		kread * ibuflen					;RESCALE READ POINTER WITH PHASOR RANGE SLIDER
	iFFTsize	=	1024
	ioverlap	=	256
	iwinsize	=	1024
	iwintype	=	1
	f_anal 		pvsanal		ain, iFFTsize, ioverlap, iwinsize, iwintype	;ANALYSE THE AUDIO SIGNAL THAT WAS CREATED IN INSTRUMENT 1. OUTPUT AN F-SIGNAL.
	ibuffer,ktime  	pvsbuffer   	f_anal, ibuflen					;BUFFER FSIG
	khandle		init 		ibuffer						;INITIALISE HANDLE TO BUFFER
	f_buf  		pvsbufread  	ktime , khandle					;READ BUFFER
	aresyn 		pvsynth  	f_buf			                   	;RESYNTHESIZE THE f-SIGNAL AS AN AUDIO SIGNAL	
			outs		aresyn,aresyn
endin

</CsInstruments>

<CsScore>
i 1 0 [3600*24*7]
</CsScore>

</CsoundSynthesizer>