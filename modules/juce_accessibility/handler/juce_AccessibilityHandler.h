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

/** Base class for accessible UI elements.

    This class provides methods that allow an accessibility client, such as VoiceOver on
    macOS, or Narrator on Windows, to control a UI element.

    @tags{Accessibility}
*/
class AccessibilityHandler
{
protected:
    /** Constructor.

        Creates an AccessibilityHandler representing a UI element with the specified role.

        To enable users to interact with the UI element you should provide the set of supported
        actions and their associated callbacks via the `accessibilityActions` parameter.

        For UI elements that support more complex interaction the value, text, table, and cell
        interfaces should be implemented as required and passed into this constructor. See the
        documentation of these classes for more information about the types of control they
        represent and which methods need to be implemented.
    */
    explicit AccessibilityHandler (AccessibilityRole accessibilityRole,
                                   AccessibilityActions accessibilityActions = {},
                                   std::unique_ptr<ValueInterface> valueInterface = {},
                                   std::unique_ptr<TextInterface> textInterface = {},
                                   std::unique_ptr<TableInterface> tableInterface = {},
                                   std::unique_ptr<CellInterface> cellInterface = {});

public:
    /** Destructor. */
    virtual ~AccessibilityHandler() = default;

    //==============================================================================
    /** The type of UI element that this accessibility handler represents.

        @see AccessibilityRole
    */
    AccessibilityRole getRole() const noexcept  { return role; }

    //==============================================================================
    /** The title of the UI element.

        This will be read out by the system and should be concise, preferably matching
        the visible title of the UI element (if any). For example, this might be the
        text of a button or a simple label.

        This must be a localised string.
    */
    virtual String getTitle() const = 0;

    /** A short description of the UI element.

        This may be read out by the system. It should not include the type of the UI
        element and should ideally be a single word, for example "Open" for a button
        which opens a window.

        This must be a localised string.
    */
    virtual String getDescription() const = 0;

    /** Some help text for the UI element (if required).

        This may be read out by the system. This string functions in a similar way
        to a tooltip for the UI element, for example "Click to open window." for a
        button which opens a window.

        This must be a localised string.
    */
    virtual String getHelp() const = 0;

    //==============================================================================
    /** Returns the current state of the UI element.

        @see AccessibleState
    */
    virtual AccessibleState getCurrentState() const = 0;

    //==============================================================================
    /** Returns the bounds of the UI element relative to the top-left of the screen.
        The bounds must be in JUCE's coordinate space, in logical pixels.
    */
    virtual Rectangle<int> getScreenBounds() const = 0;

    /** Given some bounds in the native coordinate space, this method should return
        the equivalent in JUCE's coordinate space.

        @see convertToNativeBounds
    */
    virtual Rectangle<int> convertFromNativeBounds (const Rectangle<int>& nativeRect) const = 0;

    /** Given some bounds in JUCE's coordinate space, this method should return the
        equivalent in the native coordinate space.

        @see convertFromNativeBounds
    */
    virtual Rectangle<int> convertToNativeBounds (const Rectangle<int>& juceRect) const = 0;

    /** Given a position in the native coordinate space, this method should return
        the equivalent in JUCE's coordinate space.

        @see convertToNativePoint
    */
    virtual Point<int> convertFromNativePoint (const Point<int>& nativePoint) const = 0;

    /** Given a position in JUCE's coordinate space, this method should return the
        equivalent in the native coordinate space.

        @see convertFromNativePoint
    */
    virtual Point<int> convertToNativePoint (const Point<int>& jucePoint) const = 0;

    /** Returns the underlying native window handle which contains the UI element. */
    virtual void* getWindow() const = 0;

    /** Returns the AccessibilityHandler of the immediate parent of the UI element
        in the accessibility hierarchy, or nullptr if no parent exists (for example,
        if it is the root).

        @see getChildren, getRoot
    */
    virtual const AccessibilityHandler* getParent() const = 0;

    /** Returns the AccessibilityHandlers of the direct children of the UI element
        in the accessibility hierarchy.

        @see getChildrenInNavigationOrder
    */
    virtual std::vector<const AccessibilityHandler*> getChildren() const = 0;

    /** Returns the AccessibilityHandlers of the direct children of the UI element
        in the accessibility hierarchy, but in the order in which they should be
        navigated by an accessibility client.

        This should be used when a specific navigation order is required, for example
        when presenting an email and password field where the password field should be
        the next element in the navigation order after the email field.

        @see getChildren
    */
    virtual std::vector<const AccessibilityHandler*> getChildrenInNavigationOrder() const = 0;

    /** Returns the AccessibilityHandler of the direct child of the UI element in the
        accessibility hierarchy which contains the given point in logical screen
        coordinates, or nullptr if there is no direct child at this point.
    */
    virtual const AccessibilityHandler* getChildAt (Point<int>) const = 0;

    /** Returns the AccessibilityHandler of the direct child of the UI element in the
        accessibility hierarchy which currently has focus, or nullptr if there is no
        direct child with focus.
    */
    virtual const AccessibilityHandler* getFocusedChild() const = 0;

    /**
    */
    virtual const AccessibilityHandler* getModal() const = 0;

    /** Returns the AccessibilityHandler of the root UI element in the accessibility
        hierarchy, or nullptr if this handler is the root.

        @see isRoot
    */
    virtual const AccessibilityHandler* getRoot() const = 0;

    /** Returns true if the UI element is the root in the accessibility hierarchy. */
    virtual bool isRoot() const = 0;

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
    /** Used to send a notification to any observing accessibility clients.

        @see AccessibilityEvent
    */
    void notifyAccessibilityEvent (AccessibilityEvent event) const;

    //==============================================================================
    /** @internal */
    AccessibilityNativeHandle* getNativeImplementation() const;

private:
    const AccessibilityRole role = AccessibilityRole::unspecified;
    AccessibilityActions actions;

    std::unique_ptr<ValueInterface> valueInterface;
    std::unique_ptr<TextInterface> textInterface;
    std::unique_ptr<TableInterface> tableInterface;
    std::unique_ptr<CellInterface> cellInterface;

    //==============================================================================
    struct Pimpl
    {
        virtual ~Pimpl() = default;
        virtual AccessibilityNativeHandle* getNativeImplementation() const noexcept = 0;
        virtual void notifyAccessibilityEvent (AccessibilityEvent) const = 0;
    };

    std::unique_ptr<Pimpl> pimpl;
    std::unique_ptr<Pimpl> createPimpl();

    class AccessibilityNativeImpl;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AccessibilityHandler)
};

}
