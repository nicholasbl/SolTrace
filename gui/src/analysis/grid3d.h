#pragma once

#include <span>
#include <vector>

namespace analysis {

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

} // namespace analysis
