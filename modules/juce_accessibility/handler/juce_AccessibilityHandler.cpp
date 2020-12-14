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

AccessibilityHandler::AccessibilityHandler (AccessibilityRole accessibilityRole,
                                            AccessibilityActions accessibilityActions,
                                            std::unique_ptr<ValueInterface> value,
                                            std::unique_ptr<TextInterface> text,
                                            std::unique_ptr<TableInterface> table,
                                            std::unique_ptr<CellInterface> cell)
    : role (accessibilityRole),
      actions (std::move (accessibilityActions)),
      valueInterface (std::move (value)),
      textInterface (std::move (text)),
      tableInterface (std::move (table)),
      cellInterface (std::move (cell)),
      pimpl (createPimpl())
{
}

ValueInterface* AccessibilityHandler::getValueInterface() const
{
    return valueInterface.get();
}

TableInterface* AccessibilityHandler::getTableInterface() const
{
    return tableInterface.get();
}

CellInterface* AccessibilityHandler::getCellInterface() const
{
    return cellInterface.get();
}

TextInterface* AccessibilityHandler::getTextInterface() const
{
    return textInterface.get();
}

void AccessibilityHandler::notifyAccessibilityEvent (AccessibilityEvent event) const
{
    if (pimpl != nullptr)
        pimpl->notifyAccessibilityEvent (event);
}

AccessibilityNativeHandle* AccessibilityHandler::getNativeImplementation() const
{
    if (pimpl != nullptr)
        return pimpl->getNativeImplementation();

    return nullptr;
}

#if ! (JUCE_MAC || JUCE_WINDOWS)
 std::unique_ptr<AccessibilityHandler::Pimpl> AccessibilityHandler::createPimpl()
 {
     return nullptr;
 }
#endif

} // namespace juce
