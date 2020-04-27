#ifndef BITCOINUTIL_H
#define BITCOINUTIL_H

#include <QDebug>

class MultisigInfo :  public QObject
{
    Q_OBJECT
    Q_PROPERTY(int          required    READ getRequired    CONSTANT)
    Q_PROPERTY(QStringList  pubKeys     READ getPubKeys     CONSTANT)
    Q_PROPERTY(QString      contractAddress     READ getContractAddress    WRITE setContractAddress)

public:
    int required = 0;
    QStringList pubKeys;        // BTC是公钥 ETH是地址
    // ETH
    QString contractAddress;

    MultisigInfo()
    {
    }

    MultisigInfo(const MultisigInfo & m)
    {
        required = m.required;
        pubKeys = m.pubKeys;
        contractAddress = m.contractAddress;
    }

    MultisigInfo& operator = (const MultisigInfo& m)
    {
        required = m.required;
        pubKeys = m.pubKeys;
        contractAddress = m.contractAddress;
        return *this;
    }

    Q_INVOKABLE int getRequired() const { return required;}
    Q_INVOKABLE QStringList getPubKeys() const { return pubKeys;}
    Q_INVOKABLE QString getContractAddress() const { return contractAddress;}
    Q_INVOKABLE void setContractAddress(const QString& _contractAddress) { contractAddress = _contractAddress;}
};
Q_DECLARE_METATYPE(MultisigInfo)

namespace  BitcoinUtil{

    QString pubkeyToAddress(QString _pubkey);
    QString makePubkeyScript(QString _pubkey);
    QString multisigScriptToAddress(QByteArray scriptData, const QString& asset);

    std::string get_address_by_pubkey(const std::string& pubkey_hex_str, uint8_t version);

    QByteArray serializeScriptUnit(int unit);
    QByteArray serializeScriptUnit(QByteArray unit);
    QVector<QByteArray> deserializeScript(QByteArray script);
    QByteArray makeMultisigRedeemScript( QStringList pubKeyList, int k);
    QByteArray makeMultisigRedeemScript( const MultisigInfo& info);
    QByteArray makeMultisigSignatureScript(QStringList signatureHexList, QByteArray redeemScript, bool fillZero);
    int validMultisigRedeemScriptStr(QString scriptStr);
    MultisigInfo decodeMultisigRedeemScript(const QString &scriptHex);

    QString decoderawtransaction(const QString& trx,uint8_t kh,uint8_t sh);
    QString addScriptToVin(const QString &script, const QString &rawTrx, int index);
    QString addSignatureToBtcRawTrx(QByteArray signature, QByteArray pubKey, QString rawTrx, int index);
    QString clearAllVinScript(const QString &rawTrx);

    QString btcTrxHash(QString rawTrx, QString hashCode);

    // for qml
    class BitcoinUtilObject: public QObject
    {
        Q_OBJECT
        Q_PROPERTY(MultisigInfo* multisigInfo    READ getMultisigInfo)

//        ~BitcoinUtilObject();

    public:
        Q_INVOKABLE QString signRawTrx(const QString& asset, const QString& rawTrx);
        Q_INVOKABLE QString addBtcMultisigAddress(const QStringList& pubkeysList, int required);
        Q_INVOKABLE void deleteBtcMultisigAddress(const QString& multisigAddress);
        Q_INVOKABLE void parseMultisigInfoFromTrx(const QString& rawTrx);
        Q_INVOKABLE QString signMultisigTrx(const QString& asset, const QString& rawTrx);

        MultisigInfo* getMultisigInfo();
        MultisigInfo* info = nullptr;
    };

}


#endif // BITCOINUTIL_H
