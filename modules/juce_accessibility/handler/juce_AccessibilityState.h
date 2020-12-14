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

/** Represents the state of an accessible UI element.

    An instance of this class is returned by `AccessibilityHandler::getCurrentState()`
    method to convey its current state to an accessibility client.

    @see AccessibilityHandler

    @tags{Accessibility}
*/
struct AccessibleState
{
    /** Constructor.

        Represents a "default" state with no flags set. To set a flag, use one of the
        `withX()` methods - these can be chained together to set multiple flags.
    */
    AccessibleState() = default;

    //==============================================================================
    /** Sets the focused flag and returns the new state.

        @see isFocused
    */
    AccessibleState withFocused() const noexcept         { return withFlag (Flags::focused);  }

    /** Sets the toggled flag and returns the new state.

        @see isToggled
    */
    AccessibleState withToggled() const noexcept         { return withFlag (Flags::toggled);  }

    /** Sets the selected flag and returns the new state.

        @see isSelected
    */
    AccessibleState withSelected() const noexcept        { return withFlag (Flags::selected); }

    /** Sets the expanded flag and returns the new state.

        @see isExpanded
    */
    AccessibleState withExpanded() const noexcept        { return withFlag (Flags::expanded); }

    /** Sets the ignored flag and returns the new state.

        @see isIgnored
    */
    AccessibleState withIgnored() const noexcept         { return withFlag (Flags::ignored);  }

    /** Sets the disabled flag and returns the new state.

        @see isDisabled
    */
    AccessibleState withDisabled() const noexcept        { return withFlag (Flags::disabled);  }

    /** Sets the invisible flag and returns the new state.

        @see isDisabled
    */
    AccessibleState withInvisible() const noexcept       { return withFlag (Flags::invisible);  }

    /** Sets the maximised flag and returns the new state.

        @see isMaximised
    */
    AccessibleState withMaximised() const noexcept       { return withFlag (Flags::maximised); }

    /** Sets the minimised flag and returns the new state.

        @see isMinimised
    */
    AccessibleState withMinimised() const noexcept       { return withFlag (Flags::minimised); }

    /** Sets the modal flag and returns the new state.

        @see isModal
    */
    AccessibleState withModal() const noexcept           { return withFlag (Flags::modal); }

    /** Sets the blockedByModal flag and returns the new state.

        @see isBlockedByModal
    */
    AccessibleState withBlockedByModal() const noexcept  { return withFlag (Flags::blockedByModal); }

    //==============================================================================
    /** Returns true if the UI element is currently focused.

        @see withFocused
    */
    bool isFocused() const noexcept         { return isFlagSet (Flags::focused); }

    /** Returns true if the UI element is currently toggled "on".

        @see withToggled
    */
    bool isToggled() const noexcept         { return isFlagSet (Flags::toggled); }

    /** Returns true if the UI element is currently selected.

        @see withSelected
    */
    bool isSelected() const noexcept        { return isFlagSet (Flags::selected); }

    /** Returns true if the UI element is currently expanded.

        @see withExpanded
    */
    bool isExpanded() const noexcept        { return isFlagSet (Flags::expanded); }

    /** Returns true if the UI element should be ignored.

        @see withIgnored
    */
    bool isIgnored() const noexcept         { return isFlagSet (Flags::ignored); }

    /** Returns true if the UI element is currently disabled.

        @see withDisabled
    */
    bool isDisabled() const noexcept        { return isFlagSet (Flags::disabled); }

    /** Returns true if the UI element is currently invisible.

        @see withInvisible
    */
    bool isInvisible() const noexcept       { return isFlagSet (Flags::invisible); }

    /** Returns true if the UI element is currently maximised.

        @see withMaximised
    */
    bool isMaximised() const noexcept       { return isFlagSet (Flags::maximised); }

    /** Returns true if the UI element is currently minimised.

        @see withMinimised
    */
    bool isMinimised() const noexcept       { return isFlagSet (Flags::minimised); }

    /** Returns true if the UI element is currently in a modal state.

        @see withModal
    */
    bool isModal() const noexcept           { return isFlagSet (Flags::modal); }

    /** Returns true if the UI element is currently blocked by another UI element
        in a modal state.

        @see withBlockedByModal
    */
    bool isBlockedByModal() const noexcept  { return isFlagSet (Flags::blockedByModal); }

private:
    enum Flags
    {
        focused        = (1 << 0),
        toggled        = (1 << 1),
        selected       = (1 << 2),
        expanded       = (1 << 3),
        ignored        = (1 << 4),
        disabled       = (1 << 5),
        invisible      = (1 << 6),
        maximised      = (1 << 7),
        minimised      = (1 << 8),
        modal          = (1 << 9),
        blockedByModal = (1 << 10)
    };

    AccessibleState withFlag (int flag) const noexcept
    {
        auto copy = *this;
        copy.flags |= flag;

        return copy;
    }

    bool isFlagSet (int flag) const noexcept
    {
        return (flags & flag) != 0;
    }

    int flags = 0;
};

} // namespace juce
