#ifndef BCHUTIL_H
#define BCHUTIL_H

#include "BitcoinUtil.h"
#include <bitcoin/bitcoin.hpp>

namespace BCHUtil {

//template<typename T, size_t N>
//class array {
//  public:
//  /**
//   *  Checked indexing (when in debug build) that also simplifies dereferencing
//   *  when you have an array<T,N>*.
//   */
//  ///@{
//  T&       at( size_t pos )      { assert( pos < N); return data[pos]; }
//  const T& at( size_t pos )const { assert( pos < N); return data[pos]; }
//  ///@}

//  T&       operator[]( size_t pos )      { assert( pos < N); return data[pos]; }
//  const T& operator[]( size_t pos )const { assert( pos < N); return data[pos]; }


//  const T*     begin()const  {  return &data[0]; }
//  const T*     end()const    {  return &data[N]; }

//  T*           begin()       {  return &data[0]; }
//  T*           end()         {  return &data[N]; }

//  size_t       size()const { return N; }

//  T data[N];
//};

template <int frombits, int tobits, bool pad, typename O, typename I>
bool ConvertBits(O &out, I it, I end) {
    size_t acc = 0;
    size_t bits = 0;
    constexpr size_t maxv = (1 << tobits) - 1;
    constexpr size_t max_acc = (1 << (frombits + tobits - 1)) - 1;
    while (it != end) {
        acc = ((acc << frombits) | *it) & max_acc;
        bits += frombits;
        while (bits >= tobits) {
            bits -= tobits;
            out.push_back((acc >> bits) & maxv);
        }
        ++it;
    }

    // We have remaining bits to encode but do not pad.
    if (!pad && bits) {
        return false;
    }

    // We have remaining bits to encode so we do pad.
    if (pad && bits) {
        out.push_back((acc << (tobits - bits)) & maxv);
    }

    return true;
}

template <class T>
std::vector<uint8_t> PackAddrData(const T &id, uint8_t type) {
    uint8_t version_byte(type << 3);
    size_t size = id.size();
    uint8_t encoded_size = 0;
    switch (size * 8) {
    case 160:
        encoded_size = 0;
        break;
    case 192:
        encoded_size = 1;
        break;
    case 224:
        encoded_size = 2;
        break;
    case 256:
        encoded_size = 3;
        break;
    case 320:
        encoded_size = 4;
        break;
    case 384:
        encoded_size = 5;
        break;
    case 448:
        encoded_size = 6;
        break;
    case 512:
        encoded_size = 7;
        break;
    default:
        throw std::runtime_error(
                    "Error packing cashaddr: invalid address length");
    }
    version_byte |= encoded_size;
    std::vector<uint8_t> data = { version_byte };
    data.insert(data.end(), std::begin(id), std::end(id));

    std::vector<uint8_t> converted;
    // Reserve the number of bytes required for a 5-bit packed version of a
    // hash, with version byte.  Add half a byte(4) so integer math provides
    // the next multiple-of-5 that would fit all the data.
    converted.reserve(((size + 1) * 8 + 4) / 5);
    ConvertBits<8, 5, true>(converted, std::begin(data), std::end(data));

    return converted;
}

struct pts_address
{
    pts_address();
    pts_address( const std::string& base58str );   ///< converts to binary, validates checksum

    uint8_t version()const { return addr.at(0); }
    bool is_valid()const;

    operator std::string()const; ///< converts to base58 + checksum

    QByteArray addr;
};

struct pts_address_bch
{
    pts_address_bch();
    pts_address_bch(const pts_address & pts_addr, uint8_t kh, uint8_t sh);


    uint8_t version()const { return addr.at(0); }
    bool is_valid()const;

    operator std::string()const; ///< converts to base58 + checksum

    QByteArray addr; ///< binary representation of address

};


QString btcAddrToBch(const QString& btcAddr, int btc_pubKey = 0x0, int btc_script = 0x05);
QByteArray createToSignData( const std::string& redeemscript_hex, const std::string& raw_trx, int vin_index,const uint64_t& value);
std::string makeRedeemScript(const std::string& pubkey_hex_str);
}

std::string BinToHex(const std::vector<char> &strBin, bool bIsUpper);

#endif // BCHUTIL_H
