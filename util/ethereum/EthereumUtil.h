#ifndef ETHEREUMUTIL_H
#define ETHEREUMUTIL_H

#include <QDebug>

namespace  EthereumUtil{
    QString pubkeyToAddress(QString _pubkey);

    // ETH系的资产自己构造交易
    QString transfer(QString toAddress, QString amount, QString gasLimit, QString gasPrice, QString data, QString nonce);

    QString ethSign(QString data);

    std::string calculateMethodId(QString _method);
    // 多签合约相关
    QString createMultisigContractCode(int requireCount, QStringList addressList);
    QString createContractWithdrawData(QString toAddress, QString amount);
    QString createContractTrxCountData();       // 生成调用transactionCount()的data
    QString createContractTrxsData(int _index);  // 生成调用transactions(uint256)的data
    QString createGetConfirmationsData(int _index); // 生成调用getConfirmations(uint256)的data
    QString createConfirmTrxData(int _index);       // 生成调用 confirmTransaction(uint256)的data

    // ERC20相关
    QString createERC20BalanceOfData(QString address);
    QString createERC20DecimalsData();
    QString createERC20TransferData(QString toAddress, QString amount);      // 单位wei


    // for qml
    class EthereumUtilObject: public QObject
    {
        Q_OBJECT

    public:
        Q_INVOKABLE QString transfer(QString toAddress, QString amount, QString gasLimit, QString gasPrice, QString data, QString nonce);
        Q_INVOKABLE QString transferUSDT(QString toAddress, QString amount, QString gasLimit, QString gasPrice, QString nonce);


    };

}



#endif // ETHEREUMUTIL_H
