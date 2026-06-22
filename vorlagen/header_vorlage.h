#pragma once

// ---------------------------------------------------------------------------
// Vorlage für AdlerMail-Header-Dateien
// ---------------------------------------------------------------------------
// KOPIERE diese Datei als Ausgangspunkt für neue Header.
// Passe Folgendes an:
//   1. Include-Guard: $HEADERNAME_H$  →  tatsächlicher Name
//   2. Namespace:      $NAMESPACE$    →  AdlerMail::Kern / Protokoll / ...
//   3. Klassenname:    $KLASSENNAME$  →  PascalCase, deutsch
//   4. Basisklasse:    $BASISKLASSE$  →  QObject oder andere
//   5. Vorlagenname:   $VORLAGE$      →  Name dieser Vorlage, unten entfernen
// ---------------------------------------------------------------------------

// $VORLAGE$: Vorlagen-Header — ersetzt diese Zeile durch echte Includes.
// Pfad zu qt_alias.h anpassen (z.B. "../kern/qt_alias.h" für protokoll/)
#include "../kern/qt_alias.h"

#include <QtCore/QObject>

namespace AdlerMail {
namespace $NAMESPACE$ {

/**
 * $KLASSENNAME$ — kurze Beschreibung, WAS die Klasse tut.
 *
 * Längere Beschreibung hier, wenn nötig.
 *
 * @thread Nicht threadsicher — nur im GUI-Strang verwenden.
 */
class $KLASSENNAME$ : public $BASISKLASSE$ {
    Q_OBJECT

    // --- Öffentliche Schnittstelle ------------------------------------------
public:
    /**
     * Erzeugt ein neues $KLASSENNAME$-Objekt.
     *
     * @param eltern  Besitzer-Objekt (Qt-Speicherverwaltung).
     */
    explicit $KLASSENNAME$(QObject *eltern = nullptr);

    /// Räumt auf.
    ~$KLASSENNAME$() override;

    // --- Signale ------------------------------------------------------------
signale:
    // Beispiel:
    // void fertig();
    // void fehlerAufgetreten(const QString &meldung);

    // --- Öffentliche Slots --------------------------------------------------
oeffentlicheSlots:
    // Beispiel:
    // void starten();

    // --- Private Slots ------------------------------------------------------
privateSlots:
    // Beispiel:
    // void beiFertig();

    // --- Privat -------------------------------------------------------------
private:
    // Member-Variablen mit m_-Präfix und Vorgabewerten.
};

} // namespace $NAMESPACE$
} // namespace AdlerMail

// $VORLAGE$: Ende der Vorlage — diese Zeile vor dem ersten Commit entfernen.
