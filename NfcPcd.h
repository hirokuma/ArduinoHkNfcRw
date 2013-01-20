/**
 * @file	nfcpcd.h
 * @brief	NFCのPCD(Proximity Coupling Device)アクセス用ヘッダ
 */

#ifndef NFCPCD_H
#define NFCPCD_H

#include "HkNfcRw.h"
#include "misc.h"

/**
 * @class		NfcPcd
 * @brief		NFCのPCDアクセスクラス
 * @defgroup	gp_NfcPcd	NfcPcdクラス
 *
 * NFCのPCDにアクセスするPHY部
 */
class NfcPcd {
public:
	static const uint16_t DATA_MAX = 265;		///< データ部最大長

	static const uint8_t NFCID2_LEN = 8;		///< NFCID2サイズ
	static const uint8_t NFCID3_LEN = 10;		///< NFCID3サイズ

	/// 最大UID長(NFC-Bでの値)
	static const uint8_t MAX_NFCID_LEN = 12;


	/// @enum	ActPass
	/// @brief	inJumpForDep()参照
	enum ActPass {
		AP_PASSIVE	= 0x00,		///< パッシブモード(受動通信)
        AP_ACTIVE	= 0x01		///< アクティブモード(能動通信)
	};

	/// @enum	BaudRate
	/// @brief	inJumpForDep()参照
	enum BaudRate {
		BR_106K		= 0x00,		///< 106kbps(MIFAREなど)
		BR_212K		= 0x01,		///< 212kbps(FeliCa)
        BR_424K		= 0x02		///< 424kbps(FeliCa)
	};

	/// @struct	DepInitiatorParam
	/// @brief	NFC-DEPイニシエータパラメータ
	struct DepInitiatorParam {
		ActPass Ap;						///< Active/Passive
		BaudRate Br;					///< 通信速度
		const uint8_t*		pNfcId3;	///< NFCID3(不要ならnull)
		const uint8_t*		pGb;		///< GeneralBytes(GbLenが0:未使用)
		uint8_t				GbLen;		///< pGbサイズ(不要なら0)
		uint8_t*			pResponse;		///< [out]Targetからの戻り値(不要なら0)
		uint8_t				ResponseLen;	///< [out]pResponseのサイズ(不要なら0)
	};
	
	/// @struct	TargetParam
	/// @brief	ターゲットパラメータ
	struct TargetParam {
//		uint16_t			SystemCode;	///< [NFC-F]システムコード
//		const uint8_t*		pUId;		///< [NFC-A]UID(3byte)。
//										///  UID 4byteの先頭は強制的に0x04。
//										///  null時には乱数生成する。
//		const uint8_t*		pIDm;		///< [NFC-F]IDm
//										///  null時には6byteを乱数生成する(先頭は01fe)。
		const uint8_t*		pGb;		///< GeneralBytes(GbLenが0:未使用)
		uint8_t				GbLen;		///< pGbサイズ(不要なら0)
		uint8_t*			pCommand;		///< [out]Initiatorからの送信データ(不要なら0)
		uint8_t				CommandLen;		///< [out]pCommandのサイズ(不要なら0)
	};

private:
	NfcPcd();
	NfcPcd(const NfcPcd&);
	~NfcPcd();

public:
	/// @addtogroup gp_port	Device Port Control
	/// @ingroup gp_NfcPcd
	/// @{

	/// オープン
	static bool portOpen();
	/// オープン済みかどうか
	static bool isOpened() { return m_bOpened; }
	/// クローズ
	static void portClose();
	/// @}


public:
	/// @addtogroup gp_commoncmd	Common Command
	/// @ingroup gp_NfcPcd
	/// @{

