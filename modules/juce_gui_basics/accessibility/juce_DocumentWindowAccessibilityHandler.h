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

/** Basic accessible interface for a DocumentWindow which can be minimised,
    maximised, closed, and restored.

    @tags{Accessibility}
*/
struct DocumentWindowAccessibilityHandler  : public ComponentAccessibilityHandler
{
    DocumentWindowAccessibilityHandler (DocumentWindow& documentWindowToWrap)
        : ComponentAccessibilityHandler (documentWindowToWrap,
                                         AccessibilityRole::window,
                                         buildAccessibilityActions (documentWindowToWrap)),
          documentWindow (documentWindowToWrap)
    {
    }

    AccessibleState getCurrentState() const override
    {
        auto state = ComponentAccessibilityHandler::getCurrentState();

        if (documentWindow.isMinimised())
            return state.withMinimised();

        if (documentWindow.isFullScreen())
            return state.withMaximised();

        return state;
    }

private:
    static AccessibilityActions buildAccessibilityActions (DocumentWindow& documentWindow)
    {
        auto styleFlags = documentWindow.getDesktopWindowStyleFlags();

        auto canMinimise = (styleFlags & ComponentPeer::windowHasMinimiseButton) != 0;
        auto canMaximise = (styleFlags & ComponentPeer::windowHasMaximiseButton) != 0;
        auto canClose = (styleFlags & ComponentPeer::windowHasCloseButton) != 0;

        AccessibilityActions actions;

        if (canMinimise || canMaximise)
        {
            actions.addAction (AccessibilityActionType::restore,
                               [&documentWindow]
                               {
                                   documentWindow.setMinimised (false);
                                   documentWindow.setFullScreen (false);
                               });

            if (canMinimise)
                actions.addAction (AccessibilityActionType::minimise, [&documentWindow] { documentWindow.minimiseButtonPressed(); });

            if (canMaximise)
                actions.addAction (AccessibilityActionType::maximise, [&documentWindow] { documentWindow.maximiseButtonPressed(); });
        }

        if (canClose)
            actions.addAction (AccessibilityActionType::close, [&documentWindow] { documentWindow.closeButtonPressed(); });

        return actions;
    }

    DocumentWindow& documentWindow;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DocumentWindowAccessibilityHandler)
};

} // namespace juce
