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

bool isValid (const AccessibilityHandler& handler)
{
    bool valid = false;

    if (auto* provider = handler.getNativeImplementation())
    {
        valid = provider->isElementValid();
        provider->Release();
    }

    return valid;
}

//==============================================================================
static String getAutomationId (const AccessibilityHandler& handler)
{
    auto result = handler.getTitle();
    auto* parent = handler.getParent();

    while (parent != nullptr)
    {
        auto parentTitle = parent->getTitle();
        result = result + "." + (parentTitle.isNotEmpty() ? parentTitle : "<empty>");

        parent = parent->getParent();
    }

    return result;
}

static long roleToControlTypeId (AccessibilityRole roleType)
{
    static const std::map<AccessibilityRole, long> roleMap
    {
        { AccessibilityRole::button,       UIA_ButtonControlTypeId },
        { AccessibilityRole::toggleButton, UIA_CheckBoxControlTypeId },
        { AccessibilityRole::radioButton,  UIA_RadioButtonControlTypeId },
        { AccessibilityRole::comboBox,     UIA_ComboBoxControlTypeId },
        { AccessibilityRole::image,        UIA_ImageControlTypeId },
        { AccessibilityRole::slider,       UIA_SliderControlTypeId },
        { AccessibilityRole::staticText,   UIA_TextControlTypeId },
        { AccessibilityRole::editableText, UIA_EditControlTypeId },
        { AccessibilityRole::menuItem,     UIA_MenuItemControlTypeId },
        { AccessibilityRole::menuBar,      UIA_MenuBarControlTypeId },
        { AccessibilityRole::popupMenu,    UIA_WindowControlTypeId },
        { AccessibilityRole::table,        UIA_TableControlTypeId },
        { AccessibilityRole::tableHeader,  UIA_HeaderControlTypeId },
        { AccessibilityRole::column,       UIA_HeaderItemControlTypeId },
        { AccessibilityRole::row,          UIA_HeaderItemControlTypeId },
        { AccessibilityRole::cell,         UIA_DataItemControlTypeId },
        { AccessibilityRole::hyperlink,    UIA_HyperlinkControlTypeId },
        { AccessibilityRole::list,         UIA_ListControlTypeId },
        { AccessibilityRole::listItem,     UIA_ListItemControlTypeId },
        { AccessibilityRole::tree,         UIA_TreeControlTypeId },
        { AccessibilityRole::treeItem,     UIA_TreeItemControlTypeId },
        { AccessibilityRole::progressBar,  UIA_ProgressBarControlTypeId },
        { AccessibilityRole::group,        UIA_GroupControlTypeId },
        { AccessibilityRole::dialogWindow, UIA_WindowControlTypeId },
        { AccessibilityRole::window,       UIA_WindowControlTypeId },
        { AccessibilityRole::scrollBar,    UIA_ScrollBarControlTypeId },
        { AccessibilityRole::tooltip,      UIA_ToolTipControlTypeId },
        { AccessibilityRole::splashScreen, UIA_WindowControlTypeId },
        { AccessibilityRole::ignored,      UIA_CustomControlTypeId },
        { AccessibilityRole::unspecified,  UIA_CustomControlTypeId }
    };

    const auto it = roleMap.find (roleType);
    return it != roleMap.cend() ? it->second : UIA_CustomControlTypeId;
}

//==============================================================================
AccessibilityNativeHandle::AccessibilityNativeHandle (AccessibilityHandler& handler)
    : accessibilityHandler (handler)
{
}

//==============================================================================
HRESULT AccessibilityNativeHandle::QueryInterface (REFIID refId, void** result)
{
    if ((refId == __uuidof (IRawElementProviderFragmentRoot) && ! accessibilityHandler.isRoot())
        || ! isElementValid())
    {
        *result = nullptr;
        return E_NOINTERFACE;
    }

    return ComBaseClassHelper::QueryInterface (refId, result);
}

//==============================================================================
HRESULT AccessibilityNativeHandle::get_HostRawElementProvider (IRawElementProviderSimple** provider)
{
    if (provider == nullptr)
        return E_INVALIDARG;

    *provider = nullptr;

    if (! isElementValid())
        return UIA_E_ELEMENTNOTAVAILABLE;

    if (accessibilityHandler.isRoot())
        if (auto* wrapper = WindowsUIAWrapper::getInstance())
            return wrapper->hostProviderFromHwnd ((HWND) accessibilityHandler.getWindow(), provider);

    return S_OK;
}

