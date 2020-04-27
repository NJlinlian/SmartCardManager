#ifndef ADDPUBKEYDIALOG_H
#define ADDPUBKEYDIALOG_H

#include <QDialog>

namespace Ui {
class AddPubKeyDialog;
}

class AddPubKeyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddPubKeyDialog(QWidget *parent = nullptr);
    ~AddPubKeyDialog();

    QString pubKey;

private slots:
    void on_okBtn_clicked();

    void on_cancelBtn_clicked();

private:
    Ui::AddPubKeyDialog *ui;
};

#endif // ADDPUBKEYDIALOG_H
