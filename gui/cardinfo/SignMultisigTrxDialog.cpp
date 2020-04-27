#include "SignMultisigTrxDialog.h"
#include "ui_SignMultisigTrxDialog.h"

#include "GUIData.h"
#include "card/CSmartCard.h"
#include "BroadcastTrxDialog.h"
#include "common/commondialog.h"
#include "util/bitcoin/BCHUtil.h"


SignMultisigTrxDialog::SignMultisigTrxDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SignMultisigTrxDialog)
{
    ui->setupUi(this);

    setWindowTitle(QString::fromLocal8Bit("签名多签交易"));
    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);

    ui->signBtn->setEnabled(false);

//    int rt = GUIData::getInstance()->sc->signTrxHash("786360379ee0b21c2f68a1af0721c2dac0556950ce4194f8e0697837604f3363");
//    QByteArray signatureFinal = GUIData::getInstance()->sc->signedData.append(0x01);
//    qDebug() << "mmmmmmmmmm "  << rt << signatureFinal.toHex();

//    BroadcastTrxDialog dialog;
//    dialog.setRawTrx("02000000021ce714af56476943fd8230f9d1d49a18cd39157af36d7fcf83aeecc832e2d70400000000fdfe0000483045022100f96b174a7eb0594088fafe56479b480f6346359393be694c6b3729861d40923a02204eb74fb850b00db1212e96b46f188d657cd199d8bdfe712b4b09ca737dc2528001483045022100a0308f2563d765e2c43f244046c54917cef0d28a9a6611f79dfcc2768e234cc002200a1adb09798333ff066e5e8baaf096554614a0a77edd188d0518d6473d99b319014c695221028e5fce316189786f8fe473fb54a7c411ae8ef1c4f51e76649189b00e0e99a20e21022edf425a960ec0c134251637031d7ce4f8ba7ce565c1e759a5f86bebb2bf55e921025b93a38f78158dd4c7387ed8532f08c403add54014c7d5d8a06a1416428c609b53aef0ffffff5685ef82c713f4a933db55eb76fd62b80315fd28c44efe3463435f72f6fcf3c300000000fdfd000047304402207dd2dcae191b3a96ff4509698addcc30cb853407226c8750f236dbb30e3a519602201d7486d35bd31d218f9c808cc68677d4f4efce041b5f5bf12d02296cfef29b5801483045022100a28cbc71b95ce2ce498988cbfa5df141a934add9136b9a86e0369fd7227bf2fc022027317ab021c1df1d986e071137b76f9113172e4370deb753d0d8c442c150d827014c695221028e5fce316189786f8fe473fb54a7c411ae8ef1c4f51e76649189b00e0e99a20e21022edf425a960ec0c134251637031d7ce4f8ba7ce565c1e759a5f86bebb2bf55e921025b93a38f78158dd4c7387ed8532f08c403add54014c7d5d8a06a1416428c609b53aef0ffffff02001bb700000000001976a914019ddcb33f0661a9e109f646facb6ecdae49c20d88ac60dc2c000000000017a91423c1405725486db1ec1e050cc54e8d5c30a660f38700000000");
//    dialog.exec();
}

SignMultisigTrxDialog::~SignMultisigTrxDialog()
{
    delete ui;
}

void SignMultisigTrxDialog::on_trxTextEdit_textChanged()
{
    ui->tipLabel->clear();
    ui->signBtn->setEnabled(false);

    QString text = ui->trxTextEdit->toPlainText();
    if( coin == "BCH")
    {
        QJsonParseError *error = new QJsonParseError;
        QJsonObject object = QJsonDocument::fromJson(text.toUtf8(), error).object();
        if(error->error == QJsonParseError::NoError)
        {
            QString rawTrx = object.value("rawTrx").toString();
            QJsonArray array = object.value("vinAmount").toArray();
            QVector<unsigned long long> vinAmountArray;
            for(QJsonValue v : array)
            {
                vinAmountArray.append( jsonValueToULL(v));
            }

            QString trx = BitcoinUtil::decoderawtransaction(rawTrx,0,5);
            QJsonObject object = QJsonDocument::fromJson(trx.toUtf8()).object();
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
//                if(scriptStr != firstStr)
//                {
//                    ui->tipLabel->setText(tr("Scripts are inconsistent!"));
//                    return;
//                }
            }

            QStringList strList = firstStr.split(" ");
            QString redeemScriptHex = strList.last();
            redeemScriptHex.remove("[");
            redeemScriptHex.remove("]");
            MultisigInfo info = BitcoinUtil::decodeMultisigRedeemScript(redeemScriptHex);
            qDebug() << "zzzzzzzzzzzz " << info.required << info.pubKeys;
            QByteArray pubKey = GUIData::getInstance()->sc->readCompressedPubKey();
            if(info.pubKeys.contains(pubKey.toHex()))
            {
                ui->tipLabel->setText(tr("you can sign this multisig trx"));
                ui->signBtn->setEnabled(true);
            }
            else
            {
                ui->tipLabel->setText(tr("you cannot sign this multisig trx"));
            }

        }
        else
        {
            ui->tipLabel->setText("parse json error: " + error->errorString());
        }

    }
    else
    {
        QString trx = BitcoinUtil::decoderawtransaction(ui->trxTextEdit->toPlainText(),0,5);
        qDebug() << "dddddddddd " << trx;
        QJsonObject object = QJsonDocument::fromJson(trx.toUtf8()).object();
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
//            if(scriptStr != firstStr)
//            {
//                ui->tipLabel->setText(tr("Scripts are inconsistent!"));
//                return;
//            }
        }

        QStringList strList = firstStr.split(" ");
        QString redeemScriptHex = strList.last();
        redeemScriptHex.remove("[");
        redeemScriptHex.remove("]");
        MultisigInfo info = BitcoinUtil::decodeMultisigRedeemScript(redeemScriptHex);
        QByteArray pubKey = GUIData::getInstance()->sc->readCompressedPubKey();
        if(info.pubKeys.contains(pubKey.toHex()))
        {
            ui->tipLabel->setText(tr("you can sign this multisig trx"));
            ui->signBtn->setEnabled(true);
        }
        else
        {
            ui->tipLabel->setText(tr("you cannot sign this multisig trx"));
        }
    }
}

