#pragma once

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QDateTime>
#include <QtCore/QVector>
#include "anhang.h"

namespace AdlerMail {
namespace Kern {

/**
 * Datenmodell für eine einzelne E-Mail-Nachricht.
 */
struct Nachricht {
    qint64    id         = 0;
    QString   absender;
    QString   betreff;
    QString   inhalt;        // plain text
    QString   inhaltHtml;    // HTML-Version (leer = nur plain)
    QDateTime datum;
    bool      gelesen       = false;
    bool      hatAnhaenge   = false;
    bool      istEntwurf    = false;
};

} // namespace Kern
} // namespace AdlerMail
