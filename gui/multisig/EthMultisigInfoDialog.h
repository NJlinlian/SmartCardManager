#ifndef ETHMULTISIGINFODIALOG_H
#define ETHMULTISIGINFODIALOG_H

#include <QDialog>
#include "common/HttpManager.h"

namespace Ui {
class EthMultisigInfoDialog;
}

class EthMultisigInfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EthMultisigInfoDialog(QString _contractAddress, QWidget *parent = nullptr);
    ~EthMultisigInfoDialog();

private slots:
    void httpReplied(QByteArray _data, int _status);

    void on_okBtn_clicked();

    void queryConfirmations(int _index);

    void on_tableWidget_cellClicked(int row, int column);

private:
    Ui::EthMultisigInfoDialog *ui;
    HttpManager httpManager;

    int trxCount = 0;
};

#endif // ETHMULTISIGINFODIALOG_H
