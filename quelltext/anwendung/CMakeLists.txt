# Anwendung — ausführbare Datei
add_executable(adlermail
    main.cpp
)

target_link_libraries(adlermail PRIVATE
    adlermail_bibliothek
    adlermail_oberflaeche
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
)

adlermail_setze_warnungen(adlermail)
