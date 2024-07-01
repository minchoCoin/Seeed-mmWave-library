/**
 * @file mmWave.cpp
 * @date  09 May 2024

 * @author Spencer Yan
 *
 * @note Description of the file
 *
 * @copyright © 2024, Seeed Studio
 */

#include "Seeed_XIAO_mmWave.h"
#include <utility>
#include <vector>

SeeedmmWave::SeeedmmWave() {}

SeeedmmWave::~SeeedmmWave() {
    _serial->end();
    _serial = nullptr;
}

void SeeedmmWave::begin(HardwareSerial *serial, uint32_t baud, uint32_t wait_delay, int rst) {
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

int SeeedmmWave::available() {
    return _serial ? _serial->available() : 0;
}

int SeeedmmWave::read(char *data, int length) {
    return _serial ? _serial->readBytes(data, length) : 0;
}

int SeeedmmWave::write(const char *data, int length) {
    return _serial ? _serial->write(data, length) : 0;
}

float SeeedmmWave::extractFloat(const uint8_t *bytes) const {
    return *reinterpret_cast<const float *>(bytes);
}
uint32_t SeeedmmWave::extractU32(const uint8_t *bytes) const {
    return *reinterpret_cast<const uint32_t *>(bytes);
}

uint8_t SeeedmmWave::calculateChecksum(const uint8_t *data, size_t len) {
    uint8_t checksum = 0;
    for (size_t i = 0; i < len; i++) {
        checksum ^= data[i];
    }
    checksum = ~checksum;
    return checksum;
}

bool SeeedmmWave::validateChecksum(const uint8_t *data, size_t len, uint8_t expected_checksum) {
    return calculateChecksum(data, len) == expected_checksum;
}

void SeeedmmWave::floatToBytes(float value, uint8_t *bytes) {
    uint8_t *p = reinterpret_cast<uint8_t *>(&value);
    for (size_t i = 0; i < sizeof(float); ++i) {
        bytes[i] = p[i];
    }
}

void SeeedmmWave::uint32ToBytes(uint32_t value, uint8_t *bytes) {
    uint8_t *p = reinterpret_cast<uint8_t *>(&value);
    for (size_t i = 0; i < sizeof(uint32_t); ++i) {
        bytes[i] = p[i];
    }
}

size_t expectedFrameLength(const std::vector<uint8_t> &buffer) {
    if (buffer.size() < SIZE_FRAME_HEADER) {
        return SIZE_FRAME_HEADER;  // minimum frame header size
    }
    size_t len = (buffer[3] << 8) | buffer[4];
    return SIZE_FRAME_HEADER + len + SIZE_DATA_CKSUM;
}

void printHexBuff(const std::vector<uint8_t> &buffer) {
    for (auto b : buffer) {
        Serial.print(b, HEX);
        Serial.print(" ");
    }
    Serial.println();
}

bool SeeedmmWave::fetch(uint16_t data_type, uint32_t timeout) {
    uint32_t expire_time = millis() + timeout;
    // bool result          = false;
    bool startFrame      = false;
    std::vector<uint8_t> frameBuffer;
    while (millis() < expire_time) {
        if (_serial->available()) {
            uint8_t byte = _serial->read();
            if (!startFrame && byte == SOF_BYTE) {
                startFrame = true;
                frameBuffer.clear();
                frameBuffer.push_back(byte);
            } else if (startFrame) {
                frameBuffer.push_back(byte);
                if (frameBuffer.size() >= SIZE_FRAME_HEADER && frameBuffer.size() == expectedFrameLength(frameBuffer)) {
                    if (processFrame(frameBuffer.data(), frameBuffer.size(), data_type)) {
                        // Serial.print("Recieved>>>");
                        // printHexBuff(frameBuffer);
                        return true;
                    }
                    startFrame = false;
                    frameBuffer.clear();
                }
            }
        }
    }
    // return result;
    return false;
}

bool SeeedmmWave::processFrame(const uint8_t *frame_bytes, size_t len, uint16_t data_type = 0xFFFF) {
    if (len < SIZE_FRAME_HEADER)
        return false;  // Not enough data to process header

    uint16_t id        = (frame_bytes[1] << 8) | frame_bytes[2];
    uint16_t data_len  = (frame_bytes[3] << 8) | frame_bytes[4];
    uint16_t type      = (frame_bytes[5] << 8) | frame_bytes[6];
    uint8_t head_cksum = frame_bytes[7];
    uint8_t data_cksum = frame_bytes[SIZE_FRAME_HEADER + data_len];

    // Only proceed if the type matches or if data_type is set to the default, indicating no specific type is required
    if (data_type != 0xFFFF && data_type != type)
        return false;

    // Checksum validation
    if (!validateChecksum(frame_bytes, SIZE_FRAME_HEADER - SIZE_DATA_CKSUM, head_cksum) ||
        !validateChecksum(&frame_bytes[SIZE_FRAME_HEADER], data_len, data_cksum)) {
        return false;
    }

    return handleType(type, &frame_bytes[SIZE_FRAME_HEADER], data_len);
}

bool SeeedmmWave::SendFrame(uint16_t type, const uint8_t *data = nullptr, size_t len = 0) {
    std::vector<uint8_t> frame;  // SOF, ID, LEN, TYPE, HEAD_CKSUM,
                                 // DATA, DATA_CKSUM
    frame.push_back(SOF_BYTE);   // Start of Frame
    frame.push_back(0x00);       // ID
    frame.push_back(0x00);       // ID
    frame.push_back(len >> 8);
    frame.push_back(len & 0xFF);
    frame.push_back(type >> 8);
    frame.push_back(type & 0xFF);
    uint8_t head_cksum = calculateChecksum(frame.data(), frame.size());
    frame.push_back(head_cksum);
    if (data != nullptr) {
        for (size_t i = 0; i < len; i++) {
            frame.push_back(data[i]);
        }
        uint8_t data_cksum = calculateChecksum(data, len);
        frame.push_back(data_cksum);
    }
    _serial->write(frame.data(), frame.size());
    Serial.print("Send<<<");
    printHexBuff(frame);
    return true;
}