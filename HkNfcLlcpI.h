#ifndef HK_NFCLLCPI_H
#define HK_NFCLLCPI_H

#include "HkNfcRule.h"
#ifdef HKNFCRW_USE_LLCPI

#ifndef HKNFCRW_USE_NFCDEP
#define HKNFCRW_USE_NFCDEP
#endif

#include <stdint.h>
#include "HkNfcDep.h"

class HkNfcLlcpI : public HkNfcDep {
public:
	static bool start(DepMode mode, void (*pRecvCb)(const void* pBuf, uint8_t len));
	static bool stopRequest();
	static bool addSendData(const void* pBuf, uint8_t len);
	static bool sendRequest();

	static bool poll();

private:
	HkNfcLlcpI();
	HkNfcLlcpI(const HkNfcLlcpI&);
	~HkNfcLlcpI();
};

#endif /* HKNFCRW_USE_LLCPI */

#endif /* HK_NFCLLCP_INITIATOR_H */
