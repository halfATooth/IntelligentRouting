#ifndef NET_BUILDER_H
#define NET_BUILDER_H

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/internet-module.h"
#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"

#include <cassert>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

// Add a doxygen group for this module.
// If you have more than one file, this should be in only one of them.
/**
 * @defgroup net-builder Description of the net-builder
 */

namespace ns3
{

struct LinkState
{
    int dropCount = 0;
    int sendCount = 0;
    int throughput = 0;
    int64_t latestSendTime = 0; // us
    int64_t delay = 0;  // us
};

class NetBuilder
{
  private:
    NodeContainer c;
    PointToPointHelper p2p;
    Ipv4AddressHelper ipv4;
    int networkNumCt;
    // nodeInterfaces[i][j] = {k, ifindex} means: from node i to node k through port ifindex
    // nodeInterfaces[i].size() means: the num of neighbors of node i
    static std::vector<std::vector<std::vector<int>>> nodeInterfaces;
    // not necessary
    Ipv4Address dst;
    // record Ipv4Address on nodes
    std::vector<Ipv4Address> nodeToIpAddress;
    static std::map<std::string, int> ipStrToNodeIndex;
    Time defaultStartTime = Seconds(0);
    Time defaultEndTime = Seconds(10.0);
    uint16_t port = 9;
    // 记录链路数据
    static std::vector<std::vector<LinkState>> linkStates;

    void init(int n);
    int randomPick(std::vector<int> arr);
    std::string getIpBase();
    static std::string getIpString(Ipv4Address ip);
    static int getNeighbor(int nodeIndex, int ifIndex);
    static void TxCallback(int nodeIndex, Ptr<const Packet>, Ptr<Ipv4>, uint32_t);
    static void RxCallback(int nodeIndex, Ptr<const Packet>, Ptr<Ipv4>, uint32_t);

  public:
    NetBuilder()
    {
    }

    NetBuilder(int n)
    {
        init(n);
    }

    void randomRouting();
    void connect(int i, int j);
    void connect(int m[][2], int len);
    void quadConnect(int width);
    void cubeConnect(int x, int y);
    void GEANT2();
    void run();
    int getPort(int from, int to);
    std::vector<Ipv4Address> getNodeToIpAddress();
    NodeContainer getNodes();
    void installSendApp(int srcIndex, int destIndex, Time startTime, Time endTime);
    void installSendApp(int srcIndex, int destIndex); // use default start/end time
    void installSendToAllApp(int srcIndex, Time startTime, Time endTime);
    void installSendToAllApp(int srcIndex); // use default start/end time
    void installReceiveApp(int nodeIndex, Time startTime, Time endTime);
    void installReceiveApp(int nodeIndex); // use default start/end time
    void EnableForwardCallback();
    std::vector<std::vector<LinkState>> getLinkStates();
};

} // namespace ns3

#endif // NET_BUILDER_H
