#ifndef NFCLOG_H
#define NFCLOG_H

//#define HKNFCRW_ENABLE_DEBUG

#ifdef HKNFCRW_ENABLE_DEBUG
#include <stdio.h>
#define LOGI	printf
#define LOGE	printf
#define LOGD	printf

#else
#define LOGI(...)
#define LOGE(...)
#define LOGD(...)

#endif	//HKNFCRW_ENABLE_DEBUG

#endif /* NFCLOG_H */