void SignMultisigTrxDialog::on_signBtn_clicked()
{
    if(coin == "BTC" || coin == "LTC")
    {
        QString rawTrx = ui->trxTextEdit->toPlainText();
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
            rt = GUIData::getInstance()->sc->signTrxHash(hashStr);
            if(rt != 1) break;
            QByteArray signatureFinal = GUIData::getInstance()->sc->signedData.append(0x01);
            strList.removeFirst();
            strList.removeLast();
            MultisigInfo info = BitcoinUtil::decodeMultisigRedeemScript(redeemScriptHex);
            int index = info.pubKeys.indexOf(GUIData::getInstance()->sc->readCompressedPubKey().toHex());
            if(index == -1)     return;
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
            CommonDialog dialog(CommonDialog::OkOnly);
            dialog.setText("sign trx failed");
            dialog.exec();
        }
        else if(sigEnough)
        {
            close();
            BroadcastTrxDialog dialog;
            dialog.setRawTrx(singedTrxBuffer);
            dialog.exec();
        }
        else
        {
            CommonDialog dialog(CommonDialog::OkOnly);
            dialog.setText(singedTrxBuffer);
            dialog.exec();
        }
    }
    else if( coin == "BCH")
    {
        QJsonParseError *error = new QJsonParseError;
        QJsonObject withAmountObject = QJsonDocument::fromJson(ui->trxTextEdit->toPlainText().toUtf8(), error).object();
        if(error->error == QJsonParseError::NoError)
        {
            QString rawTrx = withAmountObject.value("rawTrx").toString();
            QJsonArray array = withAmountObject.value("vinAmount").toArray();
            QVector<unsigned long long> vinAmountArray;
            for(QJsonValue v : array)
            {
                vinAmountArray.append( jsonValueToULL(v));
            }

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

                QByteArray hashHex = BCHUtil::createToSignData(redeemScriptHex.toStdString(), rawTrx.toStdString(), i, vinAmountArray[i]);
                rt = GUIData::getInstance()->sc->signTrxHash(hashHex);
                qDebug() << "hash: " << hashHex;
                if(rt != 1) break;
                QByteArray signatureFinal = GUIData::getInstance()->sc->signedData.append(0x41);
                strList.removeFirst();
                strList.removeLast();
                MultisigInfo info = BitcoinUtil::decodeMultisigRedeemScript(redeemScriptHex);
                int index = info.pubKeys.indexOf(GUIData::getInstance()->sc->readCompressedPubKey().toHex());
                if(index == -1)     return;
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
                CommonDialog dialog(CommonDialog::OkOnly);
                dialog.setText("sign trx failed");
                dialog.exec();
            }
            else if(sigEnough)
            {
                close();
                BroadcastTrxDialog dialog;
                dialog.setRawTrx(singedTrxBuffer);
                dialog.exec();
            }
            else
            {
                withAmountObject.insert("rawTrx", singedTrxBuffer);
                CommonDialog dialog(CommonDialog::OkOnly);
                dialog.setText( QJsonDocument(withAmountObject).toJson());
                dialog.exec();
            }
        }
    }


    return;

//    if(coin == "BCH")
//    {
//        QJsonParseError *error = new QJsonParseError;
//        QJsonObject object = QJsonDocument::fromJson(ui->trxTextEdit->toPlainText().toUtf8(), error).object();
//        if(error->error == QJsonParseError::NoError)
//        {
//            QString rawTrx = object.value("rawTrx").toString();
//            QJsonArray array = object.value("vinAmount").toArray();
//            QVector<unsigned long long> vinAmountArray;
//            for(QJsonValue v : array)
//            {
//                vinAmountArray.append( jsonValueToULL(v));
//            }

