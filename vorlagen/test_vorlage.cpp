// ---------------------------------------------------------------------------
// Vorlage für MailAdler-Testdateien
// ---------------------------------------------------------------------------
// KOPIERE diese Datei als Ausgangspunkt für neue Tests.
// Passe Folgendes an:
//   1. Test-Name: $TESTNAME$ → tatsächlicher Name (z.B. "tst_datenbank")
//   2. SetUp:     Testumgebung einrichten (Datenbank, Objekte)
//   3. CleanUp:   Aufräumen nach jedem Test
//   4. Vorlagenname: $VORLAGE$ → Name dieser Vorlage, unten entfernen
// ---------------------------------------------------------------------------

#include <QtCore>
#include <QtTest>

// $VORLAGE$: Vorlagen-Test — ersetzt diese Zeile durch echte Includes.

/**
 * Testet die $GETESTETE_KLASSE$-Klasse.
 *
 * Prüft:
 *   - Grundlegende Funktionalität
 *   - Randfälle
 *   - Fehlerzustände
 */
class $TESTNAME$ : public QObject {
    Q_OBJECT

privateSlots:
    // --- Initialisierung / Aufräumen ---

    /// Wird EINMAL vor allen Tests ausgeführt.
    void initTestCase() {
        // Globale Testumgebung aufbauen.
        // Z.B. In-Memory-Datenbank:
        // m_datenbank = new Datenbank(":memory:");
        // m_datenbank->initialisiere();
    }

    /// Wird NACH jedem einzelnen Test ausgeführt.
    void cleanup() {
        // Test-Objekt in definierten Zustand zurückversetzen.
    }

    /// Wird EINMAL nach allen Tests ausgeführt.
    void cleanupTestCase() {
        // Globale Testumgebung abbauen.
        // delete m_datenbank;
    }

    // --- Testfälle ---------------------------------------------------------

    /// Prüft, dass die grundlegende Funktion wie erwartet arbeitet.
    void sollteEtwasTun() {
        // Aufbau (Arrange)
        // Ausführung (Act)
        // Prüfung (Assert)
        QVERIFY(true);  // $VORLAGE$: durch echten Test ersetzen
    }

    /// Prüft, dass ein Randfall korrekt behandelt wird.
    void sollteRandfallBehandeln() {
        // Z.B.: leere Eingabe, Null-Zeiger, Maximalwerte
        QVERIFY(true);  // $VORLAGE$: durch echten Test ersetzen
    }

    /// Prüft, dass ein Fehlerzustand ein Signal auslöst.
    void sollteFehlerMelden() {
        // QSignalSpy benutzen, um Signale zu prüfen.
        // QSignalSpy spion(m_ziel, &Klasse::fehlerAufgetreten);
        // m_ziel->macheEtwasWasFehlschlaegt();
        // QCOMPARE(spion.count(), 1);
        QVERIFY(true);  // $VORLAGE$: durch echten Test ersetzen
    }

    // --- Daten -------------------------------------------------------------
private:
    // Test-Objekte
    // Datenbank *m_datenbank = nullptr;
    // GetesteteKlasse *m_getestet = nullptr;
};

// Qt-Test-Makro — muss außerhalb des Namespaces stehen.
QTEST_MAIN($TESTNAME$)
#include "$TESTNAME$.moc"

// $VORLAGE$: Ende der Vorlage — diese Zeile vor dem ersten Commit entfernen.
