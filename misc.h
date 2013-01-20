#ifndef MISC_H
#define MISC_H

#include <stdint.h>

#if 0
#define h16(u16)		((uint8_t)(u16 >> 8))
#define l16(u16)		((uint8_t)u16)
#define ul16(h8,l8)		((uint16_t)((h8<<8) | l8))
#endif

namespace HkNfcRwMisc {

/// @addtogroup gp_utils	Utilities
/// @ingroup gp_NfcPcd
/// @{
/**
 * @brief	上位8bit取得
 *
 * 16bitの上位8bitを返す
 */
inline uint8_t h16(uint16_t u16) {
	return uint8_t(u16 >> 8);
}

/**
 *  @brief	下位8bit取得
 *
 * 16bitの下位8bitを返す
 */
inline uint8_t l16(uint16_t u16) {
	return uint8_t(u16 & 0x00ff);
}

/**
 *  @brief	8bitx2→16bit
 *
 * 16bit値の作成
 */
inline uint16_t hl16(uint8_t h8, uint8_t l8) {
	return uint16_t((h8 << 8) | l8);
}


void msleep(uint16_t msec);
void startTimer(uint16_t tmval);
bool isTimeout();

/// @}

}	//namespace

#if 1
void* hk_memcpy(void* dst, const void* src, uint16_t len);
void* hk_memset(void* dst, uint8_t dat, uint16_t len);
int hk_memcmp(const void *s1, const void *s2, uint16_t n);
uint8_t hk_strlen(const char* pStr);
#else
#define hk_memcpy	memcpy
#define hk_memset	memset
#define hk_memcmp	memcmp
#define hk_strlen	strlen
#endif

#endif // MISC_H
