#include <QtCore>
#include <QtTest>
#include "dienst/postfach_dienst.h"
#include "speicher/zwischenspeicher.h"
#include "kern/nachricht.h"

using AdlerMail::Dienst::PostfachDienst;
using AdlerMail::Speicher::Zwischenspeicher;
using AdlerMail::Kern::Nachricht;

class TestPostfachDienst : public QObject {
    Q_OBJECT

private slots:
    void initTestCase() {
        m_cache = new Zwischenspeicher(this);
        m_dienst = new PostfachDienst(m_cache, this);
    }

    void sollteLeerStarten() {
        QVERIFY(m_dienst->nachrichten().isEmpty());
        QCOMPARE(m_dienst->anzahl(), 0);
    }

    void sollteNachrichtenSetzen() {
        Nachricht n;
        n.id = 1; n.betreff = "Test";
        m_dienst->setzeNachrichten({n});

        QCOMPARE(m_dienst->anzahl(), 1);
        QCOMPARE(m_dienst->nachrichten()[0].betreff, "Test");
    }

    void sollteSignaleSenden() {
        QSignalSpy spion(m_dienst, &PostfachDienst::nachrichtenGeaendert);

        Nachricht n;
        m_dienst->setzeNachrichten({n});

        QCOMPARE(spion.count(), 1);
    }

private:
    Zwischenspeicher *m_cache = nullptr;
    PostfachDienst *m_dienst = nullptr;
};

QTEST_MAIN(TestPostfachDienst)
#include "tst_postfach_dienst.moc"
