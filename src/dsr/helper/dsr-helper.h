/*
 * Copyright (c) 2011 Yufei Cheng
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Author: Yufei Cheng   <yfcheng@ittc.ku.edu>
 *
 * James P.G. Sterbenz <jpgs@ittc.ku.edu>, director
 * ResiliNets Research Group  https://resilinets.org/
 * Information and Telecommunication Technology Center (ITTC)
 * and Department of Electrical Engineering and Computer Science
 * The University of Kansas Lawrence, KS USA.
 *
 * Work supported in part by NSF FIND (Future Internet Design) Program
 * under grant CNS-0626918 (Postmodern Internet Architecture),
 * NSF grant CNS-1050226 (Multilayer Network Resilience Analysis and Experimentation on GENI),
 * US Department of Defense (DoD), and ITTC at The University of Kansas.
 */

#ifndef DSR_HELPER_H
#define DSR_HELPER_H

#include "ns3/dsr-routing.h"
#include "ns3/icmpv4-l4-protocol.h"
#include "ns3/node-container.h"
#include "ns3/node.h"
#include "ns3/object-factory.h"
#include "ns3/tcp-l4-protocol.h"
#include "ns3/udp-l4-protocol.h"

namespace ns3
{

/**
 * @ingroup dsr
 *
 * @brief DSR helper class to manage creation of DSR routing instance and
 *        to insert it on a node as a sublayer between transport and
 *        IP layers.
 */
class DsrHelper
{
  public:
    /**
     * Create an DsrHelper that makes life easier for people who want to install
     * Dsr routing to nodes.
     */
    DsrHelper();
    ~DsrHelper();

    // Delete assignment operator to avoid misuse
    DsrHelper& operator=(const DsrHelper&) = delete;

    /**
     * @brief Construct an DsrHelper from another previously initialized instance
     * (Copy Constructor).
     * @param o object to copy from
     */
    DsrHelper(const DsrHelper& o);
    /**
     * @returns pointer to clone of this DsrHelper
     *
     * This method is mainly for internal use by the other helpers;
     * clients are expected to free the dynamic memory allocated by this method
     */
    DsrHelper* Copy() const;
    /**
     * @param node the node on which the routing protocol will run
     * @returns a newly-created L4 protocol
     */
    Ptr<ns3::dsr::DsrRouting> Create(Ptr<Node> node) const;
    /**
     * Set attribute values for future instances of DSR that this helper creates
     * @param name the node on which the routing protocol will run
     * @param value newly-created L4 protocol
     */
    void Set(std::string name, const AttributeValue& value);

  private:
    ObjectFactory m_agentFactory; ///< DSR factory
};

} // namespace ns3

#endif // DSR_HELPER_H
