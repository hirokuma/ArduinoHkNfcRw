#include "HkNfcRule.h"
#include "HkNfcSnep.h"
#include "HkNfcLlcpI.h"
#include "HkNfcLlcpT.h"
#include "NfcPcd.h"


const HkNfcNdefMsg*		HkNfcSnep::m_pMessage = 0;
HkNfcSnep::Mode			HkNfcSnep::m_Mode = HkNfcSnep::MD_INITIATOR;
HkNfcSnep::Status		HkNfcSnep::m_Status = HkNfcSnep::ST_INIT;
bool					(*HkNfcSnep::m_PollFunc)();


namespace {
	const uint8_t SNEP_SUCCESS = 0x81;
}


HkNfcSnep::Result HkNfcSnep::getResult()
{
	switch(m_Status) {
	case ST_INIT:
	case ST_SUCCESS:
		return SUCCESS;
	
	case ST_ABORT:
		return FAIL;
	
	default:
		return PROCESSING;
	}
}


/**
 * SNEP PUT開始.
 * NDEFメッセージはPUT完了まで保持するので、解放したり書き換えたりしないこと.
 *
 * @param[in]	mode	モード
 * @param[in]	pMsg	NDEFメッセージ(送信が終わるまで保持する)
 * @return		開始成功/失敗
 */
bool HkNfcSnep::putStart(Mode mode, const HkNfcNdefMsg* pMsg)
{
	if((m_pMessage != 0) || (pMsg == 0)) {
		return false;
	}
	m_Status = ST_START_PUT;
	m_pMessage = pMsg;

	if(mode == MD_INITIATOR) {
		m_Mode = MD_INITIATOR;
#ifdef HKNFCRW_USE_SNEPI
		m_PollFunc = HkNfcSnep::pollI;
#endif
	} else {
		m_Mode = MD_TARGET;
#ifdef HKNFCRW_USE_SNEPT
		m_PollFunc = HkNfcSnep::pollT;
#endif
	}

	return true;
}

bool HkNfcSnep::poll()
{
	return (*m_PollFunc)();
}

void HkNfcSnep::stop()
{
	if(m_Mode == MD_INITIATOR) {
#ifdef HKNFCRW_USE_SNEPI
		HkNfcLlcpI::stopRequest();
		HkNfcDep::stopAsInitiator();
#endif
	} else {
#ifdef HKNFCRW_USE_SNEPT
		HkNfcLlcpT::stopRequest();
#endif
	}
}


#ifdef HKNFCRW_USE_SNEPI
bool HkNfcSnep::pollI()
{
	bool b = false;

	switch(m_Status) {
	case ST_START_PUT:
		b = HkNfcLlcpI::start(HkNfcLlcpI::PSV_424K, HkNfcSnep::recvCbI);
		if(b) {
			uint8_t snep_head[6];
			snep_head[0] = 0x10;
			snep_head[1] = 0x02;	//PUT
			snep_head[2] = (uint8_t)((m_pMessage->Length) >> 24);
			snep_head[3] = (uint8_t)((m_pMessage->Length) >> 16);
			snep_head[4] = (uint8_t)((m_pMessage->Length) >> 8);
			snep_head[5] = (uint8_t)(m_pMessage->Length);
			b = HkNfcLlcpI::addSendData(snep_head, sizeof(snep_head));
			b = b && HkNfcLlcpI::addSendData(m_pMessage->Data, m_pMessage->Length);
		}
		if(b) {
			m_Status = ST_PUT;
		} else {
			m_Status = ST_ABORT;
			HkNfcLlcpI::stopRequest();
		}
		break;

	case ST_PUT:
		b = HkNfcLlcpI::sendRequest();
		if(b) {
			m_Status = ST_PUT_RESPONSE;
		} else {
			m_Status = ST_ABORT;
			HkNfcLlcpI::stopRequest();
		}
		break;

	case ST_PUT_RESPONSE:
	case ST_SUCCESS:
	case ST_ABORT:
		b = HkNfcLlcpI::poll();
		break;
	}

	if(!b) {
		m_pMessage = 0;
	}

	return b;
}


void HkNfcSnep::recvCbI(const void* pBuf, uint8_t len)
{
	const uint8_t* pData = reinterpret_cast<const uint8_t*>(pBuf);

	switch(m_Status) {
	case ST_PUT_RESPONSE:
		//PUT後の応答
		if(pData[1] == SNEP_SUCCESS) {
			m_Status = ST_SUCCESS;
		} else {
			m_Status = ST_ABORT;
		}
		HkNfcLlcpI::stopRequest();
		break;
	}
}
#endif


#ifdef HKNFCRW_USE_SNEPT
bool HkNfcSnep::pollT()
{
	bool b = false;

	switch(m_Status) {
	case ST_START_PUT:
		b = HkNfcLlcpT::start(HkNfcSnep::recvCbT);
		if(b) {
			uint8_t snep_head[6];
			snep_head[0] = 0x10;
			snep_head[1] = 0x02;	//PUT
			snep_head[2] = (uint8_t)((m_pMessage->Length) >> 24);
			snep_head[3] = (uint8_t)((m_pMessage->Length) >> 16);
			snep_head[4] = (uint8_t)((m_pMessage->Length) >> 8);
			snep_head[5] = (uint8_t)(m_pMessage->Length);
			b = HkNfcLlcpT::addSendData(snep_head, sizeof(snep_head));
			b = b && HkNfcLlcpT::addSendData(m_pMessage->Data, m_pMessage->Length);
		}
		if(b) {
			m_Status = ST_PUT;
		} else {
			m_Status = ST_ABORT;
			HkNfcLlcpT::stopRequest();
		}
		break;

	case ST_PUT:
		b = HkNfcLlcpT::sendRequest();
		if(b) {
			m_Status = ST_PUT_RESPONSE;
		} else {
			m_Status = ST_ABORT;
			HkNfcLlcpT::stopRequest();
		}
		break;
	case ST_PUT_RESPONSE:
	case ST_SUCCESS:
	case ST_ABORT:
		b = HkNfcLlcpT::poll();
		break;
	}

	if(!b) {
		m_pMessage = 0;
	}
	return b;
}


void HkNfcSnep::recvCbT(const void* pBuf, uint8_t len)
{
	const uint8_t* pData = reinterpret_cast<const uint8_t*>(pBuf);

	switch(m_Status) {
	case ST_PUT_RESPONSE:
		//PUT後の応答
		if(pData[1] == SNEP_SUCCESS) {
			m_Status = ST_SUCCESS;
		} else {
			m_Status = ST_ABORT;
		}
		HkNfcLlcpT::stopRequest();
		break;
	}
}
#endif
