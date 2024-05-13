/**
 * @file mmWaveBreath.cpp
 * @date  09 May 2024

 * @author Spencer Yan
 *
 * @note Description of the file
 *
 * @copyright © 2024, Seeed Studio
 */

#include "mmWaveBreath.h"
#include <utility>
#include <vector>

mmWaveBreath::mmWaveBreath() {}

mmWaveBreath::~mmWaveBreath() {
    _serial->end();
    _serial = nullptr;
}

void mmWaveBreath::begin(HardwareSerial *serial, uint32_t baud, uint32_t wait_delay, int rst) {
    this->_serial     = serial;
    this->_baud       = baud;
    this->_wait_delay = wait_delay;

    _serial->begin(_baud);
    _serial->setTimeout(1000);
    _serial->flush();
    if (rst >= 0) {
        pinMode(rst, OUTPUT);
        digitalWrite(rst, LOW);
        delay(50);
        digitalWrite(rst, HIGH);
        delay(500);
    }
}

int mmWaveBreath::available() {
    return _serial ? _serial->available() : 0;
}

int mmWaveBreath::read(char *data, int length) {
    return _serial ? _serial->readBytes(data, length) : 0;
}

float mmWaveBreath::extractFloat(const uint8_t *bytes) {
    return *reinterpret_cast<const float *>(bytes);
}
uint32_t mmWaveBreath::extractU32(const uint8_t *bytes) {
    return *reinterpret_cast<const uint32_t *>(bytes);
}

bool validateChecksum(const uint8_t *data, size_t len, uint8_t expected_checksum) {
    uint8_t checksum = 0;
    for (size_t i = 0; i < len; i++) {
        checksum ^= data[i];
    }
    checksum = ~checksum;
    return checksum == expected_checksum;
}

size_t expectedFrameLength(const std::vector<uint8_t> &buffer) {
    size_t len = (buffer[3] << 8) | buffer[4];
    return SIZE_FRAME_HEADER + len + SIZE_DATA_CKSUM;
}

bool mmWaveBreath::fetch(uint32_t timeout) {
    uint32_t expire_time = millis() + timeout;
    bool startFrame      = false;
    std::vector<uint8_t> frameBuffer;
    bool result = false;
    while (millis() < expire_time) {
        if (_serial->available()) {
            uint8_t byte = _serial->read();
            if (byte == SOF_BYTE) {
                startFrame = true;
                frameBuffer.clear();
            }
            if (startFrame) {
                frameBuffer.push_back(byte);
                if (frameBuffer.size() >= SIZE_FRAME_HEADER && frameBuffer.size() == expectedFrameLength(frameBuffer)) {
                    result     = processFrame(frameBuffer.data(), frameBuffer.size());
                    startFrame = false;
                    // print buffer bytes
                    // for (auto b : frameBuffer) {
                    //     Serial.print(b, HEX);
                    //     Serial.print(" ");
                    // }
                    // Serial.println();
                }
            }
        }
    }
    return result;
}

/**
 * @brief Process a single frame
 *
 * @param frame_bytes
 * @param len
 * @return true
 * @return false
 */
bool mmWaveBreath::processFrame(const uint8_t *frame_bytes, size_t len) {
    if (len < SIZE_FRAME_HEADER)
        return false;  // Not enough data to process header

    uint16_t id        = (frame_bytes[1] << 8) | frame_bytes[2];
    uint16_t data_len  = (frame_bytes[3] << 8) | frame_bytes[4];
    uint16_t type      = (frame_bytes[5] << 8) | frame_bytes[6];
    uint8_t head_cksum = frame_bytes[7];
    uint8_t data_cksum = frame_bytes[SIZE_FRAME_HEADER + data_len];

    // Checksum validation
    if (!validateChecksum(frame_bytes, SIZE_FRAME_HEADER - SIZE_DATA_CKSUM, head_cksum) ||
        !validateChecksum(&frame_bytes[SIZE_FRAME_HEADER], data_len, data_cksum)) {
        return false;
    }

    switch (static_cast<TypeHeartBreath>(type)) {
        case TypeHeartBreath::TypeHeartBreathPhase:
            _heartBreath = std::make_unique<HeartBreath>(
                extractFloat(&frame_bytes[SIZE_FRAME_HEADER]),
                extractFloat(&frame_bytes[SIZE_FRAME_HEADER + sizeof(float)]),
                extractFloat(&frame_bytes[SIZE_FRAME_HEADER + 2 * sizeof(float)]));
            break;
        case TypeHeartBreath::TypeBreathRate:
            _breathRate = std::make_unique<BreathRate>(extractFloat(&frame_bytes[SIZE_FRAME_HEADER]));
            break;
        case TypeHeartBreath::TypeHeartRate:
            _heartRate = std::make_unique<HeartRate>(extractFloat(&frame_bytes[SIZE_FRAME_HEADER]));
            break;
        case TypeHeartBreath::TypeHeartBreathDistance:
            _heartBreathDistance = std::make_unique<HeartBreathDistance>(
                extractU32(&frame_bytes[SIZE_FRAME_HEADER]),
                extractFloat(&frame_bytes[SIZE_FRAME_HEADER + sizeof(uint32_t)]));
            break;
        default:
            return false;  // Unhandled type
    }

    return true;
}