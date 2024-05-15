/**
 * @file mmWave.cpp
 * @date  09 May 2024

 * @author Spencer Yan
 *
 * @note Description of the file
 *
 * @copyright © 2024, Seeed Studio
 */

#include "mmWave.h"
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
    bool result          = false;
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
                    result     = processFrame(frameBuffer.data(), frameBuffer.size(), data_type);
                    startFrame = false;
                    printHexBuff(frameBuffer);
                    return true;
                }
            }
        }
    }
    return result;
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

int SeeedmmWave::_sendFrame(const uint8_t *frame_bytes, size_t len) {
    return _serial ? _serial->write(frame_bytes, len) : 0;
}

bool SeeedmmWave::SendFrame(const uint16_t type, const uint8_t *data, size_t data_len) {
    uint8_t frame[SIZE_FRAME_HEADER + data_len + SIZE_DATA_CKSUM];  // Ensure buffer is large enough
    int packed_len = packFrame(type, data, data_len, frame, sizeof(frame));
    if (packed_len == SIZE_FRAME_HEADER + data_len + SIZE_DATA_CKSUM) {
        return _sendFrame(frame, packed_len) == packed_len;
    }
    return false;
}

int SeeedmmWave::packFrame(const uint16_t type, const uint8_t *data, size_t data_len, uint8_t *frame, size_t frame_size) {
    const size_t header_len = SIZE_FRAME_HEADER;  // SOF, ID, LEN, TYPE, HEAD_CKSUM
    const size_t total_len  = header_len + data_len + SIZE_DATA_CKSUM;
    uint16_t ID             = 0x00;

    if (frame_size < total_len) {
        return -1;  // Buffer too small
    }

    size_t index       = 0;
    frame[index++]     = SOF_BYTE;  // SOF
    frame[index++]     = (ID >> 8) & 0xFF;
    frame[index++]     = ID & 0xFF;
    frame[index++]     = (data_len >> 8) & 0xFF;
    frame[index++]     = data_len & 0xFF;
    frame[index++]     = (type >> 8) & 0xFF;
    frame[index++]     = type & 0xFF;
    uint8_t head_cksum = calculateChecksum(frame, index);
    frame[index++]     = head_cksum;

    memcpy(frame + index, data, data_len);
    index += data_len;
    uint8_t data_cksum = calculateChecksum(frame + header_len, data_len);
    frame[index++]     = data_cksum;

    return total_len;
}