#ifndef SEEEDMMWAVE_H
#define SEEEDMMWAVE_H

#include <Arduino.h>
#include <stdint.h>

#include <memory>
#include <queue>

#define _MMWAVE_DEBUG 0

#ifndef _UART_BAUD
#  define _UART_BAUD 115200
#else
#  warning "Notice: the uart baud of mmWave serial should be 115200"
#endif

#define MAX_QUEUE_SIZE    10
#define FRAME_BUFFER_SIZE 512
#define SOF_BYTE          0x01

// Frame structure sizes
#define SIZE_SOF        1
#define SIZE_ID         2
#define SIZE_LEN        2
#define SIZE_TYPE       2
#define SIZE_HEAD_CKSUM 1
#define SIZE_FRAME_HEADER                                                      \
  (SIZE_SOF + SIZE_ID + SIZE_LEN + SIZE_TYPE + SIZE_HEAD_CKSUM)
#define SIZE_DATA_CKSUM 1

class SeeedmmWave {
 private:
  HardwareSerial* _serial;
  uint32_t _baud;
  uint32_t _wait_delay;

 protected:
  uint16_t _id = 0;
  uint8_t calculateChecksum(const uint8_t* data, size_t len);
  bool validateChecksum(const uint8_t* data, size_t len,
                        uint8_t expected_checksum);
  bool processFrame(const uint8_t* frame_bytes, size_t len, uint16_t data_type);

  /**
   * @brief Handle different types of data frames.
   *
   * This is a pure virtual function that must be overridden by derived classes
   * to handle specific types of data frames. The function processes the data
   * based on the type and data length, and performs necessary actions.
   *
   * @param _type The type of the data frame.
   * @param data The pointer to the data buffer.
   * @param data_len The length of the data buffer.
   * @return true if the data is processed successfully, false otherwise.
   */
  virtual bool handleType(uint16_t _type, const uint8_t* data,
                          size_t data_len) = 0;

 public:
  SeeedmmWave() {}
  virtual ~SeeedmmWave() {
    if (_serial) {
      _serial->end();
      _serial = nullptr;
    }
  }
  void begin(HardwareSerial* serial, uint32_t baud = _UART_BAUD,
             uint32_t wait_delay = 1, int rst = -1);
  int available();
  int read(char* data, int length);
  int write(const char* data, int length);
  bool sendFrame(const uint16_t type, const uint8_t* data, size_t data_len);

  bool fetch(uint16_t data_type = 0xFFFF, uint32_t timeout = 1);

  float extractFloat(const uint8_t* bytes) const;
  uint32_t extractU32(const uint8_t* bytes) const;
  void floatToBytes(float value, uint8_t* bytes);
  void uint32ToBytes(uint32_t value, uint8_t* bytes);
};

#endif  // SEEEDMMWAVE_H
