#include <SimpleMenu.h>
#include <U8g2lib.h>
#include <GParser.h>
#include "GyverEncoder.h"
#include <Wire.h>

#define PIN_LED 13
#define S2 18
#define S1 19
#define KEY 20
#define OLEDSCL A0
#define OLEDSDA A1
#define MenuPointsNum 20

String inString;
Encoder enc(S1, S2, KEY); 
U8G2_ST7567_ENH_DG128064I_F_SW_I2C oled(U8G2_R0, OLEDSCL, OLEDSDA, U8X8_PIN_NONE);
simpleMenu* menu = nullptr;

const menuStruct points [MenuPointsNum] PROGMEM = {
    {"Главное меню", 0},     // Заголовок
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
    enc.setType(TYPE2);
    
    Serial.println("Starting setup");
    menu = new simpleMenu(points, oled, MenuPointsNum);
    Serial.println("Menu created");
    menu->DrawMenu();
    Serial.println("Menu drawn");
}

void loop() {
    enc.tick();
    bool right = enc.isRight();
    bool left = enc.isLeft();
    if (right || left) {
        Serial.print("Detected - Right: ");
        Serial.print(right);
        Serial.print(", Left: ");
        Serial.println(left);
        menu->updateEncoder(right, left);
    }
}

void serialEvent3() {
    while (Serial3.available()) {
        char str[30];
        int amount = Serial3.readBytesUntil(';', str, 30);
        str[amount] = NULL;
        GParser data(str);
        int datacount = data.split();
        if (data[0] != NULL && datacount >= 2) {
            switch (data.getInt(0)) {
                case 0: Serial.println(data[1]); break;
                case 1: Serial.print(data[1]); break;
                case 2: Serial.println(" "); break;
                case 3:
                    if (data.getInt(1) == 1) {
                        digitalWrite(PIN_LED, HIGH);
                        Serial.println("Светодиод включен");
                    } else {
                        digitalWrite(PIN_LED, LOW);
                        Serial.println("Светодиод выключен");
                    }
                    break;
                default:
                    String strout(str);
                    Serial.println("Нет такого ключа: " + strout);
            }
        }
    }
}