<Cabbage>
form size(400, 140), caption("PythonTest"), pluginID("plu1")


</Cabbage>
<CsoundSynthesizer>
<CsOptions>
-n -d -+rtmidi=NULL -M0 --midi-key-cps=4 --midi-velocity-amp=5
</CsOptions>
<CsInstruments>
sr = 44100
ksmps = 64
nchnls = 2
0dbfs=1

pyinit
pyruni {{
def f(x):
	return x + 5
}}

	instr 1
ires	pycall1i "f", 5.00
	print	ires
	endin

</CsInstruments>  
<CsScore>
f0 3600
</CsScore>
</CsoundSynthesizer>