#include "ns3/central-controller.h"
#include "ns3/core-module.h"
#include "ns3/net-builder.h"
#include "ns3/shared-memory.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("ScratchSimulator");

int
getTopology(std::string filename, std::vector<std::vector<int>>& res)
{
    std::ifstream inFile(filename);
    std::set<int> nodes;
    if (inFile.is_open())
    {
        std::string line;
        while (std::getline(inFile, line))
        {
            int spaceIndex = line.find(" ");
            int n0 = atoi(line.substr(0, spaceIndex).c_str());
            int n1 = atoi(line.substr(spaceIndex + 1, line.length() - spaceIndex - 1).c_str());
            res.push_back({n0, n1, 1});
            nodes.insert(n0);
            nodes.insert(n1);
        }
        inFile.close();
    }
    else
    {
        std::cerr << "无法打开文件进行读取" << std::endl;
    }
    return nodes.size();
}

void
runSimulator(std::vector<std::vector<int>> graph, int n, int duration)
{
    NetBuilder netBuilder(n);
    netBuilder.connect(graph);

    CentralController controller(netBuilder);
    controller.InitRoutingTable();

    netBuilder.EnableForwardCallback();

    std::set<std::pair<int, int>> sendAndRevs;
    while (sendAndRevs.size() < graph.size())
    {
        int send = netBuilder.generateRandomInteger(0, n);
        int rev = netBuilder.generateRandomInteger(0, n);
        if (send != rev)
        {
            sendAndRevs.insert({send, rev});
        }
    }
    for (auto sendAndRev : sendAndRevs)
    {
        int send = sendAndRev.first;
        int rev = sendAndRev.second;
        netBuilder.installSendApp(send, rev, Seconds(1), Seconds(duration));
    }
    netBuilder.installReceiveAppForAll(Seconds(0), Seconds(duration));

    Callback<std::string> CollectCallback =
        MakeCallback(&CentralController::CollectNetInfo, &controller);
    Callback<void, std::string> UpdateCallback =
        MakeCallback(&CentralController::UpdateRoutingTable, &controller);
    CommunicateWithAIModule communication(CollectCallback, UpdateCallback);
    communication.Start();

    Simulator::Stop(Seconds(duration));
    Simulator::Run();
    Simulator::Destroy();
}
// /home/lhs/workspace/ns-3-dev/build/scratch/ns3-dev-scratch-simulator-default 
int
main(int argc, char* argv[])
{
    GlobalValue::Bind("SimulatorImplementationType", StringValue("ns3::RealtimeSimulatorImpl"));

    int duration = 600;

    // std::string root = "/home/lhs/workspace/python/intelligent-routing";
    // if(argc < 3){
    //     std::cout<<"program: "<<argv[0]<<std::endl;
    //     return 0;
    // }
    // // prog nodeNum topoId times
    // int nodeNum = atoi(argv[1]);
    // int topoId = atoi(argv[2]);
    // std::string dir = root + "/data/net/nodes_num_" + std::to_string(nodeNum) + "/" +
    //                           std::to_string(topoId) + "/";
    // std::vector<std::vector<int>> graph;
    // int n = getTopology(dir + "topology", graph);

    // runSimulator(graph, n, duration);

    // NetBuilder netBuilder(4);
    // std::vector<std::vector<int>> graph = {{0, 1, 1}, {0, 2, 1}, {1, 3, 2}, {2, 3, 1}};
    // netBuilder.connect(graph);

    NetBuilder netBuilder;
    netBuilder.GEANT2();

    CentralController controller(netBuilder);
    controller.InitRoutingTable();

    netBuilder.EnableForwardCallback();

    // netBuilder.installSendApp(0, 3);
    // netBuilder.installReceiveApp(3);
    std::set<std::pair<int, int>> sendAndRevs;
    while (sendAndRevs.size() < 37)
    {
        int send = netBuilder.generateRandomInteger(0, 24);
        int rev = netBuilder.generateRandomInteger(0, 24);
        if (send != rev)
        {
            sendAndRevs.insert({send, rev});
        }
    }
    for (auto sendAndRev : sendAndRevs)
    {
        int send = sendAndRev.first;
        int rev = sendAndRev.second;
        netBuilder.installSendApp(send, rev, Seconds(1), Seconds(duration));
    }
    netBuilder.installReceiveAppForAll(Seconds(0), Seconds(duration));

    Callback<std::string> CollectCallback =
        MakeCallback(&CentralController::CollectNetInfo, &controller);
    Callback<void, std::string> UpdateCallback =
        MakeCallback(&CentralController::UpdateRoutingTable, &controller);
    CommunicateWithAIModule communication(CollectCallback, UpdateCallback);
    communication.Start();

    Simulator::Stop(Seconds(duration));
    Simulator::Run();
    Simulator::Destroy();
    return 0;
}