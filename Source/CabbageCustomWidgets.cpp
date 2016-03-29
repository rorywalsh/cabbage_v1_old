//custom widgets

#include "./Plugin/CabbagePluginEditor.h"
#include "CabbageCustomWidgets.h"

//==============================================================================================
// Example stepper widget
//==============================================================================================
CabbageStepper::CabbageStepper (CabbageGUIType &cAttr, CabbagePluginAudioProcessorEditor* _owner)
    : Component(),
      owner(_owner),
      name(cAttr.getStringProp(CabbageIDs::name)),
      channel(cAttr.getStringProp(CabbageIDs::channel)),
      colour(Colour::fromString(cAttr.getStringProp(CabbageIDs::colour))),
      stepBPM(cAttr.getNumProp("stepbpm")),
      numberOfSteps(cAttr.getNumProp("numberofsteps")),
      currentStep(0),
      isRunning(cAttr.getNumProp(CabbageIDs::active)),
      isMouseDown(false)
{
    for(int i=0; i<numberOfSteps; i++)
        stepStates.add(0);
    if(isRunning==1)
        startTimer((60.f/stepBPM)*1000.f);
}

void CabbageStepper::timerCallback()
{
    currentStep = currentStep<11 ? currentStep+1 : 0;
    int valueToSendToCsound = stepStates[currentStep]==1 ? currentStep+1 : 0;
    owner->getFilter()->messageQueue.addOutgoingChannelMessageToQueue(channel, valueToSendToCsound);
    repaint();
}

void CabbageStepper::mouseDown(const MouseEvent& event)
{
    isMouseDown = true;
    int x = event.getMouseDownX();

    float stepWidth = getWidth()/numberOfSteps;
    for(int i=0; i<numberOfSteps; i++)
    {
        if(x>i*stepWidth && x<i*stepWidth+stepWidth)
            stepStates.getReference(i) = stepStates.getReference(i)==1 ? 0 : 1;

        repaint();
    }
}

void CabbageStepper::mouseEnter(const MouseEvent& event)
{
    int x = event.getMouseDownX();
    float stepWidth = getWidth()/numberOfSteps;
    if(isMouseDown)
        for(int i=0; i<numberOfSteps; i++)
        {
            if(x>i*stepWidth && x<i*stepWidth+stepWidth)
                stepStates.getReference(i) = stepStates.getReference(i)==1 ? 0 : 1;

            repaint();
        }
}

void CabbageStepper::mouseUp(const MouseEvent& event)
{
    isMouseDown = false;
}

void CabbageStepper::paint(Graphics& g)
{
    g.fillAll(Colours::transparentBlack);
    float scaleSize, scaleTop;
    float stepWidth = getWidth()/numberOfSteps;
    for(int i=0; i<numberOfSteps; i++)
    {
        g.setColour(stepStates[i]==1 ? Colours::lime : colour);
        scaleSize = currentStep==i ? .95 : .9;
        scaleTop = currentStep==i ? .025 : .05;

        g.fillRoundedRectangle(stepWidth*i, getHeight()*scaleTop, stepWidth*scaleSize, getHeight()*scaleSize, 5.f);
    }
}

void CabbageStepper::update(CabbageGUIType cAttr)
{
    if(cAttr.getNumProp("stepbpm")!=stepBPM)
    {
        stepBPM = cAttr.getNumProp("stepbpm");
        if(isRunning)
            startTimer((60.f/stepBPM)*1000.f);
    }

    if(cAttr.getNumProp(CabbageIDs::active)!=isRunning)
    {
        isRunning = cAttr.getNumProp(CabbageIDs::active);
        if(isRunning==1)
            startTimer((60.f/stepBPM)*1000.f);
        else
            stopTimer();
    }

    if(colour.toString()!=cAttr.getStringProp("colour"))
        colour = Colour::fromString(cAttr.getStringProp("colour"));
}
//==============================================================================================
// Cabbage Label widget
//==============================================================================================
CabbageLabel::CabbageLabel (CabbageGUIType &cAttr, CabbagePluginAudioProcessorEditor* _owner)
    : text(cAttr.getStringProp(CabbageIDs::text)),
      owner(_owner),
      channel(cAttr.getStringProp(CabbageIDs::channel)),
      colour(cAttr.getStringProp(CabbageIDs::colour)),
      fontcolour(cAttr.getStringProp(CabbageIDs::fontcolour)),
      align(cAttr.getStringProp(CabbageIDs::align)),
      textAlign(Justification::centred),
      rotate(cAttr.getNumProp(CabbageIDs::rotate)),
      pivotx(cAttr.getNumProp(CabbageIDs::pivotx)),
      pivoty(cAttr.getNumProp(CabbageIDs::pivoty)),
      fontstyle(cAttr.getNumProp(CabbageIDs::fontstyle)),
      corners(cAttr.getNumProp(CabbageIDs::corners)),
      counter(0)
{
    if(!cAttr.getNumProp(CabbageIDs::visible))
    {
        setVisible(false);
        //Logger::writeToLog("visivle");
    }
    else
    {
        setVisible(true);
        //Logger::writeToLog("visivle");
    }

    cUtils::debug(cAttr.getStringProp(CabbageIDs::channel));

    setText(cAttr.getStringProp(CabbageIDs::text));

    if(align=="centre")
        textAlign = Justification::centred;
    else if(align=="left")
        textAlign = Justification::left;
    else
        textAlign = Justification::right;

    setAlpha(cAttr.getNumProp(CabbageIDs::alpha));
}

CabbageLabel::~CabbageLabel()
{
}

void CabbageLabel::resized()
{
    if(rotate!=0)
        setTransform(AffineTransform::rotation(rotate, getX()+pivotx, getY()+pivoty));
}

void CabbageLabel::paint(Graphics& g)
{
    g.setColour(Colour::fromString(colour));
    g.fillRoundedRectangle(getLocalBounds().toFloat(), corners);
    g.setColour(Colour::fromString(fontcolour));
    g.setFont(cUtils::getComponentFont(fontstyle));
    g.setFont(getHeight());
    g.drawFittedText(text, 0, 0, getWidth(), getHeight(), textAlign, 1, 1);
}

void CabbageLabel::setText(String _text)
{
    text = _text;
    repaint();
}

void CabbageLabel::mouseDown(const MouseEvent& event)
{
    if(!event.mods.isPopupMenu())
    {
        counter = (counter==0 ? 1 : 0);
        owner->getFilter()->messageQueue.addOutgoingChannelMessageToQueue(channel, counter);
    }
}

//update control
void CabbageLabel::update(CabbageGUIType m_cAttr)
{
    colour = m_cAttr.getStringProp(CabbageIDs::colour);
    fontcolour = m_cAttr.getStringProp(CabbageIDs::fontcolour);
    setBounds(m_cAttr.getBounds());
    setAlpha(m_cAttr.getNumProp(CabbageIDs::alpha));
    if(rotate!=m_cAttr.getNumProp(CabbageIDs::rotate))
    {
        rotate = m_cAttr.getNumProp(CabbageIDs::rotate);
        setTransform(AffineTransform::rotation(rotate, getX()+m_cAttr.getNumProp(CabbageIDs::pivotx), getY()+m_cAttr.getNumProp(CabbageIDs::pivoty)));
    }
    if(!m_cAttr.getNumProp(CabbageIDs::visible))
    {
        setVisible(false);
        setEnabled(false);
    }
    else
    {
        setEnabled(true);
        setVisible(true);
    }
    if(!m_cAttr.getNumProp(CabbageIDs::active))
    {
        setEnabled(false);
    }

    if(fontstyle!=m_cAttr.getNumProp(CabbageIDs::fontstyle))
    {
        fontstyle = m_cAttr.getNumProp(CabbageIDs::fontstyle);
    }
    else
    {
        setEnabled(true);
    }
    setText(m_cAttr.getStringProp(CabbageIDs::text));
    repaint();
}


