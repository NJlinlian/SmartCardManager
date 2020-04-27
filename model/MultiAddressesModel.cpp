#include "MultiAddressesModel.h"

MultisigAddress::MultisigAddress(const int _required, const QStringList _pubKeys)
{
    required = _required;
    pubKeys = _pubKeys;
}


MultiAddressesModel::MultiAddressesModel()
{

}

