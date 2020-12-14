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

JUCE_COMCLASS (ISelectionProvider2, "14f68475-ee1c-44f6-a869-d239381f0fe7")  : public ISelectionProvider
{
    JUCE_COMCALL get_FirstSelectedItem (IRawElementProviderSimple** retVal) = 0;
    JUCE_COMCALL get_LastSelectedItem (IRawElementProviderSimple** retVal) = 0;
    JUCE_COMCALL get_CurrentSelectedItem (IRawElementProviderSimple** retVal) = 0;
    JUCE_COMCALL get_ItemCount (int* retVal) = 0;
};

//==============================================================================
class UIASelectionItemProvider  : public ComBaseClassHelper<ISelectionItemProvider>
{
public:
    UIASelectionItemProvider (AccessibilityHandler& handler)  : accessibilityHandler (handler)  {}

    //==============================================================================
    JUCE_COMRESULT AddToSelection() override
    {
        if (! isValid (accessibilityHandler))
            return UIA_E_ELEMENTNOTAVAILABLE;

        if (auto actionCallback = accessibilityHandler.getActions().get (AccessibilityActionType::select))
        {
            actionCallback();
            return S_OK;
        }

        return UIA_E_NOTSUPPORTED;
    }

    JUCE_COMRESULT get_IsSelected (BOOL* pRetVal) override
    {
        if (pRetVal == nullptr)
            return E_INVALIDARG;

        *pRetVal = false;

        if (! isValid (accessibilityHandler))
            return UIA_E_ELEMENTNOTAVAILABLE;

        *pRetVal = accessibilityHandler.getCurrentState().isSelected();

        return S_OK;
    }

    JUCE_COMRESULT get_SelectionContainer (IRawElementProviderSimple** pRetVal) override
    {
        if (pRetVal == nullptr)
            return E_INVALIDARG;

        *pRetVal = nullptr;

        if (! isValid (accessibilityHandler))
            return UIA_E_ELEMENTNOTAVAILABLE;

        auto* parentHandler = accessibilityHandler.getParent();

        while (parentHandler != nullptr)
        {
            if (parentHandler->getRole() == AccessibilityRole::list)
            {
                *pRetVal = static_cast<IRawElementProviderSimple*> (parentHandler->getNativeImplementation());
                break;
            }

            parentHandler = parentHandler->getParent();
        }

        return S_OK;
    }

    JUCE_COMRESULT RemoveFromSelection() override
    {
        if (! isValid (accessibilityHandler))
            return UIA_E_ELEMENTNOTAVAILABLE;

        if (auto actionCallback = accessibilityHandler.getActions().get (AccessibilityActionType::deselect))
        {
            actionCallback();
            return S_OK;
        }

        return UIA_E_NOTSUPPORTED;
    }

    JUCE_COMRESULT Select() override
    {
        if (! isValid (accessibilityHandler))
            return UIA_E_ELEMENTNOTAVAILABLE;

        if (auto actionCallback = accessibilityHandler.getActions().get (AccessibilityActionType::select))
        {
            if (! accessibilityHandler.getCurrentState().isSelected())
                actionCallback();

            return S_OK;
        }

        return UIA_E_NOTSUPPORTED;
    }

private:
    AccessibilityHandler& accessibilityHandler;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UIASelectionItemProvider)
};

//==============================================================================
class UIASelectionProvider  : public ComBaseClassHelper<ISelectionProvider2>
{
public:
    UIASelectionProvider (AccessibilityHandler& handler)  : accessibilityHandler (handler)  {}

    //==============================================================================
    JUCE_COMRESULT QueryInterface (REFIID iid, void** result)
    {
        if (iid == _uuidof (IUnknown) || iid == _uuidof (ISelectionProvider))
            return castToType<ISelectionProvider> (result);

        if (iid == _uuidof (ISelectionProvider2))
            return castToType<ISelectionProvider2> (result);

        *result = nullptr;
        return E_NOINTERFACE;
    }

