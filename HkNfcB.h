#ifndef HKNFCB_H
#define HKNFCB_H

#include "HkNfcRule.h"
#ifdef HKNFCRW_USE_NFCB

#include <stdint.h>

/**
 * @class	HkNfcB
 * @brief	NFC-Bアクセス
 */
class HkNfcB {
public:
	static const uint8_t NFCID_LEN = 4;

private:
	HkNfcB();
	HkNfcB(const HkNfcB&);
	~HkNfcB();

public:
	static bool polling();

	static bool read(uint8_t* buf, uint8_t blockNo=0x00) { return false; }
	static bool write(const uint8_t* buf, uint8_t blockNo=0x00) { return false; }
};

#endif /* HKNFCRW_USE_NFCB */

#endif /* HKNFCB_H */
