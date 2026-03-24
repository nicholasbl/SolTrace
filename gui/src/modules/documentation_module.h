#pragma once
#include "module_common.h"
#include "utilities/qt_file_helpers.h"
#include "utilities/qt_helpers.h"
#include <QObject>

namespace SolTrace::GUI::App {
/**
 * @class DocumentationModule
 * @brief Inline documentation registry for the SolTrace GUI.
 *
 * Loads and provides access to documentation segments displayed
 * inline alongside controls in the application.
 *
 * ## File Structure
 * Documentation is organized in a locale-aware directory hierarchy:
 * @code
 *   docs/
 *     en/
 *       configuration/
 *         sun/
 *           sun_type.md
 *           gaussian.md
 *           ...
 *         tracing/
 *           ...
 *     es/
 *       configuration/
 *         sun/
 *           sun_type.md       <- same filenames, different locale
 *           ...
 * @endcode
 *
 * ## Ordering
 * Each directory contains a manifest file (manifest.txt) that declares
 * the ordered list of files for section number derivation:
 * @code
 *   sun_type.md
 *   gaussian.md
 *   pillbox.md
 * @endcode
 * The registry walks manifests recursively at load time and assigns
 * section numbers based on traversal order. Section numbers are a
 * rendering concern — they are not stored in the files themselves.
 *
 *
 * ## Access Pattern
 * Keys follow the path schema convention using dots as separators,
 * mirroring the directory structure with slashes replaced by dots:
 * @code
 *   docs/en/configuration/sun/sun_type.md  →  "configuration.sun.sun_type"
 * @endcode
 *
 * QML access: App.docs.get("configuration.sun.sun_type")
 *
 * ## Pandoc Pipeline
 * The same directory structure is consumed by the pandoc publishing
 * pipeline to generate academic papers and technical documentation.
 * The manifest ordering determines section structure in published output.
 * Short labels for controls use Qt's tr() / .ts localization system;
 * this class handles long-form documentation body text only.
 */

class DocumentationContent : public QObject {
  Q_OBJECT

public:
  explicit DocumentationContent(QObject *parent = nullptr,
                                QString section_number = "", QString title = "",
                                QString body = "");

  Q_WRITABLE_PROPERTY(QString, section_number, "")
  Q_WRITABLE_PROPERTY(QString, title, "")
  Q_WRITABLE_PROPERTY(QString, body, "")
};

class DocumentationModule : public QObject {
  Q_OBJECT

public:
  /**
   * @param directory Root documentation directory for the active locale.
   *  e.g. ":/docs/en" or an absolute filesystem path.
   */
  DocumentationModule(QObject *parent = nullptr);

  QOBJECT_READONLY_PROPERTY(StatusComponent, status)
  Q_WRITABLE_PROPERTY(QString, directory_path, "")

  enum class Locale { EN, ES };
  Q_ENUM(Locale)

  Q_WRITABLE_PROPERTY(Locale, locale, Locale::EN)

  /**
   * @brief Eagerly loads all documentation from the locale directory.
   *
   * Walks the directory tree using manifest.txt files in each subdirectory
   * to determine file order. Assigns section numbers based on traversal
   * order. Stores all content in m_content keyed by path-schema IDs.
   *
   * Call once at startup before QML begins binding.
   */
  Q_INVOKABLE void load();

  /**
   * @brief Returns the documentation body for the given key.
   * @param key Path-schema ID, e.g. "configuration.sun.sun_type"
   * @return Markdown content string, or empty string if key not found.
   */
  Q_INVOKABLE QString get(QString key);

private:
  void doc_walker(const QString &dir_path, const QString &key_prefix = "",
                  const QString &section_number_prefix = "", int depth = 0);

  QString locale_string();
  QString locale_name_string();
  QString locale_directory();
  QHash<Locale, QHash<QString, MarkdownDocument *>> m_docs;
};

} // namespace SolTrace::GUI::App
