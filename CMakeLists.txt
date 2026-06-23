# MailAdler — Qt6 E-Mail-Client
# Build mit: cmake -B bau -DCMAKE_BUILD_TYPE=Debug && cmake --build bau
cmake_minimum_required(VERSION 3.22)

project(MailAdler
    VERSION 1.0.0
    DESCRIPTION "Plattformübergreifender E-Mail-Client"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Kompilierwarnungen
include(cmake/KompilierWarnungen.cmake)

# Sicherer-Bau-Modus (Sanitizer)
include(cmake/SichererBau.cmake)

# Qt6
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Qml
    Quick
    Network
    Sql
)

# Unterverzeichnisse
add_subdirectory(quelltext)
enable_testing()
add_subdirectory(pruefungen)
