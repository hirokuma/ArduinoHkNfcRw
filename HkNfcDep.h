#ifndef HK_NFCDEP_H
#define HK_NFCDEP_H

#include "HkNfcRule.h"
#ifdef HKNFCRW_USE_NFCDEP

#include <stdint.h>

/**
 * @class		HkNfcDep
 * @brief		NFC-DEPアクセス
 * @defgroup	gp_NfcDep	NfcDepクラス
 */
class HkNfcDep {
public:
	friend class HkNfcLlcpI;
	friend class HkNfcLlcpT;

private:
	static const uint32_t _ACT = 0x80000000;		///< Active
	static const uint32_t _PSV = 0x00000000;		///< Passive
	static const uint32_t _AP_MASK= 0x80000000;		///< Active/Passive用マスク
	
	static const uint32_t _BR106K = 0x40000000;		///< 106kbps
	static const uint32_t _BR212K = 0x20000000;		///< 212kbps
	static const uint32_t _BR424K = 0x10000000;		///< 424kbps
	static const uint32_t _BR_MASK= 0x70000000;		///< Baudrate用マスク
	

public:
	/**
	 * @enum	HkNfcDep::DepMode
	 * 
	 * DEPでの通信モード
	 */
	enum DepMode {
		DEP_NONE = 0x00000000,			///< DEP開始前(取得のみ)
		
		ACT_106K = _ACT | _BR106K,		///< 106kbps Active
		PSV_106K = _PSV | _BR106K,		///< 106kbps Passive
		
		ACT_212K = _ACT | _BR212K,		///< 212kbps Active
		PSV_212K = _PSV | _BR212K,		///< 212kbps Passive
		
		ACT_424K = _ACT | _BR424K,		///< 424kbps Active
		PSV_424K = _PSV | _BR424K,		///< 424kbps Passive
	};

public:
	/**
	 * @enum	HkNfcDep::PduType
	 * 
	 * 	PDU種別
	 */
	enum PduType {
		PDU_SYMM	= 0x00,			///< SYMM
		PDU_PAX		= 0x01,			///< PAX
		PDU_AGF		= 0x02,			///< AGF
		PDU_UI		= 0x03,			///< UI
		PDU_CONN	= 0x04,			///< CONNECT
		PDU_DISC	= 0x05,			///< DISC
		PDU_CC		= 0x06,			///< CC
		PDU_DM		= 0x07,			///< DM
		PDU_FRMR	= 0x08,			///< FRMR
		PDU_RESV1	= 0x09,			///< -
		PDU_RESV2	= 0x0a,			///< -
		PDU_RESV3	= 0x0b,			///< -
		PDU_I		= 0x0c,			///< I
		PDU_RR		= 0x0d,			///< RR
		PDU_RNR		= 0x0e,			///< RNR
		PDU_RESV4	= 0x0f,			///< -
		PDU_LAST = PDU_RESV4,		///< -

		PDU_NONE	= 0xff			///< PDU範囲外
	};

	/**
	 * @enum	HkNfcDep::LlcpStatus
	 *
	 * LLCP状態
	 */
	enum LlcpStatus {
		LSTAT_NONE,			///< 未接続
		LSTAT_NOT_CONNECT,	///< ATR交換後、CONNECT前
		LSTAT_CONNECTING,	///< CONNECT要求
		LSTAT_NORMAL,		///< CONNECT/CC交換後
		LSTAT_BUSY,			///< Receiver Busy
		LSTAT_TERM,			///< Connection Termination
							// 送信する番になったら、DISCを送信し、#LSTAT_DMに遷移.
							// 受信する場合は、特に何もしない.
		LSTAT_DM,			///< DM送信待ち
							// 送信する番になったら、DMを送信し、#LSTAT_NONEに遷移.
							// 受信する場合は、特に何もしない.
		LSTAT_WAIT_DM,		///< DM受信待ち
							// DMを受信したら、#LSTAT_NONEに遷移.
							// DM以外の受信は破棄
							// 送信は破棄
	};

	static const uint8_t SAP_MNG = 0;		///< LLC Link Management
	static const uint8_t SAP_SDP = 1;		///< SDP
	static const uint8_t SAP_SNEP = 4;		///< SNEP
	
	static const uint8_t LLCP_MIU = 128;	///< MIU
	
private:
	static const uint8_t PDU_INFOPOS = 2;		///< PDUパケットのInformation開始位置


private:
	HkNfcDep();
	HkNfcDep(const HkNfcDep&);
	~HkNfcDep();


