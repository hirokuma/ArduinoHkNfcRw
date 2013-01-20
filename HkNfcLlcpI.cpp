#include "HkNfcRule.h"
#ifdef HKNFCRW_USE_LLCPI

#include <cstring>
#include <cstdlib>

#include "HkNfcLlcpI.h"
#include "NfcPcd.h"
#include "misc.h"

#define LOG_TAG "HkNfcLlcpI"
#include "nfclog.h"


using namespace HkNfcRwMisc;

/**
 * LLCP開始
 *
 * @param[in]	mode		開始するDepMode
 * @return		成功/失敗
 */
bool HkNfcLlcpI::start(DepMode mode, void (*pRecvCb)(const void* pBuf, uint8_t len))
{
	LOGD("%s\n", __PRETTY_FUNCTION__);
	
	bool ret = HkNfcDep::startAsInitiator(mode, true);
	if(ret) {
		//PDU送信側
		m_bSend = true;
		m_LlcpStat = LSTAT_NOT_CONNECT;
		m_pRecvCb = pRecvCb;
	} else {
		killConnection();
	}

	return ret;
}


/**
 * LLCP(Initiator)終了要求
 *
 * @return		true:要求受け入れ
 */
bool HkNfcLlcpI::stopRequest()
{
	LOGD("%s(%d)\n", __PRETTY_FUNCTION__, m_LlcpStat);
	
	switch(m_LlcpStat) {
	case LSTAT_NOT_CONNECT:
	case LSTAT_CONNECTING:
		LOGD("==>LSTAT_DM\n");
		m_LlcpStat = LSTAT_DM;
		break;
	
	case LSTAT_NONE:
	case LSTAT_TERM:
	case LSTAT_DM:
	case LSTAT_WAIT_DM:
		break;
	
	case LSTAT_NORMAL:
	case LSTAT_BUSY:
		LOGD("==>LSTAT_TERM\n");
		m_LlcpStat = LSTAT_TERM;
		break;
	}
	
	return true;
}



/**
 * LLCP(Initiator)送信データ追加
 *
 * @param[in]	pBuf	送信データ。最大#LLCP_MIU[byte]
 * @param[in]	len		送信データ長。最大#LLCP_MIU.
 * @retval		true	送信データ受け入れ
 */
bool HkNfcLlcpI::addSendData(const void* pBuf, uint8_t len)
{
	LOGD("%s(%d)\n", __PRETTY_FUNCTION__, m_LlcpStat);
	
	bool b;
	b = HkNfcDep::addSendData(pBuf, len);
	
	return b;
}


/**
 * LLCP(Initiator)データ送信要求
 *
 * @retval		true	送信受け入れ
 */
bool HkNfcLlcpI::sendRequest()
{
	LOGD("%s(%d)\n", __PRETTY_FUNCTION__, m_LlcpStat);
	
	bool b;
	b = connect();
	
	return b;
}


/**
 * LLCP(Initiator)定期処理.
 * 開始後、やることがない間は #poll() を呼び出し続けること.
 * 基本的には、 #getDepMode() が #DEP_NONE になるまで呼び出し続ける.
 * 
 * @retval	true	#DEP_NONEになっていない
 * @retval	false	#DEP_NONEになった
 */
bool HkNfcLlcpI::poll()
{
	if(getDepMode() == DEP_NONE) {
		return false;
	}

	if(m_bSend) {
		//PDU送信時
		switch(m_LlcpStat) {
		case LSTAT_NONE:
			//
			break;
		
		case LSTAT_NOT_CONNECT:
			//CONNECT前はSYMMを投げる
			m_CommandLen = PDU_INFOPOS;
			LOGD("*");
			createPdu(PDU_SYMM);
			break;
		
		case LSTAT_CONNECTING:
			//CONNECT or CCはデータ設定済み
			if(m_CommandLen) {
				LOGD("send CONNECT or CC\n");
			}
			break;

		case LSTAT_NORMAL:
			//
			if(m_SendLen) {
				//送信データあり
				LOGD("send I(VR:%d / VS:%d)\n", m_ValueR, m_ValueS);
				m_CommandLen = PDU_INFOPOS + 1 + m_SendLen;
				createPdu(PDU_I);
				NfcPcd::commandBuf(PDU_INFOPOS) = (uint8_t)((m_ValueS << 4) | m_ValueR);
				hk_memcpy(NfcPcd::commandBuf() + PDU_INFOPOS + 1, m_SendBuf, m_SendLen);
				m_SendLen = 0;
				m_ValueS++;
			} else {
				m_CommandLen = PDU_INFOPOS + 1;
				createPdu(PDU_RR);
				NfcPcd::commandBuf(PDU_INFOPOS) = m_ValueR;		//N(R)
			}
			break;

		case LSTAT_TERM:
			//切断シーケンス
			m_CommandLen = PDU_INFOPOS;
			createPdu(PDU_DISC);
			LOGD("send DISC\n");
			break;
		
		case LSTAT_DM:
			//切断
			NfcPcd::commandBuf(PDU_INFOPOS) = NfcPcd::commandBuf(0);
			m_CommandLen = PDU_INFOPOS + 1;
			createPdu(PDU_DM);
			LOGD("send DM\n");
			break;
		}
		if(m_CommandLen == 0) {
			//SYMMでしのぐ
			m_CommandLen = PDU_INFOPOS;
			createPdu(PDU_SYMM);
			LOGD("*");
		}
		
		uint8_t len;
		startTimer(m_LinkTimeout);
		bool b = sendAsInitiator(NfcPcd::commandBuf(), m_CommandLen, NfcPcd::responseBuf(), &len);
		if(m_LlcpStat == LSTAT_DM) {
			//DM送信後は強制終了する
			LOGD("DM sent\n");
			stopAsInitiator();
			killConnection();
		} else if(b) {
			if(isTimeout()) {
				//相手から通信が返ってこない
				LOGE("Link timeout\n");
				m_bSend = true;
				m_LlcpStat = LSTAT_TERM;
				m_DSAP = SAP_MNG;
				m_SSAP = SAP_MNG;
			} else {
				if((m_LlcpStat == LSTAT_CONNECTING) && (m_LastSentPdu == PDU_CC)) {
					m_LlcpStat = LSTAT_NORMAL;
				}

				//受信は済んでいるので、次はPDU送信側になる
				m_bSend = true;
				m_CommandLen = 0;

				PduType type;
				uint8_t pdu = analyzePdu(NfcPcd::responseBuf(), len, &type);
			}
		} else {
			LOGE("error\n");
			//もうだめ
			killConnection();
		}
	}
	
	return true;
}

#endif /* HKNFCRW_USE_LLCPI */
