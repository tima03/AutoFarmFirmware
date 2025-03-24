#include <GParser.h>
#include <MemoryFree.h>
#include <EEPROM.h>

#define PIN_LED 13  // вывод светодиода
String inString;

// Настройка
void setup() {
  // Инициализация портов и выходов
  Serial.begin(115200);
  Serial3.begin(115200);
  Serial.setTimeout(5);
  Serial3.setTimeout(5);
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);
}

// Выполнение
void loop() {
}

// Проверка события на порту Serial3
void serialEvent3() {
  while (Serial3.available()) {
    // Чтение данных из порта Serial3
    char str[30];
    int amount = Serial3.readBytesUntil(';', str, 30);
    str[amount] = NULL;

    GParser data(str);
    int datacount = data.split();
    if (data[0] != NULL && datacount>=2) {
      switch (data.getInt(0)) {
        case 0: //вывод с новой строки
          Serial.println(data[1]);
          break;
        case 1: //вывод подряд
          Serial.print(data[1]);
          break;
        case 2: //отступ строки
          Serial.println(" ");
          break;
        case 3: //Управление светодиодом
          if(data.getInt(1)==1){
            digitalWrite(PIN_LED, HIGH);
            Serial.println("Светодиод включен");
          }else{
            digitalWrite(PIN_LED, LOW);
            Serial.println("Светодиод выключен");
          }
          break;
        default:
          String strout(str);
          Serial.println("Нет такого ключа: "+ strout);
          // код для выполнения
      }
    }
  }
}
