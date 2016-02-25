<Cabbage>
form size(520, 350), caption("FFTDisplay")
fftdisplay bounds(10, 0, 500, 300), colour("cornflowerblue"), identchannel("fft")
button bounds(12, 304, 120, 25), channel("displaytype"), text("Spectroscope", "Spectrogram")
</Cabbage>
<CsoundSynthesizer> 
<CsOptions>
-n --displays
</CsOptions>
<CsInstruments>
sr = 44100
ksmps = 16
nchnls = 2
0dbfs=1

instr 1

kFreq randi 1000, 2
a1 vco2 1, kFreq			;generate some frequencies using a sawtooth 
outs a1, a1
							;dispfft takes a signal, analyses it, and send the FFT data
							;to the fftdisplay widget
dispfft a1, .1, 2048


							; allows users to toggle the type of display
kChangeDisplay chnget "displaytype"
if changed:k(kChangeDisplay)==1 then
	if kChangeDisplay==1 then
	chnset "displaytype(\"spectrogram\")", "fft"
	else
	chnset "displaytype(\"spectroscope\")", "fft"
	endif
endif

endin


</CsInstruments>  
<CsScore>
i1 0 3600
</CsScore>
</CsoundSynthesizer>