<Cabbage>
form size(400, 330), caption("ePad"), pluginID("epad")
#define SLIDER1 colour(143, 179, 238, 255), outlinecolour(143, 179, 238, 20), trackercolour(63, 78, 161, 255), fontcolour(255, 255, 255, 255), 
#define SLIDER2 colour(63, 78, 161, 255), outlinecolour(143, 179, 238, 20), trackercolour("white"), fontcolour(255, 255, 255, 255), 

;image bounds(12, 0, 373, 217), alpha(1), child(0), linethickness(1), colour(160, 160, 160, 50),
gentable bounds(44, 36, 162, 67), tablenumber(2), zoom(-1), amprange(0, 1, 2), tablecolour("white"), tablebackgroundcolour(20, 20, 20), tablegridcolour(160, 160, 160, 60), identchannel("table1")

;main controls
rslider bounds(240, 8, 70, 70), channel("attack"), range(0.01, 1, 0.2, .5), text("Attack") 
rslider bounds(310, 8, 70, 70), channel("release"), range(0.1, 2, 1), text("Release")
rslider bounds(240, 74, 70, 70), channel("fmindex"), range(0.1, 20, 13), text("FM Index") 
rslider bounds(310, 74, 70, 70), channel("filter"), range(1, 12, 3, .7), text("Filter")
rslider bounds(240, 140, 70, 70), channel("rvbTime"), range(0, 0.99, 0.87, 3), text("Reverb Time") 
rslider bounds(310, 140, 70, 70), channel("amp"), range(0, 1, 0.9, .5), text("Amplitude") 
;filter envelope
rslider bounds(26, 110, 50, 50), channel("filterAtt"), range(0.01, 1, 0.5, 1, 0.01), alpha(1), text("Att."), 
rslider bounds(78, 110, 50, 50), channel("filterDec"), range(0, 1, 0.25, 1, 0.01), alpha(1), text("Dec"), 
rslider bounds(130, 110, 50, 50), channel("filterSus"), range(0, 1, .5, 1, 0.01), alpha(1), text("Sus."),
rslider bounds(182, 110, 50, 50), channel("filterRel"), range(0, 2, 1, 1, 0.01), alpha(1), text("Rel."),  

keyboard bounds(18, 214, 373, 70), value(40)

label bounds(18, 8, 220, 16), text("Filter Envelope"), colour(0, 0, 0, 0)

filebutton bounds(20, 176, 98, 27), text("Save Preset", "Save Preset"), populate("*.*", ""), colour:1(20, 20, 20), mode("file"), channel("filechooser")
combobox bounds(122, 176, 105, 26), channel("combobox"), items("Ambient"), 
image bounds(18, 26, 219, 142), outlinecolour("white"), outlinethickness(2), colour(0,0,0,0)
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

;*********************************************************************
	instr	1,2,3,4,5,6,7,8
	inote	notnum
	ivel	veloc 0, 1
        instrN  = 17 + inote*0.001
                xtratim 1/kr
        krelease release    
                event_i "i", instrN, 0, -1, inote, ivel
        if krelease > 0 then
                event "i", -instrN, 0, .1, inote, ivel
        endif
	endin
    
	instr	17
	kamp	chnget "amp"
	iatck	chnget "attack"
	irel	chnget "release"
	ifmind	chnget "fmindex"
	kfilt	chnget "filter"
	
	kfilt 	tonek kfilt, 10

	inote	= p4
	icps	= cpsmidinn(inote)
	icps1	= icps * 0.999
	icps2	= icps * 1.001
	ivel	= p5
	ktimb 	expcurve ivel, 4
	indx	= (cpsmidinn(60)/icps)* ifmind
	 
	kindx madsr i(gkAtt), i(gkDec), i(gkSus), i(gkRel)
	;kindx 	linsegr indx*0.4, iatck*2.5,  indx, 1, indx*0.7, irel, 0 
	a1 	foscili ivel * 0.05, icps1, 2, 1, ktimb * (kindx*indx), giSine
	a2 	vco2 ivel * 0.3, icps2, 8, 0.5 * ktimb
	a2 	lpf18 a2, icps * kfilt, 0.85 * ivel, 2 * ivel
	a3	= (a1+a2)*0.5
	aenv 	linsegr 0, iatck, 1, irel, 0
	a3 	= a3 * aenv * kamp
		outs a3, a3
		chnmix a3, "send"
	endin


	instr 99
	a1	chnget "send"
	a0	= 0
		chnset a0, "send"
	kfeed	chnget "rvbTime"
	aL,aR	reverbsc a1, a1, kfeed, 7000
	aL	dcblock2, aL
	aR	dcblock2, aR
		outs aL,aR

	gkAtt	chnget "filterAtt"
	gkDec	chnget "filterDec"
	gkSus	chnget "filterSus"
	gkRel	chnget "filterRel"

	;add table update code here
	kchanged changed gkAtt+gkDec+gkSus+gkRel
	if(kchanged==1) then					;x  y  x                y  x                              y     x     y        x                   y
		event "f", 2, 0, 1024, 27,   0, 0, int(gkAtt*(256)), 1, int(gkAtt*(256))+int(gkDec*256), gkSus, 1024-int(gkRel*256), gkSus,  1024-int(gkRel*256)+int(gkRel*256), 0
		chnset "tablenumber(2)", "table1"
	endif
	endin

</CsInstruments>
<CsScore>
i 99 	0	86400

e
</CsScore>
</CsoundSynthesizer>

