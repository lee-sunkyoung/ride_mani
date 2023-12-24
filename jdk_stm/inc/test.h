#ifndef TEST_H
#define TEST_H

#include "header.h"
#include "stm_read.h"
#include "stm_write.h"
#include <tutorial_msgs/mydmxel.h>


class Test
{
private:
  StmPacketGenerator stmPacketGenerator;
  StmPacketTranslator stmPacketTranslator;
  
  enum ConversationStatus
  {
    CONVERSATION_REQUEST,
    CONVERSATION_RESPONSE
  };
  ConversationStatus conversationStatus;

public:
  Test(Serial *s, bool *serialRead, mutex *m, queue<uint8_t> *readByte);
  ~Test();

  //float _x, _z;

  void myCallBack(const tutorial_msgs::mydmxelConstPtr& msg);
  void myCallBack_autorace(const tutorial_msgs::mydmxelConstPtr& msg);

  void Algorithm_Test();

  Serial *s;
  bool *serialRead;
  mutex *m;
  queue<uint8_t> *readByte;
};

#endif // MINIBOT_H
