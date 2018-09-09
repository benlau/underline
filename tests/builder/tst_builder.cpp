#include <QtTest>
#include <QtShell>
#include <underline.h>
#include <QProcess>

class Builder : public QObject
{
    Q_OBJECT

public:
    Builder();
    ~Builder();

    class Result {
    public:
        QStringList messages;
        QStringList errors;
        int exitCode;
    };

    Builder::Result build(const QString& name, const QString &code);

    QFile log;

private slots:
    void initTestCase();
    void cleanupTestCase();
    void spec_map_static_assert_cap_arg1_collection();
};

using namespace QtShell;

#define CODE(x) #x

Builder::Builder()
{
    log.setFileName("build_log.txt");
    log.open(QIODevice::WriteOnly);
}

Builder::~Builder()
{
    log.close();
}

static QString run(const QString& program, const QStringList& args = QStringList(), int* code = nullptr) {
    QProcess process;
    process.setProcessChannelMode(QProcess::MergedChannels);
    process.start(program, args, QIODevice::ReadWrite);

    process.waitForFinished(-1);

    QString p_stdout = process.readAllStandardOutput();

    if (code) {
        *code = process.exitCode();
    }
    return p_stdout;
}

static void writeTo(QString& path, QString& content) {

    QFile file(path);
    file.open(QIODevice::WriteOnly);
    file.write(content.toUtf8());
    file.close();

}

Builder::Result Builder::build(const QString& name, const QString &code)
{
    Builder::Result ret;

    QString normalizedCode = code;
    normalizedCode.remove(QRegExp("  +"));
    normalizedCode.remove(QRegExp("\n"));

    log.write(normalizedCode.toUtf8());
    log.write("\n");

    QString templatePath = realpath_strip(SRCDIR, "templates");
    QString buildPath = realpath_strip(pwd(), name, "build-code");

    rm("-rf", buildPath);
    mkdir("-p", buildPath);
    cp(templatePath + "/*", buildPath);

    QString cppFile = realpath_strip(buildPath, "template.cpp");

    QString content = cat(cppFile);
    content.replace("{{code}}", code);
    writeTo(cppFile, content);

    QString proFile = realpath_strip(buildPath, "template.pro");
    QString includePath = realpath_strip(SRCDIR, "../../src/cpp");
    content = cat(proFile);
    content.replace("{{includepath}}", includePath);
    writeTo(proFile, content);

    QDir::setCurrent(buildPath);

    run("qmake");

    QString make;
#ifdef Q_OS_WIN32
    make = "nmake";
#else
    make = "make";
#endif

    auto messages = run(make, QStringList(), &ret.exitCode).split("\n");
    auto errors = [=]() {
        QStringList output;
        for (int i = 0 ; i < messages.size() ; i++) {
            auto line = messages[i];
            if (line.indexOf("error:") >= 0) {
                output << line;
                log.write(line.toUtf8());
                log.write("\n");
            }
        }
        return output;
    }();

    ret.messages = messages;
    ret.errors = errors;

    if (ret.exitCode == 0 || ret.errors.size() > 5 || ret.errors.size() < 1) {
        qDebug() << ret.messages.join("\n");
    }

    return ret;
}

void Builder::initTestCase()
{
}

void Builder::cleanupTestCase()
{

}

void Builder::spec_map_static_assert_cap_arg1_collection()
{
    Result ret = build(__FUNCTION__, CODE([]() {
        _::map(std::vector<int>{1,2,3}, [](int) {});
    }));


    QVERIFY(ret.exitCode != 0);
    QVERIFY(ret.errors.size() <= 5);
    QVERIFY(ret.errors.size() > 0);

    QVERIFY(ret.errors[0].indexOf("static_assert") >= 0);
}

QTEST_APPLESS_MAIN(Builder)

#include "tst_builder.moc"
