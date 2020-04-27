#include <QApplication>
#include <QQmlApplicationEngine>
#include <QFile>
#include <QQmlContext>

#include "mainwindow.h"
#include "GUIData.h"
#include "common/HttpManager.h"
#include "gui/cardinfo/TransferBtcDialog.h"


int main(int argc, char *argv[])
{
#ifdef VERSION_QML
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    GUIData::getInstance();

    QQmlApplicationEngine engine;
    qmlRegisterType<HttpManager>("SCM", 1, 0, "HttpManager");
    qmlRegisterType<BitcoinUtil::BitcoinUtilObject>("SCM", 1, 0, "BitcoinUtil");
    qmlRegisterType<MultisigInfo>("SCM", 1, 0, "MultisigInfo");
    engine.rootContext()->setContextProperty("$guiData", GUIData::getInstance());
    engine.rootContext()->setContextProperty("$cardView", GUIData::getInstance()->scView);
    engine.rootContext()->setContextProperty("$cardView", GUIData::getInstance()->scView);



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