//            QString trx = BitcoinUtil::decoderawtransaction(rawTrx,0,5);
//            QJsonObject object = QJsonDocument::fromJson(trx.toUtf8()).object();
//            qDebug() <<"ooooooooooooo " << object;
//            QJsonArray vinArray = object.value("vin").toArray();
//            if(vinArray.size() < 1)     return;
//            QString firstStr = vinArray.at(0).toObject().value("script").toString();
//            QStringList ss = firstStr.split(" ");
//            QStringList strList;
//            foreach (QString str, ss)
//            {
//                if(str == "zero")
//                {
//                    strList << "";
//                }
//                else
//                {
//                    str.remove("[");
//                    str.remove("]");
//                    strList << str;
//                }
//            }
//            QString redeemScriptHex = strList.last();
//            QString rawTrxBuffer = rawTrx;
//            QByteArray pubKey = GUIData::getInstance() ->sc->readCompressedPubKey();
//            for(int i = 0; i < vinArray.count(); i++)
//            {
//                libbitcoin::hash_digest higest = BCHUtil::createToSignData(redeemScriptHex.toStdString(), rawTrx.toStdString(), i, vinAmountArray[i]);
//                int rt = GUIData::getInstance()->sc->signTrxHash(QString::fromStdString( BinToHex(std::vector<char>(higest.begin(), higest.end()), false)));
//                if(rt == 1)
//                {
//                    QByteArray signatureFinal;
//                    signatureFinal = GUIData::getInstance()->sc->signedData.append(0x41);
//                    rawTrxBuffer = BitcoinUtil::addSignatureToBtcRawTrx(signatureFinal, pubKey, rawTrxBuffer, i);
//                }
//            }
//        }
//    }
//    else
//    {
//        QString rawTrx = ui->trxTextEdit->toPlainText();
//        QString trx = BitcoinUtil::decoderawtransaction(rawTrx,0,5);
//        QJsonObject object = QJsonDocument::fromJson(trx.toUtf8()).object();
//        qDebug() <<"ooooooooooooo " << object;
//        QJsonArray vinArray = object.value("vin").toArray();
//        if(vinArray.size() < 1)     return;
//        // 把每个vin的script都置为 0 0 ... redeemscript
//        QString firstStr = vinArray.at(0).toObject().value("script").toString();
//        QStringList ss = firstStr.split(" ");
//        QStringList strList;
//        foreach (QString str, ss)
//        {
//            if(str == "zero")
//            {
//                strList << "";
//            }
//            else
//            {
//                str.remove("[");
//                str.remove("]");
//                strList << str;
//            }
//        }
//        QString redeemScriptHex = strList.last();
//        QString rawTrxBuffer = rawTrx;

//        for(int i = 0; i < vinArray.count(); i++)
//        {
//            rawTrxBuffer = BitcoinUtil::addScriptToVin(redeemScriptHex, rawTrxBuffer, i);
//        }
//        QString hashStr = BitcoinUtil::btcTrxHash(rawTrxBuffer,"01000000");
//        qDebug() << "hhhhhhhhhhhhhhhhh " << hashStr;
//        qDebug() <<"rrrrrrrrrrrrrrrrrrr " << rawTrxBuffer;
//        int rt = GUIData::getInstance()->sc->signTrxHash(hashStr);
//        if(rt == 1)
//        {
//            QByteArray signatureFinal = GUIData::getInstance()->sc->signedData.append(0x01);
//            qDebug() << "222222222222 " << signatureFinal.toHex();
//            strList.removeFirst();
//            strList.removeLast();
//            MultisigInfo info = BitcoinUtil::decodeMultisigRedeemScript(redeemScriptHex);
//            int index = info.pubKeys.indexOf(GUIData::getInstance()->sc->readCompressedPubKey().toHex());
//            if(index == -1)     return;
//            strList.replace(index, signatureFinal.toHex());
//            int count = 0;
//            foreach (QString str2, strList)
//            {
//                if(!str2.isEmpty())     count++;
//            }
//            if(count >= info.required)      // 签名数量足够
//            {
//                strList.removeAll("");
//                QByteArray signedScript = BitcoinUtil::makeMultisigSignatureScript(strList, QByteArray::fromHex(redeemScriptHex.toUtf8()), false);
//                for(int i = 0; i < vinArray.count(); i++)
//                {
//                    rawTrxBuffer = BitcoinUtil::addScriptToVin(signedScript.toHex(), rawTrxBuffer, i);
//                }
//                close();
//                BroadcastTrxDialog dialog;
//                dialog.setRawTrx(rawTrxBuffer);
//                dialog.exec();
//            }
//            else
//            {
//                QByteArray signedScript = BitcoinUtil::makeMultisigSignatureScript(strList, QByteArray::fromHex(redeemScriptHex.toUtf8()), true);
//                for(int i = 0; i < vinArray.count(); i++)
//                {
//                    rawTrxBuffer = BitcoinUtil::addScriptToVin(signedScript.toHex(), rawTrxBuffer, i);
//                }

//                CommonDialog dialog(CommonDialog::OkOnly);
//                dialog.setText(rawTrxBuffer);
//                dialog.exec();
//            }

//        }
//    }
}
