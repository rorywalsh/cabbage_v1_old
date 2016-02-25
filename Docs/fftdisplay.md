# FFTDisplay

Displays a frequency representation of a signal. Must be used with the **dispfft** opcode in Csound. Instruments can only support one of these widgets. Adding a second fftdisplay widget will not work. 


```csharp
fftdisplay bounds(x, y, width, height), colour("colour"), tablebackgroundcolour("colour"), \
displaytype("type"), identchannel("channel"), alpha(val), visible(val), active(val), zoom(val)
```
<!--(End of syntax)/-->

##Identifiers

**bounds(x, y, width, height)** integer values that set position and size on screen(in pixels). 

**tablebackgroundcolour("colour")** This sets the background colour of the display. Any CSS or HTML colour string can be passed to this identifier. The colour identifier can also be passed an RBG, or RGBA value. All channel values must be between 0 and 255. For instance colour(0, 0, 255) will create a blue, while colour(0, 255, 0, 255) will create a green with an alpha channel set to full.  

**colour("colour")** Sets the colour of the FFT data drawn to screen. See above for details on valid colours.

**identchannel("channel")** Sets the channel which messages from Csound are sent to the widget on. When used with a chnset opcode users can override widget attributes. See [Identifier channels](./identchannels.md)  

**alpha(val)** A value between 0 and 1 will set the alpha blend value for the entire component. Can be useful if you need to fade widgets in and out. 

**visible(val)** A value of 0 will cause the widget to become invisible. Widgets have their visibility set to 1 by default. 

**active(val)** Will deactivate a control if 0 is passed. Controls which are deactivate can still be updated from Csound.

**displaytype("type")** Sets the the type of display. Must be 'spectrogram' or 'spectroscope'. Set to 'spectroscope by default'

**zoom(val)** Sets the initial zoom value. Passing a -1 to zoom will cause the zoom buttons to disappear.  

<!--(End of identifiers)/-->

> To enable the use of the fftdisplay widget you must pass --displays to your CsOptions. See the FFTDisplay example for details.  

##Example
```csharp
<Cabbage>
form size(520, 350), caption("FFTDisplay")
fftdisplay bounds(10, 0, 500, 300), colour("cornflowerblue"), identchannel("fft")
button bounds(12, 304, 120, 25), channel("displaytype"), text("Spectroscope", "Spectrogram")
</Cabbage>
```

![](images/fftdisplayExample.gif)