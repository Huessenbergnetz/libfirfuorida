/*
 * SPDX-FileCopyrightText: (C) 2019-2022 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include <QCoreApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QFileInfo>
#include <QDir>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <algorithm>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    app.setOrganizationName(QStringLiteral("Huessenbergnetz"));
    app.setOrganizationDomain(QStringLiteral("huessenbergnetz.de"));
    app.setApplicationName(QStringLiteral("firfuorida"));


    QCommandLineParser parser;

    QCommandLineOption createMigration(QStringList({QStringLiteral("c"), QStringLiteral("create")}), QStringLiteral("Create a new migration class with the given name."), QStringLiteral("name"));
    parser.addOption(createMigration);

    QCommandLineOption numberedMigration(QStringList({QStringLiteral("n"), QStringLiteral("number")}), QStringLiteral("Use ascending numbers instead of a date for sorting."));
    parser.addOption(numberedMigration);

    parser.addHelpOption();
    parser.addVersionOption();

    parser.process(app);

    if (parser.isSet(createMigration)) {

        QString name = parser.value(createMigration).trimmed();

        if (name.isEmpty()) {
            qCritical("%s", "The name can not be empty");
            return 1;
        }

        QString counter;
        if (parser.isSet(numberedMigration)) {
            const QDir current = QDir::current();
            const QFileInfoList fis = current.entryInfoList(QStringList({QStringLiteral("m*.cpp")}), QDir::Files|QDir::NoSymLinks|QDir::NoDotAndDotDot, QDir::Name);
            if (fis.empty()) {
                counter = QStringLiteral("0001");
            } else {
                int highest = 0;
                static QRegularExpression rex = QRegularExpression(QStringLiteral("^m(\\d\\d\\d\\d)_\\w+"));
                for (const QFileInfo &fi : fis) {
                    const QRegularExpressionMatch match = rex.match(fi.baseName());
                    if (match.hasMatch()) {
                        bool ok = false;
                        int n = match.captured(1).toInt(&ok);
                        if (ok) {
                            highest = std::max(highest, n);
                        }
                    }
                }
                highest++;
                counter = QStringLiteral("%1").arg(highest, 4, 10, QLatin1Char('0'));
            }
        } else {
            const QDateTime dt = QDateTime::currentDateTime();
            counter = dt.toString(QStringLiteral("yyyyMMdd'T'HHmmss"));
        }

        const QString className = QLatin1Char('M') + counter + QLatin1Char('_') + name;
        const QString hfName = className.toLower() + QStringLiteral(".h");
        const QString cfName = className.toLower() + QStringLiteral(".cpp");

        QFile hf(hfName);
        if (hf.exists()) {
            qCritical("File %s already exists!", qUtf8Printable(hfName));
            return 1;
        }

        QFile cf(cfName);
        if (cf.exists()) {
            qCritical("File %s already exists!", qUtf8Printable(cfName));
            return 1;
        }

        if (!hf.open(QFile::WriteOnly|QFile::Text)) {
            qCritical("Can not open %s for writing: %s", qUtf8Printable(hfName), qUtf8Printable(hf.errorString()));
            return 1;
        }

        if (!cf.open(QFile::WriteOnly|QFile::Text)) {
            qCritical("Can not open %s for writing: %s", qUtf8Printable(cfName), qUtf8Printable(hf.errorString()));
            return 1;
        }

        QTextStream hfOut(&hf);
        hfOut << "#ifndef " << className.toUpper() << "_H" << '\n';
        hfOut << "#define " << className.toUpper() << "_H" << '\n';
        hfOut << '\n';
        hfOut << "#include <Firfuorida/Migration>" << '\n';
        hfOut << '\n';
        hfOut << "class " << className << " final : public Firfuorida::Migration" << '\n';
        hfOut << '{' << '\n';
        hfOut << "    Q_OBJECT" << '\n';
        hfOut << "    Q_DISABLE_COPY(" << className << ')' << '\n';
        hfOut << "public:" << '\n';
        hfOut << "    explicit " << className << "(Firfuorida::Migrator *parent);" << '\n';
        hfOut << "    ~" << className << "() final;" << '\n';
        hfOut << '\n';
        hfOut << "    void up() final;" << '\n';
        hfOut << "    void down() final;" << '\n';
        hfOut << "};" << '\n';
        hfOut << '\n';
        hfOut << "#endif // " << className.toUpper() << "_H" << '\n';
        hfOut << '\n';
        hfOut.flush();

        QTextStream cfOut(&cf);
        cfOut << "#include \"" << hfName << '"' << '\n';
        cfOut << '\n';
        cfOut << className << "::" << className << "(Firfuorida::Migrator *parent) :" << '\n';
        cfOut << "    Firfuorida::Migration(parent)" << '\n';
        cfOut << '{' << '\n';
        cfOut << '\n';
        cfOut << '}' << '\n';
        cfOut << '\n';
        cfOut << className << "::~" << className << "()" << '\n';
        cfOut << '{' << '\n';
        cfOut << '\n';
        cfOut << '}' << '\n';
        cfOut << '\n';
        cfOut << "void " << className << "::up()" << '\n';
        cfOut << '{' << '\n';
        cfOut << '\n';
        cfOut << '}' << '\n';
        cfOut << '\n';
        cfOut << "void " << className << "::down()" << '\n';
        cfOut << '{' << '\n';
        cfOut << '\n';
        cfOut << '}' << '\n';
        cfOut << '\n';
        cfOut << "#include \"moc_" << className.toLower() << ".cpp\"" << '\n';
        cfOut << '\n';
        cfOut.flush();
    } else {
        parser.showHelp();
    }

    return 0;
}
