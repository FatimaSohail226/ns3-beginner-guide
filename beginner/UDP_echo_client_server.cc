#include "ns3/core-module.h"           // Core module for basic functionality like logging, randomness, and simulation control
#include "ns3/network-module.h"        // Network module for basic network-related classes like Node and NetDevice
#include "ns3/internet-module.h"       // Internet module for internet protocols, IP addresses, and sockets
#include "ns3/point-to-point-module.h" // Point-to-Point module to set up point-to-point links
#include "ns3/applications-module.h"   // Applications module for application layer protocols and traffic generation

using namespace ns3; // Use the ns3 namespace to simplify code (avoid repetitive "ns3::" prefix)

int main (int argc, char *argv[])
{
    // Create a container to hold two network nodes
    NodeContainer nodes;
    nodes.Create (2); // Creates 2 nodes

    // Set up a Point-to-Point connection
    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps")); // Set the data rate of the link to 5 Mbps
    pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));     // Set the delay of the link to 2 milliseconds

    // Install the Point-to-Point devices on the nodes
    NetDeviceContainer devices = pointToPoint.Install (nodes);

    // Install the internet stack (TCP, UDP, IP, etc.) on the nodes
    InternetStackHelper stack;
    stack.Install (nodes);

    // Assign IP addresses to the devices
    Ipv4AddressHelper address;
    address.SetBase ("10.1.1.0", "255.255.255.0"); // Sets the base IP address and subnet mask
    Ipv4InterfaceContainer interfaces = address.Assign (devices); // Assigns IP addresses to devices in the NetDeviceContainer

    uint16_t port = 9; // Port number for the UDP echo server

    // Setup UDP Echo Server on both nodes
    UdpEchoServerHelper echoServer (port);
    ApplicationContainer serverApps1 = echoServer.Install (nodes.Get (0));
    serverApps1.Start (Seconds (1.0)); // Start the server application on Node 0 at 1 second
    serverApps1.Stop (Seconds (5.0));  // Stop the server application on Node 0 at 5 seconds

    ApplicationContainer serverApps2 = echoServer.Install (nodes.Get (1));
    serverApps2.Start (Seconds (6.0)); // Start the server application on Node 1 at 6 seconds
    serverApps2.Stop (Seconds (10.0)); // Stop the server application on Node 1 at 10 seconds

    // Setup UDP Echo Client on both nodes
    UdpEchoClientHelper echoClient1 (interfaces.GetAddress (1), port);
    echoClient1.SetAttribute ("MaxPackets", UintegerValue (1)); // Node 0 client will send 1 packet
    echoClient1.SetAttribute ("Interval", TimeValue (Seconds (1.0))); // Node 0 client will send packets every 1 second
    echoClient1.SetAttribute ("PacketSize", UintegerValue (1024)); // Node 0 client will send packets of size 1024 bytes

    ApplicationContainer clientApps1 = echoClient1.Install (nodes.Get (0));
    clientApps1.Start (Seconds (2.0)); // Start the client application on Node 0 at 2 seconds
    clientApps1.Stop (Seconds (6.0));  // Stop the client application on Node 0 at 6 seconds

    UdpEchoClientHelper echoClient2 (interfaces.GetAddress (0), port);
    echoClient2.SetAttribute ("MaxPackets", UintegerValue (1)); // Node 1 client will send 1 packet
    echoClient2.SetAttribute ("Interval", TimeValue (Seconds (1.0))); // Node 1 client will send packets every 1 second
    echoClient2.SetAttribute ("PacketSize", UintegerValue (1024)); // Node 1 client will send packets of size 1024 bytes

    ApplicationContainer clientApps2 = echoClient2.Install (nodes.Get (1));
    clientApps2.Start (Seconds (5.0)); // Start the client application on Node 1 at 5 seconds
    clientApps2.Stop (Seconds (10.0)); // Stop the client application on Node 1 at 10 seconds

    // Run the simulation
    Simulator::Run ();
    // Clean up and destroy the simulation
    Simulator::Destroy ();

    return 0;
}
