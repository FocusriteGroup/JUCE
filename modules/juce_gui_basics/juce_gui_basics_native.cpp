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

#ifdef JUCE_GUI_BASICS_H_INCLUDED
 /* When you add this cpp file to your project, you mustn't include it in a file where you've
    already included any other headers - just put it inside a file on its own, possibly with your config
    flags preceding it, but don't include anything else. That also includes avoiding any automatic prefix
    header files that the compiler may be using.
 */
 #error "Incorrect use of JUCE cpp file"
#endif

#define JUCE_CORE_INCLUDE_OBJC_HELPERS 1
#define JUCE_CORE_INCLUDE_COM_SMART_PTR 1
#define JUCE_CORE_INCLUDE_JNI_HELPERS 1
#define JUCE_CORE_INCLUDE_NATIVE_HEADERS 1
#define JUCE_EVENTS_INCLUDE_WIN32_MESSAGE_WINDOW 1
#define JUCE_GRAPHICS_INCLUDE_COREGRAPHICS_HELPERS 1
#define JUCE_GUI_BASICS_INCLUDE_XHEADERS 1

#define NS_FORMAT_FUNCTION(F,A) // To avoid spurious warnings from GCC

#include "juce_gui_basics.h"

//==============================================================================
#include "components/juce_ScalingHelpers.h"

#if JUCE_IOS || JUCE_WINDOWS
 #include "native/juce_MultiTouchMapper.h"
#endif

//==============================================================================
#if JUCE_MAC
 #import <IOKit/pwr_mgt/IOPMLib.h>

 #if JUCE_SUPPORT_CARBON
  #import <Carbon/Carbon.h> // still needed for SetSystemUIMode()
 #endif

 JUCE_BEGIN_IGNORE_WARNINGS_GCC_LIKE ("-Wundeclared-selector")

 #include "native/juce_mac_NSViewComponentPeer.mm"
 #include "native/juce_mac_Windowing.mm"
 #include "native/juce_mac_MainMenu.mm"
 #include "native/juce_mac_FileChooser.mm"

 JUCE_END_IGNORE_WARNINGS_GCC_LIKE

#elif JUCE_IOS
 #if JUCE_PUSH_NOTIFICATIONS && defined (__IPHONE_10_0) && __IPHONE_OS_VERSION_MAX_ALLOWED >= __IPHONE_10_0
  #import <UserNotifications/UserNotifications.h>
 #endif

 #import <UIKit/UIActivityViewController.h>

 JUCE_BEGIN_IGNORE_WARNINGS_GCC_LIKE ("-Wundeclared-selector")

 #include "native/juce_ios_UIViewComponentPeer.mm"
 #include "native/juce_ios_Windowing.mm"
 #include "native/juce_ios_FileChooser.mm"

 #if JUCE_CONTENT_SHARING
  #include "native/juce_ios_ContentSharer.cpp"
 #endif

 JUCE_END_IGNORE_WARNINGS_GCC_LIKE

#elif JUCE_WINDOWS
 #include <windowsx.h>
 #include <vfw.h>
 #include <commdlg.h>

 #if JUCE_MINGW
  #include <imm.h>
 #elif ! JUCE_DONT_AUTOLINK_TO_WIN32_LIBRARIES
  #pragma comment(lib, "vfw32.lib")
  #pragma comment(lib, "imm32.lib")

  #if JUCE_OPENGL
   #pragma comment(lib, "OpenGL32.Lib")
   #pragma comment(lib, "GlU32.Lib")
  #endif

  #if JUCE_DIRECT2D
   #pragma comment (lib, "Dwrite.lib")
   #pragma comment (lib, "D2d1.lib")
  #endif
 #endif

 #include "native/juce_win32_Windowing.cpp"
 #include "native/juce_win32_DragAndDrop.cpp"
 #include "native/juce_win32_FileChooser.cpp"

#elif JUCE_LINUX
 #include "native/x11/juce_linux_X11_Symbols.cpp"
 #include "native/x11/juce_linux_X11_DragAndDrop.cpp"

 JUCE_BEGIN_IGNORE_WARNINGS_GCC_LIKE ("-Wzero-as-null-pointer-constant")

 #include "native/juce_linux_Windowing.cpp"
 #include "native/x11/juce_linux_XWindowSystem.cpp"

 JUCE_END_IGNORE_WARNINGS_GCC_LIKE

 #include "native/juce_linux_FileChooser.cpp"

#elif JUCE_ANDROID
 #include "native/juce_android_Windowing.cpp"
 #include "native/juce_common_MimeTypes.cpp"
 #include "native/juce_android_FileChooser.cpp"

 #if JUCE_CONTENT_SHARING
  #include "native/juce_android_ContentSharer.cpp"
 #endif

#endif
