<Cabbage>
form caption("Quadbezier"), size(620,400), pluginID("qb65"), colour(13, 50, 67,50)


gentable bounds( 15, 30, 580, 360), tablenumbers(3), tablecolour("red"), tablebackgroundcolour("white"), tablegridcolour(230,230,230), identchannel("table"), amprange(0,1,1), zoom(-1), active(1)


</Cabbage>
                  
<CsoundSynthesizer>

<CsOptions>   
-m0 -+rtmidi=null -M0 
</CsOptions>

<CsInstruments>

sr 		= 	44100	;SAMPLE RATE
ksmps 		= 	32	;NUMBER OF AUDIO SAMPLES IN EACH CONTROL CYCLE
nchnls 		= 	2	;NUMBER OF CHANNELS (1=MONO)
0dbfs		=	1	;MAXIMUM AMPLITUDE

gi2	ftgen	6, 0,   800, "quadbezier", 0, 0, 0, 99, 0, 100, 0, 100, 100, 450, 300, 800, 100

instr	1
;	outs	asig, asig
;	ftsave "table.ftsave", 1, 4
endin

</CsInstruments>

<CsScore>
f 2 0 800 "quadbezier" 0 0 0 99 0 100 0 100 100 100 100 800 100
f 3 0 800 "quadbezier" 0 40 46 70 80 150 50 300 100 600 300 800 20
f 4 0 800 "quadbezier" 100 450 150 800 299
f 5 0 800 -7 0 100 150 800 300

; play instrument 1 for 1 hour
i 1 0 3600
</CsScore>

</CsoundSynthesizer>
