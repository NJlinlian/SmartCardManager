#include "BitcoinUtil.h"

#include <QDebug>
#include "GUIData.h"
#include "util/SSLUtils.h"
#include "base58.h"
#include "BCHUtil.h"

// 有问题
QString BitcoinUtil::pubkeyToAddress(QString _pubkey)
{
    QByteArray data = QByteArray::fromHex(_pubkey.toUtf8());
    QByteArray ba1 = SSLUtils::sha256(data);
    QByteArray ba2 = SSLUtils::ripemd160(ba1);
    char ch = 0x00;
    ba2.prepend(ch);

    QByteArray ba3 = SSLUtils::sha256(ba2);
    QByteArray ba4 = SSLUtils::sha256(ba3);
    ba2.append(ba4.left(4));

//    QString result = base58Encode(QString::fromStdString( ba2.toHex().toStdString()));

//    return result;
    return "";
}


QString BitcoinUtil::makePubkeyScript(QString _pubkey)
{
    QByteArray data = QByteArray::fromHex(_pubkey.toUtf8());
    QByteArray ba1 = SSLUtils::sha256(data);
    QByteArray ba2 = SSLUtils::ripemd160(ba1);
    QString script = "76a914" + ba2.toHex() + "88ac";
    return script;
}


QString BitcoinUtil::multisigScriptToAddress(QByteArray scriptData, const QString &asset)
{
    QByteArray data = scriptData;
    QByteArray ba1 = SSLUtils::sha256(data);
    QByteArray ba2 = SSLUtils::ripemd160(ba1);
    char ch = 0x05;
    if(asset == "BTC")      ch = 0x05;
    else if(asset == "LTC") ch = 0x32;
    ba2.prepend(ch);
    QByteArray ba3 = SSLUtils::sha256(ba2);
    QByteArray ba4 = SSLUtils::sha256(ba3);
    ba2.append(ba4.left(4));

    QString result = QString::fromStdString( SSLUtils::base58(ba2.toHex().toStdString()));
    return result;
}


std::string BitcoinUtil::get_address_by_pubkey(const std::string& pubkey_hex_str, uint8_t version)
{
    //get public key
    libbitcoin::wallet::ec_public libbitcoin_pub(pubkey_hex_str);

    if(libbitcoin_pub == libbitcoin::wallet::ec_public())       qDebug() << "the pubkey hex str is invalid!";

    auto addr = libbitcoin_pub.to_payment_address(version);

    return addr.encoded();
}

QByteArray BitcoinUtil::serializeScriptUnit(int unit)
{
    if(unit == 0)
    {
        return QByteArray().append(char(0));
    }
    else if(unit < 16)
    {
        return QByteArray().append(char(unit + 80));
    }
    else
    {
        return QByteArray().append(char(unit));
    }
}

QByteArray BitcoinUtil::serializeScriptUnit(QByteArray unit)
{
    QByteArray ba;
    if( unit.size() <= 75)
    {
        ba.append(unit.size()).append(unit);
    }
    else if( unit.size() < 256)
    {
         ba.append(char(76)).append(unit.size()).append(unit);
    }
    // 后面用不到就没写
//    else if( unit.size() < 65536)
//    {
//         ba.append(char(77)).append(unit.size()).append(unit);
//    }
    return ba;
}


QVector<QByteArray> BitcoinUtil::deserializeScript(QByteArray script)
{
    QVector<QByteArray> vector;
    int pos = 0;
    while( pos < script.size())
    {
        int code = (unsigned char)(script.at(pos));
        if(code == 0)
        {
            vector.append(QByteArray().append(char(0)));
            pos++;
        }
        else if(code <= 75)
        {
            vector.append(QByteArray().append(script.mid(pos + 1, code)));
            pos = pos + 1 + code;
        }
        // 中间用不到就没写
        else if(code > 78 && code <= 96)
        {
            vector.append(QByteArray().append(char(code - 80)));
            pos++;
        }
        else if(code > 96)
        {
            vector.append(QByteArray().append(char(code)));
            pos++;
        }
        else
        {
            pos++;
        }
    }

    return vector;
}


QByteArray BitcoinUtil::makeMultisigRedeemScript(QStringList pubKeyList, int k)
{
    QByteArray ba;
    ba.append( serializeScriptUnit(k));
    foreach (QString pubKey, pubKeyList)
    {
        ba.append( serializeScriptUnit( QByteArray::fromHex(pubKey.toUtf8())));
    }
    ba.append( serializeScriptUnit(pubKeyList.size()));
    ba.append( serializeScriptUnit(174));

    return ba;
}

