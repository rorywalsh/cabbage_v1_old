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
      trackerColour(Colour::fromString(cAttr.getStringProp(CabbageIDs::trackercolour)))
{
    if(cAttr.getStringArrayProp(CabbageIDs::channel).size()>1)
    {
        channels.add(cAttr.getStringArrayPropValue(CabbageIDs::channel, 0));
        channels.add(cAttr.getStringArrayPropValue(CabbageIDs::channel, 1));
    }

    sliderThumb.setColour(Colour::fromString(colour));

#ifdef Cabbage_Build_Standalone
    sendValuesToCsound(value1, value2);
#endif
    addAndMakeVisible(&sliderThumb);

}

void RangeSlider::resized()
{
    thumbIncr = (incr/(max-min))*getWidth();
    sliderThumb.setBounds(0, 0, getWidth(), getHeight());
    sliderThumbBounds = getLocalBounds();
    /*
        if(!isVertical)
        {
            const int xPos = getSliderPosition(SliderType::left);
            const int width = jmax(thumbWidth*2, getSliderPosition(SliderType::right));
            sliderThumb.setBounds(jmax(0, xPos), 0, width, getHeight());
        }
        else
        {
            const int height = jmax(thumbHeight*2, getSliderPosition(SliderType::bottom));
            sliderThumb.setBounds(0, getSliderPosition(SliderType::top), getWidth(), height);
        }
    	 */
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

    const double scaledHeight = (getHeight()-(2*thumbHeight))*.99;//scale down to prevent errors at different sizes...
    double proportion1 = (double)sliderThumb.getPosition().getY()/scaledHeight;
    double proportion2 = ((double)sliderThumb.getPosition().getY()+(double)sliderThumb.getHeight()-thumbHeight*2)/scaledHeight;
    value1 = cUtils::roundToMultiple(getSkewedValue(proportion1), incr);
    value2 = cUtils::roundToMultiple(getSkewedValue(proportion2), incr);
    sendValuesToCsound(value1, value2);
}


void RangeSlider::setValue(float val1, float val2)
{
	/*
    if(currentThumb==0)//only update size if the host is controlling the slider
    {
        if(!isVertical)
        {
#ifdef Cabbage_Build_Standalone
            //cUtils::debug(String(index)+" Value:"+String(value));
            if(type==RangeSlider::left)
            {
                int left = (value/(max-min))*width;
                sliderThumb.setBounds(sliderThumbBounds.withLeft(jmax(0, left)));
                sliderThumbBounds = sliderThumb.getBounds();
            }
            else if(type==RangeSlider::right)
            {
                int right = (value/(max-min))*width;
                sliderThumb.setBounds(sliderThumbBounds.withRight(jmin(getWidth(), right)));
                sliderThumbBounds = sliderThumb.getBounds();
            }
#else
            cUtils::debug(String(index)+" Value:"+String(val1*(max-min)*10.f));
            int left = val1*width;
            sliderThumb.setBounds(sliderThumbBounds.withLeft(jmax(0, left)));
            sliderThumbBounds = sliderThumb.getBounds();

            int right = val2*width;
            cUtils::getBoundsString(sliderThumb.getBounds());
            sliderThumb.setBounds(sliderThumbBounds.withRight(jmin((int)width, right)));
            sliderThumbBounds = sliderThumb.getBounds();
            repaint();

#endif


        }
    }

    //maxVal = value;
    //minVal = value;
*/
}


void RangeSlider::horizontalDrag(const MouseEvent& event)
{
    if(currentThumb == 1)
    {
        const int rightThumb = (sliderThumbBounds.getX()+sliderThumbBounds.getWidth())-thumbWidth*2;
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
        sliderThumb.setBounds(jmax(0, xPos), 0, sliderThumb.getWidth(), getHeight());
    }

    const double scaledWidth = (getWidth()-(2*thumbWidth))*.99;//scale down to prevent errors at different sizes...
    const double proportion1 = (double)sliderThumb.getPosition().getX()/scaledWidth;
    const double proportion2 = ((double)sliderThumb.getPosition().getX()+(double)sliderThumb.getWidth()-thumbWidth*2)/scaledWidth;
    value1 = cUtils::roundToMultiple(getSkewedValue(proportion1), incr);
    value2 = cUtils::roundToMultiple(getSkewedValue(proportion2), incr);
    sendValuesToCsound(value1, value2);

}

