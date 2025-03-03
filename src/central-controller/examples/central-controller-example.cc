#include "ns3/core-module.h"
#include "ns3/central-controller-helper.h"
#include "ns3/net-builder.h"

/**
 * @file
 *
 * Explain here what the example does.
 */

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("ContralControllerExample");

int
main(int argc, char* argv[])
{
    bool verbose = true;

    CommandLine cmd(__FILE__);
    cmd.AddValue("verbose", "Tell application to log if true", verbose);

    cmd.Parse(argc, argv);

    /* ... */

    Simulator::Run();
    Simulator::Destroy();
    return 0;
}
