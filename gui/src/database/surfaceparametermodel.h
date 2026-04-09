#pragma once

#include <QObject>

#include "surface.hpp"

#include "utilities/structmodel.h"

namespace SD = SolTrace::Data;

namespace db {

struct SurfaceParameter {
    QString name;
    double  content     = 0.0;
    bool    min_bounded = false;
    bool    max_bounded = false;
    double  min         = 0.0;
    double  max         = 0.0;


    RECORD_META(SurfaceParameter,
                SM_EXPOSE_RO(name),
                SM_EXPOSE_RW(content),
                SM_EXPOSE_RO(min_bounded),
                SM_EXPOSE_RO(max_bounded),
                SM_EXPOSE_RO(min),
                SM_EXPOSE_RO(max));
};


class SurfaceParameterModel : public StructTableModel<SurfaceParameter> {
    Q_OBJECT

public:
    explicit SurfaceParameterModel(QObject* parent = nullptr);

    void            set_for(SD::SurfaceType);
    void            set_from(SD::Surface const&);
    void            write_back(SD::Surface&) const;
    QVector<double> arguments() const;

signals:
    void updated();
};

} // namespace db
