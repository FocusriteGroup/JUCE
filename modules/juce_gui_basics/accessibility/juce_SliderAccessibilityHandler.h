/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2020 - Raw Material Software Limited

   JUCE is an open source library subject to commercial or open-source
   licensing.

   By using JUCE, you agree to the terms of both the JUCE 6 End-User License
   Agreement and JUCE Privacy Policy (both effective as of the 16th June 2020).

   End User License Agreement: www.juce.com/juce-6-licence
   Privacy Policy: www.juce.com/juce-privacy-policy

   Or: You may also use this code under the terms of the GPL v3 (see
   www.gnu.org/licenses).

   JUCE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/

namespace juce
{

/** Basic accessible interface for a Slider.

    @tags{Accessibility}
*/
struct SliderAccessibilityHandler  : public ComponentAccessibilityHandler
{
    explicit SliderAccessibilityHandler (Slider& sliderToWrap)
        : ComponentAccessibilityHandler (sliderToWrap,
                                         AccessibilityRole::slider,
                                         {},
                                         std::make_unique<SliderValueInterface> (sliderToWrap))
    {
    }

private:
    struct SliderValueInterface  : public RangedNumericValueInterface
    {
        explicit SliderValueInterface (Slider& sliderToWrap)
            : slider (sliderToWrap)
        {
        }

        bool isReadOnly() const override          { return false; }

        // two and three value?
        double getCurrentValue() const override   { return slider.getValue(); }
        void setValue (double newValue) override  { slider.setValue (newValue, sendNotification); }

        double getStepSize() const override       { return getInterval(); }
        double getMinimumValue() const override   { return slider.getMinimum(); }
        double getMaximumValue() const override   { return slider.getMaximum(); }

        double getInterval() const
        {
            auto interval = slider.getInterval();

            if (interval == 0)
                return slider.proportionOfLengthToValue (0.01);

            return interval;
        }

        Slider& slider;
    };

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SliderAccessibilityHandler)
};

} // namespace juce
