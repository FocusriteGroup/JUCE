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


/*******************************************************************************
 The block below describes the properties of this module, and is read by
 the Projucer to automatically generate project code that uses it.
 For details about the syntax and how to create or use a module, see the
 JUCE Module Format.md file.


 BEGIN_JUCE_MODULE_DECLARATION

  ID:                 juce_accessibility
  vendor:             juce
  version:            6.0.4
  name:               JUCE accessibility classes
  description:        Fundamental classes for accessibility support.
  website:            http://www.juce.com/juce
  license:            ISC

  dependencies:       juce_graphics
  OSXFrameworks:      AppKit

 END_JUCE_MODULE_DECLARATION

*******************************************************************************/


#pragma once
#define JUCE_ACCESSIBILITY_H_INCLUDED

#include <juce_graphics/juce_graphics.h>

namespace juce
{
    class AccessibilityHandler;
}

#include "interfaces/juce_AccessibilityCellInterface.h"
#include "interfaces/juce_AccessibilityTableInterface.h"
#include "interfaces/juce_AccessibilityTextInterface.h"
#include "interfaces/juce_AccessibilityValueInterface.h"
#include "enums/juce_AccessibilityActions.h"
#include "enums/juce_AccessibilityEvent.h"
#include "enums/juce_AccessibilityRole.h"
#include "handler/juce_AccessibilityState.h"
#include "handler/juce_AccessibilityHandler.h"
