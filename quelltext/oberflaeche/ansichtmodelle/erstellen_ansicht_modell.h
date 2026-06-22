#pragma once
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QStringList>

namespace AdlerMail
{

class ErstellenAnsichtModell : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString an READ an WRITE setzeAn NOTIFY anGeaendert)
    Q_PROPERTY(QString cc READ cc WRITE setzeCc NOTIFY ccGeaendert)
    Q_PROPERTY(QString betreff READ betreff WRITE setzeBetreff NOTIFY betreffGeaendert)
    Q_PROPERTY(QString inhalt READ inhalt WRITE setzeInhalt NOTIFY inhaltGeaendert)
    Q_PROPERTY(bool kannSenden READ kannSenden NOTIFY kannSendenGeaendert)
    Q_PROPERTY(QStringList anhaenge READ anhaenge NOTIFY anhaengeGeaendert)

public:
    explicit ErstellenAnsichtModell(QObject* eltern = nullptr);

    Q_INVOKABLE void sendeAnfordern();
    Q_INVOKABLE void entwurfSpeichern();
    Q_INVOKABLE void antwortVorbereiten(const QString& an, const QString& betreff, const QString& zitat);
    Q_INVOKABLE void anhangHinzufuegen(const QString& pfad);

    QString an() const;
    void setzeAn(const QString& an);
    QString cc() const;
    void setzeCc(const QString& cc);
    QString betreff() const;
    void setzeBetreff(const QString& betreff);
    QString inhalt() const;
    void setzeInhalt(const QString& inhalt);
    bool kannSenden() const;
    QStringList anhaenge() const;

signals:
    void anGeaendert();
    void ccGeaendert();
    void betreffGeaendert();
    void inhaltGeaendert();
    void kannSendenGeaendert();
    void anhaengeGeaendert();
    void sendeAngefordert();
    void entwurfSpeichernAngefordert();

private:
    void aktualisiereKannSenden();
    QString m_an, m_cc, m_betreff, m_inhalt;
    bool m_kannSenden = false;
    QStringList m_anhaenge;
};

} // namespace AdlerMail
