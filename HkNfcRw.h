#ifndef QHKNFCRW_H
#define QHKNFCRW_H

#include <stdint.h>
#include <cstring>


/**
 * @class	HkNfcRw
 * @brief	NFCのR/W(Sony RC-S956系)アクセスクラス
 */
class HkNfcRw {
public:
	/**
	 * @enum	Type
	 * @brief	Polling時のNFCカードタイプ
	 */
	enum Type {
		NFC_NONE,		///< 未設定

#ifdef HKNFCRW_USE_NFCA
		NFC_A,			///< NFC-A
#endif

#ifdef HKNFCRW_USE_NFCB
		NFC_B,			///< NFC-B
#endif

#ifdef HKNFCRW_USE_NFCF
		NFC_F			///< NFC-F
#endif
	};


private:
	HkNfcRw();
	HkNfcRw(const HkNfcRw&);
	~HkNfcRw();


public:
	/// 選択解除
	static void release();
#if 0	//ここはNDEF対応してからじゃないと意味がなさそうだな
	/// データ読み込み
	virtual bool read(uint8_t* buf, uint8_t blockNo=0x00) { return false; }
	/// データ書き込み
	virtual bool write(const uint8_t* buf, uint8_t blockNo=0x00) { return false; }
#endif

public:
	/// オープン
	static bool open();
	/// クローズ
	static void close();

	static void reset();

public:
	/// @addtogroup gp_CardDetect	カード探索
	/// @{

	/// ターゲットの探索
	static Type detect(bool bNfcA, bool bNfcB, bool bNfcF);

	/**
	 * NFCID取得
	 *
	 * @param[out]	pBuf	[戻り値]保持するNFCID
	 * @return		NFCID長。0のときは未取得。
	 */
	static uint8_t getNfcId(uint8_t* pBuf);


	/**
	 * @brief NFCタイプの取得
	 *
	 * NFCタイプを取得する
	 *
	 * @return		Nfc::Type 参照
	 */
	static Type getType() {
		return m_Type;
	}
	/// @}


private:
	static Type			m_Type;				///< アクティブなNFCタイプ
};

#endif // QHKNFCRW_H
