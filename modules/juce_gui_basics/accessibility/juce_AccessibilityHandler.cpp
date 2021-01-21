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

AccessibilityHandler::AccessibilityHandler (Component& comp,
                                            AccessibilityRole accessibilityRole,
                                            AccessibilityActions accessibilityActions,
                                            std::unique_ptr<ValueInterface> value,
                                            std::unique_ptr<TextInterface> text,
                                            std::unique_ptr<TableInterface> table,
                                            std::unique_ptr<CellInterface> cell)
    : component (comp),
      role (accessibilityRole),
      actions (std::move (accessibilityActions)),
      valueInterface (std::move (value)),
      textInterface (std::move (text)),
      tableInterface (std::move (table)),
      cellInterface (std::move (cell)),
      nativeImpl (std::unique_ptr<AccessibilityNativeImpl, decltype (&destroyNativeImpl)> (createNativeImpl (*this),
                                                                                           destroyNativeImpl))
{
    notifyAccessibilityEvent (AccessibilityEvent::elementCreated);
}

AccessibilityHandler::~AccessibilityHandler()
{
    notifyAccessibilityEvent (AccessibilityEvent::elementDestroyed);
}

bool AccessibilityHandler::isFocused() const
{
    return component.hasKeyboardFocus (true) || getCurrentState().isSelected();
}

bool AccessibilityHandler::isIgnored() const
{
    return role == AccessibilityRole::ignored
          || getCurrentState().isIgnored()
          || ! component.isVisible();
}

//==============================================================================
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

//==============================================================================
static AccessibilityHandler* getUnignoredAncestor (AccessibilityHandler* handler)
{
    jassert (handler != nullptr);

    while (handler->isIgnored() && handler->getParent() != nullptr)
        handler = handler->getParent();

    return handler;
}

static AccessibilityHandler* findFirstUnignoredChild (const std::vector<AccessibilityHandler*>& handlers)
{
    if (! handlers.empty())
    {
        const auto iter = std::find_if (handlers.cbegin(), handlers.cend(),
                                        [] (const AccessibilityHandler* handler) { return ! handler->isIgnored(); });

        if (iter != handlers.cend())
            return *iter;

        for (auto* handler : handlers)
            if (auto* unignored = findFirstUnignoredChild (handler->getChildren()))
                return unignored;
    }

    return nullptr;
}

static AccessibilityHandler* getFirstUnignoredDescendant (AccessibilityHandler* handler)
{
    jassert (handler != nullptr);

    if (handler->isIgnored())
        if (auto* firstUnignoredDescendant = findFirstUnignoredChild (handler->getChildren()))
            return firstUnignoredDescendant;

    return handler;
}

AccessibilityHandler* AccessibilityHandler::getParent() const
{
    if (auto* focusContainer = component.findFocusContainer())
    {
        if (auto* handler = focusContainer->getAccessibilityHandler())
            return getUnignoredAncestor (handler);
    }

    return nullptr;
}

std::vector<AccessibilityHandler*> AccessibilityHandler::getChildren() const
{
    if (! component.isFocusContainer() && component.getParentComponent() != nullptr)
        return {};

    std::vector<AccessibilityHandler*> children;

    if (auto traverser = component.createFocusTraverser())
    {
        auto* focusChild = traverser->getDefaultComponent (&component);

        while (focusChild != nullptr)
        {
            if (auto* handler = focusChild->getAccessibilityHandler())
                children.push_back (getFirstUnignoredDescendant (handler));

            focusChild = traverser->getNextComponent (focusChild);
        }
    }

    return children;
}

AccessibilityHandler* AccessibilityHandler::getChildAt (Point<int> screenPoint)
{
    if (auto* comp = Desktop::getInstance().findComponentAt (screenPoint))
        if (component.isParentOf (comp))
            if (auto* childHandler = comp->getAccessibilityHandler())
                return getUnignoredAncestor (childHandler);

    return getUnignoredAncestor (this);
}

AccessibilityHandler* AccessibilityHandler::getFocus()
{
    auto* focusedHandler = [this]() -> AccessibilityHandler*
    {
        if (auto* modal = Component::getCurrentlyModalComponent())
        {
            if (! component.isParentOf (modal)
                 && component.isCurrentlyBlockedByAnotherModalComponent())
            {
                if (auto* modalHandler = modal->getAccessibilityHandler())
                    return modalHandler->getFocus();
            }
        }

        for (auto* child : getChildren())
        {
            if (child->isFocused())
                return child->getFocus();
        }

        return this;
    }();

    return getUnignoredAncestor (focusedHandler);
}

//==============================================================================
#if ! (JUCE_MAC || JUCE_WINDOWS)
class AccessibilityHandler::AccessibilityNativeImpl { public: AccessibilityNativeImpl (AccessibilityHandler&) {} };
void AccessibilityHandler::notifyAccessibilityEvent (AccessibilityEvent) const {}
AccessibilityNativeHandle* AccessibilityHandler::getNativeImplementation() const { return nullptr; }
AccessibilityHandler::AccessibilityNativeImpl* AccessibilityHandler::createNativeImpl (AccessibilityHandler&) { return nullptr; }
void AccessibilityHandler::destroyNativeImpl (AccessibilityHandler::AccessibilityNativeImpl*) {}
#endif

} // namespace juce
