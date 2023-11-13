/* 
This NS-3 simulation sets up a simple network with five nodes connected in a linear topology (Node A-B-C-D-E).
The nodes are connected via point-to-point links. The simulation includes a UDP Echo server installed on node E
and a UDP Echo client installed on node A. The client sends packets to the server, which then echoes them back.
The client sends a total of 10 packets, one every second starting at 2 seconds and ending at 10 seconds into the simulation.
The simulation runs for 12 seconds in total.
*/

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"

using namespace ns3;

int main()
{
    // Enable logging for the UDP Echo Client and Server applications
    LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

    // Create 5 nodes
    uint32_t numNodes = 5;
    NodeContainer nodes;
    nodes.Create(numNodes);

    // Configure point-to-point links between nodes
    PointToPointHelper p2p;
    p2p.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    p2p.SetChannelAttribute("Delay", StringValue("1ms"));

    // Install point-to-point devices between nodes
    NetDeviceContainer devicesAB = p2p.Install(nodes.Get(0), nodes.Get(1));
    NetDeviceContainer devicesBC = p2p.Install(nodes.Get(1), nodes.Get(2));
    NetDeviceContainer devicesCD = p2p.Install(nodes.Get(2), nodes.Get(3));
    NetDeviceContainer devicesDE = p2p.Install(nodes.Get(3), nodes.Get(4));

    // Install the Internet stack on all nodes
    InternetStackHelper internet;
    internet.Install(nodes);

    // Assign IP addresses to point-to-point devices
    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer interfacesAB = address.Assign(devicesAB);
    Ipv4InterfaceContainer interfacesBC = address.Assign(devicesBC);
    Ipv4InterfaceContainer interfacesCD = address.Assign(devicesCD);
    Ipv4InterfaceContainer interfacesDE = address.Assign(devicesDE);

    // Populate routing tables
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    // Set up the UDP Echo server
    uint16_t port = 9;
    UdpEchoServerHelper echoServer(port);
    ApplicationContainer serverApps = echoServer.Install(nodes.Get(4)); // Node E
    serverApps.Start(Seconds(1.0));
    serverApps.Stop(Seconds(10.0));

    // Set up the UDP Echo client
    UdpEchoClientHelper echoClient(interfacesDE.GetAddress(1), port);
    echoClient.SetAttribute("MaxPackets", UintegerValue(10));
    echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));
    echoClient.SetAttribute("PacketSize", UintegerValue(1024));
    ApplicationContainer clientApps = echoClient.Install(nodes.Get(0)); // Node A
    clientApps.Start(Seconds(2.0));
    clientApps.Stop(Seconds(10.0));

    // Set the total duration of the simulation
    Simulator::Stop(Seconds(12));

    // Run the simulation
    Simulator::Run();
    Simulator::Destroy();

    return 0;
}
