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
class UIATransformProvider  : public ComBaseClassHelper<ITransformProvider>
{
public:
    UIATransformProvider (AccessibilityHandler& handler)  : accessibilityHandler (handler)  {}

    JUCE_COMRESULT Move (double x, double y) override
    {
        if (! isValid (accessibilityHandler))
            return UIA_E_ELEMENTNOTAVAILABLE;

        if (auto* peer = accessibilityHandler.getComponent().getPeer())
        {
            RECT rect;
            GetWindowRect ((HWND) peer->getNativeHandle(), &rect);

            rect.left = roundToInt (x);
            rect.top  = roundToInt (y);

            peer->setBounds (Desktop::getInstance().getDisplays().physicalToLogical (Rectangle<int> (rect.left,
                                                                                                     rect.top,
                                                                                                     rect.right - rect.left,
                                                                                                     rect.bottom - rect.top)),
                             peer->isFullScreen());
        }

        return S_OK;
    }

    JUCE_COMRESULT Resize (double width, double height) override
    {
        if (! isValid (accessibilityHandler))
            return UIA_E_ELEMENTNOTAVAILABLE;

        if (auto* peer = accessibilityHandler.getComponent().getPeer())
        {
            auto scale = peer->getPlatformScaleFactor();

            accessibilityHandler.getComponent().setSize (roundToInt (width  / scale),
                                                         roundToInt (height / scale));
        }

        return S_OK;
    }

    JUCE_COMRESULT Rotate (double degrees) override
    {
        if (! isValid (accessibilityHandler))
            return UIA_E_ELEMENTNOTAVAILABLE;

        ignoreUnused (degrees);
        return UIA_E_NOTSUPPORTED;
    }

    JUCE_COMRESULT get_CanMove (BOOL* pRetVal) override
    {
        if (pRetVal == nullptr)
            return E_INVALIDARG;

        *pRetVal = false;

        if (! isValid (accessibilityHandler))
            return UIA_E_ELEMENTNOTAVAILABLE;

        *pRetVal = true;
        return S_OK;
    }

    JUCE_COMRESULT get_CanResize (BOOL* pRetVal) override
    {
        if (pRetVal == nullptr)
            return E_INVALIDARG;

        *pRetVal = false;

        if (! isValid (accessibilityHandler))
            return UIA_E_ELEMENTNOTAVAILABLE;

        if (auto* peer = accessibilityHandler.getComponent().getPeer())
            *pRetVal = ((peer->getStyleFlags() & ComponentPeer::windowIsResizable) != 0);

        return S_OK;
    }

    JUCE_COMRESULT get_CanRotate (BOOL* pRetVal) override
    {
        if (pRetVal == nullptr)
            return E_INVALIDARG;

        *pRetVal = false;

        if (! isValid (accessibilityHandler))
            return UIA_E_ELEMENTNOTAVAILABLE;

        return S_OK;
    }

private:
    AccessibilityHandler& accessibilityHandler;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UIATransformProvider)
};

} // namespace juce
