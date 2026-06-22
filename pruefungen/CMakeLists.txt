# Prüfungen — nur aktiv, wenn BUILD_TESTING=ON

find_package(Qt6 REQUIRED COMPONENTS Test)

# Helfer-Makro
macro(adlermail_pruefung_hinzufuegen name)
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} PRIVATE
        Qt6::Test
        adlermail_bibliothek
    )
    adlermail_setze_warnungen(${name})
    add_test(NAME ${name} COMMAND ${name})
endmacro()

# Tests (auskommentiert, bis erste Testdatei existiert)
adlermail_pruefung_hinzufuegen(tst_datenbank)
adlermail_pruefung_hinzufuegen(tst_zwischenspeicher)
# adlermail_pruefung_hinzufuegen(tst_imap_verbindung)
# adlermail_pruefung_hinzufuegen(tst_smtp_verbindung)
adlermail_pruefung_hinzufuegen(tst_konto_dienst)
adlermail_pruefung_hinzufuegen(tst_postfach_dienst)
