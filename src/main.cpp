#include <Arduino.h>
#include <M5Cardputer.h>
#include "tts.h"
#include "pitch.h"
#include "cvt.h"

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
    M5Cardputer.Speaker.begin();
    // M5Cardputer.Display.println("ws");
    // M5Cardputer.Display.println(M5Cardputer.Speaker.config().pin_ws);
    // M5Cardputer.Display.println("data_out");
    // M5Cardputer.Display.println(M5Cardputer.Speaker.config().pin_data_out);
    // M5Cardputer.Display.println("bck");
    // M5Cardputer.Display.println(M5Cardputer.Speaker.config().pin_bck);
    // M5Cardputer.Display.println("mck");
    // M5Cardputer.Display.println(M5Cardputer.Speaker.config().pin_mck);
    M5Cardputer.Display.println("0");
    tts_init();
    M5Cardputer.Display.println("1");
    tts_load_voice(440);
    M5Cardputer.Display.println("2");
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
void loop() {
    M5Cardputer.Keyboard.updateKeyList();
    M5Cardputer.Keyboard.updateKeysState();
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