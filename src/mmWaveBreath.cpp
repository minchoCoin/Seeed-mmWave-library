/**
 * @file mmWaveBreath.cpp
 * @date  09 May 2024

 * @author Spencer Yan
 *
 * @note Description of the file
 *
 * @copyright © 2024, Seeed Studio
 */

#include "mmWaveBreath.hpp"
#include <utility>
#include <vector>

bool validateChecksum(const uint8_t *data, size_t len, uint8_t expected_checksum) {
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
size_t expectedFrameLength(const std::vector<uint8_t> &buffer) {
    // Extract the length from the buffer
    size_t len = (buffer[3] << 8) | buffer[4];  // Adjust index based on your protocol
    return SIZE_FRAME_HEADER + len + SIZE_DATA_CKSUM;
}

mmWaveBreath::mmWaveBreath(HardwareSerial *serial, uint32_t baud, uint32_t wait_delay)
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

int mmWaveBreath::read(char *data, int length) {
    if (_serial) {
        return _serial->readBytes(data, length);
    }
    return 0;
}

float mmWaveBreath::extractFloat(const uint8_t *bytes) {
    // Assumes system is little endian; you might need to adjust this depending on your architecture
    return *reinterpret_cast<const float *>(bytes);
}
uint32_t mmWaveBreath::extractU32(const uint8_t *bytes) {
    // Assumes system is little endian; you might need to adjust this depending on your architecture
    return *reinterpret_cast<const uint32_t *>(bytes);
}

int mmWaveBreath::fetch(uint32_t mask, uint32_t timeout) {
    uint32_t expire_time = millis() + timeout;
    bool startFrame      = false;
    std::vector<uint8_t> frameBuffer;

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
                    processFrame(frameBuffer.data(), frameBuffer.size(), mask);
                    startFrame = false;
                }
            }
        }
    }
    return dataItems.size();
}

void mmWaveBreath::processFrame(const uint8_t *frame_bytes, size_t len, uint32_t mask) {
    if (len < SIZE_FRAME_HEADER)
        return;  // Not enough data to process header

    uint16_t type = (frame_bytes[5] << 8) | frame_bytes[6];

    if (!(TypeHeartBreathToMask(static_cast<TypeHeartBreath>(type)) & mask)) {
        return;
    }

    uint16_t id        = (frame_bytes[1] << 8) | frame_bytes[2];
    uint16_t data_len  = (frame_bytes[3] << 8) | frame_bytes[4];
    uint8_t head_cksum = frame_bytes[7];
    uint8_t data_cksum = frame_bytes[SIZE_FRAME_HEADER + data_len];

    // 校验和验证
    if (!validateChecksum(frame_bytes, SIZE_FRAME_HEADER - SIZE_DATA_CKSUM, head_cksum) ||
        !validateChecksum(&frame_bytes[SIZE_FRAME_HEADER], data_len, data_cksum) ||
        data_len < 0 || data_len > 10) {
        return;
    }

    BaseData *newData = nullptr;
    // 根据类型创建对应的数据对象
    switch (static_cast<TypeHeartBreath>(type)) {
        case HeartBreathPhaseType:
            newData = new HeartBreath(extractFloat(frame_bytes + SIZE_FRAME_HEADER),
                                      extractFloat(frame_bytes + SIZE_FRAME_HEADER + sizeof(float)),
                                      extractFloat(frame_bytes + SIZE_FRAME_HEADER + 2 * sizeof(float)));
            break;
        case BreathRateType:
            newData = new BreathRate(extractFloat(frame_bytes + SIZE_FRAME_HEADER));
            break;
        case HeartRateType:
            newData = new HeartRate(extractFloat(frame_bytes + SIZE_FRAME_HEADER));
            break;
        case HeartBreathDistanceType:
            newData = new HeartBreathDistance(extractU32(frame_bytes + SIZE_FRAME_HEADER),
                                              extractFloat(frame_bytes + SIZE_FRAME_HEADER + sizeof(uint32_t)));
            break;
        default:
            return;
    }

    if (newData) {
        if (dataItems.size() >= MAX_QUEUE_SIZE) {
            while (!dataItems.empty()) {
                delete dataItems.front();
                dataItems.pop();
            }
        }
        dataItems.push(newData);
    }
}

// void mmWaveBreath::processFrame( const uint8_t *frame_bytes, size_t len ) {
//     if ( len < SIZE_FRAME_HEADER )
//         return;  // Not enough data to process header

