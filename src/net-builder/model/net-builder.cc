#include "net-builder.h"

namespace ns3
{

std::vector<std::vector<LinkState>> NetBuilder::linkStates;
std::map<std::string, int> NetBuilder::ipStrToNodeIndex;
std::vector<std::vector<std::vector<int>>> NetBuilder::nodeInterfaces;

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
    linkStates = std::vector<std::vector<LinkState>>(n, std::vector<LinkState>(n, {0, 0, 0, 0, 0}));
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

std::string
NetBuilder::getIpString(Ipv4Address ip)
{
    std::ostringstream oss;
    oss << ip;
    return oss.str();
}

int
NetBuilder::getNeighbor(int nodeIndex, int ifIndex)
{
    for (auto v : nodeInterfaces[nodeIndex])
    {
        if (v[1] == ifIndex)
        {
            return v[0];
        }
    }
    return -1;
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
    // 记录ip对应的节点标号
    ipStrToNodeIndex[getIpString(iic.GetAddress(0))] = i;
    ipStrToNodeIndex[getIpString(iic.GetAddress(1))] = j;

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
NetBuilder::EnableForwardCallback()
{
    for (int i = 0; i < c.GetN(); i++)
    {
        Ptr<Ipv4L3Protocol> ipv4 = c.Get(i)->GetObject<Ipv4L3Protocol>();
        if (ipv4)
        {
            ipv4->TraceConnectWithoutContext("Tx", MakeBoundCallback(&TxCallback, i));
            ipv4->TraceConnectWithoutContext("Rx", MakeBoundCallback(&RxCallback, i));
        }
    }
}

void
NetBuilder::TxCallback(int nodeIndex, Ptr<const Packet> pkt, Ptr<Ipv4> ipv4, uint32_t i)
{
    int next = getNeighbor(nodeIndex, i);
    std::cout << "send: " << nodeIndex << " -> " << next << std::endl;
    linkStates[nodeIndex][next].dropCount++;
    linkStates[nodeIndex][next].sendCount++;
    linkStates[nodeIndex][next].latestSendTime = Simulator::Now().GetMicroSeconds();
}

void
NetBuilder::RxCallback(int nodeIndex, Ptr<const Packet> pkt, Ptr<Ipv4> ipv4, uint32_t i)
{
    int pre = getNeighbor(nodeIndex, i);
    std::cout << "rev: " << pre << " -> " << nodeIndex << std::endl;
    linkStates[pre][nodeIndex].dropCount--;
    linkStates[pre][nodeIndex].throughput += pkt->GetSize();
    int64_t delay = Simulator::Now().GetMicroSeconds() - linkStates[pre][nodeIndex].latestSendTime;
    linkStates[pre][nodeIndex].delay += delay;
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
    // Ptr<PacketSink> sinkApp = DynamicCast<PacketSink> (apps.Get (0));
    // Callback<void, Ptr<const Packet>, const Address &> callback =
    // MakeCallback(&NetBuilder::RxCallback, this); sinkApp->TraceConnectWithoutContext ("Rx",
    // MakeBoundCallback(&NetBuilder::RxCallback, nodeIndex));
}

void
NetBuilder::installReceiveApp(int nodeIndex)
{
    installReceiveApp(nodeIndex, defaultStartTime, defaultEndTime);
}

std::vector<std::vector<LinkState>> NetBuilder::getLinkStates()
{
    return linkStates;
}

} // namespace ns3
