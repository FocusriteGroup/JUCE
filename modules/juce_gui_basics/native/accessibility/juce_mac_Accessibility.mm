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

#if (! defined MAC_OS_X_VERSION_10_13) || MAC_OS_X_VERSION_MAX_ALLOWED < MAC_OS_X_VERSION_10_13
 using NSAccessibilityRole = NSString*;
 using NSAccessibilityNotificationName = NSString*;
#endif

//==============================================================================
class AccessibilityHandler::AccessibilityNativeImpl
{
public:
    explicit AccessibilityNativeImpl (AccessibilityHandler& handler)
        : accessibilityElement (AccessibilityElement::create (handler))
    {}

    ~AccessibilityNativeImpl() = default;

    NSAccessibilityElement<NSAccessibility>* getAccessibilityElement() const noexcept
    {
        return accessibilityElement.get();
    }

private:
    //==============================================================================
    class AccessibilityElement  : public ObjCClass<NSAccessibilityElement<NSAccessibility>>
    {
    private:
        struct Deleter
        {
            void operator() (NSAccessibilityElement<NSAccessibility>* element) const
            {
                object_setInstanceVariable (element, "handler", nullptr);
                [element release];
            }
        };

    public:
        using Holder = std::unique_ptr<NSAccessibilityElement<NSAccessibility>, Deleter>;

        static Holder create (AccessibilityHandler& handler)
        {
            static AccessibilityElement cls;
            Holder element ([cls.createInstance() init]);
            object_setInstanceVariable (element.get(), "handler", &handler);
            return element;
        }

