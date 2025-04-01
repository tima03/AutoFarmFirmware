#include <SimpleMenu.h>
#include <U8g2lib.h>
#include <GParser.h>
#include "GyverEncoder.h"
#include <Wire.h>

#define DEBUG  // Включить отладку

#ifdef DEBUG
#define DEBUG_PRINT(x) Serial.print(x)
#define DEBUG_PRINTLN(x) Serial.println(x)
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTLN(x)
#endif

#define PIN_LED 13
#define S2 18
#define S1 19
#define KEY 20
#define OLEDSCL A0
#define OLEDSDA A1
#define MENU_POINTS_NUM 20

String inString;
Encoder encoder(S1, S2, KEY);
U8G2_ST7567_ENH_DG128064I_F_SW_I2C oled(U8G2_R0, OLEDSCL, OLEDSDA, U8X8_PIN_NONE);
SimpleMenu* menu = nullptr;

const MenuItem menu_items[MENU_POINTS_NUM] PROGMEM = {
    {"Главное меню", 0},
    {"Включить свет", 1},
    {"Выключить свет", 2},
    {"Заряд батареи", 3},
    {"Температура", 4},
    {"Влажность", 5},
    {"Датчик движения", 6},
    {"Настройки Wi-Fi", 7},
    {"Обновить прошивку", 8},
    {"Режим сна", 9},
    {"Таймер", 10},
    {"Яркость экрана", 11},
    {"Звук вкл/выкл", 12},
    {"Сброс настроек", 13},
    {"Тест дисплея", 14},
    {"Версия ПО", 15},
    {"Лог событий", 16},
    {"Калибровка", 17},
    {"Режим отладки", 18},
    {"Выход", 19},
};

void setup() {
    Serial.begin(115200);
    Serial3.begin(115200);
    Serial.setTimeout(5);
    Serial3.setTimeout(5);

    pinMode(PIN_LED, OUTPUT);
    digitalWrite(PIN_LED, LOW);
    encoder.setType(TYPE2);

    DEBUG_PRINTLN("Starting setup");
    menu = new SimpleMenu(menu_items, oled, MENU_POINTS_NUM);
    DEBUG_PRINTLN("Menu created");
    menu->DrawMenu();
    DEBUG_PRINTLN("Menu drawn");
}

void loop() {
    encoder.tick();
    bool right = encoder.isRight();
    bool left = encoder.isLeft();
    if (right || left) {
        menu->UpdateEncoder(right, left);
        int selected = menu->GetSelectedIndex();
        if (selected == 4) {  // "Температура" выбрана
            int temp = analogRead(A2) / 4;
            String msg = "#0005," + String(temp) + ";";
            Serial3.print(msg);
            DEBUG_PRINTLN("Sent to ESP: " + msg);
        }
    }

    // Обработка Serial3 в loop()
    while (Serial3.available()) {
        char str[64];
        int amount = Serial3.readBytesUntil(';', str, 63);
        str[amount] = '\0';

        if (str[0] == '#') {  // Команда
            GParser data(&str[1], ',');  // Пропускаем '#'
            int datacount = data.split();
            if (datacount >= 1) {
                int cmd = data.getInt(0);
                switch (cmd) {
                    case 3:  // Управление светодиодом
                        if (datacount >= 2) {
                            int value = data.getInt(1);
                            digitalWrite(PIN_LED, value ? HIGH : LOW);
                            String msg = "@0003,LED " + String(value ? "ON" : "OFF") + ";";
                            Serial3.print(msg);
                            DEBUG_PRINTLN("Sent to ESP: " + msg);
                        }
                        break;
                    case 4:  // Запрос температуры
                        if (datacount >= 2 && data[1] == "request_temp") {
                            int temp = analogRead(A2) / 4;
                            String msg = "#0005," + String(temp) + ";";
                            Serial3.print(msg);
                            DEBUG_PRINTLN("Sent to ESP: " + msg);
                        }
                        break;
                    default:
                        String msg = "@9999,Unknown command: " + String(cmd) + ";";
                        Serial3.print(msg);
                        DEBUG_PRINTLN("Sent to ESP: " + msg);
                        break;
                }
            }
        } else if (str[0] == '@') {  // Отладка от ESP
            String msg = String(&str[1]);
            int cmd = msg.substring(0, 4).toInt();
            String content = msg.substring(5);
            if (cmd == 9998) {
                DEBUG_PRINTLN("[ESP Debug] " + content);
            } else {
                DEBUG_PRINTLN("[ESP] " + msg);
            }
        }
    }
}