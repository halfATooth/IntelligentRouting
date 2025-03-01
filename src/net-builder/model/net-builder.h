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
#include <string>
#include <vector>

// Add a doxygen group for this module.
// If you have more than one file, this should be in only one of them.
/**
 * @defgroup net-builder Description of the net-builder
 */

namespace ns3
{

class NetBuilder
{
  private:
    NodeContainer c;
    PointToPointHelper p2p;
    Ipv4AddressHelper ipv4;
    int networkNumCt;
    // nodeInterfaces[i][j] = {k, ifindex} means: from node i to node k through port ifindex
    // nodeInterfaces[i].size() means: the num of neighbors of node i
    std::vector<std::vector<std::vector<int>>> nodeInterfaces;
    // not necessary
    Ipv4Address dst;
    // record Ipv4Address on nodes
    std::vector<Ipv4Address> nodeToIpAddress;
    Time defaultStartTime = Seconds(0);
    Time defaultEndTime = Seconds(10.0);
    uint16_t port = 9;

    void init(int n);
    void randomRouting();
    int randomPick(std::vector<int> arr);
    std::string getIpBase();
    void ComputePacketDelay(Ptr<FlowMonitor> fm);
    void ComputePacketLossRate(Ptr<FlowMonitor> fm);
    void ComputeFlowCompleteTime(FlowMonitorHelper& fmHelper, Ptr<FlowMonitor> fm);
    void ComputeFlowThroughput(FlowMonitorHelper& fmHelper, Ptr<FlowMonitor> fm);
    void ComputeNetThroughput(FlowMonitorHelper& fmHelper, Ptr<FlowMonitor> fm);

  public:
    NetBuilder()
    {
    }

    NetBuilder(int n)
    {
        init(n);
    }

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
};

} // namespace ns3

#endif // NET_BUILDER_H
