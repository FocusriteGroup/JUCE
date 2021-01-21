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

/** An action that can be performed by an accessible UI element.

    @tags{Accessibility}
*/
enum class AccessibilityActionType
{
    press,
    select,
    multiSelect,
    deselect,
    toggle,
    cancel,
    showMenu,
    raise
};

/** A simple wrapper for building a collection of supported accessibility actions
    and corresponding callbacks for a UI element.

    Pass one of these when constructing an `AccessibilityHandler` to enable users
    to interact with a UI element via the supported actions.

    @tags{Accessibility}
*/
struct AccessibilityActions
{
    /** Constructor.
        Creates a default AccessibilityActions object with no action callbacks.
    */
    AccessibilityActions() = default;

    /** Adds an action.

        When the user performs this action with an accessibility client,
        `actionCallback` will be called.

        Returns a reference to itself so that several calls can be chained.
    */
    AccessibilityActions& addAction (AccessibilityActionType type,
                                     std::function<void()> actionCallback)
    {
        actionMap[type] = std::move (actionCallback);
        return *this;
    }

    /** Returns true if the specified action is supported. */
    bool contains (AccessibilityActionType type) const
    {
        return get (type) != nullptr;
    }

    /** Returns the callback for a specified action, or nullptr if it is not
        supported.
    */
    std::function<void()> get (AccessibilityActionType type) const
    {
        auto iter = actionMap.find (type);

        if (iter != actionMap.end())
            return iter->second;

        return {};
    }

private:
    std::map<AccessibilityActionType, std::function<void()>> actionMap;
};

} // namespace juce
