<Cabbage>
form size(340, 130), caption("Mincer"), pluginID("minc")
rslider channel("timpoint"), bounds(10, 25, 70, 70), text("timpoint"), range(0, 1.00, 0, 1, 0.001)
rslider channel("timefine"), bounds(70, 25, 70, 70), text("timefine"), range(0, 1.00, 0, 1, 0.001)
rslider channel("amp"), bounds(130, 25, 70, 70), text("amp"), range(0, 1.00, 1, 0.35)
rslider channel("pitch"), bounds(190, 25, 70, 70), text("pitch"), range(0, 2.00, 1, 1, 0.001)
rslider channel("feedback"), bounds(250, 25, 70, 70), text("feedback"), range(0, 1.00, 0, 1, 0.001)
;csoundoutput bounds(4, 212, 648, 150)
</Cabbage>
<CsoundSynthesizer>
<CsOptions>
-n -d
</CsOptions>
<CsInstruments>
 
	instr 1


	endin


</CsInstruments>  
<CsScore>
f1 0 1024 10 1
i1 0 84600

</CsScore>
</CsoundSynthesizer>
