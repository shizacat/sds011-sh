#pragma once

/* sds011 library by Alexey M
   Version protocol 1.3
 */

// This will load the definition for common Particle variable types
#include "Particle.h"

#define SETUP_GET 0
#define SETUP_SET 1

#define CMD_REPORT_MODE_ACTIVE 0
#define CMD_REPORT_MODE_QUERY 1

#define CMD_SLEEP_SLEEP 0
#define CMD_SLEEP_WORK 1

typedef struct {
  uint8_t head;
  uint8_t command;
  uint8_t data[15];
  uint8_t checksum;
  uint8_t tail;
} packet_out;

typedef struct {
  uint8_t head;
  uint8_t command;
  uint8_t data[6];
  uint8_t checksum;
  uint8_t tail;
} packet_in;

class Sds011
{
public:
  Sds011(): device_serial(Serial1) {};
  Sds011(USARTSerial& serial): device_serial(serial) {};

  void begin();
  void flush_in_buffer();

  int8_t cmd_data_report();  // get
  int8_t cmd_data_report(uint8_t mode); // set

  int8_t cmd_data_query(float& pm25, float& pm10);  // get

  int8_t cmd_sleep();  // get
  int8_t cmd_sleep(uint8_t mode);  // set

  int8_t cmd_working_period(); // get
  int8_t cmd_working_period(uint8_t interval); //set

  // void cmd_firmware(); // get
private:
  USARTSerial& device_serial;

  void packet_init(packet_out& packet);
  void packet_checksum(packet_out& packet);
  void packet_send(packet_out& packet);
  int8_t packet_read(packet_in& packet);
};