QByteArray BitcoinUtil::makeMultisigRedeemScript(const MultisigInfo &info)
{
    return makeMultisigRedeemScript(info.pubKeys, info.required);
}


QByteArray BitcoinUtil::makeMultisigSignatureScript(QStringList signatureHexList, QByteArray redeemScript, bool fillZero)
{
    QByteArray ba;
    ba.append( serializeScriptUnit(0));

    int count = int(redeemScript.at(redeemScript.size() - 2)) - 80;
    foreach (QString sig, signatureHexList)
    {
        if(sig.isEmpty())
        {
            ba.append( serializeScriptUnit(0));
        }
        else
        {
            ba.append( serializeScriptUnit( QByteArray::fromHex(sig.toUtf8())));
        }
        count--;
    }
    if(fillZero)
    {
        for(int i = 0; i < count; i++)
        {
            ba.append( serializeScriptUnit(0));
        }
    }

    ba.append( serializeScriptUnit(redeemScript));
    return ba;
}


int BitcoinUtil::validMultisigRedeemScriptStr(QString scriptStr)
{
    QStringList strList = scriptStr.split(" ");
    if(strList.size() < 4)  return -1;
    int count = strList.at(strList.size() - 2).toInt();
    if(count != strList.size() - 3)     return -2;
    if(strList.last() != "checkmultisig")   return -3;
    int required = strList.first().toInt();
    if(required < 1 || required > count)    return -4;
    return 1;
}


MultisigInfo BitcoinUtil::decodeMultisigRedeemScript(const QString &scriptHex)
{
    QVector<QByteArray> vector = deserializeScript( QByteArray::fromHex(scriptHex.toUtf8()));
    qDebug() <<  vector;
;

    MultisigInfo info;
    info.required = int(vector[0].at(0));
    for(int i = 1; i < vector.size() - 2; i++)
    {
        info.pubKeys << vector[i].toHex();
    }

    return  info;
}

QString BitcoinUtil::decoderawtransaction(const QString &trx, uint8_t kh, uint8_t sh)
{
    std::ostringstream obj;
    libbitcoin::chain::transaction  tx;
    try
    {
        tx.from_data(libbitcoin::config::base16(trx.toStdString()));
    }
    catch(...)
    {
        qDebug() << "decoderawtransaction error";
        return "";
    }
    libbitcoin::hash_digest hash; // =tx.hash(false);
//    std::reverse(hash.begin(),hash.end());
    QString hashStr = BitcoinUtil::btcTrxHash(trx,"01000000");
    obj << "{\"hash\": \"" << hashStr.toStdString() << "\",\"vin\": [";
    //insert input:
    libbitcoin::chain::transaction::ins ins = tx.inputs();
    auto int_size = ins.size();
    for (auto index = 0; index < int_size; index++)
    {
        if (index > 0)
            obj << ",";
        obj << "{";
        auto input = ins.at(index);
        auto previous_output = input.previous_output();
        hash = previous_output.hash();
        std::reverse(hash.begin(),hash.end());
        obj << "\"txid\": \"" << libbitcoin::encode_base16(hash);
        obj << "\",\"vout\": " << previous_output.index();

        obj <<",\"script\": \"" << input.script().to_string(libbitcoin::machine::all_rules) << "\",";
        obj << "\"sequence\": " << input.sequence() << "}";
    }
    obj << "],\
           \"lock_time\": " << tx.locktime() << ",\"vout\": [";

    auto ons = tx.outputs();
    auto out_size = ons.size();
    for (auto index = 0; index < out_size; index++)
    {
        if (index > 0)
            obj << ",";
        auto output = ons.at(index);
        obj << "{";
        obj << "\"scriptPubKey\": {";
        obj << "\"addresses\": [\"" << output.address(kh,sh) << "\"],";
        obj << "\"script\": \"" << output.script().to_string(libbitcoin::machine::all_rules) <<"\"},";
        obj << "\"value\": " << output.value() << "}";
    }
    obj << "]}";

    return QString::fromStdString( obj.str());

}

QString BitcoinUtil::addSignatureToBtcRawTrx(QByteArray signature, QByteArray pubKey, QString rawTrx, int index)
{
    libbitcoin::chain::script   libbitcoin_script;
    libbitcoin::chain::transaction  trx;
    trx.from_data(libbitcoin::config::base16(rawTrx.toStdString()));
    uint8_t hash_type = libbitcoin::machine::sighash_algorithm::all;
    std::string endorsment_script = QByteArray("[" + signature.toHex() + "]" + " [" + pubKey.toHex() + "] ").data();
    libbitcoin_script.from_string(endorsment_script);
    trx.inputs()[index].set_script(libbitcoin_script);

    std::string signed_trx = libbitcoin::encode_base16(trx.to_data());

    return QString::fromStdString( signed_trx);
}


