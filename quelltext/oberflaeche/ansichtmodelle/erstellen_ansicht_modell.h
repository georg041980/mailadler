#pragma once
#include <QtCore/QObject>
#include <QtCore/QString>

namespace AdlerMail {

class ErstellenAnsichtModell : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString an READ an WRITE setzeAn NOTIFY anGeaendert)
    Q_PROPERTY(QString betreff READ betreff WRITE setzeBetreff NOTIFY betreffGeaendert)
    Q_PROPERTY(QString inhalt READ inhalt WRITE setzeInhalt NOTIFY inhaltGeaendert)
    Q_PROPERTY(bool kannSenden READ kannSenden NOTIFY kannSendenGeaendert)

public:
    explicit ErstellenAnsichtModell(QObject *eltern = nullptr);

    QString an() const;
    void setzeAn(const QString &an);

    QString betreff() const;
    void setzeBetreff(const QString &betreff);

    QString inhalt() const;
    void setzeInhalt(const QString &inhalt);

    bool kannSenden() const;

signals:
    void anGeaendert();
    void betreffGeaendert();
    void inhaltGeaendert();
    void kannSendenGeaendert();

private:
    void aktualisiereKannSenden();
    QString m_an, m_betreff, m_inhalt;
    bool m_kannSenden = false;
};

} // namespace
