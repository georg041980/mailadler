#include <QTest>
#include <QTranslator>
#include <QApplication>

class UebersetzungTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase() {}

    void testDeutscheUebersetzungLaden()
    {
        QTranslator uebersetzer;
        bool ok = uebersetzer.load(":/translations/mailadler_de_DE.qm");
        QVERIFY(ok);
    }

    void testEnglischeUebersetzungLaden()
    {
        QTranslator uebersetzer;
        bool ok = uebersetzer.load(":/translations/mailadler_en_US.qm");
        QVERIFY(ok);
    }

    void testSpanischeUebersetzungLaden()
    {
        QTranslator uebersetzer;
        bool ok = uebersetzer.load(":/translations/mailadler_es_ES.qm");
        QVERIFY(ok);
    }

    void testTrNachSprachwechsel()
    {
        QTranslator uebersetzer;
        bool ok = uebersetzer.load(":/translations/mailadler_en_US.qm");
        QVERIFY(ok);
        qApp->installTranslator(&uebersetzer);

        // Wichtig: Kontext "MainWindow" angeben, weil dort die Übersetzung gespeichert ist
        QString uebersetzt = qApp->translate("MainWindow", "Posteingang");
        QCOMPARE(uebersetzt, QString("Inbox"));
    }

    void cleanupTestCase() {}
};

QTEST_MAIN(UebersetzungTest)
#include "test_main.moc"
