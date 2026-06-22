#pragma once
#include "../kern/qt_alias.h"
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtNetwork/QTcpSocket>

namespace AdlerMail { namespace Protokoll {

class ImapVerbindung : public QObject {
    Q_OBJECT
public:
    explicit ImapVerbindung(QObject *eltern = nullptr);
    ~ImapVerbindung() override;

    void setzeServer(const QString &server);
    void setzePort(quint16 port);
    bool istVerbunden() const;

    void verbinden();
    void trennen();

oeffentlicheSlots:
    void anmelden(const QString &benutzer, const QString &passwort);
    void ordnerListeAbrufen();

signale:
    void verbunden();
    void getrennt();
    void angemeldet();
    void ordnerListeEmpfangen(const QStringList &ordner);
    void fehlerAufgetreten(const QString &meldung);

privateSlots:
    void beiVerbunden();
    void beiBereitZumLesen();

private:
    QTcpSocket *m_verbindung = nullptr;
    QString m_server;
    quint16 m_port = 993;
    bool m_angemeldet = false;
};

}} // namespace
