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

bool validateChecksum(const uint8_t* data, size_t len, uint8_t expected_checksum) {
    unsigned char ret = 0;
    for (int i = 0; i < len; i++)
        ret = ret ^ data[i];
    ret = ~ret;
    return ret;
    uint8_t checksum = 0;
    for (size_t i = 0; i < len; i++) {
        checksum ^= data[i];
    }
    checksum = ~checksum;
    return checksum == expected_checksum;
}
/**
 * @brief 根据帧头的长度，计算帧的实际长度
 *
 * @param buffer
 * @return size_t
 */
size_t expectedFrameLength(const std::vector<uint8_t>& buffer) {
    // Extract the length from the buffer
    size_t len = (buffer[3] << 8) | buffer[4]; // Adjust index based on your protocol
    return SIZE_FRAME_HEADER + len + SIZE_DATA_CKSUM;
}

mmWaveBreath::mmWaveBreath(HardwareSerial* serial, uint32_t baud, uint32_t wait_delay)
    : _baud(baud), _wait_delay(wait_delay) {
    this->_serial = serial;
}

mmWaveBreath::~mmWaveBreath() {
    _serial->end();
    _serial = nullptr;
}

void mmWaveBreath::begin(int rst) {
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
    if (_serial) {
        return _serial->available();
    }
    return 0;
}

int mmWaveBreath::read(char* data, int length) {
    if (_serial) {
        return _serial->readBytes(data, length);
    }
    return 0;
}

int mmWaveBreath::fetch(uint32_t timeout) {
    uint32_t start_time = millis();
    bool startFrame = false;
    std::vector<uint8_t> frameBuffer;
    while (millis() - start_time < timeout) {
        while (_serial->available()) {
            uint8_t byte = _serial->read();
            if (byte == SOF_BYTE) {
                startFrame = true;
                frameBuffer.clear();
                frameBuffer.push_back(byte);
            } else if (startFrame) {
                frameBuffer.push_back(byte);
                if (frameBuffer.size() >= SIZE_FRAME_HEADER) { // 得到帧头，开始解析
                    // Assuming fixed length frames or you need to parse length and handle
                    // accordingly
                    if (frameBuffer.size() ==
                        expectedFrameLength(frameBuffer)) { // 如果长度不够，则继续读取
                        processFrame(frameBuffer.data(), frameBuffer.size()); // 长度足够，开始解析
                        startFrame = false;
                    }
                }
            }
        }
    }
    // Here you might want to return the number of frames processed, or handle differently
    return frameQueue.size();
}

void printItem(const mmwave_frame& frame) {

    // for (int i = 0; i < frame.len; i++) {
    //     Serial.printf("%02x ", frame.frame[i]);
    // }
    // Serial.println();
    switch (frame.type) {
    case TypeHeartBreath::HeartBreathPhaseType: {
        HeartBreath* hb = reinterpret_cast<HeartBreath*>(frame.frame);
        if (hb != nullptr) { // 确保指针非空
            // Serial.printf("breath_phase: %f, heart_phase: %f\n", *(float *)&hb->breath_phase,
            //               *(float *)&hb->heart_phase);
        }
        break;
    }
    case TypeHeartBreath::BreathRateType: {
        BreathRate* hb = reinterpret_cast<BreathRate*>(frame.frame);
        if (hb != nullptr) { // 确保指针非空
            Serial.printf("breath_rate: %f\n", *(float *)&hb->breath_rate);
        }
        break;
    }
    case TypeHeartBreath::HeartRateType: {
        HeartRate* hb = reinterpret_cast<HeartRate*>(frame.frame);
        if (hb != nullptr) { // 确保指针非空
            // Serial.printf("heart_rate: %f\n", *(float *)&hb->heart_rate);
        }
        break;
    }
    case TypeHeartBreath::HeartBreathDistanceType: {
        HeartBreathDistance* hb = reinterpret_cast<HeartBreathDistance*>(frame.frame);
        if (hb != nullptr) { // 确保指针非空
            // Serial.printf("range: %f\n", *(float *)&hb->range);
        }
        break;
    }
    default:
        break;
    }
}

void mmWaveBreath::processFrame(const uint8_t* frame_btyes, size_t len) {
    if (len < SIZE_FRAME_HEADER)
        return; // Not enough data to process header
    uint16_t id = (frame_btyes[1] << 8) | frame_btyes[2];
    uint16_t data_len = (frame_btyes[3] << 8) | frame_btyes[4];
    uint16_t type = (frame_btyes[5] << 8) | frame_btyes[6];
    uint8_t head_cksum = frame_btyes[7];
    // Validate header checksum
    if (!validateChecksum(frame_btyes, SIZE_FRAME_HEADER - SIZE_DATA_CKSUM, head_cksum))
        return;
    // Check if the remaining bytes are enough for data + data checksum
    if (len < SIZE_FRAME_HEADER + data_len + SIZE_DATA_CKSUM)
        return;
    // Validate data checksum
    uint8_t data_cksum = frame_btyes[SIZE_FRAME_HEADER + data_len];
    if (!validateChecksum(&frame_btyes[SIZE_FRAME_HEADER], data_len, data_cksum))
        return;
    // Allocate memory for the entire frame
    mmwave_frame newFrame;
    newFrame.frame = new uint8_t[len];
    memcpy(newFrame.frame, frame_btyes, len);
    newFrame.type = static_cast<TypeHeartBreath>(type);
    newFrame.len = len;
    // Store the frame in the vector
    frameQueue.push(newFrame);
}

/**
 * @brief 在这里进行数据的显示
 *
 * @return int
 */
void mmWaveBreath::print_data(size_t len) {
    if (frameQueue.size() < 0)
        return;

    int item_count = frameQueue.size();
    if (len > item_count)
        len = item_count;

    // Print the first frame
    for (int i = 0; i < len; i++) {
        mmwave_frame frame = frameQueue.front();
        frameQueue.pop();
        printItem(frame);
    }
}