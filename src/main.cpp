#include <Arduino.h>
#ifdef CARDPUTER
#include <M5Cardputer.h>
#else
#include <Adafruit_SSD1306.h>
#include <U8g2_for_Adafruit_GFX.h>
// #include "font.h"
#endif
#include "tts.h"
#include "pitch.h"
#include "cvt.h"
#include <LittleFS.h>
#include <vector>
#include "input.h"

#ifdef CARDPUTER
extern const uint8_t hiragana_vlw_start[] asm("_binary_data_hiragana_vlw_start");
#endif

static unsigned i = 0;
static std::vector<String> g_song = {
    "き", "ら", "き", "ら", "ひ", "か", "る",
    "お", "そ", "ら", "の", "ほ", "し", "よ",
    "ま", "ば", "た", "き", "し", "て", "は",
    "み", "ん", "な", "を", "み", "て", "る",
    "き", "ら", "き", "ら", "ひ", "か", "る",
    "お", "そ", "ら", "の", "ほ", "し", "よ",
};

#ifndef CARDPUTER
Adafruit_SSD1306 display(128, 64);
U8G2_FOR_ADAFRUIT_GFX u8g2_for_adafruit_gfx;
#endif

void setup() {
    Serial.begin(115200);
#ifdef CARDPUTER
    auto cfg = M5.config();
    M5Cardputer.begin(cfg);
    M5Cardputer.Display.setTextSize(M5Cardputer.Display.height() / 60);
    M5Cardputer.Display.loadFont(hiragana_vlw_start);
    M5Cardputer.Speaker.begin();
#else
    display.begin(SSD1306_SWITCHCAPVCC, 0x3c);
    u8g2_for_adafruit_gfx.begin(display);

    u8g2_for_adafruit_gfx.setFont(u8g2_font_b16_t_japanese1);
#endif
    // M5Cardputer.Display.println("ws");
    // M5Cardputer.Display.println(M5Cardputer.Speaker.config().pin_ws);
    // M5Cardputer.Display.println("data_out");
    // M5Cardputer.Display.println(M5Cardputer.Speaker.config().pin_data_out);
    // M5Cardputer.Display.println("bck");
    // M5Cardputer.Display.println(M5Cardputer.Speaker.config().pin_bck);
    // M5Cardputer.Display.println("mck");
    // M5Cardputer.Display.println(M5Cardputer.Speaker.config().pin_mck);
    input_init();
    tts_init();
    tts_load_voice("", 440);
    tts_set_wpm(200);
    LittleFS.begin(true);
    if(!LittleFS.exists("/songs")) LittleFS.mkdir("/songs");
    if(!LittleFS.exists("/vbs")) LittleFS.mkdir("/vbs");

    Serial.print("> ");
    Serial.flush();

    pinMode(0, INPUT_PULLUP);
}
static SynthKey pressed = KEY_NONE;
static void next_syl(float pitch) {
    tts_set_pitch(pitch_calc(440, pitch));
    String cvtd = cvt_syl(g_song[i++ % g_song.size()]);
    tts_play(cvtd.c_str());
}
uint64_t last_screen_upd = 0;
static String inp = "";
static std::vector<String> parse_args(String& raw) {
    std::vector<String> out;
    String cur;
    for(char c : raw) {
        if(c == ' ') {
            if(cur != "") out.push_back(cur);
            cur = "";
        } else cur += c;
    }
    if(cur != "") out.push_back(cur);
    return out;
}
static void print_and_close_dir(File& f) {
    while(true) {
        String fname = f.getNextFileName();
        if(fname == "") break;
        Serial.println(fname);
    }
    f.close();
}
static void write_from_serial_and_close_file(File& f) {
    String in = "";
    // blocking
    while(true) {
        if(!Serial.available()) continue;
        char c = Serial.read();
        Serial.print(c);
        if(c == '\r') continue;
        in += c;
        if(in.endsWith("EOF\n")) break;
    }
    in = in.substring(0, in.length() - 4);
    f.write((uint8_t*)in.c_str(), in.length());
    f.close();
}
void loop() {
    // CLI
    while(Serial.available()) {
        char c = Serial.read();
        Serial.print(c);
        Serial.flush();
        if(c == '\r') continue;
        if(c == '\n') {
            auto vec = parse_args(inp);
            if(vec.size() == 0) {}
            else if(vec[0] == "help") {
                Serial.println("help - print this message");
                Serial.println("v_list - list installed voicebanks");
                Serial.println("v_get <voice> - get voicebank by filename");
                Serial.println("v_set <voice> - save voicebank until \"EOF\"");
                Serial.println("v_sel <voice> - select voicebank to use");
                Serial.println("s_list - list saved songs");
                Serial.println("s_get <song> - get song by filename");
                Serial.println("s_set <song> - save song until \"EOF\"");
                Serial.println("s_sel <song> - select song to sing");
                Serial.println("wpm <wpm> - set the wpm for the espeak engine, useful for shortening consonants");
                Serial.println("stretch <cnt_vowels> - set the vowel stretch, high values may produce lag");
            } else if(vec[0] == "v_list") {
                File vbs = LittleFS.open("/vbs");
                print_and_close_dir(vbs);
            } else if(vec[0] == "s_list") {
                File songs = LittleFS.open("/songs");
                print_and_close_dir(songs);
            } else if(vec[0] == "v_get" && vec.size() >= 2) {
                File vb = LittleFS.open("/vbs/" + vec[1]);
                Serial.println(vb.readString());
                vb.close();
            } else if(vec[0] == "s_get" && vec.size() >= 2) {
                File song = LittleFS.open("/songs/" + vec[1]);
                Serial.println(song.readString());
                song.close();
            } else if(vec[0] == "v_set" && vec.size() >= 2) {
                File vb = LittleFS.open("/vbs/" + vec[1], "w");
                write_from_serial_and_close_file(vb);
            } else if(vec[0] == "s_set" && vec.size() >= 2) {
                File song = LittleFS.open("/songs/" + vec[1], "w");
                write_from_serial_and_close_file(song);
            } else if(vec[0] == "v_sel" && vec.size() >= 2) {
                File vb = LittleFS.open("/vbs/" + vec[1]);
                tts_load_voice(vb.readString(), 440);
                vb.close();
            } else if(vec[0] == "s_sel" && vec.size() >= 2) {
                File song = LittleFS.open("/songs/" + vec[1]);
                String cur = "";
                i = 0;
                g_song.clear();
                for(char c : song.readString()) {
                    if(c == '\r') continue;
                    if(c == '\n') {
                        if(cur != "") g_song.push_back(cur);
                        cur = "";
                    } else cur += c;
                }
                if(cur != "") g_song.push_back(cur);
                song.close();
            } else if(vec[0] == "wpm" && vec.size() >= 2) {
                tts_set_wpm(vec[1].toInt());
            } else if(vec[0] == "stretch" && vec.size() >= 2) {
                set_stretch(vec[1].toInt());
            } else {
                Serial.println("Invalid command or arguments");
            }
            inp = "";
            Serial.print("> ");
            Serial.flush();
        } else inp += c;
    }

    if(!digitalRead(0)) i = 0;

    // Update state
    input_upd();
    uint64_t now = millis();
    if(now - last_screen_upd > 100) {
        String t = "";
        for(int I = i % g_song.size(); I < i % g_song.size() + 7 && I < g_song.size(); I++)
            t += g_song[I];
#ifdef CARDPUTER
        M5Cardputer.Display.drawString(t + "                     ", 2, 2);
#else
        display.clearDisplay();
        u8g2_for_adafruit_gfx.setCursor(2, 32);
        u8g2_for_adafruit_gfx.print(t + "                     ");
        display.display();
#endif
        last_screen_upd = now;
    }
    SynthKey k;
    int j = 0;
    if(pressed != KEY_NONE && !input_pressed(pressed)) {
        tts_stop();
        pressed = KEY_NONE;
    }
    if(pressed == KEY_NONE) while((k = all_keys[j++]) != KEY_NONE) if(input_pressed(k)) {
        pressed = k;
        next_syl(get_freq(k));
    }
}