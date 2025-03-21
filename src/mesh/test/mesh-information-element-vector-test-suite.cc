/*
 * Copyright (c) 2009 IITP RAS
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Author: Pavel Boyko <boyko@iitp.ru>
 */

#include "ns3/mesh-information-element-vector.h"
#include "ns3/test.h"
// All information elements:
#include "ns3/ie-dot11s-beacon-timing.h"
#include "ns3/ie-dot11s-configuration.h"
#include "ns3/ie-dot11s-id.h"
#include "ns3/ie-dot11s-metric-report.h"
#include "ns3/ie-dot11s-peer-management.h"
#include "ns3/ie-dot11s-peering-protocol.h"
#include "ns3/ie-dot11s-perr.h"
#include "ns3/ie-dot11s-prep.h"
#include "ns3/ie-dot11s-preq.h"
#include "ns3/ie-dot11s-rann.h"

using namespace ns3;

/**
 * @ingroup mesh
 * @ingroup tests
 * @defgroup mesh-test mesh module tests
 */

/**
 * @ingroup mesh-test
 *
 * @brief Built-in self test for MeshInformationElementVector and all IE
 */
struct MeshInformationElementVectorBist : public TestCase
{
    MeshInformationElementVectorBist()
        : TestCase("Serialization test for all mesh information elements")
    {
    }

    void DoRun() override;
};

void
MeshInformationElementVectorBist::DoRun()
{
    MeshInformationElementVector vector;
    {
        // Mesh ID test
        Ptr<dot11s::IeMeshId> meshId = Create<dot11s::IeMeshId>("qwerty");
        vector.AddInformationElement(meshId);
    }
    {
        Ptr<dot11s::IeConfiguration> config = Create<dot11s::IeConfiguration>();
        vector.AddInformationElement(config);
    }
    {
        Ptr<dot11s::IeLinkMetricReport> report = Create<dot11s::IeLinkMetricReport>(123456);
        vector.AddInformationElement(report);
    }
    {
        Ptr<dot11s::IePeerManagement> peerMan1 = Create<dot11s::IePeerManagement>();
        peerMan1->SetPeerOpen(1);
        Ptr<dot11s::IePeerManagement> peerMan2 = Create<dot11s::IePeerManagement>();
        peerMan2->SetPeerConfirm(1, 2);
        Ptr<dot11s::IePeerManagement> peerMan3 = Create<dot11s::IePeerManagement>();
        peerMan3->SetPeerClose(1, 2, dot11s::REASON11S_MESH_CAPABILITY_POLICY_VIOLATION);
        vector.AddInformationElement(peerMan1);
        vector.AddInformationElement(peerMan2);
        vector.AddInformationElement(peerMan3);
    }
    {
        Ptr<dot11s::IeBeaconTiming> beaconTiming = Create<dot11s::IeBeaconTiming>();
        beaconTiming->AddNeighboursTimingElementUnit(1, Seconds(1), Seconds(4));
        beaconTiming->AddNeighboursTimingElementUnit(2, Seconds(2), Seconds(3));
        beaconTiming->AddNeighboursTimingElementUnit(3, Seconds(3), Seconds(2));
        beaconTiming->AddNeighboursTimingElementUnit(4, Seconds(4), Seconds(1));
        vector.AddInformationElement(beaconTiming);
    }
    {
        Ptr<dot11s::IeRann> rann = Create<dot11s::IeRann>();
        rann->SetFlags(1);
        rann->SetHopcount(2);
        rann->SetTTL(4);
        rann->DecrementTtl();
        NS_TEST_ASSERT_MSG_EQ(rann->GetTtl(), 3, "SetTtl works");
        rann->SetOriginatorAddress(Mac48Address("11:22:33:44:55:66"));
        rann->SetDestSeqNumber(5);
        rann->SetMetric(6);
        rann->IncrementMetric(2);
        NS_TEST_ASSERT_MSG_EQ(rann->GetMetric(), 8, "SetMetric works");
        vector.AddInformationElement(rann);
    }
    {
        Ptr<dot11s::IePreq> preq = Create<dot11s::IePreq>();
        preq->SetHopcount(0);
        preq->SetTTL(1);
        preq->SetPreqID(2);
        preq->SetOriginatorAddress(Mac48Address("11:22:33:44:55:66"));
        preq->SetOriginatorSeqNumber(3);
        preq->SetLifetime(4);
        preq->AddDestinationAddressElement(false, false, Mac48Address("11:11:11:11:11:11"), 5);
        preq->AddDestinationAddressElement(false, false, Mac48Address("22:22:22:22:22:22"), 6);
        vector.AddInformationElement(preq);
    }
    {
        Ptr<dot11s::IePrep> prep = Create<dot11s::IePrep>();
        prep->SetFlags(12);
        prep->SetHopcount(11);
        prep->SetTtl(10);
        prep->SetDestinationAddress(Mac48Address("11:22:33:44:55:66"));
        prep->SetDestinationSeqNumber(123);
        prep->SetLifetime(5000);
        prep->SetMetric(4321);
        prep->SetOriginatorAddress(Mac48Address("33:00:22:00:11:00"));
        prep->SetOriginatorSeqNumber(666);
        vector.AddInformationElement(prep);
    }
    {
        Ptr<dot11s::IePerr> perr = Create<dot11s::IePerr>();
        dot11s::HwmpProtocol::FailedDestination dest;
        dest.destination = Mac48Address("11:22:33:44:55:66");
        dest.seqnum = 1;
        perr->AddAddressUnit(dest);
        dest.destination = Mac48Address("10:20:30:40:50:60");
        dest.seqnum = 2;
        perr->AddAddressUnit(dest);
        dest.destination = Mac48Address("01:02:03:04:05:06");
        dest.seqnum = 3;
        perr->AddAddressUnit(dest);
        vector.AddInformationElement(perr);
    }
    Ptr<Packet> packet = Create<Packet>();
    packet->AddHeader(vector);
    uint32_t size = vector.GetSerializedSize();
    MeshInformationElementVector resultVector;
    packet->RemoveHeader(resultVector, size);
    NS_TEST_ASSERT_MSG_EQ(vector,
                          resultVector,
                          "Roundtrip serialization of all known information elements works");
}

/**
 * @ingroup mesh-test
 *
 * @brief Mesh Test Suite
 */
class MeshTestSuite : public TestSuite
{
  public:
    MeshTestSuite();
};

MeshTestSuite::MeshTestSuite()
    : TestSuite("devices-mesh", Type::UNIT)
{
    AddTestCase(new MeshInformationElementVectorBist, TestCase::Duration::QUICK);
}

static MeshTestSuite g_meshTestSuite; ///< the test suite