    private:
        AccessibilityElement()  : ObjCClass<NSAccessibilityElement<NSAccessibility>> ("JUCEAccessibilityElement_")
        {
            addIvar<AccessibilityHandler*> ("handler");

            addMethod (@selector (accessibilityNotifiesWhenDestroyed),     getAccessibilityNotifiesWhenDestroyed, "c@:");
            addMethod (@selector (isAccessibilityElement),                 getIsAccessibilityElement,             "c@:");
            addMethod (@selector (isAccessibilityEnabled),                 getIsAccessibilityEnabled,             "c@:");
            addMethod (@selector (isAccessibilitySelected),                getIsAccessibilitySelected,            "c@:");
            addMethod (@selector (setAccessibilitySelected:),              setAccessibilitySelected,              "v@:c");
            addMethod (@selector (accessibilitySelectedChildren),          getAccessibilitySelectedChildren,      "@@:");
            addMethod (@selector (setAccessibilitySelectedChildren:),      setAccessibilitySelectedChildren,      "v@:@");
            addMethod (@selector (accessibilityWindow),                    getAccessibilityWindow,                "@@:");
            addMethod (@selector (accessibilityTopLevelUIElement),         getAccessibilityWindow,                "@@:");
            addMethod (@selector (accessibilityFocusedUIElement),          getAccessibilityFocusedUIElement,      "@@:");
            addMethod (@selector (accessibilityHitTest:),                  accessibilityHitTest,                  "@@:", @encode (NSPoint));
            addMethod (@selector (accessibilityParent),                    getAccessibilityParent,                "@@:");
            addMethod (@selector (accessibilityChildren),                  getAccessibilityChildren,              "@@:");
            addMethod (@selector (isAccessibilityFocused),                 getIsAccessibilityFocused,             "c@:");
            addMethod (@selector (setAccessibilityFocused:),               setAccessibilityFocused,               "v@:c");
            addMethod (@selector (accessibilityFrame),                     getAccessibilityFrame,                 @encode (NSRect), "@:");
            addMethod (@selector (accessibilityRole),                      getAccessibilityRole,                  "@@:");
            addMethod (@selector (accessibilitySubrole),                   getAccessibilitySubrole,               "@@:");
            addMethod (@selector (accessibilityTitle),                     getAccessibilityTitle,                 "@@:");
            addMethod (@selector (accessibilityLabel),                     getAccessibilityLabel,                 "@@:");
            addMethod (@selector (accessibilityHelp),                      getAccessibilityHelp,                  "@@:");
            addMethod (@selector (accessibilityValue),                     getAccessibilityValue,                 "@@:");
            addMethod (@selector (setAccessibilityValue:),                 setAccessibilityValue,                 "v@:@");

            addMethod (@selector (accessibilityInsertionPointLineNumber),  getAccessibilityInsertionPointLineNumber, "i@:");
            addMethod (@selector (accessibilitySharedCharacterRange),      getAccessibilitySharedCharacterRange,     @encode (NSRange), "@:");
            addMethod (@selector (accessibilitySharedTextUIElements),      getAccessibilitySharedTextUIElements,     "@@:");
            addMethod (@selector (accessibilityVisibleCharacterRange),     getAccessibilityVisibleCharacterRange,    @encode (NSRange), "@:");
            addMethod (@selector (accessibilityNumberOfCharacters),        getAccessibilityNumberOfCharacters,       "i@:");
            addMethod (@selector (accessibilitySelectedText),              getAccessibilitySelectedText,             "@@:");
            addMethod (@selector (accessibilitySelectedTextRange),         getAccessibilitySelectedTextRange,        @encode (NSRange), "@:");
            addMethod (@selector (accessibilitySelectedTextRanges),        getAccessibilitySelectedTextRanges,       "@@:");
            addMethod (@selector (accessibilityAttributedStringForRange:), getAccessibilityAttributedStringForRange, "@@:", @encode (NSRange));
            addMethod (@selector (accessibilityRangeForLine:),             getAccessibilityRangeForLine,             @encode (NSRange), "@:i");
            addMethod (@selector (accessibilityStringForRange:),           getAccessibilityStringForRange,           "@@:", @encode (NSRange));
            addMethod (@selector (accessibilityRangeForPosition:),         getAccessibilityRangeForPosition,         @encode (NSRange), "@:", @encode (NSPoint));
            addMethod (@selector (accessibilityRangeForIndex:),            getAccessibilityRangeForIndex,            @encode (NSRange), "@:i");
            addMethod (@selector (accessibilityFrameForRange:),            getAccessibilityFrameForRange,            @encode (NSRect), "@:", @encode (NSRange));
            addMethod (@selector (accessibilityRTFForRange:),              getAccessibilityRTFForRange,              "@@:", @encode (NSRange));
            addMethod (@selector (accessibilityStyleRangeForIndex:),       getAccessibilityStyleRangeForIndex,       @encode (NSRange), "@:i");
            addMethod (@selector (accessibilityLineForIndex:),             getAccessibilityLineForIndex,             "i@:i");
            addMethod (@selector (setAccessibilitySelectedTextRange:),     setAccessibilitySelectedTextRange,        "v@:", @encode (NSRange));

            addMethod (@selector (accessibilityRows),               getAccessibilityRows,             "@@:");
            addMethod (@selector (accessibilityColumnCount),        getAccessibilityColumnCount,      "i@:");
            addMethod (@selector (accessibilityRowCount),           getAccessibilityRowCount,         "i@:");
            addMethod (@selector (accessibilitySortDirection),      getAccessibilitySortDirection,    "i@:");
            addMethod (@selector (accessibilitySelectedCells),      getAccessibilitySelectedCells,    "@@:");
            addMethod (@selector (accessibilityVisibleCells),       getAccessibilityVisibleCells,     "@@:");
            addMethod (@selector (accessibilityCellForColumn:row:), getAccessibilityCellForColumnRow, "@:ii");

            addMethod (@selector (accessibilityRowIndexRange),    getAccessibilityRowIndexRange,    @encode (NSRange), "@:");
            addMethod (@selector (accessibilityColumnIndexRange), getAccessibilityColumnIndexRange, @encode (NSRange), "@:");
            addMethod (@selector (accessibilityIndex),            getAccessibilityIndex,            "i@:");
            addMethod (@selector (accessibilityDisclosureLevel),  getAccessibilityDisclosureLevel,  "i@:");

            addMethod (@selector (accessibilityPerformIncrement), accessibilityPerformIncrement, "c@:");
            addMethod (@selector (accessibilityPerformDecrement), accessibilityPerformDecrement, "c@:");
            addMethod (@selector (accessibilityPerformDelete),    accessibilityPerformDelete,    "c@:");
            addMethod (@selector (accessibilityPerformCancel),    accessibilityPerformCancel,    "c@:");
            addMethod (@selector (accessibilityPerformPress),     accessibilityPerformPress,     "c@:");
            addMethod (@selector (accessibilityPerformRaise),     accessibilityPerformRaise,     "c@:");
            addMethod (@selector (accessibilityPerformShowMenu),  accessibilityPerformShowMenu,  "c@:");
            addMethod (@selector (accessibilityPerformPick),      accessibilityPerformPick,      "c@:");

            addMethod (@selector (isAccessibilitySelectorAllowed:), getIsAccessibilitySelectorAllowed, "c@:@");

           #if defined (MAC_OS_X_VERSION_10_13) && MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_13
            addMethod (@selector (accessibilityChildrenInNavigationOrder), getAccessibilityChildren, "@@:");
           #endif

            registerClass();
        }

