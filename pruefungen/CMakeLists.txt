# Prüfungen — nur aktiv, wenn BUILD_TESTING=ON

find_package(Qt6 REQUIRED COMPONENTS Test)

# Helfer-Makro
macro(mailadler_pruefung_hinzufuegen name)
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} PRIVATE
        Qt6::Test
        mailadler_bibliothek
    )
    mailadler_setze_warnungen(${name})
    add_test(NAME ${name} COMMAND ${name})
endmacro()

# Tests (auskommentiert, bis erste Testdatei existiert)
mailadler_pruefung_hinzufuegen(tst_datenbank)
mailadler_pruefung_hinzufuegen(tst_zwischenspeicher)
mailadler_pruefung_hinzufuegen(tst_imap_verbindung)
mailadler_pruefung_hinzufuegen(tst_smtp_verbindung)
mailadler_pruefung_hinzufuegen(tst_konto_dienst)
mailadler_pruefung_hinzufuegen(tst_postfach_dienst)
