cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

# ------------------------------------------------------------------------------
# Include cmake-common
# ------------------------------------------------------------------------------
        
set(CMAKE_MODULE_PATH 
    ${CMAKE_MODULE_PATH}
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/cmake-common
)

include(cmake-common)

# ------------------------------------------------------------------------------
# Add CMAKE_BUILD_TYPE variable
# ------------------------------------------------------------------------------
        
if (NOT CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE "Debug"  CACHE STRING "Choose the type of build." FORCE)
endif (NOT CMAKE_BUILD_TYPE)

# ------------------------------------------------------------------------------
# Source files
# ------------------------------------------------------------------------------
        
file(GLOB_RECURSE JUCE_MODULE_HEADERS CONFIGURE_DEPENDS modules/*.h)
file(GLOB_RECURSE JUCE_MODULE_SOURCES CONFIGURE_DEPENDS modules/*.cpp)
if (APPLE)
    file(GLOB_RECURSE JUCE_OBJC_SOURCES CONFIGURE_DEPENDS modules/*.mm)
    set(JUCE_MODULE_SOURCES ${JUCE_MODULE_SOURCES} ${JUCE_OBJC_SOURCES})
endif()

# These files are added to the IDE, but are not compiled, since they are included
# by the JuceLibraryCode/ versions
# Set the HEADER_FILE_ONLY to ensure that they're not compiled
foreach(SOURCE ${JUCE_MODULE_HEADERS} ${JUCE_MODULE_SOURCES})
    set_source_files_properties(${SOURCE} PROPERTIES HEADER_FILE_ONLY TRUE)
endforeach(SOURCE)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/modules
    PREFIX juce_modules 
    FILES ${JUCE_MODULE_HEADERS} ${JUCE_MODULE_SOURCES}
)

set(JUCE_LIBRARY_CODE
    JuceLibraryCode/juce/JuceAudio.h
    JuceLibraryCode/juce/JuceCore.h
    JuceLibraryCode/juce/JuceGraphics.h
    JuceLibraryCode/juce/JuceHeader.h
)

if (APPLE)
    set(JUCE_LIBRARY_CODE
        ${JUCE_LIBRARY_CODE}
        JuceLibraryCode/juce/include_juce_accessibility.mm
        JuceLibraryCode/juce/include_juce_analytics.cpp
        JuceLibraryCode/juce/include_juce_audio_basics.mm
        JuceLibraryCode/juce/include_juce_audio_devices.mm
        JuceLibraryCode/juce/include_juce_audio_formats.mm
        JuceLibraryCode/juce/include_juce_audio_processors.mm
        JuceLibraryCode/juce/include_juce_audio_utils.mm
        JuceLibraryCode/juce/include_juce_core.mm
        JuceLibraryCode/juce/include_juce_data_structures.mm
        JuceLibraryCode/juce/include_juce_dsp.mm
        JuceLibraryCode/juce/include_juce_events.mm
        JuceLibraryCode/juce/include_juce_graphics.mm
        JuceLibraryCode/juce/include_juce_gui_basics.mm
        JuceLibraryCode/juce/include_juce_gui_basics_native.mm
        JuceLibraryCode/juce/include_juce_gui_extra.mm
    )
else()
    set(JUCE_LIBRARY_CODE
        ${JUCE_LIBRARY_CODE}
        JuceLibraryCode/juce/include_juce_accessibility.cpp
        JuceLibraryCode/juce/include_juce_analytics.cpp
        JuceLibraryCode/juce/include_juce_audio_basics.cpp
        JuceLibraryCode/juce/include_juce_audio_devices.cpp
        JuceLibraryCode/juce/include_juce_audio_formats.cpp
        JuceLibraryCode/juce/include_juce_audio_processors.cpp
        JuceLibraryCode/juce/include_juce_audio_utils.cpp
        JuceLibraryCode/juce/include_juce_core.cpp
        JuceLibraryCode/juce/include_juce_data_structures.cpp
        JuceLibraryCode/juce/include_juce_dsp.cpp
        JuceLibraryCode/juce/include_juce_events.cpp
        JuceLibraryCode/juce/include_juce_graphics.cpp
        JuceLibraryCode/juce/include_juce_gui_basics.cpp
        JuceLibraryCode/juce/include_juce_gui_basics_native.cpp
        JuceLibraryCode/juce/include_juce_gui_extra.cpp
    )
endif()

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/JuceLibraryCode 
    PREFIX JuceLibraryCode 
    FILES ${JUCE_LIBRARY_CODE}
)

# ------------------------------------------------------------------------------
# jucelib library
# ------------------------------------------------------------------------------
        
add_library(jucelib 
    ${JUCE_LIBRARY_CODE}
    ${JUCE_MODULE_HEADERS}
    ${JUCE_MODULE_SOURCES}
)

target_include_directories(jucelib
    INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/JuceLibraryCode
    ${CMAKE_CURRENT_SOURCE_DIR}/modules
    PRIVATE
    modules
)

target_compile_definitions (jucelib PRIVATE
    $<IF:$<CONFIG:Debug>,_DEBUG=1,_NDEBUG=1>
    $<IF:$<CONFIG:Debug>,DEBUG=1,NDEBUG=1>
    "JUCE_DONT_DECLARE_PROJECTINFO=1"
    "JUCER_XCODE_MAC_75C930FD=1"
    "JUCE_APP_VERSION=1.0.0"
    "JUCE_APP_VERSION_HEX=0x10000"
    "JUCE_COREGRAPHICS_DRAW_ASYNC=1"
    "JucePlugin_Build_VST=0"
    "JucePlugin_Build_VST3=0"
    "JucePlugin_Build_AU=0"
    "JucePlugin_Build_AUv3=0"
    "JucePlugin_Build_RTAS=0"
    "JucePlugin_Build_AAX=0"
    "JucePlugin_Build_Standalone=0"
    "JucePlugin_Build_Unity=0"
)

target_compile_definitions (jucelib PUBLIC
    "JUCE_USE_MP3AUDIOFORMAT=1"
)

set(MACOS_DEPLOYMENT_TARGET 10.12)

set_target_properties(jucelib PROPERTIES
    OUTPUT_NAME                                                 "Juce"
    CXX_STANDARD                                                14
    CXX_EXTENSIONS                                              OFF
    XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_WEAK                      "YES"
    XCODE_ATTRIBUTE_CLANG_LINK_OBJC_RUNTIME                     NO
    XCODE_ATTRIBUTE_CLANG_WARN_DEPRECATED_OBJC_IMPLEMENTATIONS  "YES"
    XCODE_ATTRIBUTE_CLANG_WARN_OBJC_IMPLICIT_RETAIN_SELF        "YES"
    XCODE_ATTRIBUTE_COMBINE_HIDPI_IMAGES                        YES
    XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR                     "$(BUILD_DIR)/$(CONFIGURATION)$(EFFECTIVE_PLATFORM_NAME)"
    XCODE_ATTRIBUTE_COPY_PHASE_STRIP                            NO
    XCODE_ATTRIBUTE_GCC_DYNAMIC_NO_PIC                          NO
    XCODE_ATTRIBUTE_GCC_VERSION                                 com.apple.compilers.llvm.clang.1_0
    XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER                   com.ampify.jucelib
    XCODE_ATTRIBUTE_PRODUCT_NAME                                "Juce"
    XCODE_ATTRIBUTE_SKIP_INSTALL                                "YES"
    XCODE_ATTRIBUTE_USE_HEADERMAP                               NO
    XCODE_ATTRIBUTE_MACOSX_DEPLOYMENT_TARGET                    ${MACOS_DEPLOYMENT_TARGET}
)

if (APPLE)
    target_link_libraries(jucelib PUBLIC
            "-framework Accelerate"
            "-framework AppKit"
            "-framework AudioToolbox"
            "-framework AVKit"
            "-framework AVFoundation"
            "-framework CoreAudio"
            "-framework CoreFoundation"
            "-framework CoreGraphics"
            "-framework CoreImage"
            "-framework CoreMedia"
            "-framework CoreMIDI"
            "-framework CoreServices"
            "-framework CoreText"
            "-framework ImageIO"
            "-framework IOKit"
            "-framework WebKit"
    )

    target_compile_options(jucelib PRIVATE
        -fno-common 
        -Wswitch 
        -Wunused-variable 
        -Wparentheses 
        -Wnon-virtual-dtor 
        -Wshorten-64-to-32 
        -Wundeclared-selector 
        -Wuninitialized 
        -Wunused-function 
        -Wbool-conversion 
        -Wcomma 
        -Wconstant-conversion 
        -Wempty-body 
        -Wenum-conversion 
        -Winfinite-recursion 
        -Wint-conversion 
        -Wrange-loop-analysis 
        -Wstrict-prototypes 
        -Wmove 
        -Wunreachable-code 
        -Wduplicate-method-match 
        -Wreorder

        -mmacosx-version-min=${MACOS_DEPLOYMENT_TARGET}
        $<IF:$<CONFIG:Debug>,-O0,-O3>
        $<IF:$<CONFIG:Debug>,,-flto>
        -stdlib=libc++
    )
elseif (MSVC)
    target_compile_options(jucelib PRIVATE
        /W4 
        $<IF:$<CONFIG:Debug>,/Od,/Ox>
        $<IF:$<CONFIG:Debug>,/MTd,/MT>  # Runtime library
    )
endif()
