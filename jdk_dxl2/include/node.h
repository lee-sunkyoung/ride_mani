/*
    Embedded Develoment Kit 2023 
    made by 
    16th Baek Jong Wook.
*/

#ifndef NODE_H
#define NODE_H

#include "header.h"

#define DEVICENAME "/dev/ttyUSB1"//pantilt dxl u2d2 portname, yellow molax pin
#define PROTOCOL_VERSION 2.0
#define BAUDRATE 115200 //여기
#define Hz 80
#define NUMBER_OF_MOTORS 2//

#define OPERATING_MODE 1 //1 = position control, 2 = speed control

#define MX_DEG2PULSE (1 / MX_PULSE2DEG)
//addr: address, len: 몇 바이트로 보내는지 
#define TORQUE_ENABLE 1
#define TORQUE_DISABLE 0
#define ADDR_MX_TORQUE_ENABLE 64
#define ADDR_MX_GOAL_ACCELERATION 108 // 해당 목표지점까지 가는데 걸리는 속도
#define LEN_MX_GOAL_ACCELERATION 4
#define ADDR_MX_GOAL_VELOCITY 112
#define LEN_MX_GOAL_VELOCITY 4
#define ADDR_MX_GOAL_POSITION 116
#define LEN_MX_GOAL_POSITION 4
#define ADDR_MX_MOVING_STATUS 123
#define LEN_MX_MOVING_STATUS 1

#define DEG2RAD (0.01745329251994329576923690768489)
#define RAD2DEG (1 / DEG2RAD)
#define MX_PULSE2DEG (0.087890625)
#define MX_DEG2PULSE (1 / MX_PULSE2DEG)

enum _controlFSM
{
    INIT_WRITE,
    MOVING_STATUS_READ,
    POSITION_WRITE,
} controlFSM;

vector<int> motorID;

bool serial_result = false;
int dxl_comm_result = COMM_TX_FAIL; // Communication result
bool dxl_addparam_result = false;   // addParam result
bool dxl_getdata_result = false;    // GetParam result
uint8_t dxl_error = 0;              // Dynamixel error

int32_t dxl_moving_status = 0;
uint8_t moving_status[NUMBER_OF_MOTORS];
int in_position_cnt = 0;

int dxl_goal_position = 0;
uint8_t param_goal_position[LEN_MX_GOAL_ACCELERATION + LEN_MX_GOAL_VELOCITY + LEN_MX_GOAL_POSITION];
double init_Position[NUMBER_OF_MOTORS] = {0,};
double goal_Position[NUMBER_OF_MOTORS];
double pulse_desired[NUMBER_OF_MOTORS];

uint8_t param_goal_speed[LEN_MX_GOAL_VELOCITY];

int acc;
int vel;

PID pan;
PID tilt;
double pan_input = 0;
double pan_target = 240;
double tilt_input = 0;
double tilt_target = 135;//윈도우의 중앙값

mutex global_mutex;

Subscriber init_sub;
Subscriber pantilt_sub;

void serialPortCheck(dynamixel::PortHandler *portHandler)
{
    if (portHandler->openPort())
    {
        if (portHandler->setBaudRate(BAUDRATE))
        {
            serial_result = true;
        }
        else
        {
            serial_result = false;
        }
    }
    else
    {
        serial_result = false;
    }
}

void motorEnable(int count)
{
    for (int i = 1; i <= count; i++)
    {
        motorID.push_back(i);
    }
}

void motorTorqueOn(dynamixel::PortHandler *portHandler, dynamixel::PacketHandler *packetHandler)
{
    for (int i = 0; i < motorID.size(); i++)
    {
        dxl_comm_result = packetHandler->write1ByteTxRx(portHandler, motorID[i], ADDR_MX_TORQUE_ENABLE, TORQUE_ENABLE, &dxl_error);

        if (dxl_comm_result != COMM_SUCCESS)
        {
            printf("%s\n", packetHandler->getTxRxResult(dxl_comm_result));
        }
        else if (dxl_error != 0)
        {
            printf("%s\n", packetHandler->getRxPacketError(dxl_error));
        }
        else
        {
            printf("Dynamixel#%d has been successfully connected \n", motorID[i]);
        }
    }
}

void motorTorqueOff(dynamixel::PortHandler *portHandler, dynamixel::PacketHandler *packetHandler)
{
    for (int i = 0; i < motorID.size(); i++)
    {
        dxl_comm_result = packetHandler->write1ByteTxRx(portHandler, motorID[i], ADDR_MX_TORQUE_ENABLE, TORQUE_DISABLE, &dxl_error);

        if (dxl_comm_result != COMM_SUCCESS)
        {
            printf("%s\n", packetHandler->getTxRxResult(dxl_comm_result));
        }
        else if (dxl_error != 0)
        {
            printf("%s\n", packetHandler->getRxPacketError(dxl_error));
        }
        else
        {
            printf("Dynamixel#%d has been successfully connected \n", motorID[i]);
        }
    }
}

