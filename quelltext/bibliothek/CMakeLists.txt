# Bibliothek — gesamte Geschäftslogik (statisch gelinkt)
add_library(mailadler_bibliothek STATIC)

# Kern-Datenmodelle
target_sources(mailadler_bibliothek PRIVATE
    kern/konto.h
    kern/konto.cpp
    kern/postfach.h
    kern/postfach.cpp
    kern/nachricht.h
    kern/nachricht.cpp
)

# Protokoll (IMAP, SMTP)
target_sources(mailadler_bibliothek PRIVATE
    protokoll/imap_verbindung.h
    protokoll/imap_verbindung.cpp
    protokoll/smtp_verbindung.h
    protokoll/smtp_verbindung.cpp
)

# Speicher (SQLite)
target_sources(mailadler_bibliothek PRIVATE
    speicher/datenbank.h
    speicher/datenbank.cpp
    speicher/zwischenspeicher.h
    speicher/zwischenspeicher.cpp
)

# Dienste (High-Level-API fürs UI)
target_sources(mailadler_bibliothek PRIVATE
    dienst/postfach_dienst.h
    dienst/postfach_dienst.cpp
    dienst/konto_dienst.h
    dienst/konto_dienst.cpp
)

target_include_directories(mailadler_bibliothek PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(mailadler_bibliothek PUBLIC
    Qt6::Core
    Qt6::Network
    Qt6::Sql
)

mailadler_setze_warnungen(mailadler_bibliothek)
