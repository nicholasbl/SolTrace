#pragma once

#include "utilities/qt_helpers.h"
#include <QObject>

namespace SolTrace::GUI::App {

class ViewModule : public QObject {
  Q_OBJECT
public:
  explicit ViewModule(QObject *parent = nullptr);

  Q_WRITABLE_PROPERTY(size_t, workflow_phase, 0)
  Q_WRITABLE_PROPERTY(size_t, configure_section, 0)
  Q_WRITABLE_PROPERTY(size_t, simulate_section, 0)
  Q_WRITABLE_PROPERTY(size_t, analyze_section, 0)

  Q_WRITABLE_PROPERTY(bool, editing_material, false)
  Q_WRITABLE_PROPERTY(bool, editing_geometry, false)
  Q_WRITABLE_PROPERTY(bool, editing_layout, false)
};

} // namespace SolTrace::GUI::App