QString BitcoinUtil::addScriptToVin(const QString &script, const QString &rawTrx, int index)
{
    libbitcoin::chain::transaction  trx;
    trx.from_data(libbitcoin::config::base16(rawTrx.toStdString()));
    libbitcoin::chain::script   libbitcoin_script;
    libbitcoin_script.from_data(libbitcoin::config::base16(script.toStdString()),false);
    trx.inputs()[index].set_script(libbitcoin_script);
//qDebug() << "AAAAAAAAAA " << QString::fromStdString( libbitcoin_script.to_string(libbitcoin::machine::all_rules));
    return QString::fromStdString( libbitcoin::encode_base16(trx.to_data()));
}

QString BitcoinUtil::btcTrxHash(QString rawTrx, QString hashCode)
{
    QByteArray ba = QByteArray::fromHex(rawTrx.toUtf8());
    ba.append(QByteArray::fromHex(hashCode.toUtf8()));
    QByteArray ba1 = SSLUtils::sha256(ba);
//    QByteArray ba2 = SSLUtils::sha256(ba1);

    return ba1.toHex();
}


QString BitcoinUtil::clearAllVinScript(const QString &rawTrx)
{
    libbitcoin::chain::transaction  trx;
    trx.from_data(libbitcoin::config::base16(rawTrx.toStdString()));
    int size = trx.inputs().size();
    for(int i = 0; i < size; i++)
    {
        libbitcoin::chain::script   libbitcoin_script;
        trx.inputs()[i].set_script(libbitcoin_script);
    }
    return QString::fromStdString( libbitcoin::encode_base16(trx.to_data()));
}

//BitcoinUtil::BitcoinUtilObject::~BitcoinUtilObject()
//{
//    if(info)
//    {
//        delete info;
//        info = nullptr;
//    }
//}

QString BitcoinUtil::BitcoinUtilObject::signRawTrx(const QString &asset, const QString &rawTrx)
{
    QString trx = BitcoinUtil::decoderawtransaction(rawTrx,0,5);
    QJsonObject object = QJsonDocument::fromJson(trx.toUtf8()).object();
    QJsonArray vinArray = object.value("vin").toArray();
    int vinCount = vinArray.size();
    QString singedTrxBuffer = rawTrx;
    QByteArray pubKey = GUIData::getInstance()->scView->getCompressedPubKey();

    for(int i = 0; i < vinCount; i++)
    {
        if(asset == "BCH")
        {
//            std::string redeemScript = BCHUtil::makeRedeemScript(pubKey.toHex().toStdString());
//            QByteArray hashHex = BCHUtil::createToSignData(redeemScript, rawTrx.toStdString(), i, vinAmounts[i]);
//            qDebug() << "hash: "  << hashHex;
//            int rt = GUIData::getInstance()->sc->signTrxHash(hashHex);
//            if(rt == 1)
//            {
//                QByteArray signatureFinal;
//                signatureFinal = GUIData::getInstance()->sc->signedData.append(0x41);
//                singedTrxBuffer = BitcoinUtil::addSignatureToBtcRawTrx(signatureFinal, pubKey, singedTrxBuffer, i);
//            }
        }
        else
        {
            QString script = BitcoinUtil::makePubkeyScript(pubKey);
            QString rawTrx2 = BitcoinUtil::addScriptToVin(script, rawTrx, i);
            QString hashStr = BitcoinUtil::btcTrxHash(rawTrx2,"01000000");
            qDebug() << "hashStr " << hashStr;

            int rt = GUIData::getInstance()->scView->signTrxHash(hashStr);
            if(rt == 1)
            {
                QByteArray signatureFinal;
                signatureFinal = GUIData::getInstance()->scView->getSignedData().append(0x01);
                qDebug() << "signatureFinal " << signatureFinal.toHex();

                singedTrxBuffer = BitcoinUtil::addSignatureToBtcRawTrx(signatureFinal, QByteArray::fromHex(pubKey), singedTrxBuffer, i);
            }
            else
            {
                return "";
            }
        }
    }
    return singedTrxBuffer;
}

QString BitcoinUtil::BitcoinUtilObject::addBtcMultisigAddress(const QStringList &pubkeysList, int required)
{
    QByteArray script = BitcoinUtil::makeMultisigRedeemScript(pubkeysList, required);
    QString address = BitcoinUtil::multisigScriptToAddress(script, "BTC");

    MultisigInfoMap map = GUIData::getInstance()->readMultisigInfo(GUIData::getInstance()->currentAsset);
    MultisigInfo info;
    info.required = required;
    info.pubKeys = pubkeysList;
    map.insert(address,info);
    GUIData::getInstance()->writeMultisigInfo(map, GUIData::getInstance()->currentAsset);
    return address;
}