    private:
        static AccessibilityHandler* getHandler (id self)            { return getIvar<AccessibilityHandler*> (self, "handler"); }

        static TextInterface*  getTextInterface  (id self) noexcept  { if (auto* handler = getHandler (self)) return handler->getTextInterface();  return nullptr; }
        static ValueInterface* getValueInterface (id self) noexcept  { if (auto* handler = getHandler (self)) return handler->getValueInterface(); return nullptr; }
        static TableInterface* getTableInterface (id self) noexcept  { if (auto* handler = getHandler (self)) return handler->getTableInterface(); return nullptr; }
        static CellInterface*  getCellInterface  (id self) noexcept  { if (auto* handler = getHandler (self)) return handler->getCellInterface();  return nullptr; }

        static bool hasEditableText (AccessibilityHandler& handler) noexcept
        {
            return handler.getRole() == AccessibilityRole::editableText
                && handler.getTextInterface() != nullptr;
        }

        static bool nameIsAccessibilityValue (AccessibilityRole role) noexcept
        {
            return role == AccessibilityRole::staticText;
        }

        //==============================================================================
        static BOOL getAccessibilityNotifiesWhenDestroyed (id, SEL)  { return YES; }

        static BOOL getIsAccessibilityElement (id self, SEL)
        {
            if (auto* handler = getHandler (self))
                return ! handler->isIgnored() && handler->getRole() != AccessibilityRole::window;

            return NO;
        }

        static BOOL getIsAccessibilityEnabled (id self, SEL)
        {
            if (auto* handler = getHandler (self))
                return handler->getComponent().isEnabled();

            return NO;
        }

        static BOOL getIsAccessibilitySelected (id self, SEL)
        {
            if (auto* handler = getHandler (self))
                return handler->getCurrentState().isSelected();

            return NO;
        }

        static void setAccessibilitySelected (id self, SEL, BOOL selected)
        {
            if (auto* handler = getHandler (self))
            {
                auto actionType = selected ? AccessibilityActionType::select
                                           : AccessibilityActionType::deselect;

                if (auto actionCallback = handler->getActions().get (actionType))
                    actionCallback();
            }
        }

        static NSArray* getAccessibilitySelectedChildren (id self, SEL)
        {
            NSMutableArray* selected = [[NSMutableArray new] autorelease];

            for (id child in [self accessibilityChildren])
                if ([child isAccessibilitySelected])
                    [selected addObject: child];

            return selected;
        }

        static void setAccessibilitySelectedChildren (id self, SEL, NSArray* selected)
        {
            for (id child in [self accessibilityChildren])
                [child setAccessibilitySelected: [selected containsObject: child]];
        }

        static id getAccessibilityWindow (id self, SEL)
        {
            if (auto* handler = getHandler (self))
                return [(id) handler->getComponent().getWindowHandle() accessibilityWindow];

            return nil;
        }

        static id getAccessibilityFocusedUIElement (id self, SEL)
        {
            if (auto* handler = getHandler (self))
                if (auto* focus = handler->getFocus())
                    return (id) focus->getNativeImplementation();

            return nil;
        }

        static id accessibilityHitTest (id self, SEL, NSPoint point)
        {
            if (auto* handler = getHandler (self))
            {
                if (auto* child = handler->getChildAt (convertToPointInt (flippedScreenPoint (point))))
                    return (id) child->getNativeImplementation();

                return self;
            }

            return nil;
        }

        static id getAccessibilityParent (id self, SEL)
        {
            if (auto* handler = getHandler (self))
            {
                if (auto* parentHandler = handler->getParent())
                    return (id) parentHandler->getNativeImplementation();

                return [self accessibilityWindow];
            }

            return nil;
        }

        static NSArray* getAccessibilityChildren (id self, SEL)
        {
            if (auto* handler = getHandler (self))
            {
                NSMutableArray* accessibleChildren = [[NSMutableArray new] autorelease];

                for (auto* childHandler : handler->getChildren())
                    [accessibleChildren addObject: (id) childHandler->getNativeImplementation()];

                return accessibleChildren;
            }

            return nil;
        }

        static BOOL getIsAccessibilityFocused (id self, SEL)
        {
            if (auto* handler = getHandler (self))
                return handler->isFocused();

            return NO;
        }

        static void setAccessibilityFocused (id self, SEL, BOOL focused)
        {
            if (auto* handler = getHandler (self))
            {
                auto& component = handler->getComponent();

                if (focused)
                    component.grabKeyboardFocus();
                else if (component.hasKeyboardFocus (true))
                    Component::unfocusAllComponents();
            }
        }

