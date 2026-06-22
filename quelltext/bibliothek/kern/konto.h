#pragma once
#include <QtCore/QObject>
#include <QtCore/QString>

namespace AdlerMail { namespace Kern {

/**
 * Konfiguration eines E-Mail-Kontos.
 */
struct Konto {
    qint64  id     = 0;
    QString email;
    QString name;
    QString imapServer;
    quint16 imapPort    = 993;
    QString smtpServer;
    quint16 smtpPort    = 587;
    QString benutzer;
    QString passwort;
    bool    istAktiv = true;
};

}} // namespace
