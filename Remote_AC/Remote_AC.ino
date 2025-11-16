#include "Config.h"

void setup() {
  Serial.begin(115200);

  // Generate hardware ID from MAC address
  uint64_t chipid = ESP.getEfuseMac();
  hardwareId = "AC-HW-" + String((uint16_t)(chipid >> 32), HEX) + String((uint32_t)chipid, HEX);
  hardwareId.toUpperCase();
  Serial.println(hardwareId);

  initAC();

  mqttReconnectTimer = xTimerCreate("mqttTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0,
                                    reinterpret_cast<TimerCallbackFunction_t>(connectToMqtt));

  initWifi();
  initWebServer();

  connectToWifi();

  initMqtt();

  initTime();
  initSensor();
}
void loop() {

  pollRadar();
  updateTime();

  if (hariKerja && jamKerja) {
    applySchedule();
  }

  server.handleClient();
  delay(100);
}