        static NSRect getAccessibilityFrame (id self, SEL)
        {
            if (auto* handler = getHandler (self))
                return flippedScreenRect (makeNSRect (handler->getComponent().getScreenBounds()));

            return NSZeroRect;
        }

        static NSAccessibilityRole getAccessibilityRole (id self, SEL)
        {
            if (auto* handler = getHandler (self))
            {
                switch (handler->getRole())
                {
                    case AccessibilityRole::button:       return NSAccessibilityButtonRole;
                    case AccessibilityRole::toggleButton: return NSAccessibilityCheckBoxRole;
                    case AccessibilityRole::radioButton:  return NSAccessibilityRadioButtonRole;
                    case AccessibilityRole::comboBox:     return NSAccessibilityPopUpButtonRole;
                    case AccessibilityRole::image:        return NSAccessibilityImageRole;
                    case AccessibilityRole::slider:       return NSAccessibilitySliderRole;
                    case AccessibilityRole::staticText:   return NSAccessibilityStaticTextRole;
                    case AccessibilityRole::editableText: return NSAccessibilityTextAreaRole;
                    case AccessibilityRole::menuItem:     return NSAccessibilityMenuItemRole;
                    case AccessibilityRole::menuBar:      return NSAccessibilityMenuBarRole;
                    case AccessibilityRole::popupMenu:    return NSAccessibilityMenuRole;
                    case AccessibilityRole::table:        return NSAccessibilityTableRole;
                    case AccessibilityRole::tableHeader:  return NSAccessibilityGroupRole;
                    case AccessibilityRole::column:       return NSAccessibilityColumnRole;
                    case AccessibilityRole::row:          return NSAccessibilityRowRole;
                    case AccessibilityRole::cell:         return NSAccessibilityCellRole;
                    case AccessibilityRole::hyperlink:    return NSAccessibilityLinkRole;
                    case AccessibilityRole::list:         return NSAccessibilityListRole;
                    case AccessibilityRole::listItem:     return NSAccessibilityRowRole;
                    case AccessibilityRole::tree:         return NSAccessibilityOutlineRole;
                    case AccessibilityRole::treeItem:     return NSAccessibilityRowRole;
                    case AccessibilityRole::progressBar:  return NSAccessibilityProgressIndicatorRole;
                    case AccessibilityRole::group:        return NSAccessibilityGroupRole;
                    case AccessibilityRole::dialogWindow: return NSAccessibilityWindowRole;
                    case AccessibilityRole::window:       return NSAccessibilityWindowRole;
                    case AccessibilityRole::scrollBar:    return NSAccessibilityScrollBarRole;
                    case AccessibilityRole::tooltip:      return NSAccessibilityWindowRole;
                    case AccessibilityRole::splashScreen: return NSAccessibilityWindowRole;
                    case AccessibilityRole::ignored:      return NSAccessibilityUnknownRole;
                    case AccessibilityRole::unspecified:  return NSAccessibilityGroupRole;
                    default:                              break;
                }

                return NSAccessibilityUnknownRole;
            }

            return nil;
        }

        static NSAccessibilityRole getAccessibilitySubrole (id self, SEL)
        {
            if (auto* textInterface = getTextInterface (self))
                if (textInterface->isDisplayingProtectedText())
                    return NSAccessibilitySecureTextFieldSubrole;

            return NSAccessibilityUnknownRole;
        }

        static NSString* getAccessibilityTitle (id self, SEL)
        {
            if (auto* handler = getHandler (self))
            {
                if (nameIsAccessibilityValue (handler->getRole()))
                    return @"";

                return juceStringToNS (handler->getTitle());
            }

            return nil;
        }

        static NSString* getAccessibilityLabel (id self, SEL)
        {
            if (auto* handler = getHandler (self))
                return juceStringToNS (handler->getDescription());

            return nil;
        }

        static NSString* getAccessibilityHelp (id self, SEL)
        {
            if (auto* handler = getHandler (self))
                return juceStringToNS (handler->getHelp());

            return nil;
        }

        static id getAccessibilityValue (id self, SEL)
        {
            if (auto* handler = getHandler (self))
            {
                if (nameIsAccessibilityValue (handler->getRole()))
                    return juceStringToNS (handler->getTitle());

                if (hasEditableText (*handler))
                {
                    auto* textInterface = handler->getTextInterface();
                    return juceStringToNS (textInterface->getText ({ 0, textInterface->getTotalNumCharacters() }));
                }

                if (auto* valueInterface = handler->getValueInterface())
                    return juceStringToNS (valueInterface->getCurrentValueAsString());
            }

            return nil;
        }