void RangeSlider::sendValuesToCsound(double val1, double val2)
{
    if(owner->getFilter()->csoundCompiledOk()==OK)
    {
#ifdef Cabbage_Build_Standalone
        owner->getFilter()->setParameter(index, (float)(val1));
        owner->getFilter()->setParameterNotifyingHost(index, (float)(val1));
        owner->getFilter()->setParameter(index+1, (float)(val2));
        owner->getFilter()->setParameterNotifyingHost(index+1, (float)(val2));
#else
        owner->getFilter()->setParameter(index, (float)(val1/(maxVal-minVal)*0.1f));
        owner->getFilter()->setParameterNotifyingHost(index, (float)(val1/(maxVal-minVal)*0.1f));
        owner->getFilter()->setParameter(index+1, (float)(val2/(maxVal-minVal)*0.1f));
        owner->getFilter()->setParameterNotifyingHost(index+1, (float)(val2/(maxVal-minVal)*0.1f));
#endif
    }
}

void RangeSlider::mouseEnter(const MouseEvent& event)
{
    showPopup();
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

void RangeSlider::paint(Graphics& g)
{
    const int width = getWidth();
    const int height = getHeight();

    g.fillAll(Colours::transparentBlack);
    if(!isVertical)
    {
        g.setColour (Colours::whitesmoke);
        g.setOpacity (0.6);
        const float midPoint = getWidth()/2.f;
        const float markerGap = getWidth()/9.f;
        g.drawLine (midPoint, height*0.2, midPoint, height*0.8, 1);
        g.setOpacity (0.3);
        for (int i=1; i<5; i++)
        {
            g.drawLine (midPoint+markerGap*i, height*0.3, midPoint+markerGap*i, height*0.7, .7);
            g.drawLine (midPoint-markerGap*i, height*0.3, midPoint-markerGap*i, height*0.7, .7);
        }
        //backgrounds
        g.setColour (Colours::whitesmoke);
        g.setOpacity (0.3);
        g.fillRoundedRectangle (0, height*0.41, width, height*0.15, height*0.05); //for light effect
        g.setColour (Colour::fromRGBA(5, 5, 5, 255));
        g.fillRoundedRectangle (0, height*0.40, width*0.995, height*0.15, height*0.05); //main rectangle
        //tracker
        g.setColour(trackerColour);
        const float scale = trackerThickness;
        const int iHeight = (height * scale);
        const int iTop = ((height-iHeight)/2.f);
        g.fillRect(sliderThumb.getPosition().getX(), iTop, sliderThumb.getWidth(), iHeight);

    }
    else
    {
        g.setColour (Colours::whitesmoke);
        g.setOpacity (0.6);
        const float midPoint = height/2.f;
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
        g.fillRoundedRectangle(width*0.44, 0, width*0.15, height, width*0.05);
        g.setColour (Colour::fromRGBA(5, 5, 5, 255));
        g.fillRoundedRectangle (width*0.425, 0, width*0.15, height*0.99, width*0.05);
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
            popupText = channels.getReference(0)+": "+String(value2)+" "+channels.getReference(1)+": "+String(value1);
        else
            popupText = channels.getReference(0)+": "+String(value1)+" "+channels.getReference(1)+": "+String(value2);

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
    textLabel(text),
    isVertical(cAttr.getStringProp(CabbageIDs::kind)=="horizontal" ? false : true)
{
    cUtils::debug(cAttr.getNumProp(CabbageIDs::sliderskew));

    addAndMakeVisible(&slider);
    slider.setBounds(cAttr.getNumProp(CabbageIDs::left),
                     cAttr.getNumProp(CabbageIDs::top),
                     cAttr.getNumProp(CabbageIDs::width),
                     cAttr.getNumProp(CabbageIDs::height));

    if(!isVertical)
    {
        slider.setValue(cAttr.getNumProp(CabbageIDs::minvalue), cAttr.getNumProp(CabbageIDs::maxvalue));
    }

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
    }
    else
        slider.setBounds(getLocalBounds());

}
