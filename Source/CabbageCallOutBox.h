#ifndef __CABBAGE_CALLOUTBOX
#define __CABBAGE_CALLOUTBOX

#include "../JuceLibraryCode/JuceHeader.h"



class CabbageCallOutBox    : public Component
{
public:
    //==============================================================================
    CabbageCallOutBox (Component& contentComponent,
                const Rectangle<int>& areaToPointTo,
                Component* parentComponent);
    ~CabbageCallOutBox();

    void updatePosition (const Rectangle<int>& newAreaToPointTo,
                         const Rectangle<int>& newAreaToFitIn);
    static CabbageCallOutBox& launchAsynchronously (Component* contentComponent,
                                             const Rectangle<int>& areaToPointTo,
                                             Component* parentComponent);

    //==============================================================================
    void paint (Graphics& g);
    void resized();
    void moved();
    void childBoundsChanged (Component*);
    bool hitTest (int x, int y);
    void inputAttemptWhenModal();
    bool keyPressed (const KeyPress& key);
    void handleCommandMessage (int commandId);

private:
    //==============================================================================
    int borderSpace;
    float arrowSize;
    Component& content;
    Path outline;
    Point<float> targetPoint;
    Rectangle<int> availableArea, targetArea;
    Image background;

    void refreshPath();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageCallOutBox)
};


#endif   // __JUCE_CALLOUTBOX_JUCEHEADER__
