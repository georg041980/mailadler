#pragma once
#include <QtCore/QObject>

namespace AdlerMail { namespace Dienst {

class KontoDienst : public QObject {
    Q_OBJECT
public:
    explicit KontoDienst(QObject *eltern = nullptr);
};

}} // namespace
