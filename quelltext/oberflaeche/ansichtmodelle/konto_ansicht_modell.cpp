#include "konto_ansicht_modell.h"

namespace AdlerMail
{

KontoAnsichtModell::KontoAnsichtModell(QObject* eltern) : QObject(eltern)
{
}

QString KontoAnsichtModell::email() const
{
    return m_email;
}

void KontoAnsichtModell::setzeEmail(const QString& e)
{
    if (m_email != e)
    {
        m_email = e;
        emit datenGeaendert();
        aktualisiereKannSpeichern();
    }
}

QString KontoAnsichtModell::name() const
{
    return m_name;
}

void KontoAnsichtModell::setzeName(const QString& n)
{
    if (m_name != n)
    {
        m_name = n;
        emit datenGeaendert();
    }
}

QString KontoAnsichtModell::imapServer() const
{
    return m_imapServer;
}

void KontoAnsichtModell::setzeImapServer(const QString& s)
{
    if (m_imapServer != s)
    {
        m_imapServer = s;
        emit datenGeaendert();
        aktualisiereKannSpeichern();
    }
}

int KontoAnsichtModell::imapPort() const
{
    return m_imapPort;
}

void KontoAnsichtModell::setzeImapPort(int p)
{
    if (m_imapPort != p)
    {
        m_imapPort = p;
        emit datenGeaendert();
    }
}

QString KontoAnsichtModell::smtpServer() const
{
    return m_smtpServer;
}

void KontoAnsichtModell::setzeSmtpServer(const QString& s)
{
    if (m_smtpServer != s)
    {
        m_smtpServer = s;
        emit datenGeaendert();
        aktualisiereKannSpeichern();
    }
}

int KontoAnsichtModell::smtpPort() const
{
    return m_smtpPort;
}

void KontoAnsichtModell::setzeSmtpPort(int p)
{
    if (m_smtpPort != p)
    {
        m_smtpPort = p;
        emit datenGeaendert();
    }
}

QString KontoAnsichtModell::benutzer() const
{
    return m_benutzer;
}

void KontoAnsichtModell::setzeBenutzer(const QString& b)
{
    if (m_benutzer != b)
    {
        m_benutzer = b;
        emit datenGeaendert();
        aktualisiereKannSpeichern();
    }
}

QString KontoAnsichtModell::passwort() const
{
    return m_passwort;
}

void KontoAnsichtModell::setzePasswort(const QString& p)
{
    if (m_passwort != p)
    {
        m_passwort = p;
        emit datenGeaendert();
    }
}

QString KontoAnsichtModell::signatur() const
{
    return m_signatur;
}

void KontoAnsichtModell::setzeSignatur(const QString& s)
{
    if (m_signatur != s)
    {
        m_signatur = s;
        emit datenGeaendert();
    }
}

bool KontoAnsichtModell::kannSpeichern() const
{
    return m_kannSpeichern;
}

void KontoAnsichtModell::speichern()
{
    if (m_kannSpeichern)
        emit speichernAngefordert(konto());
}

void KontoAnsichtModell::loeschenAnfordern()
{
    if (m_kontoId > 0)
        emit loeschenAngefordert(m_kontoId);
}

void KontoAnsichtModell::ladeKonto(const Kern::Konto& k)
{
    m_kontoId = k.id;
    setzeEmail(k.email);
    setzeName(k.name);
    setzeImapServer(k.imapServer);
    setzeImapPort(k.imapPort);
    setzeSmtpServer(k.smtpServer);
    setzeSmtpPort(k.smtpPort);
    setzeBenutzer(k.benutzer);
    setzePasswort(k.passwort);
    setzeSignatur(k.signatur);
}

Kern::Konto KontoAnsichtModell::konto() const
{
    Kern::Konto k;
    k.email = m_email;
    k.name = m_name;
    k.imapServer = m_imapServer;
    k.imapPort = static_cast<quint16>(m_imapPort);
    k.smtpServer = m_smtpServer;
    k.smtpPort = static_cast<quint16>(m_smtpPort);
    k.benutzer = m_benutzer;
    k.passwort = m_passwort;
    k.signatur = m_signatur;
    k.istAktiv = true;
    return k;
}

void KontoAnsichtModell::aktualisiereKannSpeichern()
{
    bool ok = !m_email.isEmpty() && !m_imapServer.isEmpty() && !m_smtpServer.isEmpty() && !m_benutzer.isEmpty();
    if (m_kannSpeichern != ok)
    {
        m_kannSpeichern = ok;
        emit datenGeaendert();
    }
}

} // namespace AdlerMail
