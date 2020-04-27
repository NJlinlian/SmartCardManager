#include "CardView.h"

#include <QDebug>

CardView::CardView()
{
    sc = new CSmartCard;
    qDebug() << sc->Init();
}

CardView::~CardView()
{
    if(sc)
    {
        delete sc;
    }
}

bool CardView::checkConnecting()
{
    if(!sc->inited)     return false;

    std::vector<std::wstring> readers = sc->GetReaders();
//    qDebug() << readers;
    if(readers.size() > 0)
    {
        int connectRt = sc->Connect(readers.at(0));
//        qDebug() << "ccccccccccccc" <<  connectRt;
        if(connectRt)
        {
            int aidRt = sc->selectAID();
//            qDebug() << "aid" << aidRt;
            if(aidRt != 0)
            {
                return true;
            }
        }
    }

    return false;
}

bool CardView::updatePubKey()
{
    if(!checkConnecting())      return false;

    uncompressedPubKey = sc->readUncompressedPubKey();

    return (uncompressedPubKey.size() > 0);
}

QByteArray CardView::getUncompressedPubKey()
{
    return uncompressedPubKey.toHex();
}

QByteArray CardView::getCompressedPubKey()
{
    return sc->compressPubKey(uncompressedPubKey).toHex();
}

int CardView::signTrxHash(const QString& trxHash)
{
    int rt = sc->signTrxHash(trxHash);
    signedData = sc->signedData;
    return rt;
}

QByteArray CardView::getSignedData()
{
    QByteArray rt = signedData;
    signedData.clear();
    return rt;
}


