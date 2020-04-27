#include "BCHUtil.h"
#include "base58.h"
#include "cashaddr.h"
#include "util/SSLUtils.h"


const std::string bch_prefix = "bitcoincash";
QString BCHUtil::btcAddrToBch(const QString &btcAddr, int btc_pubKey, int btc_script)
{
    return QString::fromStdString(pts_address_bch(pts_address(btcAddr.toStdString()), btc_pubKey, btc_script));
}

BCHUtil::pts_address::pts_address()
{
//    addr.resize(25);
}

BCHUtil::pts_address::pts_address(const std::string &base58str)
{
    addr = DecodeBase58(QString::fromStdString(base58str));
//    qDebug() << addr.size() << addr.toHex();
    if(addr.size() > 0)
    {
    }

}

bool BCHUtil::pts_address::is_valid() const
{
    return true;
}

BCHUtil::pts_address::operator std::string() const
{
    return EncodeBase58(addr).toStdString();
}

BCHUtil::pts_address_bch::pts_address_bch()
{
//    addr.resize(42);
}

BCHUtil::pts_address_bch::pts_address_bch(const BCHUtil::pts_address &pts_addr, uint8_t kh, uint8_t sh)
{
    QByteArray ba = pts_addr.addr.mid(1,20);
    std::string addr_str_temp(ba.constData(), ba.size());
    uint8_t addr_type = 0;
    if (pts_addr.version() == kh){
        addr_type = 0;
    }
    else if(pts_addr.version() == sh){
        addr_type = 1;
    }
    std::vector<uint8_t> packed_addr = PackAddrData(addr_str_temp, addr_type);
    cashaddr::data checksum = cashaddr::CreateChecksum(bch_prefix, packed_addr);
    cashaddr::data combined = cashaddr::Cat(packed_addr, checksum);
    std::string addWithCheck(combined.begin(), combined.end());
    addr = QByteArray(addWithCheck.c_str(), addWithCheck.length());
}

BCHUtil::pts_address_bch::operator std::string() const
{
    std::string prefix = bch_prefix;
    std::string ret = prefix + ':';
    std::vector<unsigned char> combined(addr.begin(), addr.end());
    ret.reserve(ret.size() + combined.size());
    for (uint8_t c : combined) {
        ret += cashaddr::GET_CHARSET(c);
    }
    return ret;
}

static libbitcoin::chain::script strip_code_seperators(const libbitcoin::chain::script& script_code)
{
    libbitcoin::machine::operation::list ops;

    for (auto op = script_code.begin(); op != script_code.end(); ++op)
        if (op->code() != libbitcoin::machine::opcode::codeseparator)
            ops.push_back(*op);

    return libbitcoin::chain::script(std::move(ops));
}

