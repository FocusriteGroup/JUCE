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

static AccessibilityActions addFocusAction (Component& component, AccessibilityActions actions)
{
    if (component.getWantsKeyboardFocus())
        return actions.addAction (AccessibilityActionType::focus, [&component] { component.grabKeyboardFocus(); });

    return actions;
}

ComponentAccessibilityHandler::ComponentAccessibilityHandler (Component& comp,
                                                              AccessibilityRole role,
                                                              AccessibilityActions actions,
                                                              std::unique_ptr<ValueInterface> valueInterface,
                                                              std::unique_ptr<TextInterface> textInterface,
                                                              std::unique_ptr<TableInterface> tableInterface,
                                                              std::unique_ptr<CellInterface> cellInterface)
    : AccessibilityHandler (role,
                            addFocusAction (comp, actions),
                            std::move (valueInterface),
                            std::move (textInterface),
                            std::move (tableInterface),
                            std::move (cellInterface)),
      component (comp)
{
    notifyAccessibilityEvent (AccessibilityEvent::elementCreated);
}

ComponentAccessibilityHandler::~ComponentAccessibilityHandler()
{
    notifyAccessibilityEvent (AccessibilityEvent::elementDestroyed);
}

AccessibleState ComponentAccessibilityHandler::getCurrentState() const
{
    AccessibleState componentState;

    if (! component.isEnabled())            componentState = componentState.withDisabled();
    if (! component.isShowing())            componentState = componentState.withInvisible();
    if (component.hasKeyboardFocus (true))  componentState = componentState.withFocused();

    if (component.isCurrentlyModal())
        componentState = componentState.withModal().withFocused();
    else if (component.isCurrentlyBlockedByAnotherModalComponent())
        componentState = componentState.withBlockedByModal();

    return componentState;
}

const AccessibilityHandler* ComponentAccessibilityHandler::getParent() const
{
    if (auto* parent = component.getParentComponent())
        return parent->getAccessibilityHandler();

    return nullptr;
}

std::vector<const AccessibilityHandler*> ComponentAccessibilityHandler::getChildren() const
{
    std::vector<const AccessibilityHandler*> children;
    children.reserve ((size_t) component.getNumChildComponents());

    for (auto* child : component.getChildren())
        if (child != nullptr)
            children.push_back (child->getAccessibilityHandler());

    return children;
}

std::vector<const AccessibilityHandler*> ComponentAccessibilityHandler::getChildrenInNavigationOrder() const
{
    return getChildren();
}

const AccessibilityHandler* ComponentAccessibilityHandler::getChildAt (Point<int> screenPoint) const
{
    if (auto* comp = Desktop::getInstance().findComponentAt (screenPoint))
        if (component.isParentOf (comp))
            return comp->getAccessibilityHandler();

    return nullptr;
}

const AccessibilityHandler* ComponentAccessibilityHandler::getFocusedChild() const
{
    for (auto* child : getChildrenInNavigationOrder())
    {
        if (child != nullptr && child->getCurrentState().isFocused())
            return child;
    }

    return nullptr;
}

const AccessibilityHandler* ComponentAccessibilityHandler::getRoot() const
{
    if (component.isOnDesktop())
        return this;

    if (auto* peer = component.getPeer())
        return peer->getComponent().getAccessibilityHandler();

    return nullptr;
}

const AccessibilityHandler* ComponentAccessibilityHandler::getModal() const
{
    if (auto* modal = Component::getCurrentlyModalComponent())
        return modal->getAccessibilityHandler();

    return nullptr;
}

bool ComponentAccessibilityHandler::isRoot() const
{
    return component.isOnDesktop();
}

Rectangle<int> ComponentAccessibilityHandler::getScreenBounds() const
{
    return component.getScreenBounds();
}

Rectangle<int> ComponentAccessibilityHandler::convertFromNativeBounds (const Rectangle<int>& nativeRect) const
{
    return Desktop::getInstance().getDisplays().physicalToLogical (nativeRect);
}

Rectangle<int> ComponentAccessibilityHandler::convertToNativeBounds (const Rectangle<int>& juceRect) const
{
    return Desktop::getInstance().getDisplays().logicalToPhysical (juceRect);
}

Point<int> ComponentAccessibilityHandler::convertFromNativePoint (const Point<int>& nativePoint) const
{
    return Desktop::getInstance().getDisplays().physicalToLogical (nativePoint);
}

Point<int> ComponentAccessibilityHandler::convertToNativePoint (const Point<int>& jucePoint) const
{
    return Desktop::getInstance().getDisplays().logicalToPhysical (jucePoint);
}

void* ComponentAccessibilityHandler::getWindow() const
{
    if (auto* peer = component.getPeer())
        return peer->getNativeHandle();

    return nullptr;
}

} // namespace juce
