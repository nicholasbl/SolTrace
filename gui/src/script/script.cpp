#include "script.h"

#include <QRegularExpression>

namespace SolTrace::GUI::Script {

namespace {

struct HeaderBlock {
    QString text;
    QString error;
};

QString normalize_header_line(QString line) {
    line = line.trimmed();
    if (line.startsWith('*')) { line = line.mid(1).trimmed(); }
    return line;
}

HeaderBlock first_comment_block(QString const& code) {
    if (code.startsWith("/*")) {
        auto end = code.indexOf("*/", 2);
        if (end < 0) { return { {}, "Unterminated block comment header" }; }
        return { code.mid(2, end - 2), {} };
    }

    if (!code.startsWith("//")) {
        return { {}, "Script must start with a comment header" };
    }

    QStringList lines;
    auto        all_lines = code.split('\n');

    for (auto const& raw_line : std::as_const(all_lines)) {
        auto line = raw_line;
        if (line.endsWith('\r')) { line.chop(1); }
        if (!line.startsWith("//")) { break; }
        lines << line.mid(2);
    }

    return { lines.join('\n'), {} };
}

bool parse_number(QString const& text, double& value) {
    bool ok = false;
    value   = text.toDouble(&ok);
    return ok;
}

void parse_range(QString const& range, ScriptProperty& property) {

    auto parts = range.split("..", Qt::KeepEmptyParts);

    if (parts.empty()) {
        property.error = "Invalid range";
        return;
    }

    auto lower = parts.value(0);
    auto upper = parts.value(1);

    if (upper.contains("=")) {
        upper                  = upper.replace("=", "");
        property.max_inclusive = true;
    }

    if (!lower.isEmpty()) {
        property.min_bounded = true;
        if (!parse_number(lower, property.min)) {
            property.error = "Invalid lower bound";
            return;
        }
    }

    if (!upper.isEmpty()) {
        property.max_bounded = true;
        if (!parse_number(upper, property.max)) {
            property.error = "Invalid upper bound";
            return;
        }
    }

    if (!property.min_bounded && !property.max_bounded) {
        property.error = "Range must include at least one bound";
        return;
    }

    if (property.min_bounded && property.max_bounded &&
        property.min > property.max) {
        property.error = "Lower bound exceeds upper bound";
    }
}

ScriptProperty parse_property(QString const& line) {
    auto parts = line.simplified().split(' ', Qt::SkipEmptyParts);

    ScriptProperty property;
    if (parts.size() < 3) {
        property.error = "PROPERTY requires name and type";
        return property;
    }

    property.name = parts[1];
    property.type = parts[2].toLower();
    if (parts.size() > 3) { property.extra = parts.mid(3).join(' '); }

    if (property.name.isEmpty()) {
        property.error = "PROPERTY name is empty";
        return property;
    }

    if (property.type == "integer" || property.type == "real") {
        if (property.extra.isEmpty()) {
            property.error = "Numeric PROPERTY requires a range";
            return property;
        }
        parse_range(property.extra, property);
        return property;
    }

    if (property.type == "vec3") {
        if (!property.extra.isEmpty() && property.extra != "unit") {
            property.error = "vec3 PROPERTY only supports unit";
            return property;
        }
        property.unit = property.extra;
        return property;
    }

    if (property.type == "string") { return property; }

    property.error = "Unsupported PROPERTY type";
    return property;
}

} // namespace

ScriptPropertyModel::ScriptPropertyModel(QObject* parent)
    : StructTableModel(parent) { }

Script::Script(QObject* parent)
    : QObject { parent }, m_properties { new ScriptPropertyModel(this) } {
    connect(this, &Script::code_changed, this, &Script::parse);
}

void Script::set_database(db::Database* db) {
    m_database = db;
}

bool Script::parse() {
    QStringList             errors;
    QVector<ScriptProperty> properties;
    QString                 title;
    QStringList             descriptions;

    auto header = first_comment_block(code());
    if (!header.error.isEmpty()) { errors << header.error; }

    bool saw_directive = false;
    bool saw_title     = false;

    if (errors.isEmpty()) {
        auto lines = header.text.split('\n');
        for (auto const& raw_line : std::as_const(lines)) {
            auto line = normalize_header_line(raw_line);
            if (line.isEmpty()) { continue; }

            auto keyword_end = line.indexOf(QRegularExpression("\\s"));
            auto keyword     = keyword_end < 0 ? line : line.left(keyword_end);
            auto rest =
                keyword_end < 0 ? QString {} : line.mid(keyword_end).trimmed();
            keyword = keyword.toUpper();

            if (!saw_directive) {
                saw_directive = true;
                if (keyword != "TITLE") {
                    errors << "Header must start with TITLE";
                }
            }

            if (keyword == "TITLE") {
                if (saw_title) {
                    errors << "Header contains multiple TITLE lines";
                }
                saw_title = true;
                title     = rest;
                if (title.isEmpty()) { errors << "TITLE requires text"; }
                continue;
            }

            if (keyword == "DESC") {
                descriptions << rest;
                continue;
            }

            if (keyword == "PROPERTY") {
                auto property = parse_property(line);
                if (!property.error.isEmpty()) {
                    errors << QString("PROPERTY %1: %2")
                                  .arg(property.name.isEmpty() ? "<unknown>"
                                                               : property.name,
                                       property.error);
                }
                properties << property;
                continue;
            }

            errors << QString("Unknown header directive: %1").arg(keyword);
        }
    }

    if (!saw_title && errors.isEmpty()) {
        errors << "Header must include TITLE";
    }

    set_title(title);
    set_description(descriptions.join('\n'));
    set_parse_errors(errors);
    set_valid(errors.isEmpty());
    m_properties->replace(properties);

    return valid();
}

void Script::run() {
    set_run_errors({});

    if (!m_database) {
        set_run_errors({ "No database available" });
        return;
    }

    // sync for now
    auto engine = std::make_unique<QJSEngine>();
    auto api    = std::make_unique<ScriptDBInterface>(m_database);

    QStringList stack_trace;
    engine->installExtensions(QJSEngine::AllExtensions);
    engine->newQObject(api.get());

    auto object = engine->evaluate(code(), title(), 1, &stack_trace);

    if (!stack_trace.isEmpty()) {
        set_run_errors({
            QString("Script evaluation exception: %1")
                .arg(object.property("name").toString()),
            QString("Line number %1: %2")
                .arg(object.property("lineNumber").toInt())
                .arg(object.toString()),
        });
        return;
    }

    // collect args

    QJSValueList list;

    for (auto const& arg : *m_properties) {
        bool ok = false;
        if (arg.type == "integer") {
            list << QJSValue(arg.value.toInt(&ok));
        } else if (arg.type == "real") {
            list << QJSValue(arg.value.toDouble(&ok));
        } else if (arg.type == "string") {
            list << QJSValue(arg.value);
        } else if (arg.type == "vec3") {
            auto value = arg.value;
            value.replace('{', ' ');
            value.replace('}', ' ');
            value.replace('[', ' ');
            value.replace(']', ' ');
            value.replace(',', ' ');

            auto parts = value.split(' ', Qt::SkipEmptyParts);

            auto dest = engine->newArray(3);

            switch (parts.size()) {
            case 1: {
                auto d = parts.value(0).toDouble(&ok);

                if (!ok) { break; }

                dest.setProperty(0, d);
                dest.setProperty(1, d);
                dest.setProperty(2, d);
                break;
            }
            case 3: {
                auto d1 = parts.value(0).toDouble(&ok);
                if (!ok) { break; }
                auto d2 = parts.value(1).toDouble(&ok);
                if (!ok) { break; }
                auto d3 = parts.value(2).toDouble(&ok);
                if (!ok) { break; }

                dest.setProperty(0, d1);
                dest.setProperty(1, d2);
                dest.setProperty(2, d3);
                break;
            } break;
            default: break;
            }
        }

        if (!ok) {
            set_run_errors(
                { QString("Invalid argument value for %1").arg(arg.name) });
            return;
        }
    }

    auto call_ret = object.call(list);

    if (call_ret.isError()) {
        set_run_errors({
            QString("Script evaluation exception: %1")
                .arg(object.property("name").toString()),
            QString("Line number %1: %2")
                .arg(object.property("lineNumber").toInt())
                .arg(object.toString()),
        });
    }
}

} // namespace SolTrace::GUI::Script