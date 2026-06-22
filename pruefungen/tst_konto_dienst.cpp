#include <QtCore>
#include <QtTest>

#include "dienst/konto_dienst.h"
#include "kern/konto.h"
#include "speicher/datenbank.h"

using AdlerMail::Dienst::KontoDienst;
using AdlerMail::Speicher::Datenbank;

class TestKontoDienst : public QObject
{
    Q_OBJECT

private slots:

    void initTestCase()
    {
        m_db = new Datenbank(this);
        QVERIFY(m_db->oeffne(":memory:"));
        m_dienst = new KontoDienst(m_db, this);
    }

    void sollteKontoAnlegen()
    {
        auto id =
            m_dienst->kontoAnlegen("max@test.de", "Max", "imap.test.de", 993, "smtp.test.de", 587, "max", "geheim", "");
        QVERIFY(id > 0);
    }

    void sollteAlleKontenLiefern()
    {
        m_dienst->kontoAnlegen("zwei@test.de", "", "imap2.de", 993, "smtp2.de", 587, "zwei", "pw", "");
        auto konten = m_dienst->alleKonten();
        QVERIFY(konten.size() >= 2);
    }

    void sollteKontoLoeschen()
    {
        auto konten = m_dienst->alleKonten();
        QVERIFY(!konten.isEmpty());
        QVERIFY(m_dienst->kontoLoeschen(konten.first().id));
    }

    void sollteDoppelteEmailAblehnen()
    {
        m_dienst->kontoAnlegen("unique@test.de", "", "imap", 993, "smtp", 587, "u", "p", "");
        auto id = m_dienst->kontoAnlegen("unique@test.de", "", "imap2", 993, "smtp2", 587, "u2", "p2", "");
        QCOMPARE(id, -1);
    }

    void sollteSignaleSenden()
    {
        QSignalSpy spyHinzugefuegt(m_dienst, &KontoDienst::kontoHinzugefuegt);
        m_dienst->kontoAnlegen("sig@test.de", "", "imap", 993, "smtp", 587, "sig", "pw", "");
        QCOMPARE(spyHinzugefuegt.count(), 1);
    }

private:
    Datenbank* m_db = nullptr;
    KontoDienst* m_dienst = nullptr;
};

QTEST_MAIN(TestKontoDienst)
#include "tst_konto_dienst.moc"
