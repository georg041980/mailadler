#pragma once
#include <QtCore/QObject>
#include <QtCore/QString>
#include "../../bibliothek/kern/nachricht.h"

namespace AdlerMail {

/**
 * ViewModel für die Detailansicht einer einzelnen Nachricht.
 *
 * Wird per Q_PROPERTY an QML gebunden.
 * setzeNachricht() befüllt alle Properties auf einmal.
 */
class NachrichtAnsichtModell : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString absender READ absender NOTIFY nachrichtGeaendert)
    Q_PROPERTY(QString betreff  READ betreff  NOTIFY nachrichtGeaendert)
    Q_PROPERTY(QString inhalt   READ inhalt   NOTIFY nachrichtGeaendert)
    Q_PROPERTY(QString datum    READ datum    NOTIFY nachrichtGeaendert)
    Q_PROPERTY(bool hatNachricht READ hatNachricht NOTIFY nachrichtGeaendert)
    Q_PROPERTY(bool istHtml READ istHtml NOTIFY nachrichtGeaendert)
    Q_PROPERTY(int anzahlAnhaenge READ anzahlAnhaenge NOTIFY nachrichtGeaendert)
    Q_PROPERTY(QStringList anhaengeNamen READ anhaengeNamen NOTIFY nachrichtGeaendert)

public:
    explicit NachrichtAnsichtModell(QObject *eltern = nullptr);

    QString absender() const;
    QString betreff() const;
    QString inhalt() const;
    QString datum() const;
    bool hatNachricht() const;
    bool istHtml() const;
    int anzahlAnhaenge() const;
    QStringList anhaengeNamen() const;

    void setzeNachricht(const Kern::Nachricht &nachricht);
    void leeren();

signals:
    void nachrichtGeaendert();

private:
    Kern::Nachricht m_nachricht;
    bool m_hatNachricht = false;
    QStringList m_anhaengeNamen;
};

} // namespace
