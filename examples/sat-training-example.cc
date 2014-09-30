/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2014 Magister Solutions
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Sami Rantanen <sami.rantanen@magister.fi>
 *
 */


#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/satellite-module.h"
#include "ns3/traffic-module.h"
#include "ns3/config-store-module.h"


using namespace ns3;

/**
 * \file sat-training-example.cc
 * \ingroup satellite
 *
 * \brief Simulation script to be utilized in SNS3 training. The script illustrates
 * the simulation script process starting from command line arguments to running the
 * actual simulation.
 *
 * int main (int argc, char *argv[])
 * {
 *   // Set default attribute values
 *   // Parse command-line arguments
 *   // Configure the topology; nodes, channels, devices, mobility
 *   // Add (Internet) stack to nodes
 *   // Configure IP addressing and routing
 *   // Add and configure applications
 *   // Configure tracing
 *   // Run simulation
 * }
 *
 * execute command -> ./waf --run "sat-training-example --PrintHelp"
 */

NS_LOG_COMPONENT_DEFINE ("sat-training-example");

int
main (int argc, char *argv[])
{
  LogComponentEnable ("sat-training-example", LOG_LEVEL_INFO);

  NS_LOG_INFO("--- sat-training-example ---");

  /**
   * Initialize simulation script variables
   */
  uint32_t endUsersPerUt (1);
  uint32_t utsPerBeam (1);
  double simDuration (10.0); // in seconds

  /**
   * Enable all co-channel beams for user frequency id 1.
   */
  const unsigned int BEAMS (16);
  unsigned int coChannelBeams [BEAMS] = {1, 3, 5, 7, 9, 22, 24, 26, 28, 30, 44, 46, 48, 50,  59, 61};

  /**
   * Read the default attributes from XML attribute file
   */

  NS_LOG_INFO ("Reading the XML input: training-input-attributes.xml");

  Config::SetDefault ("ns3::ConfigStore::Filename", StringValue ("./src/satellite/examples/training-input-attributes.xml"));
  Config::SetDefault ("ns3::ConfigStore::Mode", StringValue ("Load"));
  Config::SetDefault ("ns3::ConfigStore::FileFormat", StringValue ("Xml"));
  ConfigStore inputConfig;
  inputConfig.ConfigureDefaults ();

  /**
   * Overwrite some attribute values for this script
   */
  Config::SetDefault ("ns3::SatLowerLayerServiceConf::DaService3_ConstantAssignmentProvided", BooleanValue (false));
  Config::SetDefault ("ns3::SatLowerLayerServiceConf::DaService3_RbdcAllowed", BooleanValue (true));
  Config::SetDefault ("ns3::SatLowerLayerServiceConf::DaService3_MinimumServiceRate", UintegerValue (40));
  Config::SetDefault ("ns3::SatLowerLayerServiceConf::DaService3_VolumeAllowed", BooleanValue (false));
  Config::SetDefault ("ns3::SatBeamScheduler::ControlSlotsEnabled", BooleanValue (true));
  Config::SetDefault ("ns3::SatBeamScheduler::ControlSlotInterval", TimeValue (Seconds (1)));

  Config::SetDefault ("ns3::SatSuperframeConf0::FrameCount", UintegerValue (3));
  Config::SetDefault ("ns3::SatSuperframeConf0::Frame0_AllocatedBandwidthHz", DoubleValue (5e+06));
  Config::SetDefault ("ns3::SatSuperframeConf0::Frame1_AllocatedBandwidthHz", DoubleValue (10e+06));
  Config::SetDefault ("ns3::SatSuperframeConf0::Frame2_AllocatedBandwidthHz", DoubleValue (10e+06));

  /**
   * Configure traces
   */
  Config::SetDefault ("ns3::SatChannel::EnableRxPowerOutputTrace", BooleanValue (true));
  Config::SetDefault ("ns3::SatChannel::EnableFadingOutputTrace", BooleanValue (true));
  Config::SetDefault ("ns3::SatPhyRxCarrier::EnableCompositeSinrOutputTrace", BooleanValue (true));
  Config::SetDefault ("ns3::SatPhyRxCarrierConf::EnableIntfOutputTrace", BooleanValue (true));

  Singleton<SatFadingOutputTraceContainer>::Get ()->EnableFigureOutput (false);
  Singleton<SatInterferenceOutputTraceContainer>::Get ()->EnableFigureOutput (false);
  Singleton<SatRxPowerOutputTraceContainer>::Get ()->EnableFigureOutput (false);
  Singleton<SatCompositeSinrOutputTraceContainer>::Get ()->EnableFigureOutput (false);

  //Singleton<SatFadingOutputTraceContainer>::Get ()->InsertTag ("fadingExampleTag_");
  //Singleton<SatInterferenceOutputTraceContainer>::Get ()->InsertTag ("interferenceExampleTag_");
  //Singleton<SatRxPowerOutputTraceContainer>::Get ()->InsertTag ("rxPowerExampleTag_");
  //Singleton<SatCompositeSinrOutputTraceContainer>::Get ()->InsertTag ("compositeSinrExampleTag_");

  /**
   * Read the command line arguments. Note, that this allows the user to change
   * the ns3 attributes also from command line when running the script.
   */
  CommandLine cmd;
  cmd.AddValue ("utsPerBeam", "Number of UTs per spot-beam", utsPerBeam);
  cmd.AddValue ("simDurationInSeconds", "Simulation duration in seconds", simDuration);
  cmd.Parse (argc, argv);

  /**
   * Create satellite system by the usage of satellite helper structures
   */
  NS_LOG_INFO ("Creating the satellite scenario");

  std::string scenarioName = "Scenario72";
  Ptr<SatHelper> helper = CreateObject<SatHelper> (scenarioName);

  std::string st ("creation-trace-training");
  helper->EnableCreationTraces (st, true);

  SatBeamUserInfo beamInfo = SatBeamUserInfo (utsPerBeam, endUsersPerUt);
  std::map<uint32_t, SatBeamUserInfo > beamMap;
  for (unsigned i = 0; i < BEAMS; ++i)
    {
      beamMap[coChannelBeams[i]] = beamInfo;
    }

  helper->CreateUserDefinedScenario (beamMap);

  // Other pre-defined satellite simulation scenario options:
  //helper->CreateSimpleScenario ();
  //helper->CreateLargerScenario ();
  //helper->CreateFullScenario ();

  helper->EnablePacketTrace ();

  /**
   * Configure end user applications. In the training example, the users use
   * on-off application in return link.
   */

  // port used for packet delivering
  uint16_t port = 9; // Discard port (RFC 863)
  const std::string protocol = "ns3::UdpSocketFactory";
  uint32_t packetSize (1280); // in bytes
  DataRate dataRate (128000); // in bps

  NS_LOG_INFO ("Configuring the on-off application; data rate: " << dataRate << " bps, packet size: " << packetSize << " bytes");

  // get users (first GW side user and first UT connected users)
  NodeContainer utUsers = helper->GetUtUsers();
  NodeContainer gwUsers = helper->GetGwUsers();

  Time appStartTime (MilliSeconds (100));

  const InetSocketAddress gwAddr = InetSocketAddress (helper->GetUserAddress (gwUsers.Get (0)), port);

  for (NodeContainer::Iterator itUt = utUsers.Begin ();
      itUt != utUsers.End ();
      ++itUt)
    {
      appStartTime += MilliSeconds (50);

      // return link
      Ptr<SatOnOffApplication> rtnApp = CreateObject<SatOnOffApplication> ();
      rtnApp->SetAttribute ("Protocol", StringValue (protocol));
      rtnApp->SetAttribute ("Remote", AddressValue (gwAddr));
      rtnApp->SetAttribute ("PacketSize", UintegerValue (packetSize));
      rtnApp->SetAttribute ("DataRate", DataRateValue (dataRate));
      rtnApp->SetAttribute ("OnTime", StringValue ("ns3::ExponentialRandomVariable[Mean=1.0|Bound=0.0]"));
      rtnApp->SetAttribute ("OffTime", StringValue ("ns3::ExponentialRandomVariable[Mean=1.0|Bound=0.0]"));
      rtnApp->SetStartTime (appStartTime);
      (*itUt)->AddApplication (rtnApp);
    }

  // setup packet sinks at all users
  Ptr<PacketSink> ps = CreateObject<PacketSink> ();
  ps->SetAttribute ("Protocol", StringValue (protocol));
  ps->SetAttribute ("Local", AddressValue (gwAddr));
  gwUsers.Get (0)->AddApplication (ps);

  /**
   * Set-up statistics
   */
  NS_LOG_INFO ("Setting up statistics");

  Ptr<SatStatsHelperContainer> s = CreateObject<SatStatsHelperContainer> (helper);

  // Delay
  s->AddGlobalRtnAppDelay (SatStatsHelper::OUTPUT_CDF_FILE);
  s->AddGlobalRtnAppDelay (SatStatsHelper::OUTPUT_CDF_PLOT);
  s->AddAverageUtUserRtnAppDelay (SatStatsHelper::OUTPUT_SCALAR_FILE);
  s->AddAverageBeamRtnAppDelay (SatStatsHelper::OUTPUT_SCALAR_FILE);

  // Composite SINR
  s->AddGlobalRtnCompositeSinr (SatStatsHelper::OUTPUT_CDF_FILE);
  s->AddGlobalRtnCompositeSinr (SatStatsHelper::OUTPUT_CDF_PLOT);

  // Throughput
  s->AddAverageUtUserRtnAppThroughput (SatStatsHelper::OUTPUT_CDF_FILE);
  s->AddAverageUtUserRtnAppThroughput (SatStatsHelper::OUTPUT_CDF_PLOT);
  s->AddPerUtUserRtnAppThroughput (SatStatsHelper::OUTPUT_SCALAR_FILE);
  s->AddPerBeamRtnAppThroughput (SatStatsHelper::OUTPUT_SCALAR_FILE);
  s->AddPerGwRtnAppThroughput (SatStatsHelper::OUTPUT_SCALAR_FILE);

  NS_LOG_INFO ("Simulation variables:");
  NS_LOG_INFO (" - Simulation duration: " << simDuration);
  NS_LOG_INFO (" - Number of UTs: " << utsPerBeam);
  NS_LOG_INFO (" - Number of end users per UT: " << endUsersPerUt);

  /**
   * Store attributes into XML output
   */

  NS_LOG_INFO ("Storing the used attributes to XML file: training-output-attributes-ut-" << utsPerBeam << ".xml");

  std::stringstream filename;
  filename << "training-output-attributes-ut" << utsPerBeam << ".xml";
  Config::SetDefault ("ns3::ConfigStore::Filename", StringValue (filename.str ()));
  Config::SetDefault ("ns3::ConfigStore::FileFormat", StringValue ("Xml"));
  Config::SetDefault ("ns3::ConfigStore::Mode", StringValue ("Save"));
  ConfigStore outputConfig;
  outputConfig.ConfigureDefaults ();
  outputConfig.ConfigureAttributes ();

  /**
   * Run simulation
   */
  NS_LOG_INFO ("Running network simulator 3");

  Simulator::Stop (Seconds (simDuration));
  Simulator::Run ();

  Simulator::Destroy ();

  return 0;
}
