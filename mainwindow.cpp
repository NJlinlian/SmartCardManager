#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "util/bitcoin/base58.h"
#include "util/SSLUtils.h"
#include "GUIData.h"
#include "gui/cardinfo/CardInfoWidget.h"
#include "gui/multisig/MultiAddressWidget.h"
#include "util/ethereum/EthereumUtil.h"
#include "util/bitcoin/BCHUtil.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle(QString::fromLocal8Bit("数银卡桌面程序"));
    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);

    GUIData::getInstance();

    setCurrentPage(0);

    ui->assetComboBox->addItem("BTC");
    ui->assetComboBox->addItem("LTC");
    ui->assetComboBox->addItem("ETH");
    ui->assetComboBox->addItem("BCH");


//    qDebug() << "sssssssssss "  << BitcoinUtil::btcTrxHash("02000000021ce714af56476943fd8230f9d1d49a18cd39157af36d7fcf83aeecc832e2d70400000000695221028e5fce316189786f8fe473fb54a7c411ae8ef1c4f51e76649189b00e0e99a20e21022edf425a960ec0c134251637031d7ce4f8ba7ce565c1e759a5f86bebb2bf55e921025b93a38f78158dd4c7387ed8532f08c403add54014c7d5d8a06a1416428c609b53aef0ffffff5685ef82c713f4a933db55eb76fd62b80315fd28c44efe3463435f72f6fcf3c30000000000f0ffffff02001bb700000000001976a914019ddcb33f0661a9e109f646facb6ecdae49c20d88ac60dc2c000000000017a91423c1405725486db1ec1e050cc54e8d5c30a660f38700000000","01000000");
//    qDebug() << "aaaaaaa "  << EncodeBase58( "ABD");
//    qDebug() << "bbbbbbb "  << DecodeBase58("NvM1");

//    qDebug() << "bchhhhhhhhhh " << BCHUtil::btcAddrToBch("1Piys7qU1oitAcXom8XuuvxbBE6cCb7kis");
//qDebug() << "nnnnnnn " << convertPre(16,10, "0000000000000000000000000000000000000000000000000de0b6b3a7640000");
//    qDebug() <<"ccccccccc " << EthereumUtil::createMultisigContractCode(2, QStringList() << "0x4d0baeda4427792d630fa1635be635b9c8f93594" << "0x7f5211bbf256e11de4955ef65ec702df6e2456d5" << "0xb37317eb29614273609001b720fd3cbb95bdbfa0");

//    EthereumUtil::createTrxHash("0x4d0baeda4427792d630fa1635be635b9c8f93594","1000","21000","21000000000","","10000");

//qDebug() <<"Eeeeeeeeeee" << EthereumUtil::pubkeyToAddress("028e5fce316189786f8fe473fb54a7c411ae8ef1c4f51e76649189b00e0e99a20e");
//    MultisigInfo info = BitcoinUtil::decodeMultisigRedeemScript("5221028e5fce316189786f8fe473fb54a7c411ae8ef1c4f51e76649189b00e0e99a20e21022edf425a960ec0c134251637031d7ce4f8ba7ce565c1e759a5f86bebb2bf55e921025b93a38f78158dd4c7387ed8532f08c403add54014c7d5d8a06a1416428c609b53ae");
//      qDebug() << "0000000000 " << info.required << info.pubKeys;
      qDebug() << "11111111 " <<  BitcoinUtil::decoderawtransaction("020000000144089d00516a8d02e8c5e55d3805b9e06808726ab1c530ec5a7b37a7b4effa6201000000fdfd000048304502210094813a9a486cf6fbfead7ee1d232d359ee592713f947a4b3de48560b08de10b10220273ee875b4e56c015059bb3066596f41f6bfd81ee7c3ca22f280d55b9d5695ce4147304402205eadf286d9f4c78f423b7c447068334571a138dbb36c8fd2b1b858e837c074e5022026001a5188c18418c83426e37dc4a2e4a14a68b1997eb5ed264e672e1bb8db35414c69522102b323f81feb7eabed88953e1dadddd9705763427f29037f36dbf3208189f14db32102705d9676c7f854451ae3274c5333948fe885c4cc75de2694e0e352dc17c7d06d21025b93a38f78158dd4c7387ed8532f08c403add54014c7d5d8a06a1416428c609b53aef0ffffff0200e1f505000000001976a914af696292c2ae6e85c9959c1b414c306ef6a318c588ace092f5050000000017a914a17144cf369be7479e44468fefca8e1c0f804f7f8700000000",0,5);
