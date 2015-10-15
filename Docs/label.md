# Label
```csharp
label bounds(x, y, width, height), text("text"), colour("colour"), \
fontcolour("colour"), align("position"), identchannel("channel"), \
alpha(val), visible(val), rotate(radians, pivotx, pivoty), \
widgetarray("chan", number)
```
<!--(End of syntax)/-->
![](images/labelExample.png)

Labels can be used for placing text on-screen. 


##Identifiers

**bounds(x, y, width, height)** integer values that set position and size on screen(in pixels). 

**text("text")** "text" will be the string to appear on the label

**align("position")** Aligns the label's text. "position" should be 'left', 'right' or 'centre'. 

**colour("colour")** This sets the background colour of the label. Any CSS or HTML colour string can be passed to this identifier. The colour identifier can also be passed an RBG, or RGBA value. All channel values must be between 0 and 255. For instance colour(0, 0, 255) will create a blue, while colour(0, 255, 0, 255) will create a green with an alpha channel set to full.  

**fontcolour("colour")** This sets the colour of the image if no file name is given with the file identifier. Any CSS or HTML colour string can be passed to this identifier. The colour identifier can also be passed an RBG, or RGBA value. All channel values must be between 0 and 255. For instance colour(0, 0, 255) will create a blue, while colour(0, 255, 0, 255) will create a green with an alpha channel set to full.  

**identchannel("channel")** Sets the channel which messages from Csound are sent to the widget on. When used with a chnset opcode users can override widget attributes. See [Identifier channels](./identchannels.md)  

**alpha(val)** A value between 0 and 1 will set the alpha blend value for the entire component. Can be useful if you need to fade widgets in and out. 

**visible(val)** A value of 0 will cause the widget to become invisible. Widgets have their visibility set to 1 by default. 

**rotate(radians, pivotx, pivoty)** Rotates the widget by a number of radians(2xPI=full rotation). pivotx and pivoty will determine the rotation pivot points, where 0, 0 represents the component's top-left position. 

**widgetarray("chan", number)** Creates an number of widgets in a single pass. See [Widget arrays](./widget_arrays.md)
<!--(End of identifiers)/-->