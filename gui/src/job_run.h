#pragma once

#include <QObject>
#include <QQuick3DGeometry>
#include <QtQuick3D/QQuick3DInstancing>

#include "qt_helpers.h"
#include "simulation_data_api.hpp"
#include "simulation_result.hpp"

namespace SD = SolTrace::Data;
namespace RD = SolTrace::Result;


using SimDataPtr = std::shared_ptr<SD::SimulationData>;

enum class RunType {
    Thread,
    Process,
};


struct ResultDB {
    using Result = RD::SimulationResult;

    Result result;

    // look up by element id. Not clean right now
    std::unordered_map<SD::element_id, std::vector<RD::ray_id>>
        element_ids_to_ray_ids;
};

/// Models a running simulation.
///
/// Provides pause and resume (if the simulation supports it)
/// Supports progress percent and text
///
/// When finished, users can collect results using the `take()` function.
/// When done (either finished or errored out), this object will destroy itself.
class RunningJob : public QObject {
    Q_OBJECT

    using Result = RD::SimulationResult;

    void* m_watcher;

    std::shared_ptr<ResultDB> m_result;

    void setup_thread(SimDataPtr data);
    void setup_process(SimDataPtr data);

public:
    explicit RunningJob(SimDataPtr data,
                        RunType    type   = RunType::Process,
                        QObject*   parent = nullptr);
    virtual ~RunningJob();

    std::shared_ptr<ResultDB> take();

public slots:
    void pause();
    void resume();

signals:
    void progress_update(int);
    void progress_text_update(QString);
    void finished();
    void error(QString);
};

// =============================================================================


/*!
 * \brief A 3D grid, with integral indicies.
 */
template <typename T>
class Grid3D {
    std::vector<T> m_data;

    using DimType = std::array<size_t, 3>;
    DimType m_dimensions;

    inline void init_size() {
        m_data.resize(m_dimensions[0] * m_dimensions[1] * m_dimensions[2]);
    }

public:
    /*!
     * \brief Construct an empty 2D grid (x and y dimensions are 0).
     */
    explicit Grid3D() : Grid3D(0, 0, 0) { }

    /*!
     * \brief Construct a 2d grid with the specified dimensions.
     */
    explicit Grid3D(DimType const& dims) : m_dimensions(dims) { init_size(); }

    /*!
     * \brief Construct a 2d grid with the specified dimensions.
     */
    explicit Grid3D(size_t xd, size_t yd, size_t zd)
        : m_dimensions({ { xd, yd, zd } }) {
        init_size();
    }

    ~Grid3D() = default;

    // disable copy
    Grid3D(Grid3D const&)            = delete;
    Grid3D& operator=(Grid3D const&) = delete;

    // enable move
    Grid3D(Grid3D&&)            = default;
    Grid3D& operator=(Grid3D&&) = default;

    /*!
     * \brief Set all grid points to the given value.
     */
    inline void fill(T const& t) {
        for (T& lt : m_data)
            lt = t;
    }

    /*!
     * \brief The number of grid points.
     */
    inline unsigned size() const { return m_data.size(); }

    /*!
     * \brief Compute the linear offset of a given point.
     *
     * This can be used to index into an array
     */
    inline unsigned index(size_t x, size_t y, size_t z) const {
        return x + m_dimensions[0] * (y + m_dimensions[1] * z);
    }

    /*!
     * \brief The x dimension.
     */
    inline size_t size_x() const { return m_dimensions[0]; }

    /*!
     * \brief The y dimension.
     */
    inline size_t size_y() const { return m_dimensions[1]; }

    /*!
     * \brief The z dimension.
     */
    inline size_t size_z() const { return m_dimensions[2]; }


    /*!
     * \brief Access a grid point.
     */
    T& operator()(size_t x, size_t y, size_t z) {
        return m_data[index(x, y, z)];
    }

    /*!
     * \brief Access a grid point.
     */
    T const& operator()(size_t x, size_t y, size_t z) const {
        return m_data[index(x, y, z)];
    }

    /*!
     * \brief Access a grid point by its linear index.
     *
     * Useful for iterating through the grid.
     */
    T& operator[](size_t x) { return m_data[x]; }

    /*!
     * \brief Access a grid point by its linear index.
     *
     * Useful for iterating through the grid.
     */
    T const& operator[](size_t x) const { return m_data[x]; }

    /*!
     * \brief Access a grid point.
     *
     * This will throw if out of bounds.
     */
    T& at(size_t x, size_t y, size_t z) {
        auto i = index(x, y, z);
        if (i >= size()) throw std::out_of_range("Grid index out of range");
        return m_data[i];
    }

    /*!
     * \brief Access a grid point.
     *
     * This will throw if out of bounds.
     */
    T const& at(size_t x, size_t y, size_t z) const {
        auto i = index(x, y, z);
        if (i >= size()) throw std::out_of_range("Grid index out of range");
        return m_data[i];
    }

    /*!
     * \brief Ensure that two given integers are within the dimensions of the
     * grid.
     *
     * Useful to make sure accesses are not out of bound.
     */
    void clamp_bounds(size_t& x, size_t& y, size_t& z) const {
        x = clamp(x, 0u, size_x() - 1);
        y = clamp(y, 0u, size_y() - 1);
        z = clamp(z, 0u, size_z() - 1);
    }

public:
    /*!
     * \brief Linear iterator support.
     */
    auto begin() { return m_data.begin(); }

    /*!
     * \brief Linear iterator support.
     */
    auto end() { return m_data.end(); }

    /*!
     * \brief Access the underlying storage.
     */
    std::span<T>& as_vector() { return m_data; }

    /*!
     * \brief Access the underlying storage.
     */
    std::span<T> const& as_vector() const { return m_data; }

    /*!
     * \brief Get a pointer to the first element in the grid.
     */
    T*       data() { return m_data.data(); }
    T const* data() const { return m_data.data(); }
};

// =============================================================================

class RayVolume : public QQuick3DInstancing {
    Q_OBJECT

    Grid3D<float> m_grid;
    QVector3D     m_origin;
    QVector3D     m_extents;
    bool          m_dirty = true;
    QByteArray    m_data;

    void clean();

public:
    explicit RayVolume();

    void set_grid(Grid3D<float>&&, QVector3D origin, QVector3D extents);

    QByteArray getInstanceBuffer(int* instance_count) override;
};

// =============================================================================

// TODO make all deltas queued up for Concurrent off thread rebuilding of geom

class RayGeometry : public QQuick3DGeometry {
    Q_OBJECT
    QML_ELEMENT

    std::shared_ptr<ResultDB> m_database;

    std::unordered_set<SolTrace::Result::RayEvent> m_exclude_events = {
        SolTrace::Result::RayEvent::CREATE,
        SolTrace::Result::RayEvent::VIRTUAL,
        SolTrace::Result::RayEvent::UNKNOWN
    };

    /*
    std::unordered_set<SD::element_id> m_selected_elements;
    std::unordered_set<RD::ray_id>     m_selected_rays;
    */

    Q_WRITABLE_PROPERTY(float, show_percent, 50);

    QOBJECT_WRITABLE_PROPERTY(RayVolume, ray_volume);


public:
    explicit RayGeometry(QQuick3DObject* parent = nullptr);

    void set_database(std::shared_ptr<ResultDB>&&);

public slots:
    void rebuild_geometry();
};
