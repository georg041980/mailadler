# Oberfläche — QML + C++-AnsichtModelle
add_library(adlermail_oberflaeche STATIC)

target_sources(adlermail_oberflaeche PRIVATE
    ansichtmodelle/erstellen_ansicht_modell.h
    ansichtmodelle/erstellen_ansicht_modell.cpp
    ansichtmodelle/nachrichten_liste_modell.h
    ansichtmodelle/nachrichten_liste_modell.cpp
    ansichtmodelle/ordner_liste_modell.h
    ansichtmodelle/ordner_liste_modell.cpp
    ansichtmodelle/nachricht_ansicht_modell.h
    ansichtmodelle/nachricht_ansicht_modell.cpp
    ansichtmodelle/konto_ansicht_modell.h
    ansichtmodelle/konto_ansicht_modell.cpp
)

# QML-Dateien als Ressourcen einbetten
set(QML_DATEIEN
    qml/HauptFenster.qml
    qml/NachrichtenListe.qml
    qml/NachrichtAnsicht.qml
    qml/ErstellenAnsicht.qml
)

qt6_add_qml_module(adlermail_oberflaeche
    URI AdlerMail
    VERSION 1.0
    QML_FILES ${QML_DATEIEN}
)

target_include_directories(adlermail_oberflaeche PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(adlermail_oberflaeche PUBLIC
    adlermail_bibliothek
    Qt6::Core
    Qt6::Qml
    Qt6::Quick
)

adlermail_setze_warnungen(adlermail_oberflaeche)
