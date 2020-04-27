#include "CardInfoWidget.h"
#include "ui_CardInfoWidget.h"

#include <QDebug>

#include "GUIData.h"
#include "card/CSmartCard.h"
#include "TransferBtcDialog.h"
#include "SignMultisigTrxDialog.h"
#include "util/ethereum/EthereumUtil.h"
#include "TransferEthDialog.h"

CardInfoWidget::CardInfoWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CardInfoWidget)
{
    ui->setupUi(this);

    connect(&httpManager,SIGNAL(httpReplied(QByteArray,int)),this,SLOT(httpReplied(QByteArray,int)));

    connect(&timer, &QTimer::timeout, this, &CardInfoWidget::onTimeout);
    timer.start(5000);

    ui->tipLabel->hide();

}

CardInfoWidget::~CardInfoWidget()
{
    delete ui;
}

void CardInfoWidget::httpReplied(QByteArray _data, int _status)
{
//    qDebug() << "CardInfoWidget querybtcbalance" << _data << _status;
    QJsonObject object  = QJsonDocument::fromJson(_data).object();
    if(object.isEmpty())    return;
    int id = object.value("id").toInt();
    QJsonObject resultObject = object.value("result").toObject();
    if(id == 8001)
    {
        QString address = resultObject.value("address").toString();
        QString balanceStr = resultObject.value("balance").toString();
        if(address == ui->addressLabel->text())
        {
            ui->balanceLabel->setText(balanceStr);
        }
    }



//    if(object.isEmpty())    return;
//    QJsonValue dataValue = object.value("data");
//    if(dataValue.isObject())
//    {
//        QJsonObject object2 = dataValue.toObject();
//        AddressInfoFromServer info;
//        info.address = object2.value("address").toString();
//        info.balance = jsonValueToULL( object2.value("balance"));

//        if(info.address == ui->addressLabel->text())
//        {
//            ui->balanceLabel->setText(getBigNumberString(info.balance,8));
//        }
//    }
//    else if(dataValue.isNull())
//    {
//        ui->balanceLabel->setText("0");
//    }
}

void CardInfoWidget::onTimeout()
{
    std::vector<std::wstring> readers = GUIData::getInstance()->sc->GetReaders();
//    qDebug() << readers;
    if(readers.size() > 0)
    {
        int connectRt = GUIData::getInstance()->sc->Connect(readers.at(0));
//        qDebug() << "ccccccccccccc" <<  connectRt;
        if(!connectRt)
        {
            ui->tipLabel->setText(tr("smart card undetected!"));
            ui->tipLabel->show();
            ui->widget->hide();
        }
        else
        {
            int aidRt = GUIData::getInstance()->sc->selectAID();
//            qDebug() << "aid" << aidRt;
            if(aidRt == 0)
            {
                ui->tipLabel->setText(tr("smart card has not been inited!"));
                ui->tipLabel->show();
                ui->widget->hide();
            }
            else
            {
                ui->tipLabel->hide();
                ui->widget->show();

                if(GUIData::getInstance()->currentAsset == "BTC" || GUIData::getInstance()->currentAsset == "LTC" || GUIData::getInstance()->currentAsset == "BCH")
                {
                    QByteArray pubkey = GUIData::getInstance()->sc->readCompressedPubKey();
                    ui->pubkeyLabel->setText(pubkey.toHex());
                }
                else if(GUIData::getInstance()->currentAsset == "ETH")    // ETH使用未压缩公钥
                {
                    QByteArray pubkey = GUIData::getInstance()->sc->readUncompressedPubKey();
                    ui->pubkeyLabel->setText(pubkey.toHex());
                }

                QString address = GUIData::getInstance()->getCurrentAddress();
                QString oldAddress = ui->addressLabel->text();
                ui->addressLabel->setText(address);

                if(address != oldAddress)
                {
                    ui->balanceLabel->setText(tr("querying"));
                    httpManager.queryAssetBalance( address, GUIData::getInstance()->currentAsset);
                }
            }
//            else if(aidRt == 1)
//            {
//                ui->tipLabel->hide();
//                ui->widget->show();

//                ui->walletTypeLabel->setText("BTC");

//                QByteArray pubkey = GUIData::getInstance()->sc->readCompressedPubKey();
//                ui->pubkeyLabel->setText(pubkey.toHex());

//                QString address = QString::fromStdString( BitcoinUtil::get_address_by_pubkey(pubkey.toHex().toStdString(), 0));
//                QString oldAddress = ui->addressLabel->text();
//                ui->addressLabel->setText(address);

//                if(address != oldAddress)
//                {
//                    ui->balanceLabel->setText(tr("querying"));
//                    httpManager.queryBtcBalance( address);
//                }
//            }
//            else if(aidRt == 2)
//            {
//                ui->tipLabel->hide();
//                ui->widget->show();

//                ui->walletTypeLabel->setText("ETH");
//                ui->pubkeyLabel->clear();
//                ui->addressLabel->clear();
//            }
        }

    }
    else
    {
        ui->tipLabel->setText(tr("device undetected!"));
        ui->tipLabel->show();
        ui->widget->hide();
    }
}

void CardInfoWidget::on_transferBtn_clicked()
{
    if(GUIData::getInstance()->currentAsset == "BTC" || GUIData::getInstance()->currentAsset == "LTC" || GUIData::getInstance()->currentAsset == "BCH")
    {
        TransferBtcDialog dialog;
        dialog.fromAddress = ui->addressLabel->text();
        dialog.balance = ui->balanceLabel->text().toDouble();
        dialog.exec();
    }
    else if(GUIData::getInstance()->currentAsset == "ETH")
    {
        TransferEthDialog dialog;
        dialog.setFromAddress(ui->addressLabel->text());
        dialog.exec();
    }
}

void CardInfoWidget::on_multisigSignBtn_clicked()
{
    SignMultisigTrxDialog dialog;
    dialog.setCoinType(GUIData::getInstance()->currentAsset);
    dialog.exec();
}
