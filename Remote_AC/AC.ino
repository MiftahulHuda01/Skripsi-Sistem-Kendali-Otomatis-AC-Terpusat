void initAC() {
  Serial.println("===== initAC =====");
  irsend.begin();
  acAux.begin();
  acMidea.begin();
  acDaikin.begin();
  acSamsung.begin();
  acGree.begin();
  acLg.begin();
  acPanasonic.begin();
}