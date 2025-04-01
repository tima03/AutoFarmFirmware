#pragma once

#include <U8g2lib.h>
#include <Arduino.h>

#ifdef DEBUG
#define DEBUG_PRINT(x) Serial.print(x)
#define DEBUG_PRINTLN(x) Serial.println(x)
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTLN(x)
#endif

struct MenuItem {
    const char name[128];
    const int8_t value;
};

class SimpleMenu {
private:
    static constexpr int8_t kScreenWidth = 128;
    static constexpr int8_t kScreenHeight = 64;
    static constexpr int8_t kItemsPerPage = 4;
    static constexpr int8_t kFirstPageItems = 3;
    static constexpr int8_t kEncoderStepThreshold = 2;
    static constexpr int8_t kTextXOffset = 5;
    static constexpr int8_t kFrameWidth = kScreenWidth;
    static constexpr int8_t kFrameHeight = 15;
    static constexpr int8_t kLineSpacing = 16;
    static constexpr int8_t kTextYOffset = 10;
    static constexpr uint8_t kDefaultContrast = 200;
    static constexpr uint8_t kCharWidth = 6;  // Ширина символа в пикселях

    const MenuItem* const items_;
    const int8_t item_count_;
    U8G2_ST7567_ENH_DG128064I_F_SW_I2C& lcd_;
    int8_t pointer_;
    int8_t encoder_pos_;
    int8_t current_page_;
    int8_t total_pages_;
    char buffer_[128];

    void InitDisplay();
    int8_t CalculateTotalPages() const;
    void DrawFirstPage();
    void DrawOtherPage(int8_t start_index);
    void DrawItem(int8_t index, int8_t y_frame, bool is_selected);
    void HandleRightTurn();
    void HandleLeftTurn();
    int8_t GetMaxItemsOnPage() const;

public:
    SimpleMenu(const MenuItem* items, U8G2_ST7567_ENH_DG128064I_F_SW_I2C& lcd, int8_t item_count)
        : items_(items), item_count_(item_count), lcd_(lcd), pointer_(0), encoder_pos_(0), current_page_(0) {
        total_pages_ = CalculateTotalPages();
        InitDisplay();
    }

    void UpdateEncoder(bool is_right, bool is_left);
    bool DrawMenu();
    int8_t GetSelectedIndex() const;
};

inline void SimpleMenu::InitDisplay() {
    lcd_.setI2CAddress(0x3F * 2);
    DEBUG_PRINTLN("Initializing LCD");
    lcd_.begin();
    DEBUG_PRINTLN("LCD initialized");
    lcd_.enableUTF8Print();
    lcd_.setDisplayRotation(U8G2_R2);
    lcd_.setContrast(kDefaultContrast);
    lcd_.setFont(u8g2_font_6x12_t_cyrillic);
}

inline int8_t SimpleMenu::CalculateTotalPages() const {
    return 1 + ((item_count_ - kItemsPerPage) + (kItemsPerPage - 1)) / kItemsPerPage;
}

inline int8_t SimpleMenu::GetMaxItemsOnPage() const {
    return (current_page_ == 0) ? min(kFirstPageItems, item_count_ - 1)
                                : min(kItemsPerPage, item_count_ - (current_page_ * kItemsPerPage));
}

inline void SimpleMenu::HandleRightTurn() {
    int8_t max_items = GetMaxItemsOnPage();
    if (pointer_ < max_items - 1) {
        pointer_++;
        DEBUG_PRINT("Pointer incremented to: ");
        DEBUG_PRINTLN(pointer_);
    } else if (current_page_ < total_pages_ - 1) {
        current_page_++;
        pointer_ = 0;
        DEBUG_PRINT("Page incremented to: ");
        DEBUG_PRINTLN(current_page_);
    } else {
        current_page_ = 0;
        pointer_ = 0;
        DEBUG_PRINTLN("Wrapped to first page, first item");
    }
    encoder_pos_ = 0;
    DrawMenu();
}