//     uint16_t id        = ( frame_bytes[1] << 8 ) | frame_bytes[2];
//     uint16_t data_len  = ( frame_bytes[3] << 8 ) | frame_bytes[4];
//     uint16_t type      = ( frame_bytes[5] << 8 ) | frame_bytes[6];
//     uint8_t head_cksum = frame_bytes[7];

//     // Validate header checksum
//     if ( !validateChecksum( frame_bytes, SIZE_FRAME_HEADER - SIZE_DATA_CKSUM, head_cksum ) )
//         return;

//     // Check if the remaining bytes are enough for data + data checksum
//     if ( len < SIZE_FRAME_HEADER + data_len + SIZE_DATA_CKSUM )
//         return;

//     // Validate data checksum
//     uint8_t data_cksum = frame_bytes[SIZE_FRAME_HEADER + data_len];
//     if ( !validateChecksum( &frame_bytes[SIZE_FRAME_HEADER], data_len, data_cksum ) )
//         return;

//     if ( data_len < 0 || data_len > 20 ) {
//         return;
//     }

//     // Depending on the frame type, create the corresponding object
//     BaseData *newData = nullptr;
//     switch ( static_cast<TypeHeartBreath>( type ) ) {
//         case HeartBreathPhaseType:
//             newData = new HeartBreath(
//                 extractFloat( frame_bytes + SIZE_FRAME_HEADER ),
//                 extractFloat( frame_bytes + SIZE_FRAME_HEADER + sizeof( float ) ),
//                 extractFloat( frame_bytes + SIZE_FRAME_HEADER + 2 * sizeof( float ) ) );
//             break;
//         case BreathRateType:
//             newData = new BreathRate(
//                 extractFloat( frame_bytes + SIZE_FRAME_HEADER ) );
//             break;
//         case HeartRateType:
//             newData = new HeartRate(
//                 extractFloat( frame_bytes + SIZE_FRAME_HEADER ) );
//             break;
//         case HeartBreathDistanceType:
//             newData = new HeartBreathDistance(
//                 extractU32( frame_bytes + SIZE_FRAME_HEADER ),
//                 extractFloat( frame_bytes + SIZE_FRAME_HEADER + sizeof( uint32_t ) ) );
//             break;
//         default:
//             return;  // Unknown type or do nothing
//     }

//     if ( newData ) {
//         if ( dataItems.size() >= MAX_QUEUE_SIZE ) {
//             while ( !dataItems.empty() ) {
//                 delete dataItems.front();  // 删除指向的对象
//                 dataItems.pop();           // 从队列中移除指针
//             }
//         }
//         dataItems.push( newData );
//     }
// }

// void printItem(const mmwave_frame &frame)
// {

//     // for (int i = 0; i < frame.len; i++) {
//     //     Serial.printf("%02x ", frame.frame[i]);
//     // }
//     // Serial.println();
//     switch (frame.type)
//     {
//     case TypeHeartBreath::HeartBreathPhaseType:
//     {
//         HeartBreath *hb = reinterpret_cast<HeartBreath *>(frame.frame);
//         if (hb != nullptr)
//         { // 确保指针非空
//             // Serial.printf("breath_phase: %f, heart_phase: %f\n", *(float *)&hb->breath_phase,
//             //               *(float *)&hb->heart_phase);
//         }
//         break;
//     }
//     case TypeHeartBreath::BreathRateType:
//     {
//         BreathRate *hb = reinterpret_cast<BreathRate *>(frame.frame);
//         if (hb != nullptr)
//         { // 确保指针非空
//             Serial.printf("breath_rate: %f\n", *(float *)&hb->breath_rate);
//         }
//         break;
//     }
//     case TypeHeartBreath::HeartRateType:
//     {
//         HeartRate *hb = reinterpret_cast<HeartRate *>(frame.frame);
//         if (hb != nullptr)
//         { // 确保指针非空
//             // Serial.printf("heart_rate: %f\n", *(float *)&hb->heart_rate);
//         }
//         break;
//     }
//     case TypeHeartBreath::HeartBreathDistanceType:
//     {
//         HeartBreathDistance *hb = reinterpret_cast<HeartBreathDistance *>(frame.frame);
//         if (hb != nullptr)
//         { // 确保指针非空
//             // Serial.printf("range: %f\n", *(float *)&hb->range);
//         }
//         break;
//     }
//     default:
//         break;
//     }
// }

void mmWaveBreath::print_data(size_t len) {
    if (dataItems.size() < 0)
        return;

    int item_count = dataItems.size();
    if (len > item_count)
        len = item_count;

    // Print the first frame
    for (int i = 0; i < len; i++) {
        BaseData *item = dataItems.front();
        dataItems.pop();  // Remove the item from the queue
        item->print();
        delete item;
    }
}