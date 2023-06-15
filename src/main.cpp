#include "SharedMemory/b3RobotSimulatorClientAPI_NoDirect.h"
#include "SharedMemory/PhysicsClientSharedMemory_C_API.h"
#include "SharedMemory/b3RobotSimulatorClientAPI_InternalData.h"
#include "Bullet3Common/b3Vector3.h"
#include "Bullet3Common/b3Quaternion.h"
#include "Bullet3Common/b3HashMap.h"
#include <Eigen/Dense>
#include "Utils/b3Clock.h"

#include <vector>
#include <iostream>
#include "FTread.h"


using namespace std;
using namespace Eigen;


extern const int CONTROL_RATE;
const int CONTROL_RATE = 2000;
const b3Scalar FIXED_TIMESTEP = 1.0 / ((b3Scalar)CONTROL_RATE);
b3SharedMemoryCommandHandle command;
int statusType, ret;


int main()
{
    FTread ft = FTread(0);

    b3PhysicsClientHandle client = b3ConnectSharedMemory(SHARED_MEMORY_KEY);
    if (!b3CanSubmitCommand(client))
    {
        printf("Not connected, start a PyBullet server first, using python -m pybullet_utils.runServer\n");
        exit(0);
    }
    b3RobotSimulatorClientAPI_InternalData data;
    data.m_physicsClientHandle = client;
    data.m_guiHelper = 0;
    b3RobotSimulatorClientAPI_NoDirect sim;
    sim.setInternalData(&data);
    

    sim.resetSimulation();
    sim.setGravity( btVector3(0 , 0 ,-9.8));
    int plane = sim.loadURDF("plane.urdf");

    sim.setTimeStep(FIXED_TIMESTEP);
    double t = 0;
    while(1){
        ft.readData();
        cout<<ft.FT.transpose()<<endl;
    	sim.stepSimulation();	
	    b3Clock::usleep(1000. * 1000. * FIXED_TIMESTEP);
	    t = t+FIXED_TIMESTEP;	
    }
}

