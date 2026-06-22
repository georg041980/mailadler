#include "erstellen_ansicht_modell.h"

namespace AdlerMail {

ErstellenAnsichtModell::ErstellenAnsichtModell(QObject *eltern) : QObject(eltern) {}

QString ErstellenAnsichtModell::an() const { return m_an; }
void ErstellenAnsichtModell::setzeAn(const QString &an) {
    if (m_an != an) { m_an = an; emit anGeaendert(); aktualisiereKannSenden(); }
}

QString ErstellenAnsichtModell::betreff() const { return m_betreff; }
void ErstellenAnsichtModell::setzeBetreff(const QString &betreff) {
    if (m_betreff != betreff) { m_betreff = betreff; emit betreffGeaendert(); aktualisiereKannSenden(); }
}

QString ErstellenAnsichtModell::inhalt() const { return m_inhalt; }
void ErstellenAnsichtModell::setzeInhalt(const QString &inhalt) {
    if (m_inhalt != inhalt) { m_inhalt = inhalt; emit inhaltGeaendert(); }
}

bool ErstellenAnsichtModell::kannSenden() const { return m_kannSenden; }

void ErstellenAnsichtModell::aktualisiereKannSenden() {
    bool ok = !m_an.isEmpty() && !m_betreff.isEmpty();
    if (m_kannSenden != ok) { m_kannSenden = ok; emit kannSendenGeaendert(); }
}

} // namespace
