void initWebServer() {
  Serial.println("===== initWebServer =====");
  server.on("/", HTTP_GET, handleRoot);
  server.on("/api/hardwareid", HTTP_GET, handleHardwareId);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");

  if (!MDNS.begin("ac-jtik")) {
    Serial.println("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }
  Serial.println("mDNS responder started");
}

void handleRoot() {
  String html = "<!DOCTYPE html>\
<html lang=\"en\">\
<head>\
    <meta charset=\"UTF-8\">\
    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\
    <title>AC Control Dashboard</title>\
    <link rel=\"stylesheet\" href=\"https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.4.0/css/all.min.css\">\
    <style>\
        * { margin: 0; padding: 0; box-sizing: border-box; font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; }\
        body { background: linear-gradient(135deg, #1a2a6c, #2a4d8f); color: #fff; min-height: 100vh; display: flex; justify-content: center; align-items: center; padding: 20px; }\
        .dashboard { width: 100%; max-width: 500px; display: flex; flex-direction: column; align-items: center; gap: 30px; }\
        header { text-align: center; margin-bottom: 10px; width: 100%; }\
        h1 { font-size: 2.5rem; margin-bottom: 10px; text-shadow: 0 2px 4px rgba(0,0,0,0.3); }\
        .subtitle { font-size: 1.1rem; opacity: 0.8; }\
        .card { background: rgba(255, 255, 255, 0.1); backdrop-filter: blur(10px); border-radius: 16px; padding: 25px; box-shadow: 0 8px 32px rgba(0, 0, 0, 0.1); border: 1px solid rgba(255, 255, 255, 0.2); transition: transform 0.3s ease; width: 100%; max-width: 400px; }\
        .card:hover { transform: translateY(-5px); }\
        .card-title { font-size: 1.5rem; margin-bottom: 20px; display: flex; align-items: center; gap: 10px; justify-content: center; }\
        .card-title i { font-size: 1.8rem; }\
        .qr-container { display: flex; flex-direction: column; align-items: center; gap: 20px; }\
        #qrcode { background: white; padding: 15px; border-radius: 10px; box-shadow: 0 4px 15px rgba(0, 0, 0, 0.2); }\
        .device-info { text-align: center; background: rgba(255, 255, 255, 0.1); padding: 15px; border-radius: 10px; width: 100%; }\
        .device-id { font-size: 1.2rem; font-weight: bold; margin-bottom: 5px; }\
        .device-status { display: flex; align-items: center; justify-content: center; gap: 8px; margin-top: 10px; }\
        .status-indicator { width: 12px; height: 12px; border-radius: 50%; background: #4CAF50; box-shadow: 0 0 10px #4CAF50; animation: pulse 2s infinite; }\
        @keyframes pulse { 0% { box-shadow: 0 0 0 0 rgba(76, 175, 80, 0.7); } 70% { box-shadow: 0 0 0 10px rgba(76, 175, 80, 0); } 100% { box-shadow: 0 0 0 0 rgba(76, 175, 80, 0); } }\
        .button-container { display: flex; gap: 15px; margin-top: 15px; justify-content: center; flex-wrap: wrap; }\
        .btn { background: #4CAF50; color: white; border: none; border-radius: 8px; padding: 12px 20px; font-size: 1rem; font-weight: 600; cursor: pointer; transition: all 0.3s ease; display: flex; align-items: center; gap: 8px; }\
        .btn:hover { background: #45a049; transform: translateY(-2px); }\
        .btn-download { background: #2196F3; }\
        .btn-download:hover { background: #0b7dda; }\
        .notification { position: fixed; bottom: 20px; left: 50%; transform: translateX(-50%) translateY(100px); background: #4CAF50; color: white; padding: 15px 25px; border-radius: 8px; box-shadow: 0 4px 15px rgba(0, 0, 0, 0.2); opacity: 0; transition: all 0.3s ease; z-index: 1000; display: flex; align-items: center; gap: 10px; }\
        .notification.show { transform: translateX(-50%) translateY(0); opacity: 1; }\
        .notification.error { background: #f44336; }\
    </style>\
</head>\
<body>\
    <div class=\"dashboard\">\
        <header>\
            <h1><i class=\"fas fa-wind\"></i> AC Control Dashboard</h1>\
            <p class=\"subtitle\">Monitor and control your air conditioning system</p>\
        </header>\
        <div class=\"card\">\
            <h2 class=\"card-title\"><i class=\"fas fa-qrcode\"></i> Device QR Code</h2>\
            <div class=\"qr-container\">\
                <div id=\"qrcode\"></div>\
                <div class=\"device-info\">\
                    <div class=\"device-id\" id=\"device-id\">"
                + hardwareId + "</div>\
                    <div class=\"device-status\">\
                        <div class=\"status-indicator\"></div>\
                        <span>Online</span>\
                    </div>\
                </div>\
                <div class=\"button-container\">\
                    <button class=\"btn btn-download\" id=\"download-btn\">\
                        <i class=\"fas fa-download\"></i> Download QR Code\
                    </button>\
                </div>\
            </div>\
        </div>\
    </div>\
    <div class=\"notification\" id=\"notification\">\
        <i class=\"fas fa-check-circle\"></i> <span id=\"notification-text\">Action completed successfully!</span>\
    </div>\
    <script src=\"https://cdn.jsdelivr.net/npm/qrcodejs@1.0.0/qrcode.min.js\"></script>\
    <script>\
        document.addEventListener('DOMContentLoaded', function() {\
            const qrcodeContainer = document.getElementById('qrcode');\
            const hardwareId = document.getElementById('device-id').textContent;\
            \
            new QRCode(qrcodeContainer, {\
                text: hardwareId,\
                width: 200,\
                height: 200,\
                colorDark: \"#000000\",\
                colorLight: \"#ffffff\",\
                correctLevel: QRCode.CorrectLevel.H,\
                useSVG: false\
            });\
            \
            const downloadBtn = document.getElementById('download-btn');\
            const notification = document.getElementById('notification');\
            const notificationText = document.getElementById('notification-text');\
            \
            downloadBtn.addEventListener('click', function() {\
                const canvas = qrcodeContainer.querySelector('canvas');\
                \
                if (canvas) {\
                    const link = document.createElement('a');\
                    link.download = `AC-QR-${hardwareId}.png`;\
                    link.href = canvas.toDataURL('image/png');\
                    \
                    document.body.appendChild(link);\
                    link.click();\
                    document.body.removeChild(link);\
                    \
                    notificationText.textContent = 'QR Code downloaded successfully!';\
                    notification.classList.remove('error');\
                    notification.classList.add('show');\
                    \
                    setTimeout(() => {\
                        notification.classList.remove('show');\
                    }, 3000);\
                } else {\
                    notificationText.textContent = 'Error: Could not download QR Code';\
                    notification.classList.add('error');\
                    notification.classList.add('show');\
                    \
                    setTimeout(() => {\
                        notification.classList.remove('show');\
                    }, 3000);\
                }\
            });\
        });\
    </script>\
</body>\
</html>";

  server.send(200, "text/html", html);
}

void handleHardwareId() {
  String json = "{\"hardwareId\":\"" + hardwareId + "\"}";
  server.send(200, "application/json", json);
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}