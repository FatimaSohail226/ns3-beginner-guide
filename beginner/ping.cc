#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/v4ping-helper.h"


using namespace ns3;

int main ()
{

  // Create two nodes
  NodeContainer nodes;
  nodes.Create (2);

  // Install Internet stack
  InternetStackHelper stack;
  stack.Install (nodes);

  // Create and install network devices (Point-to-Point links)
  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer devices;
  devices = pointToPoint.Install (nodes);

  // Assign IP addresses
  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer interfaces = address.Assign (devices);

  // Create and configure the ping application
  V4PingHelper ping (interfaces.GetAddress (1)); // Ping the second node
  ping.SetAttribute ("Verbose", BooleanValue (true));

  // Install the application on the first node
  ApplicationContainer apps = ping.Install (nodes.Get (0));
  apps.Start (Seconds (1.0));
  apps.Stop (Seconds (5.0));

  // Run the simulation
  Simulator::Run ();
  Simulator::Destroy ();

  return 0;
}
