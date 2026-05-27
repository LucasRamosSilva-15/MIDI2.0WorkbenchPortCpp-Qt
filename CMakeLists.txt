cmake_minimum_required(VERSION 3.16)
project(MidiUmpAnalyzer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_executable(MidiUmpAnalyzer
    src/main.cpp
    src/MainWindow.h
    src/MainWindow.cpp
    src/UmpParser.h
    src/UmpParser.cpp
)

target_link_libraries(MidiUmpAnalyzer PRIVATE Qt6::Widgets)

if(WIN32)
    get_target_property(QT_QMAKE_EXECUTABLE Qt6::qmake IMPORTED_LOCATION)
    get_filename_component(QT_BIN_DIR "${QT_QMAKE_EXECUTABLE}" DIRECTORY)

    add_custom_command(TARGET MidiUmpAnalyzer POST_BUILD
        COMMAND "${QT_BIN_DIR}/windeployqt.exe"
                "$<TARGET_FILE:MidiUmpAnalyzer>"
        COMMENT "Copiando DLLs necessárias do Qt com windeployqt..."
    )
endif()