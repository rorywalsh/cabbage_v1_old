# A basic Cabbage effect
Cabbage effects are used to process incoming audio. To do this we make use of the signal input opcodes. One can use either 'ins' or 'inch'. The following code is for a simple reverb unit. We assign the incoming sample data to two variables, i.e., aInL and aInR. We then process the incoming signal through the reverbsc opcode. 

![](images/simpleReverbExample.png)

The above instrument uses 3 rsliders to control the reverb size(feedback level), the cut-off frequency, and overall gain. The range() identifier is used with each slider to specify the min, max and starting value of the sliders. 

![](images/smallLogo.PNG"]
If you compare the two score sections in the above instruments you'll notice that the synth instrument doesn't use any i-statement. Instead it uses an 'f0 3600'. This tells Csound to wait for 3600 seconds before exiting. Because the instrument is to be controlled via MIDI we don't need to use an i-statement in the score. In the second example we use an i-statement with a long duration so that the instrument runs without stopping for a long time.    
