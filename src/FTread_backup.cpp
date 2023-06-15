/*
 * Kvaser Linux Canlib
 * Read CAN messages and print out their contents
 */

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

void check(const std::string& id, canStatus stat)
{
    if (stat != canOK) {
        char buf[50];
        buf[0] = '\0';
        canGetErrorText(stat, buf, sizeof(buf));
        std::cout << id << ": failed, stat=" << stat << " (" << buf << ")\n";
    }
}

void printUsageAndExit(const std::string& prgName)
{
    std::cout << "Usage: '" << prgName << " <channel>'\n";
    exit(1);
}

void sighand(int sig, siginfo_t *info, void *ucontext)
{
    (void)sig;
    (void)info;
    (void)ucontext;
}

std::string busStatToStr(const unsigned long flag)
{
    switch (flag) {
    case canSTAT_ERROR_PASSIVE:
        return "canSTAT_ERROR_PASSIVE";

    case canSTAT_BUS_OFF:
        return "canSTAT_BUS_OFF";

    case canSTAT_ERROR_WARNING:
        return "canSTAT_ERROR_WARNING";

    case canSTAT_ERROR_ACTIVE:
        return "canSTAT_ERROR_ACTIVE";

    default:
        return "";
    }
}

void notifyCallback(canNotifyData *data)
{
    switch (data->eventType) {
    case canEVENT_STATUS:
        std::cout << "CAN Status Event: " << busStatToStr(data->info.status.busStatus) << "\n";
        break;

    case canEVENT_ERROR:
        std::cout << "CAN Error Event\n";
        break;

    case canEVENT_TX:
        std::cout << "CAN Tx Event\n";
        break;

    case canEVENT_RX:
        std::cout << "CAN Rx Event\n";
        break;
    }
    return;
}

int main(int argc, char *argv[])
{
    canHandle hnd;
    canStatus stat;
    int channel;
    struct sigaction sigact;

    if (argc != 2) {
        printUsageAndExit(argv[0]);
    }

    channel = std::stoi(argv[1]);

    /* Use sighand and allow SIGINT to interrupt syscalls */
    sigact.sa_flags = SA_SIGINFO;
    sigemptyset(&sigact.sa_mask);
    sigact.sa_sigaction = sighand;
    if (sigaction(SIGINT, &sigact, NULL) != 0) {
        perror("sigaction SIGINT failed");
        return -1;
    }

    std::cout << "Reading CAN messages on channel " << channel << "\n";

    canInitializeLibrary();

    /* Open channel, set parameters and go on bus */
    hnd = canOpenChannel(channel,
                         canOPEN_EXCLUSIVE | canOPEN_REQUIRE_EXTENDED | canOPEN_ACCEPT_VIRTUAL);
    if (hnd < 0) {
        check("canOpenChannel", (canStatus)hnd);
        return -1;
    }

    stat = canSetNotify(hnd, notifyCallback,
                        canNOTIFY_RX | canNOTIFY_TX | canNOTIFY_ERROR | canNOTIFY_STATUS |
                            canNOTIFY_ENVVAR,
                        (char *)0);
    check("canSetNotify", stat);

    stat = canSetBusParams(hnd, canBITRATE_1M, 0, 0, 0, 0, 0);
    check("canSetBusParams", stat);
    if (stat != canOK) {
	return -1;
    }

    stat = canBusOn(hnd);
    check("canBusOn", stat);
    if (stat != canOK) {
	return -1;
    }

long id = 0x102;
    unsigned char sendMsg[8] = {0x01, 0x03, 0x01,0x00,0x00,0x00,0x00,0x00};


    stat = canWrite(hnd, id, sendMsg, sizeof(sendMsg)/sizeof(sendMsg[0]), canMSG_STD);
    check("canWrite", stat);
    if (stat != canOK) {
	return -1;
    }

    do {
        long id;
        unsigned char msg[8];
        unsigned int dlc;
        unsigned int flag;
        unsigned long time;

        stat = canReadWait(hnd, &id, &msg, &dlc, &flag, &time, READ_WAIT_INFINITE);

        if (stat == canOK) {
            msgCounter++;
            if (flag & canMSG_ERROR_FRAME) {
                std::cout << "(" << msgCounter << ") ERROR FRAME";
            } else {
                if(id==1){
                    std::cout << "FORCE -- ";
                    double Fx = (msg[0]*256+msg[1])/100.0-300.0;
                    double Fy = (msg[2]*256+msg[3])/100.0-300.0;
                    double Fz = (msg[4]*256+msg[5])/100.0-300.0;                        
                    std::cout << Fx << " " << Fy << " " << Fz;
                }else{
                    std::cout << "TORQUE -- ";
                    double Tx = (msg[0]*256+msg[1])/500.0-50.0;
                    double Ty = (msg[2]*256+msg[3])/500.0-50.0;
                    double Tz = (msg[4]*256+msg[5])/500.0-50.0;                        
                    std::cout << Tx << " " << Ty << " " << Tz;
                }
                
            }
            std::cout << " flags:0x" << flag << " time:" << time << "\n";
        } else {
            if (errno == 0) {
                check("\ncanReadWait", stat);
            } else {
                perror("\ncanReadWait error");
            }
        }

    } while (stat == canOK);



    return 0;
}

