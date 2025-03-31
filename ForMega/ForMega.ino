#include <SimpleMenu.h>

#include <U8g2lib.h>
#include <GParser.h>
#include "GyverEncoder.h"


#include <Wire.h>

#define PIN_LED 13  // вывод светодиода
#define S2 18
#define S1 19
#define KEY 20
#define OLEDSCL A0
#define OLEDSDA A1
#define MenuPointsNum 4


String inString;
Encoder enc(S1, S2, KEY); 

U8G2_ST7567_ENH_DG128064I_F_SW_I2C oled(U8G2_R0, OLEDSCL, OLEDSDA, U8X8_PIN_NONE);

const menuStruct points [MenuPointsNum] PROGMEM= {
	{"Меню", 0},
	{"Реле", 1 },
	{"Лит энерджи", 2}, 
	{"Стрим ёлки", 3},  
};



void setup() {   
  simpleMenu menu(points, oled, MenuPointsNum);
  Serial.begin(115200);
  Serial3.begin(115200);
  Serial.setTimeout(5);
  Serial3.setTimeout(5);
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);
  enc.setType(TYPE2);
  menu.DrawMenu();
    
}

// Выполнение
void loop() {
  //menu.control();
  //enc.tick();

  //if (enc.isRight()) Serial.println("Right");         // если был поворот
  //if (enc.isLeft()) Serial.println("Left");
  
  //if (enc.isRightH()) Serial.println("Right holded"); // если было удержание + поворот
  //if (enc.isLeftH()) Serial.println("Left holded");
  
  //if (enc1.isPress()) Serial.println("Press");         // нажатие на кнопку (+ дебаунс)
  //if (enc1.isRelease()) Serial.println("Release");     // то же самое, что isClick
  
  //if (enc.isClick()) Serial.println("Click");         // одиночный клик
  //if (enc.isSingle()) Serial.println("Single");       // одиночный клик (с таймаутом для двойного)
  //if (enc.isDouble()) Serial.println("Double");       // двойной клик
  
  
  //if (enc.isHolded()) Serial.println("Holded");       // если была удержана и энк не поворачивался
  //if (enc1.isHold()) Serial.println("Hold");   

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
