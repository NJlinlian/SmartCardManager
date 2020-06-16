#include "EthereumUtil.h"
#include <libethcore/Common.h>
#include <libethcore/TransactionBase.h>
#include <libethcore/CommonJS.h>
#include <libdevcrypto/DevCryptoCommon.h>
#include <libdevcore/DevCoreCommon.h>
//#include <libdevcore/CommonData.h>


#include "Keccak.hpp"
#include "util/SSLUtils.h"
#include "card/CSmartCard.h"
#include "GUIData.h"

QString EthereumUtil::pubkeyToAddress(QString _pubkey)
{
    if(_pubkey.isEmpty())   return "";
    QByteArray data = QByteArray::fromHex(_pubkey.toUtf8());
    char* ch = data.data();
    Keccak tmp_addr;
    tmp_addr.add(ch + 1, data.size() - 1);
    std::string addr_str_keccaksha3 = tmp_addr.getHash();
    std::string hex_str = addr_str_keccaksha3.substr(24, addr_str_keccaksha3.size());
    return  "0x" + QString::fromStdString(hex_str);
}

QString EthereumUtil::transfer(QString toAddress, QString amount, QString gasLimit, QString gasPrice, QString data, QString nonce)
{
    dev::eth::TransactionSkeleton ret;
//    ret.from = dev::jsToAddress("0x2b19fc37698adf40711c5595450eea8f05b0e86e");
    if(!toAddress.isEmpty())
    {
        ret.to = dev::jsToAddress(toAddress.toStdString());
        ret.creation = false;
    }
    else
    {
        ret.creation = true;
    }
qDebug() << "EthereumUtil::transfer" << toAddress << amount << gasLimit << gasPrice << data << nonce;
    ret.value = dev::jsToU256(amount.toStdString());
    ret.gas = dev::jsToU256(gasLimit.toStdString());
    ret.gasPrice = dev::jsToU256(gasPrice.toStdString());//1GWei
    ret.data = dev::jsToBytes(data.toStdString());
    ret.nonce = dev::jsToU256(nonce.toStdString());

    dev::eth::TransactionBase trx_base(ret);
    dev::h256 without_sign = trx_base.sha3(dev::eth::IncludeSignature::WithoutSignature);
    qDebug() << "EthereumUtil::transfer" << QString::fromStdString( without_sign.hex());

    QString sigHex = ethSign(QString::fromStdString( without_sign.hex()));
    qDebug() << "eth signature" << sigHex;
    if(sigHex.isEmpty())    return "";
    dev::h520 sig(sigHex.toStdString());
    dev::SignatureStruct sigStruct = *(dev::SignatureStruct const*)&sig;

    if (sigStruct.isValid())
        trx_base.m_vrs = sigStruct;
    auto ret_data = trx_base.rlp();
    std::string retstr = dev::toHex(ret_data);
    return QString::fromStdString( retstr);
}

QString EthereumUtil::ethSign(QString data)
{
    QByteArray pubKeyHex = GUIData::getInstance()->scView->getUncompressedPubKey();
    pubKeyHex = pubKeyHex.mid(2);   // 本来需要去掉第一个字节，转成了hex形式则需要去掉头两位
    dev::h512 pub(pubKeyHex.toStdString());
    bool success = false;
    int sigLimit = 5; // 据说eth签名有概率，失败了就多签几次
    while(sigLimit > 0)
    {
        sigLimit--;
        int rt = GUIData::getInstance()->scView->signTrxHash(data);
        qDebug() << "EthereumUtil::ethSign rt: " << rt;
        if(rt <= 0)     return "";
        QPair<QString,QString> pair = SSLUtils::decodeDerSig(GUIData::getInstance()->scView->getSignedData().toHex());
        for(int i : {1,0})      // 感觉1的成功率高?，放前面，也有可能是错觉
        {
            QString rsv = pair.first + pair.second + ((i == 0)? "00" : "01");
            dev::h520 sig(rsv.toStdString());
            dev::h256 toSignData(data.toStdString());
            qDebug() << i;
            qDebug() << "pubKeyHex:" << pubKeyHex;
            qDebug() << rsv;
            qDebug() << data;
            success = dev::verify(pub, sig, toSignData);
            if(success) return QString::fromStdString(sig.hex());
        }
    }
    return "";
}

