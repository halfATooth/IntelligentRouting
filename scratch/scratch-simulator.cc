// #include "ns3/core-module.h"
// #include "../intelligent-routing-src/sharedmemory/communication.h"

// using namespace ns3;

// NS_LOG_COMPONENT_DEFINE("ScratchSimulator");

// int ct = 0;
// std::string CollectNetInfo(){
//     return std::to_string(ct++);
// }

// void UpdateRouting(std::string data){
//     NS_LOG_INFO("UpdateRouting rev: " + data);
// }
// int
// main(int argc, char* argv[])
// {
//     LogComponentEnable ("ScratchSimulator", LOG_LEVEL_INFO);
//     NS_LOG_INFO ("This is a shared memory simulation.");
//     GlobalValue::Bind("SimulatorImplementationType", StringValue("ns3::RealtimeSimulatorImpl"));

//     Callback<std::string> CollectCallback = MakeCallback(&CollectNetInfo);
//     Callback<void, std::string> UpdateCallback = MakeCallback(&UpdateRouting);
//     Ptr<CommunicateWithAIModule> communication = CreateObject<CommunicateWithAIModule>(CollectCallback, UpdateCallback);
//     communication->Start();
//     // CommunicateWithAIModule communication(CollectCallback, UpdateCallback);
//     // communication.Start();

//     NS_LOG_INFO("start");
//     Simulator::Stop(Seconds(11));
//     Simulator::Run();
//     Simulator::Destroy();
//     NS_LOG_INFO("stop");
//     return 0;
// }

#include <iostream>
#include "ns3/simulator.h"
#include "ns3/nstime.h"
#include "ns3/command-line.h"
#include "ns3/double.h"
#include "ns3/random-variable-stream.h"
using namespace ns3;
class MyModel
{
public:
    MyModel(){ std::cout<<"MyModel"<<std::endl; }
    ~MyModel(){ std::cout<<"~MyModel"<<std::endl; }
  void Start (void);
private:
  void HandleEvent (double eventValue);
};
void
MyModel::Start (void)
{
  Simulator::Schedule (Seconds (10.0),
                       &MyModel::HandleEvent,
                       this, Simulator::Now ().GetSeconds ());
}
void
MyModel::HandleEvent (double value)
{
  std::cout << "Member method received event at "
            << Simulator::Now ().GetSeconds ()
            << "s started at " << value << "s" << std::endl;
}
static void
ExampleFunction (MyModel *model)
{
  std::cout << "ExampleFunction received event at "
            << Simulator::Now ().GetSeconds () << "s" << std::endl;
  model->Start ();
}
static void
RandomFunction (void)
{
  std::cout << "RandomFunction received event at "
            << Simulator::Now ().GetSeconds () << "s" << std::endl;
}
static void
CancelledEvent (void)
{
  std::cout << "I should never be called... " << std::endl;
}
int main (int argc, char *argv[])
{
  CommandLine cmd;
  cmd.Parse (argc, argv);
  MyModel model;
  Ptr<UniformRandomVariable> v = CreateObject<UniformRandomVariable> ();
  v->SetAttribute ("Min", DoubleValue (10));
  v->SetAttribute ("Max", DoubleValue (20));
  Simulator::Schedule (Seconds (10.0), &ExampleFunction, &model);
  Simulator::Schedule (Seconds (v->GetValue ()), &RandomFunction);
  EventId id = Simulator::Schedule (Seconds (30.0), &CancelledEvent);
  Simulator::Cancel (id);
  Simulator::Run ();
  Simulator::Destroy ();
}
