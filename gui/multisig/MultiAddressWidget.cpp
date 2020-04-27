#include "MultiAddressWidget.h"
#include "ui_MultiAddressWidget.h"

#include "CreateMultisigDialog.h"
#include "CreateEthMultisigContractDialog.h"
#include "common/showcontentdialog.h"
#include "gui/cardinfo/TransferBtcDialog.h"
#include "common/commondialog.h"
#include "EthContractTransferDialog.h"
#include "EthMultisigInfoDialog.h"

MultiAddressWidget::MultiAddressWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MultiAddressWidget)
{
    ui->setupUi(this);

    connect(&httpManager,SIGNAL(httpReplied(QByteArray,int)),this,SLOT(httpReplied(QByteArray,int)));

    ui->tableWidget->installEventFilter(this);
    ui->tableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setFocusPolicy(Qt::NoFocus);
//    ui->tableWidget->setFrameShape(QFrame::NoFrame);
    ui->tableWidget->setMouseTracking(true);
    ui->tableWidget->setShowGrid(false);//隐藏表格线

    ui->tableWidget->horizontalHeader()->setSectionsClickable(true);
//    ui->tableWidget->horizontalHeader()->setFixedHeight(40);
    ui->tableWidget->horizontalHeader()->setVisible(true);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    ui->tableWidget->setColumnWidth(0,320);
    ui->tableWidget->setColumnWidth(1,140);
    ui->tableWidget->setColumnWidth(2,80);
    ui->tableWidget->setColumnWidth(3,80);
    ui->tableWidget->setColumnWidth(4,80);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);

    if(GUIData::getInstance()->currentAsset == "BTC")
    {
        ui->tableWidget->horizontalHeaderItem(0)->setText(QString::fromLocal8Bit("已添加的BTC多签地址"));
    }
    else if(GUIData::getInstance()->currentAsset == "LTC")
    {
        ui->tableWidget->horizontalHeaderItem(0)->setText(QString::fromLocal8Bit("已添加的LTC多签地址"));
    }
    else if(GUIData::getInstance()->currentAsset == "ETH")
    {
        ui->tableWidget->horizontalHeaderItem(0)->setText(QString::fromLocal8Bit("已添加的ETH多签合约"));
        connect(&timerForEthMultisigContract, &QTimer::timeout, this, &MultiAddressWidget::checkEthMultisigContractState);
        timerForEthMultisigContract.start(15000);
    }
    else if(GUIData::getInstance()->currentAsset == "BCH")
    {
        ui->tableWidget->horizontalHeaderItem(0)->setText(QString::fromLocal8Bit("已添加的BCH多签地址"));
    }

    showMultisigs();

}

MultiAddressWidget::~MultiAddressWidget()
{
    delete ui;
}

void MultiAddressWidget::httpReplied(QByteArray _data, int _status)
{
    QJsonObject object  = QJsonDocument::fromJson(_data).object();
    if(object.isEmpty())    return;
    int id = object.value("id").toInt();
    QJsonObject resultObject = object.value("result").toObject();
    if(id == 8001)
    {
        QString address = resultObject.value("address").toString();
        QString balanceStr = resultObject.value("balance").toString();
        for(int i = 0; i < ui->tableWidget->rowCount(); i++)
        {
            if(ui->tableWidget->item(i,0) && ui->tableWidget->item(i,1)
                    && ui->tableWidget->item(i,0)->text() == address)
            {
                ui->tableWidget->item(i,1)->setText(balanceStr);
                break;
            }
        }
    }
    else if(id == 8005)
    {
        QJsonObject respitTrxObject = resultObject.value("data").toObject().value("respit_trx").toObject();
        QString contractAddress = respitTrxObject.value("contractAddress").toString();
        QString trxId = respitTrxObject.value("transactionHash").toString();

        MultisigInfoMap map = GUIData::getInstance()->readMultisigInfo("ETH");
        if(!contractAddress.isEmpty() && map.contains(trxId))
        {
            map[trxId].contractAddress = contractAddress;
            GUIData::getInstance()->writeMultisigInfo(map, "ETH");
        }
    }


// btc.com
//    QJsonValue dataValue = object.value("data");
//    if(dataValue.isArray())
//    {
//        QJsonArray dataArray = dataValue.toArray();
//        int row = 0;
//        foreach (QJsonValue value, dataArray)
//        {
//            if(!value.isNull())
//            {
//                QJsonObject object2 = value.toObject();
//                AddressInfoFromServer info;
//                info.address = object2.value("address").toString();
//                info.balance = jsonValueToULL( object2.value("balance"));

//                if(ui->tableWidget->item(row,0) && ui->tableWidget->item(row,1))
//                {
//                    if(ui->tableWidget->item(row,0)->text() == info.address)
//                    {
//                        ui->tableWidget->item(row,1)->setText( getBigNumberString(info.balance,8));
//                    }
//                }
//            }
//            else
//            {
//                if(ui->tableWidget->item(row,1))
//                {
//                    ui->tableWidget->item(row,1)->setText("0");
//                }
//            }

//            row++;
//        }
//    }
//    else if(dataValue.isObject())
//    {
//        QJsonObject object2 = dataValue.toObject();
//        AddressInfoFromServer info;
//        info.address = object2.value("address").toString();
//        info.balance = jsonValueToULL( object2.value("balance"));

//        if(ui->tableWidget->item(0,0) && ui->tableWidget->item(0,1))
//        {
//            if(ui->tableWidget->item(0,0)->text() == info.address)
//            {
//                ui->tableWidget->item(0,1)->setText( getBigNumberString(info.balance,8));
//            }
//        }
//    }

}

