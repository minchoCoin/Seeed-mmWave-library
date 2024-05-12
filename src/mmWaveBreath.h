/**
 * @file mmWaveBreath.h
 * @date  12 May 2024

 * @author Spencer Yan
 *
 * @note Description of the file
 *
 * @copyright © 2024, Seeed Studio
 */

#ifndef MMWAVEBREATH_H
#define MMWAVEBREATH_H

#include "mmWaveBreathTypes.hpp"
#include <memory>
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

class mmWaveBreath {
  private:
    HardwareSerial *_serial;
    uint32_t _baud;
    uint32_t _wait_delay;

    std::unique_ptr<HeartBreath> _heartBreath;
    std::unique_ptr<BreathRate> _breathRate;
    std::unique_ptr<HeartRate> _heartRate;
    std::unique_ptr<HeartBreathDistance> _heartBreathDistance;

    bool processFrame(const uint8_t *frame_btyes, size_t len);

  public:
    // mmWaveBreath();
    mmWaveBreath();
    ~mmWaveBreath();

    void begin(HardwareSerial *serial, uint32_t baud = _UART_BAUD, uint32_t wait_delay = 1, int rst = -1);
    int available();
    int read(char *data, int length);
    bool fetch(uint32_t timeout = 1);

    float extractFloat(const uint8_t *bytes);
    uint32_t extractU32(const uint8_t *bytes);

    bool getHeartBreathPhases(float &total_phase, float &breath_phase, float &heart_phase) const {
        if (_heartBreath && _heartBreath->isValid()) {
            _heartBreath->getPhase(total_phase, breath_phase, heart_phase);
            return true;
        }
        return false;
    }

    bool getBreathRate(float &rate) const {
        if (_breathRate && _breathRate->isValid()) {
            _breathRate->getBreathRate(rate);
            return true;
        }
        return false;
    }

    bool getHeartRate(float &rate) const {
        if (_heartRate && _heartRate->isValid()) {
            _heartRate->getHeartRate(rate);
            return true;
        }
        return false;
    }

    bool getHeartBreathDistance(float &distance) const {
        if (_heartBreathDistance && _heartBreathDistance->isValid()) {
            _heartBreathDistance->getDistance(distance);
            return true;
        }
        return false;
    }
};

#endif /*MMWAVEBREATH_H*/