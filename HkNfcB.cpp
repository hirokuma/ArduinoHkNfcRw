#include "HkNfcRule.h"
#ifdef HKNFCRW_USE_NFCB

#include "HkNfcRw.h"
#include "HkNfcB.h"
#include "NfcPcd.h"

#define LOG_TAG "HkNfcB"
#include "nfclog.h"


namespace {
	//Polling
	const uint8_t INLISTPASSIVETARGET[] = { 0x03, 0x00 };
	const uint8_t INLISTPASSIVETARGET_RES = 0x01;
}


/**
 * [NFC-B]Polling
 *
 * @retval		true			成功
 * @retval		false			失敗
 */
bool HkNfcB::polling()
{
	int ret;
	uint8_t responseLen;
	uint8_t* pData;

	ret = NfcPcd::inListPassiveTarget(
					INLISTPASSIVETARGET, sizeof(INLISTPASSIVETARGET),
					&pData, &responseLen);
	if (!ret || (responseLen < 17)) {
		//LOGE("pollingB fail: ret=%d/len=%d\n", ret, responseLen);
		return false;
	}

	//[0] d5
	//[1] 4b
	//[2] NbTg
	//[3] Tg
	if(*(pData + 3) != 0x01) {
		LOGE("bad TargetNo : %02x", *(pData + 3));
		return false;
	}

	//[4] 50
	if(*(pData + 4) != 0x50) {
		LOGE("not ATQB : %02x", *(pData + 4));
		return false;
	}
	
	//[5..8]NFCID0
	NfcPcd::setNfcId(pData + 5, NFCID_LEN);
	LOGD("NFCID0 : %02x%02x%02x%02x\n", *(pData + 5), *(pData + 6), *(pData + 7), *(pData + 8));

	//Application Data
	//[9]AFI
	LOGD("AFI : %02x\n", *(pData + 9));
	
	//[10]CRC_B(AID)[0]
	//[11]CRC_B(AID)[1]
	LOGD("CRC_B(AID) : %02x%02x\n", *(pData + 10), *(pData + 11));
	
	//[12]Number of application
	LOGD("Number of application : %02x\n", *(pData + 12));

	//Protocol Info
	//[13]Bit_Rate_Capability
	LOGD("Bit_Rate_Capability : %02x\n", *(pData + 13));
	
	//[14][FSCI:4][Protocol_Type:4]
	LOGD("FSCI : %x\n", *(pData + 14) >> 4);
	LOGD("Protocol_Type : %x\n", *(pData + 14) & 0x0f);
	
	//[15][FWI:4][ADC:2][FO:2]
	LOGD("FWI : %x\n", *(pData + 15) >> 4);
	LOGD("ADC : %x\n", (*(pData + 15) >> 2) & 0x03);
	LOGD("FO  : %x\n", *(pData + 15) & 0x03);
	//[??]optional
	
	//ATTRIB_RES
	//[16]Length
	for(int i=0; i<*(pData + 16); i++) {
		LOGD("[ATTRIB_RES]%02x\n", *(pData + 17 + i));
	}

	return true;
}

#endif /* HKNFCRW_USE_NFCB */
