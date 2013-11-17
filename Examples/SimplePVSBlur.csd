<Cabbage> 
form caption("PVS Blur") size(450, 90), colour(150, 150, 150)
image bounds(1, 1, 450, 90), colour(40, 40, 40), outline(150, 150, 150), line(4)
hslider bounds(10, 1, 430, 50), channel("blur"), range(0, 2, 1), colour("yellow"), tracker("cornflowerblue"), textbox(1), text("Blur time") 
hslider bounds(10, 40, 430, 50), channel("space"), range(0, 3, 1), colour("red"), textbox(1), text("Space") 
</Cabbage> 
<CsoundSynthesizer> 
<CsOptions> 
-d -n -+rtmidi=null -M0 -b1024 
</CsOptions> 
<CsInstruments> 
sr = 44100 
ksmps = 32 
nchnls = 2 

instr 1 
;access data from named channels
kblurtime chnget "blur" 
kpanfreq chnget "space"
asig inch 1                                
fsig  pvsanal   asig, 1024, 256, 1024, 1 
ftps  pvsblur   fsig, kblurtime, 2         
atps  pvsynth   ftps  
apan jspline 1, 1, kpanfreq 
outs atps*apan, atps*(1-apan) 
endin 
        
</CsInstruments> 
<CsScore> 
f1 0 1024 10 1 
i1 0 3600 
</CsScore> 
</CsoundSynthesizer>     