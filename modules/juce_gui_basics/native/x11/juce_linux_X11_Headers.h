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

#pragma once

//==============================================================================
// If you're missing these headers, you need to install the libx11-dev package
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xresource.h>
#include <X11/Xutil.h>
#include <X11/Xmd.h>
#include <X11/keysym.h>
#include <X11/XKBlib.h>
#include <X11/cursorfont.h>
#include <unistd.h>

#if JUCE_USE_XRANDR
 // If you're missing this header, you need to install the libxrandr-dev package
 #include <X11/extensions/Xrandr.h>
#endif

#if JUCE_USE_XINERAMA
 // If you're missing this header, you need to install the libxinerama-dev package
 #include <X11/extensions/Xinerama.h>
#endif

#if JUCE_USE_XSHM
 // If you're missing these headers, you need to install the libxext-dev package
 #include <X11/extensions/XShm.h>
 #include <sys/shm.h>
 #include <sys/ipc.h>
#endif

#if JUCE_USE_XRENDER
 // If you're missing these headers, you need to install the libxrender-dev and libxcomposite-dev packages
 #include <X11/extensions/Xrender.h>
 #include <X11/extensions/Xcomposite.h>
#endif

#if JUCE_USE_XCURSOR
 // If you're missing this header, you need to install the libxcursor-dev package
 #include <X11/Xcursor/Xcursor.h>
#endif

#undef KeyPress
