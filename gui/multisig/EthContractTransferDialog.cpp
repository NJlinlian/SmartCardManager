#include "EthContractTransferDialog.h"
#include "ui_EthContractTransferDialog.h"

#include "GUIData.h"
#include "gui/cardinfo/TransferEthDialog.h"
#include "util/ethereum/EthereumUtil.h"

EthContractTransferDialog::EthContractTransferDialog(QString _contractAddress, QWidget *parent) :
    contractAddress(_contractAddress),
    QDialog(parent),
    ui(new Ui::EthContractTransferDialog)
{
    ui->setupUi(this);

    setWindowTitle(QString::fromLocal8Bit("ETH多签合约提现"));
    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);

}

EthContractTransferDialog::~EthContractTransferDialog()
{
    delete ui;
}

void EthContractTransferDialog::on_transferBtn_clicked()
{
    QString data = EthereumUtil::createContractWithdrawData(ui->toAddressLineEdit->text(), DecimalToIntegerStr(ui->amountLineEdit->text(), 18));

    TransferEthDialog dialog;
    dialog.setFromAddress(GUIData::getInstance()->getCurrentAddress());
    dialog.setToAddress(contractAddress);
    dialog.setCallContract(true);
    dialog.setData(data);
    dialog.exec();

}
