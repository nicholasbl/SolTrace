#include "materials_module.h"


namespace SolTrace::GUI::App {

MaterialsModule::MaterialsModule(QObject* parent)
    : QObject(parent),
      m_status(new StatusComponent(this)),
      m_materials_list(new MaterialsBackend(this)) { }

} // namespace SolTrace::GUI::App

