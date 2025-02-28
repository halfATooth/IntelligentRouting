#include "ns3/core-module.h"
#include "ns3/shared-memory.h"

/**
 * @file
 *
 * Explain here what the example does.
 */

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("SharedMemorySimulator");

int ct = 0;
std::string CollectNetInfo(){
    return std::to_string(ct++);
}

void UpdateRouting(std::string data){
    NS_LOG_INFO("UpdateRouting rev: " + data);
}

int
main(int argc, char* argv[])
{
    bool verbose = true;

    CommandLine cmd(__FILE__);
    cmd.AddValue("verbose", "Tell application to log if true", verbose);

    cmd.Parse(argc, argv);

    LogComponentEnable ("SharedMemorySimulator", LOG_LEVEL_INFO);

    GlobalValue::Bind("SimulatorImplementationType", StringValue("ns3::RealtimeSimulatorImpl"));

    Callback<std::string> CollectCallback = MakeCallback(&CollectNetInfo);
    Callback<void, std::string> UpdateCallback = MakeCallback(&UpdateRouting);
    CommunicateWithAIModule communication(CollectCallback, UpdateCallback);
    communication.Start();

    NS_LOG_INFO("simulator start");
    Simulator::Stop(Seconds(300));
    Simulator::Run();
    Simulator::Destroy();
    NS_LOG_INFO("simulator end");
    return 0;
}
