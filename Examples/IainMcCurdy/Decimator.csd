<Cabbage>
form size(210, 125), caption("BitC")
image bounds(5, 5, 200, 90), colour("black"), outline("white")
rslider bounds(15, 15, 70, 70), channel("bit"), text("Depth"), colour("lightgrey") range(0, 16, 16)
checkbox pos(100, 15) text("On/Off"), channel("on_off")
</Cabbage>
<CsoundSynthesizer>
<CsOptions>
-d -n  
</CsOptions>
<CsInstruments>
sr = 44100
ksmps = 64
nchnls = 2

;Author: Iain McCurdy (2012)

/*
decimator - Sample rate / Bit depth reduce. Based on the work of Steven Cook. k-rate parameters.

DESCRIPTION
This opcode implements one possible algorithm for sample rate / bit depth reduction. It is based on the work of Steven Cook but varies in that it utilizes the local ksmps feature of the UDO and has k-rate input parameters (the original was i-rate and can be viewed here http://www.csounds.com/cook/csound/Decimator.orc)

SYNTAX
aout  decimator  ain, kbitdepth, ksrate

PERFORMANCE
ain  -  Audio input signal.

kbitdepth  -  The bit depth of signal aout. Typically in range (1-16). Floats are OK.

ksrate  -  The sample rate of signal aout. Non-integer values are OK.

CREDITS
Steven Cook. Implemented as a UDO by David Akbari - 2005.
*/

opcode	decimator, a, akk
	setksmps	1
ain, kbit, ksrate		xin

kbits    = 2^kbit			; Bit depth (1 to 16)
kfold    = (sr/ksrate)			; Sample rate
kin      downsamp  ain			; Convert to kr
kin      = (kin + 32768)		; Add DC to avoid (-)
kin      = kin*(kbits / 65536)		; Divide signal level
kin      = int(kin)			; Quantise
aout     upsamp  kin			; Convert to sr
aout     = aout * (65536/kbits) - 32768	; Scale and remove DC

a0ut     fold  aout, kfold		; Resample

	xout      a0ut

		endop
 

instr 1
kbit chnget "bit"
kOnOff chnget "on_off"
ain inch 1
adec decimator ain, kbit, sr
outs adec*kOnOff, adec*kOnOff
endin




</CsInstruments>
<CsScore>
f1 0 1024 10 1
i1 0 1000
</CsScore>
</CsoundSynthesizer>