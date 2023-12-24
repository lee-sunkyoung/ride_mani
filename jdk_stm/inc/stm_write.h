#ifndef STM_WRITE_H
#define STM_WRITE_H

#include "header.h"
#include <tutorial_msgs/mydmxel.h>

class StmPacketGenerator
{
private:

public:
  StmPacketGenerator();
  ~StmPacketGenerator();
  // 11/22
  int _x;
  int _z;
  int ID_1=0;
  int ID_2=0;
  int ID_3=0;
  int ID_5=0;
  int ID_6=0;
  int left_wheel=10;
  int right_wheel=10;
  // 11/22
  

  void divideByte(vector<uint8_t> &packet, int value, int length);
  void divideFloat(vector<uint8_t> &packet, float value);
  void update_header(vector<uint8_t> &packet);
  void update_data(vector<uint8_t> &packet);
  void update_crc(vector<uint8_t> &data_blk_ptr);
  void writePacket(vector<uint8_t> &packet);
};

#endif // STM_WRITE_H