	/// デバイス初期化
	static bool init();
	/// RF出力停止
	static bool rfOff();
	/// RFConfiguration
	static bool rfConfiguration(uint8_t cmd, const uint8_t* pCommand, uint8_t CommandLen);
	/// Reset
	static bool reset();
	/// Diagnose
	static bool diagnose(
			uint8_t cmd, const uint8_t* pCommand, uint8_t CommandLen,
			uint8_t* pResponse, uint8_t* pResponseLen);
	/// SetParameters
	static bool setParameters(uint8_t val);
	/// WriteRegister
	static bool writeRegister(const uint8_t* pCommand, uint8_t CommandLen);
	/// GetFirmware
	static const int GF_IC = 0;				///< GetFirmware:IC
	static const int GF_VER = 1;			///< GetFirmware:Ver
	static const int GF_REV = 2;			///< GetFirmware:Rev
	static const int GF_SUPPORT = 3;		///< GetFirmware:Support
	static bool getFirmwareVersion(uint8_t* pResponse);
	/// GetGeneralStatus
	static const int GGS_ERR = 0;			///< GetGeneralStatus:??
	static const int GGS_FIELD = 1;			///< GetGeneralStatus:??
	static const int GGS_NBTG = 2;			///< GetGeneralStatus:??
	static const int GGS_TG = 3;			///< GetGeneralStatus:??
	static const int GGS_TXMODE = 4;		///< GetGeneralStatus:??
	static const int GGS_TXMODE_DEP = 0x03;			///< GetGeneralStatus:DEP
	static const int GGS_TXMODE_FALP = 0x05;		///< GetGeneralStatus:FALP
	static bool getGeneralStatus(uint8_t* pResponse);
	/// CommunicateThruEX
	static bool communicateThruEx();
	/// CommunicateThruEX
	static bool communicateThruEx(
			const uint8_t* pCommand, uint8_t CommandLen,
			uint8_t* pResponse, uint8_t* pResponseLen);
	/// CommunicateThruEX
	static bool communicateThruEx(
			uint16_t Timeout,
			const uint8_t* pCommand, uint8_t CommandLen,
			uint8_t* pResponse, uint8_t* pResponseLen);
	/// @}


	/// @addtogroup gp_initiatorcmd	Initiator Command
	/// @ingroup gp_NfcPcd
	/// @{

	/// InJumpForDEP or ImJumpForPSL
private:
	static bool _inJump(uint8_t Cmd, DepInitiatorParam* pParam);

public:
	/// InJumpForDEP
	static bool inJumpForDep(DepInitiatorParam* pParam);
	/// InJumpForPSL
	static bool inJumpForPsl(DepInitiatorParam* pParam);
	/// InListPassiveTarget
	static bool inListPassiveTarget(
			const uint8_t* pInitData, uint8_t InitLen,
			uint8_t** ppTgData, uint8_t* pTgLen);
	/// InDataExchange
	static bool inDataExchange(
			const uint8_t* pCommand, uint8_t CommandLen,
			uint8_t* pResponse, uint8_t* pResponseLen,
			bool bCoutinue=false);
	/// InCommunicateThru
	static bool inCommunicateThru(
			const uint8_t* pCommand, uint8_t CommandLen,
			uint8_t* pResponse, uint8_t* pResponseLen);
	/// @}


	/// @addtogroup gp_targetcmd	Target Command
	/// @ingroup gp_NfcPcd
	/// @{

	/// TgInitAsTarget
	static bool tgInitAsTarget(TargetParam* pParam);
	/// TgSetGeneralBytes
	static bool tgSetGeneralBytes(const TargetParam* pParam);
	/// TgResponseToInitiator
	static bool tgResponseToInitiator(
			const uint8_t* pData, uint8_t DataLen,
			uint8_t* pResponse=0, uint8_t* pResponseLen=0);
	/// TgGetInitiatorCommand
	static bool tgGetInitiatorCommand(uint8_t* pResponse, uint8_t* pResponseLen);
	/// TgGetData
	static bool tgGetData(uint8_t* pCommand, uint8_t* pCommandLen);
	/// TgSetData
	static bool tgSetData(const uint8_t* pResponse, uint8_t ResponseLen);
	/// InRelease
	static bool inRelease();
	/// @}


