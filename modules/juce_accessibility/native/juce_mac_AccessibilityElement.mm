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

static NSAccessibilityRole juceAccessibilityRoleToNS (AccessibilityRole role)
{
    static const std::map<AccessibilityRole, NSAccessibilityRole> roleMap
    {
        { AccessibilityRole::button,       NSAccessibilityButtonRole },
        { AccessibilityRole::toggleButton, NSAccessibilityCheckBoxRole },
        { AccessibilityRole::radioButton,  NSAccessibilityRadioButtonRole },
        { AccessibilityRole::comboBox,     NSAccessibilityPopUpButtonRole },
        { AccessibilityRole::image,        NSAccessibilityImageRole },
        { AccessibilityRole::slider,       NSAccessibilitySliderRole },
        { AccessibilityRole::staticText,   NSAccessibilityStaticTextRole },
        { AccessibilityRole::editableText, NSAccessibilityTextAreaRole },
        { AccessibilityRole::menuItem,     NSAccessibilityMenuItemRole },
        { AccessibilityRole::menuBar,      NSAccessibilityMenuBarRole },
        { AccessibilityRole::popupMenu,    NSAccessibilityMenuRole },
        { AccessibilityRole::table,        NSAccessibilityTableRole },
        { AccessibilityRole::tableHeader,  NSAccessibilityGroupRole },
        { AccessibilityRole::column,       NSAccessibilityColumnRole },
        { AccessibilityRole::row,          NSAccessibilityRowRole },
        { AccessibilityRole::cell,         NSAccessibilityCellRole },
        { AccessibilityRole::hyperlink,    NSAccessibilityLinkRole },
        { AccessibilityRole::list,         NSAccessibilityListRole },
        { AccessibilityRole::listItem,     NSAccessibilityRowRole },
        { AccessibilityRole::tree,         NSAccessibilityOutlineRole },
        { AccessibilityRole::treeItem,     NSAccessibilityRowRole },
        { AccessibilityRole::progressBar,  NSAccessibilityProgressIndicatorRole },
        { AccessibilityRole::group,        NSAccessibilityGroupRole },
        { AccessibilityRole::dialogWindow, NSAccessibilityWindowRole },
        { AccessibilityRole::window,       NSAccessibilityWindowRole },
        { AccessibilityRole::scrollBar,    NSAccessibilityScrollBarRole },
        { AccessibilityRole::tooltip,      NSAccessibilityWindowRole },
        { AccessibilityRole::splashScreen, NSAccessibilityWindowRole },
        { AccessibilityRole::ignored,      NSAccessibilityUnknownRole },
        { AccessibilityRole::unspecified,  NSAccessibilityGroupRole }
    };

    auto iter = roleMap.find (role);
    return iter != roleMap.end() ? iter->second : NSAccessibilityUnknownRole;
}

static bool nameIsAccessibilityValue (AccessibilityRole role) noexcept
{
    return role == AccessibilityRole::staticText;
}

