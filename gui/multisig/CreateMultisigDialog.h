#ifndef CREATEMULTISIGDIALOG_H
#define CREATEMULTISIGDIALOG_H

#include <QDialog>

namespace Ui {
class CreateMultisigDialog;
}

class CreateMultisigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateMultisigDialog(QWidget *parent = nullptr);
    ~CreateMultisigDialog();

    void showPubKeys();

private slots:
    void on_addPubkeyBtn_clicked();

    void on_tableWidget_cellClicked(int row, int column);

    void on_createBtn_clicked();

private:
    Ui::CreateMultisigDialog *ui;

    QStringList pubKeys;
};

#endif // CREATEMULTISIGDIALOG_H
