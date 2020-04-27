#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class CardInfoWidget;
class MultiAddressWidget;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_cardInfoBtn_clicked();

    void on_multisigBtn_clicked();

    void on_assetComboBox_currentIndexChanged(const QString &arg1);

private:
    Ui::MainWindow *ui;

    int currentPage = 0;        // 0: cardInfoWidget  1: multiAddressWidget
    CardInfoWidget* cardInfoWidget = nullptr;
    MultiAddressWidget* multiAddressWidget = nullptr;
    void setCurrentPage(int _page);
    void closeCurrentPage();

};

#endif // MAINWINDOW_H
