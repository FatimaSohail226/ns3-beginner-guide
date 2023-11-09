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

    // Create and configure a UDP Echo client
    uint16_t port = 9; // Port number of the server the client will connect to
    UdpEchoClientHelper echoClient (interfaces.GetAddress (1), port); // Create a UDP client targeting the server's IP and port
    echoClient.SetAttribute ("MaxPackets", UintegerValue (1)); // Set the maximum number of packets the client will send
    echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0))); // Set the time interval between packets
    echoClient.SetAttribute ("PacketSize", UintegerValue (1024)); // Set the size of the packet the client will send

    // Install the client application on one of the nodes
    ApplicationContainer clientApps = echoClient.Install (nodes.Get (0));
    clientApps.Start (Seconds (2.0)); // Start the client application 2 seconds into the simulation
    clientApps.Stop (Seconds (10.0)); // Stop the client application 10 seconds into the simulation

    // Run the simulation
    Simulator::Run ();
    // Clean up and destroy the simulation
    Simulator::Destroy ();

    return 0;
}
