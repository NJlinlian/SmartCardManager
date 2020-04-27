#include "SignBtcTrxDialog.h"
#include "ui_SignBtcTrxDialog.h"

#include "GUIData.h"
#include "card/CSmartCard.h"
#include "BroadcastTrxDialog.h"
#include "util/bitcoin/BCHUtil.h"

SignBtcTrxDialog::SignBtcTrxDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SignBtcTrxDialog)
{
    ui->setupUi(this);
}

SignBtcTrxDialog::~SignBtcTrxDialog()
{
    delete ui;
}

void SignBtcTrxDialog::setRawTrx(QString _rt)
{
    rawTrx = _rt;
    ui->trxTextEdit->setPlainText( rawTrx);
}

void SignBtcTrxDialog::on_signBtn_clicked()
{
    QString trx = BitcoinUtil::decoderawtransaction(rawTrx,0,5);
    qDebug() << "tttttttttttttt " << trx;
    QJsonObject object = QJsonDocument::fromJson(trx.toUtf8()).object();
    QJsonArray vinArray = object.value("vin").toArray();
    int vinCount = vinArray.size();
    QString singedTrxBuffer = rawTrx;
    QByteArray pubKey = GUIData::getInstance()->sc->readCompressedPubKey();
    for(int i = 0; i < vinCount; i++)
    {
        if(coin == "BCH")
        {
            std::string redeemScript = BCHUtil::makeRedeemScript(pubKey.toHex().toStdString());
            QByteArray hashHex = BCHUtil::createToSignData(redeemScript, rawTrx.toStdString(), i, vinAmounts[i]);
            qDebug() << "hash: "  << hashHex;
            int rt = GUIData::getInstance()->sc->signTrxHash(hashHex);
            if(rt == 1)
            {
                QByteArray signatureFinal;
                signatureFinal = GUIData::getInstance()->sc->signedData.append(0x41);
                singedTrxBuffer = BitcoinUtil::addSignatureToBtcRawTrx(signatureFinal, pubKey, singedTrxBuffer, i);
            }
        }
        else
        {
            QString script = BitcoinUtil::makePubkeyScript(pubKey.toHex());
            QString rawTrx2 = BitcoinUtil::addScriptToVin(script, rawTrx, i);
            QString hashStr = BitcoinUtil::btcTrxHash(rawTrx2,"01000000");
            int rt = GUIData::getInstance()->sc->signTrxHash(hashStr);
            if(rt == 1)
            {
                QByteArray signatureFinal;
                signatureFinal = GUIData::getInstance()->sc->signedData.append(0x01);
                singedTrxBuffer = BitcoinUtil::addSignatureToBtcRawTrx(signatureFinal, pubKey, singedTrxBuffer, i);
            }
            else
            {
                return;
            }
        }
    }
    qDebug() << "signed trx" << singedTrxBuffer;
    qDebug() << "ssssssssss " <<  BitcoinUtil::decoderawtransaction(singedTrxBuffer,0,5);

    close();

    BroadcastTrxDialog dialog;
    dialog.setRawTrx(singedTrxBuffer);
    dialog.exec();

}
