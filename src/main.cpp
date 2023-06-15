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
#include <iomanip>


using namespace std;
using namespace Eigen;


extern const int CONTROL_RATE;
const int CONTROL_RATE = 2000;
const b3Scalar FIXED_TIMESTEP = 1.0 / ((b3Scalar)CONTROL_RATE);
b3SharedMemoryCommandHandle command;
int statusType, ret;


int main()
{
    int channel =0;
    int canId = 1;
    FTread ft = FTread(channel,canId);
    ft.setCutOffFreq(105);

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
    sim.setGravity( btVector3(0 , 0 ,0));
    int plane = sim.loadURDF("plane.urdf");
    int satellite = sim.loadURDF("model/satellite.urdf");
    btVector3 base_pos(0,0,1);
    btQuaternion base_orn(0,0,0,1);
    sim.resetBasePositionAndOrientation(satellite,base_pos,base_orn);
    //sim.enableJointForceTorqueSensor(satellite,1,1);
    sim.setTimeStep(FIXED_TIMESTEP);
    double t = 0;
    btVector3 position;
    btVector3 force;
    position[0] =0;
    position[1] =0;
    position[2] =0;
    force[0] = 0;
    force[1] = 0;
    force[2] = 0;

    while(1){
        ft.readData();
        ft.print(ft.filtered_FT);
        
        force[0]=ft.filtered_FT(0);
        force[1]=ft.filtered_FT(1);
        force[2]=ft.filtered_FT(2);
        sim.applyExternalForce(satellite,-1,force,position,EF_LINK_FRAME);
    	sim.stepSimulation();	
	    b3Clock::usleep(1000. * 1000. * FIXED_TIMESTEP);
	    t = t+FIXED_TIMESTEP;	
    }
}

