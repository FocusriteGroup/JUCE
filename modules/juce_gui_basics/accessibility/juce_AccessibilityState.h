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

    //==============================================================================
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

private:
    enum Flags
    {
        toggled  = (1 << 0),
        selected = (1 << 1),
        expanded = (1 << 2),
        ignored  = (1 << 3)
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
