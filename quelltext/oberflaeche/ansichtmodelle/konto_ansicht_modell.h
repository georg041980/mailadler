#pragma once
#include <QtCore/QObject>
#include <QtCore/QString>

#include "../../bibliothek/kern/konto.h"

namespace AdlerMail
{

class KontoAnsichtModell : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString email READ email WRITE setzeEmail NOTIFY datenGeaendert)
    Q_PROPERTY(QString name READ name WRITE setzeName NOTIFY datenGeaendert)
    Q_PROPERTY(QString imapServer READ imapServer WRITE setzeImapServer NOTIFY datenGeaendert)
    Q_PROPERTY(int imapPort READ imapPort WRITE setzeImapPort NOTIFY datenGeaendert)
    Q_PROPERTY(QString smtpServer READ smtpServer WRITE setzeSmtpServer NOTIFY datenGeaendert)
    Q_PROPERTY(int smtpPort READ smtpPort WRITE setzeSmtpPort NOTIFY datenGeaendert)
    Q_PROPERTY(QString benutzer READ benutzer WRITE setzeBenutzer NOTIFY datenGeaendert)
    Q_PROPERTY(QString passwort READ passwort WRITE setzePasswort NOTIFY datenGeaendert)
    Q_PROPERTY(bool kannSpeichern READ kannSpeichern NOTIFY datenGeaendert)

public:
    explicit KontoAnsichtModell(QObject* eltern = nullptr);

    Q_INVOKABLE void speichern();

    QString email() const;
    void setzeEmail(const QString& e);
    QString name() const;
    void setzeName(const QString& n);
    QString imapServer() const;
    void setzeImapServer(const QString& s);
    int imapPort() const;
    void setzeImapPort(int p);
    QString smtpServer() const;
    void setzeSmtpServer(const QString& s);
    int smtpPort() const;
    void setzeSmtpPort(int p);
    QString benutzer() const;
    void setzeBenutzer(const QString& b);
    QString passwort() const;
    void setzePasswort(const QString& p);
    bool kannSpeichern() const;

    Q_INVOKABLE void loeschenAnfordern();

    void ladeKonto(const Kern::Konto& k);
    Kern::Konto konto() const;

signals:
    void datenGeaendert();
    void speichernAngefordert(const Kern::Konto& konto);
    void loeschenAngefordert(qint64 id);

private:
    void aktualisiereKannSpeichern();
    QString m_email, m_name, m_imapServer, m_smtpServer, m_benutzer, m_passwort;
    int m_imapPort = 993, m_smtpPort = 587;
    bool m_kannSpeichern = false;
    qint64 m_kontoId = 0;
};

} // namespace AdlerMail
