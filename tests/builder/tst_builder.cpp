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

private slots:
    void initTestCase();
    void cleanupTestCase();

    void spec_map_static_assert_arg1_is_not_a_collection();
    void spec_toCollection_static_assert_arg1_is_not_a_map();
    void spec_registerQtType_static_assert_non_key_value_type();
    void spec_first_static_assert_collection_value_type_should_match_with_defaultValue();
};

using namespace QtShell;

static QString logFileName;
static int buildErrorCountThreshold = 10;

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

static void appendTo(QString& path, QStringList& content) {
    QFile file(path);
    file.open(QIODevice::ReadWrite);
    file.seek(file.size());
    _::forEach(content, [&](auto line) {
        file.write(line.toUtf8());
        file.write("\n");
    });
    file.close();
}

Builder::Result Builder::build(const QString& name, const QString &code)
{
    Builder::Result ret;

    QString normalizedCode = code;
    normalizedCode.remove(QRegExp("  +"));
    normalizedCode.remove(QRegExp("\n"));

    QStringList log;

    log << normalizedCode;

    QString templatePath = realpath_strip(SRCDIR, "templates");

    QString normalizedName = name;
    normalizedName.replace(":", "");
    QString buildPath = realpath_strip(pwd(), normalizedName, "build-code");

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

    QString make, error;
#ifdef Q_OS_WIN32
    make = "nmake";
    error = " error ";
#else
    make = "make";
    error = "error:";
#endif

    auto messages = run(make, QStringList(), &ret.exitCode).split("\n");
    auto errors = _::filter(messages, [=](auto line) {
        return line.indexOf(error) >= 0;
    });

    ret.messages = messages;
    ret.errors = errors;

    _::forEach(errors, [&](auto line) {
        log << line;
    });
    log << "";

    if (ret.exitCode == 0 || ret.errors.size() > 5 || ret.errors.size() < 1) {
        qDebug() << ret.messages.join("\n");
    }

    appendTo(logFileName, log);

    return ret;
}

void Builder::initTestCase()
{
    logFileName = realpath_strip(pwd(), "build_log.txt");
}

void Builder::cleanupTestCase()
{
}

void Builder::spec_map_static_assert_arg1_is_not_a_collection()
{
    Result ret = build(__FUNCTION__, CODE([]() {
        class A {};
        A a;
        _::map(a, [](int) { return 0;});
    }));

    QVERIFY(ret.exitCode != 0);
    QVERIFY(ret.errors.size() <= buildErrorCountThreshold);
    QVERIFY(ret.errors.size() > 0);

    QVERIFY(ret.errors[0].indexOf(_underline_input_type_is_not_array) >= 0);
}

void Builder::spec_toCollection_static_assert_arg1_is_not_a_map()
{
    Result ret = build(__FUNCTION__, CODE([]() {
        _::toCollection(std::vector<int>{0});
    }));

    QVERIFY(ret.exitCode != 0);
    QVERIFY(ret.errors.size() <= buildErrorCountThreshold);
    QVERIFY(ret.errors.size() > 0);

    QVERIFY(ret.errors[0].indexOf("The expected input is an valid Map container class") >= 0);
}

void Builder::spec_registerQtType_static_assert_non_key_value_type()
{
    Result ret = build(__FUNCTION__, CODE([]() {
        _::registerQtType<int>();
    }));

    QVERIFY(ret.exitCode != 0);

    QVERIFY(ret.errors.size() <= buildErrorCountThreshold);
    QVERIFY(ret.errors.size() > 0);

    QVERIFY(ret.errors[0].indexOf("_::registerQtType: Invalid argument type. It should be a QtMetable type. Check the document of _::isQtMetable for the list of supported types.") >= 0);
}

void Builder::spec_first_static_assert_collection_value_type_should_match_with_defaultValue()
{
    Result ret = build(__FUNCTION__, CODE([]() {
        QStringList collection;
        int defaultValue;
        _::first(collection, defaultValue);
    }));

    QVERIFY(ret.exitCode != 0);

    QVERIFY(ret.errors.size() <= buildErrorCountThreshold);
    QVERIFY(ret.errors.size() > 0);

    QVERIFY(ret.errors[0].indexOf(_underline_default_value_type_doesnot_match_with_collection_value_type) >= 0);
}

QTEST_APPLESS_MAIN(Builder)

#include "tst_builder.moc"
