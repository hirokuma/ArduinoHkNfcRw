#include "HkNfcRule.h"
#ifdef HKNFCRW_USE_NFCF

#include "HkNfcRw.h"
#include "HkNfcF.h"
#include "NfcPcd.h"
#include "misc.h"

#define LOG_TAG "HkNfcF"
#include "nfclog.h"

#include <cstring>

using namespace HkNfcRwMisc;

namespace {
	const uint16_t kSC_BROADCAST = 0xffff;		///<
	const uint16_t kDEFAULT_TIMEOUT = 1000 * 2;
	const uint16_t kPUSH_TIMEOUT = 2100 * 2;
}


uint16_t		HkNfcF::m_SystemCode = kSC_BROADCAST;		///< システムコード
uint16_t		HkNfcF::m_SvcCode = SVCCODE_RW;

#ifdef QHKNFCRW_USE_FELICA
uint16_t		HkNfcF::m_syscode[16];
uint8_t			HkNfcF::m_syscode_num;
#endif	//QHKNFCRW_USE_FELICA


namespace {

	enum AccessModeType {
		AM_NORMAL = 0x00,
	};
	enum SCOrderType {
		SCO_NORMAL = 0x00,
	};
	inline uint16_t create_blocklist2(uint16_t blockNo, AccessModeType am=AM_NORMAL, SCOrderType sco=SCO_NORMAL)
	{
		return uint16_t(0x8000U | (am << 12) | (sco << 8) | blockNo);
	}
}


/**
 * pollingしたカードの解放
 */
void HkNfcF::release()
{
	m_SystemCode = kSC_BROADCAST;
}


/**
 * Polling
 *
 * @param[in]		systemCode		システムコード
 * @retval			true			取得成功
 * @retval			false			取得失敗
 *
 * @retval		true			成功
 * @retval		false			失敗
 *
 * @attention	- 取得失敗は、主にカードが認識できない場合である。
 */
bool HkNfcF::polling(uint16_t systemCode /* = 0xffff */)
{
	//InListPassiveTarget
	const uint8_t INLISTPASSIVETARGET[] = {
		0x02,				// 0x01:212Kbps  0x02:424Kbps
		0x00,
		0xff, 0xff,			// SystemCode
		0x01,				// opt:0x01...get SystemCode
		0x0f				// Time Slot
	};
	const uint8_t INLISTPASSIVETARGET_RES[] = {
		0x01,				// System Number
		0x14,				// length(opt:+2)
		0x01				// response code
	};

	bool ret;
	uint8_t responseLen = 0;
	uint8_t* pData;

	// 424Kbps
	hk_memcpy(NfcPcd::commandBuf(), INLISTPASSIVETARGET, sizeof(INLISTPASSIVETARGET));
	NfcPcd::commandBuf(2) = h16(systemCode);
	NfcPcd::commandBuf(3) = l16(systemCode);

	ret = NfcPcd::inListPassiveTarget(
				NfcPcd::commandBuf(), sizeof(INLISTPASSIVETARGET),
				&pData, &responseLen);
	if (!ret
	  || (hk_memcmp(&pData[3], INLISTPASSIVETARGET_RES, sizeof(INLISTPASSIVETARGET_RES)) != 0)) {
		LOGE("pollingF fail(424Kbps): ret=%d/len=%d\n", ret, responseLen);

		//212Kbps
		NfcPcd::commandBuf(0) = 0x01;
		ret = NfcPcd::inListPassiveTarget(
				NfcPcd::commandBuf(), sizeof(INLISTPASSIVETARGET),
				&pData, &responseLen);
		if (!ret
		  || (hk_memcmp(&pData[3], INLISTPASSIVETARGET_RES, sizeof(INLISTPASSIVETARGET_RES)) != 0)) {
			LOGE("pollingF fail(212Kbps): ret=%d/len=%d\n", ret, responseLen);
			m_SystemCode = kSC_BROADCAST;
			return false;
		}
	}
	//[0] d5
	//[1] 4b
	//[2] NbTg
	//[3] Tg

	NfcPcd::setNfcId(pData + 6, NfcPcd::NFCID2_LEN);
	m_SystemCode = (uint16_t)(*(pData + 22) << 8 | *(pData + 23));

	return true;
}