QString EthereumUtil::createMultisigContractCode(int requireCount, QStringList addressList)
{
    QString contractCode = "0x60806040523480156200001157600080fd5b50604051620021ff380380620021ff83398101806040528101908080519060200190929190805182019291905050506000828251600681111515156200005657600080fd5b600082118015620000675750808211155b15156200007357600080fd5b600092505b8351831015620001ae576002600085858151811015156200009557fe5b9060200190602002015173ffffffffffffffffffffffffffffffffffffffff1673ffffffffffffffffffffffffffffffffffffffff16815260200190815260200160002060009054906101000a900460ff16158015620001245750600084848151811015156200010157fe5b9060200190602002015173ffffffffffffffffffffffffffffffffffffffff1614155b15156200013057600080fd5b60016002600086868151811015156200014557fe5b9060200190602002015173ffffffffffffffffffffffffffffffffffffffff1673ffffffffffffffffffffffffffffffffffffffff16815260200190815260200160002060006101000a81548160ff021916908315150217905550828060010193505062000078565b8360049080519060200190620001c6929190620002f9565b508460058190555060016003600060405180807f4554480000000000000000000000000000000000000000000000000000000000815250600301905060405180910390206000191660001916815260200190815260200160002060006101000a81548173ffffffffffffffffffffffffffffffffffffffff021916908373ffffffffffffffffffffffffffffffffffffffff16021790555060016003600060405180807f6574680000000000000000000000000000000000000000000000000000000000815250600301905060405180910390206000191660001916815260200190815260200160002060006101000a81548173ffffffffffffffffffffffffffffffffffffffff021916908373ffffffffffffffffffffffffffffffffffffffff1602179055505050505050620003ce565b82805482825590600052602060002090810192821562000375579160200282015b82811115620003745782518260006101000a81548173ffffffffffffffffffffffffffffffffffffffff021916908373ffffffffffffffffffffffffffffffffffffffff160217905550916020019190600101906200031a565b5b50905062000384919062000388565b5090565b620003cb91905b80821115620003c757600081816101000a81549073ffffffffffffffffffffffffffffffffffffffff0219169055506001016200038f565b5090565b90565b611e2180620003de6000396000f300608060405260043610610128576000357c0100000000000000000000000000000000000000000000000000000000900463ffffffff168063025e7c271461018257806320ea8d86146101ef5780632f54bf6e1461021c5780633411c81c1461027757806342159723146102dc5780635474152514610307578063784547a7146103565780637d6ca7451461039b5780638b51d13f146104425780639ace38c214610483578063a0e67e2b14610511578063a8abe69a1461057d578063b5dc40c314610621578063b77bf600146106a3578063c01a8c84146106ce578063c444b202146106fb578063d74f8edd1461076c578063dbe3a38114610797578063dc8452cd146107ca578063e3a15eb1146107f5578063ee22610b14610828578063fa3c4b6014610855575b6000341115610180573373ffffffffffffffffffffffffffffffffffffffff167fe1fffcc4923d04b559f4d29a8bfc6cda04eb5b0d3c460751c2402c5c5cc9109c346040518082815260200191505060405180910390a25b005b34801561018e57600080fd5b506101ad600480360381019080803590602001909291905050506108de565b604051808273ffffffffffffffffffffffffffffffffffffffff1673ffffffffffffffffffffffffffffffffffffffff16815260200191505060405180910390f35b3480156101fb57600080fd5b5061021a6004803603810190808035906020019092919050505061091c565b005b34801561022857600080fd5b5061025d600480360381019080803573ffffffffffffffffffffffffffffffffffffffff169060200190929190505050610ac4565b604051808215151515815260200191505060405180910390f35b34801561028357600080fd5b506102c260048036038101908080359060200190929190803573ffffffffffffffffffffffffffffffffffffffff169060200190929190505050610ae4565b604051808215151515815260200191505060405180910390f35b3480156102e857600080fd5b506102f1610b13565b6040518082815260200191505060405180910390f35b34801561031357600080fd5b50610340600480360381019080803515159060200190929190803515159060200190929190505050610b18565b6040518082815260200191505060405180910390f35b34801561036257600080fd5b5061038160048036038101908080359060200190929190505050610baa565b604051808215151515815260200191505060405180910390f35b3480156103a757600080fd5b5061042c600480360381019080803590602001908201803590602001908080601f0160208091040260200160405190810160405280939291908181526020018383808284378201915050505050509192919290803573ffffffffffffffffffffffffffffffffffffffff16906020019092919080359060200190929190505050610c8f565b6040518082815260200191505060405180910390f35b34801561044e57600080fd5b5061046d60048036038101908080359060200190929190505050610cae565b6040518082815260"
                           "200191505060405180910390f35b34801561048f57600080fd5b506104ae60048036038101908080359060200190929190505050610d79565b6040518085600019166000191681526020018473ffffffffffffffffffffffffffffffffffffffff1673ffffffffffffffffffffffffffffffffffffffff1681526020018381526020018215151515815260200194505050505060405180910390f35b34801561051d57600080fd5b50610526610dd6565b6040518080602001828103825283818151815260200191508051906020019060200280838360005b8381101561056957808201518184015260208101905061054e565b505050509050019250505060405180910390f35b34801561058957600080fd5b506105ca6004803603810190808035906020019092919080359060200190929190803515159060200190929190803515159060200190929190505050610e64565b6040518080602001828103825283818151815260200191508051906020019060200280838360005b8381101561060d5780820151818401526020810190506105f2565b505050509050019250505060405180910390f35b34801561062d57600080fd5b5061064c60048036038101908080359060200190929190505050610fd5565b6040518080602001828103825283818151815260200191508051906020019060200280838360005b8381101561068f578082015181840152602081019050610674565b505050509050019250505060405180910390f35b3480156106af57600080fd5b506106b8611212565b6040518082815260200191505060405180910390f35b3480156106da57600080fd5b506106f960048036038101908080359060200190929190505050611218565b005b34801561070757600080fd5b5061072a60048036038101908080356000191690602001909291905050506113f5565b604051808273ffffffffffffffffffffffffffffffffffffffff1673ffffffffffffffffffffffffffffffffffffffff16815260200191505060405180910390f35b34801561077857600080fd5b50610781611428565b6040518082815260200191505060405180910390f35b3480156107a357600080fd5b506107ac61142d565b60405180826000191660001916815260200191505060405180910390f35b3480156107d657600080fd5b506107df611466565b6040518082815260200191505060405180910390f35b34801561080157600080fd5b5061080a61146c565b60405180826000191660001916815260200191505060405180910390f35b34801561083457600080fd5b50610853600480360381019080803590602001909291905050506114a5565b005b34801561086157600080fd5b506108dc600480360381019080803590602001908201803590602001908080601f0160208091040260200160405190810160405280939291908181526020018383808284378201915050505050509192919290803573ffffffffffffffffffffffffffffffffffffffff169060200190929190505050611959565b005b6004818154811015156108ed57fe5b906000526020600020016000915054906101000a900473ffffffffffffffffffffffffffffffffffffffff1681565b33600260008273ffffffffffffffffffffffffffffffffffffffff1673ffffffffffffffffffffffffffffffffffffffff16815260200190815260200160002060009054906101000a900460ff16151561097557600080fd5b81336001600083815260200190815260200160002060008273ffffffffffffffffffffffffffffffffffffffff1673ffffffffffffffffffffffffffffffffffffffff16815260200190815260200160002060009054906101000a900460ff1615156109e057600080fd5b8360008082815260200190815260200160002060030160009054906101000a900460ff16151515610a1057600080fd5b60006001600087815260200190815260200160002060003373ffffffffffffffffffffffffffffffffffffffff1673ffffffffffffffffffffffffffffffffffffffff16815260200190815260200160002060006101000a81548160ff021916908315150217905550843373ffffffffffffffffffffffffffffffffffffffff167ff6a317157440607f36269043eb55f1287a5a19ba2216afeab88cd46cbcfb88e960405160405180910390a35050505050565b60026020528060005260406000206000915054906101000a900460ff1681565b60016020528160005260406000206020528060005260406000206000915091509054906101000a900460ff1681565b600481565b600080600090505b600654811015610ba357838015610b57575060008082815260200190815260200160002060030160009054906101000a900460ff16155b80610b8a5750828015610b89575060008082815260200190815260200160002060030160009054906101000a900460ff165b5b15610b96576001820191505b8080600101915050610b20565b5092915050565b6000806000809150600090505b600480549050811015610c8757600160008581526020019081526020016000206000600483815481101515610be857fe5b9060005260206000200160009054906101000a900473ffffffffffffffffffffffffffffffffffffffff1673ffffffffffffffffffffffffffffffffffffffff1673ffffffffffffffffffffffffffffffffffffffff16815260200190815260200160002060009054906101000a900460ff1615610c67576001820191505b600554821415610c7a5760019250610c88565b8080600101915050610bb7565b5b5050919050565b6000610c9c848484611b85565b9050610ca781611218565b9392505050565b600080600090505b600480549050811015610d735760016000848152"
                           "6020019081526020016000206000600483815481101515610ce757fe5b9060005260206000200160009054906101000a900473ffffffffffffffffffffffffffffffffffffffff1673ffffffffffffffffffffffffffffffffffffffff1673ffffffffffffffffffffffffffffffffffffffff16815260200190815260200160002060009054906101000a900460ff1615610d66576001820191505b8080600101915050610cb6565b50919050565b60006020528060005260406000206000915090508060000154908060010160009054906101000a900473ffffffffffffffffffffffffffffffffffffffff16908060020154908060030160009054906101000a900460ff16905084565b60606004805480602002602001604051908101604052809291908181526020018280548015610e5a57602002820191906000526020600020905b8160009054906101000a900473ffffffffffffffffffffffffffffffffffffffff1673ffffffffffffffffffffffffffffffffffffffff1681526020019060010190808311610e10575b5050505050905090565b606080600080600654604051908082528060200260200182016040528015610e9b5781602001602082028038833980820191505090505b50925060009150600090505b600654811015610f4757858015610ede575060008082815260200190815260200160002060030160009054906101000a900460ff16155b80610f115750848015610f10575060008082815260200190815260200160002060030160009054906101000a900460ff165b5b15610f3a57808383815181101515610f2557fe5b90602001906020020181815250506001820191505b8080600101915050610ea7565b878703604051908082528060200260200182016040528015610f785781602001602082028038833980820191505090505b5093508790505b86811015610fca578281815181101515610f9557fe5b9060200190602002015184898303815181101515610faf57fe5b90602001906020020181815250508080600101915050610f7f565b505050949350505050565b60608060008060048054905060405190808252806020026020018201604052801561100f5781602001602082028038833980820191505090505b50925060009150600090505b60048054905081101561115c5760016000868152602001908152602001600020600060048381548110151561104c57fe5b9060005260206000200160009054906101000a900473ffffffffffffffffffffffffffffffffffffffff1673ffffffffffffffffffffffffffffffffffffffff1673ffffffffffffffffffffffffffffffffffffffff16815260200190815260200160002060009054906101000a900460ff161561114f576004818154811015156110d357fe5b9060005260206000200160009054906101000a900473ffffffffffffffffffffffffffffffffffffffff16838381518110151561110c57fe5b9060200190602002019073ffffffffffffffffffffffffffffffffffffffff16908173ffffffffffffffffffffffffffffffffffffffff16815250506001820191505b808060010191505061101b565b8160405190808252806020026020018201604052801561118b5781602001602082028038833980820191505090505b509350600090505b8181101561120a5782818151811015156111a957fe5b9060200190602002015184828151811015156111c157fe5b9060200190602002019073ffffffffffffffffffffffffffffffffffffffff16908173ffffffffffffffffffffffffffffffffffffffff16815250508080600101915050611193565b505050919050565b60065481565b33600260008273ffffffffffffffffffffffffffffffffffffffff1673ffffffffffffffffffffffffffffffffffffffff16815260200190815260200160002060009054906101000a900460ff16151561127157600080fd5b81600080600083815260200190815260200160002060010160009054906101000a900473ffffffffffffffffffffffffffffffffffffffff1673ffffffffffffffffffffffffffffffffffffffff16141515156112cd57600080fd5b82336001600083815260200190815260200160002060008273ffffffffffffffffffffffffffffffffffffffff1673ffffffffffffffffffffffffffffffffffffffff16815260200190815260200160002060009054906101000a900460ff1615151561133957600080fd5b600180600087815260200190815260200160002060003373ffffffffffffffffffffffffffffffffffffffff1673ffffffffffffffffffffffffffffffffffffffff16815260200190815260200160002060006101000a81548160ff021916908315150217905550843373ffffffffffffffffffffffffffffffffffffffff167f4a504a94899432a9846e1aa406dceb1bcfd538bb839071d49d1e5e23f5be30ef60405160405180910390a36113ee856114a5565b5050505050565b60036020528060005260406000206000915054906101000a900473ffffffffffffffffffffffffffffffffffffffff1681565b600681565b60405180807f45544800000000000000000000000000000000000000000000000000000000008152506003019050604051809103902081565b60055481565b60405180807f65746800000000000000000000000000000000000000000000000000000000008152506003019050604051809103902081565b600080600033600260008273ffffffffffffffffffffffffffffffffffffffff1673ffffffffffffffffffffffffffffffffffffffff16815260200190815260200160002060009054906101000a900460ff16151561150357600080fd5b84336001600083815260200190815260200160002060008273ffffff"
                           "ffffffffffffffffffffffffffffffffff1673ffffffffffffffffffffffffffffffffffffffff16815260200190815260200160002060009054906101000a900460ff16151561156e57600080fd5b8660008082815260200190815260200160002060030160009054906101000a900460ff1615151561159e57600080fd5b6115a788610baa565b1561194f57600080898152602001908152602001600020965060405180807f45544800000000000000000000000000000000000000000000000000000000008152506003019050604051809103902060001916876000015460001916148061164f575060405180807f65746800000000000000000000000000000000000000000000000000000000008152506003019050604051809103902060001916876000015460001916145b156117555760018760030160006101000a81548160ff0219169083151502179055508660010160009054906101000a900473ffffffffffffffffffffffffffffffffffffffff1673ffffffffffffffffffffffffffffffffffffffff166108fc88600201549081150290604051600060405180830381858888f193505050501561170557877f33e13ecb54c3076d8e8bb8c2881800a4d972b792045ffae98fdf46df365fed7560405160405180910390a2611750565b877f526441bb6c1aba3c9a4a6ca1d6545da9c2333c8c48343ef398eb858d72b7923660405160405180910390a260008760030160006101000a81548160ff0219169083151502179055505b61194e565b60018760030160006101000a81548160ff02191690831515021790555060405180807f7472616e7366657228616464726573732c75696e7432353629000000000000008152506019019050604051809103902095506003600088600001546000191660001916815260200190815260200160002060009054906101000a900473ffffffffffffffffffffffffffffffffffffffff1673ffffffffffffffffffffffffffffffffffffffff16867c010000000000000000000000000000000000000000000000000000000090048860010160009054906101000a900473ffffffffffffffffffffffffffffffffffffffff1689600201546040518363ffffffff167c0100000000000000000000000000000000000000000000000000000000028152600401808373ffffffffffffffffffffffffffffffffffffffff1673ffffffffffffffffffffffffffffffffffffffff168152602001828152602001925050506000604051808303816000875af19250505094508415156119205760008760030160006101000a81548160ff021916908315150217905550877f526441bb6c1aba3c9a4a6ca1d6545da9c2333c8c48343ef398eb858d72b7923660405160405180910390a2600080fd5b877f33e13ecb54c3076d8e8bb8c2881800a4d972b792045ffae98fdf46df365fed7560405160405180910390a25b5b5050505050505050565b600033600260008273ffffffffffffffffffffffffffffffffffffffff1673ffffffffffffffffffffffffffffffffffffffff16815260200190815260200160002060009054906101000a900460ff1615156119b457600080fd5b83600060036000836040518082805190602001908083835b6020831015156119f157805182526020820191506020810190506020830392506119cc565b6001836020036101000a03801982511681845116808217855250505050505090500191505060405180910390206000191660001916815260200190815260200160002060009054906101000a900473ffffffffffffffffffffffffffffffffffffffff1673ffffffffffffffffffffffffffffffffffffffff16141515611a7757600080fd5b846040518082805190602001908083835b602083101515611aad5780518252602082019150602081019050602083039250611a88565b6001836020036101000a038019825116818451168082178552505050505050905001915050604051809103902092508360036000856000191660001916815260200190815260200160002060006101000a81548173ffffffffffffffffffffffffffffffffffffffff021916908373ffffffffffffffffffffffffffffffffffffffff1602179055508373ffffffffffffffffffffffffffffffffffffffff1683600019167fc77413bc832a9e6b65e4449c868bf9fe22c388baf4a8893f419c0ccbdce75e2f60405160405180910390a35050505050565b60008260008173ffffffffffffffffffffffffffffffffffffffff1614151515611bae57600080fd5b84600060036000836040518082805190602001908083835b602083101515611beb5780518252602082019150602081019050602083039250611bc6565b6001836020036101000a03801982511681845116808217855250505050505090500191505060405180910390206000191660001916815260200190815260200160002060009054906101000a900473ffffffffffffffffffffffffffffffffffffffff1673ffffffffffffffffffffffffffffffffffffffff1614151515611c7257600080fd5b6006549250608060405190810160405280876040518082805190602001908083835b602083101515611cb95780518252602082019150602081019050602083039250611c94565b6001836020036101000a03801982511681845116808217855250505050505090500191505060405180910390206000191681526020018673ffffffffffffffffffffffffffffffffffffffff168152602001858152602001600015158152506000808581526020019081526020016000206000820151816000019060001916905560208201518160010160006101000a81548173ffffffffffffffffffffffffffffffffffffffff021916908373ffffffffffffffffffffffffff"
                           "ffffffffffffff1602179055506040820151816002015560608201518160030160006101000a81548160ff0219169083151502179055509050506001600660008282540192505081905550827fc0ba8fe4b176c1714197d43b9cc6bcf797a4a7461c5fe8d0ef6e184ae7601e5160405160405180910390a2505093925050505600a165627a7a72305820942d1cc34f95286bc75fe2e5afbe4b701da67191962ba0eb2de64757e10331b80029";

    std::vector<byte> params;
    std::vector<byte> require = dev::toBigEndian(dev::jsToU256(std::to_string(requireCount)));
    std::vector<byte> offset = dev::toBigEndian(dev::jsToU256(std::to_string(64)));
    std::vector<byte> addressCount = dev::toBigEndian(dev::jsToU256(std::to_string(addressList.size())));
    params.insert(params.end(), require.begin(),require.end());
    params.insert(params.end(), offset.begin(),offset.end());
    params.insert(params.end(), addressCount.begin(),addressCount.end());
    foreach (QString address, addressList) {
        std::vector<byte> addr = dev::toBigEndian(dev::jsToU256(address.toStdString()));
        params.insert(params.end(), addr.begin(),addr.end());
    }

    return contractCode + QString::fromStdString( dev::toHex(params));
}

