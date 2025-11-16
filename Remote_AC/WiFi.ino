void initWifi() {
  Serial.println("===== initWifi =====");
  WiFi.mode(WIFI_AP_STA);
  WiFi.setAutoReconnect(true);
  WiFi.setSleep(false);
  WiFi.onEvent(WiFiEvent);

  IPAddress ap_ip(192, 168, 4, 1);
  IPAddress ap_mask(255, 255, 255, 0);
  IPAddress ap_leaseStart(192, 168, 4, 2);
  IPAddress ap_dns(8, 8, 4, 4);

  WiFi.AP.begin();
  WiFi.AP.config(ap_ip, ap_ip, ap_mask, ap_leaseStart, ap_dns);
  WiFi.AP.create(hardwareId.c_str(), "ACjtikunm", 3);
  WiFi.AP.enableNAPT(true);
  if (!WiFi.AP.waitStatusBits(ESP_NETIF_STARTED_BIT, 1000)) {
    Serial.println(F("Failed to start AP!"));
    return;
  }
}

void connectToWifi() {
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(SSID_STA, PASSWORD_STA);
}

void WiFiEvent(WiFiEvent_t event) {
  switch (event) {
#if USING_CORE_ESP32_CORE_V200_PLUS

    case ARDUINO_EVENT_WIFI_READY:
      Serial.println("WiFi ready");
      break;

    case ARDUINO_EVENT_WIFI_STA_START:
      Serial.println("WiFi STA starting");
      break;

    case ARDUINO_EVENT_WIFI_STA_CONNECTED:
      Serial.println("WiFi STA connected");
      break;

    case ARDUINO_EVENT_WIFI_STA_GOT_IP6:
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
      Serial.println("WiFi connected");
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
      connectToMqtt();
      break;

    case ARDUINO_EVENT_WIFI_STA_LOST_IP:
      Serial.println("WiFi lost IP");
      break;

    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
      Serial.println("WiFi lost connection");
      xTimerStop(mqttReconnectTimer, 0); 
      break;
#else

    case SYSTEM_EVENT_STA_GOT_IP:
      Serial.println("WiFi connected");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
      connectToMqtt();
      break;

    case SYSTEM_EVENT_STA_DISCONNECTED:
      Serial.println("WiFi lost connection");
      xTimerStop(mqttReconnectTimer, 0);
      break;
#endif

    default:
      break;
  }
}