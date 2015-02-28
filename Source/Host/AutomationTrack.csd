<CsoundSynthesizer>
<CsOptions>
-n -d
</CsOptions>
<CsInstruments>
sr 	= 	44100
ksmps 	= 	32
nchnls 	= 	2
0dbfs	=	1

;this instrument is used for the automation
;track. Each table is 4096 in size but uses cubic interpolation
;during playback.  
;Each time a new automation track is added another 
;instance of instr1 is created with a unique channel
;and table number.

gkIndex init 0

instr 1000
kIsPlaying chnget "isPlaying"
reset:
	prints "Instrument 10 starting"
	
	if kIsPlaying ==1 then
		gkIndex phasor .01
		chnset gkIndex*10, "scrubberPosition"
	endif

	printk2 kIsPlaying

	if changed:k(kIsPlaying)==1 && kIsPlaying==0 then
	printks "Resetting\n", 0
	reinit reset 	;reinit if user presses stop
	endif
endin

instr 1
	kValue table3 gkIndex, p4, 1
	SChannel sprintfk "auto_%d", p4
	;kValue randi 1, 1
	chnset abs(kValue), SChannel
	;printk, 1, kValue
endin

</CsInstruments>
<CsScore>
i1000 0 [3600*24*7]
f0 36000
</CsScore>
</CsoundSynthesizer>