	/// @addtogroup gp_depinit	NFC-DEP(Initiator)
	/// @ingroup gp_NfcDep
	/// @{
public:
	/// InJumpForDEP
	static bool startAsInitiator(DepMode mode, bool bLlcp = true);
	/// InDataExchange
	static bool sendAsInitiator(
			const void* pCommand, uint8_t CommandLen,
			void* pResponse, uint8_t* pResponseLen);
	/// RLS_REQ
	static bool stopAsInitiator();
	/// @}


	/// @addtogroup gp_depinit	NFC-DEP(Target)
	/// @ingroup gp_NfcDep
	/// @{
public:
	/// TgInitTarget, TgSetGeneralBytes
	static bool startAsTarget(bool bLlcp=true);
	/// TgGetData
	static bool recvAsTarget(void* pCommand, uint8_t* pCommandLen);
	/// TgSetData
	static bool respAsTarget(const void* pResponse, uint8_t ResponseLen);
	/// @}


	/// @addtogroup gp_depstat	Common
	/// @ingroup gp_NfcDep
	/// @{
public:
	static void close();
	static DepMode getDepMode() { return m_DepMode; }
	/// @}


	/// @addtogroup gp_llcp		LLCP PDU
	/// @ingroup gp_NfcDep
	/// @{
protected:
	static uint8_t analyzePdu(const uint8_t* pBuf, uint8_t len, PduType* pResPdu);
	static uint8_t analyzeSymm(const uint8_t* pBuf, uint8_t len, uint8_t dsap, uint8_t ssap);
	static uint8_t analyzePax(const uint8_t* pBuf, uint8_t len, uint8_t dsap, uint8_t ssap);
	static uint8_t analyzeAgf(const uint8_t* pBuf, uint8_t len, uint8_t dsap, uint8_t ssap);
	static uint8_t analyzeUi(const uint8_t* pBuf, uint8_t len, uint8_t dsap, uint8_t ssap);
	static uint8_t analyzeConn(const uint8_t* pBuf, uint8_t len, uint8_t dsap, uint8_t ssap);
	static uint8_t analyzeDisc(const uint8_t* pBuf, uint8_t len, uint8_t dsap, uint8_t ssap);
	static uint8_t analyzeCc(const uint8_t* pBuf, uint8_t len, uint8_t dsap, uint8_t ssap);
	static uint8_t analyzeDm(const uint8_t* pBuf, uint8_t len, uint8_t dsap, uint8_t ssap);
	static uint8_t analyzeFrmr(const uint8_t* pBuf, uint8_t len, uint8_t dsap, uint8_t ssap);
	static uint8_t analyzeI(const uint8_t* pBuf, uint8_t len, uint8_t dsap, uint8_t ssap);
	static uint8_t analyzeRr(const uint8_t* pBuf, uint8_t len, uint8_t dsap, uint8_t ssap);
	static uint8_t analyzeRnr(const uint8_t* pBuf, uint8_t len, uint8_t dsap, uint8_t ssap);
	static uint8_t analyzeDummy(const uint8_t* pBuf, uint8_t len, uint8_t dsap, uint8_t ssap);
	static uint8_t (*sAnalyzePdu[])(const uint8_t* pBuf, uint8_t len, uint8_t dsap, uint8_t ssap);

	static uint8_t analyzeParamList(const uint8_t *pBuf);
	static void createPdu(PduType type);
	static void killConnection();
	static bool addSendData(const void* pBuf, uint8_t len);
	static bool connect();
	/// @}


protected:
	static DepMode		m_DepMode;			///< 現在のDepMode
	static bool			m_bInitiator;		///< true:Initiator / false:Target or not DEP mode

protected:
	static uint16_t		m_LinkTimeout;		///< Link Timeout値[msec](デフォルト:100ms)
	static bool			m_bSend;			///< true:送信側 / false:受信側
	static LlcpStatus	m_LlcpStat;			///< LLCP状態
	static uint8_t		m_DSAP;				///< DSAP
	static uint8_t		m_SSAP;				///< SSAP
	static PduType		m_LastSentPdu;		///< 最後に送信したPDU
	static uint8_t		m_CommandLen;		///< 次に送信するデータ長
	static uint8_t		m_SendBuf[LLCP_MIU];	///< 送信データバッファ
	static uint8_t		m_SendLen;				///< 送信データサイズ
	static uint8_t		m_ValueS;			///< V(S)
	static uint8_t		m_ValueR;			///< V(R)
	static uint8_t		m_ValueSA;			///< V(SA)
	static uint8_t		m_ValueRA;			///< V(SA)

	static void (*m_pRecvCb)(const void* pBuf, uint8_t len);
};

#endif /* HKNFCRW_USE_NFCDEP */

#endif /* HK_NFCDEP_H */