//==============================================================================
class AccessibilityNativeHandle  : public ObjCClass<NSAccessibilityElement<NSAccessibility>>
{
public:
    AccessibilityNativeHandle()  : ObjCClass<NSAccessibilityElement<NSAccessibility>> ("JUCEAccessibilityNativeHandle_")
    {
        addIvar<AccessibilityHandler*> ("accessibilityHandler");

        addMethod (NSSelectorFromString (@"initWithAccessibilityHandler:"), initWithAccessibilityHandler, "@@:@");
        addMethod (@selector (detach),                                      detach,                       "v@:");

        addMethod (@selector (accessibilityNotifiesWhenDestroyed),     getAccessibilityNotifiesWhenDestroyed,     "c@:");
        addMethod (@selector (isAccessibilityElement),                 getIsAccessibilityElement,                 "c@:");
        addMethod (@selector (isAccessibilityEnabled),                 getIsAccessibilityEnabled,                 "c@:");
        addMethod (@selector (isAccessibilitySelected),                getIsAccessibilitySelected,                "c@:");
        addMethod (@selector (setAccessibilitySelected:),              setAccessibilitySelected,                  "v@:c");
        addMethod (@selector (accessibilitySelectedChildren),          getAccessibilitySelectedChildren,          "@@:");
        addMethod (@selector (setAccessibilitySelectedChildren:),      setAccessibilitySelectedChildren,          "v@:@");
        addMethod (@selector (accessibilityWindow),                    getAccessibilityWindow,                    "@@:");
        addMethod (@selector (accessibilityTopLevelUIElement),         getAccessibilityTopLevelUIElement,         "@@:");
        addMethod (@selector (accessibilityFocusedUIElement),          getAccessibilityFocusedUIElement,          "@@:");
        addMethod (@selector (accessibilityHitTest:),                  accessibilityHitTest,                      "@@:", @encode (NSPoint));
        addMethod (@selector (accessibilityParent),                    getAccessibilityParent,                    "@@:");
        addMethod (@selector (accessibilityChildren),                  getAccessibilityChildrenInNavigationOrder, "@@:");
        addMethod (NSSelectorFromString (@"accessibilityChildrenInNavigationOrder"),
                                                                       getAccessibilityChildrenInNavigationOrder, "@@:");
        addMethod (@selector (isAccessibilityFocused),                 getIsAccessibilityFocused,                 "c@:");
        addMethod (@selector (setAccessibilityFocused:),               setAccessibilityFocused,                   "v@:c");
        addMethod (@selector (accessibilityFrame),                     getAccessibilityFrame,                     @encode (NSRect), "@:");
        addMethod (@selector (accessibilityRole),                      getAccessibilityRole,                      "@@:");
        addMethod (@selector (accessibilitySubrole),                   getAccessibilitySubrole,                   "@@:");
        addMethod (@selector (accessibilityTitle),                     getAccessibilityTitle,                     "@@:");
        addMethod (@selector (accessibilityLabel),                     getAccessibilityLabel,                     "@@:");
        addMethod (@selector (accessibilityHelp),                      getAccessibilityHelp,                      "@@:");
        addMethod (@selector (accessibilityValue),                     getAccessibilityValue,                     "@@:");
        addMethod (@selector (setAccessibilityValue:),                 setAccessibilityValue,                     "v@:@");

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

        addMethod (@selector (accessibilityPerformDecrement), accessibilityPerformDecrement, "c@:");
        addMethod (@selector (accessibilityPerformDelete),    accessibilityPerformDelete,    "c@:");
        addMethod (@selector (accessibilityPerformIncrement), accessibilityPerformIncrement, "c@:");
        addMethod (@selector (accessibilityPerformCancel),    accessibilityPerformCancel,    "c@:");
        addMethod (@selector (accessibilityPerformPick),      accessibilityPerformPick,      "c@:");
        addMethod (@selector (accessibilityPerformPress),     accessibilityPerformPress,     "c@:");
        addMethod (@selector (accessibilityPerformShowMenu),  accessibilityPerformShowMenu,  "c@:");

        addMethod (@selector (isAccessibilitySelectorAllowed:), getIsAccessibilitySelectorAllowed, "c@:@");

        registerClass();
    }

private:
    static id initWithAccessibilityHandler (id self, SEL, AccessibilityHandler* handler)
    {
        self = sendSuperclassMessage<id> (self, @selector (init));
        object_setInstanceVariable (self, "accessibilityHandler", handler);

        return self;
    }

    static void detach (id self, SEL)
    {
        object_setInstanceVariable (self, "accessibilityHandler", nullptr);
    }

    static AccessibilityHandler* getAccessibilityHandler (id self)
    {
        return getIvar<AccessibilityHandler*> (self, "accessibilityHandler");
    }

    static BOOL isValid (id self)
    {
        return getAccessibilityHandler (self) != nullptr;
    }

    static AccessibleState getCurrentState (id self)
    {
        if (isValid (self))
            return getAccessibilityHandler (self)->getCurrentState();

        return {};
    }

    static TextInterface* getTextInterface (id self)
    {
        if (isValid (self))
            return getAccessibilityHandler (self)->getTextInterface();

        return nullptr;
    }

    static ValueInterface* getValueInterface (id self)
    {
        if (isValid (self))
            return getAccessibilityHandler (self)->getValueInterface();

        return nullptr;
    }

    static TableInterface* getTableInterface (id self)
    {
        if (isValid (self))
            return getAccessibilityHandler (self)->getTableInterface();

        return nullptr;
    }

    static CellInterface* getCellInterface (id self)
    {
        if (isValid (self))
            return getAccessibilityHandler (self)->getCellInterface();

        return nullptr;
    }

    static BOOL hasEditableText (id self)
    {
        if (isValid (self))
            return getAccessibilityHandler (self)->getRole() == AccessibilityRole::editableText
                  && getTextInterface (self) != nullptr;

        return NO;
    }

    static NSArray* createAccessibleChildrenArray (std::vector<const juce::AccessibilityHandler*> children)
    {
        NSMutableArray* accessibleChildren = [[NSMutableArray new] autorelease];

        for (auto* child : children)
        {
            if (child == nullptr)
                continue;

            [accessibleChildren addObject: (id) child->getNativeImplementation()];
        }

        return NSAccessibilityUnignoredChildren (accessibleChildren);
    }

