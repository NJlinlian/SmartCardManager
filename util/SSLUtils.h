#ifndef SSLUTILS_H
#define SSLUTILS_H

#include <QString>
#include <QPair>
#include "openssl/evp.h"


namespace SSLUtils {

    QByteArray sha256(const QByteArray &str);
    QByteArray sha1(const QByteArray& str);

    QByteArray ripemd160(const QByteArray& data);

    std::string base58(const std::string &hexstring);

    QPair<QString,QString> decodeDerSig(QString sig);
}

#endif // SSLUTILS_H
