#include <canlib.h>
#include <canstat.h>
#include <iostream>
#include <string>
#include <csignal>
#include <cerrno>
#include <unistd.h>
#include <cstdlib>
#include <ctime>
#include <Eigen/Dense>
#include <canlib.h>
#include <canstat.h>
#include <iostream>
#include <string>
#include <csignal>
#include <cerrno>
#include <unistd.h>
#include <cstdlib>
#include <ctime>
#define READ_WAIT_INFINITE (unsigned long)(-1)
static unsigned int msgCounter = 0;

using namespace Eigen;
using namespace std;
class FTread
{
    typedef Eigen::Matrix<double, 6, 1> Vector6d;   
    canHandle hnd;
    canStatus stat;
    int channel;
    struct sigaction sigact;    

    public:
        Vector6d FT;
        FTread(int channel);
        void readData();
};