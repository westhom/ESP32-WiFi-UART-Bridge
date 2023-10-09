#include <Arduino.h>
#include <ArduinoOTA.h>
#include <WiFi.h>
#include "config.h"
#include "AsyncTCP.h"

AsyncServer server(TCP_LISTEN_PORT);

bool serialClientConnected = false;
AsyncClient* activeClient;

char next[2] = {0, '\0'};
char pinStatus[2] = {0, '\0'};

void onSerialClient(void *arg, AsyncClient *client) {
  if( serialClientConnected ){
    client->close(true);
    return;
  }

  serialClientConnected = true;
  activeClient = client;

  client->onData([](void *arg, AsyncClient *client, void *data, size_t len) {
    Serial2.write((uint8_t *)data, len);
    // Serial.write("TCP onData: ");
    // Serial.write((uint8_t *)data, len);
  }, NULL);

  client->onDisconnect([](void *arg, AsyncClient *client) {
    serialClientConnected = false;
    activeClient = NULL;
  }, NULL);
}

#ifdef ENABLE_PSU_RELAY
  AsyncServer relayServer(TCP_RELAY_CONTROL_PORT);

  void onRelayClient(void *arg, AsyncClient *client){
    client->onData([](void *arg, AsyncClient *client, void *data, size_t len) {
      char cmd = ((char *)data)[0];

      if( cmd == '1' || cmd == 0x01 ){
        digitalWrite(PSU_RELAY_PIN, PSU_RELAY_ON);
      }
      else if( cmd == '0' || cmd == 0x00 ){
        digitalWrite(PSU_RELAY_PIN, PSU_RELAY_OFF);
      }
      else if( cmd == 'S' ){
        pinStatus[0] = (char) (digitalRead(PSU_RELAY_PIN) + 48);
        client->write(pinStatus);
        client->send();
      }

      client->close(true);
    }, NULL);
  }
#endif

void setup() {
  #ifdef ENABLE_PSU_RELAY
    pinMode(PSU_RELAY_PIN, OUTPUT);
  #endif

  // initialize debug serial
  Serial.begin(SERIAL_BAUD_RATE);

  // initial 3d printer serial port
  Serial2.begin(SERIAL_BAUD_RATE, SERIAL_8N1, SERIAL_RX_GPIO_PIN, SERIAL_TX_GPIO_PIN);

  // connect to wifi network
  WiFi.mode(WIFI_STA);  
  WiFi.begin(WIFI_NAME, WIFI_PASS);
  WiFi.setHostname("esp32-serial-bridge");

  Serial.print("Connecting to WiFi network");

  int rounds = 0;

  while(WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
    rounds++;

    if( rounds > 10 ){
      ESP.restart();
    }
  }

  Serial.println();

  ArduinoOTA.setHostname("esp32-serial-bridge");
  ArduinoOTA.begin();

  Serial.print("Connected to WiFi network with IP: ");
  Serial.println(WiFi.localIP());

  server.begin();
  server.onClient(onSerialClient, NULL);

  #ifdef ENABLE_PSU_RELAY
    relayServer.begin();
    relayServer.onClient(onRelayClient, NULL);
  #endif
}

void loop() {
  ArduinoOTA.handle();

  while(Serial2.available()){
    next[0] = (char) Serial2.read();

    if( serialClientConnected ){
      // Serial.write(next);
      activeClient->write(next);
      activeClient->send();
    }
  }
}