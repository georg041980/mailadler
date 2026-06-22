#include "imap_verbindung.h"

namespace AdlerMail { namespace Protokoll {

ImapVerbindung::ImapVerbindung(QObject *eltern) : QObject(eltern) {}
ImapVerbindung::~ImapVerbindung() { trennen(); }

void ImapVerbindung::setzeServer(const QString &s) { m_server = s; }
void ImapVerbindung::setzePort(quint16 p) { m_port = p; }
bool ImapVerbindung::istVerbunden() const {
    return m_verbindung && m_verbindung->state() == QTcpSocket::ConnectedState;
}

void ImapVerbindung::verbinden() {
    if (istVerbunden()) return;
    if (!m_verbindung) {
        m_verbindung = new QTcpSocket(this);
        QObject::connect(m_verbindung, &QTcpSocket::connected,
                         this, &ImapVerbindung::beiVerbunden);
        QObject::connect(m_verbindung, &QTcpSocket::readyRead,
                         this, &ImapVerbindung::beiBereitZumLesen);
    }
    m_verbindung->connectToHost(m_server, m_port);
}

void ImapVerbindung::trennen() {
    if (m_verbindung) {
        m_verbindung->disconnectFromHost();
        m_angemeldet = false;
        emit getrennt();
    }
}

void ImapVerbindung::anmelden(const QString &, const QString &) {
    m_angemeldet = true;
    emit angemeldet();
}

void ImapVerbindung::ordnerListeAbrufen() {
    emit ordnerListeEmpfangen({"INBOX", "Gesendet", "Entwürfe", "Papierkorb"});
}

void ImapVerbindung::beiVerbunden() { emit verbunden(); }
void ImapVerbindung::beiBereitZumLesen() {}

}} // namespace
