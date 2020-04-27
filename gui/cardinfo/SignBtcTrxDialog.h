#ifndef SIGNBTCTRXDIALOG_H
#define SIGNBTCTRXDIALOG_H

#include <QDialog>

namespace Ui {
class SignBtcTrxDialog;
}

class SignBtcTrxDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SignBtcTrxDialog(QWidget *parent = nullptr);
    ~SignBtcTrxDialog();

    QString rawTrx;
    void setRawTrx(QString _rt);

    QString coin;
    void setCoinType(QString _coin) { coin = _coin;}
    QVector<unsigned long long> vinAmounts;
    void setVinAmounts(QVector<unsigned long long> vector) { vinAmounts = vector;}

private slots:
    void on_signBtn_clicked();

private:
    Ui::SignBtcTrxDialog *ui;
};

#endif // SIGNBTCTRXDIALOG_H
