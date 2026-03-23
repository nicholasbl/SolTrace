#pragma once

#include <QObject>
#include <QPointF>

#include "aperture.hpp"

#include "utilities/structmodel.h"

namespace SD = SolTrace::Data;

namespace db {

struct ApertureParameter {
    QString name;
    bool    is_point = false;
    QPointF content;
    bool    min_bounded = false;
    bool    max_bounded = false;
    QPointF min;
    QPointF max;

    RECORD_META(ApertureParameter,
                SM_EXPOSE_RO(name),
                SM_EXPOSE_RO(is_point),
                SM_EXPOSE_RW(content),
                SM_EXPOSE_RO(min_bounded),
                SM_EXPOSE_RO(max_bounded),
                SM_EXPOSE_RO(min),
                SM_EXPOSE_RO(max));
};


class ApertureParameterModel : public StructTableModel<ApertureParameter> {
    Q_OBJECT

public:
    explicit ApertureParameterModel(QObject* parent);

    void set_for(SD::ApertureType);
    void set_from(SD::Aperture const&);
    void write_back(SD::Aperture&) const;

signals:
    void updated();
};


} // namespace db
