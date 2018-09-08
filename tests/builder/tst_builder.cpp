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
        int retCode;
        QStringList messages;
    };

    Result build(const QString& code);

private slots:
    void initTestCase();
    void cleanupTestCase();
    void spec_run();

};

using namespace QtShell;

#define CODE(x) #x

Builder::Builder()
{

}

Builder::~Builder()
{
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

Builder::Result Builder::build(const QString &code)
{
    Result res;

    QString templatePath = realpath_strip(SRCDIR, "templates");
    QString buildPath = realpath_strip(pwd(), "build-code");

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

    auto messages = run(make, QStringList(), &res.retCode).split("\n");
    messages = [=]() {
        QStringList output;
        for (int i = 0 ; i < messages.size() ; i++) {
            auto line = messages[i];
            if (line.indexOf("error:") >= 0) {
                output << line;
            }
        }
        return output;
    }();

    res.messages = messages;

    return res;
}

void Builder::initTestCase()
{
}

void Builder::cleanupTestCase()
{

}

void Builder::spec_run()
{
    Result ret = build(CODE([]() {
        _::map(std::vector<int>{1,2,3}, [](int) {});
    }));

    qDebug() << ret.messages;
    QVERIFY(ret.retCode != 0);
    QVERIFY(ret.messages.size() <= 5);
    QVERIFY(ret.messages.size() > 0);

    QVERIFY(ret.messages[0].indexOf("static_assert") >= 0);
}

QTEST_APPLESS_MAIN(Builder)

#include "tst_builder.moc"
