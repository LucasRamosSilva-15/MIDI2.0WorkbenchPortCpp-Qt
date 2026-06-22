cmake_minimum_required(VERSION 3.16)
project(MidiUmpAnalyzer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

option(ENABLE_RTMIDI "Habilitar dependência opcional do RtMidi" OFF)
option(ENABLE_WINDOWS_MIDI_SERVICES "Enable Windows MIDI Services native backend (Windows 10+)" OFF)
option(ENABLE_WINDOWS_MIDI_SERVICES_SDK_EXPERIMENT "Enable theoretical SDK connection paths (does not build endpoints yet)" OFF)
option(ENABLE_WINDOWS_MIDI_SERVICES_HEADER_INCLUDE_EXPERIMENT "Enable isolated Windows MIDI Services C++ header inclusion test" OFF)
option(ENABLE_WINDOWS_MIDI_SERVICES_CPPWINRT_ALIGNMENT_RESEARCH "Enable diagnostic diagnostics for C++/WinRT alignment mismatches" OFF)
option(ENABLE_WINDOWS_MIDI_SERVICES_GENERATED_PROJECTION_STRATEGY_RESEARCH "Enable diagnostic research on C++/WinRT generated projections" OFF)
option(ENABLE_WINDOWS_MIDI_SERVICES_WINRT_ACTIVATION_EXPERIMENT "Enable isolated WinRT activation experiment" OFF)
option(ENABLE_WINDOWS_MIDI_SERVICES_REAL_CPPWINRT_INCLUDE_ATTEMPT "DANGEROUS: Force real include of misaligned C++/WinRT headers" OFF)
option(ENABLE_WINDOWS_MIDI_SERVICES_REAL_WINRT_ACTIVATION_ATTEMPT "DANGEROUS: Force real initialization of WinRT COM apartments" OFF)

if(ENABLE_RTMIDI)
    include(FetchContent)
    FetchContent_Declare(
        rtmidi
        GIT_REPOSITORY https://github.com/thestk/rtmidi.git
        GIT_TAG 6.0.0
    )
    FetchContent_MakeAvailable(rtmidi)
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Criação do Executável Principal
add_executable(MidiUmpAnalyzer WIN32
    src/main.cpp
    src/MainWindow.cpp
    src/MainWindow.h
    src/UmpParser.cpp
    src/UmpParser.h
    src/midi/MidiInputController.cpp
    src/midi/MidiInputController.h
    src/midi/IMidiInputBackend.h
    src/midi/FakeMidiInputBackend.h
    src/midi/RtMidiInputBackend.cpp
    src/midi/RtMidiInputBackend.h
    src/midi/Midi1LiveDecoder.cpp
    src/midi/Midi1LiveDecoder.h
    src/midi/Midi1ToUmpPreviewConverter.cpp
    src/midi/Midi1ToUmpPreviewConverter.h
    src/core/InputSourceType.h
    src/core/MidiRawEvent.h
    src/core/UmpRawEvent.h
    src/midi/IUmpInputBackend.h
    src/midi/FakeUmpInputBackend.h
    src/midi/FakeUmpInputBackend.cpp
    src/midi/WindowsMidiServicesBackend.h
    src/midi/WindowsMidiServicesBackend.cpp
    src/midi/WindowsMidiServicesSdkProbe.h
    src/midi/WindowsMidiServicesSdkProbe.cpp
    src/midi/WindowsMidiServicesHeaderIncludeProbe.cpp
)

target_link_libraries(MidiUmpAnalyzer PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)

if(ENABLE_WINDOWS_MIDI_SERVICES)
    target_compile_definitions(MidiUmpAnalyzer PRIVATE USE_WINDOWS_MIDI_SERVICES)
endif()

if(ENABLE_WINDOWS_MIDI_SERVICES_SDK_EXPERIMENT)
    message(STATUS "Windows MIDI Services SDK experiment enabled")
    message(WARNING "This build path is experimental and may require Windows MIDI Services SDK Runtime/Tools and C++/WinRT configuration.")
    target_compile_definitions(MidiUmpAnalyzer PRIVATE USE_WINDOWS_MIDI_SERVICES_SDK_EXPERIMENT)
endif()

set(WINDOWS_MIDI_SERVICES_SDK_ROOT "" CACHE PATH "Optional user-provided Windows MIDI Services SDK root path for research builds")

if(WINDOWS_MIDI_SERVICES_SDK_ROOT STREQUAL "")
    message(STATUS "Windows MIDI Services SDK root not provided; SDK root research remains disabled.")
else()
    message(STATUS "Windows MIDI Services SDK root provided: ${WINDOWS_MIDI_SERVICES_SDK_ROOT}")
    if(EXISTS "${WINDOWS_MIDI_SERVICES_SDK_ROOT}")
        message(STATUS "Windows MIDI Services SDK root path exists. No headers or libraries are consumed in v4.7.0.")
        target_compile_definitions(MidiUmpAnalyzer PRIVATE WINDOWS_MIDI_SERVICES_SDK_ROOT_PROVIDED)
        
        # Candidate Discovery Attempted
        target_compile_definitions(MidiUmpAnalyzer PRIVATE WINDOWS_MIDI_SERVICES_SDK_CANDIDATE_DISCOVERY_ATTEMPTED)

        # Header Candidate Detection
        set(WINDOWS_MIDI_SERVICES_HEADER_CANDIDATES
            "Microsoft.Windows.Devices.Midi2.h"
            "winrt/Microsoft.Windows.Devices.Midi2.h"
            "Windows.Devices.Midi2.h"
            "winrt/Windows.Devices.Midi2.h"
        )
        
        find_path(
            WINDOWS_MIDI_SERVICES_HEADER_DIR
            NAMES ${WINDOWS_MIDI_SERVICES_HEADER_CANDIDATES}
            PATHS "${WINDOWS_MIDI_SERVICES_SDK_ROOT}"
            PATH_SUFFIXES include Include include/winrt Include/winrt winrt generated Generated share tools
            NO_DEFAULT_PATH
        )
        
        # WinMD/Metadata Candidate Detection
        set(WINDOWS_MIDI_SERVICES_WINMD_CANDIDATES
            "Microsoft.Windows.Devices.Midi2.winmd"
            "Windows.Devices.Midi2.winmd"
        )
        
        find_file(
            WINDOWS_MIDI_SERVICES_WINMD_FILE
            NAMES ${WINDOWS_MIDI_SERVICES_WINMD_CANDIDATES}
            PATHS "${WINDOWS_MIDI_SERVICES_SDK_ROOT}"
            PATH_SUFFIXES include Include metadata Metadata winmd WinMD share tools "Desktop App SDK Runtime"
            NO_DEFAULT_PATH
        )
        
        target_compile_definitions(MidiUmpAnalyzer PRIVATE WINDOWS_MIDI_SERVICES_OPTIONAL_HEADER_DETECTION_ATTEMPTED)
        
        if(WINDOWS_MIDI_SERVICES_HEADER_DIR)
            message(STATUS "Windows MIDI Services SDK candidate header detected.")
            target_compile_definitions(MidiUmpAnalyzer PRIVATE 
                WINDOWS_MIDI_SERVICES_HEADER_CANDIDATE_DETECTED
                WINDOWS_MIDI_SERVICES_OPTIONAL_HEADERS_DETECTED
            )
            
            if(ENABLE_WINDOWS_MIDI_SERVICES_HEADER_INCLUDE_EXPERIMENT)
                target_include_directories(MidiUmpAnalyzer PRIVATE "${WINDOWS_MIDI_SERVICES_HEADER_DIR}")
            endif()
            
            if(ENABLE_WINDOWS_MIDI_SERVICES_TYPE_REFERENCE_EXPERIMENT)
                message(STATUS "Windows MIDI Services type reference experiment is ENABLED and compiling.")
                target_compile_definitions(MidiUmpAnalyzer PRIVATE 
                    WINDOWS_MIDI_SERVICES_TYPE_REFERENCE_EXPERIMENT_REQUESTED
                    WINDOWS_MIDI_SERVICES_TYPE_REFERENCE_EXPERIMENT_COMPILED
                )
                target_include_directories(MidiUmpAnalyzer PRIVATE "${WINDOWS_MIDI_SERVICES_HEADER_DIR}")
                target_sources(MidiUmpAnalyzer PRIVATE src/midi/WindowsMidiServicesTypeReferenceProbe.cpp)
            endif()
            
            if(ENABLE_WINDOWS_MIDI_SERVICES_CPPWINRT_ALIGNMENT_RESEARCH)
                message(STATUS "Windows MIDI Services C++/WinRT alignment research enabled.")
                message(STATUS "This mode is diagnostic only and does not enable endpoint listing or UMP capture.")
                target_compile_definitions(MidiUmpAnalyzer PRIVATE WINDOWS_MIDI_SERVICES_CPPWINRT_ALIGNMENT_RESEARCH_ENABLED)
                
                if(ENABLE_WINDOWS_MIDI_SERVICES_GENERATED_PROJECTION_STRATEGY_RESEARCH)
                    message(STATUS "Windows MIDI Services C++/WinRT generated projection strategy research enabled.")
                    target_compile_definitions(MidiUmpAnalyzer PRIVATE WINDOWS_MIDI_SERVICES_GENERATED_PROJECTION_STRATEGY_RESEARCH_ENABLED)
                endif()
                
                if(ENABLE_WINDOWS_MIDI_SERVICES_WINRT_ACTIVATION_EXPERIMENT)
                    message(STATUS "Windows MIDI Services WinRT activation experiment enabled.")
                    target_compile_definitions(MidiUmpAnalyzer PRIVATE WINDOWS_MIDI_SERVICES_WINRT_ACTIVATION_EXPERIMENT_COMPILED)
                    target_sources(MidiUmpAnalyzer PRIVATE src/midi/WindowsMidiServicesWinRtActivationProbe.cpp)
                endif()
                
                message(STATUS "--- Toolchain Diagnostic ---")
                message(STATUS "CMAKE_TOOLCHAIN_FILE: ${CMAKE_TOOLCHAIN_FILE}")
                message(STATUS "CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")
                message(STATUS "WINDOWS_MIDI_SERVICES_SDK_ROOT: ${WINDOWS_MIDI_SERVICES_SDK_ROOT}")
                message(STATUS "WINDOWS_MIDI_SERVICES_HEADER_DIR: ${WINDOWS_MIDI_SERVICES_HEADER_DIR}")
                if(WINDOWS_MIDI_SERVICES_WINMD_FILE)
                    message(STATUS "WINDOWS_MIDI_SERVICES_WINMD_FILE: ${WINDOWS_MIDI_SERVICES_WINMD_FILE}")
                endif()
                message(STATUS "----------------------------")
                
                if(NOT CMAKE_TOOLCHAIN_FILE)
                    message(WARNING "CMAKE_TOOLCHAIN_FILE is not set. C++/WinRT generated projections may require vcpkg toolchain (-DCMAKE_TOOLCHAIN_FILE=\"C:\\vcpkg\\scripts\\buildsystems\\vcpkg.cmake\").")
                endif()
                
                if(ENABLE_WINDOWS_MIDI_SERVICES_REAL_CPPWINRT_INCLUDE_ATTEMPT)
                    message(WARNING "DANGEROUS: Real C++/WinRT include attempt is ENABLED. This may fail compilation due to CPPWINRT_VERSION mismatches or missing generated headers. Do not use in Release/CI.")
                    target_compile_definitions(MidiUmpAnalyzer PRIVATE WINDOWS_MIDI_SERVICES_ALLOW_REAL_CPPWINRT_INCLUDE_ATTEMPT)
                endif()
                
                if(ENABLE_WINDOWS_MIDI_SERVICES_REAL_WINRT_ACTIVATION_ATTEMPT)
                    message(WARNING "DANGEROUS: Real WinRT Activation attempt is ENABLED. This may crash the application on startup if the COM apartment configuration fails.")
                    target_compile_definitions(MidiUmpAnalyzer PRIVATE WINDOWS_MIDI_SERVICES_ALLOW_REAL_WINRT_ACTIVATION_ATTEMPT)
                endif()
            endif()
        else()
            if(ENABLE_WINDOWS_MIDI_SERVICES_TYPE_REFERENCE_EXPERIMENT)
                message(WARNING "Windows MIDI Services type reference experiment was requested, but no header candidate was detected. Non-fatal in v4.10.0.")
                target_compile_definitions(MidiUmpAnalyzer PRIVATE WINDOWS_MIDI_SERVICES_TYPE_REFERENCE_EXPERIMENT_REQUESTED)
            endif()
        endif()
        
        if(WINDOWS_MIDI_SERVICES_WINMD_FILE)
            message(STATUS "Windows MIDI Services SDK candidate WinMD metadata detected.")
            target_compile_definitions(MidiUmpAnalyzer PRIVATE WINDOWS_MIDI_SERVICES_WINMD_CANDIDATE_DETECTED)
        endif()
        
        if(NOT WINDOWS_MIDI_SERVICES_HEADER_DIR AND NOT WINDOWS_MIDI_SERVICES_WINMD_FILE)
            message(WARNING "Windows MIDI Services SDK candidates (headers or WinMD) were not found inside the user-provided SDK root. This is non-fatal in v4.9.0.")
        endif()
        
    else()
        message(WARNING "Windows MIDI Services SDK root was provided but does not exist. This is non-fatal in v4.9.0 research mode.")
        target_compile_definitions(MidiUmpAnalyzer PRIVATE WINDOWS_MIDI_SERVICES_SDK_ROOT_PROVIDED WINDOWS_MIDI_SERVICES_SDK_ROOT_MISSING)
    endif()
endif()

if(ENABLE_WINDOWS_MIDI_SERVICES_HEADER_INCLUDE_EXPERIMENT)
    message(STATUS "Windows MIDI Services header include experiment is ENABLED.")
    target_compile_definitions(MidiUmpAnalyzer PRIVATE USE_WINDOWS_MIDI_SERVICES_HEADER_INCLUDE_EXPERIMENT)
endif()

if(ENABLE_RTMIDI)
    target_compile_definitions(MidiUmpAnalyzer PRIVATE USE_RTMIDI)
    target_link_libraries(MidiUmpAnalyzer PRIVATE rtmidi)
endif()

# Criação do Executável de Testes
add_executable(UmpParserTests
    tests/UmpParserTests.cpp
    tests/WindowsMidiServicesBackendTests.cpp
    tests/WindowsMidiServicesBackendTests.h
    tests/TestUtils.h
    src/UmpParser.cpp
    src/UmpParser.h
    src/midi/Midi1LiveDecoder.cpp
    src/midi/Midi1LiveDecoder.h
    src/midi/Midi1ToUmpPreviewConverter.cpp
    src/midi/Midi1ToUmpPreviewConverter.h
    src/core/UmpRawEvent.h
    src/midi/IUmpInputBackend.h
    src/midi/FakeUmpInputBackend.h
    src/midi/FakeUmpInputBackend.cpp
    src/midi/WindowsMidiServicesBackend.h
    src/midi/WindowsMidiServicesBackend.cpp
    src/midi/WindowsMidiServicesSdkProbe.h
    src/midi/WindowsMidiServicesSdkProbe.cpp
    src/midi/WindowsMidiServicesHeaderIncludeProbe.cpp
)

target_link_libraries(UmpParserTests PRIVATE Qt6::Core)

if(ENABLE_WINDOWS_MIDI_SERVICES)
    target_compile_definitions(UmpParserTests PRIVATE USE_WINDOWS_MIDI_SERVICES)
endif()

if(ENABLE_WINDOWS_MIDI_SERVICES_SDK_EXPERIMENT)
    target_compile_definitions(UmpParserTests PRIVATE USE_WINDOWS_MIDI_SERVICES_SDK_EXPERIMENT)
endif()

if(NOT WINDOWS_MIDI_SERVICES_SDK_ROOT STREQUAL "")
    if(EXISTS "${WINDOWS_MIDI_SERVICES_SDK_ROOT}")
        target_compile_definitions(UmpParserTests PRIVATE WINDOWS_MIDI_SERVICES_SDK_ROOT_PROVIDED)
        
        target_compile_definitions(UmpParserTests PRIVATE WINDOWS_MIDI_SERVICES_SDK_CANDIDATE_DISCOVERY_ATTEMPTED)
        target_compile_definitions(UmpParserTests PRIVATE WINDOWS_MIDI_SERVICES_OPTIONAL_HEADER_DETECTION_ATTEMPTED)
        
        if(WINDOWS_MIDI_SERVICES_HEADER_DIR)
            target_compile_definitions(UmpParserTests PRIVATE 
                WINDOWS_MIDI_SERVICES_HEADER_CANDIDATE_DETECTED
                WINDOWS_MIDI_SERVICES_OPTIONAL_HEADERS_DETECTED
            )
            if(ENABLE_WINDOWS_MIDI_SERVICES_HEADER_INCLUDE_EXPERIMENT)
                target_include_directories(UmpParserTests PRIVATE "${WINDOWS_MIDI_SERVICES_HEADER_DIR}")
            endif()
            
            if(ENABLE_WINDOWS_MIDI_SERVICES_TYPE_REFERENCE_EXPERIMENT)
                target_compile_definitions(UmpParserTests PRIVATE 
                    WINDOWS_MIDI_SERVICES_TYPE_REFERENCE_EXPERIMENT_REQUESTED
                    WINDOWS_MIDI_SERVICES_TYPE_REFERENCE_EXPERIMENT_COMPILED
                )
                target_include_directories(UmpParserTests PRIVATE "${WINDOWS_MIDI_SERVICES_HEADER_DIR}")
                target_sources(UmpParserTests PRIVATE src/midi/WindowsMidiServicesTypeReferenceProbe.cpp)
            endif()
            
            if(ENABLE_WINDOWS_MIDI_SERVICES_CPPWINRT_ALIGNMENT_RESEARCH)
                target_compile_definitions(UmpParserTests PRIVATE WINDOWS_MIDI_SERVICES_CPPWINRT_ALIGNMENT_RESEARCH_ENABLED)
                
                if(ENABLE_WINDOWS_MIDI_SERVICES_GENERATED_PROJECTION_STRATEGY_RESEARCH)
                    target_compile_definitions(UmpParserTests PRIVATE WINDOWS_MIDI_SERVICES_GENERATED_PROJECTION_STRATEGY_RESEARCH_ENABLED)
                endif()
                
                if(ENABLE_WINDOWS_MIDI_SERVICES_WINRT_ACTIVATION_EXPERIMENT)
                    target_compile_definitions(UmpParserTests PRIVATE WINDOWS_MIDI_SERVICES_WINRT_ACTIVATION_EXPERIMENT_COMPILED)
                    target_sources(UmpParserTests PRIVATE src/midi/WindowsMidiServicesWinRtActivationProbe.cpp)
                endif()
                
                if(ENABLE_WINDOWS_MIDI_SERVICES_REAL_CPPWINRT_INCLUDE_ATTEMPT)
                    target_compile_definitions(UmpParserTests PRIVATE WINDOWS_MIDI_SERVICES_ALLOW_REAL_CPPWINRT_INCLUDE_ATTEMPT)
                endif()
                
                if(ENABLE_WINDOWS_MIDI_SERVICES_REAL_WINRT_ACTIVATION_ATTEMPT)
                    target_compile_definitions(UmpParserTests PRIVATE WINDOWS_MIDI_SERVICES_ALLOW_REAL_WINRT_ACTIVATION_ATTEMPT)
                endif()
            endif()
        else()
            if(ENABLE_WINDOWS_MIDI_SERVICES_TYPE_REFERENCE_EXPERIMENT)
                target_compile_definitions(UmpParserTests PRIVATE WINDOWS_MIDI_SERVICES_TYPE_REFERENCE_EXPERIMENT_REQUESTED)
            endif()
        endif()
        
        if(WINDOWS_MIDI_SERVICES_WINMD_FILE)
            target_compile_definitions(UmpParserTests PRIVATE WINDOWS_MIDI_SERVICES_WINMD_CANDIDATE_DETECTED)
        endif()
    else()
        target_compile_definitions(UmpParserTests PRIVATE WINDOWS_MIDI_SERVICES_SDK_ROOT_PROVIDED WINDOWS_MIDI_SERVICES_SDK_ROOT_MISSING)
    endif()
endif()

if(ENABLE_WINDOWS_MIDI_SERVICES_HEADER_INCLUDE_EXPERIMENT)
    target_compile_definitions(UmpParserTests PRIVATE USE_WINDOWS_MIDI_SERVICES_HEADER_INCLUDE_EXPERIMENT)
endif()

if(WIN32)
    get_target_property(QT_QMAKE_EXECUTABLE Qt6::qmake IMPORTED_LOCATION)
    get_filename_component(QT_BIN_DIR "${QT_QMAKE_EXECUTABLE}" DIRECTORY)

    add_custom_command(TARGET MidiUmpAnalyzer POST_BUILD
        COMMAND "${QT_BIN_DIR}/windeployqt.exe"
                "$<TARGET_FILE:MidiUmpAnalyzer>"
        COMMENT "Copiando DLLs necessárias do Qt com windeployqt..."
    )
endif()