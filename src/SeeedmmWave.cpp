#include "SeeedmmWave.h"

/**
 * @brief Calculate the expected frame length from the buffer.
 *
 * @param buffer The input buffer containing frame data.
 * @return The expected frame length.
 *
 * @note This function calculates the expected frame length based on the
 * buffer's content. It checks the buffer size to ensure it contains at least
 * the frame header, then calculates the total frame length by adding the data
 * length and checksum size to the frame header size.
 */
size_t expectedFrameLength(const std::vector<uint8_t>& buffer) {
  if (buffer.size() < SIZE_FRAME_HEADER) {
    return SIZE_FRAME_HEADER;  // minimum frame header size
  }
  size_t len = (buffer[3] << 8) | buffer[4];
  return SIZE_FRAME_HEADER + len + SIZE_DATA_CKSUM;
}

/**
 * @brief Print the buffer content in hexadecimal format.
 *
 * @param buffer The input buffer to be printed.
 *
 * @note This function prints the content of the input buffer in hexadecimal
 * format. If the buffer contains more than 5 bytes, it highlights the 6th and
 * 7th bytes by enclosing them in brackets.
 */
void printHexBuff(const std::vector<uint8_t>& buffer) {
  for (size_t i = 0; i < buffer.size(); ++i) {
    if (i == 5 && i + 1 < buffer.size()) {
      Serial.print("[");
      Serial.print(buffer[i] < 16 ? "0" : "");
      Serial.print(buffer[i], HEX);
      Serial.print(" ");
      Serial.print(buffer[i + 1] < 16 ? "0" : "");
      Serial.print(buffer[i + 1], HEX);
      Serial.print("] ");
      ++i;  // Skip the next byte as it's already printed
    } else {
      Serial.print(buffer[i] < 16 ? "0" : "");
      Serial.print(buffer[i], HEX);
      Serial.print(" ");
    }
  }
  Serial.println();
}

/**
 * @brief Initialize the SeeedmmWave object.
 *
 *
 * @param serial Pointer to the hardware serial object.
 * @param baud The baud rate for the serial communication.
 * @param wait_delay The delay time to wait for the sensor.
 * @param rst The reset pin number. If negative, no reset is performed.
 *
 * @note This function initializes the SeeedmmWave object by setting the serial
 * port, baud rate, wait delay, and optionally resetting the hardware.
 */
