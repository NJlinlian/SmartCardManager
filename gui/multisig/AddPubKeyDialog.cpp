#include "AddPubKeyDialog.h"
#include "ui_AddPubKeyDialog.h"

#include "GUIData.h"

AddPubKeyDialog::AddPubKeyDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddPubKeyDialog)
{
    ui->setupUi(this);

    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);

    if(GUIData::getInstance()->currentAsset == "ETH")
    {
        setWindowTitle(QString::fromLocal8Bit("添加ETH地址"));
        ui->label->setText(QString::fromLocal8Bit("输入ETH地址"));
    }
    else
    {
        setWindowTitle(QString::fromLocal8Bit("添加公钥"));
    }

}

AddPubKeyDialog::~AddPubKeyDialog()
{
    delete ui;
}

void AddPubKeyDialog::on_okBtn_clicked()
{
    pubKey = ui->pubKeyLineEdit->text();
    close();
}

void AddPubKeyDialog::on_cancelBtn_clicked()
{
    close();
}
