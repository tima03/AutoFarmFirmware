#include <Arduino.h>

#define WIFI_SSID "TP-Link_A263"
#define WIFI_PASS "1357913579"

#include <GyverDBFile.h>
#include <LittleFS.h>
#include <SettingsGyver.h>
// база данных для хранения настроек
// будет автоматически записываться в файл при изменениях
GyverDBFile db(&LittleFS, "/data.db");

// указывается заголовок меню, подключается база данных
SettingsGyver sett("My Settings", &db);

// имена ячеек базы данных
DB_KEYS(
    kk,
    toggle_led13
);

// билдер! Тут строится наше окно настроек
void build(sets::Builder& b) {
    if (b.Switch(kk::toggle_led13,"Переключи светодиод")){
      delay(500);
      Serial.print("3,"+db[kk::toggle_led13].toString()+";");
    }
}

void setup() {
    Serial.begin(115200);
    Serial.setTimeout(5);

    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print("1,.;");
    }
    delay(500);
    Serial.print("2,Space;");
    delay(500);
    Serial.print("0,Connected: "+WiFi.localIP().toString()+";");

#ifdef ESP32
    LittleFS.begin(true);
#else
    LittleFS.begin();
#endif

    sett.begin();
    sett.onBuild(build);

    // запуск и инициализация полей БД
    db.begin();
    db.init(kk::toggle_led13, 0);
}

void loop() {
    // тикер, вызывать в лупе
    sett.tick();
}