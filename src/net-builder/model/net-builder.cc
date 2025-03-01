#include "net-builder.h"

namespace ns3
{

void
NetBuilder::init(int n)
{
    c.Create(n);
    nodeToIpAddress = std::vector<Ipv4Address>(n);
    p2p.SetDeviceAttribute("DataRate", DataRateValue(5000000));
    p2p.SetChannelAttribute("Delay", TimeValue(MilliSeconds(2)));
    InternetStackHelper internet;
    internet.Install(c);
    networkNumCt = 0;
    for (int i = 0; i < n; i++)
    {
        std::vector<std::vector<int>> v;
        nodeInterfaces.push_back(v);
    }
}

void
NetBuilder::randomRouting()
{
    if (!dst.IsInitialized())
    {
        std::cerr << "cannot set routes before nodes are connected" << std::endl;
        return;
    }
    Ptr<Ipv4StaticRouting> staticRouting;
    for (int i = 0; i < c.GetN() - 1; i++)
    {
        staticRouting = Ipv4RoutingHelper::GetRouting<Ipv4StaticRouting>(
            c.Get(i)->GetObject<Ipv4>()->GetRoutingProtocol());

        std::vector<int> arr;
        for (auto v : nodeInterfaces[i])
        {
            // special rule
            if (v[0] > i)
            {
                arr.push_back(v[1]);
            }
        }
        int ifindex = randomPick(arr);
        // std::cout<<"node "<<i<<", ifindex "<<ifindex<<std::endl;
        staticRouting->AddHostRouteTo(dst, ifindex);
    }
}

int
NetBuilder::randomPick(std::vector<int> arr)
{
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    int index = std::rand() % arr.size();
    return arr[index];
}

std::string
NetBuilder::getIpBase()
{
    return "10.0." + std::to_string(networkNumCt++) + ".0";
}

void
NetBuilder::ComputePacketDelay(Ptr<FlowMonitor> fm)
{
    // 获取统计结果
    FlowMonitor::FlowStatsContainer flowStats = fm->GetFlowStats();
    double delaySum = 0.0;
    int packetnum = 0;
    // 计算每个流的数据包时延
    for (auto it : flowStats)
    {
        if (it.second.txPackets > 0 && it.second.rxPackets > 0)
        {
            // 计算平均时延
            packetnum += it.second.rxPackets;
            delaySum += it.second.delaySum.GetSeconds();
        }
    }
    double overallAverageDelay = delaySum / packetnum * 1000;
    // 计算总体平均包时延
    std::cout << "Overall Average Packet Delay: " << overallAverageDelay << " ms" << std::endl;
}

void
NetBuilder::ComputePacketLossRate(Ptr<FlowMonitor> fm)
{
    // 获取统计结果
    FlowMonitor::FlowStatsContainer flowStats = fm->GetFlowStats();

    uint32_t totalSentPackets = 0;
    uint32_t totalReceivedPackets = 0;

    // 计算每个流的数据包发送和接收数量
    for (auto it : flowStats)
    {
        if (it.second.txPackets > 0)
        {
            totalSentPackets += it.second.txPackets;
            totalReceivedPackets += it.second.rxPackets;
        }
    }

    // 计算丢包率
    uint32_t lostPackets = totalSentPackets - totalReceivedPackets;
    double packetLossRate = static_cast<double>(lostPackets) / totalSentPackets * 100.0;

    std::cout << "Total Sent Packets: " << totalSentPackets << std::endl;
    // std::cout << "Total Received Packets: " << totalReceivedPackets << std::endl;
    // std::cout << "Lost Packets: " << lostPackets << std::endl;
    std::cout << "Packet Loss Rate: " << packetLossRate << "%" << std::endl;
}

void
NetBuilder::ComputeFlowCompleteTime(FlowMonitorHelper& fmHelper, Ptr<FlowMonitor> fm)
{
    // 获取统计结果
    FlowMonitor::FlowStatsContainer flowStats = fm->GetFlowStats();
    // 计算完成时间
    double totalThroughput = 0.0;
    double fct = 0;
    int validFlows = 0;
    for (auto it : flowStats)
    {
        if (it.second.txBytes > 0)
        {
            fct += (it.second.timeLastRxPacket - it.second.timeFirstTxPacket).GetSeconds() *
                   1e3; // 单位为毫秒
            validFlows++;
        }
    }
    std::cout << "FCT: " << fct / validFlows << " ms" << std::endl;
}

void
NetBuilder::ComputeFlowThroughput(FlowMonitorHelper& fmHelper, Ptr<FlowMonitor> fm)
{
    // 获取统计结果
    FlowMonitor::FlowStatsContainer flowStats = fm->GetFlowStats();
    // 计算总吞吐量
    double totalThroughput = 0.0;
    int validFlows = 0;
    for (auto it : flowStats)
    {
        if (it.second.txBytes > 0)
        {
            // 确保时间间隔非零
            Time duration = it.second.timeLastRxPacket - it.second.timeFirstTxPacket;
            if (duration.GetSeconds() > 0)
            {
                double throughput =
                    it.second.rxBytes * 8.0 / duration.GetSeconds() / 1024 / 1024; // 比特转换为Mbps
                totalThroughput += throughput;
                // std::cout << "Flow ID: " << it.first << ", Throughput: " << throughput << " Mbps"
                // << std::endl;
            }
            validFlows++;
        }
    }
    std::cout << "Total Network Throughput: " << totalThroughput << " Mbps" << std::endl;
    std::cout << "Avg Network Throughput: " << totalThroughput / validFlows << " Mbps" << std::endl;
}

void
NetBuilder::ComputeNetThroughput(FlowMonitorHelper& fmHelper, Ptr<FlowMonitor> fm)
{
    // 获取统计结果
    FlowMonitor::FlowStatsContainer flowStats = fm->GetFlowStats();
    // 计算总吞吐量
    double totalrxbyte = 0.0;
    double lastfct = 0;
    for (auto it : flowStats)
    {
        if (it.second.txBytes > 0)
        {
            // 确保时间间隔非零
            Time duration = it.second.timeLastRxPacket - it.second.timeFirstTxPacket;
            if (duration.GetSeconds() > 0)
            {
                double rxbyte = it.second.txBytes * 8.0; // 比特
                totalrxbyte += rxbyte;
            }

            if (it.second.timeLastRxPacket.GetSeconds() > lastfct)
            {
                lastfct = it.second.timeLastRxPacket.GetSeconds();
            }
        }
    }
    std::cout << "Network Throughput: " << totalrxbyte / lastfct / 1024 / 1024 << "Mbps"
              << std::endl;
}

void
NetBuilder::connect(int i, int j)
{
    if (i < 0 || i >= c.GetN() || j < 0 || j >= c.GetN())
    {
        std::cerr << "cannot connect node " << i << " and " << j << "in NodeContainer(" << c.GetN()
                  << ")" << std::endl;
        return;
    }
    NodeContainer net = NodeContainer(c.Get(i), c.Get(j));
    NetDeviceContainer ndc = p2p.Install(net);
    std::string ip = getIpBase();
    ipv4.SetBase(Ipv4Address(ip.data()), Ipv4Mask("255.255.255.0"));
    Ipv4InterfaceContainer iic = ipv4.Assign(ndc);
    dst = iic.GetAddress(1);
    // record ip on node
    if (!nodeToIpAddress[i].IsInitialized())
    {
        // if ip is 0.0.0.0, set ip
        nodeToIpAddress[i] = iic.GetAddress(0);
    }
    if (!nodeToIpAddress[j].IsInitialized())
    {
        nodeToIpAddress[j] = iic.GetAddress(1);
    }
    // record ifindex
    Ipv4InterfaceContainer::Iterator it = iic.Begin();
    nodeInterfaces[i].push_back({j, int((*it).second)});
    ++it;
    nodeInterfaces[j].push_back({i, int((*it).second)});
}

void
NetBuilder::connect(int m[][2], int len)
{
    for (int i = 0; i < len; i++)
    {
        connect(m[i][0], m[i][1]);
    }
}

void
NetBuilder::quadConnect(int width)
{
    for (int i = 0; i < (int)(c.GetN() - 1); i++)
    {
        if (i + width < (int)c.GetN())
        {
            connect(i, i + width);
        }
        if (i % width + 1 < width)
        {
            connect(i, i + 1);
        }
    }
}

void
NetBuilder::cubeConnect(int x, int y)
{
    for (int i = 0; i < (int)(c.GetN() - 1); i++)
    {
        if (i + x * y < (int)c.GetN())
        {
            connect(i, i + x * y);
        }
        if (i % (x * y) + x < x * y)
        {
            connect(i, i + x);
        }
        if (i % x + 1 < x)
        {
            connect(i, i + 1);
        }
    }
}

void
NetBuilder::GEANT2()
{
    init(24);
    int connectInfo[37][2] = {
        {0, 1},   {0, 2},   {1, 3},   {1, 6},   {1, 9},   {2, 3},   {2, 4},   {3, 5},
        {3, 6},   {4, 7},   {5, 8},   {6, 8},   {6, 9},   {7, 8},   {7, 11},  {8, 11},
        {8, 12},  {8, 17},  {8, 18},  {8, 20},  {9, 10},  {9, 12},  {9, 13},  {10, 13},
        {11, 14}, {11, 20}, {12, 13}, {12, 19}, {12, 21}, {14, 15}, {15, 16}, {16, 17},
        {17, 18}, {18, 21}, {19, 23}, {21, 22}, {22, 23},
    };
    connect(connectInfo, 37);
}

void
NetBuilder::run()
{
    randomRouting();

    uint16_t port = 9;
    OnOffHelper onoff("ns3::UdpSocketFactory", Address(InetSocketAddress(dst, port)));
    onoff.SetConstantRate(DataRate(6000));
    ApplicationContainer apps = onoff.Install(c.Get(0));
    apps.Start(Seconds(1.0));
    apps.Stop(Seconds(3.0));

    PacketSinkHelper sink("ns3::UdpSocketFactory",
                          Address(InetSocketAddress(Ipv4Address::GetAny(), port)));
    apps = sink.Install(c.Get(c.GetN() - 1));
    apps.Start(Seconds(1.0));
    apps.Stop(Seconds(3.0));

    AsciiTraceHelper ascii;
    p2p.EnableAsciiAll(ascii.CreateFileStream("eztop.tr"));

    FlowMonitorHelper fmHelper; // 安装流监控器
    Ptr<FlowMonitor> fm = fmHelper.InstallAll();

    Simulator::Stop(Seconds(5));
    Simulator::Run();
    Simulator::Destroy();
    ComputePacketDelay(fm);
}

int
NetBuilder::getPort(int from, int to)
{
    // [ from ]--------------------------------->[ to ]
    //   node  port: v[1]                         node
    for (auto v : nodeInterfaces[from])
    {
        if (v[0] == to)
        {
            return v[1];
        }
    }
    return -1;
}

std::vector<Ipv4Address>
NetBuilder::getNodeToIpAddress()
{
    return nodeToIpAddress;
}

NodeContainer
NetBuilder::getNodes()
{
    return c;
}

void
NetBuilder::installSendApp(int nodeIndex, int destIndex, Time startTime, Time endTime)
{
    Ipv4Address dest = nodeToIpAddress[destIndex];
    OnOffHelper onoff("ns3::UdpSocketFactory", Address(InetSocketAddress(dest, port)));
    onoff.SetConstantRate(DataRate(6000));
    ApplicationContainer apps = onoff.Install(c.Get(nodeIndex));
    apps.Start(startTime);
    apps.Stop(endTime);
}

void
NetBuilder::installSendApp(int nodeIndex, int destIndex)
{
    installSendApp(nodeIndex, destIndex, defaultStartTime, defaultEndTime);
}

void
NetBuilder::installSendToAllApp(int nodeIndex, Time startTime, Time endTime)
{
    for (int i = 0; i < c.GetN(); i++)
    {
        if (i == nodeIndex)
        {
            continue;
        }
        installSendApp(nodeIndex, i, startTime, endTime);
    }
}

void
NetBuilder::installSendToAllApp(int nodeIndex)
{
    installSendToAllApp(nodeIndex, defaultStartTime, defaultEndTime);
}

void
NetBuilder::installReceiveApp(int nodeIndex, Time startTime, Time endTime)
{
    PacketSinkHelper sink("ns3::UdpSocketFactory",
                          Address(InetSocketAddress(Ipv4Address::GetAny(), port)));
    ApplicationContainer apps = sink.Install(c.Get(nodeIndex));
    apps.Start(startTime);
    apps.Stop(endTime);
}

void
NetBuilder::installReceiveApp(int nodeIndex)
{
    installReceiveApp(nodeIndex, defaultStartTime, defaultEndTime);
}

} // namespace ns3
