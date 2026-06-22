#pragma once
#include <QtCore/QString>

namespace AdlerMail {
namespace Kern {

/**
 * Metadaten eines E-Mail-Anhangs.
 */
struct Anhang {
    qint64  id        = 0;
    QString dateiname;
    QString mimeTyp;
    int     groesse    = 0;  // in Bytes
    QString inhalt;          // Base64-codiert (für lokale Speicherung)
};

} // namespace Kern
} // namespace AdlerMail