        static void setAccessibilityValue (id self, SEL, NSString* value)
        {
            if (auto* handler = getHandler (self))
            {
                if (hasEditableText (*handler))
                {
                    handler->getTextInterface()->setText (nsStringToJuce (value));
                    return;
                }

                if (auto* valueInterface = handler->getValueInterface())
                    if (! valueInterface->isReadOnly())
                        valueInterface->setValueAsString (nsStringToJuce (value));
            }
        }

        //==============================================================================
        static NSInteger getAccessibilityInsertionPointLineNumber (id self, SEL)
        {
            if (auto* textInterface = getTextInterface (self))
                return [self accessibilityLineForIndex: textInterface->getTextInsertionOffset()];

            return 0;
        }

        static NSRange getAccessibilitySharedCharacterRange (id self, SEL)
        {
            return [self accessibilityVisibleCharacterRange];
        }

        static NSArray* getAccessibilitySharedTextUIElements (id self, SEL)
        {
            return [NSArray arrayWithObject: self];
        }

        static NSRange getAccessibilityVisibleCharacterRange (id self, SEL)
        {
            if (auto* textInterface = getTextInterface (self))
                return juceRangeToNS ({ 0, textInterface->getTotalNumCharacters() });

            return NSMakeRange (0, 0);
        }

        static NSInteger getAccessibilityNumberOfCharacters (id self, SEL)
        {
            if (auto* textInterface = getTextInterface (self))
                return textInterface->getTotalNumCharacters();

            return 0;
        }

        static NSString* getAccessibilitySelectedText (id self, SEL)
        {
            if (auto* textInterface = getTextInterface (self))
                return juceStringToNS (textInterface->getText (textInterface->getSelection()));

            return nil;
        }

        static NSRange getAccessibilitySelectedTextRange (id self, SEL)
        {
            if (auto* textInterface = getTextInterface (self))
                return juceRangeToNS (textInterface->getSelection());

            return NSMakeRange (0, 0);
        }

        static NSArray* getAccessibilitySelectedTextRanges (id self, SEL)
        {
            return [NSArray arrayWithObject: [NSValue valueWithRange: [self accessibilitySelectedTextRange]]];
        }

        static NSAttributedString* getAccessibilityAttributedStringForRange (id self, SEL, NSRange range)
        {
            NSString* string = [self accessibilityStringForRange: range];

            if (string != nil)
                return [[[NSAttributedString alloc] initWithString: string] autorelease];

            return nil;
        }

        static NSRange getAccessibilityRangeForLine (id self, SEL, NSInteger line)
        {
            if (auto* textInterface = getTextInterface (self))
            {
                auto text = textInterface->getText ({ 0, textInterface->getTotalNumCharacters() });
                auto lines = StringArray::fromLines (text);

                if (line < lines.size())
                {
                    auto lineText = lines[(int) line];
                    auto start = text.indexOf (lineText);

                    if (start >= 0)
                        return NSMakeRange ((NSUInteger) start, (NSUInteger) lineText.length());
                }
            }

            return NSMakeRange (0, 0);
        }

        static NSString* getAccessibilityStringForRange (id self, SEL, NSRange range)
        {
            if (auto* textInterface = getTextInterface (self))
                return juceStringToNS (textInterface->getText (nsRangeToJuce (range)));

            return nil;
        }

        static NSRange getAccessibilityRangeForPosition (id self, SEL, NSPoint position)
        {
            if (auto* handler = getHandler (self))
            {
                if (auto* textInterface = handler->getTextInterface())
                {
                    auto screenPoint = convertToPointInt (flippedScreenPoint (position));

                    if (handler->getComponent().getScreenBounds().contains (screenPoint))
                    {
                        auto offset = textInterface->getOffsetAtPoint (screenPoint);

                        if (offset >= 0)
                            return NSMakeRange ((NSUInteger) offset, 1);
                    }
                }
            }

            return NSMakeRange (0, 0);
        }

        static NSRange getAccessibilityRangeForIndex (id self, SEL, NSInteger index)
        {
            if (auto* textInterface = getTextInterface (self))
                if (isPositiveAndBelow (index, textInterface->getTotalNumCharacters()))
                    return NSMakeRange ((NSUInteger) index, 1);

            return NSMakeRange (0, 0);
        }

        static NSRect getAccessibilityFrameForRange (id self, SEL, NSRange range)
        {
            if (auto* textInterface = getTextInterface (self))
                return flippedScreenRect (makeNSRect (textInterface->getTextBounds (nsRangeToJuce (range)).getBounds()));

            return NSZeroRect;
        }

        static NSData* getAccessibilityRTFForRange (id, SEL, NSRange)
        {
            return nil;
        }

