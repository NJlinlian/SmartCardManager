#include "GUIData.h"
//#include "card/CSmartCard.h"
#include "card/CardView.h"
#include "util/bitcoin/BCHUtil.h"

GUIData* GUIData::goo = nullptr;
QVector<QString> ERCAssetsName = {"USDT"};
GUIData::GUIData()
{
    initSystemEnvironmentPath();

    btcUrl = "https://chain.api.btc.com/v3";
    middlewareUrl = "http://39.108.173.165:5005/api";     // 除BCH使用
//    middlewareUrl = "http://192.168.1.121:5005/api";      // BCH测试
    ethWalletUrl = "http://127.0.0.1:60015";
    usdtContractAddress = "0x7c5a17da1813fff2f50f13dfe48070a43fca511e";

#ifndef VERSION_QML
    sc = new CSmartCard;
    qDebug() << sc->Init();
#else
    scView = new CardView;
#endif

    setObjectName("object guidata");

}

GUIData::~GUIData()
{
#ifndef VERSION_QML
    if(sc)  delete sc;
#else
    if(scView)  delete scView;
#endif
}

GUIData *GUIData::getInstance()
{
    if( goo == nullptr)
    {
        goo = new GUIData;
    }
    return goo;
}


MultisigInfoMap GUIData::readMultisigInfo(const QString& asset)
{
    QFile file( guiDataPath + "/multisigs.json");       // 每次启动清空 log.txt文件
    MultisigInfoMap map;
    if(file.open(QIODevice::ReadOnly))
    {
        QByteArray ba = file.readAll();
        QJsonObject coinObject = QJsonDocument::fromJson(ba).object();
        QJsonObject object = coinObject.value(asset).toObject();
        QStringList keys = object.keys();
        foreach (QString key, keys)
        {
            QJsonObject object2 = object.value(key).toObject();
            MultisigInfo info;
            info.required = object2.value("required").toInt();

            QJsonArray array = object2.value("pubKeys").toArray();
            foreach (QJsonValue value, array)
            {
                info.pubKeys << value.toString();
            }

            if(object2.contains("contractAddress"))     info.contractAddress = object2.value("contractAddress").toString();

            map.insert(key,info);
        }

        file.close();
    }
    return map;
}

void GUIData::writeMultisigInfo(const MultisigInfoMap &map, const QString &asset)
{
    QFile file( guiDataPath + "/multisigs.json");
    bool rt = file.open(QIODevice::ReadWrite);
    if(rt)
    {
        QByteArray ba = file.readAll();
        QJsonObject coinobject = QJsonDocument::fromJson(ba).object();

        QJsonObject object;
        QStringList keys = map.keys();
        foreach (QString key, keys)
        {
            MultisigInfo info = map.value(key);
            QJsonObject object2;
            object2.insert("required", info.required);
            QJsonArray array2 = QJsonArray::fromStringList(info.pubKeys);
            object2.insert("pubKeys", array2);
            if(asset == "ETH" && !info.contractAddress.isEmpty())
            {
                object2.insert("contractAddress", info.contractAddress);
            }

            object.insert(key, object2);
        }

        coinobject.insert(asset,object);

        file.resize(0);
        file.write( QJsonDocument(coinobject).toJson());

        file.close();
    }
}

//QVariantList GUIData::readMultisigAddresses()
//{
//    QVariantList list;
//    MultisigInfoMap mimap = readMultisigInfo(currentAsset);
//    foreach (QString key, mimap.keys())
//    {
//        list.append(QVariant::fromValue(mimap[key]));
//    }
//    return list;
//}

QVariantMap GUIData::readTest()
{
    QVariantMap vm;
    MultisigInfoMap mimap = readMultisigInfo(currentAsset);
    foreach (QString key, mimap.keys())
    {
        MultisigInfo* info = new MultisigInfo(mimap[key]);
        vm[key] = QVariant::fromValue<MultisigInfo*>(info);
    }

    return vm;
}

