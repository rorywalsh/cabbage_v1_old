<Cabbage>
form caption("Envelope Follower") size(320, 130)
image pos(10, 0), size(300, 100), line(1), colour("brown"), shape("rounded"), outline("white"), line(4) 
rslider bounds(20, 11, 70, 75), text("Sensitivity"), channel("sens"), range(0, 1, 0.65)
rslider bounds(90, 11, 70, 75), text("Frequency"), channel("freq"), range(10, 10000, 1000, 0.5)
rslider bounds(160, 11, 70, 75), text("Resonance"), channel("res"), range(0,  1, 0.75)
rslider bounds(230, 11, 70, 75), text("Level"), channel("level"), range(0, 1, 1)
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

;Author: Iain McCurdy (2012)

opcode	EnvelopeFollower,a,akkk
	ain,ksens,kfreq,kres	xin						;READ IN INPUT ARGUMENTS
	;				     ATTCK.REL.  -  ADJUST THE RESPONSE OF THE ENVELOPE FOLLOWER HERE
	aFollow		follow2		ain, 0.01, 0.05				;AMPLITUDE FOLLOWING AUDIO SIGNAL
	kFollow		downsamp	aFollow					;DOWNSAMPLE TO K-RATE
	kFollow		expcurve	kFollow/0dbfs,3				;ADJUSTMENT OF THE RESPONSE OF DYNAMICS TO FILTER FREQUENCY MODULATION
	kFrq		=		kfreq + (cpsoct(kFollow*ksens*150))	;CREATE A LEFT CHANNEL MODULATING FREQUENCY BASE ON THE STATIC VALUE CREATED BY kfreq AND THE AMOUNT OF DYNAMIC ENVELOPE FOLLOWING GOVERNED BY ksens
	kFrq		port		kFrq, 0.05				;SMOOTH CONTROL SIGNAL USING PORTAMENTO
	kFrq		limit		kFrq, 20,sr/3				;LIMIT FREQUENCY RANGE TO PREVENT OUT OF RANGE FREQUENCIES  
	;aout		moogladder	ain, kFrq, kres				;REDEFINE GLOBAL AUDIO SIGNAL AS FILTERED VERSION OF ITSELF
	aout		moogvcf2	ain, kFrq, kres				;REDEFINE GLOBAL AUDIO SIGNAL AS FILTERED VERSION OF ITSELF
			xout		aout					;SEND AUDIO BACK TO CALLER INSTRUMENT
endop

instr 1
ksens chnget "sens"
kfreq chnget "freq"
kres chnget "res"
klevel chnget "level"
a1,a2	ins
a1	EnvelopeFollower	a1,ksens,kfreq,kres*0.95
a2	EnvelopeFollower	a2,ksens,kfreq,kres*0.95
a1	=	a1 * klevel
a2	=	a2 * klevel
	outs	a1,a2
endin

</CsInstruments>
<CsScore>
i 1 0 [60*60*24*7]
</CsScore>
</CsoundSynthesizer>