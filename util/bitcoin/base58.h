#ifndef BASE58_H
#define BASE58_H

#include <QObject>

bool DecodeBase58(const char* psz, std::vector<unsigned char>& vch);
std::string EncodeBase58(const unsigned char* pbegin, const unsigned char* pend);

QString EncodeBase58(const QByteArray& ba);

QByteArray DecodeBase58(const QString &str);

//bool b58enc(char *b58, size_t *b58sz, const void *data, size_t binsz);
//bool b58dec(void *bin, size_t *binszp, const char *b58);

//QString base58Encode(const QString& hexString);
//QString base58Decode(const QString& str);

#endif // BASE58_H
