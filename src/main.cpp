#include <Arduino.h>
#include <M5Cardputer.h>
#include "tts.h"
#include "pitch.h"

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
    tts_set_pitch(pitch_calc(440, 262));
    tts_play("haaaaaaa");
    delay(600);
    tts_set_pitch(pitch_calc(440, 262));
    tts_play("piiiiiii");
    delay(300);
    tts_set_pitch(pitch_calc(440, 294));
    tts_play("boooooo");
    delay(600);
    tts_set_pitch(pitch_calc(440, 262));
    tts_play("'rdaaaaaaaa");
    delay(600);
    tts_set_pitch(pitch_calc(440, 349));
    tts_play("ytuuuuuuu");
    delay(600);
    tts_set_pitch(pitch_calc(440, 330));
    tts_play("yuuuuuuu");
    delay(1200);

    tts_set_pitch(pitch_calc(440, 262));
    tts_play("haaaaaaa");
    delay(600);
    tts_set_pitch(pitch_calc(440, 262));
    tts_play("piiiiiii");
    delay(300);
    tts_set_pitch(pitch_calc(440, 294));
    tts_play("boooooo");
    delay(600);
    tts_set_pitch(pitch_calc(440, 262));
    tts_play("'rdaaaaaaaa");
    delay(600);
    tts_set_pitch(pitch_calc(440, 392));
    tts_play("ytuuuuuuu");
    delay(600);
    tts_set_pitch(pitch_calc(440, 349));
    tts_play("yuuuuuuu");
    delay(1200);
}