#include "documentation_module.h"
#include "utilities/qt_file_helpers.h"
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>
#include <QStringList>

#define DOCS_LOAD_INFO true
#define DOCS_DEBUG_INFO true

namespace SolTrace::GUI::App {

DocumentationContent::DocumentationContent(QObject *parent,
                                           QString section_number,
                                           QString title, QString body)
    : QObject(parent), m_section_number(section_number), m_title(title),
      m_body(body) {}

DocumentationModule::DocumentationModule(QObject *parent)
    : QObject(parent), m_directory_path(":/docs"),
      m_status(new StatusComponent()) {
  connect(this, &DocumentationModule::locale_changed, this,
          &DocumentationModule::load);

  load();
}

void DocumentationModule::load() {
  if (m_locale == Locale::EN && m_docs.contains(Locale::EN) &&
      !m_docs[Locale::EN].empty())
    return;
  if (m_locale == Locale::ES && m_docs.contains(Locale::ES) &&
      !m_docs[Locale::ES].empty())
    return;

  if (DOCS_LOAD_INFO)
    qDebug() << "Loading docs...";

  doc_walker(locale_directory());

  if (DOCS_LOAD_INFO) {
    for (auto locale = m_docs.begin(); locale != m_docs.end(); locale++) {
      qDebug() << "==========[" + locale_name_string() + " DOCS]==========";
      for (auto doc = locale.value().begin(); doc != locale.value().end();
           doc++) {
        qDebug() << doc.key() << " " << doc.value()->metadata("section_number")
                 << " " << doc.value()->metadata("title") << " ";
      }
    }

    qDebug() << "Finished loading docs...";
  }
}

void DocumentationModule::doc_walker(const QString &dir_path,
                                     const QString &key_prefix,
                                     const QString &section_number_prefix,
                                     int depth) {
  QDir dir(dir_path);

  if (!dir.exists()) {
    if (DOCS_DEBUG_INFO)
      qDebug() << "Directory does not exist: " << dir_path;
    return;
  }

  QFile manifest = load_file(dir, "manifest.txt");
  QFileInfoList file_infos;

  if (manifest.exists()) {
    QString raw_manifest = read_file(manifest);

    if (raw_manifest.isEmpty() && DOCS_DEBUG_INFO) {
      qDebug() << "Empty manifest.txt: " +
                      QFileInfo(manifest).canonicalFilePath();
    }

    for (const QString &line : raw_manifest.trimmed().split("\n")) {
      QFileInfo file_info(dir, line.trimmed());

      if (!file_info.isFile()) {
        if (DOCS_DEBUG_INFO)
          qDebug() << "File does not exist: " << file_info.canonicalFilePath();
        continue;
      }

      file_infos.append(file_info);
    }

  } else {
    file_infos = dir.entryInfoList({"*.md"}, QDir::Files);
  }

  int i = 1;

  for (const QFileInfo &file_info : file_infos) {
    QFile file(file_info.canonicalFilePath());
    MarkdownDocument *doc = parse_markdown_file(file);

    QString name = file_info.fileName().replace(".md", "");
    QDir subdir = QDir(dir_path + "/" + name);

    QString new_key, new_section_number;

    if (depth == 0) {
      new_key = name;
    } else {
      new_key = key_prefix.isEmpty() ? name : key_prefix + "." + name;
      new_section_number =
          manifest.exists()
              ? section_number_prefix.isEmpty()
                    ? QString::number(i) + "."
                    : section_number_prefix + QString::number(i) + "."
              : "";
      doc->set_metadata("section_number", new_section_number);
    }

    m_docs[m_locale].insert(new_key, doc);

    i++;

    if (!subdir.exists()) {
      continue;
    }

    doc_walker(subdir.absolutePath(), new_key, new_section_number, depth + 1);
  }
}

QString DocumentationModule::get(QString key) {
  QString doc_key, metadata_key;
  MarkdownDocument *doc;

  int separator_index = key.indexOf("#");
  if (separator_index == -1) {
    doc_key = key;
  } else {
    doc_key = key.left(separator_index);
    metadata_key = key.mid(separator_index + 1);
  }

  doc = m_docs[m_locale].value(doc_key, nullptr);

  if (doc == nullptr)
    return "ERROR: Invalid key (" + key + ")";

  if (metadata_key.isEmpty()) {
    return doc->body();
  }

  return doc->metadata(metadata_key);
}

QString DocumentationModule::locale_string() {
  switch (m_locale) {
  case Locale::EN:
    return "en";
  case Locale::ES:
    return "es";
  }
}

QString DocumentationModule::locale_name_string() {
  switch (m_locale) {
  case Locale::EN:
    return "English";
  case Locale::ES:
    return "Spanish";
  }
}

QString DocumentationModule::locale_directory() {
  switch (m_locale) {
  case Locale::EN:
    return m_directory_path + "/en";
  case Locale::ES:
    return m_directory_path + "/es";
  }
}

} // namespace SolTrace::GUI::App
