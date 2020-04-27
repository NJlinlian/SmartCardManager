#include "TransferBtcDialog.h"
#include "ui_TransferBtcDialog.h"

#include <QLineEdit>

#include "GUIData.h"
#include "SignBtcTrxDialog.h"
#include "common/commondialog.h"

TransferBtcDialog::TransferBtcDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TransferBtcDialog)
{
    ui->setupUi(this);

    setWindowTitle(QString::fromLocal8Bit("转账"));
    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);

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

    ui->tableWidget->setColumnWidth(0,280);
    ui->tableWidget->setColumnWidth(1,100);
    ui->tableWidget->setColumnWidth(2,60);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);


    on_addAddressBtn_clicked();
}

TransferBtcDialog::~TransferBtcDialog()
{
    delete ui;
}

void TransferBtcDialog::httpReplied(QByteArray _data, int _status)
{
    qDebug() << _data << _status;
    QJsonObject object  = QJsonDocument::fromJson(_data).object();
    if(object.isEmpty())    return;
    int id = object.value("id").toInt();
    QJsonObject resultObject = object.value("result").toObject();
    if(id == 8002)
    {
        QJsonObject dataObject = resultObject.value("data").toObject();
        if(dataObject.isEmpty())
        {
            CommonDialog dialog(CommonDialog::OkOnly);
            dialog.setText("create trx failed!");
            dialog.pop();
            return;
        }

        QString rawTrx = dataObject.value("hex").toString();

        if(fromAddress.startsWith("1") || fromAddress.startsWith("L"))
        {
            SignBtcTrxDialog dialog;
            dialog.setRawTrx(rawTrx);
            dialog.exec();
        }
        else if(fromAddress.startsWith("bitcoincash:q"))
        {
            QJsonObject trxObject = dataObject.value("trx").toObject();
            QJsonArray trxVinArray = trxObject.value("vin").toArray();
            QVector<unsigned long long> vinAmountArray;
            for(QJsonValue v : trxVinArray)
            {
                vinAmountArray.append( jsonValueToULL(v.toObject().value("amount")));
            }
            SignBtcTrxDialog dialog;
            dialog.setRawTrx(rawTrx);
            dialog.setCoinType("BCH");
            dialog.setVinAmounts(vinAmountArray);
            dialog.exec();
        }
        else if(fromAddress.startsWith("bitcoincash:p"))
        {
            // BCH多签 由于签名需要amount 所以返回json格式
            MultisigInfoMap map = GUIData::getInstance()->readMultisigInfo(GUIData::getInstance()->currentAsset);
            MultisigInfo info = map.value(fromAddress);
            QByteArray redeemscript = BitcoinUtil::makeMultisigRedeemScript(info.pubKeys, info.required);
            QStringList strList;
            for(int i = 0; i < info.pubKeys.size(); i++)
            {
                strList << "";
            }
            QByteArray script = BitcoinUtil::makeMultisigSignatureScript(strList,redeemscript, true);

            QString trx = BitcoinUtil::decoderawtransaction(rawTrx,0,5);
            QJsonObject object2 = QJsonDocument::fromJson(trx.toUtf8()).object();
            QJsonArray trxVinArray = object2.value("vin").toArray();
            int vinCount = trxVinArray.size();
            QString trxBuffer = rawTrx;
            for(int i = 0; i < vinCount; i++)
            {
                trxBuffer = BitcoinUtil::addScriptToVin(script.toHex(), trxBuffer, i);
            }

            QJsonObject trxObject = dataObject.value("trx").toObject();
            QJsonArray vinArray = trxObject.value("vin").toArray();
            QJsonArray array;
            for(QJsonValue v : vinArray)
            {
                array.append( v.toObject().value("amount"));
            }
            QJsonObject resultObject;
            resultObject.insert("rawTrx", trxBuffer);
            resultObject.insert("vinAmount", array);

            CommonDialog dialog(CommonDialog::OkOnly);
            dialog.setText( QJsonDocument(resultObject).toJson());
            dialog.pop();
        }
        else if(fromAddress.startsWith("3") || fromAddress.startsWith("M"))
        {
            // 如果是多签转账 把多签地址的script塞进vin的script
            MultisigInfoMap map = GUIData::getInstance()->readMultisigInfo(GUIData::getInstance()->currentAsset);
            MultisigInfo info = map.value(fromAddress);
            QByteArray redeemscript = BitcoinUtil::makeMultisigRedeemScript(info.pubKeys, info.required);
            QStringList strList;
            for(int i = 0; i < info.pubKeys.size(); i++)
            {
                strList << "";
            }
            QByteArray script = BitcoinUtil::makeMultisigSignatureScript(strList,redeemscript, true);

            QString trx = BitcoinUtil::decoderawtransaction(rawTrx,0,5);
            QJsonObject object2 = QJsonDocument::fromJson(trx.toUtf8()).object();
            QJsonArray trxVinArray = object2.value("vin").toArray();
            int vinCount = trxVinArray.size();
            QString trxBuffer = rawTrx;
            for(int i = 0; i < vinCount; i++)
            {
                trxBuffer = BitcoinUtil::addScriptToVin(script.toHex(), trxBuffer, i);
            }

            CommonDialog dialog(CommonDialog::OkOnly);
            dialog.setText(trxBuffer);
            dialog.pop();
        }
        else
        {
            return;
        }

    }
}

void TransferBtcDialog::on_addAddressBtn_clicked()
{
    int rowCount = ui->tableWidget->rowCount();
    ui->tableWidget->setRowCount(rowCount + 1);
    QLineEdit* addressLineEdit = new QLineEdit(this);
    ui->tableWidget->setCellWidget(rowCount,0, addressLineEdit);

    QLineEdit* amountLineEdit = new QLineEdit(this);
    ui->tableWidget->setCellWidget(rowCount,1, amountLineEdit);

    ui->tableWidget->setItem(rowCount,2, new QTableWidgetItem(tr("delete")));

    tableWidgetSetItemStyle(ui->tableWidget);
}



void TransferBtcDialog::on_tableWidget_cellClicked(int row, int column)
{
    if(column == 2)
    {
        ui->tableWidget->removeRow(row);
        return;
    }
}

void TransferBtcDialog::on_createTrxBtn_clicked()
{
    if(ui->tableWidget->rowCount() < 1)     return;

    QJsonObject object;
    for(int i = 0; i < ui->tableWidget->rowCount(); i++)
    {
        QLineEdit* addressLineEdit = static_cast<QLineEdit*>(ui->tableWidget->cellWidget(i,0));
        QLineEdit* amountLineEdit = static_cast<QLineEdit*>(ui->tableWidget->cellWidget(i,1));
        if(addressLineEdit && amountLineEdit && !addressLineEdit->text().isEmpty() && amountLineEdit->text().toDouble() > 0)
        {
            object.insert(addressLineEdit->text(), amountLineEdit->text().toDouble());
        }
    }

    if(object.isEmpty())    return;

    httpManager.createTrx(fromAddress, object, GUIData::getInstance()->currentAsset);
}
