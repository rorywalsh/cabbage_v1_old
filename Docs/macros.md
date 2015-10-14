# Macros
Cabbage specific macros can be used to define a range of difference identifiers, a provide tidy way of achieving a consistent look and feel across your instrument's widgets without having to hard code each and every one of a widget's identifiers. To create a macro one must define it using a #define keyword. The following code will create a macro called SLIDER1 that will define several attributes of a slider widget:
```csharp
#define SLIDER1 colour("red"), fontcolour("yellow"), outlinecolour("red"), range(0, 1000, 500)
```
The macro can then be used anywhere else in your Cabbage code by placing it on an appropriate line of Cabbage code. For example, the following code uses the above macro with an rslider.
```csharp
rslider bounds(39, 12, 50, 50), channel("rslider"), $SLIDER1
```
You can override any identifiers contained in a macro by placing them in front of the macro name. The following code will override the colour identifier from the macro with a new colour.
```csharp
rslider bounds(39, 12, 50, 50), channel("rslider"), colour("purple"), $SLIDER1
```

You can use as many macros as you wish in your Cabbage code. 