void storageMovingStatus(dynamixel::GroupSyncRead *groupSyncRead) 
{
    for (int i = 0; i < motorID.size(); i++)
    {   
        dxl_addparam_result = groupSyncRead->addParam(motorID[i]);

        if (dxl_addparam_result != true)
        {
            fprintf(stderr, "[ID:%03d] groupSyncRead addparam failed", motorID[i]);
        }
    }
}

void readMovingStatus(dynamixel::GroupSyncRead *groupSyncRead, dynamixel::PacketHandler *packetHandler)
{
    dxl_comm_result = groupSyncRead->txRxPacket();

    if (dxl_comm_result != COMM_SUCCESS)
    {
        printf("%s\n", packetHandler->getTxRxResult(dxl_comm_result));
    }

    for (int i = 0; i < motorID.size(); i++)
    {
        if (groupSyncRead->getError(motorID[i], &dxl_error))
        {
            printf("[ID:%03d] %s\n", motorID[i], packetHandler->getRxPacketError(dxl_error));
        }
    }
}

void checkData(dynamixel::GroupSyncRead *groupSyncRead)
{
    for (int i = 0; i < motorID.size(); i++)
    {   
        dxl_getdata_result = groupSyncRead->isAvailable(motorID[i], ADDR_MX_MOVING_STATUS, LEN_MX_MOVING_STATUS);

        if (dxl_getdata_result != true)
        {
            fprintf(stderr, "[ID:%03d] groupSyncRead getdata failed", motorID[i]);
        }
    }
}

void getMovingStatus(dynamixel::GroupSyncRead *groupSyncRead)
{
    for (int i = 0; i < motorID.size(); i++)
    {
        dxl_moving_status = groupSyncRead->getData(motorID[i], ADDR_MX_MOVING_STATUS, LEN_MX_MOVING_STATUS);
        
        moving_status[i] = (uint8_t)dxl_moving_status & 0x01;
    }
}

void setPosition(dynamixel::GroupBulkWrite *groupBulkWrite, dynamixel::PacketHandler *packetHandler, double *desired_pulse)
{
    for (int i = 0; i < motorID.size(); i++)
    {
        dxl_goal_position = (int)desired_pulse[i];

        param_goal_position[0] = DXL_LOBYTE(DXL_LOWORD(acc));
        param_goal_position[1] = DXL_HIBYTE(DXL_LOWORD(acc));
        param_goal_position[2] = DXL_LOBYTE(DXL_HIWORD(acc));
        param_goal_position[3] = DXL_HIBYTE(DXL_HIWORD(acc));

        param_goal_position[4] = DXL_LOBYTE(DXL_LOWORD(vel));
        param_goal_position[5] = DXL_HIBYTE(DXL_LOWORD(vel));
        param_goal_position[6] = DXL_LOBYTE(DXL_HIWORD(vel));
        param_goal_position[7] = DXL_HIBYTE(DXL_HIWORD(vel));

        param_goal_position[8] = DXL_LOBYTE(DXL_LOWORD(dxl_goal_position));
        param_goal_position[9] = DXL_HIBYTE(DXL_LOWORD(dxl_goal_position));
        param_goal_position[10] = DXL_LOBYTE(DXL_HIWORD(dxl_goal_position));
        param_goal_position[11] = DXL_HIBYTE(DXL_HIWORD(dxl_goal_position));

        dxl_addparam_result = groupBulkWrite->addParam(motorID[i], ADDR_MX_GOAL_ACCELERATION, LEN_MX_GOAL_ACCELERATION + LEN_MX_GOAL_VELOCITY + LEN_MX_GOAL_POSITION, param_goal_position);

        if (dxl_addparam_result != true)
        {
            fprintf(stderr, "[ID:%03d] groupBulkWrite addparam failed", motorID[i]);
        }

        dxl_comm_result = groupBulkWrite->txPacket();

        if (dxl_comm_result != COMM_SUCCESS)
        {
            printf("%s\n", packetHandler->getTxRxResult(dxl_comm_result));
        }
    }
}

void setSpeed(dynamixel::GroupBulkWrite *groupBulkWrite, dynamixel::PacketHandler *packetHandler)
{
    for (int i = 0; i < motorID.size(); i++)
    {
        param_goal_speed[0] = DXL_LOBYTE(DXL_LOWORD(vel));
        param_goal_speed[1] = DXL_HIBYTE(DXL_LOWORD(vel));
        param_goal_speed[2] = DXL_LOBYTE(DXL_HIWORD(vel));
        param_goal_speed[3] = DXL_HIBYTE(DXL_HIWORD(vel));

        dxl_addparam_result = groupBulkWrite->addParam(motorID[i], ADDR_MX_GOAL_VELOCITY, LEN_MX_GOAL_VELOCITY, param_goal_speed);

        if (dxl_addparam_result != true)
        {
            fprintf(stderr, "[ID:%03d] groupBulkWrite addparam failed", motorID[i]);
        }

        dxl_comm_result = groupBulkWrite->txPacket();

        if (dxl_comm_result != COMM_SUCCESS)
        {
            printf("%s\n", packetHandler->getTxRxResult(dxl_comm_result));
        }
    }
}

#endif // NODE_H
