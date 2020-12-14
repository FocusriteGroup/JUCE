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

#ifdef JUCE_ACCESSIBILITY_INCLUDED
 /* When you add this cpp file to your project, you mustn't include it in a file where you've
    already included any other headers - just put it inside a file on its own, possibly with your config
    flags preceding it, but don't include anything else. That also includes avoiding any automatic prefix
    header files that the compiler may be using.
 */
 #error "Incorrect use of JUCE cpp file"
#endif

#define JUCE_CORE_INCLUDE_OBJC_HELPERS 1
#define JUCE_CORE_INCLUDE_NATIVE_HEADERS 1
#define JUCE_CORE_INCLUDE_COM_SMART_PTR 1
#define JUCE_GRAPHICS_INCLUDE_COREGRAPHICS_HELPERS 1

#include "juce_accessibility.h"

#include "handler/juce_AccessibilityHandler.cpp"

#if JUCE_MAC
 #include "native/juce_mac_AccessibilityElement.mm"
 #include "native/juce_mac_Accessibility.mm"

#elif JUCE_WINDOWS
 #include <UIAutomation.h>

 #include "native/juce_win32_WindowsUIAWrapper.h"
 #include "native/juce_win32_AccessibilityElement.h"
 #include "native/juce_win32_UIAHelpers.h"
 #include "native/juce_win32_UIAProviders.h"
 #include "native/juce_win32_AccessibilityElement.cpp"
 #include "native/juce_win32_Accessibility.cpp"

#endif
