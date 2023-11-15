#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/applications-module.h"
using namespace ns3;

// Define the number of nodes
uint32_t numNodes = 2;

// Function to get the position and velocity of nodes
void GetPositionAndVelocity(NodeContainer nodes)
{
    Ptr<ConstantVelocityMobilityModel> mobModel;

    // Loop through each node
    for (uint32_t i = 0; i < numNodes; i++)
    {
        // Get the mobility model for the current node
        mobModel = nodes.Get(i)->GetObject<ConstantVelocityMobilityModel>();
        
        // Get the position and velocity of the node
        Vector pos = mobModel->GetPosition();
        Vector vel = mobModel->GetVelocity();
        
        // Print the position and velocity of the node
        std::cout << "Position of node: " << i << " at time: " << Simulator::Now().GetSeconds() << " is: " << pos << std::endl;
        std::cout << "Velocity of node: " << i << " at time: " << Simulator::Now().GetSeconds() << " is: " << vel << std::endl;
    }
    
    // Schedule this function to run again after 1 second
    Simulator::Schedule(Seconds(1.0), &GetPositionAndVelocity, nodes);
}

int main()
{
    // Create a container for nodes
    NodeContainer nodes;
    nodes.Create(numNodes);
    
    // Create a mobility helper
    MobilityHelper mobility;
    
    // Set the mobility model to ConstantVelocityMobilityModel
    mobility.SetMobilityModel("ns3::ConstantVelocityMobilityModel");
    
    // Install the mobility model on the nodes
    mobility.Install(nodes);
    
    Ptr<ConstantVelocityMobilityModel> mobModel;
    
    // Initialize the position and velocity of each node
    for (uint32_t i = 0; i < numNodes; i++)
    {
        mobModel = nodes.Get(i)->GetObject<ConstantVelocityMobilityModel>();
        mobModel->SetPosition(Vector(i, 0, 0));
        mobModel->SetVelocity(Vector(5, 0, 0));
    }
    
    // Schedule the GetPositionAndVelocity function to run after 1 second and repeat
    Simulator::Schedule(Seconds(1.0), &GetPositionAndVelocity, nodes);
    
    // Stop the simulation after 5 seconds
    Simulator::Stop(Seconds(5.0));
    
    // Run the simulation
    Simulator::Run();
    
    // Clean up and destroy the simulation
    Simulator::Destroy();

    return 0;
}
