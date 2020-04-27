#ifndef SIGNMULTISIGTRXDIALOG_H
#define SIGNMULTISIGTRXDIALOG_H

#include <QDialog>

namespace Ui {
class SignMultisigTrxDialog;
}

class SignMultisigTrxDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SignMultisigTrxDialog(QWidget *parent = nullptr);
    ~SignMultisigTrxDialog();

    QString coin;
    void setCoinType(QString _coin) { coin = _coin;}

private slots:
    void on_trxTextEdit_textChanged();

    void on_signBtn_clicked();

private:
    Ui::SignMultisigTrxDialog *ui;

    int required = 0;
    int pubKeyCount = 0;
};

#endif // SIGNMULTISIGTRXDIALOG_H
