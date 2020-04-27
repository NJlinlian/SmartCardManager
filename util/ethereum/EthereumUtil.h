#ifndef ETHEREUMUTIL_H
#define ETHEREUMUTIL_H

#include <QDebug>

namespace  EthereumUtil{
    QString pubkeyToAddress(QString _pubkey);

    QString transfer(QString toAddress, QString amount, QString gasLimit, QString gasPrice, QString data, QString nonce);

    QString ethSign(QString data);

    std::string calculateMethodId(QString _method);
    QString createMultisigContractCode(int requireCount, QStringList addressList);
    QString createContractWithdrawData(QString toAddress, QString amount);
    QString createContractTrxCountData();       // 生成调用transactionCount()的data
    QString createContractTrxsData(int _index);  // 生成调用transactions(uint256)的data
    QString createGetConfirmationsData(int _index); // 生成调用getConfirmations(uint256)的data
    QString createConfirmTrxData(int _index);       // 生成调用 confirmTransaction(uint256)的data
}



#endif // ETHEREUMUTIL_H
