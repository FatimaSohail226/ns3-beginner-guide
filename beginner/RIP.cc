#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/rip-helper.h"
#include "ns3/applications-module.h"

using namespace ns3;

int main ()
{
    // Enable logging for the UDP Echo Client and Server applications
    LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
    
    // Create a network simulation environment with 3 nodes
    NodeContainer nodes;
    nodes.Create (3);

    // Set up point-to-point links between nodes with specific attributes
    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
    pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

    NetDeviceContainer devicesAB = pointToPoint.Install (nodes.Get (0), nodes.Get (1));
    NetDeviceContainer devicesBC = pointToPoint.Install (nodes.Get (1), nodes.Get (2));

    // Set up RIP routing and install it on all nodes
    RipHelper ripRouting;
    InternetStackHelper stack;
    stack.SetRoutingHelper (ripRouting); // Use RIP routing
    stack.Install (nodes);

    // Assign IP addresses to devices
    Ipv4AddressHelper address;
    address.SetBase ("10.1.1.0", "255.255.255.0");
    address.Assign (devicesAB);
    address.Assign (devicesBC);

    // Get interfaces for each point-to-point link
    Ipv4InterfaceContainer interfacesAB = address.Assign(devicesAB);
    Ipv4InterfaceContainer interfacesBC = address.Assign(devicesBC);

    uint16_t port = 9;
    
    // Set up the UDP Echo server on Node C
    UdpEchoServerHelper echoServer(port);
    ApplicationContainer serverApps = echoServer.Install(nodes.Get(2)); // Node C
    serverApps.Start(Seconds(1.0));
    serverApps.Stop(Seconds(10.0));

    // Set up the UDP Echo client on Node A
    UdpEchoClientHelper echoClient(interfacesBC.GetAddress(1), port);
    echoClient.SetAttribute("MaxPackets", UintegerValue(10));
    echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));
    echoClient.SetAttribute("PacketSize", UintegerValue(1024));
    ApplicationContainer clientApps = echoClient.Install(nodes.Get(0)); // Node A
    clientApps.Start(Seconds(2.0));
    clientApps.Stop(Seconds(10.0));
    
    // Set simulation stop time
    Simulator::Stop(Seconds(12));

    // Run the simulation
    Simulator::Run ();
    Simulator::Destroy ();
    
    return 0;
}