	/// @addtogroup gp_buf		shared buffer
	/// @ingroup gp_NfcPcd
	/// @{
public:
	/**
	 * コマンドバッファ取得.
	 * アドレスを直接取得するので、あまりよろしくないが、いい方法が思いつくまでこのままとする.
	 * s_SendBufと統合してしまいたいが、後回し.
	 *
	 * @return		コマンドバッファ(サイズ:DATA_MAX)
	 */
	static uint8_t* commandBuf() { return s_CommandBuf; }

	/**
	 * コマンドバッファ取得(インデックス指定).
	 * #commandBuf()[idx]と同じ.
	 * 左辺値可能.
	 *
	 * @param[in]	idx		インデックス
	 * @return		コマンドバッファ(1byte)
	 */
	static uint8_t& commandBuf(uint32_t idx) { return s_CommandBuf[idx]; }

	/**
	 * レスポンスバッファ取得.
	 * アドレスを直接取得するので、あまりよろしくないが、いい方法が思いつくまでこのままとする.
	 * s_ResponseBufと統合してしまいたいが、後回し.
	 *
	 * @return		レスポンスバッファ(サイズ:DATA_MAX)
	 */
	static uint8_t* responseBuf() { return s_ResponseBuf; }

	/**
	 * レスポンスバッファ取得(インデックス指定).
	 * #responseBuf()[idx]と同じ.
	 * 左辺値可能.
	 *
	 * @param[in]	idx		インデックス
	 * @return		レスオンスバッファ(1byte)
	 */
	static uint8_t& responseBuf(uint32_t idx) { return s_ResponseBuf[idx]; }
	/// @}


	/// @addtogroup gp_nfcid	NFCID
	/// @ingroup gp_NfcPcd
	/// @{
public:
	/**
	 * 保持しているNFCIDサイズ取得.
	 * pollingによって更新される.
	 * clrNfcId()によって0クリアされる.
	 *
	 * @return		現在のNFCID長
	 */
	static uint8_t nfcIdLen() { return m_NfcIdLen; }

	/**
	 * 保持しているNFCID取得
	 * pollingによって更新される.
	 * clrNfcId()によって0クリアされる.
	 * 長さは #nfcIdLen() で取得すること.
	 *
	 * @return	NFCID
	 */
	static const uint8_t* nfcId() { return m_NfcId; }

	/**
	 * 保持しているNFCIDの設定.
	 *
	 * @param[in]	pNfcId		設定するNFCID
	 * @param[in]	len			設定するNFCID長
	 */
	static void setNfcId(const uint8_t* pNfcId, uint8_t len) { hk_memcpy(m_NfcId, pNfcId, len); m_NfcIdLen = len; }

	/**
	 * 保持しているNFCIDのクリア
	 */
	static void clrNfcId() { m_NfcIdLen = 0; hk_memset(m_NfcId, 0x00, MAX_NFCID_LEN); }
	/// @}


private:
	/// @addtogroup gp_comm		Communicate to Device
	/// @ingroup gp_NfcPcd
	/// @{

	/// パケット送受信
	static bool sendCmd(
			const uint8_t* pCommand, uint16_t CommandLen,
			uint8_t* pResponse, uint16_t* pResponseLen,
			bool bRecv=true);
	/// レスポンス受信
	static bool recvResp(uint8_t* pResponse, uint16_t* pResponseLen, uint8_t CmdCode=0xff);
	/// ACK送信
	static void sendAck();
	/// @}


private:
	static bool		m_bOpened;					///< オープンしているかどうか
	static uint8_t	s_CommandBuf[DATA_MAX];		///< PCDへの送信バッファ
	static uint8_t	s_ResponseBuf[DATA_MAX];	///< PCDからの受信バッファ
	static uint8_t	m_NfcId[MAX_NFCID_LEN];		///< 取得したNFCID
	static uint8_t	m_NfcIdLen;					///< 取得済みのNFCID長。0の場合は未取得。
};

#endif /* NFCPCD_H */
