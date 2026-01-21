#pragma once

#include <QObject>

struct ANotification {
    Q_GADGET
public:
    enum Type { INFO, WARNING, ERROR };

    Q_ENUM(Type);


    QString message;
    Type    type;

    ANotification() = default;
    ANotification(QString msg, Type t = INFO)
        : message(std::move(msg)), type(t) { }

    static ANotification info(QString msg) {
        return ANotification(std::move(msg), INFO);
    }

    static ANotification warning(QString msg) {
        return ANotification(std::move(msg), WARNING);
    }

    static ANotification error(QString msg) {
        return ANotification(std::move(msg), ERROR);
    }
};
