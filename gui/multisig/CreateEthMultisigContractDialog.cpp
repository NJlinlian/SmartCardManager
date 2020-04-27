#include "CreateEthMultisigContractDialog.h"
#include "ui_CreateEthMultisigContractDialog.h"

#include "GUIData.h"
#include "AddPubKeyDialog.h"
#include "util/ethereum/EthereumUtil.h"
#include "gui/cardinfo/TransferEthDialog.h"

CreateEthMultisigContractDialog::CreateEthMultisigContractDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateEthMultisigContractDialog)
{
    ui->setupUi(this);

    setWindowTitle(QString::fromLocal8Bit("创建ETH多签合约"));
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

    ui->tableWidget->setColumnWidth(0,420);
    ui->tableWidget->setColumnWidth(1,60);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
}

CreateEthMultisigContractDialog::~CreateEthMultisigContractDialog()
{
    delete ui;
}

void CreateEthMultisigContractDialog::showAddress()
{
    int size = addressList.size();
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->setRowCount(size);
    for(int i = 0; i < size; i++)
    {
        ui->tableWidget->setItem(i,0, new QTableWidgetItem(addressList.at(i)));
        ui->tableWidget->setItem(i,1, new QTableWidgetItem(tr("delete")));
    }

    tableWidgetSetItemStyle(ui->tableWidget);


    if(ui->numLineEdit->text().toInt() > addressList.size())
    {
        ui->numLabel->setText(QString("/ %1").arg(addressList.size()));
    }
}

void CreateEthMultisigContractDialog::on_createBtn_clicked()
{

    int k = ui->numLineEdit->text().toInt();
    if( k < 1 || k > addressList.size())     return;

    QString contractCode = EthereumUtil::createMultisigContractCode(k, addressList);
    QString address = GUIData::getInstance()->getCurrentAddress();

    TransferEthDialog dialog;
    dialog.setFromAddress(address);
    dialog.setCreateContract(true);
    dialog.setData(contractCode);
    dialog.exec();

    if(!dialog.trxId.isEmpty())
    {
        // ETH 多签map的key是创建合约的交易id
        MultisigInfoMap map = GUIData::getInstance()->readMultisigInfo(GUIData::getInstance()->currentAsset);
        MultisigInfo info;
        info.required = k;
        info.pubKeys = addressList;
        map.insert(dialog.trxId,info);
        GUIData::getInstance()->writeMultisigInfo(map, GUIData::getInstance()->currentAsset);

        close();
    }


}

void CreateEthMultisigContractDialog::on_addAddressBtn_clicked()
{
    AddPubKeyDialog dialog;
    dialog.exec();

    if(!dialog.pubKey.isEmpty())
    {
        addressList << dialog.pubKey;
        showAddress();

        ui->numLabel->setText(QString("/ %1").arg(addressList.size()));
    }
}

void CreateEthMultisigContractDialog::on_tableWidget_cellClicked(int row, int column)
{
    if(column == 1)
    {
        if(ui->tableWidget->item(row,column))
        {
            QString str = ui->tableWidget->item(row,0)->text();
            addressList.removeAll(str);
            showAddress();
        }

        return;
    }
}