HRESULT AccessibilityNativeHandle::get_ProviderOptions (ProviderOptions* options)
{
    if (options == nullptr)
        return E_INVALIDARG;

    if (! isElementValid())
        return UIA_E_ELEMENTNOTAVAILABLE;

    *options = ProviderOptions_ServerSideProvider | ProviderOptions_UseComThreading;
    return S_OK;
}

HRESULT AccessibilityNativeHandle::GetPatternProvider (PATTERNID pId, IUnknown** provider)
{
    if (provider == nullptr)
        return E_INVALIDARG;

    *provider = nullptr;

    if (! isElementValid())
        return UIA_E_ELEMENTNOTAVAILABLE;

    *provider = [&]() -> IUnknown*
    {
        auto role = accessibilityHandler.getRole();

        switch (pId)
        {
            case UIA_WindowPatternId:
            {
                if (role == AccessibilityRole::window || role == AccessibilityRole::dialogWindow)
                    return new UIAWindowProvider (accessibilityHandler);

                break;
            }
            case UIA_TextPatternId:
            case UIA_TextPattern2Id:
            {
                if (accessibilityHandler.getTextInterface() != nullptr)
                    return new UIATextProvider (accessibilityHandler);

                break;
            }
            case UIA_ValuePatternId:
            {
                auto isEditableText = (role == AccessibilityRole::editableText
                                       && accessibilityHandler.getTextInterface() != nullptr);

                if (accessibilityHandler.getValueInterface() != nullptr || isEditableText)
                    return new UIAValueProvider (accessibilityHandler, isEditableText);

                break;
            }
            case UIA_RangeValuePatternId:
            {
                if (accessibilityHandler.getValueInterface() != nullptr
                    && accessibilityHandler.getValueInterface()->isRanged())
                {
                    return new UIARangeValueProvider (accessibilityHandler);
                }

                break;
            }
            case UIA_TogglePatternId:
            {
                if (accessibilityHandler.getActions().contains (AccessibilityActionType::toggle))
                {
                    return new UIAToggleProvider (accessibilityHandler);
                }

                break;
            }
            case UIA_SelectionPatternId:
            {
                if (role == AccessibilityRole::list)
                    return new UIASelectionProvider (accessibilityHandler);

                break;
            }
            case UIA_SelectionItemPatternId:
            {
                if (role == AccessibilityRole::listItem
                    && accessibilityHandler.getActions().contains (AccessibilityActionType::select))
                {
                    return new UIASelectionItemProvider (accessibilityHandler);
                }

                break;
            }
            case UIA_GridPatternId:
            {
                if ((role == AccessibilityRole::table || role == AccessibilityRole::tree)
                    && accessibilityHandler.getTableInterface() != nullptr)
                {
                    return new UIAGridProvider (accessibilityHandler);
                }

                break;
            }
            case UIA_GridItemPatternId:
            {
                if ((role == AccessibilityRole::cell || role == AccessibilityRole::treeItem)
                    && accessibilityHandler.getCellInterface() != nullptr)
                {
                    return new UIAGridItemProvider (accessibilityHandler);
                }

                break;
            }
            case UIA_InvokePatternId:
            {
                if (accessibilityHandler.getActions().contains (AccessibilityActionType::press))
                    return new UIAInvokeProvider (accessibilityHandler);

                break;
            }
            case UIA_ExpandCollapsePatternId:
            {
                if (accessibilityHandler.getActions().contains (AccessibilityActionType::showMenu))
                    return new UIAExpandCollapseProvider (accessibilityHandler);

                break;
            }
            default:
                break;
        }

        return nullptr;
    }();

    return S_OK;
}