/**
 *
 */
bool HkNfcF::read(uint8_t* buf, uint8_t blockNo/*=0x00*/)
{
	uint8_t len;
	NfcPcd::commandBuf(0) = 16;
	NfcPcd::commandBuf(1) = 0x06;	// Read w/o Enc.
	hk_memcpy(NfcPcd::commandBuf() + 2, NfcPcd::nfcId(), NfcPcd::NFCID2_LEN);
	NfcPcd::commandBuf(10) = 0x01;				//サービス数
	NfcPcd::commandBuf(11) = l16(m_SvcCode);
	NfcPcd::commandBuf(12) = h16(m_SvcCode);
	NfcPcd::commandBuf(13) = 0x01;			//ブロック数
	uint16_t blist = create_blocklist2(blockNo);
	NfcPcd::commandBuf(14) = h16(blist);
	NfcPcd::commandBuf(15) = l16(blist);
	bool ret = NfcPcd::communicateThruEx(
					kDEFAULT_TIMEOUT,
					NfcPcd::commandBuf(), 16,
					NfcPcd::responseBuf(), &len);
	if (!ret || (NfcPcd::responseBuf(0) != 0x07)
	  || (hk_memcmp(NfcPcd::responseBuf() + 1, NfcPcd::nfcId(), NfcPcd::NFCID2_LEN) != 0)
	  || (NfcPcd::responseBuf(9) != 0x00)
	  || (NfcPcd::responseBuf(10) != 0x00)) {
		LOGE("read : ret=%d / %02x / %02x / %02x\n", ret, NfcPcd::responseBuf(0), NfcPcd::responseBuf(9), NfcPcd::responseBuf(10));
		return false;
	}
	//NfcPcd::responseBuf(11) == 0x01
	hk_memcpy(buf, &NfcPcd::responseBuf(12), 16);

	return true;
}


/**
 *
 */
bool HkNfcF::write(const uint8_t* buf, uint8_t blockNo/*=0x00*/)
{
	return false;
}


void HkNfcF::setServiceCode(uint16_t svccode)
{
	m_SvcCode = svccode;
}


#ifdef QHKNFCRW_USE_FELICA
/**
 *
 */
bool HkNfcF::reqSystemCode(uint8_t* pNums)
{
	// Request System Codeのテスト
	uint8_t len;

	NfcPcd::commandBuf(0) = 10;
	NfcPcd::commandBuf(1) = 0x0c;
	hk_memcpy(NfcPcd::commandBuf() + 2, NfcPcd::nfcId(), NfcPcd::NFCID2_LEN);
	bool ret = NfcPcd::communicateThruEx(
						kDEFAULT_TIMEOUT,
						NfcPcd::commandBuf(), 10,
						NfcPcd::responseBuf(), &len);
	if (!ret || (NfcPcd::responseBuf(0) != 0x0d)
	  || (hk_memcmp(NfcPcd::responseBuf() + 1, NfcPcd::nfcId(), NfcPcd::NFCID2_LEN) != 0)) {
		LOGE("req_sys_code : ret=%d\n", ret);
		return false;
	}

	*pNums = *(NfcPcd::responseBuf() + 9);

	m_syscode_num = *pNums;
	for(int i=0; i<m_syscode_num; i++) {
		m_syscode[i] = (uint16_t)(*(NfcPcd::responseBuf() + 10 + i * 2) << 8 | *(NfcPcd::responseBuf() + 10 + i * 2 + 1));
		LOGD("sys[%d] : %04x\n", i, m_syscode[i]);
	}

	return true;
}


/**
 *
 */