        static NSRange getAccessibilityStyleRangeForIndex (id self, SEL, NSInteger)
        {
            return [self accessibilityVisibleCharacterRange];
        }

        static NSInteger getAccessibilityLineForIndex (id self, SEL, NSInteger index)
        {
            if (auto* textInterface = getTextInterface (self))
            {
                auto text = textInterface->getText ({ 0, (int) index });

                if (! text.isEmpty())
                    return StringArray::fromLines (text).size() - 1;
            }

            return 0;
        }

        static void setAccessibilitySelectedTextRange (id self, SEL, NSRange selectedRange)
        {
            if (auto* textInterface = getTextInterface (self))
            {
                textInterface->setSelection ({});
                textInterface->setSelection (nsRangeToJuce (selectedRange));
            }
        }

        //==============================================================================
        static NSArray* getAccessibilityRows (id self, SEL)
        {
            NSMutableArray* rows = [[NSMutableArray new] autorelease];

            if (auto* tableInterface = getTableInterface (self))
                for (int row = 0; row < tableInterface->getNumRows(); ++row)
                    if (auto* handler = tableInterface->getCellHandler (row, 0))
                        [rows addObject: (id) handler->getNativeImplementation()];

            return rows;
        }

        static NSInteger getAccessibilityColumnCount (id self, SEL)
        {
            if (auto* tableInterface = getTableInterface (self))
                return tableInterface->getNumColumns();

            return 0;
        }

        static NSInteger getAccessibilityRowCount (id self, SEL)
        {
            if (auto* tableInterface = getTableInterface (self))
                return tableInterface->getNumRows();

            return 0;
        }

        static NSAccessibilitySortDirection getAccessibilitySortDirection (id, SEL)
        {
            return NSAccessibilitySortDirectionUnknown;
        }

        static NSArray* getAccessibilitySelectedCells (id self, SEL)
        {
            NSMutableArray* selectedCells = [[NSMutableArray new] autorelease];

            for (id cell : [self accessibilityVisibleCells])
                if ([cell isAccessibilitySelected])
                    [selectedCells addObject: cell];

            return selectedCells;
        }

        static NSArray* getAccessibilityVisibleCells (id self, SEL)
        {
            NSMutableArray* cells = [[NSMutableArray new] autorelease];

            if (auto* tableInterface = getTableInterface (self))
                for (int row = 0; row < tableInterface->getNumRows(); ++row)
                    for (int column = 0; column < tableInterface->getNumColumns(); ++column)
                        if (auto* handler = tableInterface->getCellHandler (row, column))
                            [cells addObject: (id) handler->getNativeImplementation()];

            return cells;
        }

        static id getAccessibilityCellForColumnRow (id self, SEL, NSInteger column, NSInteger row)
        {
            if (auto* tableInterface = getTableInterface (self))
                if (auto* handler = tableInterface->getCellHandler ((int) row, (int) column))
                    return (id) handler->getNativeImplementation();

            return nil;
        }

        //==============================================================================
        static NSRange getAccessibilityRowIndexRange (id self, SEL)
        {
            if (auto* cellInterface = getCellInterface (self))
                return NSMakeRange ((NSUInteger) cellInterface->getRowIndex(),
                                    (NSUInteger) cellInterface->getRowSpan());

            return NSMakeRange (0, 0);
        }

        static NSRange getAccessibilityColumnIndexRange (id self, SEL)
        {
            if (auto* cellInterface = getCellInterface (self))
                return NSMakeRange ((NSUInteger) cellInterface->getColumnIndex(),
                                    (NSUInteger) cellInterface->getColumnSpan());

            return NSMakeRange (0, 0);
        }

        static NSInteger getAccessibilityIndex (id self, SEL)
        {
            if (auto* handler = getHandler (self))
            {
                if (auto* cellInterface = handler->getCellInterface())
                {
                    auto role = handler->getRole();

                    if (role == AccessibilityRole::row)
                        return cellInterface->getRowIndex();
                    else if (role == AccessibilityRole::column)
                        return cellInterface->getColumnIndex();
                }
            }

            return 0;
        }

        static NSInteger getAccessibilityDisclosureLevel (id, SEL)
        {
            return 0;
        }

        //==============================================================================
        static BOOL performActionIfSupported (id self, AccessibilityActionType actionType)
        {
            if (auto* handler = getHandler (self))
            {
                if (auto actionCallback = handler->getActions().get (actionType))
                {
                    actionCallback();
                    return YES;
                }
            }

            return NO;
        }

