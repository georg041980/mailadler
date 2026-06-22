#include "ordner_liste_modell.h"

namespace AdlerMail {

OrdnerListeModell::OrdnerListeModell(QObject *eltern) : QStringListModel(eltern) {}

void OrdnerListeModell::setzeOrdner(const QStringList &ordner) {
    setStringList(ordner);
}

} // namespace