bool HkNfcF::searchServiceCode()
{
	// Search Service Code
	uint8_t len;
	uint16_t loop = 0x0000;
	NfcPcd::commandBuf(0) = 12;
	NfcPcd::commandBuf(1) = 0x0a;
	hk_memcpy(NfcPcd::commandBuf() + 2, NfcPcd::nfcId(), NfcPcd::NFCID2_LEN);

	do {
		NfcPcd::commandBuf(10) = l16(loop);
		NfcPcd::commandBuf(11) = h16(loop);
		bool ret = NfcPcd::communicateThruEx(
							kDEFAULT_TIMEOUT,
							NfcPcd::commandBuf(), 12,
							NfcPcd::responseBuf(), &len);
		if (!ret || (NfcPcd::responseBuf(0) != 0x0b)
		  || (hk_memcmp(NfcPcd::responseBuf() + 1, NfcPcd::nfcId(), NfcPcd::NFCID2_LEN) != 0)) {
			LOGE("searchServiceCode : ret=%d\n", ret);
			return false;
		}

		len -= 9;
		const uint8_t* p = &NfcPcd::responseBuf(9);
		if(len) {
			uint16_t svc = uint16_t((*(p+1) << 8) | *p);
#ifdef HKNFCRW_ENABLE_DEBUG
			uint16_t code = uint16_t((svc & 0xffc0) >> 6);
			uint8_t  attr = svc & 0x003f;
			if(len == 2) {
				LOGD("%04x(code:%04x : attr:%02x)\n", svc, code, attr);
			} else {
				LOGD("%04x(%s)\n", svc, (attr) ? "can" : "cannot");
			}
#endif

			//おしまいチェック
			if(svc == 0xffff) {
				break;
			}
			loop++;
		} else {
			break;
		}
	} while(true);

	return true;
}


/**
 * [FeliCa]PUSHコマンド
 *
 * @param[in]	data		PUSHデータ
 * @param[in]	dataLen		dataの長さ
 *
 * @retval		true			成功
 * @retval		false			失敗
 *
 * @attention	- dataはそのまま送信するため、上位層で加工しておくこと。
 */
bool HkNfcF::push(const uint8_t* data, uint8_t dataLen)
{
	int ret;
	uint8_t responseLen;

	LOGD("%s", __FUNCTION__);

	if (dataLen > 224) {
		LOGE("bad len\n");
		return false;
	}

	NfcPcd::commandBuf(0) = (uint8_t)(10 + dataLen);
	NfcPcd::commandBuf(1) = 0xb0;			//PUSH
	hk_memcpy(NfcPcd::commandBuf() + 2, NfcPcd::nfcId(), NfcPcd::NFCID2_LEN);
	NfcPcd::commandBuf(10) = dataLen;
	hk_memcpy(NfcPcd::commandBuf() + 11, data, dataLen);

	// xx:IDm
	// [cmd]b0 xx xx xx xx xx xx xx xx len (push data...)
	ret = NfcPcd::communicateThruEx(
						kPUSH_TIMEOUT,
						NfcPcd::commandBuf(), 10 + dataLen,
						NfcPcd::responseBuf(), &responseLen);
	if (!ret || (responseLen != 10) || (NfcPcd::responseBuf(0) != 0xb1) ||
	  (hk_memcmp(NfcPcd::responseBuf() + 1, NfcPcd::nfcId(), NfcPcd::NFCID2_LEN) != 0) ||
	  (NfcPcd::responseBuf(9) != dataLen)) {

		LOGE("push1 : ret=%d\n", ret);
		return false;
	}

	// xx:IDm
	// [cmd]a4 xx xx xx xx xx xx xx xx 00
	NfcPcd::commandBuf(0) = 11;
	NfcPcd::commandBuf(1) = 0xa4;			//inactivate? activate2?
	hk_memcpy(NfcPcd::commandBuf() + 2, NfcPcd::nfcId(), NfcPcd::NFCID2_LEN);
	NfcPcd::commandBuf(10) = 0x00;

	ret = NfcPcd::communicateThruEx(
						kDEFAULT_TIMEOUT,
						NfcPcd::commandBuf(), 11,
						NfcPcd::responseBuf(), &responseLen);
	if (!ret || (responseLen != 10) || (NfcPcd::responseBuf(0) != 0xa5) ||
	  (hk_memcmp(NfcPcd::responseBuf() + 1, NfcPcd::nfcId(), NfcPcd::NFCID2_LEN) != 0) ||
	  (NfcPcd::responseBuf(9) != 0x00)) {

		LOGE("push2 : ret=%d\n", ret);
		return false;
	}

	msleep(1000);

	return true;
}
#endif	//QHKNFCRW_USE_FELICA

#endif /* HKNFCRW_USE_NFCF */
