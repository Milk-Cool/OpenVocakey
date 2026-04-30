#pragma once

void tts_init();
// TODO: more params
void tts_load_voice(int pitch);
// pitch: 0 = 50% of pitch in load_voice, 99 = 198%, 50 = 100%
void tts_set_pitch(int pitch);
bool tts_done();
void tts_play(const char* text);
void tts_stop();