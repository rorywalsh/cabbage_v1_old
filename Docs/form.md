# Form

Form creates the main application window. pluginid() is the only required identifier. The default values for size are 600x300. 

```csharp
form caption("title"), size(Width, Height), pluginid("plug"), \
colour("colour"), guirefresh(val)
```
<!--(End of syntax)/-->
##Identifiers
**caption** The string passed to caption will be the string that appears on the main application window. 

**size(Width, Height)** integer values denoted the width and height of the form.

**pluginid("plug")** this unique string must be four characters long. It is the ID given to your plugin when loaded by plugin hosts. 

**guirefresh(val)** Sets the rate at which Cabbage will update its GUI widgets when controlled by Csound. The value passed represents the number of k-rate cycles to be skipped before the next update. The larger this is the slower the GUI updates will take place, but the less CPU intensive the instrument will be. val should be an integer greater than 1 and is set to ksmps x 2 by default. 

>For best performance one should set guirefresh to be a factor of ksmps.    

**colour("colour")** This sets the background colour of the instrument. Any CSS or HTML colour string can be passed to this identifier. The colour identifier can also be passed an RBG value. All channel values must be between 0 and 255. For instance colour(0, 0, 255) will create blue. RGBA values are not permitted when setting colours for your main form. If an RGBA value is set, Cabbage will convert it to RGB. The default colour for form is rgb(5, 15, 20). 

<!--(End of identifiers)/-->

>Every plugin must have a unique plugin ID. If you experience problems loading two different plugins, it could be because they use the same plugin ID. The plugin ID seems to be more relevant when working with OSX than on Linux or Windows.  

##Example
```csharp
<Cabbage>
form size(400, 500), caption("Untitled"), pluginID("plu1"), colour(39, 40, 34)
</Cabbage>
```

![](images/formExample.png)