void SeeedmmWave::begin(HardwareSerial* serial, uint32_t baud,
                        uint32_t wait_delay, int rst) {
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

/**
 * @brief Check the availability of data on the serial port.
 *
 * This function returns the number of bytes available for reading from the
 * serial port.
 *
 * @return The number of bytes available.
 */
int SeeedmmWave::available() {
  return _serial ? _serial->available() : 0;
}

/**
 * @brief Read data from the serial port.
 *
 * @param data The buffer to store the read data.
 * @param length The number of bytes to read.
 * @return The number of bytes actually read.
 */
int SeeedmmWave::read(char* data, int length) {
  return _serial ? _serial->readBytes(data, length) : 0;
}

/**
 * @brief Write data to the serial port.
 *
 * @param data The buffer containing the data to be written.
 * @param length The number of bytes to write.
 * @return The number of bytes actually written.
 */
int SeeedmmWave::write(const char* data, int length) {
  return _serial ? _serial->write(data, length) : 0;
}

/**
 * @brief Extract a float value from a byte array.
 *
 * @param bytes The byte array containing the float value.
 * @return The extracted float value.
 */
float SeeedmmWave::extractFloat(const uint8_t* bytes) const {
  return *reinterpret_cast<const float*>(bytes);
}

/**
 * @brief Extract a 32-bit unsigned integer from a byte array.
 *
 * This function extracts a 32-bit unsigned integer from the provided byte
 * array.
 *
 * @param bytes The byte array containing the 32-bit unsigned integer.
 * @return The extracted 32-bit unsigned integer.
 */
uint32_t SeeedmmWave::extractU32(const uint8_t* bytes) const {
  return *reinterpret_cast<const uint32_t*>(bytes);
}

/**
 * @brief Calculate the checksum for a byte array.
 *
 * This function calculates the checksum for the provided byte array using an
 * XOR-based checksum algorithm.
 *
 * @param data The byte array to calculate the checksum for.
 * @param len The length of the byte array.
 * @return The calculated checksum.
 */
uint8_t SeeedmmWave::calculateChecksum(const uint8_t* data, size_t len) {
  uint8_t checksum = 0;
  for (size_t i = 0; i < len; i++) {
    checksum ^= data[i];
  }
  checksum = ~checksum;
  return checksum;
}

/**
 * @brief Validate the checksum of a byte array.
 *
 * This function validates the checksum of the provided byte array by comparing
 * it to the expected checksum.
 *
 * @param data The byte array to validate.
 * @param len The length of the byte array.
 * @param expected_checksum The expected checksum.
 * @return True if the checksum is valid, false otherwise.
 */
bool SeeedmmWave::validateChecksum(const uint8_t* data, size_t len,
                                   uint8_t expected_checksum) {
  return calculateChecksum(data, len) == expected_checksum;
}

/**
 * @brief Convert a float value to a byte array.
 *
 * This function converts a float value to a byte array.
 *
 * @param value The float value to convert.
 * @param bytes The byte array to store the converted value.
 */
void SeeedmmWave::floatToBytes(float value, uint8_t* bytes) {
  uint8_t* p = reinterpret_cast<uint8_t*>(&value);
  for (size_t i = 0; i < sizeof(float); ++i) {
    bytes[i] = p[i];
  }
}

/**
 * @brief Convert a 32-bit unsigned integer to a byte array.
 *
 * This function converts a 32-bit unsigned integer to a byte array.
 *
 * @param value The 32-bit unsigned integer to convert.
 * @param bytes The byte array to store the converted value.
 */
void SeeedmmWave::uint32ToBytes(uint32_t value, uint8_t* bytes) {
  uint8_t* p = reinterpret_cast<uint8_t*>(&value);
  for (size_t i = 0; i < sizeof(uint32_t); ++i) {
    bytes[i] = p[i];
  }
}

/**
 * @brief Send a frame of data.
 *
 * @attention This function constructs and sends a frame of data, including the frame
 * header, data, and checksums.
 *
 * @param type The type of the frame.
 * @param data The data to include in the frame. Defaults to nullptr.
 * @param len The length of the data. Defaults to 0.
 * @return True if the frame is sent successfully, false otherwise.
 */
bool SeeedmmWave::sendFrame(uint16_t type, const uint8_t* data = nullptr,
                            size_t len = 0) {
  std::vector<uint8_t> frame;   // SOF, ID, LEN, TYPE, HEAD_CKSUM,
                                // DATA, DATA_CKSUM
  frame.push_back(SOF_BYTE);    // Start of Frame
  frame.push_back(_id >> 8);    // ID
  frame.push_back(_id & 0xFF);  // ID
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
#if _MMWAVE_DEBUG == 1
  Serial.print("Send<<<");
  printHexBuff(frame);
#endif
  _id++;
  return true;
}

/**
 * @brief Fetch data from the sensor.
 *
 * @attention This function attempts to fetch a frame of data from the sensor within the
 * specified timeout period.
 *
 * @param data_type The expected data type of the frame.
 * @param timeout The timeout period in milliseconds.
 * @return True if a frame is successfully fetched and processed, false
 * otherwise.
 */
bool SeeedmmWave::fetch(uint16_t data_type, uint32_t timeout) {
  uint32_t expire_time = millis() + timeout;
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
        if (frameBuffer.size() >= SIZE_FRAME_HEADER &&
            frameBuffer.size() == expectedFrameLength(frameBuffer)) {
          if (processFrame(frameBuffer.data(), frameBuffer.size(), data_type)) {
#if _MMWAVE_DEBUG == 1
            Serial.print("Recieved>>>");
            printHexBuff(frameBuffer);
#endif
            return true;
          }
          startFrame = false;
          frameBuffer.clear();
        }
      }
    }
  }
  return false;
}

/**
 * @brief Process a received frame of data.
 *
 * @attention This function processes a received frame of data, validating the checksums
 * and handling the frame based on its type.
 *
 * @param frame_bytes The byte array containing the frame data.
 * @param len The length of the frame data.
 * @param data_type The expected data type of the frame. Defaults to 0xFFFF.
 * @return True if the frame is successfully processed, false otherwise.
 */
bool SeeedmmWave::processFrame(const uint8_t* frame_bytes, size_t len,
                               uint16_t data_type = 0xFFFF) {
  if (len < SIZE_FRAME_HEADER)
    return false;  // Not enough data to process header

  uint16_t id        = (frame_bytes[1] << 8) | frame_bytes[2];
  uint16_t data_len  = (frame_bytes[3] << 8) | frame_bytes[4];
  uint16_t type      = (frame_bytes[5] << 8) | frame_bytes[6];
  uint8_t head_cksum = frame_bytes[7];
  uint8_t data_cksum = frame_bytes[SIZE_FRAME_HEADER + data_len];

  // Only proceed if the type matches or if data_type is set to the default,
  // indicating no specific type is required
  if (data_type != 0xFFFF && data_type != type)
    return false;

  // Checksum validation
  if (!validateChecksum(frame_bytes, SIZE_FRAME_HEADER - SIZE_DATA_CKSUM,
                        head_cksum) ||
      !validateChecksum(&frame_bytes[SIZE_FRAME_HEADER], data_len,
                        data_cksum)) {
    return false;
  }

  return handleType(type, &frame_bytes[SIZE_FRAME_HEADER], data_len);
}