/**
 * @file mmWaveBreath.hpp
 * @date  10 May 2024

 * @author Spencer Yan
 *
 * @note Description of the file
 *
 * @copyright © 2024, Seeed Studio
 */

#ifndef MMWAVEBREATH_H
#define MMWAVEBREATH_H

#include "mmWaveBreathTypes.hpp"
#include <queue>

#ifndef _UART_BAUD
#define _UART_BAUD 115200
#endif

#define MAX_QUEUE_SIZE    10
#define FRAME_BUFFER_SIZE 512
#define SOF_BYTE          0x01

// Frame structure sizes
#define SIZE_SOF          1
#define SIZE_ID           2
#define SIZE_LEN          2
#define SIZE_TYPE         2
#define SIZE_HEAD_CKSUM   1
#define SIZE_FRAME_HEADER (SIZE_SOF + SIZE_ID + SIZE_LEN + SIZE_TYPE + SIZE_HEAD_CKSUM)
#define SIZE_DATA_CKSUM   1

#pragma pack(pop)

class mmWaveBreath {
  private:
    HardwareSerial *_serial;
    uint32_t _baud;
    uint32_t _wait_delay;
    // mmwaveBuffer rx_buffer;
    std::queue<BaseData *> dataItems;

    void processFrame(const uint8_t *frame_btyes, size_t len, uint32_t mask);
  public:
    // mmWaveBreath();
    mmWaveBreath(HardwareSerial *serial, uint32_t baud = _UART_BAUD, uint32_t wait_delay = 1);
    ~mmWaveBreath();
    void begin(int rst = -1);
    int available();
    int read(char *data, int length);
    int fetch(uint32_t mask, uint32_t timeout = 1);
    // int fetch(uint32_t timeout = 1, uint32_t mask);

    float extractFloat(const uint8_t *bytes);
    uint32_t extractU32(const uint8_t *bytes);

    void print_data(size_t len = 1);
};

#endif /*MMWAVEBREATH_H*/