#ifndef HK_NFCNDEF_H
#define HK_NFCNDEF_H

#include <stdint.h>
#include "HkNfcNdefMsg.h"

#define HKNFCNDEF_LC(c1,c2) (((c1)<<8) | c2)

class HkNfcNdef {
public:
	/**
	 * @enum	HkNfcNdef::RecordType
	 * @brief	レコードタイプ
	 */
	enum RecordType {
		RTD_NONE,
		RTD_TEXT,
		RTD_URI,
		RTD_SP,
	};
	
	/**
	 * @enum	HkNfcNdef::LanguageCode
	 * @brief	国コード
	 */
	enum LanguageCode {
		LC_EN = HKNFCNDEF_LC('e', 'n'),
		LC_JP = HKNFCNDEF_LC('j', 'a'),
	};
	
	enum UriType {
		HTTP_WWW = 0x01,
		HTTPS_WWW = 0x02,
		HTTP = 0x03,
		HTTPS = 0x04
	};

public:
	static bool createText(HkNfcNdefMsg* pMsg, const void* pUTF8, uint16_t len, bool bLong=false, LanguageCode lc=LC_EN);
	static bool createUrl(HkNfcNdefMsg* pMsg, UriType Type, const char* pUrl);


private:
	HkNfcNdef();
	HkNfcNdef(const HkNfcNdef&);
	~HkNfcNdef();
};

#endif /* HK_NFCNDEF_H */
