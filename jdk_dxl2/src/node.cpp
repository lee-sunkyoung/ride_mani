#include "../include/node.h"

void initCallback(const std_msgs::Bool::ConstPtr &msg)
{
    if (msg->data == true)
    {
        goal_Position[0] = 2.5;
        goal_Position[1] = 0;

        controlFSM = INIT_WRITE;
    }
}

void pantiltCallback(const geometry_msgs::Point::ConstPtr &msg)
{
    lock_guard<mutex> lg(global_mutex);

    pan_input = msg->x;
    tilt_input = msg->y;

    PID_Control(&pan, pan_target, pan_input);
    PID_Control(&tilt, tilt_target, tilt_input);

    goal_Position[0] += pan.output;
    goal_Position[1] += tilt.output;

    cout << "pan: " << goal_Position[0] << endl;
    cout << "tilt: " << goal_Position[1] << endl
         << endl;
}

void motor()
{
    Rate loopRate(Hz);

    dynamixel::PortHandler *portHandler = dynamixel::PortHandler::getPortHandler(DEVICENAME);
    dynamixel::PacketHandler *packetHandler = dynamixel::PacketHandler::getPacketHandler(PROTOCOL_VERSION);
    dynamixel::GroupBulkWrite groupBulkWrite(portHandler, packetHandler);
    dynamixel::GroupSyncRead groupSyncRead(portHandler, packetHandler, ADDR_MX_MOVING_STATUS, LEN_MX_MOVING_STATUS);

    serialPortCheck(portHandler);
    motorEnable(NUMBER_OF_MOTORS);
    motorTorqueOn(portHandler, packetHandler);
    storageMovingStatus(&groupSyncRead);

    PID_Init(&pan, 25, 0.001, 0, 150, 80);
    PID_Init(&tilt, 25, 0.001, 0, 150, 80);

    while (ok())
    {
        serialPortCheck(portHandler);

        if (serial_result == true)
        {
            if (OPERATING_MODE == 1)
            {
                switch (controlFSM)
                {
                case INIT_WRITE:
                {
                    acc = 10;
                    vel = 60;

                    for (int i = 0; i < NUMBER_OF_MOTORS; i++)
                    {
                        pulse_desired[i] = (init_Position[i] + 180.0) * MX_DEG2PULSE;
                    }

                    setPosition(&groupBulkWrite, packetHandler, pulse_desired);
                    groupBulkWrite.clearParam();

                    controlFSM = MOVING_STATUS_READ;

                    break;
                }
                case MOVING_STATUS_READ:
                {
                    in_position_cnt = 0;

                    readMovingStatus(&groupSyncRead, packetHandler);
                    checkData(&groupSyncRead);
                    getMovingStatus(&groupSyncRead);

                    for (int i = 0; i < NUMBER_OF_MOTORS; i++)
                    {
                        if (moving_status[i] == 0x01)
                        {
                            in_position_cnt++;
                        }
                    }

                    if (in_position_cnt == NUMBER_OF_MOTORS)
                    {
                        acc = 0;
                        vel = 0;

                        controlFSM = POSITION_WRITE;
                    }
                    else
                    {
                        controlFSM = MOVING_STATUS_READ;
                    }

                    break;
                }
                case POSITION_WRITE:
                {
                    global_mutex.lock();
                    for (int i = 0; i < NUMBER_OF_MOTORS; i++)
                    {
                        pulse_desired[i] = (goal_Position[i] + 180.0) * MX_DEG2PULSE;
                    }
                    global_mutex.unlock();

                    setPosition(&groupBulkWrite, packetHandler, pulse_desired);
                    groupBulkWrite.clearParam();

                    controlFSM = POSITION_WRITE;

                    break;
                }
                default:
                    break;
                }
            }

            if (OPERATING_MODE == 2)
            {
                vel = 0;

                setSpeed(&groupBulkWrite, packetHandler);
                groupBulkWrite.clearParam();
            }
        }

        spinOnce();
        loopRate.sleep();
    }

    motorTorqueOff(portHandler, packetHandler);
}

int main(int argc, char **argv)
{
    init(argc, argv, "jdk_dxl");

    NodeHandle n;

    init_sub = n.subscribe("init_pub", 1, initCallback);
    pantilt_sub = n.subscribe("target", 1, pantiltCallback);

    controlFSM = INIT_WRITE;

    thread t2(motor);
    t2.join();

    return 0;
}
