/*
 *  Copyright (c) 2007,2008, 2009 INRIA, UDcast
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 *         Mohamed Amine Ismail <amine.ismail@sophia.inria.fr>
 *
 */

#ifndef IPCS_CLASSIFIER_RECORD_H
#define IPCS_CLASSIFIER_RECORD_H

#include "wimax-tlv.h"

#include "ns3/ipv4-address.h"

#include <stdint.h>

namespace ns3
{

/**
 * @ingroup wimax
 * @brief IpcsClassifierRecord class
 */
class IpcsClassifierRecord
{
  public:
    IpcsClassifierRecord();
    ~IpcsClassifierRecord();
    /**
     * @brief creates a classifier records and sets all its parameters
     * @param srcAddress the source ip address
     * @param srcMask the mask to apply on the source ip address
     * @param dstAddress the destination ip address
     * @param dstMask the mask to apply on the destination ip address
     * @param srcPortLow the lower boundary of the source port range
     * @param srcPortHigh the higher boundary of the source port range
     * @param dstPortLow the lower boundary of the destination port range
     * @param dstPortHigh the higher boundary of the destination port range
     * @param protocol the L4 protocol
     * @param priority the priority of this classifier
     *
     */
    IpcsClassifierRecord(Ipv4Address srcAddress,
                         Ipv4Mask srcMask,
                         Ipv4Address dstAddress,
                         Ipv4Mask dstMask,
                         uint16_t srcPortLow,
                         uint16_t srcPortHigh,
                         uint16_t dstPortLow,
                         uint16_t dstPortHigh,
                         uint8_t protocol,
                         uint8_t priority);
    /**
     * @brief Decodes a TLV and creates a classifier
     * @param tlv the TLV to decode and from which the classifier parameters will be extracted
     */
    IpcsClassifierRecord(Tlv tlv);
    /**
     * @brief Creates a TLV from this classifier
     * @return the created TLV
     */
    Tlv ToTlv() const;
    /**
     * @brief add a new source ip address to the classifier
     * @param srcAddress the source ip address
     * @param srcMask the mask to apply on the source ip address
     */
    void AddSrcAddr(Ipv4Address srcAddress, Ipv4Mask srcMask);
    /**
     * @brief add a new destination ip address to the classifier
     * @param dstAddress the destination ip address
     * @param dstMask the mask to apply on the destination ip address
     */
    void AddDstAddr(Ipv4Address dstAddress, Ipv4Mask dstMask);
    /**
     * @brief add a range of source port to the classifier
     * @param srcPortLow the lower boundary of the source port range
     * @param srcPortHigh the higher boundary of the source port range
     */
    void AddSrcPortRange(uint16_t srcPortLow, uint16_t srcPortHigh);
    /**
     * @brief add a range of destination port to the classifier
     * @param dstPortLow the lower boundary of the destination port range
     * @param dstPortHigh the higher boundary of the destination port range
     */
    void AddDstPortRange(uint16_t dstPortLow, uint16_t dstPortHigh);
    /**
     * @brief add a protocol to the classifier
     * @param proto the L4 protocol to add
     */
    void AddProtocol(uint8_t proto);
    /**
     * @brief Set the priority of this classifier
     * @param prio the priority of the classifier
     */
    void SetPriority(uint8_t prio);
    /**
     * @brief Set the index of the classifier
     * @param index the index of the classifier
     */
    void SetIndex(uint16_t index);
    /**
     * @brief check if a packets can be used with this classifier
     * @param srcAddress the source ip address of the packet
     * @param dstAddress the destination ip address of the packet
     * @param srcPort the source port of the packet
     * @param dstPort the destination port of the packet
     * @param proto The L4 protocol of the packet
     * @return true if there is a match
     */
    bool CheckMatch(Ipv4Address srcAddress,
                    Ipv4Address dstAddress,
                    uint16_t srcPort,
                    uint16_t dstPort,
                    uint8_t proto) const;
    /**
     * @return the cid associated with this classifier
     */
    uint16_t GetCid() const;
    /**
     * @return the priority of this classifier
     */
    uint8_t GetPriority() const;
    /**
     * @return the index of this classifier
     */
    uint16_t GetIndex() const;
    /**
     * @brief Set the cid associated to this classifier
     * @param cid the connection identifier
     */
    void SetCid(uint16_t cid);

  private:
    /**
     * Check match source address function
     * @param srcAddress source IP address to check
     * @returns true if a match
     */
    bool CheckMatchSrcAddr(Ipv4Address srcAddress) const;
    /**
     * Check match destination address function
     * @param dstAddress destination IP address to check
     * @returns true if a match
     */
    bool CheckMatchDstAddr(Ipv4Address dstAddress) const;
    /**
     * Check match source port function
     * @param srcPort source port to check
     * @returns true if a match
     */
    bool CheckMatchSrcPort(uint16_t srcPort) const;
    /**
     * Check match destination port function
     * @param dstPort destination port to check
     * @returns true if a match
     */
    bool CheckMatchDstPort(uint16_t dstPort) const;
    /**
     * Check match protocol function
     * @param proto protocol number to check
     * @returns true if a match
     */
    bool CheckMatchProtocol(uint8_t proto) const;

    /// PortRange structure
    struct PortRange
    {
        uint16_t PortLow;  ///< port low
        uint16_t PortHigh; ///< port high
    };

    /// Ipv4Addr structure
    struct Ipv4Addr
    {
        Ipv4Address Address; ///< IP address
        Ipv4Mask Mask;       ///< net mask
    };

    uint8_t m_priority;                    ///< priority
    uint16_t m_index;                      ///< index
    uint8_t m_tosLow;                      ///< TOS low
    uint8_t m_tosHigh;                     ///< TOS high
    uint8_t m_tosMask;                     ///< TOS mask
    std::vector<uint8_t> m_protocol;       ///< protocol
    std::vector<Ipv4Addr> m_srcAddr;       ///< source address
    std::vector<Ipv4Addr> m_dstAddr;       ///< destination address
    std::vector<PortRange> m_srcPortRange; ///< source port range
    std::vector<PortRange> m_dstPortRange; ///< destination port range

    uint16_t m_cid; ///< the CID
};
} // namespace ns3

#endif /* IPCS_CLASSIFIER_RECORD_H */
