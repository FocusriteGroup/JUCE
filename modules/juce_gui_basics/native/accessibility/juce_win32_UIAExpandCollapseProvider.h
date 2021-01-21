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

//==============================================================================
class UIAExpandCollapseProvider  : public ComBaseClassHelper<IExpandCollapseProvider>
{
public:
    UIAExpandCollapseProvider (AccessibilityHandler& handler)  : accessibilityHandler (handler)  {}

    //==============================================================================
    JUCE_COMRESULT Expand() override
    {
        if (! isValid (accessibilityHandler))
            return UIA_E_ELEMENTNOTAVAILABLE;

        if (auto actionCallback = accessibilityHandler.getActions().get (AccessibilityActionType::showMenu))
        {
            actionCallback();
            return S_OK;
        }

        return UIA_E_NOTSUPPORTED;
    }

    JUCE_COMRESULT Collapse() override
    {
        if (! isValid (accessibilityHandler))
            return UIA_E_ELEMENTNOTAVAILABLE;

        if (auto actionCallback = accessibilityHandler.getActions().get (AccessibilityActionType::cancel))
        {
            actionCallback();
            return S_OK;
        }

        return UIA_E_NOTSUPPORTED;
    }

    JUCE_COMRESULT get_ExpandCollapseState (ExpandCollapseState* pRetVal) override
    {
        if (pRetVal == nullptr)
            return E_INVALIDARG;

        *pRetVal = ExpandCollapseState_LeafNode;

        if (! isValid (accessibilityHandler))
            return UIA_E_ELEMENTNOTAVAILABLE;

        *pRetVal = accessibilityHandler.getCurrentState().isExpanded()
                       ? ExpandCollapseState_Expanded
                       : ExpandCollapseState_Collapsed;

        return S_OK;
    }

private:
    AccessibilityHandler& accessibilityHandler;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UIAExpandCollapseProvider)
};

} // namespace juce
