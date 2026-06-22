#pragma once
#include "../kern/qt_alias.h"
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QStringList>

namespace AdlerMail { namespace Protokoll {

class SmtpVerbindung : public QObject {
    Q_OBJECT
public:
    explicit SmtpVerbindung(QObject *eltern = nullptr);
    ~SmtpVerbindung() override;

    void setzeServer(const QString &server);
    void setzePort(quint16 port);

oeffentlicheSlots:
    void sende(const QString &absender, const QStringList &empfaenger,
               const QString &betreff, const QString &inhalt);

signale:
    void gesendet();
    void fehlerAufgetreten(const QString &meldung);

private:
    QString m_server;
    quint16 m_port = 587;
};

}} // namespace
