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

namespace KeyboardFocusHelpers
{
    template <FocusHelpers::NavigationDirection>
    static Component* traverse (ComponentTraverser&, Component*)
    {
        return nullptr;
    }

    template <>
    Component* traverse<FocusHelpers::NavigationDirection::forwards> (ComponentTraverser& traverser, Component* currentComponent)
    {
        return traverser.getNextComponent (currentComponent);
    }

    template <>
    Component* traverse<FocusHelpers::NavigationDirection::backwards> (ComponentTraverser& traverser, Component* currentComponent)
    {
        return traverser.getPreviousComponent (currentComponent);
    }

    template <FocusHelpers::NavigationDirection direction>
    static Component* findComponent (ComponentTraverser& traverser, Component* currentComponent)
    {
        if (auto* comp = traverse<direction> (traverser, currentComponent))
        {
            if (comp->getWantsKeyboardFocus())
                return comp;

            return findComponent<direction> (traverser, comp);
        }

        return nullptr;
    }

    template <FocusHelpers::NavigationDirection direction>
    static Component* getComponent (Component* current)
    {
        jassert (current != nullptr);

        if (auto focusTraverser = current->createFocusTraverser())
            return findComponent<direction> (*focusTraverser, current);

        return nullptr;
    }
}

//==============================================================================
Component* KeyboardFocusTraverser::getNextComponent (Component* current)
{
    return KeyboardFocusHelpers::getComponent<FocusHelpers::NavigationDirection::forwards> (current);
}

Component* KeyboardFocusTraverser::getPreviousComponent (Component* current)
{
    return KeyboardFocusHelpers::getComponent<FocusHelpers::NavigationDirection::backwards> (current);
}

Component* KeyboardFocusTraverser::getDefaultComponent (Component* parentComponent)
{
    jassert (parentComponent != nullptr);

    if (auto focusTraverser = parentComponent->createFocusTraverser())
    {
        if (auto* defaultComponent = focusTraverser->getDefaultComponent (parentComponent))
        {
            if (defaultComponent->getWantsKeyboardFocus())
                return defaultComponent;

            return KeyboardFocusHelpers::findComponent<FocusHelpers::NavigationDirection::forwards> (*focusTraverser, defaultComponent);
        }
    }

    return nullptr;
}

} // namespace juce
