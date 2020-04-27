#ifndef CARDVIEW_H
#define CARDVIEW_H

#include <QObject>
#include "CSmartCard.h"

class CardView : public QObject
{
    Q_OBJECT
public:
    CardView();
    ~CardView();

    bool checkConnecting();
    Q_INVOKABLE bool updatePubKey();

    Q_INVOKABLE QByteArray getUncompressedPubKey();
    Q_INVOKABLE QByteArray getCompressedPubKey();
    Q_INVOKABLE int signTrxHash(const QString& trxHash);
    Q_INVOKABLE QByteArray getSignedData();     // 只允许读取一次 读取完清空



private:
    CSmartCard* sc = nullptr;
    QByteArray uncompressedPubKey;
    QByteArray signedData;

};

#endif // CARDVIEW_H