        static BOOL accessibilityPerformPress (id self, SEL)     { return performActionIfSupported (self, AccessibilityActionType::press); }
        static BOOL accessibilityPerformRaise (id self, SEL)     { return performActionIfSupported (self, AccessibilityActionType::raise); }
        static BOOL accessibilityPerformShowMenu (id self, SEL)  { return performActionIfSupported (self, AccessibilityActionType::showMenu); }
        static BOOL accessibilityPerformCancel (id self, SEL)    { return performActionIfSupported (self, AccessibilityActionType::cancel); }
        static BOOL accessibilityPerformPick (id self, SEL)      { return [self accessibilityPerformPress]; }

        static BOOL accessibilityPerformIncrement (id self, SEL)
        {
            if (auto* valueInterface = getValueInterface (self))
            {
                if (! valueInterface->isReadOnly() && valueInterface->isRanged())
                {
                    valueInterface->setValue (jlimit (valueInterface->getMinimumValue(),
                                                      valueInterface->getMaximumValue(),
                                                      valueInterface->getCurrentValue() + valueInterface->getStepSize()));
                    return YES;
                }
            }

            return NO;
        }

        static BOOL accessibilityPerformDecrement (id self, SEL)
        {
            if (auto* valueInterface = getValueInterface (self))
            {
                if (! valueInterface->isReadOnly() && valueInterface->isRanged())
                {
                    valueInterface->setValue (jlimit (valueInterface->getMinimumValue(),
                                                      valueInterface->getMaximumValue(),
                                                      valueInterface->getCurrentValue() - valueInterface->getStepSize()));
                    return YES;
                }
            }

            return NO;
        }

        static BOOL accessibilityPerformDelete (id self, SEL)
        {
            if (auto* handler = getHandler (self))
            {
                if (hasEditableText (*handler))
                {
                    handler->getTextInterface()->setText ({});
                    return YES;
                }

                if (auto* valueInterface = handler->getValueInterface())
                {
                    if (! valueInterface->isReadOnly())
                    {
                        valueInterface->setValue ({});
                        return YES;
                    }
                }
            }

            return NO;
        }

