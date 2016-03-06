#ifndef HKNFCA_H
#define HKNFCA_H

#include "HkNfcRule.h"

#ifdef HKNFCRW_USE_NFCA

#include <stdint.h>

/**
 * @class		HkNfcA
 * @brief		NFC-Aアクセス
 */
class HkNfcA {
public:

	/**
	 * @enum	SelRes
	 * @brief	SEL_RES
	 */
	enum SelRes {
		MIFARE_UL		= 0x00,		///< MIFARE Ultralight
		MIFARE_1K		= 0x08,		///< MIFARE 1K
		MIFARE_MINI		= 0x09,		///< MIFARE MINI
		MIFARE_4K		= 0x18,		///< MIFARE 4K
		MIFARE_DESFIRE	= 0x20,		///< MIFARE DESFIRE
		JCOP30			= 0x28,		///< JCOP30
		GEMPLUS_MPCOS	= 0x98,		///< Gemplus MPCOS

        SELRES_UNKNOWN	= 0xff		///< 不明
	};

#if 0
	enum Error {
		SUCCESS				= 0x00,

		ERR_TIMEOUT			= 0x01,
		ERR_CRC				= 0x02,
		ERR_PARITY			= 0x03,
		ERR_ANTICOL			= 0x04,
		ERR_FRAMING			= 0x05,
		ERR_BITCOL			= 0x06,
		ERR_LESSBUF			= 0x07,
		ERR_RFBUF			= 0x08,
		ERR_
	};
#endif

private:
	HkNfcA();
	HkNfcA(const HkNfcA&);
	~HkNfcA();

public:
	static bool polling();
	static bool read(uint8_t* buf, uint8_t blockNo, bool bClassic=false);
	static bool write(const uint8_t* buf, uint8_t blockNo);

	/// SEL_RES取得
	static SelRes getSelRes() { return m_SelRes; }

private:
	static uint16_t		m_SensRes;
	static SelRes		m_SelRes;
};

#endif /* HKNFCRW_USE_NFCA */

#endif /* HKNFCA_H */
