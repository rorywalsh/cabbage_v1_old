; ModeFilters.csd
; Written by Iain McCurdy, 2015.

; Bank of 10 parallel mode filters
; Frequency, power (dB) and Q can be set globally by setting the global numberbox and then clicking its 'Set' button.

<Cabbage>
form caption("Mode Filters"), size(1000,260), colour(230,230,230), pluginID("MoFi")

checkbox  bounds(  0,  0,  0,  0), colour("Lime"), text("On/Off"),   fontcolour("black"), shape("square"),            widgetarray("OnOff",10)

numberbox bounds( 10, 45, 75, 30), colour("White"), text("Freq.1"),  fontcolour("black"), textcolour("black"), range(20,20000,100,1,0.01), channel("Freq1")
numberbox bounds( 90, 45, 75, 30), colour("White"), text("Freq.2"),  fontcolour("black"), textcolour("black"), range(20,20000,200,1,0.01), channel("Freq2")
numberbox bounds(170, 45, 75, 30), colour("White"), text("Freq.3"),  fontcolour("black"), textcolour("black"), range(20,20000,300,1,0.01), channel("Freq3")
numberbox bounds(250, 45, 75, 30), colour("White"), text("Freq.4"),  fontcolour("black"), textcolour("black"), range(20,20000,400,1,0.01), channel("Freq4")
numberbox bounds(330, 45, 75, 30), colour("White"), text("Freq.5"),  fontcolour("black"), textcolour("black"), range(20,20000,500,1,0.01), channel("Freq5")
numberbox bounds(410, 45, 75, 30), colour("White"), text("Freq.6"),  fontcolour("black"), textcolour("black"), range(20,20000,600,1,0.01), channel("Freq6")
numberbox bounds(490, 45, 75, 30), colour("White"), text("Freq.7"),  fontcolour("black"), textcolour("black"), range(20,20000,700,1,0.01), channel("Freq7")
numberbox bounds(570, 45, 75, 30), colour("White"), text("Freq.8"),  fontcolour("black"), textcolour("black"), range(20,20000,800,1,0.01), channel("Freq8")
numberbox bounds(650, 45, 75, 30), colour("White"), text("Freq.9"),  fontcolour("black"), textcolour("black"), range(20,20000,900,1,0.01), channel("Freq9")
numberbox bounds(730, 45, 75, 30), colour("White"), text("Freq.10"), fontcolour("black"), textcolour("black"), range(20,20000,1000,1,0.01), channel("Freq10")

numberbox bounds(835, 45, 75, 30), colour("White"), text("Freq.Glob."), fontcolour("black"), textcolour("black"), range(20,20000,1000,1,0.01), channel("FreqGlob")
button    bounds(920, 59, 40, 17), text("Set","Set"), channel("FreqSet")

numberbox bounds( 10, 85, 75, 30), colour("White"), text("DB.1"),  fontcolour("black"), textcolour("black"), range(-120,0,-20,1,0.01), channel("db1")
numberbox bounds( 90, 85, 75, 30), colour("White"), text("DB.2"),  fontcolour("black"), textcolour("black"), range(-120,0,-20,1,0.01), channel("db2")
numberbox bounds(170, 85, 75, 30), colour("White"), text("DB.3"),  fontcolour("black"), textcolour("black"), range(-120,0,-20,1,0.01), channel("db3")
numberbox bounds(250, 85, 75, 30), colour("White"), text("DB.4"),  fontcolour("black"), textcolour("black"), range(-120,0,-20,1,0.01), channel("db4")
numberbox bounds(330, 85, 75, 30), colour("White"), text("DB.5"),  fontcolour("black"), textcolour("black"), range(-120,0,-20,1,0.01), channel("db5")
numberbox bounds(410, 85, 75, 30), colour("White"), text("DB.6"),  fontcolour("black"), textcolour("black"), range(-120,0,-20,1,0.01), channel("db6")
numberbox bounds(490, 85, 75, 30), colour("White"), text("DB.7"),  fontcolour("black"), textcolour("black"), range(-120,0,-20,1,0.01), channel("db7")
numberbox bounds(570, 85, 75, 30), colour("White"), text("DB.8"),  fontcolour("black"), textcolour("black"), range(-120,0,-20,1,0.01), channel("db8")
numberbox bounds(650, 85, 75, 30), colour("White"), text("DB.9"),  fontcolour("black"), textcolour("black"), range(-120,0,-20,1,0.01), channel("db9")
numberbox bounds(730, 85, 75, 30), colour("White"), text("DB.10"), fontcolour("black"), textcolour("black"), range(-120,0,-20,1,0.01), channel("db10")

