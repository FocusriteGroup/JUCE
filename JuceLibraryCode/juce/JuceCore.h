#pragma once

#include "AppConfig.h"

#include <juce_analytics/juce_analytics.h>
#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>
#include <juce_events/juce_events.h>

#if ! DONT_SET_USING_JUCE_NAMESPACE
 using namespace juce;
#endif

#if ! JUCE_DONT_DECLARE_PROJECTINFO
namespace ProjectInfo
{
    const char* const  projectName    = "Juce-Lib";
    const char* const  versionString  = "1.0.0";
    const int          versionNumber  = 0x10000;
}
#endif
