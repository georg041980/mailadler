#include "smtp_verbindung.h"

namespace AdlerMail { namespace Protokoll {

SmtpVerbindung::SmtpVerbindung(QObject *eltern) : QObject(eltern) {}
SmtpVerbindung::~SmtpVerbindung() = default;

void SmtpVerbindung::setzeServer(const QString &s) { m_server = s; }
void SmtpVerbindung::setzePort(quint16 p) { m_port = p; }

void SmtpVerbindung::sende(const QString &, const QStringList &,
                           const QString &, const QString &) {
    emit gesendet();
}

}} // namespace