HRESULT AccessibilityNativeHandle::GetPropertyValue (PROPERTYID propertyId, VARIANT* pRetVal)
{
    if (pRetVal == nullptr)
        return E_INVALIDARG;

    VariantHelpers::clear (pRetVal);

    if (! isElementValid())
        return UIA_E_ELEMENTNOTAVAILABLE;

    switch (propertyId)
    {
    case UIA_AutomationIdPropertyId:
        VariantHelpers::setString (getAutomationId (accessibilityHandler), pRetVal);
        break;
    case UIA_ControlTypePropertyId:
        VariantHelpers::setInt (roleToControlTypeId (accessibilityHandler.getRole()), pRetVal);
        break;
    case UIA_FrameworkIdPropertyId:
        VariantHelpers::setString ("JUCE", pRetVal);
        break;
    case UIA_FullDescriptionPropertyId:
        VariantHelpers::setString (accessibilityHandler.getDescription(), pRetVal);
        break;
    case UIA_HasKeyboardFocusPropertyId:
        VariantHelpers::setBool (accessibilityHandler.getCurrentState().isFocused(), pRetVal);
        break;
    case UIA_HelpTextPropertyId:
        VariantHelpers::setString (accessibilityHandler.getHelp(), pRetVal);
        break;
    case UIA_IsContentElementPropertyId: JUCE_FALLTHROUGH
    case UIA_IsControlElementPropertyId:
        VariantHelpers::setBool (! accessibilityHandler.getCurrentState().isIgnored()
                                 && accessibilityHandler.getRole() != AccessibilityRole::ignored, pRetVal);
        break;
    case UIA_IsDialogPropertyId:
        VariantHelpers::setBool (accessibilityHandler.getRole() == AccessibilityRole::dialogWindow, pRetVal);
        break;
    case UIA_IsEnabledPropertyId:
        VariantHelpers::setBool (! accessibilityHandler.getCurrentState().isDisabled(), pRetVal);
        break;
    case UIA_IsKeyboardFocusablePropertyId:
        VariantHelpers::setBool (accessibilityHandler.getActions().contains (AccessibilityActionType::focus), pRetVal);
        break;
    case UIA_IsOffscreenPropertyId:
        VariantHelpers::setBool (false, pRetVal);
        break;
    case UIA_IsPasswordPropertyId:
        if (auto* textInterface = accessibilityHandler.getTextInterface())
            VariantHelpers::setBool (textInterface->isDisplayingProtectedText(), pRetVal);

        break;
    case UIA_IsPeripheralPropertyId:
        VariantHelpers::setBool (accessibilityHandler.getRole() == AccessibilityRole::tooltip
                                 || accessibilityHandler.getRole() == AccessibilityRole::popupMenu
                                 || accessibilityHandler.getRole() == AccessibilityRole::splashScreen,
                                 pRetVal);

        break;
    case UIA_NamePropertyId:
        VariantHelpers::setString (getElementName(), pRetVal);
        break;
    case UIA_ProcessIdPropertyId:
        VariantHelpers::setInt ((int) GetCurrentProcessId(), pRetVal);
        break;

    default:
        break;
    }

    return S_OK;
}

//==============================================================================
HRESULT AccessibilityNativeHandle::Navigate (NavigateDirection direction, IRawElementProviderFragment** pRetVal)
{
    if (pRetVal == nullptr)
        return E_INVALIDARG;

    *pRetVal = nullptr;

    if (! isElementValid())
        return UIA_E_ELEMENTNOTAVAILABLE;

    auto* handler = [&]() -> const AccessibilityHandler*
    {
        if (direction == NavigateDirection_Parent)
            return accessibilityHandler.getParent();

        if (direction == NavigateDirection_NextSibling)
            return getSibling (SiblingNavigationDirection::forwards);

        if (direction == NavigateDirection_PreviousSibling)
            return getSibling (SiblingNavigationDirection::backwards);

        if (direction == NavigateDirection_FirstChild || direction == NavigateDirection_LastChild)
        {
            auto children = accessibilityHandler.getChildrenInNavigationOrder();

            if (! children.empty())
                return direction == NavigateDirection_FirstChild ? children.front() : children.back();
        }

        return nullptr;
    }();

    if (handler != nullptr)
        *pRetVal = static_cast<IRawElementProviderFragment*> (handler->getNativeImplementation());

    return S_OK;
}

HRESULT AccessibilityNativeHandle::GetRuntimeId (SAFEARRAY** pRetVal)
{
    if (pRetVal == nullptr)
        return E_INVALIDARG;

    *pRetVal = nullptr;

    if (! isElementValid())
        return UIA_E_ELEMENTNOTAVAILABLE;

    int rtId[] = { 3, rtid };

    *pRetVal = SafeArrayCreateVector (VT_I4, 0, 2);

    for (LONG i = 0; i < 2; ++i)
        SafeArrayPutElement (*pRetVal, &i, &rtId[i]);

    return S_OK;
}

