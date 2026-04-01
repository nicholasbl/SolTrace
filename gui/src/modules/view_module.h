#pragma once

#include "utilities/qt_helpers.h"
#include <QObject>
#include <QQmlEngine>

namespace SolTrace::GUI::App {

class ViewModule : public QObject {
    Q_OBJECT
    QML_ELEMENT
public:
    explicit ViewModule(QObject* parent = nullptr);

    enum PanelSize { Small = 0, Normal = 1, Wide = 2 };

    Q_ENUM(PanelSize)

    Q_WRITABLE_PROPERTY(int, workflow_phase, 0)
    Q_WRITABLE_PROPERTY(int, configure_section, 0)
    Q_WRITABLE_PROPERTY(int, simulate_section, 0)
    Q_WRITABLE_PROPERTY(int, analyze_section, 0)

    Q_WRITABLE_PROPERTY(int, sun_section, 0)

    Q_WRITABLE_PROPERTY(bool, editing_material, false)
    Q_WRITABLE_PROPERTY(bool, editing_geometry, false)
    Q_WRITABLE_PROPERTY(bool, editing_layout, false)
    Q_WRITABLE_PROPERTY(bool, editing_appearance, false)

    Q_WRITABLE_PROPERTY(bool, show_left_panel, true)
    Q_WRITABLE_PROPERTY(bool, show_right_panel, false)
    Q_WRITABLE_PROPERTY(PanelSize, left_panel_size, PanelSize::Wide)
    // Unused (potential feature)
    Q_WRITABLE_PROPERTY(PanelSize, right_panel_size, PanelSize::Normal)

    Q_WRITABLE_PROPERTY(int, right_panel_section, 0)
    Q_WRITABLE_PROPERTY(bool, show_settings_panel, false)
};

} // namespace SolTrace::GUI::App
