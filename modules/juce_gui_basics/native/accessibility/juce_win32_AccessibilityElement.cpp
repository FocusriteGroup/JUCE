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

int AccessibilityNativeHandle::idCounter = 0;

bool isValid (const AccessibilityHandler& handler)
{
    bool valid = false;

    if (auto* provider = handler.getNativeImplementation())
        valid = provider->isElementValid();

    return valid;
}

//==============================================================================
static String getAutomationId (const AccessibilityHandler& handler)
{
    auto result = handler.getTitle();
    auto* parentComponent = handler.getComponent().getParentComponent();

    while (parentComponent != nullptr)
    {
        if (auto* parentHandler = parentComponent->getAccessibilityHandler())
        {
            auto parentTitle = parentHandler->getTitle();
            result = result + "." + (parentTitle.isNotEmpty() ? parentTitle : "<empty>");
        }

        parentComponent = parentComponent->getParentComponent();
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
    : ComBaseClassHelper<IRawElementProviderSimple,
                        IRawElementProviderFragment,
                        IRawElementProviderFragmentRoot> (0),
      accessibilityHandler (handler),
      isFragmentRoot (accessibilityHandler.getParent() == nullptr)
{
}

//==============================================================================
HRESULT AccessibilityNativeHandle::QueryInterface (REFIID refId, void** result)
{
    *result = nullptr;

    if (! isElementValid())
        return UIA_E_ELEMENTNOTAVAILABLE;

    if ((refId == __uuidof (IRawElementProviderFragmentRoot) && ! isFragmentRoot))
        return E_NOINTERFACE;

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

    if (isFragmentRoot)
        if (auto* wrapper = WindowsUIAWrapper::getInstanceWithoutCreating())
            return wrapper->hostProviderFromHwnd ((HWND) accessibilityHandler.getComponent().getWindowHandle(), provider);

    return S_OK;
}

HRESULT AccessibilityNativeHandle::get_ProviderOptions (ProviderOptions* options)
{
    if (options == nullptr)
        return E_INVALIDARG;

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
                if (isFragmentRoot)
                    return new UIAWindowProvider (accessibilityHandler);

                break;
            }
            case UIA_TransformPatternId:
            {
                if (isFragmentRoot)
                    return new UIATransformProvider (accessibilityHandler);

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
        VariantHelpers::setInt (isFragmentRoot ? UIA_WindowControlTypeId
                                               : roleToControlTypeId (accessibilityHandler.getRole()),
                                pRetVal);
        break;
    case UIA_FrameworkIdPropertyId:
        VariantHelpers::setString ("JUCE", pRetVal);
        break;
    case UIA_FullDescriptionPropertyId:
        VariantHelpers::setString (accessibilityHandler.getDescription(), pRetVal);
        break;
    case UIA_HelpTextPropertyId:
        VariantHelpers::setString (accessibilityHandler.getHelp(), pRetVal);
        break;
    case UIA_IsContentElementPropertyId:
        VariantHelpers::setBool (! accessibilityHandler.isIgnored(), pRetVal);
        break;
    case UIA_IsControlElementPropertyId:
        VariantHelpers::setBool (true, pRetVal);
        break;
    case UIA_IsDialogPropertyId:
        VariantHelpers::setBool (accessibilityHandler.getRole() == AccessibilityRole::dialogWindow, pRetVal);
        break;
    case UIA_IsEnabledPropertyId:
        VariantHelpers::setBool (accessibilityHandler.getComponent().isEnabled(), pRetVal);
        break;
    case UIA_IsKeyboardFocusablePropertyId:
        VariantHelpers::setBool (isFragmentRoot || accessibilityHandler.getComponent().getWantsKeyboardFocus(),
                                 pRetVal);
        break;
    case UIA_HasKeyboardFocusPropertyId:
        VariantHelpers::setBool ((isFragmentRoot && accessibilityHandler.getComponent().getPeer()->isFocused())
                                 || accessibilityHandler.getComponent().hasKeyboardFocus (true),
                                 pRetVal);
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
    case UIA_NativeWindowHandlePropertyId:
        if (isFragmentRoot)
            VariantHelpers::setInt ((int) (pointer_sized_int) accessibilityHandler.getComponent().getWindowHandle(), pRetVal);

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

    auto* handler = [&]() -> AccessibilityHandler*
    {
        if (direction == NavigateDirection_Parent)
        {
            return accessibilityHandler.getParent();
        }
        else if (direction == NavigateDirection_FirstChild
                 || direction == NavigateDirection_LastChild)
        {
            auto children = accessibilityHandler.getChildren();

            if (! children.empty())
                return (direction == NavigateDirection_FirstChild ? children.front()
                                                                  : children.back());
        }
        else if (direction == NavigateDirection_NextSibling
                 || direction == NavigateDirection_PreviousSibling)
        {
            if (auto* parent = accessibilityHandler.getParent())
            {
                const auto siblings = parent->getChildren();
                const auto iter = std::find (siblings.cbegin(), siblings.cend(), &accessibilityHandler);

                if (iter == siblings.end())
                    return nullptr;

                if (direction == NavigateDirection_NextSibling && iter != std::prev (siblings.cend()))
                    return *std::next (iter);

                if (direction == NavigateDirection_PreviousSibling && iter != siblings.cbegin())
                    return *std::prev (iter);
            }
        }

        return nullptr;
    }();

    if (handler != nullptr && isValid (*handler))
        handler->getNativeImplementation()->QueryInterface (IID_PPV_ARGS (pRetVal));

    return S_OK;
}

HRESULT AccessibilityNativeHandle::GetRuntimeId (SAFEARRAY** pRetVal)
{
    if (pRetVal == nullptr)
        return E_INVALIDARG;

    *pRetVal = nullptr;

    if (! isElementValid())
        return UIA_E_ELEMENTNOTAVAILABLE;

    if (! isFragmentRoot)
    {
        *pRetVal = SafeArrayCreateVector (VT_I4, 0, 2);

        if (*pRetVal == nullptr)
            return E_OUTOFMEMORY;

        for (LONG i = 0; i < 2; ++i)
        {
            auto hr = SafeArrayPutElement (*pRetVal, &i, &rtid[i]);

            if (FAILED (hr))
                return E_FAIL;
        }
    }

    return S_OK;
}

HRESULT AccessibilityNativeHandle::get_BoundingRectangle (UiaRect* pRetVal)
{
    if (pRetVal == nullptr)
        return E_INVALIDARG;

    if (! isElementValid())
        return UIA_E_ELEMENTNOTAVAILABLE;

    auto bounds = Desktop::getInstance().getDisplays().logicalToPhysical (accessibilityHandler.getComponent().getScreenBounds());

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

    auto& component = accessibilityHandler.getComponent();

    if (component.getWantsKeyboardFocus())
        accessibilityHandler.grabFocus();

    if (auto actionCallback = accessibilityHandler.getActions().get (AccessibilityActionType::raise))
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

    auto* handler = [&]() -> AccessibilityHandler*
    {
        if (isFragmentRoot)
            return &accessibilityHandler;

        if (auto* peer = accessibilityHandler.getComponent().getPeer())
            if (auto* handler = peer->getComponent().getAccessibilityHandler())
                return handler;

        return nullptr;
    }();

    if (handler != nullptr)
    {
        handler->getNativeImplementation()->QueryInterface (IID_PPV_ARGS (pRetVal));
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

    auto* handler = [&]
    {
        auto logicalScreenPoint = Desktop::getInstance().getDisplays()
                                    .physicalToLogical (Point<int> (roundToInt (x),
                                                                    roundToInt (y)));

        if (auto* child = accessibilityHandler.getChildAt (logicalScreenPoint))
            return child;

        return &accessibilityHandler;
    }();

    handler->getNativeImplementation()->QueryInterface (IID_PPV_ARGS (pRetVal));
    return S_OK;
}

HRESULT AccessibilityNativeHandle::GetFocus (IRawElementProviderFragment** pRetVal)
{
    if (pRetVal == nullptr)
        return E_INVALIDARG;

    *pRetVal = nullptr;

    if (! isElementValid())
        return UIA_E_ELEMENTNOTAVAILABLE;

    if (auto* focus = accessibilityHandler.getFocus())
        focus->getNativeImplementation()->QueryInterface (IID_PPV_ARGS (pRetVal));

    return S_OK;
}

//==============================================================================
String AccessibilityNativeHandle::getElementName() const
{
    auto name = accessibilityHandler.getTitle();

    if (name.isEmpty() && isFragmentRoot)
        if (auto* app = JUCEApplicationBase::getInstance())
            return app->getApplicationName();

    return name;
}

} // namespace juce
