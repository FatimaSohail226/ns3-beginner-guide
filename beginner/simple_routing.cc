#include "ns3/core-module.h"           // Core NS-3 module, necessary for all simulations
#include "ns3/network-module.h"        // Network module, provides classes for creating network topologies
#include "ns3/internet-module.h"       // Internet module, required for IP layer functionalities
#include "ns3/point-to-point-module.h" // Point-to-Point module, offers functionalities to create point-to-point links
#include "ns3/applications-module.h"   // Applications module, contains application-layer traffic generators and sinks

using namespace ns3; // Namespace declaration to simplify syntax

int main ()
{
    // Enable logging for the UDP Echo Client and Server applications
    // This will help in tracing their activities during the simulation
    LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

    // Create a container for 3 nodes in the network
    NodeContainer nodes;
    nodes.Create (3); // Instantiates 3 nodes

    // Configure the attributes for the point-to-point links in the network
    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps")); // Set data rate for the link
    pointToPoint.SetChannelAttribute ("Delay", StringValue ("1ms"));     // Set propagation delay for the link

    // Create two point-to-point links: one between nodes A and B, and another between nodes B and C
    NetDeviceContainer devicesAB = pointToPoint.Install (nodes.Get(0), nodes.Get(1));
    NetDeviceContainer devicesBC = pointToPoint.Install (nodes.Get(1), nodes.Get(2));

    // Install the Internet stack (IP, TCP, UDP, etc.) on the nodes
    InternetStackHelper stack;
    stack.Install (nodes);

    // Assign IP addresses to the point-to-point devices (interfaces) on each link
    Ipv4AddressHelper address;
    address.SetBase ("10.1.1.0", "255.255.255.0"); // Set base IP and subnet mask for first link
    Ipv4InterfaceContainer interfacesAB = address.Assign (devicesAB);
    address.SetBase ("10.1.2.0", "255.255.255.0"); // Set base IP and subnet mask for second link
    Ipv4InterfaceContainer interfacesBC = address.Assign (devicesBC);

    // Set up a UDP Echo server application on node C (the third node)
    uint16_t port = 9; // Port number for the Echo server
    UdpEchoServerHelper echoServer (port); // Instantiate the Echo server with the specified port
    ApplicationContainer serverApps = echoServer.Install (nodes.Get (2)); // Install server on node C
    serverApps.Start (Seconds (1.0)); // Start server at 1 second into the simulation
    serverApps.Stop (Seconds (10.0)); // Stop server at 10 seconds into the simulation

    // Set up a UDP Echo client application on node A (the first node)
    // This client will send packets to the server application on node C
    UdpEchoClientHelper echoClient (interfacesBC.GetAddress (1), port); // Configure client to connect to server's IP and port
    echoClient.SetAttribute ("MaxPackets", UintegerValue (1)); // Set maximum number of packets to send
    echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0))); // Set interval between packets
    echoClient.SetAttribute ("PacketSize", UintegerValue (1024)); // Set packet size in bytes

    ApplicationContainer clientApps = echoClient.Install (nodes.Get (0)); // Install client on node A
    clientApps.Start (Seconds (2.0)); // Start client at 2 seconds into the simulation
    clientApps.Stop (Seconds (10.0)); // Stop client at 10 seconds into the simulation

    // Enable global routing which will populate routing tables for the nodes
    // This step is crucial for packet forwarding between nodes
    Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

    // Execute the simulation
    Simulator::Run (); // Run the simulation
    Simulator::Destroy (); // Cleanup after simulation ends

    return 0;
}
