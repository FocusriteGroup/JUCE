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
class AccessibilityHandler::AccessibilityNativeImpl  : public AccessibilityHandler::Pimpl
{
public:
    explicit AccessibilityNativeImpl (AccessibilityHandler& owner)
    {
        accessibilityElement = new AccessibilityNativeHandle (owner);
    }

    ~AccessibilityNativeImpl()
    {
        if (auto* wrapper = WindowsUIAWrapper::getInstance())
            wrapper->disconnectProvider (accessibilityElement);

        accessibilityElement->invalidateElement();
        accessibilityElement->Release();
    }

    AccessibilityNativeHandle* getNativeImplementation() const noexcept override
    {
        accessibilityElement->AddRef();
        return accessibilityElement;
    }

    void notifyAccessibilityEvent (AccessibilityEvent eventType) const override
    {
        if (auto* wrapper = WindowsUIAWrapper::getInstance())
            if (wrapper->clientsAreListening() && ! isStartingUpOrShuttingDown())
                wrapper->raiseAutomationEvent (accessibilityElement, eventToEventID (eventType));
    }

private:
    //==============================================================================
    static EVENTID eventToEventID (AccessibilityEvent event)
    {
        static const std::map<AccessibilityEvent, EVENTID> eventMap
        {
            { AccessibilityEvent::focusChanged,         UIA_AutomationFocusChangedEventId },
            { AccessibilityEvent::windowOpened,         UIA_Window_WindowOpenedEventId },
            { AccessibilityEvent::windowClosed,         UIA_Window_WindowClosedEventId },
            { AccessibilityEvent::textSelectionChanged, UIA_Text_TextSelectionChangedEventId },
            { AccessibilityEvent::textChanged,          UIA_Text_TextChangedEventId }
        };

        const auto it = eventMap.find (event);
        return it != eventMap.cend() ? it->second : EVENTID{};
    }

    AccessibilityNativeHandle* accessibilityElement = nullptr;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AccessibilityNativeImpl)
};

std::unique_ptr<AccessibilityHandler::Pimpl> AccessibilityHandler::createPimpl()
{
    return std::make_unique<AccessibilityHandler::AccessibilityNativeImpl> (*this);
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
        if (isStartingUpOrShuttingDown())
            return false;

        if (auto* wrapper = WindowsUIAWrapper::getInstance())
        {
            auto* provider = static_cast<IRawElementProviderSimple*> (handler->getNativeImplementation());

            if (! wrapper->isProviderDisconnecting (provider))
                *res = wrapper->returnRawElementProvider ((HWND) handler->getWindow(), wParam, lParam, provider);

            provider->Release();

            return true;
        }

        return false;
    }

    void revokeUIAMapEntriesForWindow (HWND hwnd)
    {
        if (auto* wrapper = WindowsUIAWrapper::getInstance())
            wrapper->returnRawElementProvider (hwnd, 0, 0, nullptr);
    }
}


JUCE_IMPLEMENT_SINGLETON (WindowsUIAWrapper)

} // namespace juce
