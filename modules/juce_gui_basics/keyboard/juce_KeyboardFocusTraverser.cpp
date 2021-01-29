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
    static Component* findComponent (ComponentTraverser& traverser, Component* currentComponent, Component* parentComponent)
    {
        if (auto* comp = traverse<direction> (traverser, currentComponent))
        {
            if (comp->getWantsKeyboardFocus() && parentComponent->isParentOf (comp))
                return comp;

            return findComponent<direction> (traverser, comp, parentComponent);
        }

        return nullptr;
    }

    template <FocusHelpers::NavigationDirection direction>
    static Component* getComponent (Component* current)
    {
        jassert (current != nullptr);

        if (auto focusTraverser = current->createFocusTraverser())
            return findComponent<direction> (*focusTraverser, current, current->findFocusContainer());

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

            return KeyboardFocusHelpers::findComponent<FocusHelpers::NavigationDirection::forwards> (*focusTraverser,
                                                                                                     defaultComponent,
                                                                                                     parentComponent);
        }
    }

    return nullptr;
}

std::vector<Component*> KeyboardFocusTraverser::getAllComponents (Component* parentComponent)
{
    std::vector<Component*> components;

    if (auto focusTraverser = parentComponent->createFocusTraverser())
    {
        auto* current = getDefaultComponent (parentComponent);

        while (current != nullptr)
        {
            components.push_back (current);

            current = KeyboardFocusHelpers::findComponent<FocusHelpers::NavigationDirection::forwards> (*focusTraverser,
                                                                                                        current,
                                                                                                        parentComponent);
        }
    }

    return components;
}


//==============================================================================
//==============================================================================
#if JUCE_UNIT_TESTS

struct KeyboardFocusTraverserTests  : public UnitTest
{
    KeyboardFocusTraverserTests()
        : UnitTest ("KeyboardFocusTraverser", UnitTestCategories::gui)
    {}

    void runTest() override
    {
        ScopedJuceInitialiser_GUI libraryInitialiser;

        beginTest ("No child wants keyboard focus");
        {
            TestComponent parent;

            expect (traverser.getDefaultComponent (&parent) == nullptr);
            expect (traverser.getAllComponents (&parent).empty());
        }

        beginTest ("Single child wants keyboard focus");
        {
            TestComponent parent;

            parent.children[5].setWantsKeyboardFocus (true);

            auto* defaultComponent = traverser.getDefaultComponent (&parent);

            expect (defaultComponent == &parent.children[5]);
            expect (defaultComponent->getWantsKeyboardFocus());

            expect (traverser.getNextComponent (defaultComponent) == nullptr);
            expect (traverser.getPreviousComponent (defaultComponent) == nullptr);
            expect (traverser.getAllComponents (&parent).size() == 1);
        }

        beginTest ("Multiple children want keyboard focus");
        {
            TestComponent parent;

            std::array<Component*, 6> focusChildren
            {
                &parent.children[1],
                &parent.children[9],
                &parent.children[3],
                &parent.children[5],
                &parent.children[8],
                &parent.children[0]
            };

            for (auto* focusChild : focusChildren)
                focusChild->setWantsKeyboardFocus (true);

            auto allComponents = traverser.getAllComponents (&parent);

            for (auto* focusChild : focusChildren)
                expect (std::find (allComponents.cbegin(), allComponents.cend(), focusChild) != allComponents.cend());

            auto* componentToTest = traverser.getDefaultComponent (&parent);

            for (;;)
            {
                expect (componentToTest->getWantsKeyboardFocus());
                expect (std::find (focusChildren.cbegin(), focusChildren.cend(), componentToTest) != focusChildren.cend());

                componentToTest = traverser.getNextComponent (componentToTest);

                if (componentToTest == nullptr)
                    break;
            }

            int focusOrder = 1;
            for (auto* focusChild : focusChildren)
                focusChild->setExplicitFocusOrder (focusOrder++);

            componentToTest = traverser.getDefaultComponent (&parent);

            for (auto* focusChild : focusChildren)
            {
                expect (componentToTest == focusChild);
                expect (componentToTest->getWantsKeyboardFocus());

                componentToTest = traverser.getNextComponent (componentToTest);
            }
        }

        beginTest ("Single nested child wants keyboard focus");
        {
            TestComponent parent;
            Component grandparent;

            grandparent.addAndMakeVisible (parent);

            auto& focusChild = parent.children[5];

            focusChild.setWantsKeyboardFocus (true);

            expect (traverser.getDefaultComponent (&grandparent) == &focusChild);
            expect (traverser.getDefaultComponent (&parent) == &focusChild);
            expect (traverser.getNextComponent (&focusChild) == nullptr);
            expect (traverser.getPreviousComponent (&focusChild) == nullptr);
            expect (traverser.getAllComponents (&parent).size() == 1);
        }

        beginTest ("Multiple nested children want keyboard focus");
        {
            TestComponent parent;
            Component grandparent;

            grandparent.addAndMakeVisible (parent);

            std::array<Component*, 3> focusChildren
            {
                &parent.children[1],
                &parent.children[4],
                &parent.children[5]
            };

            for (auto* focusChild : focusChildren)
                focusChild->setWantsKeyboardFocus (true);

            auto allComponents = traverser.getAllComponents (&parent);

            expect (std::equal (allComponents.cbegin(), allComponents.cend(), focusChildren.cbegin(),
                                [] (const Component* c1, const Component* c2) { return c1 == c2; }));

            expect (traverser.getDefaultComponent (&grandparent) == focusChildren.front());
            expect (traverser.getDefaultComponent (&parent) == focusChildren.front());
            expect (traverser.getNextComponent (focusChildren.front()) == *std::next (focusChildren.begin()));
            expect (traverser.getPreviousComponent (focusChildren.back()) == *std::prev (focusChildren.end(), 2));

            std::array<Component, 3> otherParents;

            for (auto& p : otherParents)
            {
                grandparent.addAndMakeVisible (p);
                p.setWantsKeyboardFocus (true);
            }

            expect (traverser.getDefaultComponent (&grandparent) == focusChildren.front());
            expect (traverser.getDefaultComponent (&parent) == focusChildren.front());
            expect (traverser.getNextComponent (focusChildren.back()) == &otherParents.front());
            expect (traverser.getNextComponent (&otherParents.back()) == nullptr);
            expect (traverser.getAllComponents (&grandparent).size() == focusChildren.size() + otherParents.size());
            expect (traverser.getAllComponents (&parent).size() == focusChildren.size());

            for (auto* focusChild : focusChildren)
                focusChild->setWantsKeyboardFocus (false);

            expect (traverser.getDefaultComponent (&grandparent) == &otherParents.front());
            expect (traverser.getDefaultComponent (&parent) == nullptr);
            expect (traverser.getAllComponents (&grandparent).size() == otherParents.size());
            expect (traverser.getAllComponents (&parent).empty());
        }
    }

private:
    struct TestComponent  : public Component
    {
        TestComponent()
        {
            for (auto& child : children)
                addAndMakeVisible (child);
        }

        std::array<Component, 10> children;
    };

    KeyboardFocusTraverser traverser;
};

static KeyboardFocusTraverserTests keyboardFocusTraverserTests;

#endif

} // namespace juce
