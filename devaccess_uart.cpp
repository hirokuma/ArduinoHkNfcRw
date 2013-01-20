#include "Arduino.h"
#include "HardwareSerial.h"

#include "devaccess.h"
#include "nfclog.h"

// デバッグ設定
//#define DBG_WRITEDATA
//#define DBG_READDATA


/**
* @brief		RC-S620/S用のI/F
*/
namespace DevAccess {

/**
 * ポートオープン
 *
 * @retval	true	オープン成功
 */
bool open()
{
	Serial.begin(115200);

	return true;
}


/**
 * ポートクローズ
 */
void close()
{
	//Serial.end();		//終わりはないようだ
}


/**
 * ポート送信
 *
 * @param[in]	data		送信データ
 * @param[in]	len			dataの長さ
 * @return					送信したサイズ
 */
uint16_t write(const uint8_t* data, uint16_t len)
{
	//LOGD("[NfcPcd]_port_write");

#ifdef DBG_WRITEDATA
	LOGD("write(%d):", len);
	for(int i=0; i<len; i++) {
		LOGD("[W]%02x ", data[i]);
	}
#endif

	Serial.write(data, len);
	Serial.flush();

	return len;
}

/**
 * 受信
 *
 * @param[out]	data		受信バッファ
 * @param[in]	len			受信サイズ
 *
 * @return					受信したサイズ
 *
 * @attention	- len分のデータを受信するか失敗するまで処理がブロックされる。
 */
uint16_t read(uint8_t* data, uint16_t len)
{
	//LOGD("[NfcPcd]_port_read");
	uint16_t ret_len = 0;

	uint16_t nread = 0;
	while (nread < len) {
		if (Serial.available() > 0) {
			data[nread] = Serial.read();
			nread++;
		}
	}
	
	return nread;
}

}	//namespace DevAccess

