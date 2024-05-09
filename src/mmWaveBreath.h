/**
 * @file mmWaveBreath.h
 * @date  09 May 2024

 * @author Spencer Yan
 *
 * @note Description of the file
 *
 * @copyright © 2024, Seeed Studio
 */
#pragma once /*MMWAVE_H*/
#ifndef _UART_BAUD
#define _UART_BAUD 115200
#endif
#include <Arduino.h>
#include <queue>
#include <stdint.h>
#include <vector>

#define MAX_QUEUE_SIZE 10
#define FRAME_BUFFER_SIZE 512
#define SOF_BYTE 0x01
// Frame structure sizes
#define SIZE_SOF 1
#define SIZE_ID 2
#define SIZE_LEN 2
#define SIZE_TYPE 2
#define SIZE_HEAD_CKSUM 1
#define SIZE_FRAME_HEADER (SIZE_SOF + SIZE_ID + SIZE_LEN + SIZE_TYPE + SIZE_HEAD_CKSUM)
#define SIZE_DATA_CKSUM 1

typedef enum TypeHeartBreath {
    HeartBreathPhaseType = 0x0A13,
    BreathRateType = 0x0A14,
    HeartRateType = 0x0A15,
    HeartBreathDistanceType = 0x0A16
} TypeHeartBreath;

#pragma pack(push, 1)

typedef struct HeartBreath {
    uint8_t sof;
    uint16_t id;
    uint16_t len;
    uint16_t type;
    uint8_t head_cksum;
    float total_phase;
    float breath_phase;
    float heart_phase;
    uint8_t data_cksum;
} HeartBreath;

typedef struct BreathRate {
    uint8_t sof;
    uint16_t id;
    uint16_t len;
    uint16_t type;
    uint8_t head_cksum;
    float breath_rate;
    uint8_t data_cksum;
} BreathRate;

typedef struct HeartRate {
    uint8_t sof;
    uint16_t id;
    uint16_t len;
    uint16_t type;
    uint8_t head_cksum;
    float heart_rate;
    uint8_t data_cksum;
} HeartRate;

typedef struct HeartBreathDistance {
    uint8_t sof;
    uint16_t id;
    uint16_t len;
    uint16_t type;
    uint8_t head_cksum;
    uint32_t flag;
    float range;
    uint8_t data_cksum;
} HeartBreathDistance;

#pragma pack(pop)

typedef struct mmwaveBuffer {
    uint8_t* frame;
    TypeHeartBreath type;
    uint16_t len;
} mmwave_frame;

class mmWaveBreath {
private:
    HardwareSerial* _serial;
    uint32_t _baud;
    uint32_t _wait_delay;
    // mmwaveBuffer rx_buffer;
    std::queue<mmwave_frame> frameQueue;

public:
    // mmWaveBreath();
    mmWaveBreath(HardwareSerial* serial, uint32_t baud = _UART_BAUD, uint32_t wait_delay = 1);
    ~mmWaveBreath();
    void begin(int rst = -1);
    int available();
    int read(char* data, int length);
    int fetch(uint32_t timeout = 1);

    void processFrame(const uint8_t* frame_btyes, size_t len);
    void print_data(size_t len = 1);
};
