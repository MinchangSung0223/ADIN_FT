#include "FTread.h"

void check(const std::string& id, canStatus stat)
{
    if (stat != canOK) {
        char buf[50];
        buf[0] = '\0';
        canGetErrorText(stat, buf, sizeof(buf));
        std::cout << id << ": failed, stat=" << stat << " (" << buf << ")\n";
    }
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
        //std::cout << "CAN Status Event: " << busStatToStr(data->info.status.busStatus) << "\n";
        break;

    case canEVENT_ERROR:
       // std::cout << "CAN Error Event\n";
        break;

    case canEVENT_TX:
        //std::cout << "CAN Tx Event\n";
        break;

    case canEVENT_RX:
        //std::cout << "CAN Rx Event\n";
        break;
    }
    return;
}

FTread::FTread(int channel){

        this->FT = Vector6d::Zero();
        this->channel = channel;
        canInitializeLibrary();
        hnd = canOpenChannel(channel,
                                canOPEN_EXCLUSIVE | canOPEN_REQUIRE_EXTENDED | canOPEN_ACCEPT_VIRTUAL);
        if (hnd < 0) {
            check("canOpenChannel", (canStatus)hnd);
        }
        stat = canSetNotify(hnd, notifyCallback,
                            canNOTIFY_RX | canNOTIFY_TX | canNOTIFY_ERROR | canNOTIFY_STATUS |
                                canNOTIFY_ENVVAR,
                            (char *)0);
        check("canSetNotify", stat);

        stat = canSetBusParams(hnd, canBITRATE_1M, 0, 0, 0, 0, 0);
        check("canSetBusParams", stat);
        if (stat != canOK) {
        
           cout<<"canSetBusParams Error"<<endl;
        }

        stat = canBusOn(hnd);
        check("canBusOn", stat);
        if (stat != canOK) {
            cout<<"canBus Error"<<endl;
        }

        long id = 0x102;
        unsigned char sendMsg[8] = {0x01, 0x03, 0x01,0x00,0x00,0x00,0x00,0x00};


        stat = canWrite(hnd, id, sendMsg, sizeof(sendMsg)/sizeof(sendMsg[0]), canMSG_STD);
        check("canWrite", stat);
        if (stat != canOK) {
            cout<<"canWrite Error"<<endl;
        }

}
void FTread::readData(){
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
                    this->FT(0) = (msg[0]*256+msg[1])/100.0-300.0;
                    this->FT(1) = (msg[2]*256+msg[3])/100.0-300.0;
                    this->FT(2) = (msg[4]*256+msg[5])/100.0-300.0;                        
                }else{
                    this->FT(3) = (msg[0]*256+msg[1])/500.0-50.0;
                    this->FT(4) = (msg[2]*256+msg[3])/500.0-50.0;
                    this->FT(5) = (msg[4]*256+msg[5])/500.0-50.0;                        
                }
                
            }
        } else {
            if (errno == 0) {
                check("\ncanReadWait", stat);
            } else {
                perror("\ncanReadWait error");
            }
        }

}