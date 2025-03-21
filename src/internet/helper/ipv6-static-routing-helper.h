/*
 * Copyright (c) 2009 University of Washington
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ifndef IPV6_STATIC_ROUTING_HELPER_H
#define IPV6_STATIC_ROUTING_HELPER_H

#include "ipv6-routing-helper.h"

#include "ns3/ipv6-address.h"
#include "ns3/ipv6-static-routing.h"
#include "ns3/ipv6.h"
#include "ns3/net-device-container.h"
#include "ns3/net-device.h"
#include "ns3/node-container.h"
#include "ns3/node.h"
#include "ns3/ptr.h"

namespace ns3
{

/**
 * @ingroup ipv6Helpers
 *
 * @brief Helper class that adds ns3::Ipv6StaticRouting objects
 *
 * This class is expected to be used in conjunction with
 * ns3::InternetStackHelper::SetRoutingHelper
 */
class Ipv6StaticRoutingHelper : public Ipv6RoutingHelper
{
  public:
    /**
     * @brief Constructor.
     */
    Ipv6StaticRoutingHelper();

    /**
     * @brief Construct an Ipv6ListRoutingHelper from another previously
     * initialized instance (Copy Constructor).
     * @param o object to be copied
     */
    Ipv6StaticRoutingHelper(const Ipv6StaticRoutingHelper& o);

    // Delete assignment operator to avoid misuse
    Ipv6StaticRoutingHelper& operator=(const Ipv6StaticRoutingHelper&) = delete;

    /**
     * @returns pointer to clone of this Ipv6StaticRoutingHelper
     *
     * This method is mainly for internal use by the other helpers;
     * clients are expected to free the dynamic memory allocated by this method
     */
    Ipv6StaticRoutingHelper* Copy() const override;

    /**
     * @param node the node on which the routing protocol will run
     * @returns a newly-created routing protocol
     *
     * This method will be called by ns3::InternetStackHelper::Install
     */
    Ptr<Ipv6RoutingProtocol> Create(Ptr<Node> node) const override;

    /**
     * @brief Get Ipv6StaticRouting pointer from IPv6 stack.
     * @param ipv6 Ipv6 pointer
     * @return Ipv6StaticRouting pointer or 0 if not found
     */
    Ptr<Ipv6StaticRouting> GetStaticRouting(Ptr<Ipv6> ipv6) const;

    /**
     * @brief Add a multicast route to a node and net device using explicit
     * Ptr<Node> and Ptr<NetDevice>
     *
     * @param n The node.
     * @param source Source address.
     * @param group Multicast group.
     * @param input Input NetDevice.
     * @param output Output NetDevices.
     */
    void AddMulticastRoute(Ptr<Node> n,
                           Ipv6Address source,
                           Ipv6Address group,
                           Ptr<NetDevice> input,
                           NetDeviceContainer output);

    /**
     * @brief Add a multicast route to a node and device using a name string
     * previously associated to the node using the Object Name Service and a
     * Ptr<NetDevice>
     *
     * @param n The node.
     * @param source Source address.
     * @param group Multicast group.
     * @param input Input NetDevice.
     * @param output Output NetDevices.
     */
    void AddMulticastRoute(std::string n,
                           Ipv6Address source,
                           Ipv6Address group,
                           Ptr<NetDevice> input,
                           NetDeviceContainer output);

    /**
     * @brief Add a multicast route to a node and device using a Ptr<Node> and a
     * name string previously associated to the device using the Object Name Service.
     *
     * @param n The node.
     * @param source Source address.
     * @param group Multicast group.
     * @param inputName Input NetDevice.
     * @param output Output NetDevices.
     */
    void AddMulticastRoute(Ptr<Node> n,
                           Ipv6Address source,
                           Ipv6Address group,
                           std::string inputName,
                           NetDeviceContainer output);

    /**
     * @brief Add a multicast route to a node and device using name strings
     * previously associated to both the node and device using the Object Name
     * Service.
     *
     * @param nName The node.
     * @param source Source address.
     * @param group Multicast group.
     * @param inputName Input NetDevice.
     * @param output Output NetDevices.
     */
    void AddMulticastRoute(std::string nName,
                           Ipv6Address source,
                           Ipv6Address group,
                           std::string inputName,
                           NetDeviceContainer output);

#if 0
  /**
   * @brief Add a default route to the static routing protocol to forward
   *        packets out a particular interface
   */
  void SetDefaultMulticastRoute (Ptr<Node> n, Ptr<NetDevice> nd);
  void SetDefaultMulticastRoute (Ptr<Node> n, std::string ndName);
  void SetDefaultMulticastRoute (std::string nName, Ptr<NetDevice> nd);
  void SetDefaultMulticastRoute (std::string nName, std::string ndName);
#endif
};

} // namespace ns3

#endif /* IPV6_STATIC_ROUTING_HELPER_H */