//==============================================================================================
// Encoder widget
//==============================================================================================
CabbageEncoder::CabbageEncoder(CabbageGUIType &cAttr, CabbagePluginAudioProcessorEditor* _owner):
    owner(_owner),
    name(cAttr.getStringProp(CabbageIDs::name)),
    colour(cAttr.getStringProp(CabbageIDs::colour)),
    trackercolour(cAttr.getStringProp(CabbageIDs::trackercolour)),
    textcolour(cAttr.getStringProp(CabbageIDs::textcolour)),
    outlinecolour(cAttr.getStringProp(CabbageIDs::outlinecolour)),
    line(cAttr.getNumProp(CabbageIDs::outlinethickness)),
    transform(AffineTransform::identity),
    rotate(cAttr.getNumProp(CabbageIDs::rotate)),
    corners(cAttr.getNumProp(CabbageIDs::corners)),
    pivotx(cAttr.getNumProp(CabbageIDs::pivotx)),
    pivoty(cAttr.getNumProp(CabbageIDs::pivoty)),
    tooltipText(String::empty),
    text(cAttr.getStringProp(CabbageIDs::text)),
    sliderincr(cAttr.getNumProp(CabbageIDs::sliderincr)),
    channel(cAttr.getStringProp(CabbageIDs::channel)),
    value(cAttr.getNumProp(CabbageIDs::value)),
    textLabel(""),
    valueLabel(""),
    isMouseOver(false),
    sliderIncr(cAttr.getNumProp(CabbageIDs::sliderincr)),
    sliderPos(0),
    yAxis(0),
    maxEnabled(cAttr.getNumProp(CabbageIDs::maxenabled)),
    minEnabled(cAttr.getNumProp(CabbageIDs::minenabled)),
    min(cAttr.getNumProp(CabbageIDs::minvalue)),
    max(cAttr.getNumProp(CabbageIDs::maxvalue)),
    shouldDisplayPopup(false),
    currentValue(value),
    startingValue(value),
    textBox(cAttr.getNumProp(CabbageIDs::textbox))
{
    //addAndMakeVisible(slider);
    //slider.addChangeListener(this);
    addAndMakeVisible(textLabel);
    slider.setSize(cAttr.getNumProp(CabbageIDs::width), cAttr.getNumProp(CabbageIDs::height));
    textLabel.setColour(Label::textColourId, Colour::fromString(textcolour));
    textLabel.setColour(Label::outlineColourId, Colours::transparentBlack);
    valueLabel.setColour(Label::textColourId, Colour::fromString(textcolour));
    valueLabel.setColour(Label::outlineColourId, Colours::whitesmoke.withAlpha(.2f));
    valueLabel.setEditable(true);
    valueLabel.addListener(this);
    if(textBox<1)
        shouldDisplayPopup=true;
    else
        addAndMakeVisible(valueLabel);
}

void CabbageEncoder::labelTextChanged (Label *label)
{
    float value = label->getText().getFloatValue();
    sliderPos=0;
    currentValue=value;
    valueLabel.setText(String(value, 2), dontSendNotification);
    owner->getFilter()->messageQueue.addOutgoingChannelMessageToQueue(channel, currentValue);
    showPopup();
}

void CabbageEncoder::mouseDown(const MouseEvent &e)
{
    if(e.getNumberOfClicks()>1)
    {
        sliderPos=0;
        currentValue=startingValue;
        repaint();
        owner->getFilter()->messageQueue.addOutgoingChannelMessageToQueue(channel, currentValue);
        showPopup();
    }
}

void CabbageEncoder::mouseEnter(const MouseEvent &e)
{
    isMouseOver = true;
    repaint();
    showPopup();
}

void CabbageEncoder::mouseDrag(const MouseEvent& e)
{
    if(yAxis!=e.getOffsetFromDragStart().getY())
    {
        sliderPos = sliderPos+(e.getOffsetFromDragStart().getY()<yAxis ? -50 : 50);
        currentValue = cUtils::roundToPrec(currentValue+(e.getOffsetFromDragStart().getY()<yAxis ? sliderIncr : -sliderIncr), 5);
        if (minEnabled==1)
            currentValue = jmax(min, currentValue);
        if (maxEnabled==1)
            currentValue = jmin(max, currentValue);

        yAxis = e.getOffsetFromDragStart().getY();
        repaint();
        valueLabel.setText(String(currentValue, 2), dontSendNotification);
        owner->getFilter()->messageQueue.addOutgoingChannelMessageToQueue(channel, currentValue);
        showPopup();
    }
}

void CabbageEncoder::showPopup()
{
    if(shouldDisplayPopup)
    {
        String popupText;
        if(tooltipText.isNotEmpty())
            popupText = tooltipText;
        else
            popupText = channel+": "+String(currentValue);

        owner->showBubble(this, popupText);
    }
}

void CabbageEncoder::mouseExit(const MouseEvent &e)
{
    isMouseOver = false;
    repaint();
}

void CabbageEncoder::paint(Graphics &g)
{
    float rotaryStartAngle = 0;
    float rotaryEndAngle = 2*3.14;
    bool isEnabled=true;
    const float radius = jmin (slider.getWidth() / 2, slider.getHeight() / 2) - 2.0f;
    const float diameter = radius*2.f;
    const float centreX = getWidth() * 0.5f;
    const float centreY = slider.getY()+slider.getHeight() * 0.5f;
    const float rx = centreX - radius;
    const float ry = centreY - radius;
    const float rw = radius * 2.0f;
    const float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

    if (radius > 12.0f)
    {
        g.setColour (Colour::fromString(trackercolour).withAlpha (isMouseOver ? 1.0f : 0.9f));

        const float thickness = 0.7f;
        {
            Path filledArc;
            filledArc.addPieSegment (rx, ry, rw, rw, angle-.25, angle+.25f, thickness);
            g.fillPath (filledArc);
        }

        g.setColour (Colour::fromString(outlinecolour).withAlpha (isMouseOver ? 1.0f : 0.7f));
        g.drawEllipse(slider.reduced(1.f).toFloat(), isMouseOver ? 1.5f : 1.f);
        g.drawEllipse(slider.reduced(getWidth()*.11).toFloat(), isMouseOver ? 1.5f : 1.f);

        Path newPolygon;
        Point<float> centre (centreX, centreY);

        if (diameter >= 25)   //If diameter is >= 40 then polygon has 12 steps
        {
            newPolygon.addPolygon(centre, 12.f, radius*.65, 0.f);
            newPolygon.applyTransform (AffineTransform::rotation (angle,
                                       centreX, centreY));
        }
        else //Else just use a circle. This is clearer than a polygon when very small.
            newPolygon.addEllipse (-radius*.2, -radius*.2, radius * .3f, radius * .3f);


        g.setColour (Colour::fromString(colour));

        Colour thumbColour = Colour::fromString(colour).withAlpha (isMouseOver ? 1.0f : 0.9f);
        ColourGradient cg = ColourGradient (Colours::white, 0, 0, thumbColour, diameter*0.6, diameter*0.4, false);
        //if(slider.findColour (Slider::thumbColourId)!=Colour(0.f,0.f,0.f,0.f))
        g.setGradientFill (cg);
        g.fillPath (newPolygon);
    }
    else
    {
        Path p;
        g.setColour (Colour::fromString(colour).withAlpha (isMouseOver ? 1.0f : 0.7f));
        p.addEllipse (-0.4f * rw, -0.4f * rw, rw * 0.8f, rw * 0.8f);
        g.fillPath(p, AffineTransform::rotation (angle).translated (centreX, centreY));

        //if (slider.isEnabled())
        g.setColour (Colour::fromString(trackercolour).withAlpha (isMouseOver ? 0.7f : 0.5f));
        //else
        //    g.setColour (Colour (0x80808080));

        p.addEllipse (-0.4f * rw, -0.4f * rw, rw * 0.8f, rw * 0.8f);
        PathStrokeType (rw * 0.1f).createStrokedPath (p, p);

        p.addLineSegment (Line<float> (0.0f, 0.0f, 0.0f, -radius), rw * 0.1f);

        g.fillPath (p, AffineTransform::rotation (angle).translated (centreX, centreY));

    }
}

