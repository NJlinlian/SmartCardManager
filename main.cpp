#include <QApplication>
#include <QQmlApplicationEngine>
#include <QFile>
#include <QQmlContext>
#include <QMutex>

#include "mainwindow.h"
#include "GUIData.h"
#include "common/HttpManager.h"
#include "gui/cardinfo/TransferBtcDialog.h"


void myMsgOutput(QtMsgType type, const QMessageLogContext &context, const QString& msg)
{
    static QMutex mutex;
    mutex.lock();
    QString time=QDateTime::currentDateTime().toString(QString("[ yyyy-MM-dd HH:mm:ss:zzz ]"));
    QString mmsg;
    switch(type)
    {
    case QtDebugMsg:
        mmsg=QString("%1: Debug:\t%2 (file:%3, line:%4, func: %5)").arg(time).arg(msg).arg(QString(context.file)).arg(context.line).arg(QString(context.function));
        break;
    case QtInfoMsg:
        mmsg=QString("%1: Info:\t%2 (file:%3, line:%4, func: %5)").arg(time).arg(msg).arg(QString(context.file)).arg(context.line).arg(QString(context.function));
        break;
    case QtWarningMsg:
        mmsg=QString("%1: Warning:\t%2 (file:%3, line:%4, func: %5)").arg(time).arg(msg).arg(QString(context.file)).arg(context.line).arg(QString(context.function));
        break;
    case QtCriticalMsg:
        mmsg=QString("%1: Critical:\t%2 (file:%3, line:%4, func: %5)").arg(time).arg(msg).arg(QString(context.file)).arg(context.line).arg(QString(context.function));
        break;
    case QtFatalMsg:
        mmsg=QString("%1: Fatal:\t%2 (file:%3, line:%4, func: %5)").arg(time).arg(msg).arg(QString(context.file)).arg(context.line).arg(QString(context.function));
        abort();
    }
    QFile file("debug.txt");
    file.open(QIODevice::ReadWrite | QIODevice::Append);
    QTextStream stream(&file);
    stream << mmsg << "\r\n";
    file.flush();
    file.close();
    mutex.unlock();
}



int main(int argc, char *argv[])
{
#ifdef VERSION_QML
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

//    qInstallMessageHandler(myMsgOutput);

    GUIData::getInstance();

    QQmlApplicationEngine engine;
    qmlRegisterType<HttpManager>("SCM", 1, 0, "HttpManager");
    qmlRegisterType<BitcoinUtil::BitcoinUtilObject>("SCM", 1, 0, "BitcoinUtil");
    qmlRegisterType<EthereumUtil::EthereumUtilObject>("SCM", 1, 0, "EthereumUtil");
    qmlRegisterType<MultisigInfo>("SCM", 1, 0, "MultisigInfo");
    engine.rootContext()->setContextProperty("$guiData", GUIData::getInstance());
    engine.rootContext()->setContextProperty("$cardView", GUIData::getInstance()->scView);
    engine.rootContext()->setContextProperty("$cardView", GUIData::getInstance()->scView);


//    qDebug() << "get erc20 decimal() param code "  <<  EthereumUtil::createERC20DecimalsData();

    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();

#else
    QApplication a(argc, argv);

//    QFile qss(":/gui/style/style.qss");
//    qss.open(QFile::ReadOnly);
//    qApp->setStyleSheet(qss.readAll());
//    qss.close();

    MainWindow w;
    w.show();



    return a.exec();
#endif
}
