/** @file	qnfcrw.cpp
 *	@brief	NFCアクセスのインターフェース実装.
 * 	@author	uenokuma@gmail.com
 */
#include "HkNfcRule.h"
#include "HkNfcRw.h"
#include "HkNfcA.h"
#include "HkNfcB.h"
#include "HkNfcF.h"
#include "NfcPcd.h"
#include "misc.h"

// ログ用
#define LOG_TAG "HkNfcRw"
#include "nfclog.h"

using namespace HkNfcRwMisc;

HkNfcRw::Type	HkNfcRw::m_Type = HkNfcRw::NFC_NONE;				///< アクティブなNFCタイプ



/**
 * NFCデバイスオープン
 *
 * @retval	true		成功
 * @retval	false		失敗
 *
 * @attention	- 初回に必ず呼び出すこと。
 * 				- falseの場合、呼び出し側が#close()すること。
 * 				- 終了時には#close()を呼び出すこと。
 *
 * @note		- 呼び出しただけでは搬送波を出力しない。
 */
bool HkNfcRw::open()
{
	LOGD("%s\n", __PRETTY_FUNCTION__);

	if(!NfcPcd::portOpen()) {
		return false;
	}

	bool ret = NfcPcd::init();
	if(!ret) {
		close();
	}
	
	return ret;
}


/**
 * NFCデバイスをクローズする。
 *
 * @attention	- 使用が終わったら、必ず呼び出すこと。
 * 				- 呼び出さない場合、搬送波を出力したままとなる。
 * 				- 呼び出し後、再度使用する場合は#open()を呼び出すこと。
 */
void HkNfcRw::close()
{
	if(NfcPcd::isOpened()) {
		reset();
		NfcPcd::portClose();
	}
}


void HkNfcRw::reset()
{
	NfcPcd::rfOff();
	NfcPcd::reset();
}

/**
 * 選択しているカードを内部的に解放する。
 *
 * @attention	RLS_REQ/RLS_RESとは関係ない
 */
void HkNfcRw::release()
{
	NfcPcd::clrNfcId();
	m_Type = NFC_NONE;
}


/**
 * ターゲットの探索
 *
 * @param[in]	pNfcA	NfcA(null時は探索しない)
 * @param[in]	pNfcB	NfcB(null時は探索しない)
 * @param[in]	pNfcF	NfcF(null時は探索しない)
 *
 * @retval	true		成功
 * @retval	false		失敗
 *
 * @note		- #open()後に呼び出すこと。
 * 				- アクティブなカードが変更された場合に呼び出すこと。
 */
HkNfcRw::Type HkNfcRw::detect(bool bNfcA, bool bNfcB, bool bNfcF)
{
	m_Type = NFC_NONE;

#ifdef HKNFCRW_USE_NFCF
	if(bNfcF) {
        bool ret = HkNfcF::polling();
		if(ret) {
			LOGD("PollingF\n");
			m_Type = NFC_F;
			return m_Type;
		}
	}
#endif

#ifdef HKNFCRW_USE_NFCA
	if(bNfcA) {
        bool ret = HkNfcA::polling();
		if(ret) {
			LOGD("PollingA\n");
			m_Type = NFC_A;
			return m_Type;
		}
	}
#endif

#ifdef HKNFCRW_USE_NFCB
	if(bNfcB) {
        bool ret = HkNfcB::polling();
		if(ret) {
			LOGD("PollingB\n");
			m_Type = NFC_B;
			return m_Type;
		}
	}
	LOGD("Detect fail\n");
#endif

	return NFC_NONE;
}


/**
 * @brief UIDの取得
 *
 * UIDを取得する
 *
 * @param[out]	pBuf		UID値
 * @return					pBufの長さ
 *
 * @note		- 戻り値が0の場合、UIDは未取得
 */
uint8_t HkNfcRw::getNfcId(uint8_t* pBuf) {
	hk_memcpy(pBuf, NfcPcd::nfcId(), NfcPcd::nfcIdLen());
	return NfcPcd::nfcIdLen();
}