    //==============================================================================
    static BOOL getAccessibilityNotifiesWhenDestroyed (id, SEL)
    {
        return YES;
    }

    static BOOL getIsAccessibilityElement (id self, SEL)
    {
        if (! isValid (self))
            return NO;

        auto role = [self accessibilityRole];

        if (role == NSAccessibilityUnknownRole)
            return NO;

        auto state = getCurrentState (self);

        if (state.isInvisible() || state.isIgnored())
            return NO;

        return YES;
    }

    static BOOL getIsAccessibilityEnabled (id self, SEL)
    {
        if (! isValid (self))
            return NO;

        return ! getCurrentState (self).isDisabled();
    }

    static BOOL getIsAccessibilitySelected (id self, SEL)
    {
        return getCurrentState (self).isSelected();
    }

    static void setAccessibilitySelected (id self, SEL, BOOL selected)
    {
        if (! isValid (self))
            return;

        auto actionType = selected ? AccessibilityActionType::select
                                   : AccessibilityActionType::deselect;

        if (auto actionCallback = getAccessibilityHandler (self)->getActions().get (actionType))
            actionCallback();
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
        return [[self accessibilityParent] accessibilityWindow];
    }

    static id getAccessibilityTopLevelUIElement (id self, SEL)
    {
        return [[self accessibilityParent] accessibilityTopLevelUIElement];
    }

    static id getAccessibilityFocusedUIElement (id self, SEL)
    {
        if (! isValid (self))
            return nil;

        if (getCurrentState (self).isBlockedByModal())
            return NSAccessibilityUnignoredAncestor ([(id) getAccessibilityHandler (self)->getModal()->getNativeImplementation() accessibilityFocusedUIElement]);

        if (auto* handler = getAccessibilityHandler (self)->getFocusedChild())
            return NSAccessibilityUnignoredAncestor ([(id) handler->getNativeImplementation() accessibilityFocusedUIElement]);

        return NSAccessibilityUnignoredAncestor (self);
    }

    static id accessibilityHitTest (id self, SEL, NSPoint point)
    {
        if (! isValid (self))
            return nil;

        point = flippedScreenPoint (point);

        if (auto* handler = getAccessibilityHandler (self)->getChildAt ({ (int) point.x, (int) point.y }))
            return NSAccessibilityUnignoredAncestor ((id) handler->getNativeImplementation());

        return NSAccessibilityUnignoredAncestor (self);
    }

    static id getAccessibilityParent (id self, SEL)
    {
        if (! isValid (self))
            return nil;

        if (auto* handler = getAccessibilityHandler (self)->getParent())
            return NSAccessibilityUnignoredAncestor ((id) handler->getNativeImplementation());

        if (auto* window = getAccessibilityHandler (self)->getWindow())
            return NSAccessibilityUnignoredAncestor ((id) window);

        return nil;
    }

    static NSArray* getAccessibilityChildrenInNavigationOrder (id self, SEL)
    {
        if (! isValid (self))
            return nil;

        return createAccessibleChildrenArray (getAccessibilityHandler (self)->getChildrenInNavigationOrder());
    }

    static BOOL getIsAccessibilityFocused (id self, SEL)
    {
        if (! isValid (self))
            return NO;

        return getCurrentState (self).isFocused();
    }

    static void setAccessibilityFocused (id self, SEL, BOOL isFocused)
    {
        if (isFocused)
            if (auto focusAction = getAccessibilityHandler (self)->getActions().get (AccessibilityActionType::focus))
                focusAction();
    }

    static NSRect getAccessibilityFrame (id self, SEL)
    {
        if (! isValid (self))
            return NSZeroRect;

        return flippedScreenRect (makeNSRect (getAccessibilityHandler (self)->getScreenBounds()));
    }

    static NSAccessibilityRole getAccessibilityRole (id self, SEL)
    {
        if (! isValid (self))
            return nil;

        return juceAccessibilityRoleToNS (getAccessibilityHandler (self)->getRole());
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
        if (! isValid (self))
            return nil;

        if (nameIsAccessibilityValue (getAccessibilityHandler (self)->getRole()))
            return @"";

        return juceStringToNS (getAccessibilityHandler (self)->getTitle());
    }

    static NSString* getAccessibilityLabel (id self, SEL)
    {
        if (! isValid (self))
            return nil;

        return juceStringToNS (getAccessibilityHandler (self)->getDescription());
    }

    static NSString* getAccessibilityHelp (id self, SEL)
    {
        if (! isValid (self))
            return nil;

        return juceStringToNS (getAccessibilityHandler (self)->getHelp());
    }