void CabbageEncoder::resized()
{
    if(text.isNotEmpty())
    {
        if(textBox>0)
        {
            textLabel.setBounds(0, 0, getWidth(), 20);
            textLabel.setText(text, dontSendNotification);
            textLabel.setJustificationType(Justification::centred);
            textLabel.setVisible(true);
            slider.setBounds(20, 20, getWidth()-40, getHeight()-40);
            valueLabel.setBounds(25, getHeight()-15, getWidth()-50, 15);
            valueLabel.setJustificationType(Justification::centred);
            valueLabel.setText(String(currentValue, 2), dontSendNotification);
        }
        else
        {
            textLabel.setBounds(0, getHeight()-20, getWidth(), 20);
            textLabel.setText(text, dontSendNotification);
            textLabel.setJustificationType(Justification::centred);
            textLabel.setVisible(true);
            slider.setBounds(10, 0, getWidth()-20, getHeight()-20);
        }
    }
    else
        slider.setBounds(0, 0, getWidth(), getHeight());
}

void CabbageEncoder::update(CabbageGUIType m_cAttr)
{
    const MessageManagerLock mmLock;
    colour = m_cAttr.getStringProp(CabbageIDs::colour);
    textcolour = m_cAttr.getStringProp(CabbageIDs::fontcolour);
    trackercolour = m_cAttr.getStringProp(CabbageIDs::trackercolour);
    outlinecolour = m_cAttr.getStringProp(CabbageIDs::outlinecolour);
    setBounds(m_cAttr.getBounds());
    textLabel.setColour(Label::textColourId, Colour::fromString(m_cAttr.getStringProp(CabbageIDs::textcolour)));

    if(!m_cAttr.getNumProp(CabbageIDs::visible))
    {
        setVisible(false);
        setEnabled(false);
    }
    else
    {
        setVisible(true);
        setEnabled(true);
    }
    if(!m_cAttr.getNumProp(CabbageIDs::active))
    {
        setEnabled(false);
    }
    else
    {
        setEnabled(true);
    }
    if(rotate!=m_cAttr.getNumProp(CabbageIDs::rotate))
    {
        rotate = m_cAttr.getNumProp(CabbageIDs::rotate);
        setTransform(AffineTransform::rotation(rotate, getX()+m_cAttr.getNumProp(CabbageIDs::pivotx), getY()+m_cAttr.getNumProp(CabbageIDs::pivoty)));
    }
    if(tooltipText!=m_cAttr.getStringProp(CabbageIDs::popuptext))
    {
        tooltipText = m_cAttr.getStringProp(CabbageIDs::popuptext);
    }

    setAlpha(m_cAttr.getNumProp(CabbageIDs::alpha));
    repaint();
}

//==============================================================================================
// RangeSlider2, homegrown version of JUCE's two value slider, with dragable range
//==============================================================================================
RangeSlider::RangeSlider(CabbageGUIType &cAttr, CabbagePluginAudioProcessorEditor* _owner)
    : Component(),
      owner(_owner),
      name(cAttr.getStringProp(CabbageIDs::name)),
      isVertical(cAttr.getStringProp(CabbageIDs::kind)=="horizontal" ? false : true),
      sliderThumb(this, cAttr.getStringProp(CabbageIDs::kind)),
      currentThumb(0),
      thumbWidth(10),
      thumbHeight(10),
      tooltipText(String::empty),
      shouldDisplayPopup(true),
      initVal(cAttr.getNumProp(CabbageIDs::value)),
      max(cAttr.getNumProp(CabbageIDs::max)),
      min(cAttr.getNumProp(CabbageIDs::min)),
      maxVal(cAttr.getNumProp(CabbageIDs::maxvalue)),
      minVal(cAttr.getNumProp(CabbageIDs::minvalue)),
      incr(cAttr.getNumProp(CabbageIDs::sliderincr)),
      width(cAttr.getNumProp(CabbageIDs::width)),
      height(cAttr.getNumProp(CabbageIDs::height)),
      skew(cAttr.getNumProp(CabbageIDs::sliderskew)),
      colour(cAttr.getStringProp(CabbageIDs::colour)),
      trackerThickness(cAttr.getNumProp(CabbageIDs::trackerthickness)),
      trackerColour(Colour::fromString(cAttr.getStringProp(CabbageIDs::trackercolour))),
      index(-99)
{
    if(cAttr.getStringArrayProp(CabbageIDs::channel).size()>1)
    {
        channels.add(cAttr.getStringArrayPropValue(CabbageIDs::channel, 0));
        channels.add(cAttr.getStringArrayPropValue(CabbageIDs::channel, 1));
    }

    sliderThumb.setColour(Colour::fromString(colour));

#ifdef Cabbage_Build_Standalone
    sendValuesToCsound(minVal, maxVal, 3);
#endif
    addAndMakeVisible(&sliderThumb);

}

void RangeSlider::resized()
{
    thumbIncr = (incr/(max-min))*getWidth();
    sliderThumb.setBounds(0, 0, getWidth(), getHeight());
    sliderThumbBounds = getLocalBounds();
}

int RangeSlider::getSliderPosition(SliderType type)
{
    if(type == SliderType::left)
    {
        const double proportion = abs((minVal- (minVal<0 ? min : 0))/(max-min));
        return (getWidth()*proportion)- (minVal<0 ? thumbWidth : 0);
    }
    else if(type == SliderType::right)
    {
        const double proportion = abs((maxVal-minVal)/(max-min));
        int newWidth = getWidth()*proportion;
        return newWidth + (minVal < 0 ? thumbWidth : 0);
    }
    else if(type == SliderType::top)
    {
        const double proportion = abs((maxVal-max)/(max-min));
        return (getHeight()*proportion);
    }
    else if(type == SliderType::bottom)
    {
        const double newWidth = abs((maxVal-minVal)/(max-min));
        return (getHeight()*newWidth)+ (minVal<0 ? thumbHeight : 0);
    }
}

void RangeSlider::mouseDown(const MouseEvent& event)
{
    if(getThumbOuterBounds(1).contains(event.getMouseDownPosition()))
        currentThumb = 1;
    else if(getThumbOuterBounds(2).contains(event.getMouseDownPosition()))
        currentThumb = 2;
    else if(getThumbOuterBounds(3).contains(event.getMouseDownPosition()))
        currentThumb = 3;

    sliderThumbBounds = sliderThumb.getBounds();
    showPopup();
}

void RangeSlider::mouseMove(const MouseEvent& event)
{
    showPopup();
}

Rectangle<int> RangeSlider::getThumbOuterBounds(int type)
{
    if(!isVertical)
    {
        if(type==1)
            return Rectangle<int>(sliderThumb.getPosition().getX(), 0, thumbWidth, getHeight());
        else if(type==2)
            return Rectangle<int>(sliderThumb.getPosition().getX()+sliderThumb.getWidth()-thumbWidth, 0, thumbWidth, getHeight());
        else if(type==3)
            return Rectangle<int>(sliderThumb.getPosition().getX()+thumbWidth, 0, sliderThumb.getWidth()-thumbWidth, getHeight());
    }
    else
    {
        if(type==1)
            return Rectangle<int>(0, sliderThumb.getPosition().getY(), getWidth(), thumbHeight);
        else if(type==2)
            return Rectangle<int>(0, sliderThumb.getPosition().getY()+sliderThumb.getHeight()-thumbHeight, getWidth(), thumbHeight);
        else if(type==3)
            return Rectangle<int>(0, sliderThumb.getPosition().getY()-thumbHeight, getWidth(), sliderThumb.getPosition().getY()+sliderThumb.getHeight());
    }
}

void RangeSlider::mouseUp(const MouseEvent& event)
{
    sliderThumbBounds = sliderThumb.getBounds();
    currentThumb=0;
}

void RangeSlider::mouseDrag(const MouseEvent& event)
{
    if(isVertical == false)
        horizontalDrag(event);
    else
        verticalDrag(event);

    showPopup();
}

