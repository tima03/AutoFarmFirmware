#pragma once

#include <U8g2lib.h>
#include <Arduino.h>

struct menuStruct {
    const char p_name[128];
    const int8_t val1;
};

class simpleMenu {
private:
    const menuStruct* const _pCtrls;
    const int8_t _pNum;
    int8_t _pointer;
    int8_t _encoderPos;
    int8_t _currentPage;
    int8_t _totalPages;
    char buffer[128];
    U8G2_ST7567_ENH_DG128064I_F_SW_I2C& _LCD;

public:
    simpleMenu(const menuStruct* const pointCtrls, U8G2_ST7567_ENH_DG128064I_F_SW_I2C& objectLCD, const int8_t pointsNum)
        : _pCtrls(pointCtrls), _LCD(objectLCD), _pNum(pointsNum), _pointer(0), _encoderPos(0), _currentPage(0)
    {
        _totalPages = 1 + ((_pNum - 4) + 3) / 4;
        _LCD.setI2CAddress(0x3F * 2);
        Serial.println("Initializing LCD");
        _LCD.begin();
        Serial.println("LCD initialized");
        _LCD.enableUTF8Print();
        _LCD.setDisplayRotation(U8G2_R2);
        _LCD.setContrast(200);
        _LCD.setFont(u8g2_font_6x12_t_cyrillic);
    }

    void updateEncoder(bool isRight, bool isLeft) {
        Serial.print("Encoder update - Right: ");
        Serial.print(isRight);
        Serial.print(", Left: ");
        Serial.println(isLeft);

        if (isRight) {
            _encoderPos++;
            Serial.print("Encoder pos: ");
            Serial.println(_encoderPos);
            if (_encoderPos >= 2) {
                int8_t maxItems = (_currentPage == 0) ? min(3, _pNum - 1) : min(4, _pNum - (_currentPage * 4));
                if (_pointer < maxItems - 1) {
                    _pointer++;
                    Serial.print("Pointer incremented to: ");
                    Serial.println(_pointer);
                } else if (_currentPage < _totalPages - 1) {
                    _currentPage++;
                    _pointer = 0;
                    Serial.print("Page incremented to: ");
                    Serial.println(_currentPage);
                } else {
                    _currentPage = 0;
                    _pointer = 0;
                    Serial.println("Wrapped to first page, first item");
                }
                _encoderPos = 0;
                DrawMenu();
            }
        }
        if (isLeft) {
            _encoderPos--;
            Serial.print("Encoder pos: ");
            Serial.println(_encoderPos);
            if (_encoderPos <= -2) {
                int8_t maxItems = (_currentPage == 0) ? min(3, _pNum - 1) : min(4, _pNum - (_currentPage * 4));
                if (_pointer > 0) {
                    _pointer--;
                    Serial.print("Pointer decremented to: ");
                    Serial.println(_pointer);
                } else if (_currentPage > 0) {
                    _currentPage--;
                    _pointer = min(3, _pNum - (_currentPage * 4) - 1);
                    Serial.print("Page decremented to: ");
                    Serial.println(_currentPage);
                } else {
                    _currentPage = _totalPages - 1;
                    _pointer = min(3, _pNum - (_currentPage * 4) - 1);
                    Serial.print("Wrapped to last page, last item: ");
                    Serial.println(_pointer);
                }
                _encoderPos = 0;
                DrawMenu();
            }
        }
    }

    bool DrawMenu() {
        Serial.print("Drawing menu - Page: ");
        Serial.print(_currentPage);
        Serial.print(", Pointer: ");
        Serial.println(_pointer);

        _LCD.clearBuffer();

        if (_currentPage == 0) {
            strcpy_P(buffer, _pCtrls[0].p_name);
            int8_t textLength = strlen(buffer);  // Точная длина строки в байтах
            int8_t textWidth = textLength * 6 / 2;  // Ширина текста (6 пикселей на символ, делим на 2 для UTF-8)
            int8_t x = (128 - textWidth) / 2;  // Центрирование: (ширина экрана - ширина текста) / 2
            _LCD.drawUTF8(x, 10, buffer);  // Центрированный заголовок
            Serial.println("Title centered");

            for (int8_t i = 0; i < 3 && (i + 1) < _pNum; i++) {
                strcpy_P(buffer, _pCtrls[i + 1].p_name);
                int8_t yFrame = 16 * (i + 1);  // 16, 32, 48
                int8_t yText = yFrame + 10;     // 26, 42, 58
                if (i == _pointer) {
                    _LCD.drawRFrame(0, yFrame, 128, 15, 5);  // Отступ 5 пикселей, ширина 118 (128 - 5 - 5)
                    Serial.print("Frame drawn at item: ");
                    Serial.println(i);
                }
                _LCD.drawUTF8(5, yText, buffer);  // Отступ 5 пикселей для текста
            }
        } else {
            int8_t startIndex = 4 + (_currentPage - 1) * 4;
            for (int8_t i = 0; i < 4 && (startIndex + i) < _pNum; i++) {
                strcpy_P(buffer, _pCtrls[startIndex + i].p_name);
                int8_t yFrame = 16 * i;      // 0, 16, 32, 48
                int8_t yText = yFrame + 10;  // 10, 26, 42, 58
                if (i == _pointer) {
                    _LCD.drawRFrame(0, yFrame, 128, 15, 5);  // Отступ 5 пикселей, ширина 118
                    Serial.print("Frame drawn at item: ");
                    Serial.println(i);
                }
                _LCD.drawUTF8(5, yText, buffer);  // Отступ 5 пикселей для текста
            }
        }

        _LCD.sendBuffer();
        Serial.println("Buffer sent");
        return true;
    }

    int8_t getPointer() {
        if (_currentPage == 0) {
            return _pointer + 1;
        }
        return 4 + (_currentPage - 1) * 4 + _pointer;
    }
};