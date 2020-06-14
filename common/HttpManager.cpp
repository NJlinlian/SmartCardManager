#include "HttpManager.h"

#include "GUIData.h"

HttpManager::HttpManager()
{
    httpRequest.setRawHeader("Content-Type","application/json");
    connect(&networkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(requestFinished(QNetworkReply*))); //关联信号和槽
}

HttpManager::~HttpManager()
{

}

void HttpManager::get(const QString url)
{
    httpRequest.setUrl(QUrl(url));
    networkAccessManager.get(httpRequest);
}

void HttpManager::post(const QString url, const QByteArray &data)
{
    if(timeoutSeconds > 0)
    {
        connect(&timer, &QTimer::timeout, [&](){
            Q_EMIT httpError(0);
            timer.stop();
        });
        timer.start(timeoutSeconds * 1000);
    }

    httpRequest.setUrl(QUrl(url));
    networkAccessManager.post(httpRequest, data);
}

void HttpManager::queryAssetBalance(const QString &address, const QString &asset)
{
    if(address.isEmpty()) return;
    QJsonObject object;
    object.insert("jsonrpc","2.0");
    object.insert("id",8001);
    object.insert("method","Zchain.Address.GetBalance");
    QJsonObject paramObject;
    paramObject.insert("chainId",asset);
    paramObject.insert("addr",address);
    object.insert("params",paramObject);
    post(GUIData::getInstance()->middlewareUrl,QJsonDocument(object).toJson());
}

void HttpManager::createTrx(const QString &fromAddress, const QString& toDictStr, const QString &asset)
{
    createTrx(fromAddress, QJsonDocument::fromJson(toDictStr.toUtf8()).object(), asset);
}

void HttpManager::createTrx(const QString &fromAddress, QJsonObject toDict, const QString& asset)
{
    // BTC系的资产通过中间件构造交易
    if(fromAddress.isEmpty() || toDict.isEmpty()) return;
    QJsonObject object;
    object.insert("jsonrpc","2.0");
    object.insert("id",8002);
    object.insert("method","Zchain.Trans.createTrx");
    QJsonObject paramObject;
    paramObject.insert("chainId",asset.toLower());
    paramObject.insert("from_addr",fromAddress);
    paramObject.insert("dest_info",toDict);
    object.insert("params",paramObject);
    post(GUIData::getInstance()->middlewareUrl,QJsonDocument(object).toJson());
    qDebug() << "createTrx: " << QJsonDocument(object).toJson();
}

void HttpManager::broadcastTrx(const QString &rawTrx, const QString& asset)
{
    if(rawTrx.isEmpty()) return;
    QString chain = asset.toLower();
    if(ERCAssetsName.contains(asset))     chain = "eth";
    QJsonObject object;
    object.insert("jsonrpc","2.0");
    object.insert("id",8003);
    object.insert("method","Zchain.Trans.broadcastTrx");
    QJsonObject paramObject;
    paramObject.insert("chainId",chain);
    paramObject.insert("trx", rawTrx);
    object.insert("params",paramObject);
    post(GUIData::getInstance()->middlewareUrl,QJsonDocument(object).toJson());
}

void HttpManager::queryMultisigBalances(QStringList addressList)
{
    if(addressList.size() < 1)  return;
    get(GUIData::getInstance()->btcUrl + "/address/" + addressList.join(","));
    qDebug() << GUIData::getInstance()->btcUrl + "/address/" + addressList.join(",");
}

void HttpManager::getEthAddressNonce(const QString &address, const QString &type)
{
    if(address.isEmpty()) return;
    QJsonObject object;
    object.insert("jsonrpc","2.0");
    object.insert("id",8101);
    object.insert("method","Zchain.Trans.getEthTrxCount");
    QJsonObject paramObject;
    paramObject.insert("chainId","eth");
    paramObject.insert("addr", address);
    paramObject.insert("indexFormat", type);
    object.insert("params",paramObject);
    post(GUIData::getInstance()->middlewareUrl,QJsonDocument(object).toJson());
}

void HttpManager::ethRequest(const QString &method, QJsonArray params, int id)
{
    QJsonObject object;
    object.insert("jsonrpc","2.0");
    object.insert("id",id);
    object.insert("method",method);
    object.insert("params",params);
    post(GUIData::getInstance()->ethWalletUrl,QJsonDocument(object).toJson());

    qDebug() << "ethRequest:" << object;
}

void HttpManager::ethCall(const QString &contractAddress, const QString &data, int id)
{
    QJsonArray array;
    QJsonObject object;
    object.insert("to", contractAddress);
    object.insert("data", data);
    object.insert("gas", "0x1000000");
    object.insert("value", "0x0");
    array.append(object);
    array.append("pending");
    ethRequest("eth_call", array, id);
}

void HttpManager::queryUSDTBalance(const QString &address)
{
    ethCall(GUIData::getInstance()->usdtContractAddress,
            EthereumUtil::createERC20BalanceOfData(address),
            8111);
}


void HttpManager::queryTrx(const QString &symbol, const QString &id)
{
    QJsonObject object;
    object.insert("jsonrpc","2.0");
    object.insert("id",8005);
    object.insert("method","Zchain.Trans.queryTrans");
    QJsonObject paramObject;
    paramObject.insert("chainId",symbol.toLower());
    paramObject.insert("trxid", id);
    object.insert("params",paramObject);
    post(GUIData::getInstance()->middlewareUrl,QJsonDocument(object).toJson());
}

void HttpManager::requestFinished(QNetworkReply *reply)
{
    timer.stop();
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

//    qDebug() << "HttpManager statusCode: " << statusCode << reply->errorString();

    if(reply->error() == QNetworkReply::NoError)
    {
        emit httpReplied(reply->readAll(),statusCode);
    }
    else
    {
        emit httpError(statusCode);
    }

    reply->deleteLater();
}

void HttpManager::setTimeoutSeconds(int _second)
{
    timeoutSeconds = _second;
}
