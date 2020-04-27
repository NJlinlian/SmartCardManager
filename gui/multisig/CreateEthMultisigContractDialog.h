#ifndef CREATEETHMULTISIGCONTRACTDIALOG_H
#define CREATEETHMULTISIGCONTRACTDIALOG_H

#include <QDialog>

namespace Ui {
class CreateEthMultisigContractDialog;
}

class CreateEthMultisigContractDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateEthMultisigContractDialog(QWidget *parent = nullptr);
    ~CreateEthMultisigContractDialog();

    void showAddress();

private slots:
    void on_createBtn_clicked();

    void on_addAddressBtn_clicked();

    void on_tableWidget_cellClicked(int row, int column);

private:
    Ui::CreateEthMultisigContractDialog *ui;

    QStringList addressList;
};

#endif // CREATEETHMULTISIGCONTRACTDIALOG_H
