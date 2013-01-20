#include "HkNfcRule.h"
#ifdef HKNFCRW_USE_NFCA

#include <string.h>

#include "HkNfcRw.h"
#include "HkNfcA.h"
#include "NfcPcd.h"
#include "misc.h"

#define LOG_TAG "HkNfcA"
#include "nfclog.h"


uint16_t			HkNfcA::m_SensRes;
HkNfcA::SelRes		HkNfcA::m_SelRes;


namespace {
	//Polling
	const uint8_t INLISTPASSIVETARGET[] = { 0x00 };

	//Key A Authentication
	const uint8_t KEY_A_AUTH = 0x60;
	const uint8_t KEY_B_AUTH = 0x61;
	const uint8_t READ = 0x30;
	const uint8_t UPDATE = 0xa0;
}


/**
 * [NFC-A]Polling
 *
 * @retval		true			成功
 * @retval		false			失敗
 */
bool HkNfcA::polling()
{
	int ret;
	uint8_t responseLen;
	uint8_t* pData;

	ret = NfcPcd::inListPassiveTarget(
					INLISTPASSIVETARGET, sizeof(INLISTPASSIVETARGET),
					&pData, &responseLen);
	if (!ret
	  || (responseLen < 12)
	  || (responseLen <  7 + *(pData + 7))) {
		LOGE("pollingA fail: ret=%d/len=%d\n", ret, responseLen);
		return false;
	}

	//[0] d5
	//[1] 4b
	//[2] NbTg
	//[3] Tg
	if(*(pData + 3) != 0x01) {
		LOGE("bad TargetNo : %02x\n", *(pData + 3));
		return false;
	}

	m_SensRes = (uint16_t)((*(pData + 4) << 8) | *(pData + 5));
	LOGD("SENS_RES:%04x\n", m_SensRes);

	m_SelRes = (SelRes)*(pData + 6);
	const char* sel_res;
	switch(m_SelRes) {
	case MIFARE_UL:			sel_res = "MIFARE Ultralight";		break;
	case MIFARE_1K:			sel_res = "MIFARE 1K";				break;
	case MIFARE_MINI:		sel_res = "MIFARE MINI";			break;
	case MIFARE_4K:			sel_res = "MIFARE 4K";				break;
	case MIFARE_DESFIRE:	sel_res = "MIFARE DESFIRE";			break;
	case JCOP30:			sel_res = "JCOP30";					break;
	case GEMPLUS_MPCOS:		sel_res = "Gemplus MPCOS";			break;
	default:
		m_SelRes = SELRES_UNKNOWN;
		sel_res = "???";
	}
	LOGD("SEL_RES:%02x(%s)\n", m_SelRes, sel_res);

	if(responseLen <  7 + *(pData + 7)) {
		LOGE("bad length\n");
		return false;
	}

	NfcPcd::setNfcId(pData + 8, *(pData + 7));

	return true;
}


bool HkNfcA::read(uint8_t* buf, uint8_t blockNo)
{
	NfcPcd::commandBuf(0) = 0x01;
	NfcPcd::commandBuf(2) = blockNo;
	NfcPcd::commandBuf(3) = 0xff;
	NfcPcd::commandBuf(4) = 0xff;
	NfcPcd::commandBuf(5) = 0xff;
	NfcPcd::commandBuf(6) = 0xff;
	NfcPcd::commandBuf(7) = 0xff;
	NfcPcd::commandBuf(8) = 0xff;
	memcpy(NfcPcd::commandBuf() + 9, NfcPcd::nfcId(), NfcPcd::nfcIdLen());

	uint8_t len;
	bool ret;

#if 1
	// Key A Authentication
	NfcPcd::commandBuf(1) = KEY_A_AUTH;
	ret = NfcPcd::inDataExchange(
					NfcPcd::commandBuf(), 9 + NfcPcd::nfcIdLen(),
					NfcPcd::responseBuf(), &len);
	if(!ret) {
		LOGE("read fail1\n");
		return false;
	}
#endif

#if 0
	// Key B Authentication
	NfcPcd::commandBuf(1) = KEY_B_AUTH;
	ret = NfcPcd::inDataExchange(
					NfcPcd::commandBuf(), 9 + NfcPcd::nfcIdLen(),
					NfcPcd::responseBuf(), &len);
	if(!ret) {
		LOGE("read fail2\n");
		return false;
	}
#endif

	// Read
	NfcPcd::commandBuf(1) = READ;
	ret = NfcPcd::inDataExchange(
					NfcPcd::commandBuf(), 3,
					NfcPcd::responseBuf(), &len);
	if(ret) {
		memcpy(buf, NfcPcd::responseBuf(), len);
	} else {
		LOGE("read fail3\n");
	}

	return ret;
}

bool HkNfcA::write(const uint8_t* buf, uint8_t blockNo)
{
	return false;
}

#endif /* HKNFCRW_USE_NFCA */
