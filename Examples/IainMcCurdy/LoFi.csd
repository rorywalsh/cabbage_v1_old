<Cabbage>
form size(260, 120), caption("Lo Fi")
image pos(10, 0), size(240, 90), line(1), colour("Chocolate"), shape("rounded"), outline("white"), line(4)
rslider bounds(20,  11, 70, 70), text("Bits"), channel("bits"), range(0, 1, 0)
rslider bounds(100, 11, 70, 70), text("Foldover"), channel("fold"), range(1, 1024, 0, 0.25)
rslider bounds(170, 11, 70, 70), text("Level"), channel("level"), range(0, 1, 1)
}
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

;Iain McCurdy, 2012
;http://iainmccurdy.org/csound.html


opcode	LoFi,a,akk
	ain,kbits,kfold	xin									;READ IN INPUT ARGUMENTS
	kvalues		pow	2, ((1-(kbits^0.25))*15)+1					;RAISES 2 TO THE POWER OF kbitdepth. THE OUTPUT VALUE REPRESENTS THE NUMBER OF POSSIBLE VALUES AT THAT PARTICULAR BIT DEPTH
	k16bit		pow	2, 16								;RAISES 2 TO THE POWER OF 16
	aout		=	(int((ain*32768*kvalues)/k16bit)/32768)*(k16bit/kvalues)	;BIT DEPTH REDUCE AUDIO SIGNAL
	aout		fold 	aout, kfold							;APPLY SAMPLING RATE FOLDOVER
		xout	aout									;SEND AUDIO BACK TO CALLER INSTRUMENT
endop

instr 1
kbits chnget "bits"
kfold chnget "fold"
klevel chnget "level"
a1,a2	ins
kporttime	linseg	0,0.001,0.01
kfold	portk	kfold,kporttime
a1	LoFi	a1,kbits*0.6,kfold
a2	LoFi	a2,kbits*0.6,kfold
a1	=	a1 * klevel
a2	=	a2 * klevel
	outs	a1,a2
endin

</CsInstruments>

<CsScore>
i 1 0 [60*60*24*7]
</CsScore>

</CsoundSynthesizer>