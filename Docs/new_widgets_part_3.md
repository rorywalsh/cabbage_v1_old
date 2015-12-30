# New Widgets, Part 3

We now have a simple step sequencer that the user can click on to enable steps. They can also see which is the current step in the sequence. But it remains quite useless until we can get it to communicate with the underlying instance of Csound. Cabbage widgets communicate with Csound through the use of named channels. For example, whenever a sure moves a slider in Cabbage, the value of that slider gets sent to Csound over a named channel bus. Each widget that is capable of affecting change in a Csound instrument must have a channel associated with it. So our widget must also have an associated channel. We can retrieve the name of the channel by accessing the string passed to the channel identifier.  

```c++
class CabbageStepper	:	public Component, public Timer
{
	String name, channel;
	float numberOfSteps;
	int stepBPM, currentStep;
	Colour colour;
	Array<int> stepStates;

	public:

    CabbageStepper (CabbageGUIType &cAttr, CabbagePluginAudioProcessorEditor* _owner)
        : Component(), 
		owner(_owner),
		name(cAttr.getStringProp(CabbageIDs::name)),
		channel(cAttr.getStringProp(CabbageIDs::channel)),
        colour(Colour::fromString(cAttr.getStringProp(CabbageIDs::colour))),
		stepBPM(cAttr.getNumProp("stepbpm")),
		numberOfSteps(cAttr.getNumProp("numberofsteps")),
		currentStep(0)
    {
(...)
```

### Sending messages to Csound channels

An instance of Csound is created in the CabbagePluginAudioProcessor class, but we don't need to access it directly. We need only add some messages to the outgoing messages which are stored in CabbagePluginAudioProcesso::messageQueue. To access our main AudioProcessror class we can call the CabbagePluginAudioProcessorEditor::getFilter() method. We can then add some messages to the outgoing queue. At every k-boundary Cabbage will send the messages to Csound, this ensures complete thread safety. The best place to add messages to our queue is in the **timerCallback()** function. 

```c++
void CabbageStepper::timerCallback()
{
	currentStep = currentStep<11 ? currentStep+1 : 0;
	int valueToSendToCsound = stepStates[currentStep]==1 ? currentStep+1 : 0;
	owner->getFilter()->messageQueue.addOutgoingChannelMessageToQueue(channel, valueToSendToCsound, String::empty);
	repaint();
} 
```
**addOutgoingChannelMessageToQueue(...)** takes three parameters. The first parameter is the name of the channel to communicate with Csound on. The next parameter is the value to send, and the final parameter is the message type, which is only used for special message types. In most cases this can be set to an empty string. 

With this final method in place, Cabbage will send messages to Csound on each step. It will send a 0 if the step is disabled, otherwise it will send the step index+1. In order to use this widget with a Csound instrument we can do something like this.  

```csharp
<Cabbage>
form size(500, 100), caption("Stepper"), pluginID("plu1"), colour(57, 110, 185)
stepper bounds(10, 10, 480, 40), channel("step1"), colour("white"), numberofsteps(12), stepbpm(120)
</Cabbage>
<CsoundSynthesizer>
<CsOptions> 
-m0d
</CsOptions>
<CsInstruments>
sr = 44100
ksmps = 16
nchnls = 2
0dbfs=1

instr 1
if changed:k(chnget:k("step1"))==1 && chnget:k("step1")!=0 then
	event "i", 2, 0, 3
endif
endin

instr 2
	print p1
	a1 expon 1, p3, 0.001
	a2 oscili a1, 256
	outs a2, a2
endin

</CsInstruments>  
<CsScore>
i1 0 5000
</CsScore>
</CsoundSynthesizer>
```
Whenever channel "step1" changes, and is not equal to 0, we trigger instr 2 to play a note. 

Our new stepper widget serves as a good example of how easy it is to add new widget types to Cabbage, but it's still not quite production ready. There are still quite a few additions we could use to enhance the widget and provides users with more control. In the next part of this tutorial we'll add a few more unique identifiers and we'll see how a widgets appearance and behavior can be controlled in realtime from Csound using identifier channels. 