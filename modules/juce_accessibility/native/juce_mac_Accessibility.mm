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
class AccessibilityHandler::AccessibilityNativeImpl  : public AccessibilityHandler::Pimpl
{
public:
    explicit AccessibilityNativeImpl (AccessibilityHandler& owner)
    {
        static AccessibilityNativeHandle cls;

        SEL selector = NSSelectorFromString (@"initWithAccessibilityHandler:");
        nativeHandle = [cls.createInstance() performSelector: selector withObject: (id) &owner];
    }

    ~AccessibilityNativeImpl() override
    {
        [nativeHandle detach];
        [nativeHandle release];
    }

    AccessibilityNativeHandle* getNativeImplementation() const noexcept override
    {
        return (AccessibilityNativeHandle*) nativeHandle;
    }

    void notifyAccessibilityEvent (AccessibilityEvent eventType) const override
    {
        NSAccessibilityPostNotification (nativeHandle, eventToNotification (eventType));
    }

private:
    //==============================================================================
    static NSAccessibilityNotificationName eventToNotification (AccessibilityEvent event)
    {
        static const std::map<AccessibilityEvent, NSAccessibilityNotificationName> eventMap
        {
            { AccessibilityEvent::elementCreated,       NSAccessibilityCreatedNotification },
            { AccessibilityEvent::elementDestroyed,     NSAccessibilityUIElementDestroyedNotification },
            { AccessibilityEvent::focusChanged,         NSAccessibilityFocusedUIElementChangedNotification },
            { AccessibilityEvent::windowOpened,         NSAccessibilityWindowCreatedNotification },
            { AccessibilityEvent::textSelectionChanged, NSAccessibilitySelectedTextChangedNotification },
            { AccessibilityEvent::textChanged,          NSAccessibilityValueChangedNotification },
            { AccessibilityEvent::rowSelectionChanged,  NSAccessibilitySelectedRowsChangedNotification }
        };

        const auto it = eventMap.find (event);
        return it != eventMap.end() ? it->second : NSAccessibilityNotificationName{};
    }

    id nativeHandle = nil;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AccessibilityNativeImpl)
};

std::unique_ptr<AccessibilityHandler::Pimpl> AccessibilityHandler::createPimpl()
{
    return std::make_unique<AccessibilityHandler::AccessibilityNativeImpl> (*this);
}

} // namespace juce
