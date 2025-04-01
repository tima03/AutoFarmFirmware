#define WIFI_SSID "TP-Link_A263"
#define WIFI_PASS "1357913579"
#define DEBUG  // Включить отладку

#include <GyverDBFile.h>
#include <SettingsGyver.h>
#include <GParser.h>

#ifdef DEBUG
#define DEBUG_PRINT(x) Serial.print("@9998," + String(x) + ";")
#define DEBUG_PRINTLN(x) Serial.print("@9998," + String(x) + ";\n")
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTLN(x)
#endif

GyverDBFile db(&LittleFS, "/data.db");
SettingsGyver sett("My Settings", &db);

DB_KEYS(
    kk,
    toggle_led13
);

void build(sets::Builder& b) {
    if (b.Switch(kk::toggle_led13, "Переключи светодиод")) {
        delay(500);
        String cmd = "#0003," + String(db[kk::toggle_led13]) + ";";
        Serial.print(cmd);
        DEBUG_PRINTLN("Sent to Mega: " + cmd);
    }
}

void setup() {
    Serial.begin(115200);
    Serial.setTimeout(5);

    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print("@0001,.;");
        DEBUG_PRINT(".");
    }
    delay(500);
    Serial.print("@0002,Space;");
    delay(500);
    String ipMsg = "@0000,Connected: " + WiFi.localIP().toString() + ";";
    Serial.print(ipMsg);
    DEBUG_PRINTLN(ipMsg);

#ifdef ESP32
    LittleFS.begin(true);
#else
    LittleFS.begin();
#endif

    sett.begin();
    sett.onBuild(build);
    db.begin();
    db.init(kk::toggle_led13, 0);

    Serial.print("#0004,request_temp;");
    DEBUG_PRINTLN("Requested temperature from Mega");
}

void loop() {
    sett.tick();
    handleSerial();

    // Периодический запрос температуры каждые 10 секунд
    static unsigned long lastRequest = 0;
    if (millis() - lastRequest >= 10000) {
        Serial.print("#0004,request_temp;");
        DEBUG_PRINTLN("Requested temperature from Mega (periodic)");
        lastRequest = millis();
    }
}

void handleSerial() {
    if (Serial.available()) {
        char buffer[64];
        int len = Serial.readBytesUntil(';', buffer, 63);
        buffer[len] = '\0';

        if (buffer[0] == '#') {  // Команда
            GParser data(&buffer[1], ',');  // Пропускаем '#'
            int datacount = data.split();
            if (datacount >= 1) {
                int cmd = data.getInt(0);
                if (cmd == 5 && datacount >= 2) {  // Ответ с температурой
                    String msg = "@0005,Temperature received: " + String(data[1]) + ";";
                    Serial.print(msg);
                    DEBUG_PRINTLN(msg);
                }
            }
        }
    }
}