        //==============================================================================
        static BOOL getIsAccessibilitySelectorAllowed (id self, SEL, SEL selector)
        {
            if (auto* handler = getHandler (self))
            {
                for (auto textSelector : { @selector (accessibilityInsertionPointLineNumber),
                                           @selector (accessibilitySharedCharacterRange),
                                           @selector (accessibilitySharedTextUIElements),
                                           @selector (accessibilityVisibleCharacterRange),
                                           @selector (accessibilityNumberOfCharacters),
                                           @selector (accessibilitySelectedText),
                                           @selector (accessibilitySelectedTextRange),
                                           @selector (accessibilitySelectedTextRanges),
                                           @selector (accessibilityAttributedStringForRange:),
                                           @selector (accessibilityRangeForLine:),
                                           @selector (accessibilityStringForRange:),
                                           @selector (accessibilityRangeForPosition:),
                                           @selector (accessibilityRangeForIndex:),
                                           @selector (accessibilityFrameForRange:),
                                           @selector (accessibilityRTFForRange:),
                                           @selector (accessibilityStyleRangeForIndex:),
                                           @selector (accessibilityLineForIndex:),
                                           @selector (setAccessibilitySelectedTextRange:) })
                {
                    if (selector == textSelector)
                        return handler->getTextInterface() != nullptr;
                }

                for (auto tableSelector : { @selector (accessibilityRows),
                                            @selector (accessibilityColumnCount),
                                            @selector (accessibilityRowCount),
                                            @selector (accessibilitySortDirection),
                                            @selector (accessibilitySelectedCells),
                                            @selector (accessibilityVisibleCells),
                                            @selector (accessibilityCellForColumn:row:) })
                {
                    if (selector == tableSelector)
                        return handler->getTableInterface() != nullptr;
                }

                for (auto cellSelector : { @selector (accessibilityRowIndexRange),
                                           @selector (accessibilityColumnIndexRange),
                                           @selector (accessibilityIndex),
                                           @selector (accessibilityDisclosureLevel) })
                {
                    if (selector == cellSelector)
                        return handler->getCellInterface() != nullptr;
                }

                for (auto valueSelector : { @selector (accessibilityValue),
                                            @selector (setAccessibilityValue:),
                                            @selector (accessibilityPerformDelete),
                                            @selector (accessibilityPerformIncrement),
                                            @selector (accessibilityPerformDecrement) })
                {
                    if (selector != valueSelector)
                        continue;

                    auto* valueInterface = handler->getValueInterface();

                    if (selector == @selector (accessibilityValue))
                        return valueInterface != nullptr
                            || hasEditableText (*handler)
                            || nameIsAccessibilityValue (handler->getRole());

                    auto hasEditableValue = [valueInterface] { return valueInterface != nullptr && ! valueInterface->isReadOnly(); };

                    if (selector == @selector (setAccessibilityValue:)
                     || selector == @selector (accessibilityPerformDelete))
                        return hasEditableValue() || hasEditableText (*handler);

                    auto isRanged = [valueInterface] { return valueInterface != nullptr && ! valueInterface->isRanged(); };

                    if (selector == @selector (accessibilityPerformIncrement)
                     || selector == @selector (accessibilityPerformDecrement))
                        return hasEditableValue() && isRanged();

                    return NO;
                }

                for (auto actionSelector : { @selector (accessibilityPerformCancel),
                                             @selector (accessibilityPerformConfirm),
                                             @selector (accessibilityPerformPick),
                                             @selector (accessibilityPerformPress),
                                             @selector (accessibilityPerformRaise),
                                             @selector (accessibilityPerformShowAlternateUI),
                                             @selector (accessibilityPerformShowDefaultUI),
                                             @selector (accessibilityPerformShowMenu),
                                             @selector (setAccessibilityFocused:),
                                             @selector (setAccessibilitySelected:),
                                             @selector (accessibilitySelectedChildren),
                                             @selector (setAccessibilitySelectedChildren:) })
                {
                    if (selector != actionSelector)
                        continue;

                    if (selector == @selector (accessibilityPerformCancel))
                        return handler->getActions().contains (AccessibilityActionType::cancel);

                    if (selector == @selector (accessibilityPerformPress)
                     || selector == @selector (accessibilityPerformPick))
                        return handler->getActions().contains (AccessibilityActionType::press);

                    if (selector == @selector (accessibilityPerformShowMenu))
                        return handler->getActions().contains (AccessibilityActionType::showMenu);

                    if (selector == @selector (accessibilityPerformRaise))
                        return handler->getActions().contains (AccessibilityActionType::raise);

                    if (selector == @selector (setAccessibilitySelected:))
                        return handler->getActions().contains (AccessibilityActionType::select);

                    if (selector == @selector (accessibilitySelectedChildren)
                     || selector == @selector (setAccessibilitySelectedChildren:))
                    {
                        for (id child in [self accessibilityChildren])
                            if ([child isAccessibilitySelectorAllowed: @selector (setAccessibilitySelected:)])
                                return YES;
                    }

                    if (selector == @selector (setAccessibilityFocused:))
                        return handler->getComponent().getWantsKeyboardFocus();

                    return NO;
                }

                return sendSuperclassMessage<BOOL> (self, @selector (isAccessibilitySelectorAllowed:), selector);
            }

            return NO;
        }

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AccessibilityElement)
    };

    //==============================================================================
    AccessibilityElement::Holder accessibilityElement;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AccessibilityNativeImpl)
};

//==============================================================================
AccessibilityNativeHandle* AccessibilityHandler::getNativeImplementation() const
{
    return (AccessibilityNativeHandle*) nativeImpl->getAccessibilityElement();
}

void AccessibilityHandler::notifyAccessibilityEvent (AccessibilityEvent eventType) const
{
    auto event = [] (AccessibilityEvent et)
    {
        switch (et)
        {
            case AccessibilityEvent::elementCreated:       return NSAccessibilityCreatedNotification;
            case AccessibilityEvent::elementDestroyed:     return NSAccessibilityUIElementDestroyedNotification;
            case AccessibilityEvent::focusChanged:         return NSAccessibilityFocusedUIElementChangedNotification;
            case AccessibilityEvent::windowOpened:         return NSAccessibilityWindowCreatedNotification;
            case AccessibilityEvent::textSelectionChanged: return NSAccessibilitySelectedTextChangedNotification;
            case AccessibilityEvent::textChanged:          return NSAccessibilityValueChangedNotification;
            case AccessibilityEvent::rowSelectionChanged:  return NSAccessibilitySelectedRowsChangedNotification;

            case AccessibilityEvent::windowClosed:
            default:                                       return NSAccessibilityNotificationName{};
        }
    }(eventType);

    if (event != nullptr)
        NSAccessibilityPostNotification (nativeImpl->getAccessibilityElement(), event);
}

AccessibilityHandler::AccessibilityNativeImpl* AccessibilityHandler::createNativeImpl (AccessibilityHandler& handler)
{
    return new AccessibilityHandler::AccessibilityNativeImpl (handler);
}

void AccessibilityHandler::destroyNativeImpl (AccessibilityHandler::AccessibilityNativeImpl* impl)
{
    delete impl;
}

} // namespace juce
