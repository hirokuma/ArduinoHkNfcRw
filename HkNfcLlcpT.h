#ifndef HK_NFCLLCPT_H
#define HK_NFCLLCPT_H

#include "HkNfcRule.h"
#ifdef HKNFCRW_USE_LLCPT

#ifndef HKNFCRW_USE_NFCDEP
#define HKNFCRW_USE_NFCDEP
#endif

#include <stdint.h>
#include "HkNfcDep.h"

class HkNfcLlcpT : public HkNfcDep {
public:
	static bool start(void (*pRecvCb)(const void* pBuf, uint8_t len));
	static bool stopRequest();
	static bool addSendData(const void* pBuf, uint8_t len);
	static bool sendRequest();

	static bool poll();

private:
	HkNfcLlcpT();
	HkNfcLlcpT(const HkNfcLlcpT&);
	~HkNfcLlcpT();
};

#endif /* HKNFCRW_USE_LLCPT */

#endif /* HK_NFCLLCPT_H */
