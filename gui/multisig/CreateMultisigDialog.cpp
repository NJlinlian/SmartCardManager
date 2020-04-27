#include "CreateMultisigDialog.h"
#include "ui_CreateMultisigDialog.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "GUIData.h"
#include "AddPubKeyDialog.h"
#include "util/bitcoin/BCHUtil.h"

CreateMultisigDialog::CreateMultisigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateMultisigDialog)
{
    ui->setupUi(this);

    setWindowTitle(QString::fromLocal8Bit("创建多签地址"));
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

CreateMultisigDialog::~CreateMultisigDialog()
{
    delete ui;
}

void CreateMultisigDialog::showPubKeys()
{
    int size = pubKeys.size();
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->setRowCount(size);
    for(int i = 0; i < size; i++)
    {
        ui->tableWidget->setItem(i,0, new QTableWidgetItem(pubKeys.at(i)));
        ui->tableWidget->setItem(i,1, new QTableWidgetItem(tr("delete")));
    }

    tableWidgetSetItemStyle(ui->tableWidget);


    if(ui->numLineEdit->text().toInt() > pubKeys.size())
    {
        ui->numLabel->setText(QString("/ %1").arg(pubKeys.size()));
    }
}

void CreateMultisigDialog::on_addPubkeyBtn_clicked()
{
    AddPubKeyDialog dialog;
    dialog.exec();

    if(!dialog.pubKey.isEmpty())
    {
        pubKeys << dialog.pubKey;
        showPubKeys();

        ui->numLabel->setText(QString("/ %1").arg(pubKeys.size()));
    }
}


void CreateMultisigDialog::on_tableWidget_cellClicked(int row, int column)
{
    if(column == 1)
    {
        if(ui->tableWidget->item(row,column))
        {
            QString str = ui->tableWidget->item(row,0)->text();
            pubKeys.removeAll(str);
            showPubKeys();
        }

        return;
    }
}

void CreateMultisigDialog::on_createBtn_clicked()
{
    int k = ui->numLineEdit->text().toInt();
    QStringList pubKeyList;
    int size = ui->tableWidget->rowCount();
    for(int i = 0; i < size; i++)
    {
        if(ui->tableWidget->item(i,0))
        {
            pubKeyList << ui->tableWidget->item(i,0)->text();
        }
    }
    if( k < 1 || k > pubKeyList.size())     return;

    QByteArray script = BitcoinUtil::makeMultisigRedeemScript(pubKeyList, k);
    qDebug() << "script " << script.toHex();
    QString address = BitcoinUtil::multisigScriptToAddress(script, GUIData::getInstance()->currentAsset);
    if(GUIData::getInstance()->currentAsset == "BCH")   address = BCHUtil::btcAddrToBch(address);
    qDebug() << "multisig address" << address;

    MultisigInfoMap map = GUIData::getInstance()->readMultisigInfo(GUIData::getInstance()->currentAsset);
    MultisigInfo info;
    info.required = k;
    info.pubKeys = pubKeyList;
    map.insert(address,info);
    GUIData::getInstance()->writeMultisigInfo(map, GUIData::getInstance()->currentAsset);

    close();
}
