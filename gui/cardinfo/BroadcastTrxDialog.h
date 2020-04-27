#ifndef BROADCASTTRXDIALOG_H
#define BROADCASTTRXDIALOG_H

#include <QDialog>
#include "common/HttpManager.h"

namespace Ui {
class BroadcastTrxDialog;
}

class BroadcastTrxDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BroadcastTrxDialog(QWidget *parent = nullptr);
    ~BroadcastTrxDialog();

    QString rawTrx;
    void setRawTrx(QString _rt);

    QString trxId;

private slots:
    void httpReplied(QByteArray _data, int _status);

    void on_broadcastBtn_clicked();

    void on_cancelBtn_clicked();

private:
    Ui::BroadcastTrxDialog *ui;
    HttpManager httpManager;
};

#endif // BROADCASTTRXDIALOG_H
