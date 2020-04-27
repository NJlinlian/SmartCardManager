#include "base58.h"

#include <QDebug>


//static const char b58digits_ordered[] = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";
//static const int8_t b58digits_map[] = {
//    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
//    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
//    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
//    -1, 0, 1, 2, 3, 4, 5, 6, 7, 8,-1,-1,-1,-1,-1,-1,
//    -1, 9,10,11,12,13,14,15,16,-1,17,18,19,20,21,-1,
//    22,23,24,25,26,27,28,29,30,31,32,-1,-1,-1,-1,-1,
//    -1,33,34,35,36,37,38,39,40,41,42,43,-1,44,45,46,
//    47,48,49,50,51,52,53,54,55,56,57,-1,-1,-1,-1,-1,
//};



//bool b58enc(char *b58, size_t *b58sz, const void *data, size_t binsz)
//{
//    const uint8_t *bin = static_cast<const uint8_t *>(data);
//    int carry;
//    int64_t i, j, high, zcount = 0;
//    uint8_t buf[12 * 1024] = {0};
//    size_t size;

//    //计算待编码数据前面 0 的个数
//    while (zcount < binsz && !bin[zcount])
//        ++zcount;

//    //计算存放转换数据所需要的数组的大小    138/100 --> log(256)/log(58)
//    size = (binsz - zcount) * 138 / 100 + 1;
//    memset(buf, 0, size);

//    //遍历待转换数据
//    for (i = zcount, high = size - 1; i < binsz; ++i, high = j)
//    {
//        //将数据从后往前依次存放
//        for (carry = bin[i], j = size - 1; (j > high) || carry; --j)
//        {
////            qDebug() << "55555555 " << j;
//            carry += 256 * buf[j];
//            buf[j] = carry % 58;
//            carry /= 58;
//        }
//    }

//    for (j = 0; j < size && !buf[j]; ++j);
//    if (*b58sz <= zcount + size - j)
//    {
//        *b58sz = zcount + size - j + 1;
//        return false;
//    }

//    if (zcount)
//        memset(b58, '1', zcount);
//    for (i = zcount; j < size; ++i, ++j)
//        b58[i] = b58digits_ordered[buf[j]];
//    b58[i] = '\0';
//    *b58sz = i + 1;

//    return true;
//}

//bool b58dec(void *bin, size_t *binszp, const char *b58)
//{
//    size_t binsz = *binszp;
//    const unsigned char *b58u = (const unsigned char*)b58;
//    unsigned char *binu = static_cast<unsigned char *>(bin);

//    uint32_t outi[12 * 1024] = {0};
//    size_t outisz = (binsz + 3) / 4;

//    uint64_t t;
//    uint32_t c;
//    size_t i, j;
//    uint8_t bytesleft = binsz % 4;
//    uint32_t zeromask = bytesleft ? (0xffffffff << (bytesleft * 8)) : 0;
//    unsigned zerocount = 0;
//    size_t b58sz;

//    b58sz = strlen(b58);

//    memset(outi, 0, outisz * sizeof(*outi));

//    // Leading zeros, just count
//    for (i = 0; i < b58sz && b58u[i] == '1'; ++i)
//        ++zerocount;

//    for (; i < b58sz; ++i)
//    {
//        if (b58u[i] & 0x80)
//            // High-bit set on invalid digit
//            return false;
//        if (b58digits_map[b58u[i]] == -1)
//            // Invalid base58 digit
//            return false;
//        c = (unsigned)b58digits_map[b58u[i]];
//        for (j = outisz; j--; )
//        {
//            t = ((uint64_t)outi[j]) * 58 + c;
//            c = (t & 0x3f00000000) >> 32;
//            outi[j] = t & 0xffffffff;
//        }

//        if (c)
//            // Output number too big (carry to the next int32)
//            return false;
//        if (outi[0] & zeromask)
//            // Output number too big (last int32 filled too far)
//            return false;
//    }
//    qDebug() << "11111111 " << i;

//    j = 0;
//    switch (bytesleft) {
//    case 3:
//        *(binu++) = (outi[0] & 0xff0000) >> 16;
//        //-fallthrough
//    case 2:
//        *(binu++) = (outi[0] & 0xff00) >> 8;
//        //-fallthrough
//    case 1:
//        *(binu++) = (outi[0] & 0xff);
//        ++j;
//        //-fallthrough
//    default:
//        break;
//    }
//    qDebug() << "222222222222 " << j <<  outisz << binsz;

//    for (; j < outisz; ++j)
//    {
//        *(binu++) = (outi[j] >> 0x18) & 0xff;
//        *(binu++) = (outi[j] >> 0x10) & 0xff;
//        *(binu++) = (outi[j] >> 8) & 0xff;
//        *(binu++) = (outi[j] >> 0) & 0xff;
//    }