void RangeSlider::verticalDrag(const MouseEvent& event)
{
    if(currentThumb == 1)
    {
        const int bottomThumb = (sliderThumbBounds.getY()+sliderThumbBounds.getHeight())-thumbHeight*2;
        const int yPos = cUtils::roundToMultiple(jmin(bottomThumb, sliderThumbBounds.getY()+event.getDistanceFromDragStartY()), thumbIncr);
        sliderThumb.setBounds(sliderThumbBounds.withTop(yPos<bottomThumb ? jmax(0, yPos) : bottomThumb));
    }
    else if(currentThumb == 2)
    {
        const int yPos = jlimit(sliderThumbBounds.getY()+thumbHeight*2, getHeight(), sliderThumbBounds.getY()+sliderThumbBounds.getHeight()+event.getDistanceFromDragStartY());
        sliderThumb.setBounds(sliderThumbBounds.withBottom(cUtils::roundToMultiple(yPos, thumbIncr)));
    }
    else if(currentThumb == 3)
    {
        int yPos = sliderThumbBounds.getPosition().getY()+event.getDistanceFromDragStartY();
        yPos = cUtils::roundToMultiple(yPos+sliderThumb.getHeight() < getHeight() ? yPos : getHeight()-sliderThumb.getHeight(), thumbIncr);
        sliderThumb.setBounds(0, jmax(0, yPos), getWidth(), sliderThumbBounds.getHeight());
    }

    if(currentThumb!=0)
    {
        const double scaledHeight = (getHeight()-(2*thumbHeight))*.99;//scale down to prevent errors at different sizes...
        double proportion1 = (double)sliderThumb.getPosition().getY()/scaledHeight;
        double proportion2 = ((double)sliderThumb.getPosition().getY()+(double)sliderThumb.getHeight()-thumbHeight*2)/scaledHeight;
        minVal = cUtils::roundToMultiple(getSkewedValue(proportion1), incr);
        maxVal = cUtils::roundToMultiple(getSkewedValue(proportion2), incr);
        sendValuesToCsound(minVal, maxVal, 3);
    }
}

void RangeSlider::horizontalDrag(const MouseEvent& event)
{
    if(currentThumb == 1)
    {
        const int rightThumb = jlimit(0, (int)width, (sliderThumbBounds.getX()+sliderThumbBounds.getWidth())-thumbWidth*2);
        const int xPos = cUtils::roundToMultiple(jmin(rightThumb, sliderThumbBounds.getX()+event.getDistanceFromDragStartX()), thumbIncr);
        sliderThumb.setBounds(sliderThumbBounds.withLeft(xPos<rightThumb ? jmax(0, xPos) : rightThumb));
    }
    else if(currentThumb == 2)
    {
        const int xPos = jlimit(sliderThumbBounds.getX()+thumbWidth*2, getWidth(), sliderThumbBounds.getX()+sliderThumbBounds.getWidth()+event.getDistanceFromDragStartX());
        sliderThumb.setBounds(sliderThumbBounds.withRight(cUtils::roundToMultiple(xPos, thumbIncr)));
    }
    else if(currentThumb == 3)
    {
        int xPos = sliderThumbBounds.getPosition().getX()+event.getDistanceFromDragStartX();
        xPos = cUtils::roundToMultiple(xPos+sliderThumb.getWidth() < getWidth() ? xPos : getWidth()-sliderThumb.getWidth(), thumbIncr);
        sliderThumb.setBounds(jmax(0, xPos), 0, jmax(thumbWidth*2, sliderThumbBounds.getWidth()), getHeight());
    }

    if(currentThumb!=0)
    {
        const double scaledWidth = (getWidth()-(2*thumbWidth))*.99;//scale down to prevent errors at different sizes...
        const double proportion1 = (double)sliderThumb.getPosition().getX()/scaledWidth;
        const double proportion2 = ((double)sliderThumb.getPosition().getX()+(double)sliderThumb.getWidth()-thumbWidth*2)/scaledWidth;
        minVal = cUtils::roundToMultiple(getSkewedValue(proportion1), incr);
        maxVal = cUtils::roundToMultiple(getSkewedValue(proportion2), incr);
        sendValuesToCsound(minVal, maxVal, currentThumb);
    }

    cUtils::getBoundsString(sliderThumb.getBounds());

}

void RangeSlider::setValue(float val1, float val2)
{
    //val1 and val2 are scaled between 0 and 1

    if(currentThumb==0)//only update size if the host is controlling the slider
    {
        if(!isVertical)
        {
            int leftPos = getSkewedPosition(val1);
            int rightPos = getSkewedPosition(val2);
            sliderThumb.setBounds(sliderThumbBounds.withLeft(jmax(0, leftPos)));
            sliderThumbBounds = sliderThumb.getBounds();
            sliderThumb.setBounds(sliderThumbBounds.withRight(jlimit(leftPos+thumbWidth, (int)width, rightPos)));
            sliderThumbBounds = sliderThumb.getBounds();
            sliderThumb.setBounds(sliderThumbBounds.withWidth(jlimit(thumbWidth*2, (int)width, sliderThumbBounds.getWidth())));
            sliderThumbBounds = sliderThumb.getBounds();
            if(val1==val2)
                sliderThumb.setCentrePosition(jlimit(thumbWidth, (int)width-thumbWidth, sliderThumbBounds.getCentre().getX()-thumbWidth), sliderThumbBounds.getCentre().getY());

            repaint();
        }
        else
        {
            int topPos = getSkewedPosition(val2);
            int bottomPos = getSkewedPosition(val1);
            sliderThumb.setBounds(sliderThumbBounds.withTop(jmax(0, (int)height-topPos)));
            sliderThumbBounds = sliderThumb.getBounds();
            sliderThumb.setBounds(sliderThumbBounds.withBottom(jmin((int)height, (int)height-bottomPos)));
            sliderThumbBounds = sliderThumb.getBounds();
            sliderThumb.setBounds(sliderThumbBounds.withHeight(jlimit(thumbHeight*2, (int)height, sliderThumbBounds.getHeight())));
            sliderThumbBounds = sliderThumb.getBounds();
            if(val1==val2)
                sliderThumb.setCentrePosition(sliderThumbBounds.getCentre().getX(), jlimit(thumbHeight, (int)height-thumbHeight, sliderThumbBounds.getCentre().getY()-thumbHeight));
            repaint();
        }

    }

}

int RangeSlider::getSkewedPosition(double proportion)
{
    if(isVertical)
    {
        const double skewedPos = pow (1-proportion, skew);
        return skew==1 ? proportion*height : skewedPos*height;
    }
    else
    {
        const double skewedPos = pow (proportion, skew);
        return skew==1 ? proportion*width : skewedPos*width;
    }
}

double RangeSlider::getSkewedValue(double proportion)
{
    if(isVertical)
    {
        const double skewedValue = pow (1-proportion, 1/skew)*(max-min)+min;
        return skew == 1.0 ? jlimit(min, max, (1-proportion)*(max-min)+min) : jlimit(min, max, skewedValue);
    }
    else
    {
        const double skewedValue = pow (proportion, 1/skew)*(max-min)+min;
        return skew == 1.0 ? jlimit(min, max, proportion*(max-min)+min) : jlimit(min, max, skewedValue);
    }
}

void RangeSlider::sendValuesToCsound(double val1, double val2, int thumb)
{

    if(owner->getFilter()->csoundCompiledOk()==OK)
    {
#ifdef Cabbage_Build_Standalone
        if(thumb==1)
            owner->getFilter()->setParameterNotifyingHost(index, (float)(val1));
        else if(thumb==2)
            owner->getFilter()->setParameterNotifyingHost(index+1, (float)(val2));
        else
        {
            owner->getFilter()->setParameterNotifyingHost(index, (float)(val1));
            owner->getFilter()->setParameterNotifyingHost(index+1, (float)(val2));
        }

#else
        const float value1 = jmap(val1, min, max, 0.0, 1.0);
        const float value2 = jmap(val2, min, max, 0.0, 1.0);

        if(thumb==1)
            owner->getFilter()->setParameterNotifyingHost(index, value1);
        else if(thumb==2)
            owner->getFilter()->setParameterNotifyingHost(index+1, value2);
        else
        {
            owner->getFilter()->setParameterNotifyingHost(index, value1);
            owner->getFilter()->setParameterNotifyingHost(index+1, value2);
        }

#endif
    }

}

void RangeSlider::mouseEnter(const MouseEvent& event)
{
    showPopup();
}

