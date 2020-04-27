#ifndef ETHCONTRACTTRANSFERDIALOG_H
#define ETHCONTRACTTRANSFERDIALOG_H

#include <QDialog>

namespace Ui {
class EthContractTransferDialog;
}

class EthContractTransferDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EthContractTransferDialog( QString _contractAddress, QWidget *parent = nullptr);
    ~EthContractTransferDialog();

private slots:
    void on_transferBtn_clicked();

private:
    Ui::EthContractTransferDialog *ui;
    QString contractAddress;
};

#endif // ETHCONTRACTTRANSFERDIALOG_H