//    // Count canonical base58 byte count
//    binu = static_cast<unsigned char *>(bin);
//    for (i = 0; i < binsz; ++i)
//    {
//        if (binu[i]) {
//            if (zerocount > i) {
//                /* result too large */
//                return false;
//            }
//            break;
//        }
//        --*binszp;
//    }
//    qDebug() << "3333333333 " << *binszp;

//    *binszp += zerocount;

//    return true;
//}


//QString base58Encode(const QString &hexString)
//{
//    char convertBuf[12 * 1024] = { 0 };
//    QByteArray ba = QByteArray::fromHex(hexString.toUtf8());
//    size_t convertBufLen = size_t(hexString.size());
//    b58enc(convertBuf, &convertBufLen, ba.data(), size_t(ba.size()));
//    return convertBuf;
//}

//QString base58Decode(const QString &str)
//{
//    char convertBuf[12 * 1024] = { 0 };
//    size_t size = 34;
//    qDebug() << "size " << size << str.size();
//    int retCode = b58dec(convertBuf, &size, str.toUtf8());
//    qDebug() << "size2 " << size;

//    QByteArray ba(convertBuf,size);
//    return ba.toHex();
//}

static const char* pszBase58 = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";
bool DecodeBase58(const char *psz, std::vector<unsigned char> &vch)
{
    // Skip leading spaces.
    while (*psz && isspace(*psz))
        psz++;
    // Skip and count leading '1's.
    int zeroes = 0;
    int length = 0;
    while (*psz == '1') {
        zeroes++;
        psz++;
    }
    // Allocate enough space in big-endian base256 representation.
    int size = strlen(psz) * 733 /1000 + 1; // log(58) / log(256), rounded up.
    std::vector<unsigned char> b256(size);
    // Process the characters.
    while (*psz && !isspace(*psz)) {
        // Decode base58 character
        const char* ch = strchr(pszBase58, *psz);
        if (ch == nullptr)
            return false;
        // Apply "b256 = b256 * 58 + ch".
        int carry = ch - pszBase58;
        int i = 0;
        for (std::vector<unsigned char>::reverse_iterator it = b256.rbegin(); (carry != 0 || i < length) && (it != b256.rend()); ++it, ++i) {
            carry += 58 * (*it);
            *it = carry % 256;
            carry /= 256;
        }
        assert(carry == 0);
        length = i;
        psz++;
    }
    // Skip trailing spaces.
    while (isspace(*psz))
        psz++;
    if (*psz != 0)
        return false;
    // Skip leading zeroes in b256.
    std::vector<unsigned char>::iterator it = b256.begin() + (size - length);
    while (it != b256.end() && *it == 0)
        it++;
    // Copy result into output vector.
    vch.reserve(zeroes + (b256.end() - it));
    vch.assign(zeroes, 0x00);
    while (it != b256.end())
        vch.push_back(*(it++));
    return true;
}

std::string EncodeBase58(const unsigned char *pbegin, const unsigned char *pend)
{
    // Skip & count leading zeroes.
    int zeroes = 0;
    int length = 0;
    while (pbegin != pend && *pbegin == 0) {
        pbegin++;
        zeroes++;
    }
    // Allocate enough space in big-endian base58 representation.
    int size = (pend - pbegin) * 138 / 100 + 1; // log(256) / log(58), rounded up.
    std::vector<unsigned char> b58(size);
    // Process the bytes.
    while (pbegin != pend) {
        int carry = *pbegin;
        int i = 0;
        // Apply "b58 = b58 * 256 + ch".
        for (std::vector<unsigned char>::reverse_iterator it = b58.rbegin(); (carry != 0 || i < length) && (it != b58.rend()); it++, i++) {
            carry += 256 * (*it);
            *it = carry % 58;
            carry /= 58;
        }

        assert(carry == 0);
        length = i;
        pbegin++;
    }
    // Skip leading zeroes in base58 result.
    std::vector<unsigned char>::iterator it = b58.begin() + (size - length);
    while (it != b58.end() && *it == 0)
        it++;
    // Translate the result into a string.
    std::string str;
    str.reserve(zeroes + (b58.end() - it));
    str.assign(zeroes, '1');
    while (it != b58.end())
        str += pszBase58[*(it++)];
    return str;
}

QString EncodeBase58(const QByteArray& ba)
{
    std::vector<unsigned char> vch;
    for(int i = 0; i < ba.size(); i++)
    {
        vch.push_back((unsigned char)ba[i]);
    }
    return QString::fromStdString(EncodeBase58(vch.data(),  vch.data() + vch.size()));
}

QByteArray DecodeBase58(const QString &str)
{
    std::vector<unsigned char> vchRet;
    if(DecodeBase58(str.toStdString().c_str(), vchRet))
    {
        QByteArray ba;
        for(unsigned char ch : vchRet)
        {
            ba.append(ch);
        }
        return ba;
    }
    else
    {
        return "";
    }
}