inline void SimpleMenu::HandleLeftTurn() {
    int8_t max_items = GetMaxItemsOnPage();
    if (pointer_ > 0) {
        pointer_--;
        DEBUG_PRINT("Pointer decremented to: ");
        DEBUG_PRINTLN(pointer_);
    } else if (current_page_ > 0) {
        current_page_--;
        pointer_ = min(kFirstPageItems, item_count_ - (current_page_ * kItemsPerPage) - 1);
        DEBUG_PRINT("Page decremented to: ");
        DEBUG_PRINTLN(current_page_);
    } else {
        current_page_ = total_pages_ - 1;
        pointer_ = min(kFirstPageItems, item_count_ - (current_page_ * kItemsPerPage) - 1);
        DEBUG_PRINT("Wrapped to last page, last item: ");
        DEBUG_PRINTLN(pointer_);
    }
    encoder_pos_ = 0;
    DrawMenu();
}

inline void SimpleMenu::UpdateEncoder(bool is_right, bool is_left) {
    DEBUG_PRINT("Encoder update - Right: ");
    DEBUG_PRINT(is_right);
    DEBUG_PRINT(", Left: ");
    DEBUG_PRINTLN(is_left);

    if (is_right) {
        encoder_pos_++;
        DEBUG_PRINT("Encoder pos: ");
        DEBUG_PRINTLN(encoder_pos_);
        if (encoder_pos_ >= kEncoderStepThreshold) {
            HandleRightTurn();
        }
    }
    if (is_left) {
        encoder_pos_--;
        DEBUG_PRINT("Encoder pos: ");
        DEBUG_PRINTLN(encoder_pos_);
        if (encoder_pos_ <= -kEncoderStepThreshold) {
            HandleLeftTurn();
        }
    }
}

inline bool SimpleMenu::DrawMenu() {
    DEBUG_PRINT("Drawing menu - Page: ");
    DEBUG_PRINT(current_page_);
    DEBUG_PRINT(", Pointer: ");
    DEBUG_PRINTLN(pointer_);

    lcd_.clearBuffer();
    if (current_page_ == 0) {
        DrawFirstPage();
    } else {
        DrawOtherPage(kItemsPerPage + (current_page_ - 1) * kItemsPerPage);
    }
    lcd_.sendBuffer();
    DEBUG_PRINTLN("Buffer sent");
    return true;
}

inline void SimpleMenu::DrawFirstPage() {
    strcpy_P(buffer_, items_[0].name);
    uint8_t text_length = strlen(buffer_) / 2;  // Количество символов, а не байтов (UTF-8)
    uint8_t text_width = text_length * kCharWidth;
    int8_t x = (kScreenWidth - text_width) / 2;
    if (x < 0) x = 0;  // Предотвращаем отрицательные значения
    DEBUG_PRINT("Drawing title: ");
    DEBUG_PRINT(buffer_);
    DEBUG_PRINT(" (len: ");
    DEBUG_PRINT(text_length);
    DEBUG_PRINT(", width: ");
    DEBUG_PRINT(text_width);
    DEBUG_PRINT(") at x: ");
    DEBUG_PRINTLN(x);
    lcd_.drawUTF8(x, kTextYOffset, buffer_);

    for (int8_t i = 0; i < kFirstPageItems && (i + 1) < item_count_; i++) {
        DrawItem(i + 1, kLineSpacing * (i + 1), i == pointer_);
    }
}

inline void SimpleMenu::DrawOtherPage(int8_t start_index) {
    for (int8_t i = 0; i < kItemsPerPage && (start_index + i) < item_count_; i++) {
        DrawItem(start_index + i, kLineSpacing * i, i == pointer_);
    }
}

inline void SimpleMenu::DrawItem(int8_t index, int8_t y_frame, bool is_selected) {
    strcpy_P(buffer_, items_[index].name);
    int8_t y_text = y_frame + kTextYOffset;
    if (is_selected) {
        lcd_.drawRFrame(0, y_frame, kFrameWidth, kFrameHeight, 5);
        DEBUG_PRINT("Frame drawn at item: ");
        DEBUG_PRINTLN(index);
    }
    lcd_.drawUTF8(kTextXOffset, y_text, buffer_);
}

inline int8_t SimpleMenu::GetSelectedIndex() const {
    return (current_page_ == 0) ? pointer_ + 1 : kItemsPerPage + (current_page_ - 1) * kItemsPerPage + pointer_;
}