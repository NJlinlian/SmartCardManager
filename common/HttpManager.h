#ifndef HTTPMANAGER_H
#define HTTPMANAGER_H

#include <QObject>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QTimer>


class HttpManager: public QObject
{
    Q_OBJECT
public:
    HttpManager();
    ~HttpManager();
    void get(const QString url);
    void post(const QString url, const QByteArray& data);

    Q_INVOKABLE void queryAssetBalance(const QString& address, const QString& asset);
    Q_INVOKABLE void createTrx(const QString& fromAddress, const QString& toDictStr, const QString& asset);
    void createTrx(const QString& fromAddress, QJsonObject toDict, const QString& asset);
    Q_INVOKABLE void broadcastTrx(const QString& rawTrx, const QString& asset);

    void queryMultisigBalances(QStringList addressList);

    Q_INVOKABLE void getEthAddressNonce(const QString& address, const QString& type = "latest");
    void ethRequest(const QString& method, QJsonArray params, int id);
    void ethCall(const QString& contractAddress, const QString& data, int id = 8110);
    Q_INVOKABLE void queryUSDTBalance(const QString& address);

    void queryTrx(const QString& symbol, const QString& id);

public slots:
    void requestFinished(QNetworkReply *reply);

signals:
    void httpReplied(QByteArray data, int statusCode);
    void httpError(int statusCode);

public:
    void setTimeoutSeconds(int _second);
private:
    QTimer timer;
    int timeoutSeconds = 0;

private:
    QNetworkRequest httpRequest;
    QNetworkAccessManager   networkAccessManager;
};

#endif // HTTPMANAGER_H
