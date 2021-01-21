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
class UIARangeValueProvider  : public ComBaseClassHelper<IRangeValueProvider>
{
public:
    UIARangeValueProvider (AccessibilityHandler& handler)  : accessibilityHandler (handler)  {}

    JUCE_COMRESULT SetValue (double val) override
    {
        if (! isValid (accessibilityHandler))
            return UIA_E_ELEMENTNOTAVAILABLE;

        if (auto* rangedValueInterface = getRangedValueInterface())
        {
            if (val < rangedValueInterface->getMinimumValue()
                || val > rangedValueInterface->getMaximumValue())
            {
                return E_INVALIDARG;
            }

            if (! rangedValueInterface->isReadOnly())
            {
                rangedValueInterface->setValue (val);
                return S_OK;
            }
        }

        return UIA_E_NOTSUPPORTED;
    }

    JUCE_COMRESULT get_Value (double* pRetVal) override
    {
        if (pRetVal == nullptr)
            return E_INVALIDARG;

        if (! isValid (accessibilityHandler))
            return UIA_E_ELEMENTNOTAVAILABLE;

        if (auto* rangedValueInterface = getRangedValueInterface())
        {
            *pRetVal = rangedValueInterface->getCurrentValue();
            return S_OK;
        }

        return UIA_E_NOTSUPPORTED;
    }

    JUCE_COMRESULT get_IsReadOnly (BOOL* pRetVal) override
    {
        if (pRetVal == nullptr)
            return E_INVALIDARG;

        *pRetVal = false;

        if (! isValid (accessibilityHandler))
            return UIA_E_ELEMENTNOTAVAILABLE;

        if (auto* rangedValueInterface = getRangedValueInterface())
        {
            *pRetVal = rangedValueInterface->isReadOnly();
            return S_OK;
        }

        return UIA_E_NOTSUPPORTED;
    }

    JUCE_COMRESULT get_Maximum (double* pRetVal)
    {
        if (pRetVal == nullptr)
            return E_INVALIDARG;

        if (! isValid (accessibilityHandler))
            return UIA_E_ELEMENTNOTAVAILABLE;

        if (auto* rangedValueInterface = getRangedValueInterface())
        {
            *pRetVal = rangedValueInterface->getMaximumValue();
            return S_OK;
        }

        return UIA_E_NOTSUPPORTED;
    }

    JUCE_COMRESULT get_Minimum (double* pRetVal)
    {
        if (pRetVal == nullptr)
            return E_INVALIDARG;

        if (! isValid (accessibilityHandler))
            return UIA_E_ELEMENTNOTAVAILABLE;

        if (auto* rangedValueInterface = getRangedValueInterface())
        {
            *pRetVal = rangedValueInterface->getMinimumValue();
            return S_OK;
        }

        return UIA_E_NOTSUPPORTED;
    }

    JUCE_COMRESULT get_LargeChange (double* pRetVal)
    {
        return get_SmallChange (pRetVal);
    }

    JUCE_COMRESULT get_SmallChange (double* pRetVal)
    {
        if (pRetVal == nullptr)
            return E_INVALIDARG;

        if (! isValid (accessibilityHandler))
            return UIA_E_ELEMENTNOTAVAILABLE;

        if (auto* rangedValueInterface = getRangedValueInterface())
        {
            *pRetVal = rangedValueInterface->getStepSize();
            return S_OK;
        }

        return UIA_E_NOTSUPPORTED;
    }

private:
    //==============================================================================
    ValueInterface* getRangedValueInterface() const noexcept
    {
        auto* valueInterface = accessibilityHandler.getValueInterface();

        if (valueInterface != nullptr && valueInterface->isRanged())
            return valueInterface;

        return nullptr;
    }

    AccessibilityHandler& accessibilityHandler;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UIARangeValueProvider)
};

} // namespace juce
