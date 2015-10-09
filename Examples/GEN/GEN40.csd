; GEN40.csd
; Written by Iain McCurdy, 2014

<Cabbage>
form caption("GEN40"), size(410, 230), pluginID("gn40"), colour("20,70,170,150")

gentable bounds(  5,  5, 400, 120), tablenumber(1), tablecolour("yellow"), identchannel("table1"), zoom(-1), amprange(0,1,1)


</Cabbage>
                    
<CsoundSynthesizer>

<CsOptions>   
-dm0 -n -+rtmidi=null -M0
</CsOptions>

<CsInstruments>

sr 		= 	44100	; SAMPLE RATE
ksmps 		= 	32	; NUMBER OF AUDIO SAMPLES IN EACH CONTROL CYCLE
nchnls 		= 	2	; NUMBER OF CHANNELS (1=MONO)
0dbfs		=	1	; MAXIMUM AMPLITUDE
			
gidata		ftgen	2,0,131072,21,3,1
gidist		ftgen	1,0,1024,40,2

instr	1
endin

</CsInstruments>

<CsScore>
i 1 0 [3600*24*7]
e
</CsScore>

</CsoundSynthesizer>
