#ifndef GUIDATA_H
#define GUIDATA_H

#include <QMap>
#include <QVector>
#include <QDialog>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QProcess>
#include <QFile>
#include <QDir>
#include <QTableWidget>

#include "util/bitcoin/BitcoinUtil.h"
#include "util/ethereum/EthereumUtil.h"
#include "gui/style/style.h"
#include "card/CardView.h"
#include <QQmlListProperty>

class CSmartCard;


typedef QMap<QString,MultisigInfo> MultisigInfoMap;
QString findContractAddressInMap(const MultisigInfoMap& map, QString _contractAddress);     // 返回key(创建合约的交易id)
extern QVector<QString> ERCAssetsName;

struct AddressInfoFromServer
{
    QString address;
//    unsigned long long received = 0;
//    unsigned long long sent = 0;
    unsigned long long balance = 0;
//    int tx_count = 0;
//    int unconfirmed_tx_count = 0;
};


class GUIData : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString btcUrl                 READ getBtcUrl              NOTIFY btcUrlChanged  )
    Q_PROPERTY(QString currentAsset           READ getCurrentAsset        NOTIFY currentAssetChanged  WRITE setCurrentAsset)
    Q_PROPERTY(QQmlListProperty<MultisigInfo> multisigAddresses           READ readMultisigAddresses  NOTIFY multisigAddressChanged)

public:
    ~GUIData();
    static GUIData*   getInstance();

    CSmartCard* sc = nullptr;
    CardView* scView = nullptr;

    QString guiDataPath;
    MultisigInfoMap readMultisigInfo(const QString& asset);
    void writeMultisigInfo(const MultisigInfoMap& map, const QString& asset);

    QString btcUrl;
    QString getBtcUrl() const { return btcUrl;}
    QString middlewareUrl;
    QString ethWalletUrl;
    QString usdtContractAddress;

    QQmlListProperty<MultisigInfo> readMultisigAddresses();
//    Q_INVOKABLE QVariantList readMultisigAddresses();
    Q_INVOKABLE QVariantMap readTest();
    QList<MultisigInfo*> mi_list;

signals:
    void multisigAddressChanged();

public:
    QString currentAsset = "BTC";
    QString getCurrentAsset() const { return currentAsset;}

    unsigned char getCurrentAssetAddressVersion();

    Q_INVOKABLE QString getCurrentAddress();
    Q_INVOKABLE bool isMultisigEnable(const QString& asset);

    Q_INVOKABLE QString decimalToIntegerStr(QString number, int precision);
    Q_INVOKABLE QString intStrSetPrecision(QString amount, int precision);
    Q_INVOKABLE QString numberStrConvertBase(int old_base, int new_base, QString sourceStr);

signals:
    void btcUrlChanged();
    void currentAssetChanged(QString);

public slots:
    void setCurrentAsset(const QString& _asset);

private:
    GUIData();
    static GUIData* goo;

    void initSystemEnvironmentPath();

    class CGarbo
    {
    public:
        ~CGarbo()
        {
            if (GUIData::goo)
                delete GUIData::goo;
        }
    };

};

unsigned long long jsonValueToULL(QJsonValue v);
QString removeLastZeros(QString number);        // qstring::number() 对小数的处理有问题  使用std::to_string() 然后把后面的0去掉
QString getBigNumberString(unsigned long long number,int precision);
void tableWidgetSetItemStyle(QTableWidget* w, int alignment = Qt::AlignCenter);
QString DecimalToIntegerStr(QString number, int precision);
QString convertPre(int old_base, int new_base, QString sourceStr);
QString amountSetPrecision(QString amount, int precision);

#endif // GUIDATA_H
