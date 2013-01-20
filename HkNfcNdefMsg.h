#ifndef HK_NFCNDEFMSG_H
#define HK_NFCNDEFMSG_H

#include <stdint.h>


struct HkNfcNdefMsg {
	uint8_t		Data[256];
	uint16_t	Length;
};


#endif /* HK_NFCNDEFMSG_H */