QQmlListProperty<MultisigInfo> GUIData::readMultisigAddresses()
{

    MultisigInfoMap mimap = readMultisigInfo(currentAsset);
    mi_list.clear();

    foreach (QString key, mimap.keys())
    {
        mi_list.append( new MultisigInfo(mimap[key]));
    }
qDebug() << "aaaaaa" << mi_list[0]->required <<  mi_list[0]->pubKeys;
//    mi_list.clear();
//    MultisigInfo* info = new MultisigInfo();
//    info->required = 2;
//    info->pubKeys << "111" << "222" << "333";
//    mi_list.append(info);

//    MultisigInfo* info2 = new MultisigInfo();
//    info2->required = 1;
//    info2->pubKeys << "aaa" << "bbb";
//    mi_list.append(info2);
//    qDebug() << "aaaaaa" << mi_list[0]->required <<  mi_list[0]->pubKeys;

    return QQmlListProperty<MultisigInfo>(this, mi_list);
}


unsigned char GUIData::getCurrentAssetAddressVersion()
{
    unsigned char version = 0;
    if(currentAsset == "BTC")
    {
        version = 0;
    }
    else if(currentAsset == "LTC")
    {
        version = 48;
    }

    return version;
}

QString GUIData::getCurrentAddress()
{
    QString address;
    if(currentAsset == "BTC" || currentAsset == "LTC")
    {
#ifndef VERSION_QML
        QByteArray pubkey = sc->readCompressedPubKey().toHex();
#else
        QByteArray pubkey = scView->getCompressedPubKey();
#endif
        address = QString::fromStdString( BitcoinUtil::get_address_by_pubkey(pubkey.toStdString(), getCurrentAssetAddressVersion()));
    }
    else if(GUIData::getInstance()->currentAsset == "ETH" || GUIData::getInstance()->currentAsset == "USDT")
    {
#ifndef VERSION_QML
        QByteArray pubkey = sc->readUncompressedPubKey();
#else
        QByteArray pubkey = scView->getUncompressedPubKey();
#endif
        address = EthereumUtil::pubkeyToAddress(pubkey);
    }
    else if(GUIData::getInstance()->currentAsset == "BCH")
    {
#ifndef VERSION_QML
        QByteArray pubkey = sc->readCompressedPubKey();
#else
        QByteArray pubkey = scView->getCompressedPubKey();
#endif
        QString oldAddress = QString::fromStdString( BitcoinUtil::get_address_by_pubkey(pubkey.toStdString(), getCurrentAssetAddressVersion()));
        address = BCHUtil::btcAddrToBch(oldAddress);
    }

    return address;
}

bool GUIData::isMultisigEnable(const QString &asset)
{
    return (asset == "BTC" || asset == "LTC");
}

QString GUIData::decimalToIntegerStr(QString number, int precision)
{
    return ::DecimalToIntegerStr(number, precision);
}

QString GUIData::intStrSetPrecision(QString amount, int precision)
{
    return ::amountSetPrecision(amount, precision);
}

QString GUIData::numberStrConvertBase(int old_base, int new_base, QString sourceStr)
{
    return ::convertPre(old_base, new_base, sourceStr);
}

void GUIData::setCurrentAsset(const QString& _asset)
{
    if(_asset != currentAsset)
    {
        currentAsset = _asset;
        Q_EMIT currentAssetChanged(currentAsset);
    }
}

void GUIData::initSystemEnvironmentPath()
{
    QStringList environment = QProcess::systemEnvironment();
    QString str;

#ifdef WIN32
    foreach(str,environment)
    {
        if (str.startsWith("APPDATA="))
        {

            guiDataPath = str.mid(8) + "\\SmartCardManager";
            qDebug() << "guiDataPath:" << guiDataPath;
            QDir dir(guiDataPath);
            if(!dir.exists())
            {
               qDebug() << "initSystemEnvironmentPath mkpath " <<  dir.mkpath(guiDataPath);
            }
            break;
        }
    }
#endif
}

unsigned long long jsonValueToULL(QJsonValue v)
{
    unsigned long long result = 0;
    if(v.isString())
    {
        result = v.toString().toULongLong();
    }
    else
    {
//        result = QString::number(v.toDouble(),'g',16).toULongLong();
        result = v.toVariant().toULongLong();
    }

    return result;
}

QString removeLastZeros(QString number)     // 去掉小数最后面的0
{
    if( !number.contains('.'))  return number;

    while (number.endsWith('0'))
    {
        number.chop(1);
    }

    if( number.endsWith('.'))   number.chop(1);

    return number;
}

