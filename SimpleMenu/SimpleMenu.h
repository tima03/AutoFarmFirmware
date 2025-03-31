#pragma once

#include <U8g2lib.h>
#include <Arduino.h>
//in this struct there are the point parameters 
struct menuStruct {
    const char p_name[128];
    const int8_t val1;
};


class simpleMenu {
private:                    //references

    const menuStruct* const _pCtrls;
    const int8_t _pNum;
    int8_t _pointer;
    char buffer[128];
    U8G2_ST7567_ENH_DG128064I_F_SW_I2C& _LCD;

public:
    
    simpleMenu(const menuStruct* const pointCtrls, U8G2_ST7567_ENH_DG128064I_F_SW_I2C& objectLCD, const int8_t pointsNum):_pCtrls(pointCtrls), _LCD(objectLCD), _pNum(pointsNum) 
    {   
        _LCD.setI2CAddress(0x3F * 2);
        _LCD.begin();
        _LCD.enableUTF8Print();  
        _LCD.clearBuffer();  
        _LCD.setDisplayRotation(U8G2_R2) ;  
        _LCD.setContrast(200); 
        _LCD.setFont(u8g2_font_6x12_t_cyrillic);
        _pointer = 1;
        //_LCD.drawRFrame(0,0,128,15,5);
        //_LCD.drawUTF8(0, 10, "абвгдеёжзийклмнопрстуфхцчъыьэюя");
        //_LCD.sendBuffer();
        //objectLCD.drawRFrame(0,16,128,15,5);
        //objectLCD.drawUTF8(0, 26, "абвгдеёжзийклмнопрстуфхцчъыьэюя");
        //objectLCD.drawRFrame(0,32,128,15,5);
        //objectLCD.drawUTF8(0, 42, "абвгдеёжзийклмнопрстуфхцчъыьэюя");
        //objectLCD.drawRFrame(0,48,128,15,5);
        //objectLCD.drawUTF8(0, 58, "абвгдеёжзийклмнопрстуфхцчъыьэюя");
        
        //firstLaunch();
    }                   //public functions

public:
bool DrawMenu() {
    if (_pNum < 5) {
        for (int8_t i = 0; i < _pNum; i++) {
            // Читаем val1 из PROGMEM
            int8_t myVal = pgm_read_byte(&_pCtrls[i].val1);
            // Копируем имя пункта из PROGMEM в буфер
            strcpy_P(buffer, _pCtrls[i].p_name);
            if (_pointer == 1){_LCD.drawRFrame(0, 16, 128, 15, 5);}
            //_LCD.drawRFrame(0, 0, 128, 15, 5);
            //_LCD.drawRFrame(0, 16, 128, 15, 5);
            //_LCD.drawRFrame(0, 32, 128, 15, 5);
            //_LCD.drawRFrame(0, 48, 128, 15, 5);
            if (myVal == 0) {
                // Если val1 == 0, это заголовок — центрируем
                int8_t textLength = strlen(buffer)/4; // Длина строки в символах
                int8_t textWidth = textLength * 6;  // Ширина текста в пикселях (6 — ширина символа)
                int8_t x = 64 - textWidth;   // Центрируем: (ширина экрана - ширина текста) / 2
                _LCD.drawUTF8(x, 10, buffer);       // Выводим заголовок
            } else {
                // Если val1 != 0, это пункт меню — рисуем рамку и текст
                int8_t yPos = 10+16*i;
                _LCD.drawUTF8(5, yPos, buffer);  
            }
        }
    }
    _LCD.sendBuffer();
    return true;
}

};

