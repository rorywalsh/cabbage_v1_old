<Cabbage>
form size(1035, 500), caption("Spook-ePad"), pluginID("epad"), colour(10, 10, 10), guirefresh(32), androidinfo("Spook-ePad instrument, by Rory Walsh")
image bounds(294, 0, 582, 305), alpha(1), child(0), shape("sharp"), linethickness(1), colour(160, 160, 160, 50),
groupbox bounds(302, 8, 573, 295), text("Index Envelope"), colour(0, 0, 0, 50), caption("groupbBox")
gentable bounds(310, 34, 555, 148), tablenumber(2), amprange(0, 1, 2), zoom(-1), tablegridcolour(160, 160, 160, 60), identchannel("table1")
rslider bounds(876, 6, 150, 150), channel("car"), range(0.5, 5, 1, 1, 0.055), alpha(1), text("Car Factor"), 
rslider bounds(876, 158, 150, 150), channel("mod"), range(0.1, 20, 13), text("Mod Factor") 
rslider bounds(886, 312, 140, 140), channel("amp"), range(0, 1, 0.9, .5), text("Amplitude")   
rslider bounds(310, 188, 110, 110), channel("filterAtt"), range(0.01, 1, 0.5, 1, 0.01), alpha(1), text("Att."), 
rslider bounds(422, 188, 110, 110), channel("filterDec"), range(0, 1, 0.25, 1, 0.01), alpha(1), text("Dec"), 
rslider bounds(534, 188, 110, 110), channel("filterSus"), range(0, 1, .5, 1, 0.01), alpha(1), text("Sus."), 
rslider bounds(646, 188, 110, 110), channel("filterRel"), range(0, 2, 1, 1, 0.01), alpha(1), text("Rel."), 
rslider bounds(758, 188, 110, 110), channel("index"), range(0.01, 25, 0.2, 0.5, 0.01), text("Index"),  
rslider bounds(6, 0, 140, 140), channel("rvbTime"), range(0, 0.99, 0.87, 3, 0.001), text("Reverb Time"),  
rslider bounds(148, 0, 140, 140), channel("noiseFreq"), range(0, 1000, 10, 1, 0.001), text("Noise Freq"),  
rslider bounds(6, 168, 140, 140), channel("noiseBand"), range(0, 500, 10, 1, 0.001), text("Noise Band")
rslider bounds(148, 168, 140, 140), channel("noise"), range(0, 1, 0, 1, 0.001), text("Noise Amp") 
rslider bounds(8, 314, 140, 140), channel("random"), range(0, 1, 0, 1, 0.001), text("Random") 
keyboard bounds(160, 316, 719, 131), value(40)

;filebutton bounds(8, 312, 98, 27), text("Save Preset", "Save Preset"), populate("*.*", ""), colour:1(20, 20, 20), mode("file"), channel("filechooser")
;combobox bounds(106, 312, 105, 26), channel("combobox"), items("Crazy"), 


</Cabbage>
<CsoundSynthesizer>
<CsOptions>
-n -+rtmidi=null -M0 -m0d
</CsOptions>
<CsInstruments>

	sr = 44100 
	ksmps = 64
	nchnls = 2
	0dbfs = 1
	massign 1, 1
	massign 2, 2
	massign 3, 3
	massign 4, 4
	massign 5, 5
	massign 6, 6
	massign 7, 7
	massign 8, 8

	giSine		ftgen	1, 0, 2^16, 10, 1
	giFilterEnv	ftgen	2, 0, 1024, 27, 0, 0, 100, 1, 500, .5, 800, .5, 1024, 0 
	
	gkAtt init .5
	gkDec init .5
	gkSus init .5
	gkRel init .5
	gkNote init 0
	gkAmp init .9
	gkModIndex init .2 
	
	;***********************************************************
	; rescale containing plant so it fits screen
	instr 1000
	Smessage sprintf "rescale(%f, %f)", strtod($SCREEN_WIDTH)/1025, strtod($SCREEN_HEIGHT)/500
	chnset Smessage, "mainPlant"
	endin
	
	;*********************************************************************
	instr	1,2,3,4,5,6,7,8
	;this will prevent notes from getting stuck
	inote	notnum
	ivel	veloc 0, 1
	instrN  = 100 + inote*0.001
	xtratim 1/kr
	krelease release    
	event_i "i", instrN, 0, -1, inote, ivel
	                
	if krelease > 0 then
		event "i", -instrN, 0, .1, inote, ivel
	endif
	endin


    ;*********************************************************************
	instr	100
	icps = p4
	kcar chnget "car"
	kmod chnget "mod"

	kndx madsr i(gkAtt), i(gkDec), i(gkSus), i(gkRel)
	asig foscili p5*i(gkAmp), icps, kcar, kmod, kndx*(gkModIndex), 1
	kPan jspline 1, 1.1, 1.3
	aLeft = (asig*kndx)*abs(kPan)
	aRight = (asig*kndx)*(1-abs(kPan))

		outs aLeft, aRight

	chnmix aLeft, "sendL"
	chnmix aRight, "sendR"
	
	endin

	;*********************************************************************
	instr 200
	kPrevious init 0
	aL	chnget "sendL"
	aR  chnget "sendR"
	;clear channels now that we have them
	chnclear "sendL"
	chnclear "sendR"	
	
	gkAmp chnget "amp"	
	;bring down overall amplitude as index goes upwards
	gkModIndex chnget "index"	
	kTrig changed gkModIndex	
	if kTrig==1 then
		kFactor = (kPrevious<gkModIndex ? -gkModIndex/50 : gkModIndex/50) 
		chnset gkAmp+kFactor, "amp"
		kPrevious = gkModIndex
	endif
	
	kRandom chnget "random"	
	kRandomNoiseFreq jspline kRandom, 0.2, .2
	kRandomNoiseBand jspline kRandom, 0.1, .3
	
	if kRandom > 0 then
		chnset abs(kRandomNoiseFreq)*1000, "noiseFreq"
		chnset abs(kRandomNoiseBand)*500, "noiseBand"
	endif
	
	kNoiseAmp chnget "noise"
	kNoiseFreq chnget "noiseFreq"
	kNoiseBand chnget "noiseBand"

	
	kEnv linen 1, .2, p3, p3-.2
	aNoise randi kNoiseAmp, 10000
	
	aFilter butterbp aNoise, kNoiseFreq, kNoiseBand
	aFilter = aFilter*kNoiseAmp
	
	kfeed	chnget "rvbTime"
	
	aL,aR	reverbsc aL+aFilter, aR+aFilter, kfeed, 7000
	aL	dcblock2, aL
	aR	dcblock2, aR
	
		outs aL,aR
		
	endin
	
	;*********************************************************************
	instr 300
	;look after table update
	gkAtt	chnget "filterAtt"
	gkDec	chnget "filterDec"
	gkSus	chnget "filterSus"
	gkRel	chnget "filterRel"
	kchanged changed gkAtt+gkDec+gkSus+gkRel
	if(kchanged==1) then					;x  y  x                y  x                              y     x     y        x                   y
		event "f", 2, 0, 1024, 27,   0, 0, int(gkAtt*(256)), 1, int(gkAtt*(256))+int(gkDec*256), gkSus, 1024-int(gkRel*256), gkSus,  1024-int(gkRel*256)+int(gkRel*256), 0
		chnset "tablenumber(2)", "table1"
	endif
	
	endin

</CsInstruments>
<CsScore>
i1000 	0	1
i200 	1	86400
i300 	1	86400
e
</CsScore>
</CsoundSynthesizer>

