#include "gfx.h"
#include <Adafruit_SSD1306.h>
#include <U8g2_for_Adafruit_GFX.h>

#ifdef CARDPUTER
extern const uint8_t hiragana_vlw_start[] asm("_binary_data_hiragana_vlw_start");
#else
Adafruit_SSD1306 display(128, 64);
U8G2_FOR_ADAFRUIT_GFX u8g2_for_adafruit_gfx;
#endif

void gfx_init() {
#ifdef CARDPUTER
    M5Cardputer.Display.setTextSize(M5Cardputer.Display.height() / 60);
#else
    display.begin(SSD1306_SWITCHCAPVCC, 0x3c);
    u8g2_for_adafruit_gfx.begin(display);

    u8g2_for_adafruit_gfx.setFont(u8g2_font_b16_t_japanese1);
#endif
}
void gfx_set_jp(bool jp) {
#ifdef CARDPUTER
    if(jp) M5Cardputer.Display.loadFont(hiragana_vlw_start);
    else M5Cardputer.Display. // TODO
#else
    if(jp) u8g2_for_adafruit_gfx.setFont(u8g2_font_b16_t_japanese1);
    else u8g2_for_adafruit_gfx.setFont(u8g2_font_9x15_tf);
#endif
}
void gfx_set_inv(bool inv) {
#ifdef CARDPUTER
    // TODO
#else
    if(inv) {
        u8g2_for_adafruit_gfx.setBackgroundColor(0xffff);
        u8g2_for_adafruit_gfx.setForegroundColor(0x0000);
    } else {
        u8g2_for_adafruit_gfx.setBackgroundColor(0xffff);
        u8g2_for_adafruit_gfx.setForegroundColor(0x0000);
    }
#endif
}
void gfx_print(int x, int y, String text) {
#ifdef CARDPUTER
    M5Cardputer.Display.drawString(t + "                     ", x, y);
#else
    u8g2_for_adafruit_gfx.setCursor(x, y);
    u8g2_for_adafruit_gfx.print(text + "                     ");
#endif
}
void gfx_start_render() {
#ifndef CARDPUTER
    display.clearDisplay();
#endif
}
void gfx_end_render() {
#ifndef CARDPUTER
    display.display();
#endif
}