std::vector<byte> strTo32Size(QString str)
{
    std::vector<byte> result;
    if(str.size() > 32) return result;
    QByteArray ba = str.toUtf8();
    int i = 0;
    for(; i < ba.size(); i++)
    {
        result.push_back( (unsigned char)(ba.at(i)));
    }

    while (i < 32) {
        result.push_back(0);
        i++;
    }

    return result;
}

std::string EthereumUtil::calculateMethodId(QString _method)
{
    std::string method = _method.toStdString();
    Keccak tmp_addr;
    tmp_addr.add(method.data(), method.size());
    std::string addr_str_keccaksha3 = tmp_addr.getHash();
    std::string methodId = addr_str_keccaksha3.substr(0,8);
    return methodId;
}


QString EthereumUtil::createContractWithdrawData(QString toAddress, QString amount)
{
    std::string methodId = calculateMethodId("submitTransaction(string,address,uint256)");

    std::vector<byte> params;
    std::vector<byte> offset = dev::toBigEndian(dev::jsToU256(std::to_string(96)));
    std::vector<byte> to = dev::toBigEndian(dev::jsToU256(toAddress.toStdString()));
    std::vector<byte> amountVector = dev::toBigEndian(dev::jsToU256(amount.toStdString()));
    std::vector<byte> symbolSize = dev::toBigEndian(dev::jsToU256(std::to_string(3)));
    std::vector<byte> assetSymbol = strTo32Size("ETH");

    params.insert(params.end(), offset.begin(),offset.end());
    params.insert(params.end(), to.begin(),to.end());
    params.insert(params.end(), amountVector.begin(),amountVector.end());
    params.insert(params.end(), symbolSize.begin(),symbolSize.end());
    params.insert(params.end(), assetSymbol.begin(),assetSymbol.end());

    return "0x" + QString::fromStdString(methodId) + QString::fromStdString( dev::toHex(params));
}

