#pragma once
#include <QtCore/QObject>

namespace AdlerMail { namespace Dienst {

class PostfachDienst : public QObject {
    Q_OBJECT
public:
    explicit PostfachDienst(QObject *eltern = nullptr);
};

}} // namespace
