#include "input.h"
#ifdef CARDPUTER
#include <M5Cardputer.h>
#else
#include <Arduino.h>
#include <map>
#endif

#define DEBOUNCE 10

SynthKey all_keys[] = {
    KEY_C4,
    KEY_CS4,
    KEY_D4,
    KEY_DS4,
    KEY_E4,
    KEY_F4,
    KEY_FS4,
    KEY_G4,
    KEY_GS4,
    KEY_A4,
    KEY_AS4,
    KEY_B4,
    KEY_C5,
    KEY_NONE
};
#ifndef CARDPUTER
typedef struct {
    uint64_t t;
    bool pressed;
} Debounce;
std::map<SynthKey, Debounce> debounce = {
    { KEY_C4, { 0, false } },
    { KEY_CS4, { 0, false } },
    { KEY_D4, { 0, false } },
    { KEY_DS4, { 0, false } },
    { KEY_E4, { 0, false } },
    { KEY_F4, { 0, false } },
    { KEY_FS4, { 0, false } },
    { KEY_G4, { 0, false } },
    { KEY_GS4, { 0, false } },
    { KEY_A4, { 0, false } },
    { KEY_AS4, { 0, false } },
    { KEY_B4, { 0, false } },
    { KEY_C5, { 0, false } },
};
#endif

void input_init() {
#ifndef CARDPUTER
    pinMode(15, INPUT_PULLUP);
    pinMode(36, INPUT);
    pinMode(39, INPUT);
    pinMode(34, INPUT);
    pinMode(35, INPUT);
    pinMode(32, INPUT_PULLUP);
    pinMode(33, INPUT_PULLUP);
    pinMode(25, INPUT_PULLUP);
    pinMode(26, INPUT_PULLUP);
    pinMode(27, INPUT_PULLUP);
    pinMode(14, INPUT_PULLUP);
    pinMode(12, INPUT_PULLUP);
    pinMode(13, INPUT_PULLUP);
#endif
}

float get_freq(SynthKey key) {
    if(key == KEY_C4) return 261.63;
    if(key == KEY_CS4) return 277.18;
    if(key == KEY_D4) return 293.67;
    if(key == KEY_DS4) return 311.13;
    if(key == KEY_E4) return 329.63;
    if(key == KEY_F4) return 349.23;
    if(key == KEY_FS4) return 369.99;
    if(key == KEY_G4) return 392.00;
    if(key == KEY_GS4) return 415.30;
    if(key == KEY_A4) return 440.00;
    if(key == KEY_AS4) return 466.16;
    if(key == KEY_B4) return 493.88;
    if(key == KEY_C5) return 523.25;
    return 440;
}
void input_upd() {
#ifdef CARDPUTER
    M5Cardputer.Keyboard.updateKeyList();
    M5Cardputer.Keyboard.updateKeysState();
#endif
}
#ifdef CARDPUTER
#define KEY1(k, o) if(key == o) return M5Cardputer.Keyboard.isKeyPressed(k);
bool input_pressed(SynthKey key) {
    KEY1('a', KEY_C4)
    KEY1('w', KEY_CS4)
    KEY1('s', KEY_D4)
    KEY1('e', KEY_DS4)
    KEY1('d', KEY_E4)
    KEY1('f', KEY_F4)
    KEY1('t', KEY_FS4)
    KEY1('g', KEY_G4)
    KEY1('y', KEY_GS4)
    KEY1('h', KEY_A4)
    KEY1('u', KEY_AS4)
    KEY1('j', KEY_B4)
    KEY1('k', KEY_C5)
    return false;
}
#else
#define KEY2(p, o) if(key == o) { if(now - debounce[o].t < DEBOUNCE) { return debounce[o].pressed; } debounce[o].t = now; int v = !digitalRead(p); debounce[o].pressed = v; return v; }
bool input_pressed(SynthKey key) {
    uint64_t now = millis();
    // Serial.printf("%d %hhu\n", 15, !digitalRead(15));
    // Serial.printf("%d %hu\n", 36, analogRead(36));
    // Serial.printf("%d %hu\n", 39, analogRead(39));
    // Serial.printf("%d %hu\n", 34, analogRead(34));
    // Serial.printf("%d %hu\n", 35, analogRead(35));
    // Serial.printf("%d %hhu\n", 32, !digitalRead(32));
    // Serial.printf("%d %hhu\n", 33, !digitalRead(33));
    // Serial.printf("%d %hhu\n", 25, !digitalRead(25));
    // Serial.printf("%d %hhu\n", 26, !digitalRead(26));
    // Serial.printf("%d %hhu\n", 27, !digitalRead(27));
    // Serial.printf("%d %hhu\n", 14, !digitalRead(14));
    // Serial.printf("%d %hhu\n", 12, !digitalRead(12));
    // Serial.printf("%d %hhu\n", 13, !digitalRead(13));
    KEY2(15, KEY_C4)
    KEY2(36, KEY_CS4)
    KEY2(39, KEY_D4)
    KEY2(34, KEY_DS4)
    KEY2(35, KEY_E4)
    KEY2(32, KEY_F4)
    KEY2(33, KEY_FS4)
    KEY2(25, KEY_G4)
    KEY2(26, KEY_GS4)
    KEY2(27, KEY_A4)
    KEY2(14, KEY_AS4)
    KEY2(12, KEY_B4)
    KEY2(13, KEY_C5)
    return false;
}
#endif