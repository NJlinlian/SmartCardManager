#include "BroadcastTrxDialog.h"
#include "ui_BroadcastTrxDialog.h"

#include "GUIData.h"
#include "common/commondialog.h"

BroadcastTrxDialog::BroadcastTrxDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BroadcastTrxDialog)
{
    ui->setupUi(this);

    connect(&httpManager,SIGNAL(httpReplied(QByteArray,int)),this,SLOT(httpReplied(QByteArray,int)));

}

BroadcastTrxDialog::~BroadcastTrxDialog()
{
    delete ui;
}

void BroadcastTrxDialog::setRawTrx(QString _rt)
{
    rawTrx = _rt;
    ui->rawTrxTextEdit->setPlainText( rawTrx);
}

void BroadcastTrxDialog::httpReplied(QByteArray _data, int _status)
{
    qDebug() << "BroadcastTrxDialog broadcastTrx" << _data << _status;
    QJsonObject object  = QJsonDocument::fromJson(_data).object();
    if(object.isEmpty())    return;
    int id = object.value("id").toInt();
    QJsonObject resultObject = object.value("result").toObject();
    if(id == 8003)
    {
        if(resultObject.keys().contains("error_code"))
        {
            CommonDialog dialog(CommonDialog::OkOnly);
            dialog.setText(tr("Fail to broadcast trx: %1").arg(resultObject.value("error_code").toInt()));
            dialog.exec();
        }
        else
        {
            trxId = resultObject.value("data").toString();
            CommonDialog dialog(CommonDialog::OkOnly);
            dialog.setText(tr("Trx has been broadcast, trxid : %1").arg(trxId));
            dialog.exec();
        }
    }

    close();
}

void BroadcastTrxDialog::on_broadcastBtn_clicked()
{
    httpManager.broadcastTrx(rawTrx, GUIData::getInstance()->currentAsset);
}

void BroadcastTrxDialog::on_cancelBtn_clicked()
{
    close();
}
