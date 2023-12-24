#ifndef STM_READ_H
#define STM_READ_H

#include "header.h"

#define BUFFER_SIZE 256

#define H1 0
#define H2 1
#define H3 2
#define RSRV 3
#define CRC2 8

class StmPacketTranslator
{
private:
  unsigned char state;
  unsigned char buffer[BUFFER_SIZE] = {0, };
  unsigned short packetCRC = 0;
  unsigned short bufferCRC = 0;

public:
  StmPacketTranslator();
  ~StmPacketTranslator();

  uint16_t combineByte(uint8_t lowByte, uint8_t highByte);
  unsigned short update_crc(unsigned short crc_accum, unsigned char *data_blk_ptr, unsigned short data_blk_size);
  void parsing(uint8_t *buffer);
  void readPacket(uint8_t front);
};

#endif // STM_READ_H
