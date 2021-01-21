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

/** Basic accessible interface for a Label which can also show a TextEditor
    when clicked.

    @tags{Accessibility}
*/
struct LabelAccessibilityHandler  : public AccessibilityHandler
{
    LabelAccessibilityHandler (Label& labelToWrap)
        : AccessibilityHandler (labelToWrap,
                                AccessibilityRole::staticText,
                                buildAccessibilityActions (labelToWrap)),
          label (labelToWrap)
    {
    }

    String getTitle() const override  { return label.getText(); }

private:
    static AccessibilityActions buildAccessibilityActions (Label& label)
    {
        if (label.isEditable())
            return AccessibilityActions().addAction (AccessibilityActionType::press,  [&label] { label.showEditor(); })
                                         .addAction (AccessibilityActionType::cancel, [&label] { label.hideEditor (true); });

        return {};
    }

    Label& label;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LabelAccessibilityHandler)
};

} // namespace juce
