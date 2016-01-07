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
	for(int i=0;i<numberOfSteps;i++)
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
	for(int i=0;i<numberOfSteps;i++)
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
		for(int i=0;i<numberOfSteps;i++)
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
	for(int i=0;i<numberOfSteps;i++)
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