numberbox bounds(835, 85, 75, 30), colour("White"), text("dB.Glob."), fontcolour("black"), textcolour("black"), range(-120,0,-20,1,0.01), channel("dbGlob")
button    bounds(920, 99, 40, 17), text("Set","Set"), channel("dbSet")

numberbox bounds( 10,125, 75, 30), colour("White"), text("Q.1"),  fontcolour("black"), textcolour("black"), range(1,5000,100,1,0.01), channel("Q1")
numberbox bounds( 90,125, 75, 30), colour("White"), text("Q.2"),  fontcolour("black"), textcolour("black"), range(1,5000,100,1,0.01), channel("Q2")
numberbox bounds(170,125, 75, 30), colour("White"), text("Q.3"),  fontcolour("black"), textcolour("black"), range(1,5000,100,1,0.01), channel("Q3")
numberbox bounds(250,125, 75, 30), colour("White"), text("Q.4"),  fontcolour("black"), textcolour("black"), range(1,5000,100,1,0.01), channel("Q4")
numberbox bounds(330,125, 75, 30), colour("White"), text("Q.5"),  fontcolour("black"), textcolour("black"), range(1,5000,100,1,0.01), channel("Q5")
numberbox bounds(410,125, 75, 30), colour("White"), text("Q.6"),  fontcolour("black"), textcolour("black"), range(1,5000,100,1,0.01), channel("Q6")
numberbox bounds(490,125, 75, 30), colour("White"), text("Q.7"),  fontcolour("black"), textcolour("black"), range(1,5000,100,1,0.01), channel("Q7")
numberbox bounds(570,125, 75, 30), colour("White"), text("Q.8"),  fontcolour("black"), textcolour("black"), range(1,5000,100,1,0.01), channel("Q8")
numberbox bounds(650,125, 75, 30), colour("White"), text("Q.9"),  fontcolour("black"), textcolour("black"), range(1,5000,100,1,0.01), channel("Q9")
numberbox bounds(730,125, 75, 30), colour("White"), text("Q.10"), fontcolour("black"), textcolour("black"), range(1,5000,100,1,0.01), channel("Q10")

numberbox bounds(835,125, 75, 30), colour("White"), text("Q.Glob."), fontcolour("black"), textcolour("black"), range(1,5000,100,1,0.01), channel("QGlob")
button    bounds(920,139, 40, 17), text("Set","Set"), channel("QSet")

line bounds(820,10,2,150)

rslider bounds( 10,175, 70, 70), colour("DarkSlateGrey"), text("Scale Freq."), fontcolour("black"), textcolour("black"), range(0.0625,8,1,0.5,0.001), channel("ScaleFreq")
rslider bounds( 90,175, 70, 70), colour("DarkSlateGrey"), text("Scale Q"),     fontcolour("black"), textcolour("black"), range(0.01,16,1,0.5,0.001), channel("ScaleQ")
rslider bounds(170,175, 70, 70), colour("DarkSlateGrey"), text("Mix"),         fontcolour("black"), textcolour("black"), range(0,1,1,0.5,0.001), channel("Mix")
rslider bounds(250,175, 70, 70), colour("DarkSlateGrey"), text("Level"),       fontcolour("black"), textcolour("black"), range(0,1,0.5,0.5,0.001), channel("Level")

