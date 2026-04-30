#include "tts.h"
#include <BackgroundAudioSpeech.h>
#include <ESP32I2SAudio.h>
#include <libespeak-ng/voice/ja.h>
#include <M5Cardputer.h>

static ESP32I2SAudio* audio;
static BackgroundAudioSpeech* BMP;
static bool loaded = false;
static unsigned char cvoice[512];
void tts_init() {
    audio = new ESP32I2SAudio(41, 43, 42);
    BMP = new BackgroundAudioSpeech(*audio);
}
void tts_load_voice(int pitch) {
    memset(cvoice, 0, 512);
    memcpy(cvoice, voice_ja.data, voice_ja.len);
    strcat((char*)cvoice, (String("pitch ") + pitch + " " + pitch + "\n").c_str());
    strcat((char*)cvoice, "speed 1\n");
    BackgroundAudioVoice vdata = {
        .name = voice_ja.name,
        .len = strlen((char*)cvoice),
        .data = cvoice
    };
    BMP->setVoice(vdata);
    if(!loaded) {
        BMP->begin();
        BMP->setWordGap(0);
        BMP->setGain(0.5);
        loaded = true;
    }
}
void tts_set_pitch(int pitch) {
    BMP->setPitch(pitch);
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