void initSensor() {
  Serial.println("=== DFRobot C4001 24GHz Human Presence (UART, simple) ===");
  Serial.print("Mendeteksi sensor C4001");
  while (!radar.begin()) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nSensor C4001 terdeteksi!");
}
void pollRadar() {
  unsigned long now = millis();
  if (now - lastPoll < POLL_INTERVAL) return;
  lastPoll = now;

  bool motion = radar.motionDetection();  

  if (motion) {
    lastMotionTime = now;  
  }

  if (now - lastMotionTime < ABSENCE_DELAY) {
    gerakan = true;
  } else {
    gerakan = false;
  }

  if (gerakan != lastPresence) {
    lastPresence = gerakan;
  }
}
