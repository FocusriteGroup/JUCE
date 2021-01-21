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

namespace FocusHelpers
{
    static int getOrder (const Component* c)
    {
        auto order = c->getExplicitFocusOrder();
        return order > 0 ? order : (std::numeric_limits<int>::max() / 2);
    }

    static void findAllFocusableComponents (Component* container, std::vector<Component*>& comps)
    {
        if (container->getNumChildComponents() == 0)
            return;

        std::vector<Component*> localComps;

        for (auto* c : container->getChildren())
            if (c->isVisible() && c->isEnabled())
                localComps.push_back (c);

        const auto compareComponents = [&] (const Component* a, const Component* b)
        {
            const auto getComponentOrderAttributes = [] (const Component* c)
            {
                return std::make_tuple (getOrder (c), c->getY(), c->getX());
            };

            return getComponentOrderAttributes (a) < getComponentOrderAttributes (b);
        };

        // This will sort so that they are ordered in terms of left-to-right
        // and then top-to-bottom.
        std::stable_sort (localComps.begin(), localComps.end(), compareComponents);

        for (auto* c : localComps)
        {
            comps.push_back (c);

            if (! c->isFocusContainer())
                findAllFocusableComponents (c, comps);
        }
    }

    enum class NavigationDirection { forwards, backwards };

    Component* navigateFocus (Component* current, NavigationDirection direction)
    {
        if (auto* focusContainer = current->findFocusContainer())
        {
            std::vector<Component*> comps;
            findAllFocusableComponents (focusContainer, comps);

            const auto iter = std::find (comps.cbegin(), comps.cend(), current);

            if (iter == comps.cend())
                return nullptr;

            switch (direction)
            {
                case NavigationDirection::forwards:
                    if (iter != std::prev (comps.cend()))
                        return *std::next (iter);

                    break;
                case NavigationDirection::backwards:
                    if (iter != comps.cbegin())
                        return *std::prev (iter);

                    break;
                default:
                    break;
            }
        }

        return nullptr;
    }
}

//==============================================================================
Component* FocusTraverser::getNextComponent (Component* current)
{
    jassert (current != nullptr);
    return FocusHelpers::navigateFocus (current, FocusHelpers::NavigationDirection::forwards);
}

Component* FocusTraverser::getPreviousComponent (Component* current)
{
    jassert (current != nullptr);
    return FocusHelpers::navigateFocus (current, FocusHelpers::NavigationDirection::backwards);
}

Component* FocusTraverser::getDefaultComponent (Component* parentComponent)
{
    if (parentComponent != nullptr)
    {
        std::vector<Component*> comps;
        FocusHelpers::findAllFocusableComponents (parentComponent, comps);

        if (! comps.empty())
            return comps.front();
    }

    return nullptr;
}

} // namespace juce
