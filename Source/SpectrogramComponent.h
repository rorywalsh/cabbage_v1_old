/*
  ==============================================================================

    JUCE demo code - use at your own risk!

  ==============================================================================
*/


class SpectrogramComponent   : public Component,
    private Timer
{
public:
    SpectrogramComponent()
        : forwardFFT (fftOrder, false),
          Component(),
          spectrogramImage (Image::RGB, 512, 512, true),
          fifoIndex (0),
          nextFFTBlockReady (false)
    {
        setOpaque (true);
        startTimerHz (60);
        setSize (700, 500);
    }

    ~SpectrogramComponent()
    {

    }

    void setAudioBlock (AudioSampleBuffer& buffer)
    {
        if (buffer.getNumChannels() > 0)
        {
            float* channelData = buffer.getWritePointer(0);

            for (int i = 0; i < buffer.getNumSamples(); ++i)
            {
                pushNextSampleIntoFifo (channelData[i]);
            }
        }
    }

    //=======================================================================
    void paint (Graphics& g)
    {
        g.fillAll (Colours::black);

        g.setOpacity (1.0f);
        g.drawImageWithin (spectrogramImage, 0, 0, getWidth(), getHeight(), RectanglePlacement::stretchToFit);
    }

    void timerCallback() override
    {
        if (nextFFTBlockReady)
        {
            drawNextLineOfSpectrogram();
            nextFFTBlockReady = false;
            repaint();
        }
    }

    void pushNextSampleIntoFifo (float sample) noexcept
    {
        // if the fifo contains enough data, set a flag to say
        // that the next line should now be rendered..
        if (fifoIndex == fftSize)
        {
            if (! nextFFTBlockReady)
            {
                zeromem (fftData, sizeof (fftData));
                memcpy (fftData, fifo, sizeof (fifo));
                nextFFTBlockReady = true;
            }

            fifoIndex = 0;
        }

        fifo[fifoIndex++] = sample;
    }

    void drawNextLineOfSpectrogram()
    {
        const int rightHandEdge = spectrogramImage.getWidth() - 1;
        const int imageHeight = spectrogramImage.getHeight();

        // first, shuffle our image leftwards by 1 pixel..
        spectrogramImage.moveImageSection (0, 0, 1, 0, rightHandEdge, imageHeight);

        // then render our FFT data..
        forwardFFT.performFrequencyOnlyForwardTransform (fftData);

        // find the range of values produced, so we can scale our rendering to
        // show up the detail clearly
        Range<float> maxLevel = FloatVectorOperations::findMinAndMax (fftData, fftSize / 2);

        for (int y = 1; y < imageHeight; ++y)
        {
            const float skewedProportionY = 1.0f - std::exp (std::log (y / (float) imageHeight) * 0.2f);
            const int fftDataIndex = jlimit (0, fftSize / 2, (int) (skewedProportionY * fftSize / 2));
            const float level = jmap (fftData[fftDataIndex], 0.0f, maxLevel.getEnd(), 0.0f, 1.0f);

            spectrogramImage.setPixelAt (rightHandEdge, y, Colour::fromHSV (level, 1.0f, level, 1.0f));
        }
    }

    enum
    {
        fftOrder = 12,
        fftSize  = 1 << fftOrder
    };

private:
    FFT forwardFFT;
    Image spectrogramImage;

    float fifo [fftSize];
    float fftData [2 * fftSize];
    int fifoIndex;
    bool nextFFTBlockReady;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpectrogramComponent)
};
