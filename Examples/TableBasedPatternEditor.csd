<Cabbage>
form size(535, 402), caption("PatternTable"), pluginID("plu1"), colour(10, 10, 10)  
;table bounds(10, 10, 512, 160), tablenumbers(1), channel("table1"), colour("red")
image bounds(7, 6, 530, 289), shape("rounded"), line(0),  colour(192, 191, 191, 20), outline(255, 255, 255, 255),
table bounds(16, 16, 512, 30), tablenumbers(1), channel("table1"), colour("green")
table bounds(16, 46, 512, 30), tablenumbers(2), channel("table2"), colour("red")
table bounds(16, 76, 512, 30), tablenumbers(3), channel("table3"), colour("blue")
table bounds(16, 106, 512, 30), tablenumbers(4), channel("table4"), colour("yellow")
table bounds(16, 136, 512, 30), tablenumbers(5), channel("table5"), colour("white")
table bounds(16, 166, 512, 30), tablenumbers(6), channel("table6"), colour("cornflowerblue")
table bounds(16, 196, 512, 30), tablenumbers(7), channel("table7"), colour("indigo")
table bounds(16, 226, 512, 30), tablenumbers(8), channel("table8"), colour("brown")
table bounds(16, 256, 512, 30), tablenumbers(8), channel("table8"), colour("purple")
hslider bounds(8, 300, 500, 30), channel("tempo"), range(0.2, 4, 0.2, 1, 0.0001), text("tempo"), textbox(1),  colour(255, 255, 255, 255), trackercolour(255, 0, 0, 255), fontcolour(160, 160, 160, 255), 
hslider bounds(8, 332, 501, 30), channel("echo"), range(0, 1, 0, 1, 0.0001), text("Echo"), textbox(1),  colour(255, 255, 255, 255), trackercolour(0, 255, 0, 255), fontcolour(160, 160, 160, 255), 
</Cabbage>        
<CsoundSynthesizer>    
<CsOptions>       
-n -g -+rtmidi=null
;-+rtaudio=alsa -odac:hw:1,0
</CsOptions>  
<CsInstruments>        
sr = 44100          
ksmps = 64                        
0dbfs=1                         
nchnls = 2  
                      
   
instr 10
;tablew 0, 15, 1
chnset	-1, "table1"
chnset	-1, "table2"
chnset	-1, "table3"
chnset	-1, "table4"
chnset	-1, "table5"
chnset	-1, "table6"
chnset	-1, "table7"
chnset	-1, "table8"
chnset	-1, "table9"
endin
 
instr 20
kTempo chnget "tempo"
kEcho chnget "echo"
k1 oscil .15, kTempo, 1
a1 oscili k1, 800, 9
k1 oscil .15, kTempo, 2
a2 oscili k1, 700, 9
k1 oscil .15, kTempo, 3
a3 oscili k1, 600, 9
k1 oscil .15, kTempo, 4
a4 oscili k1, 500, 9
k1 oscil .15, kTempo, 5
a5 oscili k1, 400, 9
k1 oscil .15, kTempo, 6
a6 oscili k1, 300, 9
k1 oscil .15, kTempo, 7
a7 oscili k1, 200, 9
k1 oscil .15, kTempo, 8
a8 oscili k1, 100, 9
amix = a1+a2+a3+a4+a5+a6+a7+a8
aCombL comb amix, 2, .2
aCombR comb amix, 2, .3
outs amix+(kEcho*aCombL), amix+(aCombR*kEcho)
endin  
                  
</CsInstruments>          
<CsScore>   
f1 0 16 -2 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 
f2 0 16 -2 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 
f3 0 16 -2 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 
f4 0 16 -2 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 
f5 0 16 -2 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 
f6 0 16 -2 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 
f7 0 16 -2 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 
f8 0 16 -2 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 
f9 0 1024 10 1
i10 0 1
i20 1 1000         
</CsScore>  
</CsoundSynthesizer>         