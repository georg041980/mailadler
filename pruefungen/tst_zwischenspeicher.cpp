#include <QtCore>
#include <QtTest>
#include "speicher/zwischenspeicher.h"
#include "kern/nachricht.h"

using AdlerMail::Speicher::Zwischenspeicher;
using AdlerMail::Kern::Nachricht;

class TestZwischenspeicher : public QObject {
    Q_OBJECT

private slots:
    void sollteLeerStarten() {
        Zwischenspeicher cache;
        QVERIFY(cache.alle().isEmpty());
    }

    void sollteNachrichtenSetzenUndLesen() {
        Zwischenspeicher cache;

        Nachricht n1;
        n1.id = 1;
        n1.betreff = "Hallo";
        n1.absender = "max@test.de";

        Nachricht n2;
        n2.id = 2;
        n2.betreff = "Re: Hallo";
        n2.absender = "lisa@test.de";

        cache.setze({n1, n2});

        auto alle = cache.alle();
        QCOMPARE(alle.size(), 2);
        QCOMPARE(alle[0].betreff, "Hallo");
        QCOMPARE(alle[1].absender, "lisa@test.de");
    }

    void sollteLeerenFunktionieren() {
        Zwischenspeicher cache;
        cache.setze({Nachricht()});
        QCOMPARE(cache.alle().size(), 1);

        cache.leeren();
        QVERIFY(cache.alle().isEmpty());
    }

    void sollteUeberschreiben() {
        Zwischenspeicher cache;

        Nachricht alt; alt.betreff = "Alt";
        cache.setze({alt});
        QCOMPARE(cache.alle()[0].betreff, "Alt");

        Nachricht neu; neu.betreff = "Neu";
        cache.setze({neu});
        QCOMPARE(cache.alle()[0].betreff, "Neu");
        QCOMPARE(cache.alle().size(), 1);
    }
};

QTEST_MAIN(TestZwischenspeicher)
#include "tst_zwischenspeicher.moc"
