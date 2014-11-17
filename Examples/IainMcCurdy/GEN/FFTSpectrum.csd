; Spectrum.csd
; written by Iain McCurdy 2014
; stereo FFT spectrum display
; left channel - yellow
; right channel - green

<Cabbage>
form caption("FFT Spectrum"), size(410, 140), colour( 50, 50, 50),pluginID("spec")                                                                                  
table   bounds(  0,  0, 410,100), tableNumbers(1,2), tablecolour("yellow","green"), identchannel("ampFFT"), amprange(0,1)
hslider bounds(  5,105,400, 30), channel("gain"), text("Input Gain"), textBox(1), range(0.1,50.00, 4, 0.5)
</Cabbage>                                                   

<CsoundSynthesizer>                                                                                                 

<CsOptions>                                                     
-dm0 -n
</CsOptions>
                                  
<CsInstruments>

sr 	= 	44100
ksmps 	= 	32
nchnls 	= 	2
0dbfs	=	1


giampFFT_L	ftgen	1,0,1024,-2,0
giampFFT_R	ftgen	2,0,1024,-2,0

instr	1
 /* INPUT */
 ainL,ainR	ins

 /* FFT ATTRIBUTES */
 ifftsize	=	2048
 ioverlap	=	ifftsize/4
 iwinsize	=	ifftsize
 iwintype	=	1

 kgain	chnget	"gain"
 fsigL	pvsanal	ainL*kgain, ifftsize, ioverlap, iwinsize, iwintype
 fsigR	pvsanal	ainR*kgain, ifftsize, ioverlap, iwinsize, iwintype
 if	metro(10)==1 then
  kflagL	pvsftw fsigL, giampFFT_L
  kflagR	pvsftw fsigR, giampFFT_R
 endif
 if kflagL==1||kflagR==1 then
 	 chnset	"tablenumber(1,2)", "ampFFT"
 endif
endin                                                                                                                     


</CsInstruments>

<CsScore>
i 1 0 [3600*24*7]
e
</CsScore>                            

</CsoundSynthesizer>
