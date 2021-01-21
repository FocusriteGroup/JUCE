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
class UIAGridProvider  : public ComBaseClassHelper<IGridProvider>
{
public:
    UIAGridProvider (AccessibilityHandler& handler)  : accessibilityHandler (handler)  {}

    //==============================================================================
    JUCE_COMRESULT GetItem (int row, int column, IRawElementProviderSimple** pRetVal) override
    {
        if (pRetVal == nullptr)
            return E_INVALIDARG;

        *pRetVal = nullptr;

        if (! isValid (accessibilityHandler))
            return UIA_E_ELEMENTNOTAVAILABLE;

        if (auto* tableInterface = accessibilityHandler.getTableInterface())
        {
            if (! isPositiveAndBelow (row, tableInterface->getNumRows())
                || ! isPositiveAndBelow (column, tableInterface->getNumColumns()))
                return E_INVALIDARG;

            if (auto* handler = tableInterface->getCellHandler (row, column))
                handler->getNativeImplementation()->QueryInterface (IID_PPV_ARGS (pRetVal));

            return S_OK;
        }

        return UIA_E_NOTSUPPORTED;
    }

    JUCE_COMRESULT get_RowCount (int* pRetVal) override
    {
        if (pRetVal == nullptr)
            return E_INVALIDARG;

        *pRetVal = 0;

        if (! isValid (accessibilityHandler))
            return UIA_E_ELEMENTNOTAVAILABLE;

        if (auto* tableInterface = accessibilityHandler.getTableInterface())
        {
            *pRetVal = tableInterface->getNumRows();
            return S_OK;
        }

        return UIA_E_NOTSUPPORTED;
    }

    JUCE_COMRESULT get_ColumnCount (int* pRetVal) override
    {
        if (pRetVal == nullptr)
            return E_INVALIDARG;

        *pRetVal = 0;

        if (! isValid (accessibilityHandler))
            return UIA_E_ELEMENTNOTAVAILABLE;

        if (auto* tableInterface = accessibilityHandler.getTableInterface())
        {
            *pRetVal = tableInterface->getNumColumns();
            return S_OK;
        }

        return UIA_E_NOTSUPPORTED;
    }

private:
    AccessibilityHandler& accessibilityHandler;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UIAGridProvider)
};

} // namespace juce
