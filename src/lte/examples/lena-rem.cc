/*
 * Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Author: Manuel Requena <manuel.requena@cttc.es>
 *         Nicola Baldo <nbaldo@cttc.es>
 */

#include "ns3/buildings-helper.h"
#include "ns3/config-store.h"
#include "ns3/core-module.h"
#include "ns3/lte-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/spectrum-module.h"
// #include "ns3/gtk-config-store.h"

using namespace ns3;

int
main(int argc, char* argv[])
{
    CommandLine cmd(__FILE__);
    cmd.Parse(argc, argv);

    // to save a template default attribute file run it like this:
    // ./ns3 run src/lte/examples/lena-first-sim --command-template="%s
    // --ns3::ConfigStore::Filename=input-defaults.txt --ns3::ConfigStore::Mode=Save
    // --ns3::ConfigStore::FileFormat=RawText"
    //
    // to load a previously created default attribute file
    // ./ns3 run src/lte/examples/lena-first-sim --command-template="%s
    // --ns3::ConfigStore::Filename=input-defaults.txt --ns3::ConfigStore::Mode=Load
    // --ns3::ConfigStore::FileFormat=RawText"

    ConfigStore inputConfig;
    inputConfig.ConfigureDefaults();

    // Parse again so you can override default values from the command line
    cmd.Parse(argc, argv);

    Ptr<LteHelper> lteHelper = CreateObject<LteHelper>();

    // Uncomment to enable logging
    // lteHelper->EnableLogComponents ();

    // Create Nodes: eNodeB and UE
    NodeContainer enbNodes;
    NodeContainer ueNodes;
    enbNodes.Create(1);
    ueNodes.Create(1);

    // Install Mobility Model
    MobilityHelper mobility;
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(enbNodes);
    BuildingsHelper::Install(enbNodes);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(ueNodes);
    BuildingsHelper::Install(ueNodes);

    // Create Devices and install them in the Nodes (eNB and UE)
    NetDeviceContainer enbDevs;
    NetDeviceContainer ueDevs;
    // Default scheduler is PF, uncomment to use RR
    // lteHelper->SetSchedulerType ("ns3::RrFfMacScheduler");

    enbDevs = lteHelper->InstallEnbDevice(enbNodes);
    ueDevs = lteHelper->InstallUeDevice(ueNodes);

    // Attach a UE to a eNB
    lteHelper->Attach(ueDevs, enbDevs.Get(0));

    // Activate an EPS bearer
    EpsBearer::Qci q = EpsBearer::GBR_CONV_VOICE;
    EpsBearer bearer(q);
    lteHelper->ActivateDataRadioBearer(ueDevs, bearer);

    // Configure Radio Environment Map (REM) output
    // for LTE-only simulations always use /ChannelList/0 which is the downlink channel
    Ptr<RadioEnvironmentMapHelper> remHelper = CreateObject<RadioEnvironmentMapHelper>();
    remHelper->SetAttribute("ChannelPath", StringValue("/ChannelList/0"));
    remHelper->SetAttribute("OutputFile", StringValue("rem.out"));
    remHelper->SetAttribute("XMin", DoubleValue(-400.0));
    remHelper->SetAttribute("XMax", DoubleValue(400.0));
    remHelper->SetAttribute("YMin", DoubleValue(-300.0));
    remHelper->SetAttribute("YMax", DoubleValue(300.0));
    remHelper->SetAttribute("Z", DoubleValue(0.0));
    remHelper->Install();

    // here's a minimal gnuplot script that will plot the above:
    //
    // set view map;
    // set term x11;
    // set xlabel "X"
    // set ylabel "Y"
    // set cblabel "SINR (dB)"
    // plot "rem.out" using ($1):($2):(10*log10($4)) with image

    Simulator::Run();

    // GtkConfigStore config;
    // config.ConfigureAttributes ();

    Simulator::Destroy();
    return 0;
}
