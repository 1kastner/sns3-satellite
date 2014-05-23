/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/satellite-module.h"
#include "ns3/applications-module.h"
#include "ns3/config-store.h"


using namespace ns3;

/**
* \ingroup satellite
*
*\brief
*
* execute command -> ./waf --run "sat-profiling-sim-tn8 --PrintHelp"
*/

NS_LOG_COMPONENT_DEFINE ("sat-profiling-sim-tn8");

void
TimeTickerCallback ()
{
  std::cout << "Time: " << Simulator::Now ().GetSeconds () << "s" << std::endl;
}

int
main (int argc, char *argv[])
{
  uint32_t endUsersPerUt (1);
  uint32_t utsPerBeam (1);
  uint32_t profilingConf (0);

  // 256 kbps per end user 
  uint32_t packetSize (1280); // in bytes
  double intervalSeconds = 0.04;

  double simLength; // defined later in scenario creation
  Time appStartTime = Seconds (0.1);

  // To read attributes from file
//  Config::SetDefault ("ns3::ConfigStore::Filename", StringValue ("./src/satellite/examples/tn8-profiling-input-attributes.xml"));
//  Config::SetDefault ("ns3::ConfigStore::Mode", StringValue ("Load"));
//  Config::SetDefault ("ns3::ConfigStore::FileFormat", StringValue ("Xml"));
//  ConfigStore inputConfig;
//  inputConfig.ConfigureDefaults ();

  // read command line parameters given by user
  CommandLine cmd;
  cmd.AddValue ("utsPerBeam", "Number of UTs per spot-beam", utsPerBeam);
  cmd.AddValue ("profilingConf", "Profiling configuration", profilingConf);
  cmd.Parse (argc, argv);

  // use 5 seconds store time for control messages
  Config::SetDefault ("ns3::SatBeamHelper::CtrlMsgStoreTimeInRtnLink", TimeValue (Seconds (5)));

  Config::SetDefault ("ns3::SatSuperframeConf0::FrameConfigType", StringValue("Config type 2"));
  Config::SetDefault ("ns3::SatWaveformConf::AcmEnabled", BooleanValue(true));

  Config::SetDefault ("ns3::SatStatsDelayHelper::MinValue", DoubleValue (0.0));
  Config::SetDefault ("ns3::SatStatsDelayHelper::MaxValue", DoubleValue (3.0));
  Config::SetDefault ("ns3::SatStatsDelayHelper::BinLength", DoubleValue (0.01));
  Config::SetDefault ("ns3::SatStatsThroughputHelper::MinValue", DoubleValue (0.0));
  Config::SetDefault ("ns3::SatStatsThroughputHelper::MaxValue", DoubleValue (400.0));
  Config::SetDefault ("ns3::SatStatsThroughputHelper::BinLength", DoubleValue (4.0));

  Config::SetDefault ("ns3::SatLowerLayerServiceConf::DaService3_ConstantAssignmentProvided", BooleanValue(false));
  Config::SetDefault ("ns3::SatLowerLayerServiceConf::DaService3_RbdcAllowed", BooleanValue(true));
  Config::SetDefault ("ns3::SatLowerLayerServiceConf::DaService3_MinimumServiceRate", UintegerValue(64));
  Config::SetDefault ("ns3::SatLowerLayerServiceConf::DaService3_VolumeAllowed", BooleanValue(false));

  // Creating the reference system. Note, currently the satellite module supports
  // only one reference system, which is named as "Scenario72". The string is utilized
  // in mapping the scenario to the needed reference system configuration files. Arbitrary
  // scenario name results in fatal error.
  std::string scenarioName = "Scenario72";

  Ptr<SatHelper> helper = CreateObject<SatHelper> (scenarioName);

  switch (profilingConf)
  {
    // Single beam
    case 0:
      {
        // Spot-beam over Finland
        uint32_t beamId = 18;
        simLength = 60.0; // in seconds

        // create user defined scenario
        std::map<uint32_t, SatBeamUserInfo > beamMap;
        SatBeamUserInfo beamInfo = SatBeamUserInfo (utsPerBeam, endUsersPerUt);
        beamMap[beamId] = beamInfo;
        helper->CreateUserDefinedScenario (beamMap);
        break;
      }
    // Full
    case 1:
      {
        simLength = 30.0; // in seconds
        helper->CreatePredefinedScenario (SatHelper::FULL);
        break;
      }
    default:
      {
        NS_FATAL_ERROR ("Invalid profiling configuration");
      }
  }

  // get users
  NodeContainer utUsers = helper->GetUtUsers();
  NodeContainer gwUsers = helper->GetGwUsers();

  // port used for packet delivering
  uint16_t port = 9; // Discard port (RFC 863)
  const std::string protocol = "ns3::UdpSocketFactory";

  /**
   * Set-up CBR traffic
   */
  const InetSocketAddress gwAddr = InetSocketAddress (helper->GetUserAddress (gwUsers.Get (0)), port);

  for (NodeContainer::Iterator itUt = utUsers.Begin ();
      itUt != utUsers.End ();
      ++itUt)
    {
      appStartTime += MilliSeconds (10);

      // return link
      Ptr<CbrApplication> rtnApp = CreateObject<CbrApplication> ();
      rtnApp->SetAttribute ("Protocol", StringValue (protocol));
      rtnApp->SetAttribute ("Remote", AddressValue (gwAddr));
      rtnApp->SetAttribute ("PacketSize", UintegerValue (packetSize));
      rtnApp->SetAttribute ("Interval", TimeValue (Seconds (intervalSeconds)));
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
  Ptr<SatStatsHelperContainer> s = CreateObject<SatStatsHelperContainer> (helper);

  s->AddPerBeamRtnAppThroughput (SatStatsHelper::OUTPUT_SCATTER_FILE);
  s->AddPerBeamRtnAppThroughput (SatStatsHelper::OUTPUT_SCATTER_PLOT);
  s->AddPerBeamRtnAppThroughput (SatStatsHelper::OUTPUT_SCALAR_FILE);
  s->AddPerBeamRtnDevThroughput (SatStatsHelper::OUTPUT_SCALAR_FILE);
  s->AddPerBeamRtnMacThroughput (SatStatsHelper::OUTPUT_SCALAR_FILE);
  s->AddPerBeamRtnPhyThroughput (SatStatsHelper::OUTPUT_SCALAR_FILE);
  s->AddPerBeamRtnAppDelay (SatStatsHelper::OUTPUT_SCALAR_FILE);
  s->AddPerBeamFrameLoad (SatStatsHelper::OUTPUT_SCALAR_FILE);

  NS_LOG_INFO("--- sat-profiling-sim-tn8 ---");
  NS_LOG_INFO("  Packet size: " << packetSize);
  NS_LOG_INFO("  Simulation length: " << simLength);
  NS_LOG_INFO("  Number of UTs: " << utsPerBeam);
  NS_LOG_INFO("  Number of end users per UT: " << endUsersPerUt);
  NS_LOG_INFO("  ");

  /**
   * Store attributes into XML output
   */
  std::stringstream filename;
  filename << "tn8-profiling-output-attributes-conf-" << profilingConf << "-uts-" << utsPerBeam << ".xml";

//  Config::SetDefault ("ns3::ConfigStore::Filename", StringValue (filename.str ()));
//  Config::SetDefault ("ns3::ConfigStore::FileFormat", StringValue ("Xml"));
//  Config::SetDefault ("ns3::ConfigStore::Mode", StringValue ("Save"));
//  ConfigStore outputConfig;
//  outputConfig.ConfigureDefaults ();
//  outputConfig.ConfigureAttributes ();

  /**
   * Install simple stdout time ticker
   */

  double t = 0.0;
  while (t <= simLength)
    {
      Simulator::Schedule (Seconds (t), &TimeTickerCallback);
      t = t + 1.0;
    }

  /**
   * Run simulation
   */
  Simulator::Stop (Seconds (simLength));
  Simulator::Run ();

  Simulator::Destroy ();

  return 0;
}
