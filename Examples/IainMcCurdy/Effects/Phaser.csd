<Cabbage>
form caption("Phaser") size(360, 120), pluginID("phsr")
image pos(0, 0), size(360, 90), colour("200,100,100"), shape("rounded"), outline(255,50,50), line(4) 
rslider bounds(  5, 10, 70, 70), text("Rate"), 		channel("rate"), 	range(0, 1, 0.2, 0.25, 0.001), colour("255,50,50"), fontcolour("black"), midiCtrl(1,1)
rslider bounds( 75, 10, 70, 70), text("Depth"), 	channel("depth"), 	range(0, 1, 0.5, 1, .01), colour(255,50,50), fontcolour("black")
rslider bounds(145, 10, 70, 70), text("Freq."), 	channel("freq"), 	range(0, 1, 0.4, 1, .01), colour(255,50,50), fontcolour("black")
rslider bounds(215, 10, 70, 70), text("Feedback"), 	channel("fback"), 	range(0, 1, 0.4, 1, .01), colour(255,50,50), fontcolour("black")
rslider bounds(285, 10, 70, 70), text("Level"), 	channel("level"), 	range(0, 1, 1, 1, .01), colour(255,50,50), fontcolour("black")
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
;http://iainmccurdy.org/csound.html


opcode	Phaser,a,akkkk
	ain,krate,kdepth,kfreq,kfback	xin					;READ IN INPUT ARGUMENTS
	krate		expcurve	krate,10				;CREATE AN EXPONENTIAL REMAPPING OF krate
	krate		scale	krate,14,0.01					;RESCALE 0 - 1 VALUE TO 0.01 - 14	
	klfo	lfo	kdepth*0.5, krate, 1					;LFO FOR THE PHASER (TRIANGULAR SHAPE)
	aout	phaser1	ain, cpsoct((klfo+(kdepth*0.5)+kfreq)), 8, kfback	;PHASER1 IS APPLIED TO THE INPUT AUDIO
		xout	aout							;SEND AUDIO BACK TO CALLER INSTRUMENT
endop

instr 1
krate chnget "rate"
kdepth chnget "depth"
kfreq chnget "freq"
kfback chnget "fback"
klevel chnget "level"
a1,a2	ins
a1	Phaser	a1,krate,kdepth*8,(kfreq*5)+6,kfback*0.9	
a2	Phaser	a2,krate,kdepth*8,(kfreq*5)+6,kfback*0.9	
a1	=	a1 * klevel
a2	=	a2 * klevel
	outs	a1,a2
endin

</CsInstruments>

<CsScore>
i 1 0 [60*60*24*7]
</CsScore>

</CsoundSynthesizer>