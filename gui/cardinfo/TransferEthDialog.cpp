#include "TransferEthDialog.h"
#include "ui_TransferEthDialog.h"

#include "GUIData.h"
#include "card/CSmartCard.h"
#include "util/ethereum/EthereumUtil.h"
#include "BroadcastTrxDialog.h"

TransferEthDialog::TransferEthDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TransferEthDialog)
{
    ui->setupUi(this);

    setWindowTitle(QString::fromLocal8Bit("ETH转账"));
    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);

    connect(&httpManager,SIGNAL(httpReplied(QByteArray,int)),this,SLOT(httpReplied(QByteArray,int)));

    ui->gasLimitLineEdit->setText("21000");
    ui->gasPriceSpinBox->setValue(50);
}

TransferEthDialog::~TransferEthDialog()
{
    delete ui;
}

void TransferEthDialog::setFromAddress(QString _fromAddr)
{
    fromAddress = _fromAddr;
    httpManager.getEthAddressNonce(fromAddress);
}

void TransferEthDialog::setToAddress(QString _toAddr)
{
    ui->toAddressLineEdit->setText(_toAddr);
}

void TransferEthDialog::setCreateContract(bool _isCreateContract)
{
    isCreateContract = _isCreateContract;
    if(isCreateContract)
    {
        ui->toAddressLineEdit->setText("0x0");
        ui->toAddressLineEdit->setReadOnly(true);
        ui->amountLineEdit->setText("0");
        ui->amountLineEdit->setReadOnly(true);
        ui->gasLimitLineEdit->setText("3000000");
    }
}

void TransferEthDialog::setCallContract(bool _isCallContract)
{
    isCallContract = _isCallContract;
    if(isCallContract)
    {
        ui->amountLineEdit->setText("0");
        ui->amountLineEdit->setReadOnly(true);
        ui->gasLimitLineEdit->setText("3000000");
    }
}

void TransferEthDialog::setData(QString _data)
{
    ui->dataLineEdit->setText(_data);
}

void TransferEthDialog::httpReplied(QByteArray _data, int _status)
{
    qDebug() << _data << _status;
    QJsonObject object  = QJsonDocument::fromJson(_data).object();
    if(object.isEmpty())    return;
    int id = object.value("id").toInt();
    if(id == 8101)
    {
        QString resultStr = object.value("result").toString();
        int nonce = resultStr.toInt(nullptr, 16);
        ui->nonceLabel->setText(QString::number(nonce));
    }
}


void TransferEthDialog::on_transferBtn_clicked()
{
    if(fromAddress.isEmpty())   return;
    if( !isCreateContract && (ui->toAddressLineEdit->text().size() != 42 || !ui->toAddressLineEdit->text().startsWith("0x")))   return;
    if( !isCreateContract && !isCallContract && ui->amountLineEdit->text().toDouble() <= 0)      return;
    if(ui->nonceLabel->text().isEmpty())    return;

    QString amountStr = DecimalToIntegerStr(ui->amountLineEdit->text(), 18);
    QString gasPriceStr = DecimalToIntegerStr(ui->gasPriceSpinBox->text(), 9);
    QString trx = EthereumUtil::transfer( isCreateContract ? "" : ui->toAddressLineEdit->text(), amountStr, ui->gasLimitLineEdit->text(),
                                                  gasPriceStr, ui->dataLineEdit->text(), ui->nonceLabel->text());
    if(trx.isEmpty())   return;
    trx.prepend("0x");

    close();
    BroadcastTrxDialog dialog;
    dialog.setRawTrx(trx);
    dialog.exec();

    trxId = dialog.trxId;
}
