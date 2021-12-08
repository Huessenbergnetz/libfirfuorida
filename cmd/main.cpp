/*
 * SPDX-FileCopyrightText: (C) 2019-2021 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include <QCoreApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QFile>
#include <QTextStream>
#include <QDateTime>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    app.setOrganizationName(QStringLiteral("Huessenbergnetz"));
    app.setOrganizationDomain(QStringLiteral("huessenbergnetz.de"));
    app.setApplicationName(QStringLiteral("firfuorida"));


    QCommandLineParser parser;

    QCommandLineOption createMigration(QStringList({QStringLiteral("c"), QStringLiteral("create")}), QStringLiteral("Create a new migration class with the given name."), QStringLiteral("name"));
    parser.addOption(createMigration);

    parser.addHelpOption();
    parser.addVersionOption();

    parser.process(app);

    if (parser.isSet(createMigration)) {

        QString name = parser.value(createMigration).trimmed();

        if (name.isEmpty()) {
            qCritical("%s", "The name can not be empty");
            return 1;
        }

        name[0] = name.at(0).toUpper();
        const QDateTime dt = QDateTime::currentDateTime();
        const QString dtString = dt.toString(QStringLiteral("yyyyMMdd'T'HHmmss"));
        const QString className = QLatin1Char('M') + dtString + QLatin1Char('_') + name;
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
        hfOut << "#include <Firfuorida/migration.h>" << '\n';
        hfOut << '\n';
        hfOut << "class " << className << " : public Firfuorida::Migration" << '\n';
        hfOut << '{' << '\n';
        hfOut << "    Q_OBJECT" << '\n';
        hfOut << "    Q_DISABLE_COPY(" << className << ')' << '\n';
        hfOut << "public:" << '\n';
        hfOut << "    explicit " << className << "(Firfuorida::Migrator *parent);" << '\n';
        hfOut << "    ~" << className << "() override;" << '\n';
        hfOut << '\n';
        hfOut << "    void up() override;" << '\n';
        hfOut << "    void down() override;" << '\n';
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
