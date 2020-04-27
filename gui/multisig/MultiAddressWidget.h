#ifndef MULTIADDRESSWIDGET_H
#define MULTIADDRESSWIDGET_H

#include <QWidget>
#include "GUIData.h"
#include "common/HttpManager.h"

namespace Ui {
class MultiAddressWidget;
}


class MultiAddressWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MultiAddressWidget(QWidget *parent = nullptr);
    ~MultiAddressWidget();

private slots:
    void httpReplied(QByteArray _data, int _status);

    void on_addBtn_clicked();

    void on_tableWidget_cellClicked(int row, int column);

private:
    QTimer timerForEthMultisigContract;
private slots:
    void checkEthMultisigContractState();

private:
    Ui::MultiAddressWidget *ui;
    HttpManager httpManager;
    QMap<QString,AddressInfoFromServer>     addressInfoMap;


    void showMultisigs();
    void deleteMultisig(QString multisigAddress);
};

#endif // MULTIADDRESSWIDGET_H