void BitcoinUtil::BitcoinUtilObject::parseMultisigInfoFromTrx(const QString &rawTrx)
{
    if(info)
    {
        delete info;
        info = nullptr;
    }

    if(rawTrx.isEmpty())    return;
    QString trx = BitcoinUtil::decoderawtransaction(rawTrx,0,5);
    if(trx.isEmpty())   return;
    QJsonObject object = QJsonDocument::fromJson(trx.toUtf8()).object();
    qDebug() << "ooooooooo" << object;

    QJsonArray vinArray = object.value("vin").toArray();
    int vinCount = vinArray.size();
    QString firstStr;
    for(int i = 0; i < vinCount; i++)
    {
        QString scriptStr = vinArray.at(i).toObject().value("script").toString();
        if(i == 0)
        {
            firstStr = scriptStr;
        }

    }
    if(firstStr.isEmpty())  return;
    QStringList strList = firstStr.split(" ");
    QString redeemScriptHex = strList.last();
    redeemScriptHex.remove("[");
    redeemScriptHex.remove("]");
    info = new MultisigInfo(BitcoinUtil::decodeMultisigRedeemScript(redeemScriptHex));
        qDebug() << "Aaaaaaaaaaa" << info->required << info->pubKeys;
}

QString BitcoinUtil::BitcoinUtilObject::signMultisigTrx(const QString &asset, const QString &rawTrx)
{
    if(asset == "BTC" || asset == "LTC")
    {
        QString trx = BitcoinUtil::decoderawtransaction(rawTrx,0,5);
        QJsonObject object = QJsonDocument::fromJson(trx.toUtf8()).object();
        QJsonArray vinArray = object.value("vin").toArray();
        QString singedTrxBuffer = rawTrx;
        QString noScriptTrx = BitcoinUtil::clearAllVinScript(rawTrx);

        int rt = 0;
        bool sigEnough = false;
        for(int i = 0; i < vinArray.size(); i++)
        {
            QString scriptStr = vinArray.at(i).toObject().value("script").toString();
            QStringList ss = scriptStr.split(" ");
            QStringList strList;
            foreach (QString str, ss)
            {
                if(str == "zero")
                {
                    strList << "";
                }
                else
                {
                    str.remove("[");
                    str.remove("]");
                    strList << str;
                }
            }
            QString redeemScriptHex = strList.last();
            QString rawTrxBuffer = BitcoinUtil::addScriptToVin(redeemScriptHex, noScriptTrx, i);

            QString hashStr = BitcoinUtil::btcTrxHash(rawTrxBuffer,"01000000");
            rt = GUIData::getInstance()->scView->signTrxHash(hashStr);
            if(rt != 1) break;
            QByteArray signatureFinal = GUIData::getInstance()->scView->getSignedData().append(0x01);
            strList.removeFirst();
            strList.removeLast();
            MultisigInfo info = BitcoinUtil::decodeMultisigRedeemScript(redeemScriptHex);
            int index = info.pubKeys.indexOf(GUIData::getInstance()->scView->getCompressedPubKey());
            if(index == -1)     return "";
            strList.replace(index, signatureFinal.toHex());
            int count = 0;
            foreach (QString str2, strList)
            {
                if(!str2.isEmpty())     count++;
            }
            if(count >= info.required)       // 签名数量足够
            {
                strList.removeAll("");
                sigEnough = true;
            }

            QByteArray signedScript = BitcoinUtil::makeMultisigSignatureScript(strList, QByteArray::fromHex(redeemScriptHex.toUtf8()), false);
            singedTrxBuffer = BitcoinUtil::addScriptToVin(signedScript.toHex(), singedTrxBuffer, i);
        }

        if(rt != 1)
        {
            return "";
        }
        else if(sigEnough)
        {
            return singedTrxBuffer;
        }
        else
        {
            // 加个问号表示签名未满足
            return "?" + singedTrxBuffer;
        }
    }
}

MultisigInfo* BitcoinUtil::BitcoinUtilObject::getMultisigInfo()
{
    return info;
}

void BitcoinUtil::BitcoinUtilObject::deleteBtcMultisigAddress(const QString& multisigAddress)
{
    MultisigInfoMap map = GUIData::getInstance()->readMultisigInfo(GUIData::getInstance()->currentAsset);
    map.remove(multisigAddress);
    GUIData::getInstance()->writeMultisigInfo(map, GUIData::getInstance()->currentAsset);
}
