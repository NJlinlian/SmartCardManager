#ifndef TRANSFERBTCDIALOG_H
#define TRANSFERBTCDIALOG_H

#include <QDialog>
#include "common/HttpManager.h"

namespace Ui {
class TransferBtcDialog;
}

class TransferBtcDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TransferBtcDialog(QWidget *parent = nullptr);
    ~TransferBtcDialog();

    QString fromAddress;
    double balance = 0;

private slots:
    void httpReplied(QByteArray _data, int _status);

    void on_addAddressBtn_clicked();

    void on_tableWidget_cellClicked(int row, int column);

    void on_createTrxBtn_clicked();

private:
    Ui::TransferBtcDialog *ui;
    HttpManager httpManager;

};

#endif // TRANSFERBTCDIALOG_H