HRESULT AccessibilityNativeHandle::get_BoundingRectangle (UiaRect* pRetVal)
{
    if (pRetVal == nullptr)
        return E_INVALIDARG;

    if (! isElementValid())
        return UIA_E_ELEMENTNOTAVAILABLE;

    auto bounds = accessibilityHandler.convertToNativeBounds (accessibilityHandler.getScreenBounds());

    pRetVal->left   = bounds.getX();
    pRetVal->top    = bounds.getY();
    pRetVal->width  = bounds.getWidth();
    pRetVal->height = bounds.getHeight();

    return S_OK;
}

HRESULT AccessibilityNativeHandle::GetEmbeddedFragmentRoots (SAFEARRAY** pRetVal)
{
    if (pRetVal == nullptr)
        return E_INVALIDARG;

    if (! isElementValid())
        return UIA_E_ELEMENTNOTAVAILABLE;

    *pRetVal = nullptr;
    return S_OK;
}

HRESULT AccessibilityNativeHandle::SetFocus()
{
    if (! isElementValid())
        return UIA_E_ELEMENTNOTAVAILABLE;

    if (auto actionCallback = accessibilityHandler.getActions().get (AccessibilityActionType::focus))
        actionCallback();

    return S_OK;
}

HRESULT AccessibilityNativeHandle::get_FragmentRoot (IRawElementProviderFragmentRoot** pRetVal)
{
    if (pRetVal == nullptr)
        return E_INVALIDARG;

    *pRetVal = nullptr;

    if (! isElementValid())
        return UIA_E_ELEMENTNOTAVAILABLE;

    if (auto* handler = accessibilityHandler.getRoot())
    {
        *pRetVal = static_cast<IRawElementProviderFragmentRoot*> (handler->getNativeImplementation());
        return S_OK;
    }

    return UIA_E_ELEMENTNOTAVAILABLE;
}

//==============================================================================
HRESULT AccessibilityNativeHandle::ElementProviderFromPoint (double x, double y, IRawElementProviderFragment** pRetVal)
{
    if (pRetVal == nullptr)
        return E_INVALIDARG;

    *pRetVal = nullptr;

    if (! isElementValid())
        return UIA_E_ELEMENTNOTAVAILABLE;

    auto jucePoint = accessibilityHandler.convertFromNativePoint ({ roundToInt (x), roundToInt (y) });

    if (auto* handler = accessibilityHandler.getChildAt (jucePoint))
        *pRetVal = static_cast<IRawElementProviderFragment*> (handler->getNativeImplementation());

    return S_OK;
}

HRESULT AccessibilityNativeHandle::GetFocus (IRawElementProviderFragment** pRetVal)
{
    if (pRetVal == nullptr)
        return E_INVALIDARG;

    *pRetVal = nullptr;

    if (! isElementValid())
        return UIA_E_ELEMENTNOTAVAILABLE;

    auto* focusHandler = [this]
    {
        if (accessibilityHandler.getCurrentState().isBlockedByModal())
        {
            if (auto* modal = accessibilityHandler.getModal())
            {
                if (auto* focused = modal->getFocusedChild())
                    return focused;

                return modal;
            }
        }

        return accessibilityHandler.getFocusedChild();
    }();

    if (focusHandler != nullptr)
        *pRetVal = static_cast<IRawElementProviderFragment*> (focusHandler->getNativeImplementation());

    return S_OK;
}

//==============================================================================
const AccessibilityHandler* AccessibilityNativeHandle::getSibling (SiblingNavigationDirection direction) const
{
    if (auto* parentHandler = accessibilityHandler.getParent())
    {
        const auto siblings = parentHandler->getChildrenInNavigationOrder();
        const auto it = std::find (siblings.cbegin(), siblings.cend(), &accessibilityHandler);

        if (it == siblings.end())
            return nullptr;

        if (direction == SiblingNavigationDirection::forwards && it != std::prev (siblings.cend()))
            return *std::next (it);

        if (direction == SiblingNavigationDirection::backwards && it != siblings.cbegin())
            return *std::prev (it);
    }

    return nullptr;
}

String AccessibilityNativeHandle::getElementName() const
{
    auto name = accessibilityHandler.getTitle();

    if (name.isEmpty() && accessibilityHandler.isRoot())
        if (auto* app = JUCEApplicationBase::getInstance())
            return app->getApplicationName();

    return name;
}

} // namespace juce