void RangeSlider::paint(Graphics& g)
{
    //the juce native sliders seem to have their width reduced by 18 for som reason or another...


    int sliderThumbWidth=7;

    g.fillAll(Colours::transparentBlack);
    if(!isVertical)
    {
        const int height = getHeight();
        const int width = getWidth()-18;
        g.setColour (Colours::whitesmoke);
        g.setOpacity (0.6);
        const float midPoint = width/2.f+sliderThumbWidth;
        const float markerGap = width/9.f;
        g.drawLine (midPoint, height*0.25, midPoint, height*0.75, 1.5);
        g.setOpacity (0.3);
        for (int i=1; i<5; i++)
        {
            g.drawLine (midPoint+markerGap*i, height*0.3, midPoint+markerGap*i, height*0.7, .7);
            g.drawLine (midPoint-markerGap*i, height*0.3, midPoint-markerGap*i, height*0.7, .7);
        }
        //backgrounds
        g.setColour (Colours::whitesmoke);
        g.setOpacity (0.1);
        g.fillRoundedRectangle (sliderThumbWidth/2.f, height*0.44, width*1.025, height*0.15, height*0.05); //for light effect
        g.setColour (Colour::fromRGBA(5, 5, 5, 255));
        g.fillRoundedRectangle (sliderThumbWidth/2.f, height*0.425, width*1.02, height*0.15, height*0.05); //main rectangle
        //tracker
        g.setColour(trackerColour);
        const float scale = trackerThickness;
        const int iHeight = (height * scale);
        const int iTop = ((height-iHeight)/2.f);
        g.fillRect(sliderThumb.getPosition().getX(), iTop, sliderThumb.getWidth(), iHeight);

    }
    else
    {
        const int width = getWidth();
        const int height = getHeight()-18;
        g.setColour (Colours::whitesmoke);
        g.setOpacity (0.6);
        const float midPoint = height/2.f+sliderThumbWidth;
        const float markerGap = height/9.f;
        g.drawLine (width*0.25, midPoint, width*0.75, midPoint, 1.59);
        g.setOpacity (0.3);

        for (int i=1; i<5; i++)
        {
            g.drawLine (width*0.3, midPoint+markerGap*i, width*0.7, midPoint+markerGap*i, .7);
            g.drawLine (width*0.3, midPoint-markerGap*i, width*0.7, midPoint-markerGap*i, .7);
        }

        g.setColour(Colours::whitesmoke);
        g.setOpacity (0.1);
        g.fillRoundedRectangle(width*0.44, sliderThumbWidth/2.f, width*0.15, height*1.003, width*0.05);
        g.setColour (Colour::fromRGBA(5, 5, 5, 255));
        g.fillRoundedRectangle (width*0.425, sliderThumbWidth/2.f, width*0.15, height*1.029, width*0.05);
        //tracker
        g.setColour(trackerColour);
        const float scale = trackerThickness;
        const int iWidth = (width* scale);
        const int iLeft = ((width-iWidth)/2.f);
        g.fillRect(iLeft, sliderThumb.getPosition().getY(), iWidth, sliderThumb.getHeight());
    }
}

void RangeSlider::showPopup()
{
    if(shouldDisplayPopup)
    {
        String popupText;
        if(tooltipText.isNotEmpty())
            popupText = tooltipText;
        else if(isVertical)
            popupText = channels.getReference(0)+": "+String(maxVal)+" "+channels.getReference(1)+": "+String(minVal);
        else
            popupText = channels.getReference(0)+": "+String(minVal)+" "+channels.getReference(1)+": "+String(maxVal);

        owner->showBubble(this, popupText);
    }
}

void RangeSlider::update(CabbageGUIType cAttr)
{
}

//--------------------------------------------------------
//slider thumb class
//--------------------------------------------------------
RangeSlider::SliderThumb::SliderThumb(RangeSlider* _owner, String _kind):
    Component(), owner(_owner), isVertical(_kind=="vertical" ? true : false)
{
    this->setInterceptsMouseClicks(false, false);
}

void RangeSlider::SliderThumb::paint(Graphics& g)
{
    const int thumbWidth = 10;
    const int thumbHeight = 10;
    //g.fillAll(Colours::red);

    if(isVertical ==false)
    {
        g.setColour(colour.darker(.7));
        g.fillRoundedRectangle(0, getHeight()*.2, thumbWidth, getHeight()*.6, 4);
        g.setColour(colour);
        g.fillRoundedRectangle(0, getHeight()*.2, thumbWidth*.9, getHeight()*.57, 4);

        g.setColour(colour.darker(.7));
        g.fillRoundedRectangle(getWidth()-thumbWidth, getHeight()*.2, thumbWidth, getHeight()*.60, 4);
        g.setColour(colour);
        g.fillRoundedRectangle(getWidth()-thumbWidth, getHeight()*.2, thumbWidth*.9, getHeight()*.57, 4);
    }
    else
    {
        g.setColour(colour.darker(.7));
        g.fillRoundedRectangle(getWidth()*.3, 0, getWidth()*.4, thumbHeight, 4);
        g.setColour(colour);
        g.fillRoundedRectangle(getWidth()*.3, 0, getWidth()*.37, thumbHeight*.9, 4);

        g.setColour(colour.darker(.7));
        g.fillRoundedRectangle(getWidth()*.3, getHeight()-thumbHeight, getWidth()*.4, thumbHeight, 4);
        g.setColour(colour);
        g.fillRoundedRectangle(getWidth()*.3, getHeight()-thumbHeight, getWidth()*.37, thumbHeight*.9, 4);
    }
}
//---------------------------------------------------------------------
//container class for rangeslider widget
//---------------------------------------------------------------------
CabbageRangeSlider2::CabbageRangeSlider2(CabbageGUIType &cAttr, CabbagePluginAudioProcessorEditor* _owner):
    slider(cAttr, _owner),
    text(cAttr.getStringProp(CabbageIDs::text)),
    textColour(cAttr.getStringProp(CabbageIDs::textcolour)),
    textLabel(text)
{
    isVertical = cAttr.getStringProp(CabbageIDs::kind)=="horizontal" ? false : true;
    //cUtils::debug(cAttr.getNumProp(CabbageIDs::sliderskew));

    addAndMakeVisible(&slider);
    slider.setBounds(cAttr.getNumProp(CabbageIDs::left), cAttr.getNumProp(CabbageIDs::top),
                     cAttr.getNumProp(CabbageIDs::width), cAttr.getNumProp(CabbageIDs::height));

    initVal1 = cAttr.getNumProp(CabbageIDs::minvalue);
    initVal2 = cAttr.getNumProp(CabbageIDs::maxvalue);
    min = cAttr.getNumProp(CabbageIDs::min);
    max = cAttr.getNumProp(CabbageIDs::max);

    const float value1 = jmap(initVal1, min, max, 0.f, 1.f);
    const float value2 = jmap(initVal2, min, max, 0.f, 1.f);

    slider.setValue(value1, value2);

    textLabel.setColour(Label::textColourId, Colour::fromString(textColour));
    textLabel.setColour(Label::outlineColourId, Colours::transparentBlack);
    addAndMakeVisible(&textLabel);
    textLabel.setVisible(false);
}

void CabbageRangeSlider2::resized()
{
    if(text.isNotEmpty())
    {
        if(isVertical)
        {
            textLabel.setBounds(0, getHeight()-20, getWidth(), 20);
            textLabel.setJustificationType(Justification::centred);
            textLabel.setText(text, dontSendNotification);
            textLabel.setVisible(true);
            slider.setBounds(0, 4, getWidth(), getHeight()-30);
        }
        else
        {
            float width = textLabel.getFont().getStringWidthFloat(text)+10.f;
            textLabel.setBounds(0, 0, width, getHeight());
            textLabel.setText(text, dontSendNotification);
            textLabel.setVisible(true);
            slider.setBounds(width, 0, getWidth()-(width*1.10), getHeight());
        }

        const float value1 = jmap(initVal1, min, max, 0.f, 1.f);
        const float value2 = jmap(initVal2, min, max, 0.f, 1.f);
        slider.setValue(value1, value2);
    }
    else if(isVertical)
        slider.setBounds(getLocalBounds().withTop(3).withHeight(getHeight()-8));
    else
        slider.setBounds(getLocalBounds().withLeft(3).withWidth(getWidth()-8));

}

