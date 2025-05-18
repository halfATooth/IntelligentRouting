#include "ns3/core-module.h"
#include "ns3/net-builder-helper.h"
#include "ns3/central-controller.h"

/**
 * @file
 *
 * Explain here what the example does.
 */

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("NetBuilderExample");

static void
RouteOutputCallback (const Ipv4Header& header, Ptr<const Packet> pkt, uint32_t i)
{
    std::cout << header.GetSource()<<"->"<<header.GetDestination() << std::endl;
    std::cout << pkt->GetUid() << std::endl;
    std::cout << i << std::endl;
    // std::cout << "Routing packet with ID " << packet->GetUid() << " to destination " << InetSocketAddress::ConvertFrom (dest).GetIpv4 () << std::endl;
}

int
main(int argc, char* argv[])
{
    bool verbose = true;

    CommandLine cmd(__FILE__);
    cmd.AddValue("verbose", "Tell application to log if true", verbose);

    cmd.Parse(argc, argv);

    // GlobalValue::Bind("SimulatorImplementationType", StringValue("ns3::RealtimeSimulatorImpl"));
    // Time::SetResolution(Time::NS);
    // LogComponentEnable("OnOffApplication", LOG_LEVEL_INFO);
    // LogComponentEnable("PacketSink", LOG_LEVEL_INFO);
    LogComponentEnable ("NetBuilderExample", LOG_LEVEL_INFO);

    NetBuilder netBuilder(4);
    netBuilder.connect(0, 1);
    netBuilder.connect(0, 2);
    netBuilder.connect(1, 3);
    netBuilder.connect(2, 3);

    CentralController controller(netBuilder);
    std::vector<std::vector<int>> pairs = {{0,1,1}, {0,2,1}, {1,3,2}, {2,3,1}};
    controller.AddTopologyInfo(pairs, 4);
    controller.InitRoutingTable();
    controller.PrintRoutingTable();

    // 测试转发回调
    // Ptr<Ipv4L3Protocol> ipv4 = netBuilder.getNodes().Get(2)->GetObject<Ipv4L3Protocol>();
    // ipv4->TraceConnectWithoutContext ("UnicastForward", MakeCallback (&RouteOutputCallback));
    netBuilder.EnableForwardCallback();
    

    netBuilder.installSendApp(0, 3);
    // netBuilder.installReceiveApp(1);
    // netBuilder.installReceiveApp(2);
    netBuilder.installReceiveApp(3);

    // for(auto i : netBuilder.ipStrToNodeIndex){
    //     std::cout << i << std::endl;
    // }

    NS_LOG_INFO("start");
    Simulator::Stop(Seconds(11));
    Simulator::Run();
    Simulator::Destroy();
    NS_LOG_INFO("end");
    return 0;
}