QString getBigNumberString(unsigned long long number, int precision)
{
    QString str = QString::number(number);
    int size = precision;
    if( size < 0)  return "";
    if( size == 0) return str;

    if( str.size() > size)
    {
        str.insert(str.size() - size, '.');
    }
    else
    {
        // 前面需要补0
        QString zeros;
        zeros.fill('0',size - str.size() + 1);
        str.insert(0,zeros);
        str.insert(1,'.');
    }

    return removeLastZeros(str);
}

void tableWidgetSetItemStyle(QTableWidget *w, int alignment)
{
    for(int row = 0; row < w->rowCount(); row++)
    {
        for(int column = 0; column < w->columnCount(); column++)
        {
            QTableWidgetItem* item = w->item(row,column);
            if(item)
            {
                item->setTextAlignment(alignment);
            }
        }
    }
}

QString removeFrontZeros(QString number)     // 去掉整数最前面的0
{
    while (number.startsWith('0'))
    {
        number.remove(0,1);
    }

    if( number.isEmpty())   number = "0";

    return number;
}

QString DecimalToIntegerStr(QString number, int precision)
{
    int pos = number.indexOf(".");
    if( pos == -1)  pos = number.size();

    number.remove(".");
    int size = number.size();

    QString zeroStr;
    zeroStr.fill('0', precision - (size - pos) );

    number.append(zeroStr);
    number = number.mid(0, pos + precision);     // 万一原数据小数位数超过precision  舍去超过的部分

    return removeFrontZeros(number);
}

QString convertPre(int old_base, int new_base, QString sourceStr)
{
    std::string source_str = sourceStr.toStdString();
    std::string ret;
    int data[1010];
    int output[1010];
    memset(output, 0, sizeof(output));
    for (int i = 0; i < source_str.length(); i++) {
        if (isalpha(source_str[i]))
            if (isupper(source_str[i]))
            {
                data[i] = source_str[i] - 'A' + 10;
            }
            else {
                data[i] = source_str[i] - 'a' + 10;
            }
        else
            data[i] = source_str[i] - '0';
    }
    int sum = 1;
    int d = 0;
    int len = source_str.length();
    int k = 0;
    while (sum) {
        sum = 0;
        for (int i = 0; i < len; i++) {
            d = data[i] / new_base;
            sum += d;
            if (i == len - 1) {
                output[k++] = data[i] % new_base;
            }
            else {
                data[i + 1] += (data[i] % new_base) * old_base;
            }
            data[i] = d;
        }
    }
    if (k == 0) {
        output[k] = 0;
        k--;
    }
    if (k == -1) {
        ret = "0";
    }
    else {
        for (int i = 0; i < k; i++) {
            if (output[k - i - 1] > 9) {
                ret += (char)(output[k - i - 1] + 'a' - 10);
                //cout << (char)(output[k - i - 1] + 'a' - 10);
            }
            else {
                ret += output[k - i - 1] + '0';
            }
        }
    }
    return QString::fromStdString(ret);
}

QString amountSetPrecision(QString amount, int precision)
{
    std::string source = amount.toStdString();
    std::string ret;
    if (source.size() <= precision) {
        ret += "0.";
        std::string temp_precision((precision - source.size()), '0');
        ret += temp_precision;
        size_t amount_pos = source.find_last_not_of('0');
        if (amount_pos != source.npos)
        {
            ret += source.substr(0, amount_pos+1);
        }
        else {
            ret += source;
        }

    }
    else {
        ret += source.substr(0, source.size() - precision);
        std::string amountFloat = source.substr(source.size() - precision);
        size_t amount_pos = amountFloat.find_last_not_of('0');
        if (amount_pos != amountFloat.npos)
        {
            ret += '.';
            ret += amountFloat.substr(0, amount_pos + 1);
        }

    }
    return QString::fromStdString(ret);
}

QString findContractAddressInMap(const MultisigInfoMap &map, QString _contractAddress)
{
    QStringList keys = map.keys();
    foreach (QString key, keys)
    {
        if(map.value(key).contractAddress == _contractAddress)  return key;
    }
    return "";
}
