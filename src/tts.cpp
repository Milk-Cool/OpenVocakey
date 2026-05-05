#include "tts.h"
#include <BackgroundAudioSpeech.h>
#include <ESP32I2SAudio.h>
#include <libespeak-ng/voice/ja.h>

static ESP32I2SAudio* audio;
static BackgroundAudioSpeech* BMP;
static bool loaded = false;
static unsigned char cvoice[1024];
static int last_wpm = 0;
static float last_vol = 1;
void tts_init() {
#ifdef CARDPUTER
    audio = new ESP32I2SAudio(41, 43, 42);
#else
    audio = new ESP32I2SAudio(16, 17, 4);
#endif
    BMP = new BackgroundAudioSpeech(*audio);
}
void tts_load_voice(String concat, int pitch) {
    memset(cvoice, 0, 1024);
    memcpy(cvoice, voice_ja.data, voice_ja.len);
    strcat((char*)cvoice, concat.c_str());
    strcat((char*)cvoice, (String("pitch ") + pitch + " " + pitch + "\n").c_str());
    // strcat((char*)cvoice, "speed 1\n");
    BackgroundAudioVoice vdata = {
        .name = voice_ja.name,
        .len = strlen((char*)cvoice),
        .data = cvoice
    };
    BMP->setVoice(vdata);
    if(loaded) BMP->end();
    BMP->begin();
    BMP->setWordGap(0);
    BMP->setGain(0.5);
    loaded = true;
}
void tts_set_pitch(int pitch) {
    BMP->setPitch(pitch);
}
void tts_set_wpm(int wpm) {
    BMP->setRate(wpm);
    last_wpm = wpm;
}
int tts_get_wpm() {
    return last_wpm;
}
void tts_set_vol(float vol) {
    BMP->setGain(vol);
    last_vol = vol;
}
float tts_get_vol() {
    return last_vol;
}
bool tts_done() {
    return BMP->done();
}
void tts_play(const char* text) {
    // M5Cardputer.Display.println(String("speaking: ") + text);
    tts_stop();
    BMP->speak(text);
}
void tts_stop() {
    BMP->flush();
}