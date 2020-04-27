#include "SSLUtils.h"


#include "openssl/err.h"
#include "openssl/ssl.h"
#include "openssl/aes.h"
#include "openssl/ripemd.h"
#include <QDebug>
#include <QDir>

#define BASE58TABLE "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz"
static const char b58digits_ordered[] = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";
const char ALPHABET_MAP[128] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1,  0,  1,  2,  3,  4,  5,  6,  7,  8, -1, -1, -1, -1, -1, -1,
    -1,  9, 10, 11, 12, 13, 14, 15, 16, -1, 17, 18, 19, 20, 21, -1,
    22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, -1, -1, -1, -1, -1,
    -1, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, -1, 44, 45, 46,
    47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, -1, -1, -1, -1, -1
};


QByteArray SSLUtils::sha256(const QByteArray& source)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, source.data(), source.size());
    SHA256_Final(hash, &sha256);
    QByteArray ba((char*)hash, SHA256_DIGEST_LENGTH);

    return ba;
}

QByteArray SSLUtils::sha1(const QByteArray& source)
{
    unsigned char hash[SHA_DIGEST_LENGTH] = {0};
    SHA_CTX sha1;
    SHA_Init(&sha1);
    SHA_Update(&sha1, source.data(), source.size());
    SHA_Final(hash, &sha1);
    QByteArray ba((char*)hash, SHA_DIGEST_LENGTH);
    return ba;
}


QByteArray SSLUtils::ripemd160(const QByteArray& data)
{
    unsigned char hash[RIPEMD160_DIGEST_LENGTH] = {0};
    RIPEMD160_CTX ripemd160_CTX;
    RIPEMD160_Init(&ripemd160_CTX);
    RIPEMD160_Update(&ripemd160_CTX, data.data(), data.size());
    RIPEMD160_Final(hash, &ripemd160_CTX);
    QByteArray ba((char*)hash, RIPEMD160_DIGEST_LENGTH);

    return ba;
}


std::string SSLUtils::base58(const std::string &hexstring)
{
    std::string result = "";
    BN_CTX * bnctx = BN_CTX_new();
    BIGNUM * bn = BN_new();
    BIGNUM * bn0= BN_new();
    BIGNUM * bn58=BN_new();
    BIGNUM * dv = BN_new();
    BIGNUM * rem= BN_new();

    BN_hex2bn(&bn, hexstring.c_str());
    //printf("bn:%s\n", BN_bn2dec(bn));
    BN_hex2bn(&bn58, "3a");//58
    BN_hex2bn(&bn0,"0");

    while(BN_cmp(bn, bn0)>0){
        BN_div(dv, rem, bn, bn58, bnctx);
        BN_copy(bn, dv);
        //printf("dv: %s\n", BN_bn2dec(dv));
        //printf("rem:%s\n", BN_bn2dec(rem));
        char base58char = BASE58TABLE[BN_get_word(rem)];

        result += base58char;
    }

    std::string::iterator pbegin = result.begin();
    std::string::iterator pend   = result.end();
    while(pbegin < pend) {
        char c = *pbegin;
        *(pbegin++) = *(--pend);
        *pend = c;
    }
    return result;


//        char* b58;
//        int b58sz;
//        int binsz = hexstring.size();
//        int carry;
//        int i, j, high, zcount = 0;
//        uint8_t buf[12 * 1024] = {0};
//        int size;

//        //计算待编码数据前面 0 的个数
//        while (zcount < binsz && !hexstring[zcount])
//            ++zcount;

//        //计算存放转换数据所需要的数组的大小    138/100 --> log(256)/log(58)
//        size = (binsz - zcount) * 138 / 100 + 1;
//        memset(buf, 0, size);

//        //遍历待转换数据
//        for (i = zcount, high = size - 1; i < binsz; ++i, high = j)
//        {
//            //将数据从后往前依次存放
//            for (carry = hexstring[i], j = size - 1; (j > high) || carry; --j)
//            {
//                carry += 256 * buf[j];
//                buf[j] = carry % 58;
//                carry /= 58;
//            }
//        }

//        for (j = 0; j < size && !buf[j]; ++j);

//        if (b58sz <= zcount + size - j)
//        {
//            b58sz = zcount + size - j + 1;
//            return false;
//        }

//        if (zcount)
//            memset(b58, '1', zcount);
//        for (i = zcount; j < size; ++i, ++j)
//            b58[i] = b58digits_ordered[buf[j]];
//        b58[i] = '\0';
//        b58sz = i + 1;
//        QByteArray ba(b58, b58sz);

//        return ba;
}

QPair<QString, QString> SSLUtils::decodeDerSig(QString sig)
{
    int leftLen = sig.mid(6,2).toInt(nullptr,16) * 2;
    QString left = sig.mid(8,leftLen);
    int rightLen = sig.mid(10 + leftLen, 2).toInt(nullptr,16) * 2;
    QString right = sig.mid(12 + leftLen, rightLen);
    return  QPair<QString,QString>(left,right);
}
