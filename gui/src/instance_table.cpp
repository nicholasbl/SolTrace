#include "instance_table.h"

#include "backend.h"

#include <random>

// QColor random_color() {
//     int r = std::rand() % 256;
//     int g = std::rand() % 256;
//     int b = std::rand() % 256;

//     return QColor(r, g, b);
// }

// void InstanceTable::clean() {
//     m_instance_data.resize(m_source.positions.size() *
//                            sizeof(InstanceTableEntry));

//     float radius = 10;

//     // Random number generator
//     std::random_device               rd;
//     std::mt19937                     gen(rd());
//     std::uniform_real_distribution<> dis_theta(0.0, 2 * M_PI);
//     std::uniform_real_distribution<> dis_phi(0.0, 1.0);

//     auto dest =
//     reinterpret_cast<InstanceTableEntry*>(m_instance_data.data());

//     for (int i = 0; i < m_source.positions.size(); ++i) {

//         auto const& position = m_source.positions[i];

//         auto p   = QVector3D { position.x, position.y, position.z };
//         auto dir = (-p).normalized();

//         auto entry = calculateTableEntryFromQuaternion(
//             { position.x, position.y, position.z },
//             { 1, 1, 1 },
//             QQuaternion::rotationTo({ 0, 0, 1 }, dir).normalized(),
//             random_color());

//         dest[i] = entry;
//     }
// }

// InstanceTable::InstanceTable() { }

// void InstanceTable::update(DataSet const& d) {
//     m_source = d;

//     markDirty();
// }

// QByteArray InstanceTable::getInstanceBuffer(int* instance_count) {
//     if (m_dirty) clean();

//     if (instance_count) { *instance_count = m_source.positions.size(); }

//     return m_instance_data;
// }
