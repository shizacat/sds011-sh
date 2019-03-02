/* sds011 library by Alexey M
 */

#include "sds011.h"

/**
 * Methods
 */
void Sds011::begin()
{
    // initialize hardware
    device_serial.begin(9600, SERIAL_8N1);
}

/**
 * Get data reporting mode
 */
int8_t Sds011::cmd_data_report()
{
    packet_out packet;
    packet_in  answer;
    int8_t r;

    packet_init(packet);
    packet.data[0] = 2;
    packet.data[1] = SETUP_GET;

    packet_send(packet);
    r = packet_read(answer);

    if (r != 0) return r;
    if (answer.command != 0xC5) return -3;

    return answer.data[2];
}

/**
 * Set data reporting mode
 */
int8_t Sds011::cmd_data_report(uint8_t mode){
    packet_out packet;
    packet_in  answer;
    int8_t r;

    packet_init(packet);
    packet.data[0] = 2;
    packet.data[1] = SETUP_SET;
    packet.data[2] = mode;

    packet_send(packet);
    r = packet_read(answer);

    if (r != 0) return r;
    if (answer.command != 0xC5) return -3;

    return 0;
}

/**
 * Get data from sensor by query
 * pm - mk g/m^2
 */
int8_t Sds011::cmd_data_query(float& pm25, float& pm10)
{
    packet_out packet;
    packet_in  answer;
    int8_t r;

    packet_init(packet);
    packet.data[0] = 4;

    packet_send(packet);
    r = packet_read(answer);

    if (r != 0) return r;
    if (answer.command != 0xC0) return -3;

    // convert
    pm25 = (float) ((answer.data[1]<<8) + answer.data[0]) / 10.0;
    pm10 = (float) ((answer.data[3]<<8) + answer.data[2]) / 10.0;

    return 0;
}

/**
 * Get sleep/work
 */
int8_t Sds011::cmd_sleep()
{
    packet_out packet;
    packet_in  answer;
    int8_t r;

    packet_init(packet);
    packet.data[0] = 6;
    packet.data[1] = SETUP_GET;

    packet_send(packet);
    r = packet_read(answer);

    if (r != 0) return r;
    if (answer.command != 0xC5) return -3;

    return answer.data[2];
}

/**
 * Set sleep/work
 */
int8_t Sds011::cmd_sleep(uint8_t mode)
{
    packet_out packet;
    packet_in  answer;
    int8_t r;

    packet_init(packet);
    packet.data[0] = 6;
    packet.data[1] = SETUP_SET;
    packet.data[2] = mode;

    packet_send(packet);
    r = packet_read(answer);

    if (r != 0) return r;
    if (answer.command != 0xC5) return -3;

    return 0;
}

/**
 * Get working period
 * Return interval
 */
int8_t Sds011::cmd_working_period()
{
    packet_out packet;
    packet_in  answer;
    int8_t r;

    packet_init(packet);
    packet.data[0] = 8;
    packet.data[1] = SETUP_GET;

    packet_send(packet);
    r = packet_read(answer);

    if (r != 0) return r;
    if (answer.command != 0xC5) return -3;

    return answer.data[2];
}

/**
 * Set working period
 * :param: interval 0：continuous(default)
 *                  1-30 minute：【work 30 seconds and sleep n*60-30 seconds】
 */
int8_t Sds011::cmd_working_period(uint8_t interval)
{
    packet_out packet;
    packet_in  answer;
    int8_t r;

    packet_init(packet);
    packet.data[0] = 8;
    packet.data[1] = SETUP_SET;
    packet.data[2] = interval;

    packet_send(packet);
    r = packet_read(answer);

    if (r != 0) return r;
    if (answer.command != 0xC5) return -3;

    return 0;
}

/**
 * Initialize packet
 */
void Sds011::packet_init(packet_out& packet)
{
    packet = {0};
    packet.head = 0xAA;
    packet.command = 0xB4;
    packet.tail = 0xAB;
    // All devices
    packet.data[13] = 0xFF;
    packet.data[14] = 0xFF;
}

/**
 * Calculation checksum
 */
void Sds011::packet_checksum(packet_out& packet)
{
    packet.checksum = 0;

    for(uint8_t i=0; i < sizeof(packet.data); i++){
        packet.checksum += packet.data[i];
    }
}

/**
 * Sending packet
 */
void Sds011::packet_send(packet_out& packet)
{
    uint8_t* p = (uint8_t*) &packet;

    packet_checksum(packet);

    for (uint8_t i=0; i < sizeof(packet); i++) {
        device_serial.write(p[i]);
    }
    device_serial.flush();
}

/**
 * Reading packet
 * return: negative value if error; 0 if success
 */
int8_t Sds011::packet_read(packet_in& packet)
{
    uint8_t count = 0;
    uint8_t checksum = 0;

    uint8_t* p = (uint8_t*) &packet;

    while (count != sizeof(packet)){
        if (device_serial.available() > 0) {
            p[count] = device_serial.read();
            count += 1;
        }
    }

    for (uint8_t i=0; i < sizeof(packet.data); i++){
        checksum += packet.data[i];
    }

    if (checksum != packet.checksum) return -2;
    if ((packet.head != 0xAA) || (packet.tail != 0xAB)) return -1;

    return 0;
}
