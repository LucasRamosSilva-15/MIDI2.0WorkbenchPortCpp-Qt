cmake_minimum_required(VERSION 3.16)
project(MidiUmpAnalyzer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets LinguistTools)

set(TS_FILES translations/MidiUmpAnalyzer_en_US.ts)
qt_add_translations(MidiUmpAnalyzer TS_FILES ${TS_FILES} RESOURCE_PREFIX "/i18n")
option(ENABLE_RTMIDI "Habilitar dependência opcional do RtMidi" OFF)

if(ENABLE_RTMIDI)
    include(FetchContent)
    FetchContent_Declare(
        rtmidi
        GIT_REPOSITORY https://github.com/thestk/rtmidi.git
        GIT_TAG 6.0.0
    )
    FetchContent_MakeAvailable(rtmidi)
endif()

# libremidi FetchContent (Cross-platform MIDI 2.0 / UMP backend)
include(FetchContent)
set(LIBREMIDI_EXAMPLES OFF CACHE BOOL "" FORCE)
set(LIBREMIDI_TESTS OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    libremidi
    GIT_REPOSITORY https://github.com/celtera/libremidi.git
    GIT_TAG v5.4.3
)
FetchContent_MakeAvailable(libremidi)

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
    src/midi/LibremidiBackend.h
    src/midi/LibremidiBackend.cpp
)

target_link_libraries(MidiUmpAnalyzer PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets libremidi::libremidi)

if(ENABLE_RTMIDI)
    target_compile_definitions(MidiUmpAnalyzer PRIVATE USE_RTMIDI)
    target_link_libraries(MidiUmpAnalyzer PRIVATE rtmidi)
endif()

# Criação do Executável de Testes
add_executable(UmpParserTests
    tests/UmpParserTests.cpp
    tests/TestUtils.h
    src/UmpParser.cpp
    src/UmpParser.h
    src/midi/Midi1LiveDecoder.cpp
    src/midi/Midi1LiveDecoder.h
    src/midi/Midi1ToUmpPreviewConverter.cpp
    src/midi/Midi1ToUmpPreviewConverter.h
    src/core/UmpRawEvent.h
    src/midi/IUmpInputBackend.h
    src/midi/MidiInputController.h
    src/midi/MidiInputController.cpp
    src/midi/FakeUmpInputBackend.h
    src/midi/FakeUmpInputBackend.cpp
    src/midi/LibremidiBackend.h
    src/midi/LibremidiBackend.cpp
)

target_link_libraries(UmpParserTests PRIVATE Qt6::Core libremidi::libremidi)

if(WIN32)
    get_target_property(QT_QMAKE_EXECUTABLE Qt6::qmake IMPORTED_LOCATION)
    get_filename_component(QT_BIN_DIR "${QT_QMAKE_EXECUTABLE}" DIRECTORY)

    add_custom_command(TARGET MidiUmpAnalyzer POST_BUILD
        COMMAND "${QT_BIN_DIR}/windeployqt.exe"
                "$<TARGET_FILE:MidiUmpAnalyzer>"
        COMMENT "Copiando DLLs necessárias do Qt com windeployqt..."
    )
endif()