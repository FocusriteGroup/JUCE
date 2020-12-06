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

/** Base class for accessible Components.

    This class wraps a Component and makes it visible to accessibility clients.
    It handles hierarchical navigation, properties, state, and various interfaces.

    @tags{Accessibility}
*/
class ComponentAccessibilityHandler  : public AccessibilityHandler
{
public:
    /** Constructor.

        This will create a ComponentAccessibilityHandler which wraps the given component
        and makes it visible to accessibility clients. You must also specify a role for
        the component from the `AccessibilityRole` list which best describes it.

        You can optionally pass in a list of actions which the component supports
        and interfaces for value, text, table and cell support.
    */
    ComponentAccessibilityHandler (Component& componentToWrap,
                                   AccessibilityRole accessibilityRole,
                                   AccessibilityActions actions = {},
                                   std::unique_ptr<ValueInterface> valueInterface = {},
                                   std::unique_ptr<TextInterface> textInterface = {},
                                   std::unique_ptr<TableInterface> tableInterface = {},
                                   std::unique_ptr<CellInterface> cellInterface = {});

    /** Destructor. */
    virtual ~ComponentAccessibilityHandler() override;

    //==============================================================================
    /** Returns the component that this handler represents. */
    const Component& getComponent() const noexcept   { return component; }

    /** Returns the component that this handler represents. */
    Component& getComponent() noexcept               { return component; }

    //==============================================================================
    /** Returns the title of the component.

        The default implementation will call `Component::getName()`, but you can override
        this to return a different string if required.

        For more information about the requirements for this string, look at docs for the
        `AccessibilityHandler` base class.

        @see AccessibilityHandler::getTitle
    */
    virtual String getTitle() const override         { return component.getName(); }

    /** Returns a short description of the component.

        The default implementation will call `Component::getDescription()`, but you
        can override this to return a different string if required.

        For more information about the requirements for this string, look at docs for the
        `AccessibilityHandler` base class.

        @see AccessibilityHandler::getDescription
    */
    virtual String getDescription() const override   { return component.getDescription(); }

    /** Returns some help text for the component (if required).

        The default implementation will call `Component::getHelpText()`, but you can
        override this to return a different string if required.

        For more information about the requirements for this string, look at docs for the
        `AccessibilityHandler` base class.

        @see AccessibilityHandler::getHelp
    */
    virtual String getHelp() const override          { return component.getHelpText(); }

    /** Returns the current state of the component.

        The default implementation will set the appropriate flags for a standard component,
        but you can override this method for more fine-grained control. You'll probably
        still want to call the base implementation however.
    */
    virtual AccessibleState getCurrentState() const override;

    /** Returns a vector containing all of the direct children of this component in
        the order which they should be navigated by an accessibility client moving
        between them.

        The default implementation for this just calls `getChildren()` but you can
        override this method to have more control over navigation order between children.
    */
    virtual std::vector<const AccessibilityHandler*> getChildrenInNavigationOrder() const override;

    //==============================================================================
    /** @internal */
    Rectangle<int> getScreenBounds() const override;
    /** @internal */
    Rectangle<int> convertFromNativeBounds (const Rectangle<int>& nativeRect) const override;
    /** @internal */
    Rectangle<int> convertToNativeBounds (const Rectangle<int>& juceRect) const override;
    /** @internal */
    Point<int> convertFromNativePoint (const Point<int>& nativePoint) const override;
    /** @internal */
    Point<int> convertToNativePoint (const Point<int>& jucePoint) const override;
    /** @internal */
    void* getWindow() const override;
    /** @internal */
    const AccessibilityHandler* getParent() const override;
    /** @internal */
    std::vector<const AccessibilityHandler*> getChildren() const override;
    /** @internal */
    const AccessibilityHandler* getChildAt (Point<int>) const override;
    /** @internal */
    const AccessibilityHandler* getFocusedChild() const override;
    /** @internal */
    const AccessibilityHandler* getRoot() const override;
    /** @internal */
    const AccessibilityHandler* getModal() const override;
    /** @internal */
    bool isRoot() const override;

private:
    Component& component;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ComponentAccessibilityHandler)
};

} // namespace juce
