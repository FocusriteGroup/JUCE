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
class UIAValueProvider  : public ComBaseClassHelper<IValueProvider>
{
public:
    UIAValueProvider (AccessibilityHandler& handler, bool editableText)
        : accessibilityHandler (handler),
          isEditableText (editableText)
    {
    }

    JUCE_COMRESULT SetValue (LPCWSTR val) override
    {
        if (! isValid (accessibilityHandler))
            return UIA_E_ELEMENTNOTAVAILABLE;

        if (isEditableText)
        {
            accessibilityHandler.getTextInterface()->setText (String (val));
            return S_OK;
        }

        auto& valueInterface = *accessibilityHandler.getValueInterface();

        if (valueInterface.isReadOnly())
            return UIA_E_NOTSUPPORTED;

        valueInterface.setValueAsString (String (val));
        return S_OK;
    }

    JUCE_COMRESULT get_Value (BSTR* pRetVal) override
    {
        if (pRetVal == nullptr)
            return E_INVALIDARG;

        *pRetVal = nullptr;

        if (! isValid (accessibilityHandler))
            return UIA_E_ELEMENTNOTAVAILABLE;

        auto currentValueString = [this]
        {
            if (isEditableText)
            {
                auto& textInterface = *accessibilityHandler.getTextInterface();
                return textInterface.getText ({ 0, textInterface.getTotalNumCharacters() });
            }

            return accessibilityHandler.getValueInterface()->getCurrentValueAsString();
        }();

        *pRetVal = SysAllocString ((const OLECHAR*) currentValueString.toWideCharPointer());
        return S_OK;
    }

    JUCE_COMRESULT get_IsReadOnly (BOOL* pRetVal) override
    {
        if (pRetVal == nullptr)
            return E_INVALIDARG;

        *pRetVal = false;

        if (! isValid (accessibilityHandler))
            return UIA_E_ELEMENTNOTAVAILABLE;

        *pRetVal = (isEditableText ? false
                                   : accessibilityHandler.getValueInterface()->isReadOnly());

        return S_OK;
    }

private:
    AccessibilityHandler& accessibilityHandler;
    const bool isEditableText;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UIAValueProvider)
};

} // namespace juce
