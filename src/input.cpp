#include "input.h"
#include <M5Cardputer.h>

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
    M5Cardputer.Keyboard.updateKeyList();
    M5Cardputer.Keyboard.updateKeysState();
}
#define KEY(k, o) if(key == o) return M5Cardputer.Keyboard.isKeyPressed(k);
bool input_pressed(SynthKey key) {
    KEY('a', KEY_C4)
    KEY('w', KEY_CS4)
    KEY('s', KEY_D4)
    KEY('e', KEY_DS4)
    KEY('d', KEY_E4)
    KEY('f', KEY_F4)
    KEY('t', KEY_FS4)
    KEY('g', KEY_G4)
    KEY('y', KEY_GS4)
    KEY('h', KEY_A4)
    KEY('u', KEY_AS4)
    KEY('j', KEY_B4)
    KEY('k', KEY_C5)
    return false;
}