#include "ns3/core-module.h"           // Core module for basic functionality
#include "ns3/point-to-point-module.h" // Point-to-Point module for P2P connections
#include "ns3/network-module.h"        // Network module for networking components
#include "ns3/internet-module.h"       // Internet module for IP address assignment

using namespace ns3;

int main()
{
    // Create a container to hold the network nodes, including the central hub
    NodeContainer nodes;
    uint32_t numNodes = 5; // Specify the number of peripheral nodes in the star
    nodes.Create(numNodes + 1); // +1 for the central hub node

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

    // The central hub node is the last node in the container
    Ptr<Node> centralNode = nodes.Get(numNodes);

    // Create a star topology with the central node connected to each peripheral node
    for (uint32_t i = 0; i < numNodes; i++)
    {
        // Establish Point-to-Point connections between central hub and peripheral nodes
        NetDeviceContainer devices = p2p.Install(centralNode, nodes.Get(i));
        
        // Assign IP addresses to the devices
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
