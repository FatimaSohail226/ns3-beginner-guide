// Include necessary NS-3 header files for network simulation
#include "ns3/core-module.h"           // Core module for basic functionality
#include "ns3/point-to-point-module.h" // Point-to-Point module for P2P connections
#include "ns3/network-module.h"        // Network module for networking components
#include "ns3/internet-module.h"       // Internet module for IP address assignment

// Use the ns3 namespace to simplify code (avoid repetitive "ns3::" prefix)
using namespace ns3;

int main()
{
    // Create a container to hold two network nodes
    NodeContainer nodes;
    nodes.Create(2);

    // Create a Point-to-Point link helper
    PointToPointHelper p2p;

    // Set attributes of the Point-to-Point link:
    // - DataRate: Specifies the link data rate as 5 Mbps
    // - Delay: Specifies the link delay as 2 ms
    p2p.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    p2p.SetChannelAttribute("Delay", StringValue("2ms"));

    // Create a container to hold the Point-to-Point devices and install them on the nodes
    NetDeviceContainer devices;
    devices = p2p.Install(nodes);

    // Install the Internet stack on the nodes
    InternetStackHelper stack;
    stack.Install(nodes);

    // Define IP address and subnet mask for the network
    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");

    // Assign IP addresses to the devices based on the defined addressing scheme
    Ipv4InterfaceContainer interfaces = address.Assign(devices);

    // Start the network simulation
    Simulator::Run();

    // Stop the simulation after 1 second (optional)
    Simulator::Stop(Seconds(1.0));

    // Clean up and destroy the simulation
    Simulator::Destroy();

    return 0;
}