//      qDebug() << "22222222 " <<  BitcoinUtil::decoderawtransaction("02000000021ce714af56476943fd8230f9d1d49a18cd39157af36d7fcf83aeecc832e2d70400000000695221028e5fce316189786f8fe473fb54a7c411ae8ef1c4f51e76649189b00e0e99a20e21022edf425a960ec0c134251637031d7ce4f8ba7ce565c1e759a5f86bebb2bf55e921025b93a38f78158dd4c7387ed8532f08c403add54014c7d5d8a06a1416428c609b53aef0ffffff5685ef82c713f4a933db55eb76fd62b80315fd28c44efe3463435f72f6fcf3c30000000000f0ffffff02001bb700000000001976a914019ddcb33f0661a9e109f646facb6ecdae49c20d88ac60dc2c000000000017a91423c1405725486db1ec1e050cc54e8d5c30a660f38700000000",0,5);
//          qDebug() << "33333333 " <<  BitcoinUtil::decoderawtransaction("02000000021ce714af56476943fd8230f9d1d49a18cd39157af36d7fcf83aeecc832e2d7040000000000f0ffffff5685ef82c713f4a933db55eb76fd62b80315fd28c44efe3463435f72f6fcf3c300000000695221028e5fce316189786f8fe473fb54a7c411ae8ef1c4f51e76649189b00e0e99a20e21022edf425a960ec0c134251637031d7ce4f8ba7ce565c1e759a5f86bebb2bf55e921025b93a38f78158dd4c7387ed8532f08c403add54014c7d5d8a06a1416428c609b53aef0ffffff02001bb700000000001976a914019ddcb33f0661a9e109f646facb6ecdae49c20d88ac60dc2c000000000017a91423c1405725486db1ec1e050cc54e8d5c30a660f38700000000",0,5);
//    qDebug() << "55555555 " << base58Decode("1E3YK3XuWqCjhmGGEGSWw8qgUBkANWdcvh");

//    qDebug() << "ssssssssss  " << base58Encode("8f149ab375a30e20207052a1a97c1850a9fc8f2b8727aa6a");
//    qDebug() <<  QString::fromStdString( SSLUtils::base58("8f149ab375a30e20207052a1a97c1850a9fc8f2b8727aa6a"));

//  qDebug() << "rrrrrrrr " << QString::fromStdString( BitcoinUtil::decoderawtransaction("0200000001dd94a131dc7b42f217789d3c3157fc759b8bab3a68c3dbe283a72bbed7563efc0000000000f0ffffff02e09aa435000000001976a9148f149ab375a30e20207052a1a97c1850a9fc8f2b88ac00e1f505000000001976a9149ee9b76d840d0c4fcb634ed88ec821cdfc0400e288ac00000000"
//                                      ,0,5));

//    qDebug() << "address " << BitcoinUtil::pubkeyToAddress("022edf425a960ec0c134251637031d7ce4f8ba7ce565c1e759a5f86bebb2bf55e9");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setCurrentPage(int _page)
{
    closeCurrentPage();

    currentPage = _page;
    switch (currentPage) {
    case 0:
        cardInfoWidget = new CardInfoWidget(ui->contentWidget);
        cardInfoWidget->show();
        break;
    case 1:
        multiAddressWidget = new MultiAddressWidget(ui->contentWidget);
        multiAddressWidget->show();
        break;
    default:
        break;
    }

}

void MainWindow::closeCurrentPage()
{
    switch (currentPage) {
    case 0:
        if(cardInfoWidget)
        {
            cardInfoWidget->close();
            cardInfoWidget = nullptr;
        }
        break;
    case 1:
        if(multiAddressWidget)
        {
            multiAddressWidget->close();
            multiAddressWidget = nullptr;
        }
        break;
    default:
        break;
    }
}

void MainWindow::on_cardInfoBtn_clicked()
{
    setCurrentPage(0);
}

void MainWindow::on_multisigBtn_clicked()
{
    setCurrentPage(1);
}

void MainWindow::on_assetComboBox_currentIndexChanged(const QString &arg1)
{
    GUIData::getInstance()->currentAsset = ui->assetComboBox->currentText();
    setCurrentPage(currentPage);
}