void MultiAddressWidget::on_addBtn_clicked()
{
    if(GUIData::getInstance()->currentAsset == "BTC" || GUIData::getInstance()->currentAsset == "LTC" || GUIData::getInstance()->currentAsset == "BCH")
    {
        CreateMultisigDialog dialog;
        dialog.exec();
    }
    else
    {
        CreateEthMultisigContractDialog dialog;
        dialog.exec();
    }

    showMultisigs();
}

void MultiAddressWidget::showMultisigs()
{
    MultisigInfoMap map = GUIData::getInstance()->readMultisigInfo(GUIData::getInstance()->currentAsset);
    QStringList keys = map.keys();
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->setRowCount(keys.size());
    for(int i = 0; i < keys.size(); i++)
    {
        if(GUIData::getInstance()->currentAsset == "ETH")
        {
            QString trxId = keys.at(i);
            MultisigInfo info = map.value(trxId);
            if(info.contractAddress.isEmpty())
            {
                ui->tableWidget->setItem(i,0, new QTableWidgetItem(QString::fromLocal8Bit("多签合约创建中")));
            }
            else
            {
                ui->tableWidget->setItem(i,0, new QTableWidgetItem(info.contractAddress));
                // 查询合约余额
                httpManager.queryAssetBalance(info.contractAddress, GUIData::getInstance()->currentAsset);
            }

            ui->tableWidget->setItem(i,1, new QTableWidgetItem(tr("querying")));
            ui->tableWidget->setItem(i,2, new QTableWidgetItem("transfer"));
            ui->tableWidget->setItem(i,3, new QTableWidgetItem("detail"));
            ui->tableWidget->setItem(i,4, new QTableWidgetItem("delete"));
        }
        else
        {
            QString address = keys.at(i);
            ui->tableWidget->setItem(i,0, new QTableWidgetItem(address));
            ui->tableWidget->setItem(i,1, new QTableWidgetItem(tr("querying")));
            ui->tableWidget->setItem(i,2, new QTableWidgetItem("transfer"));
            ui->tableWidget->setItem(i,3, new QTableWidgetItem("detail"));
            ui->tableWidget->setItem(i,4, new QTableWidgetItem("delete"));
            httpManager.queryAssetBalance(address, GUIData::getInstance()->currentAsset);
        }
    }

    tableWidgetSetItemStyle(ui->tableWidget);
//    queryMultisigBalances(QStringList() << "3BmpxMsd9LxnS4fGjafsLeRbCkvG8PdBDJ" << "3AoQNQQWi3618PjvRrpL68NAae7YNc79CU");
//    httpManager.queryMultisigBalances(QStringList() << keys);

}

void MultiAddressWidget::deleteMultisig(QString multisigAddress)
{
    MultisigInfoMap map = GUIData::getInstance()->readMultisigInfo(GUIData::getInstance()->currentAsset);
    map.remove(multisigAddress);
    GUIData::getInstance()->writeMultisigInfo(map, GUIData::getInstance()->currentAsset);
}

void MultiAddressWidget::on_tableWidget_cellClicked(int row, int column)
{
    if( column == 0)
    {
        if( !ui->tableWidget->item(row, 0))    return;

        ShowContentDialog showContentDialog( ui->tableWidget->item(row, column)->text(),this);

        int x = ui->tableWidget->columnViewportPosition(column) + ui->tableWidget->columnWidth(column) / 2
                - showContentDialog.width() / 2;
        int y = ui->tableWidget->rowViewportPosition(row) - 10 + ui->tableWidget->horizontalHeader()->height();

        showContentDialog.move( ui->tableWidget->mapToGlobal( QPoint(x, y)));
        showContentDialog.exec();


        return;
    }


    if( column == 2)
    {
        if( !ui->tableWidget->item(row, 0))    return;

        if(GUIData::getInstance()->currentAsset == "ETH")
        {
            EthContractTransferDialog dialog(ui->tableWidget->item(row, 0)->text());
            dialog.exec();
        }
        else
        {
            TransferBtcDialog dialog;
            dialog.fromAddress = ui->tableWidget->item(row, 0)->text();
    //        dialog.balance = ui->balanceLabel->text().toDouble();
            dialog.exec();
        }


        return;
    }

    if( column == 3)
    {
        if( !ui->tableWidget->item(row, 0))    return;

        if(GUIData::getInstance()->currentAsset == "ETH")
        {
            EthMultisigInfoDialog dialog(ui->tableWidget->item(row, 0)->text());
            dialog.exec();
        }

        return;
    }


    if( column == 4)
    {
        if( !ui->tableWidget->item(row, 0))    return;

        CommonDialog dialog(CommonDialog::YesOrNo);
        dialog.setText(tr("Sure to delete this multisig address?"));
        if(dialog.pop())
        {

            QString multisigAddress = ui->tableWidget->item(row, 0)->text();
            deleteMultisig(multisigAddress);
            showMultisigs();
        }

        return;
    }
}

void MultiAddressWidget::checkEthMultisigContractState()
{
    MultisigInfoMap map = GUIData::getInstance()->readMultisigInfo("ETH");
    QStringList keys = map.keys();
    foreach (QString trxId, keys)
    {
        MultisigInfo info = map.value(trxId);
        if(info.contractAddress.isEmpty())
        {
            httpManager.queryTrx("ETH", trxId);
        }
    }
}
