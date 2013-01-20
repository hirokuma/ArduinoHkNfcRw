#include <cstring>
#include "HkNfcNdef.h"
#include "misc.h"

namespace {
	const uint8_t MB = 0x80;
	const uint8_t ME = 0x40;
	const uint8_t CF = 0x20;
	const uint8_t SR = 0x10;
	const uint8_t IL = 0x08;
	const uint8_t TNF_EMPTY = 0x00;
	const uint8_t TNF_WK = 0x01;
}


/**
 * NDEF TEXT作成
 *
 * @param[out]	pMsg		NDEF TEXT
 * @param[in]	pUTF8		テキストデータ(UTF-8)
 * @param[in]	len			テキストデータ長(\0は含まない)
 * @param[in]	bLong		true:Short formatではない形式
 * @param[in]	lc			国コード
 * @return		作成成功/失敗
 */
bool HkNfcNdef::createText(HkNfcNdefMsg* pMsg, const void* pUTF8, uint16_t len, bool bLong/*=false*/, LanguageCode lc/*=LC_EN*/)
{
	if(bLong == true) {
		//まだ長い文字列には対応していない
		return false;
	}

	uint16_t pos = 0;
	
	pMsg->Data[pos++] = (uint8_t)(MB | ME | SR | TNF_WK);
	pMsg->Data[pos++] = 1;		//Type Length
	pMsg->Data[pos++] = (uint8_t)len;		//Payload Length
	//ID Lengthは、なし
	pMsg->Data[pos++] = 'T';		//Type=TEXT
	//IDも、なし

	//TEXT本体
	pMsg->Data[pos++] = 0x02;		//UTF-8かつ国コードは2byte
	pMsg->Data[pos++] = (uint8_t)(lc >> 8);
	pMsg->Data[pos++] = (uint8_t)lc;
	hk_memcpy(pMsg->Data + pos, pUTF8, len);
	pMsg->Length = (uint16_t)(pos + len);

	return true;
}

bool HkNfcNdef::createUrl(HkNfcNdefMsg* pMsg, UriType Type, const char* pUrl)
{
	uint16_t pos = 0;
	uint8_t len = (uint8_t)hk_strlen(pUrl);
	
	pMsg->Data[pos++] = (uint8_t)(MB | ME | SR | TNF_WK);
	pMsg->Data[pos++] = 1;		//Type Length
	pMsg->Data[pos++] = (uint8_t)(1 + len);		//Payload Length
	//ID Lengthは、なし
	pMsg->Data[pos++] = 'U';		//Type=URI
	//IDも、なし

	//URI本体
	pMsg->Data[pos++] = (uint8_t)Type;
	hk_memcpy(pMsg->Data + pos, pUrl, len);
	pMsg->Length = (uint16_t)(pos + len);

	return true;
}
