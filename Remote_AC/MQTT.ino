void initMqtt() {
  Serial.println("===== initMqtt =====");
  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onSubscribe(onMqttSubscribe);
  mqttClient.onUnsubscribe(onMqttUnsubscribe);
  mqttClient.onMessage(onMqttMessage);
  mqttClient.onPublish(onMqttPublish);
  mqttClient.setCredentials("acjtik", "acjtik");
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
}

void connectToMqtt() {
  Serial.println("Connecting to MQTT...");
  mqttClient.connect();
}

void onMqttConnect(bool sessionPresent) {
  Serial.print("Connected to MQTT broker: ");
  Serial.print(MQTT_HOST);
  Serial.print(", port: ");
  Serial.println(MQTT_PORT);
  Serial.print("PubTopic: ");
  Serial.println(hardwareId);

  printSeparationLine();
  Serial.print("Session present: ");
  Serial.println(sessionPresent);

  uint16_t packetIdSub = mqttClient.subscribe(hardwareId.c_str(), 1);
  Serial.print("Subscribing at QoS 0, packetId: ");
  Serial.println(packetIdSub);

  printSeparationLine();
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  (void)reason;

  Serial.println("Disconnected from MQTT.");
}

void onMqttSubscribe(const uint16_t& packetId, const uint8_t& qos) {
  Serial.println("Subscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
  Serial.print("  qos: ");
  Serial.println(qos);
}

void onMqttUnsubscribe(const uint16_t& packetId) {
  Serial.println("Unsubscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

void onMqttMessage(char* topic, char* payload, const AsyncMqttClientMessageProperties& properties,
                   const size_t& len, const size_t& index, const size_t& total) {
  (void)payload;
  char msg[len + 1];
  memcpy(msg, payload, len);
  msg[len] = '\0';

  String message = String(msg);

  Serial.println(message);

  JsonDocument doc;
  deserializeJson(doc, message);
  brand = doc["Brand"].as<String>();
  power = doc["Power"].as<int>();
  temp = doc["Suhu"].as<int>();
  mode = doc["Mode"].as<int>();
  fan = doc["Fan"].as<int>();
  swing = doc["Swing"].as<int>();

  Serial.println("[JSON PARSED DATA]");
  Serial.print(" Brand    : ");
  Serial.println(brand);
  Serial.print(" Power    : ");
  Serial.println(power);
  Serial.print(" Suhu     : ");
  Serial.println(temp);
  Serial.print(" Mode     : ");
  Serial.println(mode);
  Serial.print(" Fan      : ");
  Serial.println(fan);
  Serial.println("------------------------------------");

  if (brand == "Midea") {
    if (power == 1) {
      acMidea.on();
      acMidea.setTemp(temp);
      acMidea.setMode(mapModeCoolix(mode));
      acMidea.setFan(mapFanCoolix(fan));
      if (swing == 1) acMidea.setSwing();
      acMidea.send();
      Serial.println("✅ Midea ON signal sent");
    } else {
      acMidea.off();
      acMidea.send();
      Serial.println("✅ Midea OFF signal sent (Coolix48)");
    }

  } else if (brand == "Aux") {
    acAux.setPower(power == 1);
    acAux.setTemp(temp);
    acAux.setMode(mapModeElectra(mode));
    acAux.setFan(mapFanElectra(fan));
    acAux.setSwingV(swing == 1); 
    acAux.send();
    Serial.println("✅ AUX (Electra) command sent");

  } else if (brand == "Daikin") { 
    acDaikin.setPowerToggle(power == 1);
    acDaikin.setMode(mapModeDaikin128(mode));
    acDaikin.setFan(mapFanDaikin128(fan));
    acDaikin.setTemp(temp);
    acDaikin.setSwingVertical(swing == 1);
    acDaikin.send();
    Serial.println("✅ Daikin command sent");

  } else if (brand == "Panasonic") { 
    acPanasonic.setPower(power == 1);
    acPanasonic.setTemp(temp);
    acPanasonic.setMode(mapModePanasonic(mode));
    acPanasonic.setFan(mapFanPanasonic(fan));
    acPanasonic.setSwingVertical(swing == 1);
    acPanasonic.send();
    Serial.println("✅ Panasonic command sent");

  } else if (brand == "Gree") { 
    acGree.setPower(power == 1);
    acGree.setTemp(temp);
    acGree.setMode(mapModeGree(mode));
    acGree.setFan(mapFanGree(fan));
    if (swing == 1) acGree.getSwingVerticalPosition();
    acGree.send();
    Serial.println("✅ Gree command sent");

  } else if (brand == "Samsung") {  
    acSamsung.setPower(power == 1);
    acSamsung.setTemp(temp);
    acSamsung.setMode(mapModeTcl112Ac(mode));
    acSamsung.setFan(mapFanTcl112Ac(fan));
    acSamsung.setSwingVertical(swing == 1);
    acSamsung.send();
    Serial.println("✅ Samsung (TCL112AC) command sent");

  } else if (brand == "LG") { 
    acLg.setPower(power == 1);
    acLg.setTemp(temp);
    acLg.setMode(mapModeLg(mode));
    acLg.setFan(mapFanLg(fan));
    // acLg.setSwingV(swing == 1);
    acLg.send();
    Serial.println("✅ LG command sent");
  }
  publishStatus();
}
void onMqttPublish(const uint16_t& packetId) {
  Serial.println("Publish acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}
void publishStatus() {
  // Buat JSON payload status terkini
  JsonDocument doc;
  doc["Brand"] = brand;
  doc["Power"] = power;
  doc["Suhu"] = temp;
  doc["Mode"] = mode;
  doc["Fan"] = fan;
  doc["Swing"] = swing;

  String payload;
  serializeJson(doc, payload);

  String topic = hardwareId + "/status";

  mqttClient.publish(topic.c_str(), 1, true, payload.c_str());

  Serial.println("✅ Status published with retain flag");
}