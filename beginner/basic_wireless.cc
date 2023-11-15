#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/wifi-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"

using namespace ns3;

int main()
{
    // Enable logging for UdpEchoClientApplication and UdpEchoServerApplication
    LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);

    // Create a container for network nodes
    NodeContainer nodes;
    uint32_t numNodes = 2;
    nodes.Create(numNodes);

    // Set up mobility for the nodes
    MobilityHelper mobility;
    mobility.SetMobilityModel("ns3::ConstantVelocityMobilityModel");
    mobility.Install(nodes);

    // Set initial positions and velocities for the nodes
    Ptr<ConstantVelocityMobilityModel> mobModel;
    for (uint32_t i = 0; i < numNodes; i++)
    {
        mobModel = nodes.Get(i)->GetObject<ConstantVelocityMobilityModel>();
        mobModel->SetPosition(Vector(i, 0, 0));
        mobModel->SetVelocity(Vector(5, 0, 0));
    }

    // Install the internet protocol stack on the nodes
    InternetStackHelper stack;
    stack.Install(nodes);

    // Set up WiFi parameters
    WifiHelper wifi;
    wifi.SetStandard(WIFI_PHY_STANDARD_80211b);

    YansWifiPhyHelper wifiPhy = YansWifiPhyHelper::Default();
    wifiPhy.Set("TxPowerStart", DoubleValue(20));
    wifiPhy.Set("TxPowerEnd", DoubleValue(20));

    WifiMacHelper wifiMac;
    YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default();
    wifiPhy.SetChannel(wifiChannel.Create());

    // Install WiFi devices on the nodes
    NetDeviceContainer devices = wifi.Install(wifiPhy, wifiMac, nodes);

    // Set up IPv4 addressing
    Ipv4AddressHelper ipv4;
    ipv4.SetBase("10.0.0.0", "255.0.0.0");
    Ipv4InterfaceContainer interfaces = ipv4.Assign(devices);

    // Setup UDP Echo Server on Node 0
    UdpEchoServerHelper echoServer(9);
    ApplicationContainer serverApps = echoServer.Install(nodes.Get(0));
    serverApps.Start(Seconds(1.0));
    serverApps.Stop(Seconds(10.0));

    // Setup UDP Echo Client on Node 1
    UdpEchoClientHelper echoClient(interfaces.GetAddress(0), 9);
    echoClient.SetAttribute("MaxPackets", UintegerValue(1));
    echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));
    echoClient.SetAttribute("PacketSize", UintegerValue(1024));

    ApplicationContainer clientApps = echoClient.Install(nodes.Get(1));
    clientApps.Start(Seconds(2.0));
    clientApps.Stop(Seconds(9.0));

    // Run the simulation
    Simulator::Run();
    Simulator::Destroy();
    return 0;
}
