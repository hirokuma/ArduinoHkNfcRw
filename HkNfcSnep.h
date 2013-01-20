#ifndef HK_NFCSNEP_H
#define HK_NFCSNEP_H

#include "HkNfcRule.h"
#if defined(HKNFCRW_USE_SNEPI) || defined(HKNFCRW_USE_SNEPT)

#include <stdint.h>
#include "HkNfcNdefMsg.h"

class HkNfcSnep {
public:
	enum Result {
		SUCCESS = 0,
		PROCESSING = 1,
		FAIL = 1
	};

	enum Mode {
		MD_INITIATOR,
		MD_TARGET
	};

public:
	static Result getResult();
	static bool putStart(Mode mode, const HkNfcNdefMsg* pMsg);
	static bool poll();
	static void stop();

#ifdef HKNFCRW_USE_SNEPI
private:
	static bool pollI();
	static void recvCbI(const void* pBuf, uint8_t len);
#endif

#ifdef HKNFCRW_USE_SNEPT
private:
	static bool pollT();
	static void recvCbT(const void* pBuf, uint8_t len);
#endif

private:
	HkNfcSnep();
	HkNfcSnep(const HkNfcSnep&);
	~HkNfcSnep();

private:
	enum Status {
		ST_INIT,
		
		ST_START_PUT,
		ST_PUT,
		ST_PUT_RESPONSE,

		ST_START_GET,
		ST_GET,
		ST_GET_RESPONSE,

		ST_SUCCESS,
		ST_ABORT
	};

private:
	static const HkNfcNdefMsg*	m_pMessage;
	static Mode				m_Mode;
	static Status			m_Status;
	static bool (*m_PollFunc)();
};


#endif

#endif /* HK_NFCSNEP_H */