QString EthereumUtil::createContractTrxCountData()
{
    std::string methodId = calculateMethodId("transactionCount()");
    return "0x" + QString::fromStdString(methodId);
}

QString EthereumUtil::createContractTrxsData(int _index)
{
    std::string methodId = calculateMethodId("transactions(uint256)");
    std::vector<byte> params;
    std::vector<byte> index = dev::toBigEndian(dev::jsToU256(std::to_string(_index)));
    params.insert(params.end(), index.begin(),index.end());

    return "0x" + QString::fromStdString(methodId) + QString::fromStdString( dev::toHex(params));
}


QString EthereumUtil::createGetConfirmationsData(int _index)
{
    std::string methodId = calculateMethodId("getConfirmations(uint256)");
    std::vector<byte> params;
    std::vector<byte> index = dev::toBigEndian(dev::jsToU256(std::to_string(_index)));
    params.insert(params.end(), index.begin(),index.end());

    return "0x" + QString::fromStdString(methodId) + QString::fromStdString( dev::toHex(params));
}

QString EthereumUtil::createConfirmTrxData(int _index)
{
    std::string methodId = calculateMethodId("confirmTransaction(uint256)");
    std::vector<byte> params;
    std::vector<byte> index = dev::toBigEndian(dev::jsToU256(std::to_string(_index)));
    params.insert(params.end(), index.begin(),index.end());

    return "0x" + QString::fromStdString(methodId) + QString::fromStdString( dev::toHex(params));
}

