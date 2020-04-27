#ifndef MULTIADDRESSESMODEL_H
#define MULTIADDRESSESMODEL_H

#include <QObject>
#include <QQmlListProperty>

class MultisigAddress : QObject
{
    Q_OBJECT
    Q_PROPERTY(int          required    READ getRequired    CONSTANT)
    Q_PROPERTY(QStringList  pubKeys     READ getPubKeys     CONSTANT)
    Q_PROPERTY(QString      contractAddress     READ getContractAddress    WRITE setContractAddress)

public:
    MultisigAddress(const int _required, const QStringList _pubKeys);
    int getRequired() const { return required;}
    QStringList getPubKeys() const { return pubKeys;}
    QString getContractAddress() const { return contractAddress;}
    void setContractAddress(const QString& _contractAddress) { contractAddress = _contractAddress;}

private:
    int required = 0;
    QStringList pubKeys;
    QString contractAddress;
};


class MultiAddressesModel : QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<MultisigAddress> multisigAddresses      READ getMultisigAddresses)

public:
    MultiAddressesModel();
    QQmlListProperty<MultisigAddress> getMultisigAddresses() const { return multisigAddresses;}


private:
    QQmlListProperty<MultisigAddress> multisigAddresses;

};

#endif // MULTIADDRESSESMODEL_H
