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

static bool isStartingUpOrShuttingDown()
{
    if (auto* app = JUCEApplicationBase::getInstance())
        if (app->isInitialising())
            return true;

    if (auto* mm = MessageManager::getInstanceWithoutCreating())
        if (mm->hasStopMessageBeenSent())
            return true;

    return false;
}

//==============================================================================
class AccessibilityHandler::AccessibilityNativeImpl
{
public:
    explicit AccessibilityNativeImpl (AccessibilityHandler& owner)
    {
        accessibilityElement = new AccessibilityNativeHandle (owner);
    }

    ~AccessibilityNativeImpl()
    {
        accessibilityElement->invalidateElement();

        if (auto* wrapper = WindowsUIAWrapper::getInstanceWithoutCreating())
        {
            ComSmartPtr<IRawElementProviderSimple> provider;
            accessibilityElement->QueryInterface (IID_PPV_ARGS (provider.resetAndGetPointerAddress()));

            wrapper->disconnectProvider (provider);
        }
    }

    //==============================================================================
    ComSmartPtr<AccessibilityNativeHandle> accessibilityElement;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AccessibilityNativeImpl)
};

//==============================================================================
AccessibilityNativeHandle* AccessibilityHandler::getNativeImplementation() const
{
    return nativeImpl->accessibilityElement;
}

void AccessibilityHandler::notifyAccessibilityEvent (AccessibilityEvent eventType) const
{
    if (auto* wrapper = WindowsUIAWrapper::getInstanceWithoutCreating())
    {
        if (wrapper->clientsAreListening() && ! isStartingUpOrShuttingDown())
        {
            auto event = [] (AccessibilityEvent et) -> EVENTID
            {
                switch (et)
                {
                    case AccessibilityEvent::focusChanged:         return UIA_AutomationFocusChangedEventId;
                    case AccessibilityEvent::windowOpened:         return UIA_Window_WindowOpenedEventId;
                    case AccessibilityEvent::windowClosed:         return UIA_Window_WindowClosedEventId;
                    case AccessibilityEvent::textSelectionChanged: return UIA_Text_TextSelectionChangedEventId;
                    case AccessibilityEvent::textChanged:          return UIA_Text_TextChangedEventId;

                    case AccessibilityEvent::elementCreated:
                    case AccessibilityEvent::elementDestroyed:     return UIA_StructureChangedEventId;
                    case AccessibilityEvent::rowSelectionChanged:
                    default:                                       return {};
                }
            } (eventType);

            if (event != EVENTID{} && isValid (*this))
            {
                ComSmartPtr<IRawElementProviderSimple> provider;

                if (event == UIA_StructureChangedEventId)
                {
                    ComSmartPtr<IRawElementProviderFragmentRoot> root;
                    nativeImpl->accessibilityElement->get_FragmentRoot (root.resetAndGetPointerAddress());
                    root.QueryInterface (provider);
                }
                else
                {
                    nativeImpl->accessibilityElement->QueryInterface (IID_PPV_ARGS (provider.resetAndGetPointerAddress()));
                }

                wrapper->raiseAutomationEvent (provider, event);
            }
        }
    }
}

AccessibilityHandler::AccessibilityNativeImpl* AccessibilityHandler::createNativeImpl (AccessibilityHandler& handler)
{
    return new AccessibilityHandler::AccessibilityNativeImpl (handler);
}

void AccessibilityHandler::destroyNativeImpl (AccessibilityHandler::AccessibilityNativeImpl* impl)
{
    delete impl;
}

//==============================================================================
namespace WindowsAccessibility
{
    void initialiseUIAWrapper()
    {
        WindowsUIAWrapper::getInstance();
    }

    long getUiaRootObjectId()
    {
        return static_cast<long> (UiaRootObjectId);
    }

    bool handleWmGetObject (AccessibilityHandler* handler, WPARAM wParam, LPARAM lParam, LRESULT* res)
    {
        if (isStartingUpOrShuttingDown() || (handler == nullptr || ! isValid (*handler)))
            return false;

        if (auto* wrapper = WindowsUIAWrapper::getInstanceWithoutCreating())
        {
            ComSmartPtr<IRawElementProviderSimple> provider;
            handler->getNativeImplementation()->QueryInterface (IID_PPV_ARGS (provider.resetAndGetPointerAddress()));

            if (! wrapper->isProviderDisconnecting (provider))
                *res = wrapper->returnRawElementProvider ((HWND) handler->getComponent().getWindowHandle(), wParam, lParam, provider);

            return true;
        }

        return false;
    }

    void revokeUIAMapEntriesForWindow (HWND hwnd)
    {
        if (auto* wrapper = WindowsUIAWrapper::getInstanceWithoutCreating())
        {
            wrapper->returnRawElementProvider (hwnd, 0, 0, nullptr);
            wrapper->disconnectAllProviders();
        }
    }
}


JUCE_IMPLEMENT_SINGLETON (WindowsUIAWrapper)

} // namespace juce