    static id getAccessibilityValue (id self, SEL)
    {
        if (isValid (self))
        {
            if (nameIsAccessibilityValue (getAccessibilityHandler (self)->getRole()))
                return juceStringToNS (getAccessibilityHandler (self)->getTitle());

            if (hasEditableText (self))
            {
                auto& textInterface = *getTextInterface (self);
                return juceStringToNS (textInterface.getText ({ 0, textInterface.getTotalNumCharacters() }));
            }

            if (auto* valueInterface = getValueInterface (self))
                return juceStringToNS (valueInterface->getCurrentValueAsString());
        }

        return nil;
    }

    static void setAccessibilityValue (id self, SEL, NSString* value)
    {
        if (hasEditableText (self))
        {
            getTextInterface (self)->setText (nsStringToJuce (value));
            return;
        }

        if (auto* valueInterface = getValueInterface (self))
            if (! valueInterface->isReadOnly())
                valueInterface->setValueAsString (nsStringToJuce (value));
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
        if (auto* textInterface = getTextInterface (self))
        {
            position = flippedScreenPoint (position);
            juce::Point<int> screenPoint (roundToInt (position.x), roundToInt (position.y));

            if (getAccessibilityHandler (self)->getScreenBounds().contains (screenPoint))
            {
                auto offset = textInterface->getOffsetAtPoint (screenPoint);

                if (offset >= 0)
                    return NSMakeRange ((NSUInteger) offset, 1);
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
        {
            for (int row = 0; row < tableInterface->getNumRows(); ++row)
            {
                if (auto* handler = tableInterface->getCellHandler (row, 0))
                    [rows addObject: (id) handler->getNativeImplementation()];
            }
        }

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
        {
            for (int row = 0; row < tableInterface->getNumRows(); ++row)
            {
                for (int column = 0; column < tableInterface->getNumColumns(); ++column)
                {
                    if (auto* handler = tableInterface->getCellHandler (row, column))
                        [cells addObject: (id) handler->getNativeImplementation()];
                }
            }
        }

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
        if (auto* cellInterface = getCellInterface (self))
        {
            auto role = getAccessibilityHandler (self)->getRole();

            if (role == AccessibilityRole::row)
                return cellInterface->getRowIndex();
            else if (role == AccessibilityRole::column)
                return cellInterface->getColumnIndex();
        }

        return 0;
    }

    static NSInteger getAccessibilityDisclosureLevel (id, SEL)
    {
        return 0;
    }

    //==============================================================================
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
        if (hasEditableText (self))
        {
            getTextInterface (self)->setText ({});
            return YES;
        }

        if (auto* valueInterface = getValueInterface (self))
        {
            if (! valueInterface->isReadOnly())
            {
                valueInterface->setValue ({});
                return YES;
            }
        }

        return NO;
    }

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

    static BOOL accessibilityPerformCancel (id self, SEL)
    {
        if (isValid (self))
        {
            if (auto actionCallback = getAccessibilityHandler (self)->getActions().get (AccessibilityActionType::cancel))
            {
                actionCallback();
                return YES;
            }
        }

        return NO;
    }

    static BOOL accessibilityPerformPick (id self, SEL)
    {
        return [self accessibilityPerformPress];
    }

    static BOOL accessibilityPerformPress (id self, SEL)
    {
        if (isValid (self))
        {
            if (auto actionCallback = getAccessibilityHandler (self)->getActions().get (AccessibilityActionType::press))
            {
                actionCallback();
                return YES;
            }
        }

        return NO;
    }

    static BOOL accessibilityPerformShowMenu (id self, SEL)
    {
        if (isValid (self))
        {
            if (auto actionCallback = getAccessibilityHandler (self)->getActions().get (AccessibilityActionType::showMenu))
            {
                actionCallback();
                return YES;
            }
        }

        return NO;
    }

    //==============================================================================
    static bool isTextSelector (SEL selector)
    {
        static const std::array<SEL, 18> textSelectors
        {
            @selector (accessibilityInsertionPointLineNumber),
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
            @selector (setAccessibilitySelectedTextRange:)
        };

        return std::find (std::begin (textSelectors), std::end (textSelectors), selector)
                != std::end (textSelectors);
    }

    static bool isTableSelector (SEL selector)
    {
        static const std::array<SEL, 7> tableSelectors
        {
            @selector (accessibilityRows),
            @selector (accessibilityColumnCount),
            @selector (accessibilityRowCount),
            @selector (accessibilitySortDirection),
            @selector (accessibilitySelectedCells),
            @selector (accessibilityVisibleCells),
            @selector (accessibilityCellForColumn:row:)
        };

        return std::find (std::begin (tableSelectors), std::end (tableSelectors), selector)
                != std::end (tableSelectors);
    }

    static bool isCellSelector (SEL selector)
    {
        static const std::array<SEL, 4> cellSelectors
        {
            @selector (accessibilityRowIndexRange),
            @selector (accessibilityColumnIndexRange),
            @selector (accessibilityIndex),
            @selector (accessibilityDisclosureLevel)
        };

        return std::find (std::begin (cellSelectors), std::end (cellSelectors), selector)
                != std::end (cellSelectors);
    }

    static bool isValueSelector (SEL selector)
    {
        static const std::array<SEL, 5> valueSelectors
        {
            @selector (accessibilityValue),
            @selector (setAccessibilityValue:),
            @selector (accessibilityPerformDecrement),
            @selector (accessibilityPerformDelete),
            @selector (accessibilityPerformIncrement)
        };

        return std::find (std::begin (valueSelectors), std::end (valueSelectors), selector)
                != std::end (valueSelectors);
    }

    static bool isActionSelector (SEL selector)
    {
        static const std::array<SEL, 8> actionSelectors
        {
            @selector (accessibilityPerformCancel),
            @selector (accessibilityPerformPick),
            @selector (accessibilityPerformPress),
            @selector (accessibilityPerformShowMenu),
            @selector (setAccessibilityFocused:),
            @selector (setAccessibilitySelected:),
            @selector (accessibilitySelectedChildren),
            @selector (setAccessibilitySelectedChildren:)
        };

        return std::find (std::begin (actionSelectors), std::end (actionSelectors), selector)
                != std::end (actionSelectors);
    }

    static BOOL canPerformValueSelector (id self, SEL selector)
    {
        auto hasEditableValue = (getValueInterface (self) != nullptr && ! getValueInterface (self)->isReadOnly());

        if (selector == @selector (accessibilityValue))
            return getValueInterface (self) != nullptr
                  || hasEditableText (self)
                  || nameIsAccessibilityValue (getAccessibilityHandler (self)->getRole());

        if (selector == @selector (setAccessibilityValue:))
            return hasEditableValue || hasEditableText (self);

        if (selector == @selector (accessibilityPerformDecrement))
            return hasEditableValue && getValueInterface (self)->isRanged();

        if (selector == @selector (accessibilityPerformDelete))
            return hasEditableValue || hasEditableText (self);

        if (selector == @selector (accessibilityPerformIncrement))
            return hasEditableValue && getValueInterface (self)->isRanged();

        return NO;
    }

    static BOOL canPerformActionSelector (id self, SEL selector)
    {
        if (selector == @selector (accessibilityPerformCancel))
            return getAccessibilityHandler (self)->getActions().contains (AccessibilityActionType::cancel);

        if (selector == @selector (accessibilityPerformPress)
            || selector == @selector (accessibilityPerformPick))
            return getAccessibilityHandler (self)->getActions().contains (AccessibilityActionType::press);

        if (selector == @selector (accessibilityPerformShowMenu))
            return getAccessibilityHandler (self)->getActions().contains (AccessibilityActionType::showMenu);

        if (selector == @selector (setAccessibilityFocused:))
            return getAccessibilityHandler (self)->getActions().contains (AccessibilityActionType::focus);

        if (selector == @selector (setAccessibilitySelected:))
            return getAccessibilityHandler (self)->getActions().contains (AccessibilityActionType::select);

        if (selector == @selector (accessibilitySelectedChildren)
            || selector == @selector (setAccessibilitySelectedChildren:))
        {
            for (id child in [self accessibilityChildren])
                if ([child isAccessibilitySelectorAllowed: @selector (setAccessibilitySelected:)])
                    return YES;

            return NO;
        }

        return NO;
    }

    static BOOL getIsAccessibilitySelectorAllowed (id self, SEL, SEL selector)
    {
        if (isValid (self))
        {
            if (isTextSelector (selector))    return getTextInterface (self) != nullptr;
            if (isTableSelector (selector))   return getTableInterface (self) != nullptr;
            if (isCellSelector (selector))    return getCellInterface (self) != nullptr;
            if (isValueSelector (selector))   return canPerformValueSelector (self, selector);
            if (isActionSelector (selector))  return canPerformActionSelector (self, selector);

            return sendSuperclassMessage<BOOL> (self, @selector (isAccessibilitySelectorAllowed:), selector);
        }

        return NO;
    }

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AccessibilityNativeHandle)
};

} // namespace juce
