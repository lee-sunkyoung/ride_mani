#include "../inc/test.h"

Test::Test(Serial *s, bool *serialRead, mutex *m, queue<uint8_t> *readByte)
{
  this->s = s;
  this->serialRead = serialRead;
  this->m = m;
  this->readByte = readByte;

  conversationStatus = CONVERSATION_REQUEST;
}

Test::~Test()
{

}

void Test::myCallBack(const tutorial_msgs::mydmxelConstPtr& msg)
{
  //-1~1 -> 0~200
  // stmPacketGenerator._x = (int)(10 * (msg->linear.x + 1));
  // stmPacketGenerator._z = (int)(10 * (msg->angular.z + 1));
  stmPacketGenerator.ID_1 = msg->motor1;
  stmPacketGenerator.ID_2 = msg->motor2;
  stmPacketGenerator.ID_3 = msg->motor3;
  stmPacketGenerator.ID_5 = msg->motor4;
  stmPacketGenerator.ID_6 = msg->motor5;
  
  stmPacketGenerator.left_wheel = msg->L_wheel;
  stmPacketGenerator.right_wheel = msg->R_wheel;
}

void Test::myCallBack_autorace(const tutorial_msgs::mydmxelConstPtr& msg)
{
  stmPacketGenerator.left_wheel = msg->L_wheel;
  stmPacketGenerator.right_wheel = msg->R_wheel;
}


void Test::Algorithm_Test()
{
  switch (conversationStatus)
  {
  case CONVERSATION_REQUEST:
  {
    try
    {
      vector<uint8_t> packet;

      stmPacketGenerator.writePacket(packet);
      
      s->write(packet);
    }
    catch (serial::IOException e)
    {
      cerr << "Port open failed." << e.what() << endl;
      s->close();
    }

    conversationStatus = CONVERSATION_RESPONSE;

    break;
  }
  case CONVERSATION_RESPONSE:
  {
    if(!readByte->empty())
    {
      while(!readByte->empty())
      {
        int readBytefront = (int)readByte->front();

        stmPacketTranslator.readPacket(readBytefront);
        
        m->lock();
        readByte->pop();
        m->unlock();
      }
    }

    conversationStatus = CONVERSATION_REQUEST;

    break;
  }
  }
}
