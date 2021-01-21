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

/** An abstract interface representing the value of an accessibility element.

    Values should be used when information needs to be conveyed which cannot
    be represented by the accessibility element's label alone. For example, a
    gain slider with the label "Gain" needs to also provide a value for its
    position whereas a "Save" button does not.

    This class allows for full control over the value text/numeric conversion,
    ranged, and read-only properties but in most cases you'll want to use one
    of the derived classes below which handle some of this for you.

    @see TextValueInterface, NumericValueInterface, RangedNumericValueInterface

    @tags{Accessibility}
*/
struct ValueInterface
{
    /** Destructor. */
    virtual ~ValueInterface() = default;

    /** Returns true if the value is read-only and cannot be modified by an
        accessibility client.

        @see setValue, setValueAsString
    */
    virtual bool isReadOnly() const = 0;

    /** Returns the current value as a double. */
    virtual double getCurrentValue() const = 0;

    /** Returns the current value as a String. */
    virtual String getCurrentValueAsString() const = 0;

    /** Sets the current value to a new double value. */
    virtual void setValue (double newValue) = 0;

    /** Sets the current value to a new String value. */
    virtual void setValueAsString (const String& newValue) = 0;

    /** Returns true if this is a ranged value. If so, you must provide
        a step size and a minimum and maximum value.

        @see getStepSize, getMinimumValue, getMaximumValue
    */
    virtual bool isRanged() const = 0;

    /** Returns the step size for a ranged value.

        @see isRanged
    */
    virtual double getStepSize() const = 0;

    /** Returns the minimum value for a ranged value.

        @see isRanged
    */
    virtual double getMinimumValue() const = 0;

    /** Returns the maximum value for a ranged value.

        @see isRanged
    */
    virtual double getMaximumValue() const = 0;
};

//==============================================================================
/** A value interface which represents a text value.

    @tags{Accessibility}
*/
struct TextValueInterface  : public ValueInterface
{
    /** Destructor. */
    virtual ~TextValueInterface() override = default;

    /** Returns true if the value is read-only and cannot be modified by an
        accessibility client.

        @see setValueAsString
    */
    virtual bool isReadOnly() const override = 0;

    /** Returns the current value. */
    virtual String getCurrentValueAsString() const override = 0;

    /** Sets the current value to a new value. */
    virtual void setValueAsString (const String& newValue) override = 0;

    /** @internal */
    double getCurrentValue() const override final   { return getCurrentValueAsString().getDoubleValue(); }
    /** @internal */
    void setValue (double newValue) override final  { setValueAsString (String (newValue)); }
    /** @internal */
    bool isRanged() const final                     { return false; }
    /** @internal */
    double getStepSize() const override final       { return 0.0; }
    /** @internal */
    double getMinimumValue() const override final   { return 0.0; }
    /** @internal */
    double getMaximumValue() const override final   { return 0.0; }
};

//==============================================================================
/** A value interface which represents a non-ranged numeric value.

    @tags{Accessibility}
*/
struct NumericValueInterface  : public ValueInterface
{
    /** Destructor. */
    virtual ~NumericValueInterface() override = default;

    /** Returns true if the value is read-only and cannot be modified by an
        accessibility client.

        @see setValue
    */
    virtual bool isReadOnly() const override = 0;

    /** Returns the current value. */
    virtual double getCurrentValue() const override = 0;

    /** Sets the current value to a new value. */
    virtual void setValue (double newValue) override = 0;

    /** @internal */
    String getCurrentValueAsString() const override final          { return String (getCurrentValue()); }
    /** @internal */
    void setValueAsString (const String& newValue) override final  { setValue (newValue.getDoubleValue()); }
    /** @internal */
    bool isRanged() const override final                           { return false; }
    /** @internal */
    double getStepSize() const override final                      { return 0.0; }
    /** @internal */
    double getMinimumValue() const override final                  { return 0.0; }
    /** @internal */
    double getMaximumValue() const override final                  { return 0.0; }
};

//==============================================================================
/** A value interface which represents a ranged numeric value.

    @tags{Accessibility}
*/
struct RangedNumericValueInterface  : public ValueInterface
{
    /** Destructor. */
    virtual ~RangedNumericValueInterface() override = default;

    /** Returns true if the value is read-only and cannot be modified by an
        accessibility client.

        @see setValueAsString
    */
    virtual bool isReadOnly() const override = 0;

    /** Returns the current value. */
    virtual double getCurrentValue() const override = 0;

    /** Sets the current value to a new value. */
    virtual void setValue (double newValue) override = 0;

    /** Returns the step size. */
    virtual double getStepSize() const override = 0;

    /** Returns the minimum value. */
    virtual double getMinimumValue() const override = 0;

    /** Returns the maximum value. */
    virtual double getMaximumValue() const override = 0;

    /** @internal */
    String getCurrentValueAsString() const override final          { return String (getCurrentValue()); }
    /** @internal */
    void setValueAsString (const String& newValue) override final  { setValue (newValue.getDoubleValue()); }
    /** @internal */
    bool isRanged() const override final                           { return true; }
};

} // namespace juce
