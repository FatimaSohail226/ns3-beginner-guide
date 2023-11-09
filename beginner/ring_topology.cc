#include "ns3/core-module.h"           // Core module for basic functionality
#include "ns3/point-to-point-module.h" // Point-to-Point module for P2P connections
#include "ns3/network-module.h"        // Network module for networking components
#include "ns3/internet-module.h"       // Internet module for IP address assignment

using namespace ns3;

int main()
{
    // Create a container to hold five network nodes
    NodeContainer nodes;
    uint32_t numNodes = 5; // Specify the number of nodes in the ring
    nodes.Create(numNodes);

    // Create a Point-to-Point link helper
    PointToPointHelper p2p;
    // Set attributes of the Point-to-Point link:
    // - DataRate: Specifies the link data rate as 5 Mbps
    // - Delay: Specifies the link delay as 2 ms
    p2p.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    p2p.SetChannelAttribute("Delay", StringValue("2ms"));
    
    // Install the Internet stack on the nodes
    InternetStackHelper stack;
    stack.Install(nodes);

    // Define IP address and subnet mask for the network
    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");

    // Create a ring topology where each node is connected to two other nodes
    for (uint32_t i = 0; i < numNodes; i++)
    {
        // Establish Point-to-Point connections in a ring
        uint32_t j = (i + 1) % numNodes;
        NetDeviceContainer devices = p2p.Install(nodes.Get(i), nodes.Get(j));
        
        // Assign IP addresses to the devices based on the defined addressing scheme
        Ipv4InterfaceContainer interfaces = address.Assign(devices);
    }

    // Start the network simulation
    Simulator::Run();

    // Optional: Stop the simulation after a specified duration (e.g., 1 second)
    Simulator::Stop(Seconds(1.0));

    // Clean up and destroy the simulation
    Simulator::Destroy();

    return 0;
}
