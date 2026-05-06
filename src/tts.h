#pragma once
#include <Arduino.h>

void tts_init();
void tts_load_voice(String concat, int pitch);
// pitch: 0 = 50% of pitch in load_voice, 99 = 198%, 50 = 100%
void tts_set_pitch(int pitch);
void tts_set_wpm(int wpm);
int tts_get_wpm();
void tts_set_vol(float vol);
float tts_get_vol();
bool tts_done();
void tts_play(const char* text);
void tts_stop();