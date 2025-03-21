/*
 * Copyright (c) 2005, 2009 INRIA
 * Copyright (c) 2009 MIRKO BANCHI
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Authors: Mathieu Lacage <mathieu.lacage@sophia.inria.fr>
 *          Mirko Banchi <mk.banchi@gmail.com>
 *          Ghada Badawy <gbadawy@gmail.com>
 */

#include "mac-tx-middle.h"

#include "wifi-mac-header.h"

#include "ns3/log.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("MacTxMiddle");

MacTxMiddle::MacTxMiddle()
    : m_sequence(0)
{
    NS_LOG_FUNCTION(this);
}

MacTxMiddle::~MacTxMiddle()
{
    NS_LOG_FUNCTION(this);
    for (auto i = m_qosSequences.begin(); i != m_qosSequences.end(); i++)
    {
        delete[] i->second;
    }
}

uint16_t
MacTxMiddle::GetNextSequenceNumberFor(const WifiMacHeader* hdr)
{
    NS_LOG_FUNCTION(this);
    uint16_t retval;
    if (hdr->IsQosData() && !hdr->GetAddr1().IsBroadcast())
    {
        uint8_t tid = hdr->GetQosTid();
        NS_ASSERT(tid < 16);
        auto it = m_qosSequences.find(hdr->GetAddr1());
        if (it != m_qosSequences.end())
        {
            retval = it->second[tid];
            it->second[tid]++;
            it->second[tid] %= 4096;
        }
        else
        {
            retval = 0;
            std::pair<Mac48Address, uint16_t*> newSeq(hdr->GetAddr1(), new uint16_t[16]);
            auto newIns = m_qosSequences.insert(newSeq);
            NS_ASSERT(newIns.second == true);
            for (uint8_t i = 0; i < 16; i++)
            {
                newIns.first->second[i] = 0;
            }
            newIns.first->second[tid]++;
        }
    }
    else
    {
        retval = m_sequence;
        m_sequence++;
        m_sequence %= 4096;
    }
    return retval;
}

uint16_t
MacTxMiddle::PeekNextSequenceNumberFor(const WifiMacHeader* hdr)
{
    NS_LOG_FUNCTION(this);
    uint16_t retval;
    if (hdr->IsQosData() && !hdr->GetAddr1().IsBroadcast())
    {
        uint8_t tid = hdr->GetQosTid();
        NS_ASSERT(tid < 16);
        auto it = m_qosSequences.find(hdr->GetAddr1());
        if (it != m_qosSequences.end())
        {
            retval = it->second[tid];
        }
        else
        {
            retval = 0;
        }
    }
    else
    {
        retval = m_sequence;
    }
    return retval;
}

uint16_t
MacTxMiddle::GetNextSeqNumberByTidAndAddress(uint8_t tid, Mac48Address addr) const
{
    NS_LOG_FUNCTION(this);
    NS_ASSERT(tid < 16);
    uint16_t seq = 0;
    auto it = m_qosSequences.find(addr);
    if (it != m_qosSequences.end())
    {
        return it->second[tid];
    }
    return seq;
}

void
MacTxMiddle::SetSequenceNumberFor(const WifiMacHeader* hdr)
{
    NS_LOG_FUNCTION(this << *hdr);

    if (hdr->IsQosData() && !hdr->GetAddr1().IsBroadcast())
    {
        uint8_t tid = hdr->GetQosTid();
        NS_ASSERT(tid < 16);
        auto it = m_qosSequences.find(hdr->GetAddr1());
        NS_ASSERT(it != m_qosSequences.end());
        it->second[tid] = hdr->GetSequenceNumber();
    }
    else
    {
        m_sequence = hdr->GetSequenceNumber();
    }
}

} // namespace ns3
