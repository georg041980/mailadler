# AdlerMail — Qt6 E-Mail-Client
# Build mit: cmake -B bau -DCMAKE_BUILD_TYPE=Debug && cmake --build bau
cmake_minimum_required(VERSION 3.22)

project(AdlerMail
    VERSION 0.1.0
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
    LinguistTools
)

# Übersetzungen
qt6_add_translation(uebersetzungen
    TS_FILES
        uebersetzungen/adlermail_de.ts
        uebersetzungen/adlermail_en.ts
        uebersetzungen/adlermail_es.ts
)

# Unterverzeichnisse
add_subdirectory(quelltext)
enable_testing()
add_subdirectory(pruefungen)
