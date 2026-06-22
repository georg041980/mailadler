# Bibliothek — gesamte Geschäftslogik (statisch gelinkt)
add_library(adlermail_bibliothek STATIC)

# Kern-Datenmodelle
target_sources(adlermail_bibliothek PRIVATE
    kern/konto.h
    kern/konto.cpp
    kern/postfach.h
    kern/postfach.cpp
    kern/nachricht.h
    kern/nachricht.cpp
)

# Protokoll (IMAP, SMTP)
target_sources(adlermail_bibliothek PRIVATE
    protokoll/imap_verbindung.h
    protokoll/imap_verbindung.cpp
    protokoll/smtp_verbindung.h
    protokoll/smtp_verbindung.cpp
)

# Speicher (SQLite)
target_sources(adlermail_bibliothek PRIVATE
    speicher/datenbank.h
    speicher/datenbank.cpp
    speicher/zwischenspeicher.h
    speicher/zwischenspeicher.cpp
)

# Dienste (High-Level-API fürs UI)
target_sources(adlermail_bibliothek PRIVATE
    dienst/postfach_dienst.h
    dienst/postfach_dienst.cpp
    dienst/konto_dienst.h
    dienst/konto_dienst.cpp
)

target_include_directories(adlermail_bibliothek PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(adlermail_bibliothek PUBLIC
    Qt6::Core
    Qt6::Network
    Qt6::Sql
)

adlermail_setze_warnungen(adlermail_bibliothek)
