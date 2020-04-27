#pragma once

#include <vector>
#include <string>
#include <winscard.h>
#include <QString>

#include "lib/GP/GP_ITransmitter.h"

// implementation of GP_ITransmitter interface
class CSmartCard : public GPAPI::GP_ITransmitter
{
private:
	SCARDCONTEXT m_hContext;
	std::vector<std::wstring> m_pReadersList;
	DWORD m_dwReaders;
	DWORD m_dwProtocol;

public:
	SCARDHANDLE m_hCard;

	CSmartCard();
	~CSmartCard();

    bool inited = false;
	bool Init();
	void Uninit();

	std::vector<std::wstring> GetReaders();

	bool Connect(std::wstring szReader);
	void Disconnect();

	// implementation of GP_ITransmitter virtual methods
	bool Secure(GPAPI::GP_SecurityInfo* pSecurityInfo, const unsigned char* pInput, const unsigned long cInput, unsigned char* pOutput, unsigned long* cOutput);
	bool Exchange(const unsigned char* pInput, const unsigned long cInput, unsigned char* pOutput, unsigned long* cOutput);

public:
    int selectAID();        // 选择应用
    QByteArray readUncompressedPubKey();
    QByteArray readCompressedPubKey();
    QByteArray compressPubKey(QByteArray pubKey);
    QByteArray readPriKey();

    QByteArray readSerialNumber();  // 读序列号
    int setPwd(QString pwd);            // 设置密码 重设密码需要先验证密码
    int validPwd(QString pwd);          // 验证密码

    QByteArray signedData;
    int signTrxHash(const QString& trxHash);   // 签名
};
