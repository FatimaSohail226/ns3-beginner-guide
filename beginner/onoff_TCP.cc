#include "ns3/core-module.h"           // Core module, provides essential NS-3 classes and functions
#include "ns3/network-module.h"        // Network module, includes classes for network topology and devices
#include "ns3/internet-module.h"       // Internet module, required for IP addressing, routing, and transport protocols
#include "ns3/point-to-point-module.h" // Point-to-Point module, for creating point-to-point links between nodes
#include "ns3/applications-module.h"   // Applications module, provides application layer traffic generators and sinks

using namespace ns3; // Simplify code by using the ns3 namespace

int main (int argc, char *argv[])
{
    // Create a container for network nodes
    NodeContainer nodes;
    nodes.Create (2); // Creates two nodes in the simulation

    // Configure and set up a point-to-point channel between nodes
    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps")); // Set the data rate for the link to 5 Mbps
    pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));     // Set the link delay to 2 milliseconds

    // Install network devices (like NICs) on the nodes, using the point-to-point link configuration
    NetDeviceContainer devices = pointToPoint.Install (nodes);

    // Install the Internet stack (TCP, UDP, IP, etc.) on the nodes
    InternetStackHelper stack;
    stack.Install (nodes);

    // Set up IP address assignment for the devices (network interfaces)
    Ipv4AddressHelper address;
    address.SetBase ("10.1.1.0", "255.255.255.0"); // Define the IP address range and subnet
    Ipv4InterfaceContainer interfaces = address.Assign (devices); // Assign IP addresses to the devices

    uint16_t port = 9; // Define the port number to be used by the TCP applications

    // Set up a TCP server using PacketSink on Node 0
    PacketSinkHelper packetSinkHelper("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port));
    ApplicationContainer serverApps = packetSinkHelper.Install(nodes.Get(0)); // Install the server application on Node 0
    serverApps.Start(Seconds(1.0)); // Start the server application 1 second into the simulation
    serverApps.Stop(Seconds(10.0)); // Stop the server application 10 seconds into the simulation

    // Set up a TCP client using OnOffHelper on Node 1
    OnOffHelper onOffHelper("ns3::TcpSocketFactory", InetSocketAddress(interfaces.GetAddress(0), port));
    onOffHelper.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]")); // Duration of 'On' period (1 second)
    onOffHelper.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]")); // Duration of 'Off' period (0 seconds, continuous transmission)
    onOffHelper.SetAttribute("DataRate", DataRateValue(DataRate("5Mbps"))); // Data rate for the 'On' period
    onOffHelper.SetAttribute("PacketSize", UintegerValue(1024)); // Size of packets generated

    ApplicationContainer clientApps = onOffHelper.Install(nodes.Get(1)); // Install the client application on Node 1
    clientApps.Start(Seconds(2.0)); // Start the client application 2 seconds into the simulation
    clientApps.Stop(Seconds(10.0)); // Stop the client application 10 seconds into the simulation

    // Execute the simulation
    Simulator::Run();
    Simulator::Destroy(); // Clean up after the simulation ends

    return 0;
}
