#include "ns3/central-controller.h"
#include "ns3/core-module.h"
#include "ns3/net-builder.h"
#include "ns3/shared-memory.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("ScratchSimulator");

int
main(int argc, char* argv[])
{
    GlobalValue::Bind("SimulatorImplementationType", StringValue("ns3::RealtimeSimulatorImpl"));

    NetBuilder netBuilder(4);
    std::vector<std::vector<int>> graph = {{0, 1, 1}, {0, 2, 1}, {1, 3, 2}, {2, 3, 1}};
    netBuilder.connect(graph);

    CentralController controller(netBuilder);
    controller.InitRoutingTable();

    netBuilder.EnableForwardCallback();

    netBuilder.installSendApp(0, 3);
    netBuilder.installReceiveApp(3);

    Callback<std::string> CollectCallback =
        MakeCallback(&CentralController::CollectNetInfo, &controller);
    Callback<void, std::string> UpdateCallback =
        MakeCallback(&CentralController::UpdateRoutingTable, &controller);
    CommunicateWithAIModule communication(CollectCallback, UpdateCallback);
    communication.Start();

    Simulator::Stop(Seconds(300));
    Simulator::Run();
    Simulator::Destroy();
    return 0;
}