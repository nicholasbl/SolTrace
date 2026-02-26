#include "documentation_module.h"

namespace SolTrace::GUI::App {

DocumentationContent::DocumentationContent(QObject *parent) :
    QObject(parent)
{}

DocumentationContent::DocumentationContent(QObject *parent, QString section_number, QString header, QString body) :
    QObject(parent),
    m_section_number(section_number),
    m_header(header),
    m_body(body)
{}

DocumentationModule::DocumentationModule(QObject *parent, QString directory) :
    QObject(parent),
    m_directory(directory),
    m_status(new StatusComponent())
{
    connect(this, &DocumentationModule::locale_changed, this, &DocumentationModule::load);
}

void DocumentationModule::load()
{
    if (m_locale == Locale::EN && !m_docs_en.empty()) return;
    if (m_locale == Locale::ES && !m_docs_es.empty()) return;

    // stub
}

void DocumentationModule::documentation_walker(const QString dir, const QString &key_prefix, const QString &section_prefix)
{
  // stub
}

QPointer<DocumentationContent> DocumentationModule::get(QString key)
{
    switch (m_locale) {
        case Locale::EN: return m_docs_en.value(key, nullptr);
        case Locale::ES: return m_docs_es.value(key, nullptr);
    }
}

QString DocumentationModule::locale_directory()
{
    switch (m_locale) {
        case Locale::EN: return m_directory + "/en";
        case Locale::ES: return m_directory + "/es";
    }
}

}

