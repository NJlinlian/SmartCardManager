#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <QDebug>
#include <winscard.h>
#include "CSmartCard.h"

#include "lib/GP/GP_SecurityInfo.h"
#include "lib/GP/GP_Command.h"
#include "lib/GP/GP_Select.h"
#include "lib/GP/GP_InitUpd.h"
#include "lib/GP/GP_ExtAuth.h"
#include "lib/GP/GP_GetData.h"

CSmartCard::CSmartCard()
{

}

CSmartCard::~CSmartCard()
{
	Uninit();
}

bool CSmartCard::Init()
{
    inited = false;
	LONG result;

	LPTSTR pReaders, pReader;

	// context
	result = SCardEstablishContext(SCARD_SCOPE_USER,  NULL, NULL, &m_hContext);
	if(result != SCARD_S_SUCCESS)
	{
		fprintf(stderr,"error: SCardEstablishContext\n");
		goto FINISH;
	}

	// get readers
	m_dwReaders = SCARD_AUTOALLOCATE;
	result = SCardListReaders(m_hContext, NULL, (LPTSTR)&pReaders, &m_dwReaders);
	if(result != SCARD_S_SUCCESS)
	{
		fprintf(stderr,"error: SCardListReaders\n");
		goto FINISH;
	}

    pReader = pReaders;
	while(*pReader != '\0')
	{
        std::wstring tmp = (wchar_t*)pReaders;
        m_pReadersList.push_back(tmp);
		pReader = pReader + wcslen((wchar_t *)pReader) + 1;
	}

	// free the memory
	result = SCardFreeMemory(m_hContext, pReaders);
	if(result != SCARD_S_SUCCESS)
	{
		fprintf(stderr,"error: SCardFreeMemory\n");
		goto FINISH;
	}

    inited = true;

FINISH:
    return inited;
}

void CSmartCard::Uninit()
{
	m_pReadersList.clear();
	SCardReleaseContext(m_hContext);
    inited = true;
}

std::vector<std::wstring> CSmartCard::GetReaders()
{
	return m_pReadersList;
}

bool CSmartCard::Connect(std::wstring szReader)
{
	if(SCardConnect(m_hContext, (LPCTSTR)szReader.c_str(), SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1, &m_hCard, &m_dwProtocol) != SCARD_S_SUCCESS)
		return false;

	return true;
}

void CSmartCard::Disconnect()
{
	SCardDisconnect(m_hCard, SCARD_LEAVE_CARD);
}

