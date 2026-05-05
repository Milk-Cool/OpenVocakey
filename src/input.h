#pragma once

typedef enum {
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
    KEY_NONE = 0xffffffff
} SynthKey;

void input_init();
float get_freq(SynthKey key);
void input_upd();
bool input_pressed(SynthKey key);

extern SynthKey all_keys[];