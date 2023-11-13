#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"

using namespace ns3;

int main ()
{
    // Enable logging for the UDP Echo Client and Server applications
    LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

    // Create a container for network nodes
    NodeContainer nodes;
    nodes.Create (3);

    // Create a point-to-point network helper
    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps")); // Set data rate to 5 Mbps
    pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));     // Set channel delay to 2 ms

    // Install point-to-point devices between nodes
    NetDeviceContainer devicesAB = pointToPoint.Install (nodes.Get (0), nodes.Get (1));
    NetDeviceContainer devicesBC = pointToPoint.Install (nodes.Get (1), nodes.Get (2));

    // Install Internet stack on nodes
    InternetStackHelper stack;
    stack.Install (nodes);

    // Set up IP addresses for the point-to-point links
    Ipv4AddressHelper address;
    address.SetBase ("10.1.1.0", "255.255.255.0"); // Define IP address range
    address.Assign (devicesAB);                     // Assign IP addresses to devices
    address.Assign (devicesBC);
    Ipv4InterfaceContainer interfacesAB = address.Assign(devicesAB);
    Ipv4InterfaceContainer interfacesBC = address.Assign(devicesBC);

    // Set up static routes for communication between nodes A and C
    Ptr<Ipv4StaticRouting> staticRoutingA = Ipv4RoutingHelper::GetRouting<Ipv4StaticRouting>(nodes.Get(0)->GetObject<Ipv4>()->GetRoutingProtocol());
    Ptr<Ipv4StaticRouting> staticRoutingC = Ipv4RoutingHelper::GetRouting<Ipv4StaticRouting>(nodes.Get(2)->GetObject<Ipv4>()->GetRoutingProtocol());

    // Add host routes to enable direct communication
    staticRoutingA->AddHostRouteTo(Ipv4Address("10.1.1.3"), Ipv4Address("10.1.1.2"), 1);
    staticRoutingC->AddHostRouteTo(Ipv4Address("10.1.1.1"), Ipv4Address("10.1.1.2"), 1);

    // Set up a UDP Echo Server on node C
    uint16_t port = 9;
    UdpEchoServerHelper echoServer(port);
    ApplicationContainer serverApps = echoServer.Install(nodes.Get(2)); // Install the server application on Node C
    serverApps.Start(Seconds(1.0));                                     // Start the server at 1 second
    serverApps.Stop(Seconds(10.0));                                     // Stop the server at 10 seconds

    // Set up a UDP Echo Client on node A to communicate with node C
    UdpEchoClientHelper echoClient(interfacesBC.GetAddress(1), port);
    echoClient.SetAttribute("MaxPackets", UintegerValue(10));            // Set maximum packets to send
    echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));        // Set packet sending interval
    echoClient.SetAttribute("PacketSize", UintegerValue(1024));           // Set packet size
    ApplicationContainer clientApps = echoClient.Install(nodes.Get(0));   // Install the client application on Node A
    clientApps.Start(Seconds(2.0));                                       // Start the client at 2 seconds
    clientApps.Stop(Seconds(10.0));                                       // Stop the client at 10 seconds

    Simulator::Stop(Seconds(12)); // Stop the simulation at 12 seconds

    Simulator::Run();    // Run the simulation
    Simulator::Destroy(); // Destroy the simulation objects

    return 0; // Exit the program
}
