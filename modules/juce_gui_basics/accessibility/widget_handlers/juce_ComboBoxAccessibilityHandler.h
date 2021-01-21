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

/** Basic accessible interface for a ComboBox which can show or hide a menu.

    @tags{Accessibility}
*/
struct ComboBoxAccessibilityHandler  : public AccessibilityHandler
{
    explicit ComboBoxAccessibilityHandler (ComboBox& comboBoxToWrap)
        : AccessibilityHandler (comboBoxToWrap,
                                AccessibilityRole::comboBox,
                                buildAccessibilityActions (comboBoxToWrap)),
          comboBox (comboBoxToWrap)
    {
    }

    String getTitle() const override  { return comboBox.getText(); }

private:
    static AccessibilityActions buildAccessibilityActions (ComboBox& comboBox)
    {
        return AccessibilityActions().addAction (AccessibilityActionType::press,    [&comboBox] { comboBox.showPopup(); })
                                     .addAction (AccessibilityActionType::showMenu, [&comboBox] { comboBox.showPopup(); })
                                     .addAction (AccessibilityActionType::cancel,   [&comboBox] { comboBox.hidePopup(); });
    }

    ComboBox& comboBox;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ComboBoxAccessibilityHandler)
};

} // namespace juce
