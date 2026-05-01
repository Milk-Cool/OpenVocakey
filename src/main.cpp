#include <Arduino.h>
#include <M5Cardputer.h>
#include "tts.h"
#include "pitch.h"
#include "cvt.h"

extern const uint8_t hiragana_vlw_start[] asm("_binary_data_hiragana_vlw_start");

static unsigned i = 0;
static String song[] = {
    "き", "ら", "き", "ら", "ひ", "か", "る",
    "お", "そ", "ら", "の", "ほ", "し", "よ",
    "ま", "ば", "た", "き", "し", "て", "は",
    "み", "ん", "な", "を", "み", "て", "る",
    "き", "ら", "き", "ら", "ひ", "か", "る",
    "お", "そ", "ら", "の", "ほ", "し", "よ",
};

void setup() {
    auto cfg = M5.config();
    M5Cardputer.begin(cfg);
    Serial.begin(115200);
    M5Cardputer.Display.setTextSize(M5Cardputer.Display.height() / 60);
    M5Cardputer.Display.loadFont(hiragana_vlw_start);
    M5Cardputer.Speaker.begin();
    // M5Cardputer.Display.println("ws");
    // M5Cardputer.Display.println(M5Cardputer.Speaker.config().pin_ws);
    // M5Cardputer.Display.println("data_out");
    // M5Cardputer.Display.println(M5Cardputer.Speaker.config().pin_data_out);
    // M5Cardputer.Display.println("bck");
    // M5Cardputer.Display.println(M5Cardputer.Speaker.config().pin_bck);
    // M5Cardputer.Display.println("mck");
    // M5Cardputer.Display.println(M5Cardputer.Speaker.config().pin_mck);
    tts_init();
    tts_load_voice(440);
}
static bool playing = false;
static void next_syl(float pitch) {
    tts_set_pitch(pitch_calc(440, pitch));
    String cvtd = cvt_syl(song[i++ % (sizeof(song) / sizeof(song[0]))]);
    tts_play(cvtd.c_str());
    Serial.println(cvtd);
    playing = true;
}
#define KEY(c, p) if(!playing && M5Cardputer.Keyboard.isKeyPressed(c)) next_syl(p);
uint64_t last_screen_upd = 0;
void loop() {
    M5Cardputer.Keyboard.updateKeyList();
    M5Cardputer.Keyboard.updateKeysState();
    uint64_t now = millis();
    if(now - last_screen_upd > 100) {
        String t = "";
        for(int I = i % (sizeof(song) / sizeof(song[0])); I < i % (sizeof(song) / sizeof(song[0])) + 7 && I < (sizeof(song) / sizeof(song[0])); I++)
            t += song[I];
        M5Cardputer.Display.drawString(t + "                     ", 2, 2);
        last_screen_upd = now;
    }
    KEY('a', 262)
    else KEY('w', 277)
    else KEY('s', 294)
    else KEY('e', 311)
    else KEY('d', 330)
    else KEY('r', 349)
    else KEY('f', 370)
    else KEY('g', 392)
    else KEY('y', 415)
    else KEY('h', 440)
    else KEY('u', 466)
    else KEY('j', 494)
    else KEY('k', 523)
    else if(playing && !M5Cardputer.Keyboard.isPressed()) { tts_stop(); playing = false; }
}