// implementation of GP_ITransmitter virtual method for securing APDU request
bool CSmartCard::Secure(GPAPI::GP_SecurityInfo* pSecurityInfo, const unsigned char* pInput, const unsigned long cInput, unsigned char* pOutput, unsigned long* cOutput)
{
	fprintf(stderr,"SECURITY LEVEL: %d\r\n", pSecurityInfo->GetSecurityLevel());
	fprintf(stderr,"KEY VERSION: %d\r\n", pSecurityInfo->GetKeyVersion());
	fprintf(stderr,"KEY IDENTIFIER: %d\r\n", pSecurityInfo->GetKeyIdentifier());

	// simulates C-MAC computation (will not work for real application)
	unsigned char CMAC[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

	// add C-MAC
	*cOutput = cInput+sizeof(CMAC);
	memcpy(pOutput+cInput, CMAC, sizeof(CMAC));
	memcpy(pOutput, pInput, cInput);

	// modify Lc
	pOutput[4] = (char)(*cOutput-5);

	return true;
}

// implementation of GP_ITransmitter virtual method for exchanging APDU messages
bool CSmartCard::Exchange(const unsigned char* pInput, const unsigned long cInput, unsigned char* pOutput, unsigned long* cOutput)
{
    // print request
//    fprintf(stderr, ">> ");
//    for(int i=0; i<(int)cInput; i++)
//        fprintf(stderr,"%.2x ", pInput[i]);
//    fprintf(stderr,"\r\n");

	// send request to smart-card and get its response
	if(SCardTransmit(m_hCard, SCARD_PCI_T1, pInput, cInput, NULL, pOutput, cOutput) != SCARD_S_SUCCESS)
		return false;

    // print response
//    fprintf(stderr, "<< ");
//    for(int i=0; i<(int)*cOutput; i++)
//        fprintf(stderr,"%.2x ", pOutput[i]);
//    fprintf(stderr,"\r\n");

	return true;
}

int CSmartCard::selectAID()
{
    if(!inited)     return 0;

    unsigned char AID[] = {0x11, 0x00, 0x00, 0x00, 0x01};
    GPAPI::GP_Select sel;
    sel.SetAID(AID, sizeof(AID));
    if(sel.Transmit(this))
    {
        if(sel.CheckSW_9a00())
        {
            GPAPI::RESPONSE* resp = sel.GetResponse();
            if(resp->Size > 1)
            {
                return resp->Data[1];
            }
            else
            {
                return -1;
            }
        }
        else
        {
            GPAPI::RESPONSE* resp = sel.GetResponse();
            fprintf(stderr,"%.2x %.2x\r\n", resp->SW1, resp->SW2);
            return -1;
        }
    }
    else
    {
        fprintf(stderr,"Error: %s\r\n", sel.GetLastError());
        return -1;
    }
}

QByteArray CSmartCard::readUncompressedPubKey()
{
    QByteArray result;

    if(!inited)     return result;

    GPAPI::GP_GetData get;
    get.SetObjectTag(0x02, 0x00);
    if(get.Transmit(this))
    {
        if(get.CheckSW_9a00())
        {
            GPAPI::RESPONSE* resp = get.GetResponse();
            for(int i = 0; i < resp->Size; i++)
            {
                result += static_cast<char>(resp->Data[i]);
            }
            return result;
        }
        else
        {
            GPAPI::RESPONSE* resp = get.GetResponse();
            fprintf(stderr,"%.2x %.2x\r\n", resp->SW1, resp->SW2);
            return result;
        }
    }
    else
    {
        fprintf(stderr,"Error: %s\r\n", get.GetLastError());
        return result;
    }

}

QByteArray CSmartCard::readCompressedPubKey()
{
    QByteArray unCompressed = readUncompressedPubKey();
    return compressPubKey(unCompressed);
}

QByteArray CSmartCard::compressPubKey(QByteArray pubKey)
{
    if(pubKey.size() != 65 && pubKey.at(0) != 0x04)    return pubKey;

    pubKey = pubKey.mid(1);
    QByteArray x = pubKey.left(32);
    QByteArray y = pubKey.right(32);
    return x.prepend( (y.at(31) % 2) ? 0x03 : 0x02);
}

QByteArray CSmartCard::readPriKey()
{
    QByteArray result;
    if(!inited)     return result;

    GPAPI::GP_GetData get;
    get.SetObjectTag(0x05, 0x00);
    if(get.Transmit(this))
    {
        if(get.CheckSW_9a00())
        {
            GPAPI::RESPONSE* resp = get.GetResponse();
            for(int i = 0; i < resp->Size; i++)
            {
                result += static_cast<char>(resp->Data[i]);
            }
            return result;
        }
        else
        {
            GPAPI::RESPONSE* resp = get.GetResponse();
            fprintf(stderr,"%.2x %.2x\r\n", resp->SW1, resp->SW2);
            return result;
        }
    }
    else
    {
        fprintf(stderr,"Error: %s\r\n", get.GetLastError());
        return result;
    }
}

QByteArray CSmartCard::readSerialNumber()
{
    QByteArray result;
    if(!inited)     return result;

    GPAPI::GP_GetData get;
    get.SetObjectTag(0x07, 0x00);
    if(get.Transmit(this))
    {
        if(get.CheckSW_9a00())
        {
            GPAPI::RESPONSE* resp = get.GetResponse();
            for(int i = 0; i < resp->Size; i++)
            {
                result += static_cast<char>(resp->Data[i]);
            }
            return result;
        }
        else
        {
            GPAPI::RESPONSE* resp = get.GetResponse();
            fprintf(stderr,"%.2x %.2x\r\n", resp->SW1, resp->SW2);
            return result;
        }
    }
    else
    {
        fprintf(stderr,"Error: %s\r\n", get.GetLastError());
        return result;
    }
}

int CSmartCard::setPwd(QString pwd)
{
    if(!inited)     return 0;

    QVector<unsigned char> v;
    QByteArray ba = pwd.toLatin1();

    v.append({0x80,0x80,0x0A,0x00});
    v.append(static_cast<unsigned char>(ba.size()));
    for(int i = 0; i < ba.size(); i++)
    {
        v.append(static_cast<unsigned char>(ba.at(i)));
    }

    unsigned char pres[270] = {0};
    unsigned long cres = sizeof(pres);

    if(Exchange( static_cast<const unsigned char*>(v.data()), v.size(), pres, &cres))
    {
        if(cres == 2 && pres[0] == 0x9A && pres[1] == 0x00)
        {
            return  1;      // 正确
        }
        else if(cres == 2 && pres[0] == 0x6D && pres[1] == 0x07)
        {
            return -2; // 密码长度错误
        }
        else if(cres == 2 && pres[0] == 0x6D && pres[1] == 0x0C)
        {
            return -3; // 已有密码 需要先验证密码
        }
    }
    else
    {
        return -1;
    }
}

int CSmartCard::validPwd(QString pwd)
{
    if(!inited)     return 0;

    QVector<unsigned char> v;
    QByteArray ba = pwd.toLatin1();

    v.append({0x80,0x80,0x0B,0x00});
    v.append(static_cast<unsigned char>(ba.size()));
    for(int i = 0; i < ba.size(); i++)
    {
        v.append(static_cast<unsigned char>(ba.at(i)));
    }

    unsigned char pres[270] = {0};
    unsigned long cres = sizeof(pres);

    if(Exchange( static_cast<const unsigned char*>(v.data()), v.size(), pres, &cres))
    {
        if(cres == 2 && pres[0] == 0x9A && pres[1] == 0x00)
        {
            return  1;      // 正确
        }
        else if(cres == 2 && pres[0] == 0x6D && pres[1] == 0x0C)
        {
            return -3; // 验证错误
        }
    }
    else
    {
        return -1;
    }
}

int CSmartCard::signTrxHash(const QString& trxHash)
{
    QVector<unsigned char> result;
    if(!inited)     return 0;

    QVector<unsigned char> v;
    QByteArray ba = QByteArray::fromHex(trxHash.toUtf8());
    v.append({0x80,0x80,0x04,0x00});
    v.append(static_cast<unsigned char>(ba.size()));
    for(int i = 0; i < ba.size(); i++)
    {
        v.append(static_cast<unsigned char>(ba.at(i)));
    }

    unsigned char pres[270] = {0};
    unsigned long cres = sizeof(pres);

    signedData.clear();
    if(Exchange( static_cast<const unsigned char*>(v.data()), v.size(), pres, &cres))
    {
        if(cres > 2 && pres[cres - 2] == 0x9A && pres[cres - 1] == 0x00)
        {
            for(int i = 0; i < int(cres - 2); i++)
            {
                signedData[i] = char(pres[i]);
            }
            return  1;      // 正确
        }
        else if(cres == 2 && pres[0] == 0x6D && pres[1] == 0x0C)
        {
            return -3;      // 已有密码 需要先验证密码
        }
        else
        {
            return -1;
        }
    }
    else
    {
        return -1;
    }
}