QByteArray BCHUtil::createToSignData( const std::string& redeemscript_hex, const std::string& raw_trx, int vin_index,const uint64_t& value)
{
    libbitcoin::chain::script   libbitcoin_script;
    libbitcoin_script.from_data(libbitcoin::config::base16(redeemscript_hex), false);
    const auto stripped = strip_code_seperators(libbitcoin_script);
    libbitcoin::chain::transaction  trx;
    trx.from_data(libbitcoin::config::base16(raw_trx));
    BITCOIN_ASSERT(vin_index < trx.inputs().size());
    const auto& input = trx.inputs()[vin_index];
    //const auto size = libbitcoin::chain::preimage_size(script_code.serialized_size(true));
    uint32_t sighash_type = libbitcoin::machine::sighash_algorithm::all | 0x40;
    libbitcoin::data_chunk data;
    //data.reserve(size);
    libbitcoin::data_sink ostream(data);
    libbitcoin::ostream_writer sink(ostream);

    // Flags derived from the signature hash byte.
    //const auto sighash = to_sighash_enum(sighash_type);
    const auto any = (sighash_type & libbitcoin::machine::sighash_algorithm::anyone_can_pay) != 0;
    const auto single = (sighash_type & libbitcoin::machine::sighash_algorithm::single) != 0;
    const auto none = (sighash_type & libbitcoin::machine::sighash_algorithm::none) != 0;
    const auto all = (sighash_type & libbitcoin::machine::sighash_algorithm::all) != 0;

    // 1. transaction version (4-byte little endian).
    sink.write_little_endian(trx.version());

    // 2. inpoints hash (32-byte hash).
    sink.write_hash(!any ? trx.inpoints_hash() : libbitcoin::null_hash);

    // 3. sequences hash (32-byte hash).
    sink.write_hash(!any && all ? trx.sequences_hash() : libbitcoin::null_hash);

    // 4. outpoint (32-byte hash + 4-byte little endian).
    input.previous_output().to_data(sink);

    // 5. script of the input (with prefix).
    stripped.to_data(sink, true);
    //uint64_t value = 500000000;
    // 6. value of the output spent by this input (8-byte little endian).
    sink.write_little_endian(value);

    // 7. sequence of the input (4-byte little endian).
    sink.write_little_endian(input.sequence());

    // 8. outputs hash (32-byte hash).
    sink.write_hash(all ? trx.outputs_hash() :
                          (single && vin_index < trx.outputs().size() ?
                               libbitcoin::bitcoin_hash(trx.outputs()[vin_index].to_data()) : libbitcoin::null_hash));

    // 9. transaction locktime (4-byte little endian).
    sink.write_little_endian(trx.locktime());

    // 10. sighash type of the signature (4-byte [not 1] little endian).
    sink.write_4_bytes_little_endian(sighash_type);

    ostream.flush();

    QByteArray ba;
    for(uint8_t v : data)
    {
        ba.append(char(v));
    }
    QByteArray ba1 = SSLUtils::sha256(ba);

    return ba1.toHex();

//    libbitcoin::hash_digest higest = libbitcoin::bitcoin_hash(data);
//    return higest;
    /*
        uint32_t index = vin_index;

        libbitcoin::chain::input::list ins;
        const auto& inputs = trx.inputs();
        std::cout <<"sig hash type is" << sighash_type << std::endl;
        const auto any = (sighash_type & libbitcoin::machine::sighash_algorithm::anyone_can_pay) != 0;
        ins.reserve(any ? 1 : inputs.size());

        BITCOIN_ASSERT(vin_index < inputs.size());
        const auto& self = inputs[vin_index];

        if (any)
        {
            // Retain only self.
            ins.emplace_back(self.previous_output(), stripped, self.sequence());
        }
        else
        {
            // Erase all input scripts.
            for (const auto& input : inputs)
                ins.emplace_back(input.previous_output(), libbitcoin::chain::script{},
                    input.sequence());

            // Replace self that is lost in the loop.
            ins[vin_index].set_script(stripped);
            ////ins[input_index].set_sequence(self.sequence());
        }

        // Move new inputs and copy outputs to new transaction.
        libbitcoin::chain::transaction out(trx.version(), trx.locktime(), libbitcoin::chain::input::list{}, trx.outputs());
        std::cout << "trx version is " << trx.version() << std::endl;
        out.set_inputs(std::move(ins));
        out.inpoints_hash();
        out.sequences_hash();
        auto temp = out.to_data(true, false);
        std::cout << "trx out put" << BinToHex(std::vector<char>(temp.begin(), temp.end()), false) << std::endl;

        //uint32_t newForkValue = 0xff0000 | (sighash_type ^ 0xdead);
        //auto fork_signhash_type = ((newForkValue << 8) | (sighash_type & 0xff));
        auto fork_signhash_type = sighash_type;
        libbitcoin::extend_data(serialized, libbitcoin::to_little_endian(fork_signhash_type));
        std::cout << BinToHex(std::vector<char>(serialized.begin(), serialized.end()), false) << std::endl;
        */
    //auto higest = libbitcoin::bitcoin_hash(serialized);
    //std::cout << BinToHex(std::vector<char>(higest.begin(), higest.end()), false) << std::endl;


//    libbitcoin::ec_signature signature;
//    libbitcoin::endorsement endorse;
//    if (!libbitcoin::sign_bch(signature, libbitcoin_priv.secret(), higest) || !libbitcoin::encode_signature(endorse, signature))
//        return "";
//    endorse.push_back(sighash_type);
//    endorse.shrink_to_fit();
//    return libbitcoin::encode_base16(endorse);
}

std::string BCHUtil::makeRedeemScript(const std::string &pubkey_hex_str)
{
    libbitcoin::wallet::ec_public libbitcoin_pub(pubkey_hex_str);
    libbitcoin::chain::script script = libbitcoin::chain::script::to_pay_key_hash_pattern(libbitcoin_pub.to_payment_address().hash());
    std::string prev = libbitcoin::encode_base16(script.to_data(false));
    return prev;
}

std::string BinToHex(const std::vector<char> &strBin, bool bIsUpper)
{
    std::string strHex;
    strHex.resize(strBin.size() * 2);
    for (size_t i = 0; i < strBin.size(); i++)
    {
        uint8_t cTemp = strBin[i];
        for (size_t j = 0; j < 2; j++)
        {
            uint8_t cCur = (cTemp & 0x0f);
            if (cCur < 10)
            {
                cCur += '0';
            }
            else
            {
                cCur += ((bIsUpper ? 'A' : 'a') - 10);
            }
            strHex[2 * i + 1 - j] = cCur;
            cTemp >>= 4;
        }
    }

    return strHex;
}