//================================================================================================================
// Display widget
//================================================================================================================
CabbageSignalDisplay::CabbageSignalDisplay (CabbageGUIType &cAttr, CabbagePluginAudioProcessorEditor* _owner)
    : Component(),
      owner(_owner),
      colour(Colour::fromString(cAttr.getStringProp(CabbageIDs::colour))),
      backgroundColour(Colour::fromString(cAttr.getStringProp(CabbageIDs::tablebackgroundcolour))),
      fontColour(Colour::fromString(cAttr.getStringProp(CabbageIDs::fontcolour))),
      minFFTBin(0),
      maxFFTBin(1024),
      size(2048),
      shouldDrawSonogram(cAttr.getStringProp(CabbageIDs::displaytype)=="spectrogram" ? true : false),
      displayType(cAttr.getStringProp(CabbageIDs::displaytype)),
      spectrogramImage(Image::RGB, 512, 300, true),
      spectroscopeImage(Image::RGB, 512, 300, true),
      freqRangeDisplay(fontColour, backgroundColour),
      freqRange(cAttr.getNumProp(CabbageIDs::min), cAttr.getNumProp(CabbageIDs::max)),
      scrollbar(false),
      scopeWidth(cAttr.getNumProp(CabbageIDs::width)),
      zoomInButton("zoomIn", Colours::white),
      zoomOutButton("zoomOut", Colours::white),
      zoomLevel(cAttr.getNumProp(CabbageIDs::zoom)),
      leftPos(0),
      isScrollbarShowing(false),
      scrollbarHeight(20),
      rotate(cAttr.getNumProp(CabbageIDs::rotate)),
      lineThickness(cAttr.getNumProp(CabbageIDs::outlinethickness))
{
    addAndMakeVisible(freqRangeDisplay);

    if(displayType=="waveform")
        freqRangeDisplay.setVisible(false);

    addAndMakeVisible(scrollbar);
    scrollbar.setRangeLimits(Range<double>(0, 20));
    zoomInButton.addChangeListener(this);
    zoomOutButton.addChangeListener(this);
    //hide scrollbar, visible not working for disabling it...
    scrollbar.setBounds(-1000, getHeight()-15, getWidth(), 15);
    scrollbar.setAutoHide(false);
    scrollbar.addListener(this);
    if(zoomLevel>=0)
    {
        addAndMakeVisible(zoomInButton);
        addAndMakeVisible(zoomOutButton);
    }
}

void CabbageSignalDisplay::setBins(int min, int max)
{
    minFFTBin = min;
    maxFFTBin = max;
}

void CabbageSignalDisplay::scrollBarMoved (ScrollBar* scrollBarThatHasMoved, double newRangeStart)
{
    ScrollBar* scroll = dynamic_cast<ScrollBar*>(scrollBarThatHasMoved);
    if(scroll)
    {
        float moveBy = newRangeStart/scrollBarThatHasMoved->getCurrentRange().getLength();
        moveBy = freqRangeDisplay.getWidth()*moveBy;
        cUtils::debug("MoveBy:", moveBy);
        freqRangeDisplay.setTopLeftPosition(-moveBy, 0);
        leftPos = -moveBy;
    }
}

void CabbageSignalDisplay::changeListenerCallback(ChangeBroadcaster *source)
{
    RoundButton* button = dynamic_cast<RoundButton*>(source);

    if(button->getName()=="zoomIn")
        zoomIn();
    else
        zoomOut();
}

void CabbageSignalDisplay::zoomIn(int factor)
{
    for(int i=0; i<factor; i++)
    {
        zoomLevel = zoomLevel<20 ? zoomLevel+1 : 20;
        const Range<double> newRange (0.0, 20-zoomLevel);
        scrollbar.setCurrentRange (newRange);
        freqRangeDisplay.setBounds(1, 0, getWidth()*(zoomLevel+1), 18);
        freqRangeDisplay.setResolution(10*zoomLevel+1);
        scopeWidth = freqRangeDisplay.getWidth();
        showScrollbar(true);
    }
}

void CabbageSignalDisplay::zoomOut(int factor)
{
    for(int i=0; i<factor; i++)
    {
        zoomLevel = zoomLevel>1 ? zoomLevel-1 : 0;
        const Range<double> newRange (0.0, 20-zoomLevel);
        scrollbar.setCurrentRange (newRange);
        freqRangeDisplay.setBounds(0, 0, getWidth()*jmax(1, zoomLevel+1), 18);
        freqRangeDisplay.setResolution(jmax(10, 10*zoomLevel+1));
        scopeWidth = freqRangeDisplay.getWidth();
        if(zoomLevel<1)
            showScrollbar(false);
    }
}

void CabbageSignalDisplay::showScrollbar(bool show)
{
    if(show)
    {
        scrollbar.setBounds(0, getHeight()-scrollbarHeight, getWidth(), scrollbarHeight);
        zoomInButton.setBounds(getWidth()-40, getHeight()-(scrollbarHeight*2+5), 20, 20);
        zoomOutButton.setBounds(getWidth()-20, getHeight()-(scrollbarHeight*2+5), 20, 20);
        isScrollbarShowing=true;
    }
    else
    {
        scrollbar.setBounds(-1000, getHeight()-scrollbarHeight, getWidth(), scrollbarHeight);
        zoomInButton.setBounds(getWidth()-40, getHeight()-scrollbarHeight-5, 20, 20);
        zoomOutButton.setBounds(getWidth()-20, getHeight()-scrollbarHeight-5, 20, 20);
        isScrollbarShowing=false;
    }
}

void CabbageSignalDisplay::drawSonogram()
{
    const int rightHandEdge = spectrogramImage.getWidth() - 2;
    const int imageHeight = spectrogramImage.getHeight();

    spectrogramImage.moveImageSection (0, 0, 1, 0, rightHandEdge, imageHeight);

    Graphics g(spectrogramImage);
    //g.fillAll(backgroundColour);
    Range<float> maxLevel = FloatVectorOperations::findMinAndMax(points.getRawDataPointer(), points.size());

    for (int y = 0; y < imageHeight; y++)
    {
        const int index = jmap(y, 0, imageHeight, 0, size);
        const float level = jmap (points[index], 0.0f, maxLevel.getEnd(), 0.0f, 1.0f);
        g.setColour(Colour::fromHSV (level, 1.0f, level, 1.0f));
        g.drawHorizontalLine(imageHeight-y, rightHandEdge, rightHandEdge+2);
    }
}

void CabbageSignalDisplay::drawSpectroscope(Graphics& g)
{
    g.fillAll(backgroundColour);
    for (int i=0; i<size; i++)
    {
        const int position = jmap(i, 0, size, leftPos, scopeWidth);
        const int offset = isScrollbarShowing==true ? scrollbarHeight : 0;
        const int height = getHeight()-offset;
        const int amp = (points[i]*6*height);
        const int lineWidth = jmax(1, scopeWidth/size);
        g.setColour(colour);
        g.drawVerticalLine(position, height-amp, height);
    }
}

void CabbageSignalDisplay::drawWaveform(Graphics& g)
{
    g.fillAll(backgroundColour);
    const int offset = isScrollbarShowing==true ? scrollbarHeight : 0;
    const int height = getHeight()-offset;
    int newSize = 512;
    int prevXPos = 0;
    int prevYPos = height/2;

    for (int i=0; i<newSize; i++)
    {
        const int position = jmap(i, 0, newSize, leftPos, scopeWidth);
        const int amp = jmap(points[i], -1.f, 1.f, 0.f, 1.f)*height;
        const int lineWidth = jmax(1, scopeWidth/size);
        g.setColour(colour);
        g.drawLine(prevXPos, prevYPos, position, amp, lineThickness);
        prevXPos = position;
        prevYPos = amp;
    }
}

void CabbageSignalDisplay:: paint(Graphics& g)
{
    if(shouldDrawSonogram)
        g.drawImageWithin(spectrogramImage, 0, 0, getWidth(), getHeight(), RectanglePlacement::stretchToFit);
    else if(displayType=="spectroscope")
        drawSpectroscope(g);
    else if(displayType=="waveform")
        drawWaveform(g);

}

