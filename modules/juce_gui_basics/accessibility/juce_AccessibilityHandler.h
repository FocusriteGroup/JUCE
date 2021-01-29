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

class AccessibilityNativeHandle;

/** Base class for accessible Components.

    This class wraps a Component and provides methods that allow an accessibility client,
    such as VoiceOver on macOS, or Narrator on Windows, to control it.

    It handles hierarchical navigation, properties, state, and various interfaces.

    @tags{Accessibility}
*/
class AccessibilityHandler
{
public:
    /** Constructor.

        This will create a AccessibilityHandler which wraps the provided Component and makes
        it visible to accessibility clients. You must also specify a role for the UI element
        from the `AccessibilityRole` list which best describes it.

        To enable users to interact with the UI element you should provide the set of supported
        actions and their associated callbacks via the `accessibilityActions` parameter.

        For UI elements that support more complex interaction the value, text, table, and cell
        interfaces should be implemented as required and passed into this constructor. See the
        documentation of these classes for more information about the types of control they
        represent and which methods need to be implemented.
    */
    AccessibilityHandler (Component& componentToWrap,
                          AccessibilityRole accessibilityRole,
                          AccessibilityActions actions = {},
                          std::unique_ptr<ValueInterface> valueInterface = {},
                          std::unique_ptr<TextInterface> textInterface = {},
                          std::unique_ptr<TableInterface> tableInterface = {},
                          std::unique_ptr<CellInterface> cellInterface = {});

    /** Destructor. */
    virtual ~AccessibilityHandler();

    //==============================================================================
    /** Returns the Component that this handler represents. */
    const Component& getComponent() const noexcept   { return component; }

    /** Returns the Component that this handler represents. */
    Component& getComponent() noexcept               { return component; }

    //==============================================================================
    /** The type of UI element that this accessibility handler represents.

        @see AccessibilityRole
    */
    AccessibilityRole getRole() const noexcept       { return role; }

    /** The title of the UI element.

        This will be read out by the system and should be concise, preferably matching
        the visible title of the UI element (if any). For example, this might be the
        text of a button or a simple label.

        The default implementation will call `Component::getName()`, but you can override
        this to return a different string if required.

        If neither a name nor a description is provided, then the UI element may be
        ignored by accessibility clients.

        This must be a localised string.
    */
    virtual String getTitle() const                  { return component.getName(); }

    /** A short description of the UI element

        This may be read out by the system. It should not include the type of the UI
        element and should ideally be a single word, for example "Open" for a button
        which opens a window.

        The default implementation will call `Component::getDescription()`, but you
        can override this to return a different string if required.

        If neither a name nor a description is provided, then the UI element may be
        ignored by accessibility clients.

        This must be a localised string.
    */
    virtual String getDescription() const            { return component.getDescription(); }

    /** Some help text for the UI element (if required).

        This may be read out by the system. This string functions in a similar way to
        a tooltip, for example "Click to open window." for a button which opens a window.

        The default implementation will call `Component::getHelpText()`, but you can
        override this to return a different string if required.

        This must be a localised string.
    */
    virtual String getHelp() const                   { return component.getHelpText(); }

    /** Returns the current state of the UI element. */
    virtual AccessibleState getCurrentState() const  { return {}; }

    /** Returns true if this UI element should be focused by accessibility clients. */
    bool isFocused() const;

    /** Returns true if this UI element should be ignored by accessibility clients. */
    bool isIgnored() const;

    //==============================================================================
    /** Returns the set of actions that the UI element supports and the associated
        callbacks.
    */
    const AccessibilityActions& getActions() const noexcept  { return actions; }

    /** Returns the value interface for this UI element, or nullptr if it is not supported.

        @see ValueInterface
    */
    ValueInterface* getValueInterface() const;

    /** Returns the table interface for this UI element, or nullptr if it is not supported.

        @see TableInterface
    */
    TableInterface* getTableInterface() const;

    /** Returns the cell interface for this UI element, or nullptr if it is not supported.

        @see CellInterface
    */
    CellInterface* getCellInterface() const;

    /** Returns the text interface for this UI element, or nullptr if it is not supported.

        @see TextInterface
    */
    TextInterface* getTextInterface() const;

    //==============================================================================
    /** Returns the first unignored parent of this UI element in the accessibility hierarchy,
        or nullptr if this is a root element without a parent.
    */
    AccessibilityHandler* getParent() const;

    /** Returns the unignored children of this UI element in the accessibility hierarchy. */
    std::vector<AccessibilityHandler*> getChildren() const;

    /** Returns the deepest child of this UI element in the accessibility hierarchy that
        contains the given screen point, or nullptr if there is no child at this point.
    */
    AccessibilityHandler* getChildAt (Point<int> screenPoint);

    /** Returns the UI element that has focus.

        This can be a child of this UI element in the accessibility hierarchy, this element
        itself, or a modal UI element which is blocking focus.
    */
    AccessibilityHandler* getFocus();

    //==============================================================================
    /** Used to send a notification to any observing accessibility clients.

        @see AccessibilityEvent
    */
    void notifyAccessibilityEvent (AccessibilityEvent event) const;

    //==============================================================================
    /** @internal */
    AccessibilityNativeHandle* getNativeImplementation() const;

private:
    friend class AccessibilityNativeHandle;
    void grabFocus();

    Component& component;

    const AccessibilityRole role;
    AccessibilityActions actions;

    std::unique_ptr<ValueInterface> valueInterface;
    std::unique_ptr<TextInterface> textInterface;
    std::unique_ptr<TableInterface> tableInterface;
    std::unique_ptr<CellInterface> cellInterface;

    class AccessibilityNativeImpl;
    AccessibilityNativeImpl* createNativeImpl (AccessibilityHandler&);
    static void destroyNativeImpl (AccessibilityNativeImpl*);
    std::unique_ptr<AccessibilityNativeImpl, decltype (&destroyNativeImpl)> nativeImpl;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AccessibilityHandler)
};

} // namespace juce
