/*
 * Copyright (c) 2011,2012 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Author: Manuel Requena <manuel.requena@cttc.es>
 *         Nicola Baldo <nbaldo@cttc.es>
 */

#ifndef LTE_RLC_TM_H
#define LTE_RLC_TM_H

#include "lte-rlc.h"

#include "ns3/event-id.h"

#include <map>

namespace ns3
{

/**
 * LTE RLC Transparent Mode (TM), see 3GPP TS 36.322
 *
 * Please note that, as in TM it is not possible to add any header, the delay
 * measurements gathered from the trace source "RxPDU" of LteRlc are invalid
 * (they will be always 0)
 */
class LteRlcTm : public LteRlc
{
  public:
    LteRlcTm();
    ~LteRlcTm() override;
    /**
     * @brief Get the type ID.
     * @return the object TypeId
     */
    static TypeId GetTypeId();
    void DoDispose() override;

    /**
     * RLC SAP
     *
     * @param p packet
     */
    void DoTransmitPdcpPdu(Ptr<Packet> p) override;

    /**
     * MAC SAP
     *
     * @param txOpParams the LteMacSapUser::TxOpportunityParameters
     */
    void DoNotifyTxOpportunity(LteMacSapUser::TxOpportunityParameters txOpParams) override;
    /**
     * Notify HARQ deliver failure
     */
    void DoNotifyHarqDeliveryFailure() override;
    void DoReceivePdu(LteMacSapUser::ReceivePduParameters rxPduParams) override;

  private:
    /// Expire RBS timer function
    void ExpireRbsTimer();
    /// Report buffer status
    void DoReportBufferStatus();

  private:
    /**
     * @brief Store an incoming (from layer above us) PDU, waiting to transmit it
     */
    struct TxPdu
    {
        /**
         * @brief TxPdu default constructor
         * @param pdu the PDU
         * @param time the arrival time
         */
        TxPdu(const Ptr<Packet>& pdu, const Time& time)
            : m_pdu(pdu),
              m_waitingSince(time)
        {
        }

        TxPdu() = delete;

        Ptr<Packet> m_pdu;   ///< PDU
        Time m_waitingSince; ///< Layer arrival time
    };

    std::vector<TxPdu> m_txBuffer; ///< Transmission buffer

    uint32_t m_maxTxBufferSize; ///< maximum transmit buffer size
    uint32_t m_txBufferSize;    ///< transmit buffer size

    EventId m_rbsTimer; ///< RBS timer
};

} // namespace ns3

#endif // LTE_RLC_TM_H
