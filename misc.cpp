#include "Arduino.h"
#include "nfclog.h"
#include "misc.h"

namespace HkNfcRwMisc {

static uint16_t s_Timeout = 0;
static unsigned long s_Time = 0;

/**
 *  @brief	ミリ秒スリープ
 *
 * @param	msec	待ち時間[msec]
 */
void msleep(uint16_t msec)
{
	delay(msec);
}


/**
 * タイマ開始
 *
 * @param[in]	tmval	タイムアウト時間[msec]
 */
void startTimer(uint16_t tmval)
{
	s_Time = millis();
	s_Timeout = tmval;
}


/**
 *  タイムアウト監視
 * 
 * @retval	true	タイムアウト発生
 */
bool isTimeout()
{
	if(s_Timeout == 0) {
		return false;
	}

	return ((millis() - s_Time) >= s_Timeout) ? true : false;
}

}	//namespace HkNfcRwMisc


void* hk_memcpy(void* dst, const void* src, uint16_t len)
{
	uint8_t * pdst = (uint8_t *)dst;
	const uint8_t* psrc = (const uint8_t *)src;
	
	while(len--) {
		*pdst++ = *psrc++;
	}
	
	return dst;
}

void* hk_memset(void* dst, uint8_t dat, uint16_t len)
{
	uint8_t * pdst = (uint8_t *)dst;
	
	while(len--) {
		*pdst++ = dat;
	}
	
	return dst;
}

int hk_memcmp(const void *s1, const void *s2, uint16_t n)
{
	const uint8_t* p1 = (const uint8_t *)s1;
	const uint8_t* p2 = (const uint8_t *)s2;

	while(n--) {
		if(*p1++ != *p2++) {
			return 1;
		}
	}
	
	return 0;
}

uint8_t hk_strlen(const char* pStr)
{
	uint8_t len = 0;
	
	while(*pStr) {
		pStr++;
		len++;
	}
	
	return len;
}
