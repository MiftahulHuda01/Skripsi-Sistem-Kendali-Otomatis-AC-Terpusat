void initTime() {
  configTime(0, 0, "pool.ntp.org");
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    rtc.setTimeStruct(timeinfo);
    Serial.println("NTP time synced.");
  } else {
    Serial.println("Failed to sync NTP.");
  }
}

void updateTime() {
  struct tm t = rtc.getTimeStruct();
  int d = t.tm_wday;
  int h = t.tm_hour;
  int m = t.tm_min;

  hariKerja = (d >= 1 && d <= 5);
  jamKerja = ((h > 7 || (h == 7 && m >= 0)) && (h < 17 || (h == 17 && m <= 30)));
}

void applySchedule() {
  bool shouldOn = gerakan;

  if (gerakan != lastShouldOn) {
    Serial.print("Status berubah: ");
    Serial.println(shouldOn ? "ON" : "OFF");

    if (brand == "Midea") {
      if (shouldOn) {
        acMidea.on();
        acMidea.setTemp(temp);
        acMidea.setMode(mapModeCoolix(mode));
        acMidea.setFan(mapFanCoolix(fan));
      } else {
        acMidea.off();
      }
      acMidea.send();
    }

    else if (brand == "Aux") {
      acAux.setPower(shouldOn);
      if (shouldOn) {
        acAux.setTemp(temp);
        acAux.setMode(mapModeElectra(mode));
        acAux.setFan(mapFanElectra(fan));
      }
      acAux.send();
    }

    else if (brand == "Daikin") {
      acDaikin.setMode(kDaikin128Cool);
      acDaikin.setPowerToggle(shouldOn);
      acDaikin.setFan(kDaikin128FanAuto);
      acDaikin.send();
    }

    else if (brand == "Panasonic") {
      acPanasonic.setPower(shouldOn);
      if (shouldOn) {
        acPanasonic.setTemp(temp);
        acPanasonic.setMode(mapModePanasonic(mode));
        acPanasonic.setFan(mapFanPanasonic(fan));
      }
      acPanasonic.send();
    }

    else if (brand == "Gree") {
      acGree.setPower(shouldOn);
      if (shouldOn) {
        acGree.setTemp(temp);
        acGree.setMode(mapModeGree(mode));
        acGree.setFan(mapFanGree(fan));
      }
      acGree.send();
    }

    else if (brand == "Samsung") {
      acSamsung.setPower(shouldOn);
      if (shouldOn) {
        acSamsung.setTemp(temp);
        acSamsung.setMode(mapModeTcl112Ac(mode));
        acSamsung.setFan(mapFanTcl112Ac(fan));
      }
      acSamsung.send();
    }

    else if (brand == "LG") {
      acLg.setPower(shouldOn);
      if (shouldOn) {
        acLg.setTemp(temp);
        acLg.setMode(mapModeLg(mode));
        acLg.setFan(mapFanLg(fan));
      }
      acLg.send();
    }

    lastShouldOn = shouldOn;
  }
}
