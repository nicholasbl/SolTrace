#include "logging.h"

#include <QDir>
#include <QFile>
#include <QMutex>
#include <QMutexLocker>
#include <QStandardPaths>
#include <QTextStream>

#include <memory>

namespace SolTrace::GUI::App {

static std::unique_ptr<LogList>     CURRENT_LOG_LIST;
static std::unique_ptr<QFile>       CURRENT_LOG_FILE;
static std::unique_ptr<QTextStream> CURRENT_LOG_STDOUT_STREAM;
static std::unique_ptr<QTextStream> CURRENT_LOG_FILE_STREAM;
static QMutex                       LOG_MUTEX;

void LogList::spin_off() {
    auto const limit = static_cast<int>(m_max_line_count);
    auto const count = rowCount();

    if (count <= limit) { return; }

    removeRows(0, count - limit);
}

LogList::LogList() {
    connect(this, &LogList::max_line_count_changed, this, &LogList::spin_off);
}

LogList::~LogList() { }

void LogList::append_line(QString string) {
    this->append(LogRecord { .content = string });
    spin_off();
}

// =============================================================================

static void fileMessageHandler(QtMsgType                 type,
                               QMessageLogContext const& context,
                               QString const&            msg) {
    auto const line = qFormatLogMessage(type, context, msg);

    QMutexLocker locker(&LOG_MUTEX);

    if (CURRENT_LOG_FILE_STREAM) {
        (*CURRENT_LOG_FILE_STREAM) << line << '\n';
        CURRENT_LOG_FILE_STREAM->flush();
    }

    if (CURRENT_LOG_STDOUT_STREAM) {
        (*CURRENT_LOG_STDOUT_STREAM) << line << '\n';
        CURRENT_LOG_STDOUT_STREAM->flush();
    }

    if (CURRENT_LOG_LIST) {
        QMetaObject::invokeMethod(CURRENT_LOG_LIST.get(),
                                  "append_line",
                                  Qt::QueuedConnection,
                                  Q_ARG(QString, line));
    }


    if (type == QtFatalMsg) abort();
}

void rotate_log_file(const QString& path, qint64 maxBytes = 1024 * 1024) {
    QFile file(path);

    if (!file.exists()) return;

    if (file.size() < maxBytes) return;

    QFile::remove(path + ".3");
    QFile::rename(path + ".2", path + ".3");
    QFile::rename(path + ".1", path + ".2");
    QFile::rename(path, path + ".1");
}

LogList* initialize_logging_handler() {
    if (CURRENT_LOG_LIST) return CURRENT_LOG_LIST.get();

    qSetMessagePattern("[%{time yyyy-MM-dd hh:mm:ss.zzz}] [%{type}] %{message} "
                       "(%{file}:%{line})");

    CURRENT_LOG_STDOUT_STREAM = std::make_unique<QTextStream>(stdout);

    // Install log file
    QString const log_dir_path =
        QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) +
        "/logs";

    QDir().mkpath(log_dir_path);

    const QString log_file_path = log_dir_path + "/app.log";

    rotate_log_file(log_file_path);

    CURRENT_LOG_FILE = std::make_unique<QFile>(log_file_path);

    if (!CURRENT_LOG_FILE->open(QIODevice::WriteOnly | QIODevice::Append |
                                QIODevice::Text)) {
        CURRENT_LOG_FILE.reset();
    } else {
        CURRENT_LOG_FILE_STREAM =
            std::make_unique<QTextStream>(CURRENT_LOG_FILE.get());

        qInfo() << "Log file at:" << log_file_path;
    }


    CURRENT_LOG_LIST = std::make_unique<LogList>();

    qInstallMessageHandler(fileMessageHandler);

    qInfo() << "Application started.";

    return CURRENT_LOG_LIST.get();
}

LogList* current_log_list() {
    return CURRENT_LOG_LIST.get();
}


} // namespace SolTrace::GUI::App
