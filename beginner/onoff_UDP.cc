#include "ns3/core-module.h"           // Core NS-3 module, includes functionality like logging and simulation control
#include "ns3/network-module.h"        // Network module, provides basic network-related classes
#include "ns3/internet-module.h"       // Internet module, necessary for internet protocols, IP addressing, etc.
#include "ns3/point-to-point-module.h" // Point-to-Point module, used to setup point-to-point links
#include "ns3/applications-module.h"   // Applications module, includes application layer protocols and traffic generators

using namespace ns3; // Use the ns3 namespace to avoid typing 'ns3::' before each class

int main (int argc, char *argv[])
{
    // Create a container for nodes in the network
    NodeContainer nodes;
    nodes.Create (2); // Creates 2 nodes in the network

    // Set up a Point-to-Point connection between the nodes
    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps")); // Data rate for the link is 5 Mbps
    pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));     // Delay for the link is 2 milliseconds

    // Install the Point-to-Point devices (like network cards) on the nodes
    NetDeviceContainer devices = pointToPoint.Install (nodes);

    // Install the Internet stack (TCP, UDP, IP, etc.) on the nodes
    InternetStackHelper stack;
    stack.Install (nodes);

    // Assign IP addresses to the network interfaces (devices) on the nodes
    Ipv4AddressHelper address;
    address.SetBase ("10.1.1.0", "255.255.255.0"); // Network base address and subnet mask
    Ipv4InterfaceContainer interfaces = address.Assign (devices); // Assigns the IP addresses

    uint16_t port = 9; // Define the port number for the application (same for server and client)

    // Setup a UDP PacketSink application on the first node (Node 0)
    // This application will act as a server, listening for incoming packets
    PacketSinkHelper packetSinkHelper("ns3::UdpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port));
    ApplicationContainer serverApps = packetSinkHelper.Install(nodes.Get(0)); // Install the server application on Node 0
    serverApps.Start(Seconds(1.0)); // Start the server application at 1 second into the simulation
    serverApps.Stop(Seconds(10.0)); // Stop the server application at 10 seconds into the simulation

    // Setup a UDP OnOff application on the second node (Node 1)
    // This application will generate traffic in an On/Off pattern
    OnOffHelper onOffHelper("ns3::UdpSocketFactory", InetSocketAddress(interfaces.GetAddress(0), port));
    onOffHelper.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]")); // Duration of 'On' period
    onOffHelper.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=1]")); // Duration of 'Off' period
    onOffHelper.SetAttribute("DataRate", DataRateValue(DataRate("1Mbps"))); // Data rate during 'On' period
    onOffHelper.SetAttribute("PacketSize", UintegerValue(1024)); // Size of packets generated

    ApplicationContainer clientApps = onOffHelper.Install(nodes.Get(1)); // Install the client application on Node 1
    clientApps.Start(Seconds(2.0)); // Start the client application at 2 seconds into the simulation
    clientApps.Stop(Seconds(10.0)); // Stop the client application at 10 seconds into the simulation

    // Run the simulation
    Simulator::Run();
    // Clean up and destroy the simulation
    Simulator::Destroy();

    return 0;
}
