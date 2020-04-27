#ifndef TRANSFERETHDIALOG_H
#define TRANSFERETHDIALOG_H

#include <QDialog>
#include "common/HttpManager.h"

namespace Ui {
class TransferEthDialog;
}

class TransferEthDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TransferEthDialog(QWidget *parent = nullptr);
    ~TransferEthDialog();

    void setFromAddress(QString _fromAddr);
    QString fromAddress;

    void setToAddress(QString _toAddr);

    void setCreateContract(bool _isCreateContract);
    bool isCreateContract = false;

    void setCallContract(bool _isCallContract);
    bool isCallContract = false;

    void setData(QString _data);

    QString trxId;
private slots:
    void httpReplied(QByteArray _data, int _status);

    void on_transferBtn_clicked();

private:
    Ui::TransferEthDialog *ui;
    HttpManager httpManager;
};

#endif // TRANSFERETHDIALOG_H
