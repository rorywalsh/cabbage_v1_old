# Quickly creating multiple widgets
If you wish to create a large number of widgets you can use the widgetarray() identifier, which takes a base channel name, and the number of widgets to be created. Each widget channel will have an underscore and number appended to them, e.g., chan_1, chan_2. While each identchannel will have a _ident string and number appended to them, e.g., chan_ident1, chan_ident2, etc. The following code will create 100 checkboxes, all sharing the same position.

```csharp
checkbox bounds(10, 10, 50, 50), channelarray("test", 100), value(1)
```

The first checkbox widget will have a channel named "test_1", and an ident-channel named "test_ident1". The second widget channel will be named "test2", while its ident-channel will be named "test_ident2", and so on, upwards to 100. The identifiers for the newly created checkboxes can be modified using Csound code. For example, the following code will place the checkboxes around the screen in a 10x10 grid.

![](images/widgetArray1.png)

For more details on how to controls widgets from Csound code look at <<identchannels, Controlling Widgets from Csound>> 

![](images/smallLogo.PNG"]
Widget arrays don't work with plants!