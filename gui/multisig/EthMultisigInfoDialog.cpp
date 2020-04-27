#include "EthMultisigInfoDialog.h"
#include "ui_EthMultisigInfoDialog.h"

#include "GUIData.h"
#include "gui/cardinfo/TransferEthDialog.h"
#include "common/commondialog.h"

#define SYMBOL_HASH_ETH "aaaebeba3810b1e6b70781f14b2d72c1cb89c0b2b320c43bb67ff79f562f5ff4"

EthMultisigInfoDialog::EthMultisigInfoDialog(QString _contractAddress, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EthMultisigInfoDialog)
{
    ui->setupUi(this);

    connect(&httpManager,SIGNAL(httpReplied(QByteArray,int)),this,SLOT(httpReplied(QByteArray,int)));

    setWindowTitle(QString::fromLocal8Bit("ETH多签合约详情"));
    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);

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

    ui->tableWidget->setColumnWidth(0,40);
    ui->tableWidget->setColumnWidth(1,300);
    ui->tableWidget->setColumnWidth(2,100);
    ui->tableWidget->setColumnWidth(3,60);
    ui->tableWidget->setColumnWidth(4,60);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);

    ui->contractAddressLabel->setText(_contractAddress);
    ui->addressLabel->setText(GUIData::getInstance()->getCurrentAddress());

    httpManager.ethCall(_contractAddress, EthereumUtil::createContractTrxCountData());
}

EthMultisigInfoDialog::~EthMultisigInfoDialog()
{
    delete ui;
}

void EthMultisigInfoDialog::httpReplied(QByteArray _data, int _status)
{
    QJsonObject object  = QJsonDocument::fromJson(_data).object();
    qDebug() << "eeeeeeeeee" << object;
    if(object.isEmpty())    return;
    int id = object.value("id").toInt();
    if(id == 8110)
    {
        if(object.value("result").isString())
        {
            QString result = object.value("result").toString();
            trxCount = result.toInt(nullptr,16);

            for(int i = 0; i < trxCount; i++)
            {
                httpManager.ethCall( ui->contractAddressLabel->text(), EthereumUtil::createContractTrxsData(i), 9000 + i);
                if(i > 20)  break;
            }
        }
    }
    else if(id >= 9000 && id < 9100)
    {
        int trxIndex = id - 9000;
        if(object.value("result").isString())
        {
            QString result = object.value("result").toString();
            if(result.size() == 2 + 64 * 4)
            {
                result = result.mid(2);
                QString symbolHash = result.mid(0,64);
                QString symbol;
                if(symbolHash == SYMBOL_HASH_ETH)   symbol = "ETH";

                QString toAddress = result.mid(64 + 24,64 - 24);
                QString amount = amountSetPrecision( convertPre(16,10,result.mid(128,64)), 18);
                bool isExecuted = result.mid(192,64).toInt(nullptr,16);


                int rowCount = ui->tableWidget->rowCount();
                ui->tableWidget->setRowCount(rowCount + 1);
                ui->tableWidget->setItem( rowCount, 0, new QTableWidgetItem(QString::number(trxIndex)));
                ui->tableWidget->setItem( rowCount, 1, new QTableWidgetItem(toAddress));
                ui->tableWidget->setItem( rowCount, 2, new QTableWidgetItem(amount + " ETH"));
                ui->tableWidget->setItem( rowCount, 4, new QTableWidgetItem());

                if(isExecuted)
                {
                    ui->tableWidget->setItem( rowCount, 3, new QTableWidgetItem("executed"));
                }
                else
                {
                    ui->tableWidget->setItem( rowCount, 3, new QTableWidgetItem("querying"));
                    queryConfirmations(trxIndex);
                }

                ui->tableWidget->sortItems(0);
                tableWidgetSetItemStyle(ui->tableWidget);
            }

        }
    }
    else if(id >= 9100 & id < 9200)
    {
        int trxIndex = id - 9100;
        if(object.value("result").isString())
        {
            QString result = object.value("result").toString();

            result = result.mid(2);
            int confirmCount = result.mid(64,64).toInt(nullptr,16);
            result = result.mid(128);
            if(result.size() != confirmCount * 64)  return;
            QStringList confirmedAddressList;
            for(int i = 0; i < confirmCount; i++)
            {
                confirmedAddressList << result.mid(i * 64 + 24, 64 - 24);
            }

            MultisigInfoMap map = GUIData::getInstance()->readMultisigInfo("ETH");
            QString key = findContractAddressInMap(map, ui->contractAddressLabel->text());
            if(!key.isEmpty() && ui->tableWidget->item(trxIndex,3) && ui->tableWidget->item(trxIndex,4))
            {
                MultisigInfo info = map.value(key);
                ui->tableWidget->item( trxIndex, 3)->setText( QString("%1 / %2").arg(confirmCount).arg(info.required));

                QString confirmText;
                if(info.pubKeys.contains(ui->addressLabel->text()))
                {
                    confirmText = confirmedAddressList.contains(ui->addressLabel->text().mid(2)) ? QString::fromLocal8Bit("已确认") : QString::fromLocal8Bit("点击确认");
                }
                else
                {
                    confirmText = QString::fromLocal8Bit("此卡不是owner");
                }
                ui->tableWidget->item( trxIndex, 4)->setText(confirmText);
            }


        }
    }
}

void EthMultisigInfoDialog::on_okBtn_clicked()
{

}

void EthMultisigInfoDialog::queryConfirmations(int _index)
{
    httpManager.ethCall( ui->contractAddressLabel->text(), EthereumUtil::createGetConfirmationsData(_index), 9100 + _index);
}

void EthMultisigInfoDialog::on_tableWidget_cellClicked(int row, int column)
{
    if( column == 4)
    {
        if( !ui->tableWidget->item(row, 0) || !ui->tableWidget->item(row, 4))    return;
        if(ui->tableWidget->item(row,4)->text() != QString::fromLocal8Bit("点击确认"))  return;

        CommonDialog dialog(CommonDialog::YesOrNo);
        dialog.setText(QString::fromLocal8Bit("是否确认该笔合约转账?"));
        if(dialog.pop())
        {
            QString data = EthereumUtil::createConfirmTrxData(ui->tableWidget->item(row,0)->text().toInt());

            TransferEthDialog dialog;
            dialog.setFromAddress(GUIData::getInstance()->getCurrentAddress());
            dialog.setToAddress(ui->contractAddressLabel->text());
            dialog.setCallContract(true);
            dialog.setData(data);
            dialog.exec();
        }


        return;
    }}
