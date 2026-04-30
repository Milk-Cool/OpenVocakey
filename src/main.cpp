#include <Arduino.h>
#include <M5Cardputer.h>
#include "tts.h"

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
void loop() {
    tts_set_pitch(50);
    tts_play("[[kaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa]]");
    delay(1500);
    tts_set_pitch(99);
    tts_play("[[kaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa]]");
    delay(1500);
    tts_set_pitch(50);
    tts_play("[[kaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa]]");
    delay(1500);
    tts_set_pitch(0);
    tts_play("[[kaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa]]");
    delay(1500);
}