#pragma once

#include <QMutex>
#include <QObject>
#include <QQuickImageProvider>

#include "analysis/flux_map.h"
#include "database.h"
#include "database/geometryeditor.h"
#include "database/simulationresult.h"

namespace analysis {
class FluxMapComputer;
}

namespace db {

// TODO: Move to the database observer?


class FluxMapProvider;

struct FluxMappedPendingItem {
    Entity entity;
    int    progress = 0;

    RECORD_META(FluxMappedPendingItem,
                SM_EXPOSE_RO(entity),
                SM_EXPOSE_RO(progress));
};

class PendingFluxMapModel : public StructModelAdapter<FluxMappedPendingItem> {
    Q_OBJECT
    QPointer<Database>                  m_host;
    QPointer<analysis::FluxMapComputer> m_compute;

private slots:
    void on_changed();

    void on_ready(db::Entity, analysis::BakedFluxMapPtr);
    void on_progress(db::Entity, int);

public:
    // TODO: Make sure we have bins counts and bin areas to export
    // TODO: just bin per triangle option
    // power per ray

    Q_WRITABLE_PROPERTY(int, mesh_resolution_multiply, 1);
    Q_WRITABLE_PROPERTY(QSize, image_resolution, (QSize { 1024, 1024 }));
    Q_WRITABLE_PROPERTY(QColor, mesh_line_color, "black");
    Q_WRITABLE_PROPERTY(bool, show_mesh_grid, false);
    Q_WRITABLE_PROPERTY(QString, color_map, "");

    explicit PendingFluxMapModel(QObject* parent = nullptr);
    virtual ~PendingFluxMapModel() = default;

    void reset(db::SimulationResultPtr);

    Database* database() { return m_host; }

    // Create a provider for flux map textures. This MUST be done at the start
    // of the app, and before any databases!
    FluxMapProvider* make_new_provider();

public slots:
    bool start_generate_for(db::Entity);
    void cancel_for(db::Entity);

signals:
    void ready(db::Entity, analysis::BakedFluxMapPtr, db::Database*);
    void cleared();
};

// ============================================================================

struct FluxMappedItem {
    Entity                           entity;
    QString                          texture_source;
    std::shared_ptr<SurfaceGeometry> geometry;

    RECORD_META(FluxMappedItem,
                SM_EXPOSE_RO(entity),
                SM_EXPOSE_RO(texture_source),
                SM_EXPOSE_RO(geometry));
};

class FluxMapWorldModel : public StructModelAdapter<FluxMappedItem> {
    Q_OBJECT

public:
    // TODO: Make sure we have bins counts and bin areas to export
    // power per ray

    explicit FluxMapWorldModel(QObject* parent = nullptr);
    virtual ~FluxMapWorldModel() = default;

public slots:
    void on_reset();
    void on_ready(Entity, analysis::BakedFluxMapPtr, Database*);
};

// ============================================================================

class FluxMapProvider : public QQuickImageProvider {
    Q_OBJECT

    QHash<QString, analysis::BakedFluxMapPtr> m_store;
    QMutex                                    m_lock;

public:
    FluxMapProvider();

    QImage requestImage(QString const& id,
                        QSize*         size,
                        QSize const&   requestedSize) override;

public slots:
    void on_ready(Entity, analysis::BakedFluxMapPtr, Database*);
    void clear();
};


} // namespace db