</Cabbage>

<CsoundSynthesizer>

<CsOptions>
-n -dm0
</CsOptions>

<CsInstruments>

sr = 44100
ksmps = 32
nchnls = 2
0dbfs = 1
seed	0

instr	1
 kPortTime	linseg	0,0.001,0.05
 kScaleFreq	chnget	"ScaleFreq"
 kScaleQ	chnget	"ScaleQ"
 kMix		chnget	"Mix"
 kLevel		chnget	"Level"
 kScaleFreq	portk	kScaleFreq,kPortTime
 kScaleQ	portk	kScaleQ,kPortTime
 kMix		portk	kMix,kPortTime
 kLevel		portk	kLevel,kPortTime


 
 iCount	=	1
 loop1:

 Smsg	sprintf	"bounds(%d,20,80,15)",10+(iCount-1)*80		; On/Off buttons
 Sident	sprintf	"OnOff_ident%d",iCount
 	chnset	Smsg,	Sident

 Smsg	sprintf	"bounds(%d,60,80,40)",10+(iCount-1)*80		; Frequency numberboxes
 Sident	sprintf	"Freq_ident%d",iCount
 	chnset	Smsg,	Sident
 	
 	loop_le	iCount,1,10,loop1



; aInL	pinkish	0.1
; aInR	=	aInL

 aInL,aInR	ins

 aOutL,aOutR	init	0

 #define	READ_WIDGETS(N)
 #
 kOnOff$N	chnget	"OnOff$N"
 kFreq$N	chnget	"Freq$N"
 kdb$N		chnget	"db$N"
 kQ$N		chnget	"Q$N"
 
 if kOnOff$N==1 then
  aModeL		mode	aInL,kFreq$N*kScaleFreq,kQ$N*kScaleQ
  aModeL		*=	ampdb(kdb$N)
  aOutL		+=	aModeL
  
  aModeR		mode	aInR,kFreq$N*kScaleFreq,kQ$N*kScaleQ
  aModeR		*=	ampdb(kdb$N)
  aOutR		+=	aModeR
 endif
 

 #
 $READ_WIDGETS(1)
 $READ_WIDGETS(2)
 $READ_WIDGETS(3)
 $READ_WIDGETS(4)
 $READ_WIDGETS(5)
 $READ_WIDGETS(6)
 $READ_WIDGETS(7)
 $READ_WIDGETS(8)
 $READ_WIDGETS(9)
 $READ_WIDGETS(10)

 #define GLOBAL_SET(LABEL)
 #
 k$LABEL.Glob	chnget	"$LABEL.Glob"
 k$LABEL.Set	chnget	"$LABEL.Set"
 if changed(k$LABEL.Set)==1 then
  	chnset	k$LABEL.Glob,"$LABEL.1"
  	chnset	k$LABEL.Glob,"$LABEL.2"
  	chnset	k$LABEL.Glob,"$LABEL.3"
  	chnset	k$LABEL.Glob,"$LABEL.4"
  	chnset	k$LABEL.Glob,"$LABEL.5"
  	chnset	k$LABEL.Glob,"$LABEL.6"
  	chnset	k$LABEL.Glob,"$LABEL.7"
  	chnset	k$LABEL.Glob,"$LABEL.8"
  	chnset	k$LABEL.Glob,"$LABEL.9"
  	chnset	k$LABEL.Glob,"$LABEL.10"
 endif
 #
 $GLOBAL_SET(Freq)
 $GLOBAL_SET(db)
 $GLOBAL_SET(Q)
 
 aOutL	ntrpol	aInL,aOutL,kMix
 aOutR	ntrpol	aInR,aOutR,kMix

 	outs	aOutL*kLevel,aOutR*kLevel
 	clear	aOutL,aOutR
 
endin


</CsInstruments>  

<CsScore>
i 1 0 [3600*24*7]
</CsScore>

</CsoundSynthesizer>
