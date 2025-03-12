#ifndef CENTRAL_CONTROLLER_H
#define CENTRAL_CONTROLLER_H

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/ipv4-list-routing-helper.h"
#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/net-builder.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"

#include <stack>
#include <vector>

// Add a doxygen group for this module.
// If you have more than one file, this should be in only one of them.
/**
 * @defgroup central-controller Description of the central-controller
 */

namespace ns3
{

// Each class should be documented using Doxygen,
// and have an @ingroup central-controller directive

class CentralController
{
  public:
    CentralController(NetBuilder nb);
    void AddTopologyInfo(std::vector<std::vector<int>> pairs, int len);
    std::string CollectNetInfo();
    void UpdateRoutingTable(std::string weightsData);
    void InitRoutingTable();
    void PrintRoutingTable();

  private:
    // void CollectLinkInfo();

    void doUpdateRoutingTable();
    void AddRouteFromStart(int start,
                           std::vector<Ipv4Address> nodeToIpAddress,
                           std::vector<int> path,
                           std::vector<std::vector<bool>>& isVisit);
    void clearRoutingTable();
    void findNeighbors(int start,
                       int v,
                       std::vector<int>& distance2start,
                       std::vector<int>& preNodes,
                       std::vector<int> isCheck);
    std::vector<int> Dijkstra(int start);
    void UpdateWeights(std::string weightsData);
    std::string ConcatLinkState(int i, int j, LinkState linkState);

    NodeContainer m_nodes;
    // Time m_collectionInterval;
    // Time m_routingUpdateInterval;
    EventId m_collectionEvent;
    EventId m_routingUpdateEvent;
    int m_max_weight = 101;
    NetBuilder netBuilder;
    std::vector<std::vector<int>> m_adj;
    bool isAdjReady = false;
};

} // namespace ns3

#endif // CENTRAL_CONTROLLER_H