QString EthereumUtil::createERC20BalanceOfData(QString address)
{
    std::string methodId = calculateMethodId("balanceOf(address)");
    std::vector<byte> params;
    std::vector<byte> addressVector = dev::toBigEndian(dev::jsToU256(address.toStdString()));
    params.insert(params.end(), addressVector.begin(), addressVector.end());

    return "0x" + QString::fromStdString(methodId) + QString::fromStdString( dev::toHex(params));
}

QString EthereumUtil::createERC20DecimalsData()
{
    std::string methodId = calculateMethodId("decimals()");
    return "0x" + QString::fromStdString(methodId);
}


QString EthereumUtil::createERC20TransferData(QString toAddress, QString amount)
{
    std::string methodId = calculateMethodId("transfer(address,uint256)");
    std::vector<byte> params;
    std::vector<byte> to = dev::toBigEndian(dev::jsToU256(toAddress.toStdString()));
    std::vector<byte> amountVector = dev::toBigEndian(dev::jsToU256(amount.toStdString()));
    params.insert(params.end(), to.begin(), to.end());
    params.insert(params.end(), amountVector.begin(), amountVector.end());

    return "0x" + QString::fromStdString(methodId) + QString::fromStdString( dev::toHex(params));
}


QString EthereumUtil::EthereumUtilObject::transfer(QString toAddress, QString amount, QString gasLimit, QString gasPrice, QString data, QString nonce)
{
    return EthereumUtil::transfer(toAddress, amount, gasLimit, gasPrice, data, nonce);
}

QString EthereumUtil::EthereumUtilObject::transferUSDT(QString toAddress, QString amount, QString gasLimit, QString gasPrice, QString nonce)
{
    QString data = createERC20TransferData(toAddress,amount);
    return transfer(GUIData::getInstance()->usdtContractAddress,
             "0", gasLimit, gasPrice, data, nonce);
}