void CabbageSignalDisplay::mouseMove(const MouseEvent &e)
{
    if(shouldDrawSonogram)
    {
        const int position = jmap(e.getPosition().getY(), 0, getHeight(), 22050, 0);
        showPopup(String(position)+"Hz.");
    }
    else
    {
        const int position = jmap(e.getPosition().getX(), 0, scopeWidth, 0, 22050);
        showPopup(String(position)+"Hz.");
    }
}

void CabbageSignalDisplay::setPoints(Array<float, CriticalSection> _points)
{
    points = _points;
    size = points.size();
    if(size>0)
    {
        freq = 44100/size;
        //spectrogram works on a scrolling image, the other displays are drawn directly
        if(displayType=="spectrogram")
            drawSonogram();

        repaint();
    }

}

void CabbageSignalDisplay::showPopup(String text)
{
    owner->showBubble(this, text);
}

void CabbageSignalDisplay::resized()
{
    scrollbarHeight = jmin(15.0, getHeight()*.09);
    if(!shouldDrawSonogram)
    {
        freqRangeDisplay.setBounds(1, 0, getWidth(), 18);
        const int offset = isScrollbarShowing==true ? 41 : 22;
        zoomInButton.setBounds(getWidth()-40, getHeight()-offset, 20, 20);
        zoomOutButton.setBounds(getWidth()-20, getHeight()-offset, 20, 20);
    }

    zoomIn(zoomLevel);
}


void CabbageSignalDisplay::update(CabbageGUIType m_cAttr)
{
    if(m_cAttr.getStringProp(CabbageIDs::displaytype)!=displayType)
    {
        displayType = m_cAttr.getStringProp(CabbageIDs::displaytype);
        shouldDrawSonogram = displayType=="spectrogram" ? true : false;
        if(shouldDrawSonogram)
        {
            freqRangeDisplay.setVisible(false);
            zoomInButton.setVisible(false);
            zoomOutButton.setVisible(false);
            scrollbar.setVisible(false);
        }
        else if(displayType=="spectroscope")
        {
            freqRangeDisplay.setVisible(true);
            zoomInButton.setVisible(true);
            zoomOutButton.setVisible(true);
        }
        else if(displayType=="waveform")
        {
            freqRangeDisplay.setVisible(false);
            zoomInButton.setVisible(true);
            zoomOutButton.setVisible(true);
        }
    }

    if(freqRange!=Range<int>(m_cAttr.getNumProp(CabbageIDs::min),m_cAttr.getNumProp(CabbageIDs::max)))
    {
        freqRange = Range<int>(m_cAttr.getNumProp(CabbageIDs::min),m_cAttr.getNumProp(CabbageIDs::max));
        freqRangeDisplay.setMinMax(freqRange.getStart(), freqRange.getEnd());
    }

    setAlpha(m_cAttr.getNumProp(CabbageIDs::alpha));

    if(rotate!=m_cAttr.getNumProp(CabbageIDs::rotate))
    {
        rotate = m_cAttr.getNumProp(CabbageIDs::rotate);
        setTransform(AffineTransform::rotation(rotate, getX()+m_cAttr.getNumProp(CabbageIDs::pivotx), getY()+m_cAttr.getNumProp(CabbageIDs::pivoty)));
    }

    if(!m_cAttr.getNumProp(CabbageIDs::visible))
    {
        setVisible(false);
        setEnabled(false);
    }
    else
    {
        setEnabled(true);
        setVisible(true);
    }

    if(!m_cAttr.getNumProp(CabbageIDs::active))
    {
        setEnabled(false);
    }
    else
    {
        setEnabled(true);
    }
}


//================================================================================================================
// combobox widget
//================================================================================================================
CabbageComboBox::CabbageComboBox(CabbageGUIType &cAttr, CabbagePluginAudioProcessorEditor* _owner):
    name(cAttr.getStringProp(CabbageIDs::name)),
    caption(cAttr.getStringProp(CabbageIDs::caption)),
    colour(cAttr.getStringProp(CabbageIDs::colour)),
    fontcolour(cAttr.getStringProp(CabbageIDs::fontcolour)),
    rotate(cAttr.getNumProp(CabbageIDs::rotate)),
    pivotx(cAttr.getNumProp(CabbageIDs::pivotx)),
    pivoty(cAttr.getNumProp(CabbageIDs::pivoty)),
    tooltipText(String::empty),
    refresh(0),
    owner(_owner)
{
    bool isPresetCombo = false;
    setName(name);
    offX=offY=offWidth=offHeight=0;
    StringArray fileNames;
    groupbox = new GroupComponent(String("groupbox_")+name);
    combo = new ComboBox(name);

    addAndMakeVisible(groupbox);
    addAndMakeVisible(combo);
    groupbox->setVisible(false);
    groupbox->getProperties().set("groupLine", var(1));
    combo->setColour(ComboBox::textColourId, Colour::fromString(fontcolour));
    combo->setColour(ComboBox::backgroundColourId, Colour::fromString(colour));
    combo->lookAndFeelChanged();
    combo->getProperties().set("svgpath", cAttr.getStringProp(CabbageIDs::svgpath));

    groupbox->setColour(GroupComponent::textColourId, Colour::fromString(fontcolour));
    groupbox->setColour(TextButton::buttonColourId, cUtils::getComponentSkin());

    if(caption.length()>0)
    {
        offX=10;
        offY=35;
        offWidth=-20;
        offHeight=-45;
        groupbox->setVisible(true);
        groupbox->setText(caption);
    }

    if(cAttr.getStringProp(CabbageIDs::popuptext).isNotEmpty())
    {
        tooltipText = cAttr.getStringProp(CabbageIDs::popuptext);
        combo->setTooltip(tooltipText);
    }

    combo->setEditableText (false);

    Justification justify(Justification::centred);

    if(cAttr.getStringProp(CabbageIDs::align)=="left")
        justify = Justification::left;
    else if(cAttr.getStringProp(CabbageIDs::align)=="centre")
        justify = Justification::centred;
    else
        justify = Justification::right;

    combo->setJustificationType (justify);
    combo->setTextWhenNothingSelected(text);
    this->setWantsKeyboardFocus(false);
    setAlpha(cAttr.getNumProp(CabbageIDs::alpha));
    //populate combo with files
    Array<File> dirFiles;


    if(cAttr.getStringProp(CabbageIDs::file).isNotEmpty())
    {
        combo->clear(dontSendNotification);
        String file = File(cAttr.getStringProp(CabbageIDs::file)).loadFileAsString();
        StringArray lines = StringArray::fromLines(file);
        for (int i = 0; i < lines.size(); ++i)
        {
            combo->addItem(lines[i], i+1);
        }
    }

    else if(cAttr.getStringProp(CabbageIDs::filetype).length()<1)
    {
        combo->clear(dontSendNotification);
        for(int i=0; i<cAttr.getStringArrayProp("text").size(); i++)
        {
            String item  = cAttr.getStringArrayPropValue("text", i);
            combo->addItem(item, i+1);
        }
    }
    else
    {
        //appProperties->getUserSettings()->getValue("CsoundPluginDirectory");
        combo->clear(dontSendNotification);
        //cUtils::showMessage(cAttr.getStringProp(CabbageIDs::workingdir));
        pluginDir = File(cAttr.getStringProp(CabbageIDs::workingdir));

        filetype = cAttr.getStringProp("filetype");

        pluginDir.findChildFiles(dirFiles, 2, false, filetype);

        for (int i = 0; i < dirFiles.size(); ++i)
        {
            //m.addItem (i + menuSize, cabbageFiles[i].getFileNameWithoutExtension());
            //String test  = String(i+1)+": "+dirFiles[i].getFileName();
            String filename;
            if(filetype.contains("snaps"))
            {
                filename = dirFiles[i].getFileNameWithoutExtension();
                isPresetCombo = true;
            }
            else
                filename = dirFiles[i].getFileName();

            fileNames.add(filename);
            //cAttr.setStringArrayPropValue(CabbageIDs::text, i, filename);
        }

        fileNames.sort(true);
        for( int i=0; i<fileNames.size(); i++)
            combo->addItem(fileNames[i], i+1);
    }
    //cAttr.setStringArrayProp(CabbageIDs::text, fileNames);

    combo->setSelectedItemIndex(cAttr.getNumProp(CabbageIDs::value)-1, isPresetCombo ? sendNotification : dontSendNotification);
    //owner->getFilter()->setSelectedPresetFile(fileNames[cAttr.getNumProp(CabbageIDs::value)-1]+".snaps");

}
//---------------------------------------------
CabbageComboBox::~CabbageComboBox()
{

}


