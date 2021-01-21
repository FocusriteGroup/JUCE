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

class WindowsUIAWrapper  : public DeletedAtShutdown
{
public:
    bool isLoaded() const noexcept  { return loaded; }

    //==============================================================================
    LRESULT returnRawElementProvider (HWND hwnd, WPARAM wParam, LPARAM lParam, IRawElementProviderSimple* provider)
    {
        if (uiaReturnRawElementProvider != nullptr)
            return uiaReturnRawElementProvider (hwnd, wParam, lParam, provider);

        return (LRESULT) nullptr;
    }

    JUCE_COMRESULT hostProviderFromHwnd (HWND hwnd, IRawElementProviderSimple** provider)
    {
        if (uiaHostProviderFromHwnd != nullptr)
            return uiaHostProviderFromHwnd (hwnd, provider);

        return UIA_E_NOTSUPPORTED;
    }

    JUCE_COMRESULT raiseAutomationPropertyChangedEvent (IRawElementProviderSimple* provider, PROPERTYID propID, VARIANT oldValue, VARIANT newValue)
    {
        if (uiaRaiseAutomationPropertyChangedEvent != nullptr)
            return uiaRaiseAutomationPropertyChangedEvent (provider, propID, oldValue, newValue);

        return UIA_E_NOTSUPPORTED;
    }

    JUCE_COMRESULT raiseAutomationEvent (IRawElementProviderSimple* provider, EVENTID eventID)
    {
        if (uiaRaiseAutomationEvent != nullptr)
            return uiaRaiseAutomationEvent (provider, eventID);

        return UIA_E_NOTSUPPORTED;
    }

    BOOL clientsAreListening()
    {
        if (uiaClientsAreListening != nullptr)
            return uiaClientsAreListening();

        return false;
    }

    JUCE_COMRESULT disconnectProvider (IRawElementProviderSimple* provider)
    {
        if (uiaDisconnectProvider != nullptr)
        {
            const ScopedValueSetter<IRawElementProviderSimple*> disconnectingProviderSetter (disconnectingProvider, provider);
            return uiaDisconnectProvider (provider);
        }

        return UIA_E_NOTSUPPORTED;
    }

    JUCE_COMRESULT disconnectAllProviders()
    {
        if (uiaDisconnectAllProviders != nullptr)
        {
            const ScopedValueSetter<bool> disconnectingAllProvidersSetter (disconnectingAllProviders, true);
            return uiaDisconnectAllProviders();
        }

        return UIA_E_NOTSUPPORTED;
    }

    //==============================================================================
    bool isProviderDisconnecting (IRawElementProviderSimple* provider)
    {
        return disconnectingProvider == provider || disconnectingAllProviders;
    }

    //==============================================================================
    JUCE_DECLARE_SINGLETON_SINGLETHREADED_MINIMAL (WindowsUIAWrapper)

private:
    //==============================================================================
    WindowsUIAWrapper()
        : loaded (loadAllUiaFunctions())
    {
        // force UIA COM library initialisation here to prevent an exception when calling methods from SendMessage()
        if (loaded)
            returnRawElementProvider (nullptr, 0, 0, nullptr);
    }

    ~WindowsUIAWrapper()
    {
        disconnectAllProviders();

        if (uiaHandle != nullptr)
            ::FreeLibrary (uiaHandle);

        clearSingletonInstance();
    }

    //==============================================================================
    template<typename FuncType>
    static FuncType getUiaFunction (HMODULE module, StringRef funcName)
    {
        return (FuncType) GetProcAddress (module, funcName);
    }

    bool loadAllUiaFunctions()
    {
        uiaHandle = ::LoadLibraryA ("UIAutomationCore.dll");

        if (uiaHandle != nullptr)
        {
            uiaReturnRawElementProvider            = getUiaFunction<UiaReturnRawElementProviderFunc>            (uiaHandle, "UiaReturnRawElementProvider");
            uiaHostProviderFromHwnd                = getUiaFunction<UiaHostProviderFromHwndFunc>                (uiaHandle, "UiaHostProviderFromHwnd");
            uiaRaiseAutomationPropertyChangedEvent = getUiaFunction<UiaRaiseAutomationPropertyChangedEventFunc> (uiaHandle, "UiaRaiseAutomationPropertyChangedEvent");
            uiaRaiseAutomationEvent                = getUiaFunction<UiaRaiseAutomationEventFunc>                (uiaHandle, "UiaRaiseAutomationEvent");
            uiaClientsAreListening                 = getUiaFunction<UiaClientsAreListeningFunc>                 (uiaHandle, "UiaClientsAreListening");
            uiaDisconnectProvider                  = getUiaFunction<UiaDisconnectProviderFunc>                  (uiaHandle, "UiaDisconnectProvider");
            uiaDisconnectAllProviders              = getUiaFunction<UiaDisconnectAllProvidersFunc>              (uiaHandle, "UiaDisconnectAllProviders");
        }
        else
        {
            // UIAutomationCore could not be loaded!
            jassertfalse;
        }

        return uiaReturnRawElementProvider != nullptr
              && uiaHostProviderFromHwnd != nullptr
              && uiaRaiseAutomationPropertyChangedEvent != nullptr
              && uiaRaiseAutomationEvent != nullptr
              && uiaClientsAreListening != nullptr
              && uiaDisconnectProvider != nullptr
              && uiaDisconnectAllProviders != nullptr;
    }

    //==============================================================================
    using UiaReturnRawElementProviderFunc            = LRESULT (WINAPI*) (HWND, WPARAM, LPARAM, IRawElementProviderSimple*);
    using UiaHostProviderFromHwndFunc                = HRESULT (WINAPI*) (HWND, IRawElementProviderSimple**);
    using UiaRaiseAutomationPropertyChangedEventFunc = HRESULT (WINAPI*) (IRawElementProviderSimple*, PROPERTYID, VARIANT, VARIANT);
    using UiaRaiseAutomationEventFunc                = HRESULT (WINAPI*) (IRawElementProviderSimple*, EVENTID);
    using UiaClientsAreListeningFunc                 = BOOL    (WINAPI*) ();
    using UiaDisconnectProviderFunc                  = HRESULT (WINAPI*) (IRawElementProviderSimple*);
    using UiaDisconnectAllProvidersFunc              = HRESULT (WINAPI*) ();

    UiaReturnRawElementProviderFunc            uiaReturnRawElementProvider            = nullptr;
    UiaHostProviderFromHwndFunc                uiaHostProviderFromHwnd                = nullptr;
    UiaRaiseAutomationPropertyChangedEventFunc uiaRaiseAutomationPropertyChangedEvent = nullptr;
    UiaRaiseAutomationEventFunc                uiaRaiseAutomationEvent                = nullptr;
    UiaClientsAreListeningFunc                 uiaClientsAreListening                 = nullptr;
    UiaDisconnectProviderFunc                  uiaDisconnectProvider                  = nullptr;
    UiaDisconnectAllProvidersFunc              uiaDisconnectAllProviders              = nullptr;

    HMODULE uiaHandle = nullptr;
    const bool loaded;

    IRawElementProviderSimple* disconnectingProvider = nullptr;
    bool disconnectingAllProviders = false;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WindowsUIAWrapper)
};

} // namespace juce
