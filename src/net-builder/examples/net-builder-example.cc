#include "ns3/core-module.h"
#include "ns3/net-builder-helper.h"

/**
 * @file
 *
 * Explain here what the example does.
 */

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("NetBuilderExample");

int
main(int argc, char* argv[])
{
    bool verbose = true;

    CommandLine cmd(__FILE__);
    cmd.AddValue("verbose", "Tell application to log if true", verbose);

    cmd.Parse(argc, argv);

    GlobalValue::Bind("SimulatorImplementationType", StringValue("ns3::RealtimeSimulatorImpl"));
    // Time::SetResolution(Time::NS);
    LogComponentEnable("OnOffApplication", LOG_LEVEL_INFO);
    LogComponentEnable("PacketSink", LOG_LEVEL_INFO);
    LogComponentEnable ("NetBuilderExample", LOG_LEVEL_INFO);

    NetBuilder netBuilder(2);
    netBuilder.connect(0, 1);
    netBuilder.installSendApp(0, 1);
    netBuilder.installReceiveApp(1);

    NS_LOG_INFO("start");
    Simulator::Stop(Seconds(11));
    Simulator::Run();
    Simulator::Destroy();
    NS_LOG_INFO("end");
    return 0;
}