    //==============================================================================
    JUCE_COMRESULT get_CanSelectMultiple (BOOL* pRetVal) override
    {
        if (pRetVal == nullptr)
            return E_INVALIDARG;

        *pRetVal = false;

        if (! isValid (accessibilityHandler))
            return UIA_E_ELEMENTNOTAVAILABLE;

        *pRetVal = isMultiSelectable();

        return S_OK;
    }

    JUCE_COMRESULT get_IsSelectionRequired (BOOL* pRetVal) override
    {
        if (pRetVal == nullptr)
            return E_INVALIDARG;

        *pRetVal = false;

        if (! isValid (accessibilityHandler))
            return UIA_E_ELEMENTNOTAVAILABLE;

        *pRetVal = getSelectedChildren().size() > 0 && ! isMultiSelectable();
        return S_OK;
    }

    JUCE_COMRESULT GetSelection (SAFEARRAY** pRetVal) override
    {
        if (pRetVal == nullptr)
            return E_INVALIDARG;

        *pRetVal = nullptr;

        if (! isValid (accessibilityHandler))
            return UIA_E_ELEMENTNOTAVAILABLE;

        return addHandlersToArray (getSelectedChildren(), pRetVal);
    }

    //==============================================================================
    JUCE_COMRESULT get_FirstSelectedItem (IRawElementProviderSimple** pRetVal) override
    {
        if (pRetVal == nullptr)
            return E_INVALIDARG;

        *pRetVal = nullptr;

        if (! isValid (accessibilityHandler))
            return UIA_E_ELEMENTNOTAVAILABLE;

        auto selectedChildren = getSelectedChildren();

        if (! selectedChildren.empty())
            *pRetVal = static_cast<IRawElementProviderSimple*> (selectedChildren.front()->getNativeImplementation());

        return S_OK;
    }

    JUCE_COMRESULT get_LastSelectedItem (IRawElementProviderSimple** pRetVal) override
    {
        if (pRetVal == nullptr)
            return E_INVALIDARG;

        *pRetVal = nullptr;

        if (! isValid (accessibilityHandler))
            return UIA_E_ELEMENTNOTAVAILABLE;

        auto selectedChildren = getSelectedChildren();

        if (! selectedChildren.empty())
            *pRetVal = static_cast<IRawElementProviderSimple*> (selectedChildren.back()->getNativeImplementation());

        return S_OK;
    }

    JUCE_COMRESULT get_CurrentSelectedItem (IRawElementProviderSimple** pRetVal) override
    {
        if (pRetVal == nullptr)
            return E_INVALIDARG;

        *pRetVal = nullptr;

        if (! isValid (accessibilityHandler))
            return UIA_E_ELEMENTNOTAVAILABLE;

        get_FirstSelectedItem (pRetVal);

        return S_OK;
    }

    JUCE_COMRESULT get_ItemCount (int* pRetVal) override
    {
        if (pRetVal == nullptr)
            return E_INVALIDARG;

        *pRetVal = 0;

        if (! isValid (accessibilityHandler))
            return UIA_E_ELEMENTNOTAVAILABLE;

        *pRetVal = (int) getSelectedChildren().size();

        return S_OK;
    }

private:
    bool isMultiSelectable() const noexcept
    {
         return accessibilityHandler.getActions().contains (AccessibilityActionType::multiSelect);
    }

    std::vector<const AccessibilityHandler*> getSelectedChildren() const noexcept
    {
        std::vector<const AccessibilityHandler*> selectedHandlers;

        for (auto* handler : accessibilityHandler.getChildrenInNavigationOrder())
            if (handler->getCurrentState().isSelected())
                selectedHandlers.push_back (handler);

        return selectedHandlers;
    }

    AccessibilityHandler& accessibilityHandler;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UIASelectionProvider)
};

} // namespace juce
