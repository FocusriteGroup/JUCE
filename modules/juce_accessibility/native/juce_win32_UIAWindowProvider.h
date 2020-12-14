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
class UIAWindowProvider : public ComBaseClassHelper<IWindowProvider>
{
public:
    UIAWindowProvider (AccessibilityHandler& handler)  : accessibilityHandler (handler)  {}

    JUCE_COMRESULT SetVisualState (WindowVisualState state) override
    {
        if (! isValid (accessibilityHandler))
            return UIA_E_ELEMENTNOTAVAILABLE;

        const auto& actions = accessibilityHandler.getActions();

        if (state == WindowVisualState_Maximized
            && actions.contains (AccessibilityActionType::maximise))
        {
            if (! accessibilityHandler.getCurrentState().isMaximised())
                actions.get (AccessibilityActionType::maximise)();

            return S_OK;
        }

        if (state == WindowVisualState_Minimized
            && actions.contains (AccessibilityActionType::minimise))
        {
            if (! accessibilityHandler.getCurrentState().isMinimised())
                actions.get (AccessibilityActionType::minimise)();

            return S_OK;
        }

        if (state == WindowVisualState_Normal
            && actions.contains (AccessibilityActionType::restore))
        {
            if (accessibilityHandler.getCurrentState().isMinimised()
                || accessibilityHandler.getCurrentState().isMaximised())
                actions.get (AccessibilityActionType::restore)();
        }

        return UIA_E_NOTSUPPORTED;
    }

    JUCE_COMRESULT Close() override
    {
        if (! isValid (accessibilityHandler))
            return UIA_E_ELEMENTNOTAVAILABLE;

        if (auto actionCallback = accessibilityHandler.getActions().get (AccessibilityActionType::close))
        {
            actionCallback();
            return S_OK;
        }

        return UIA_E_NOTSUPPORTED;
    }

    JUCE_COMRESULT WaitForInputIdle (int milliseconds, BOOL* pRetVal) override
    {
        if (pRetVal == nullptr)
            return E_INVALIDARG;

        if (! isValid (accessibilityHandler))
            return UIA_E_ELEMENTNOTAVAILABLE;

        ignoreUnused (milliseconds);
        return UIA_E_NOTSUPPORTED;
    }

    JUCE_COMRESULT get_CanMaximize (BOOL* pRetVal) override
    {
        if (pRetVal == nullptr)
            return E_INVALIDARG;

        *pRetVal = false;

        if (! isValid (accessibilityHandler))
            return UIA_E_ELEMENTNOTAVAILABLE;

        *pRetVal = accessibilityHandler.getActions().contains (AccessibilityActionType::maximise);
        return S_OK;
    }

    JUCE_COMRESULT get_CanMinimize (BOOL* pRetVal) override
    {
        if (pRetVal == nullptr)
            return E_INVALIDARG;

        *pRetVal = false;

        if (! isValid (accessibilityHandler))
            return UIA_E_ELEMENTNOTAVAILABLE;

        *pRetVal = accessibilityHandler.getActions().contains (AccessibilityActionType::minimise);
        return S_OK;
    }

    JUCE_COMRESULT get_IsModal (BOOL* pRetVal) override
    {
        if (pRetVal == nullptr)
            return E_INVALIDARG;

        *pRetVal = false;

        if (! isValid (accessibilityHandler))
            return UIA_E_ELEMENTNOTAVAILABLE;

        *pRetVal = accessibilityHandler.getCurrentState().isModal();
        return S_OK;
    }

    JUCE_COMRESULT get_WindowVisualState (WindowVisualState* pRetVal) override
    {
        if (pRetVal == nullptr)
            return E_INVALIDARG;

        if (! isValid (accessibilityHandler))
            return UIA_E_ELEMENTNOTAVAILABLE;

        auto state = accessibilityHandler.getCurrentState();

        if (state.isMaximised())
            *pRetVal = WindowVisualState_Maximized;
        else if (state.isMinimised())
            *pRetVal = WindowVisualState_Minimized;
        else
            *pRetVal = WindowVisualState_Normal;

        return S_OK;
    }

    JUCE_COMRESULT get_WindowInteractionState (WindowInteractionState* pRetVal) override
    {
        if (pRetVal == nullptr)
            return E_INVALIDARG;

        if (! isValid (accessibilityHandler))
            return UIA_E_ELEMENTNOTAVAILABLE;

        *pRetVal = accessibilityHandler.getCurrentState().isBlockedByModal()
                                   ? WindowInteractionState::WindowInteractionState_BlockedByModalWindow
                                   : WindowInteractionState::WindowInteractionState_Running;

        return S_OK;
    }

    JUCE_COMRESULT get_IsTopmost (BOOL* pRetVal) override
    {
        if (pRetVal == nullptr)
            return E_INVALIDARG;

        *pRetVal = false;

        if (! isValid (accessibilityHandler))
            return UIA_E_ELEMENTNOTAVAILABLE;

        return UIA_E_NOTSUPPORTED;
    }

private:
    AccessibilityHandler& accessibilityHandler;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UIAWindowProvider)
};

} // namespace juce