//update controls
void CabbageComboBox::update(CabbageGUIType m_cAttr)
{
    const MessageManagerLock mmLock;
    combo->getProperties().set("colour", m_cAttr.getStringProp(CabbageIDs::colour));
    combo->getProperties().set("fontcolour", m_cAttr.getStringProp(CabbageIDs::fontcolour));
    setBounds(m_cAttr.getBounds());
    setAlpha(m_cAttr.getNumProp(CabbageIDs::alpha));
    if(rotate!=m_cAttr.getNumProp(CabbageIDs::rotate))
    {
        rotate = m_cAttr.getNumProp(CabbageIDs::rotate);
        setTransform(AffineTransform::rotation(rotate, getX()+m_cAttr.getNumProp(CabbageIDs::pivotx), getY()+m_cAttr.getNumProp(CabbageIDs::pivoty)));
    }
    if(!m_cAttr.getNumProp(CabbageIDs::visible))
    {
        setVisible(false);
        setEnabled(false);
    }
    else
    {
        setVisible(true);
        setEnabled(true);
    }

    if(refresh != m_cAttr.getNumProp(CabbageIDs::refreshfiles))
    {
        refresh = m_cAttr.getNumProp(CabbageIDs::refreshfiles);
        owner->refreshDiskReadingGUIControls("combobox");
    }

    if(!m_cAttr.getNumProp(CabbageIDs::active))
    {
        setEnabled(false);
    }
    else
    {
        setEnabled(true);
    }
    repaint();
}


//---------------------------------------------
void CabbageComboBox::resized()
{
    groupbox->setBounds(0, 0, getWidth(), getHeight());
    combo->setBounds(offX, offY, getWidth()+offWidth, getHeight()+offHeight);
    if(rotate!=0)
        setTransform(AffineTransform::rotation(rotate, getX()+pivotx, pivoty+getY()));
    this->setWantsKeyboardFocus(false);
}


//================================================================================================================
// Listbox widget
//================================================================================================================

CabbageListbox::CabbageListbox(CabbageGUIType &cAttr, CabbagePluginAudioProcessorEditor* _owner):
    owner(_owner),
    Component(),
    justify(Justification::centred),
    align(cAttr.getStringProp(CabbageIDs::align)),
    colour(cAttr.getStringProp(CabbageIDs::colour)),
    fontcolour(cAttr.getStringProp(CabbageIDs::fontcolour)),
    channel(cAttr.getStringProp(CabbageIDs::channel)),
    highlightcolour(cAttr.getStringProp(CabbageIDs::highlightcolour)),
    channelType(cAttr.getStringProp(CabbageIDs::channeltype)),
    rotate(cAttr.getNumProp(CabbageIDs::rotate)),
    currentRow(-1),
    currentRowText(""),
    refresh(0)
{
    addAndMakeVisible(listBox);
    listBox.setRowHeight (20);
    listBox.setModel (this);   // Tell the listbox where to get its data model

    bgColour = Colour::fromString(colour);

    if(align=="left")
        justify = Justification::left;
    else if(align=="centre")
        justify = Justification::centred;
    else
        justify = Justification::right;

    if(cAttr.getStringProp(CabbageIDs::file).isNotEmpty())
    {
        items.clear();
        String file = File(cAttr.getStringProp(CabbageIDs::file)).loadFileAsString();
        StringArray lines = StringArray::fromLines(file);
        for (int i = 0; i < lines.size(); ++i)
        {
            items.add(lines[i]);
        }
    }
    else if(cAttr.getStringProp(CabbageIDs::filetype).length()<1)
    {
        items.clear();
        for(int i=0; i<cAttr.getStringArrayProp("text").size(); i++)
        {
            String item  = cAttr.getStringArrayPropValue("text", i);
            items.add(item);
        }
    }
    else
    {
        items.clear();
        Array<File> dirFiles;
        File pluginDir(cAttr.getStringProp(CabbageIDs::workingdir));

        cUtils::debug(cAttr.getStringProp(CabbageIDs::workingdir));

        const String filetype = cAttr.getStringProp("filetype");

        pluginDir.findChildFiles(dirFiles, 2, false, filetype);

        for (int i = 0; i < dirFiles.size(); ++i)
        {
            String filename;
            if(filetype.contains("snaps"))
                filename = dirFiles[i].getFileNameWithoutExtension();
            else
                filename = dirFiles[i].getFileName();
            items.add(filename);
        }

        items.sort(true);
    }
    //cAttr.setStringArrayProp(CabbageIDs::text, fileNames);

    listBox.updateContent();
    listBox.selectRow(cAttr.getNumProp(CabbageIDs::value)-1);

}

CabbageListbox::~CabbageListbox()
{
    owner = nullptr;
}

void CabbageListbox::paint (Graphics& g)
{
    g.fillAll(Colour::fromString(colour));
}

void CabbageListbox::resized()
{
    listBox.setBounds(0, 0, getWidth(), getHeight());
    //demoTextBox.setBounds(0, getHeight()*.3, getWidth(), getHeight()*.7);
}


// The following methods implement the ListBoxModel virtual methods:
int CabbageListbox::getNumRows()
{
    return items.size();
}

void CabbageListbox::listBoxItemDoubleClicked(int row, const MouseEvent &e)
{
    if(channelType!="string")
        owner->getFilter()->messageQueue.addOutgoingChannelMessageToQueue(channel, row+1);
    else
        owner->getFilter()->messageQueue.addOutgoingChannelMessageToQueue(channel, items[row], "string");

    currentRow = row;

}


void CabbageListbox::paintListBoxItem (int rowNumber, Graphics& g,
                                       int width, int height, bool rowIsSelected)
{
    if (rowIsSelected)
        g.fillAll (Colour::fromString(highlightcolour));
    else
        g.fillAll(bgColour);

    g.setColour(Colour::fromString(fontcolour));
    g.drawFittedText(items[rowNumber], Rectangle<int> (width, height), justify, 0);
}

void CabbageListbox::selectedRowsChanged (int lastRowselected)
{
    currentRow = lastRowselected;
}

void CabbageListbox::update(CabbageGUIType m_cAttr)
{
    const MessageManagerLock mmLock;
    colour = m_cAttr.getStringProp(CabbageIDs::colour);
    fontcolour = m_cAttr.getStringProp(CabbageIDs::fontcolour);
    highlightcolour = m_cAttr.getStringProp(CabbageIDs::highlightcolour);
    setBounds(m_cAttr.getBounds());
    setAlpha(m_cAttr.getNumProp(CabbageIDs::alpha));
    if(rotate!=m_cAttr.getNumProp(CabbageIDs::rotate))
    {
        rotate = m_cAttr.getNumProp(CabbageIDs::rotate);
        setTransform(AffineTransform::rotation(rotate, getX()+m_cAttr.getNumProp(CabbageIDs::pivotx), getY()+m_cAttr.getNumProp(CabbageIDs::pivoty)));
    }
    if(!m_cAttr.getNumProp(CabbageIDs::visible))
    {
        setVisible(false);
        setEnabled(false);
    }
    else
    {
        setVisible(true);
        setEnabled(true);
    }

    if(refresh != m_cAttr.getNumProp(CabbageIDs::refreshfiles))
    {
        refresh = m_cAttr.getNumProp(CabbageIDs::refreshfiles);
        owner->refreshDiskReadingGUIControls("listbox");
    }

    if(!m_cAttr.getNumProp(CabbageIDs::active))
    {
        setEnabled(false);
    }
    else
    {
        setEnabled(true);
    }
    repaint();
}

