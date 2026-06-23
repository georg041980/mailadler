# Anwendung — ausführbare Datei
add_executable(mailadler
    main.cpp
)

target_link_libraries(mailadler PRIVATE
    mailadler_bibliothek
    mailadler_oberflaeche
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
)

mailadler_setze_warnungen(mailadler)
