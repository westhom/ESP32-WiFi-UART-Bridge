#include <Arduino.h>

#define WIFI_NAME "my wifi network"
#define WIFI_PASS ""

// for UART bridge service
#define TCP_LISTEN_PORT 23

#define SERIAL_BAUD_RATE 250000

// default ESP32 UART2
#define SERIAL_RX_GPIO_PIN 16
#define SERIAL_TX_GPIO_PIN 17

// uncomment to enable dedicated PSU relay TCP service
//#define ENABLE_PSU_RELAY

#ifdef ENABLE_PSU_RELAY
  #define TCP_RELAY_CONTROL_PORT 5050
  #define PSU_RELAY_PIN 33
  #define PSU_RELAY_ON HIGH
  #define PSU_RELAY_OFF LOW
#endif