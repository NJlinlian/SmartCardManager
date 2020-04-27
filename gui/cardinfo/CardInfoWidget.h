#ifndef CARDINFOWIDGET_H
#define CARDINFOWIDGET_H

#include <QWidget>
#include <QTimer>

#include "common/HttpManager.h"

namespace Ui {
class CardInfoWidget;
}

class CardInfoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CardInfoWidget(QWidget *parent = nullptr);
    ~CardInfoWidget();

private slots:
    void httpReplied(QByteArray _data, int _status);

    void onTimeout();

    void on_transferBtn_clicked();

    void on_multisigSignBtn_clicked();

private:
    Ui::CardInfoWidget *ui;
    QTimer timer;
    HttpManager httpManager;

};

#endif // CARDINFOWIDGET_H
