#include "HkNfcRule.h"
#ifdef HKNFCRW_USE_LLCPT

#include <cstring>
#include <cstdlib>

#include "HkNfcLlcpT.h"
#include "NfcPcd.h"
#include "misc.h"

#define LOG_TAG "HkNfcLlcpT"
#include "nfclog.h"


using namespace HkNfcRwMisc;


/**
 * LLCP(Target)開始.
 * 呼び出すと、Initiatorから要求が来るまで戻らない.
 * 成功すると、ATR_RESの返信まで終わっている.
 *
 * @retval	true	開始成功
 * @retval	false	開始失敗
 */
bool HkNfcLlcpT::start(void (*pRecvCb)(const void* pBuf, uint8_t len))
{
	LOGD("%s\n", __PRETTY_FUNCTION__);
	
	bool ret = HkNfcDep::startAsTarget(true);
	if(ret) {
		LOGD("%s -- success\n", __PRETTY_FUNCTION__);
		
		//PDU受信側
		m_bSend = false;
		m_LlcpStat = LSTAT_NOT_CONNECT;
		m_pRecvCb = pRecvCb;

		startTimer(m_LinkTimeout);
	}
	
	return ret;
}


/**
 * LLCP(Target)終了要求
 *
 * @return		true:要求受け入れ
 */
bool HkNfcLlcpT::stopRequest()
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
 * LLCP(Target)送信データ追加
 *
 * @param[in]	pBuf	送信データ。最大#LLCP_MIU[byte]
 * @param[in]	len		送信データ長。最大#LLCP_MIU.
 * @retval		true	送信データ受け入れ
 */
bool HkNfcLlcpT::addSendData(const void* pBuf, uint8_t len)
{
	LOGD("%s(%d)\n", __PRETTY_FUNCTION__, m_LlcpStat);
	
	bool b;
	b = HkNfcDep::addSendData(pBuf, len);
	
	return b;
}


/**
 * LLCP(Target)データ送信要求
 *
 * @retval		true	送信受け入れ
 */
bool HkNfcLlcpT::sendRequest()
{
	LOGD("%s(%d)\n", __PRETTY_FUNCTION__, m_LlcpStat);
	
	bool b;
	b = connect();
	
	return b;
}


/**
 * LLCP(Target)定期処理.
 * 開始後、やることがない間は #poll() を呼び出し続けること.
 * 基本的には、 #getDepMode() が #DEP_NONE になるまで呼び出し続ける.
 * 
 * @retval	true	#DEP_NONEになっていない
 * @retval	false	#DEP_NONEになった
 */
bool HkNfcLlcpT::poll()
{
	if(getDepMode() == DEP_NONE) {
		return false;
	}

	if(!m_bSend) {
		//PDU受信側
		uint8_t len;
		bool b = recvAsTarget(NfcPcd::responseBuf(), &len);
		if(isTimeout()) {
			//相手から通信が返ってこない
			LOGE("Link timeout\n");
			m_bSend = true;
			m_LlcpStat = LSTAT_TERM;
			m_DSAP = SAP_MNG;
			m_SSAP = SAP_MNG;
		} else if(b) {
			PduType type;
			uint8_t pdu = analyzePdu(NfcPcd::responseBuf(), len, &type);
			//PDU送信側になる
			m_bSend = true;
		} else {
			//もうだめだろう
			LOGE("recv error\n");
			killConnection();
		}
	} else {
		//PDU送信側
		uint8_t len;
		switch(m_LlcpStat) {
		case LSTAT_NONE:
			//
			break;
		
		case LSTAT_NOT_CONNECT:
			//CONNECT前はSYMMを投げる
			m_CommandLen = PDU_INFOPOS;
			createPdu(PDU_SYMM);
			//LOGD("send SYMM\n");
			LOGD("*");
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
			LOGD("send DISC\n");
			m_CommandLen = PDU_INFOPOS;
			createPdu(PDU_DISC);
			break;
		
		case LSTAT_DM:
			//切断
			LOGD("send DM\n");
			NfcPcd::commandBuf(PDU_INFOPOS) = NfcPcd::commandBuf(0);
			m_CommandLen = PDU_INFOPOS + 1;
			createPdu(PDU_DM);
			break;
		}
		if(m_CommandLen == 0) {
			//SYMMでしのぐ
			m_CommandLen = PDU_INFOPOS;
			createPdu(PDU_SYMM);
		}
		bool b = respAsTarget(NfcPcd::commandBuf(), m_CommandLen);
		if(m_LlcpStat == LSTAT_DM) {
			//DM送信後は強制終了する
			LOGD("DM sent\n");
			killConnection();
		} else if(b) {
			if(m_LlcpStat == LSTAT_TERM) {
				//DISC送信後
				if((m_DSAP == 0) && (m_SSAP == 0)) {
					LOGD("fin : Link Deactivation\n");
					killConnection();
				} else {
					LOGD("wait DM\n");
					m_LlcpStat = LSTAT_WAIT_DM;

					//PDU受信側になる
					m_bSend = false;
					m_CommandLen = 0;
					
					startTimer(m_LinkTimeout);
				}
			} else {
				if((m_LlcpStat == LSTAT_CONNECTING) && (m_LastSentPdu == PDU_CC)) {
					m_LlcpStat = LSTAT_NORMAL;
				}
				//PDU受信側になる
				m_bSend = false;
				m_CommandLen = 0;
				
				startTimer(m_LinkTimeout);
			}
		} else {
			LOGE("send error\n");
			//もうだめ
			killConnection();
		}
	}
	
	return true;
}

#endif /* HKNFCRW_USE_LLCPT */
