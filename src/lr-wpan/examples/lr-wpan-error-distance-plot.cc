/*
 * Copyright (c) 2011 The Boeing Company
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Author: Tom Henderson <thomas.r.henderson@boeing.com>
 */

/*
   This program produces a gnuplot file that plots the packet success rate
   as a function of distance for the 802.15.4 models, assuming a default
   LogDistance propagation loss model, the 2.4 GHz OQPSK error model, a
   default transmit power of 0 dBm, and a default packet size of 20 bytes of
   802.15.4 payload and a default rx sensitivity of -106.58 dBm.

   Tx power of the transmitter node and the Rx sensitivity of the receiving node
   as well as the transmitted packet size can be adjusted to obtain a different
   distance plot.

    Node1                       Node2
   (dev0) --------------------->(dev1)

   Usage:

   ./ns3 run "lr-wpan-error-distance-plot --txPower= 0 --rxSensitivity=-92"

*/
#include "ns3/abort.h"
#include "ns3/callback.h"
#include "ns3/command-line.h"
#include "ns3/constant-position-mobility-model.h"
#include "ns3/gnuplot.h"
#include "ns3/log.h"
#include "ns3/lr-wpan-error-model.h"
#include "ns3/lr-wpan-mac.h"
#include "ns3/lr-wpan-net-device.h"
#include "ns3/lr-wpan-spectrum-value-helper.h"
#include "ns3/mac16-address.h"
#include "ns3/multi-model-spectrum-channel.h"
#include "ns3/net-device.h"
#include "ns3/node.h"
#include "ns3/nstime.h"
#include "ns3/packet.h"
#include "ns3/propagation-loss-model.h"
#include "ns3/simulator.h"
#include "ns3/single-model-spectrum-channel.h"
#include "ns3/spectrum-value.h"
#include "ns3/test.h"
#include "ns3/uinteger.h"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace ns3;
using namespace ns3::lrwpan;

uint32_t g_packetsReceived = 0; //!< number of packets received

NS_LOG_COMPONENT_DEFINE("LrWpanErrorDistancePlot");

/**
 * Function called when a Data indication is invoked
 * @param params MCPS data indication parameters
 * @param p packet
 */
void
LrWpanErrorDistanceCallback(McpsDataIndicationParams params, Ptr<Packet> p)
{
    g_packetsReceived++;
}

int
main(int argc, char* argv[])
{
    std::ostringstream os;
    std::ofstream berfile("802.15.4-psr-distance.plt");

    int minDistance = 1;
    int maxDistance = 200; // meters
    int increment = 1;
    int maxPackets = 1000;
    int packetSize = 7; // PSDU = 20 bytes (11 bytes MAC header + 7 bytes MSDU )
    double txPower = 0;
    uint32_t channelNumber = 11;
    double rxSensitivity = -106.58; // dBm

    CommandLine cmd(__FILE__);

    cmd.AddValue("txPower", "transmit power (dBm)", txPower);
    cmd.AddValue("packetSize", "packet (MSDU) size (bytes)", packetSize);
    cmd.AddValue("channelNumber", "channel number", channelNumber);
    cmd.AddValue("rxSensitivity", "the rx sensitivity (dBm)", rxSensitivity);

    cmd.Parse(argc, argv);

    os << "Packet (MSDU) size = " << packetSize << " bytes; tx power = " << txPower
       << " dBm; channel = " << channelNumber << "; Rx sensitivity = " << rxSensitivity << " dBm";

    Gnuplot psrplot = Gnuplot("802.15.4-psr-distance.eps");
    Gnuplot2dDataset psrdataset("802.15.4-psr-vs-distance");

    Ptr<Node> n0 = CreateObject<Node>();
    Ptr<Node> n1 = CreateObject<Node>();
    Ptr<LrWpanNetDevice> dev0 = CreateObject<LrWpanNetDevice>();
    Ptr<LrWpanNetDevice> dev1 = CreateObject<LrWpanNetDevice>();
    dev0->SetAddress(Mac16Address("00:01"));
    dev1->SetAddress(Mac16Address("00:02"));
    Ptr<MultiModelSpectrumChannel> channel = CreateObject<MultiModelSpectrumChannel>();
    Ptr<LogDistancePropagationLossModel> model = CreateObject<LogDistancePropagationLossModel>();
    channel->AddPropagationLossModel(model);
    dev0->SetChannel(channel);
    dev1->SetChannel(channel);
    n0->AddDevice(dev0);
    n1->AddDevice(dev1);
    Ptr<ConstantPositionMobilityModel> mob0 = CreateObject<ConstantPositionMobilityModel>();
    dev0->GetPhy()->SetMobility(mob0);
    Ptr<ConstantPositionMobilityModel> mob1 = CreateObject<ConstantPositionMobilityModel>();
    dev1->GetPhy()->SetMobility(mob1);

    LrWpanSpectrumValueHelper svh;
    Ptr<SpectrumValue> psd = svh.CreateTxPowerSpectralDensity(txPower, channelNumber);
    dev0->GetPhy()->SetTxPowerSpectralDensity(psd);

    // Set Rx sensitivity of the receiving device
    dev1->GetPhy()->SetRxSensitivity(rxSensitivity);

    McpsDataIndicationCallback cb0;
    cb0 = MakeCallback(&LrWpanErrorDistanceCallback);
    dev1->GetMac()->SetMcpsDataIndicationCallback(cb0);

    McpsDataRequestParams params;
    params.m_srcAddrMode = SHORT_ADDR;
    params.m_dstAddrMode = SHORT_ADDR;
    params.m_dstPanId = 0;
    params.m_dstAddr = Mac16Address("00:02");
    params.m_msduHandle = 0;
    params.m_txOptions = 0;

    Ptr<Packet> p;
    mob0->SetPosition(Vector(0, 0, 0));
    mob1->SetPosition(Vector(minDistance, 0, 0));
    for (int j = minDistance; j < maxDistance; j += increment)
    {
        for (int i = 0; i < maxPackets; i++)
        {
            p = Create<Packet>(packetSize);
            Simulator::Schedule(Seconds(i), &LrWpanMac::McpsDataRequest, dev0->GetMac(), params, p);
        }
        Simulator::Run();
        NS_LOG_DEBUG("Received " << g_packetsReceived << " packets for distance " << j);
        psrdataset.Add(j, g_packetsReceived / 1000.0);
        g_packetsReceived = 0;

        mob1->SetPosition(Vector(j, 0, 0));
    }

    psrplot.AddDataset(psrdataset);

    psrplot.SetTitle(os.str());
    psrplot.SetTerminal("postscript eps color enh \"Times-BoldItalic\"");
    psrplot.SetLegend("distance (m)", "Packet Success Rate (PSR)");
    psrplot.SetExtra("set xrange [0:200]\n\
                      set yrange [0:1]\n\
                      set grid\n\
                      set style line 1 linewidth 5\n\
                      set style increment user");
    psrplot.GenerateOutput(berfile);
    berfile.close();

    Simulator::Destroy();
    return 0;
}
