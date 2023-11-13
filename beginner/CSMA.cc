#include "ns3/core-module.h"          // Core ns-3 modules
#include "ns3/network-module.h"       // Network related modules
#include "ns3/internet-module.h"      // Internet protocols and applications
#include "ns3/csma-module.h"          // Use CSMA (Carrier Sense Multiple Access) model
#include "ns3/applications-module.h"   // Network applications

using namespace ns3;

int main ()
{
    // Enable logging for the UDP Echo Client and Server applications
    LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);
    
    // Create a container for network nodes
    NodeContainer nodes;
    nodes.Create(3);  // Create 3 network nodes

    // Create a CSMA network helper
    CsmaHelper csma;
    csma.SetChannelAttribute("DataRate", StringValue("100Mbps"));  // Set the data rate for the CSMA channel
    csma.SetChannelAttribute("Delay", TimeValue(NanoSeconds(6560)));  // Set the delay for the CSMA channel

    // Install CSMA devices on the nodes
    NetDeviceContainer devices = csma.Install(nodes);

    // Install the internet protocol stack on the nodes
    InternetStackHelper stack;
    stack.Install(nodes);

    // Configure IPv4 addresses for the devices
    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");  // Define the IP address range and subnet mask
    Ipv4InterfaceContainer interfaces = address.Assign(devices);  // Assign IP addresses to devices

    // Configure and install a UDP Echo Server application on node 2
    uint16_t port = 9;
    UdpEchoServerHelper echoServer(port);
    ApplicationContainer serverApps = echoServer.Install(nodes.Get(2));  // Install the server on node 2
    serverApps.Start(Seconds(1.0));  // Start the server application after 1 second
    serverApps.Stop(Seconds(10.0));  // Stop the server application after 10 seconds

    // Configure and install a UDP Echo Client application on node 0
    UdpEchoClientHelper echoClient(interfaces.GetAddress(2), port);  // Specify the server IP and port
    echoClient.SetAttribute("MaxPackets", UintegerValue(10));  // Set maximum packets to send
    echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));  // Set packet sending interval
    echoClient.SetAttribute("PacketSize", UintegerValue(1024));  // Set packet size
    ApplicationContainer clientApps = echoClient.Install(nodes.Get(0));  // Install the client on node 0
    clientApps.Start(Seconds(2.0));  // Start the client application after 2 seconds
    clientApps.Stop(Seconds(10.0));  // Stop the client application after 10 seconds

    // Configure the simulation end time and run the simulator
    Simulator::Stop(Seconds(12.0));  // Set simulation end time to 12 seconds
    Simulator::Run();  // Run the ns-3 simulation
    Simulator::Destroy();  // Clean up and destroy the simulator

    return 0;
}
