#include <Arduino.h>
#include <M5Cardputer.h>
#include "tts.h"
#include "pitch.h"
#include "cvt.h"
#include <LittleFS.h>
#include <vector>

extern const uint8_t hiragana_vlw_start[] asm("_binary_data_hiragana_vlw_start");

static unsigned i = 0;
static std::vector<String> g_song = {
    "き", "ら", "き", "ら", "ひ", "か", "る",
    "お", "そ", "ら", "の", "ほ", "し", "よ",
    "ま", "ば", "た", "き", "し", "て", "は",
    "み", "ん", "な", "を", "み", "て", "る",
    "き", "ら", "き", "ら", "ひ", "か", "る",
    "お", "そ", "ら", "の", "ほ", "し", "よ",
};

void setup() {
    Serial.begin(115200);
    auto cfg = M5.config();
    M5Cardputer.begin(cfg);
    M5Cardputer.Display.setTextSize(M5Cardputer.Display.height() / 60);
    M5Cardputer.Display.loadFont(hiragana_vlw_start);
    M5Cardputer.Speaker.begin();
    // M5Cardputer.Display.println("ws");
    // M5Cardputer.Display.println(M5Cardputer.Speaker.config().pin_ws);
    // M5Cardputer.Display.println("data_out");
    // M5Cardputer.Display.println(M5Cardputer.Speaker.config().pin_data_out);
    // M5Cardputer.Display.println("bck");
    // M5Cardputer.Display.println(M5Cardputer.Speaker.config().pin_bck);
    // M5Cardputer.Display.println("mck");
    // M5Cardputer.Display.println(M5Cardputer.Speaker.config().pin_mck);
    tts_init();
    tts_load_voice("", 440);
    LittleFS.begin(true);
    if(!LittleFS.exists("/songs")) LittleFS.mkdir("/songs");
    if(!LittleFS.exists("/vbs")) LittleFS.mkdir("/vbs");

    Serial.print("> ");
    Serial.flush();
}
static bool playing = false;
static void next_syl(float pitch) {
    tts_set_pitch(pitch_calc(440, pitch));
    String cvtd = cvt_syl(g_song[i++ % g_song.size()]);
    tts_play(cvtd.c_str());
    playing = true;
}
#define KEY(c, p) if(!playing && M5Cardputer.Keyboard.isKeyPressed(c)) next_syl(p);
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
            } else {
                Serial.println("Invalid command or arguments");
            }
            inp = "";
            Serial.print("> ");
            Serial.flush();
        } else inp += c;
    }

    // Update state
    M5Cardputer.Keyboard.updateKeyList();
    M5Cardputer.Keyboard.updateKeysState();
    uint64_t now = millis();
    if(now - last_screen_upd > 100) {
        String t = "";
        for(int I = i % g_song.size(); I < i % g_song.size() + 7 && I < g_song.size(); I++)
            t += g_song[I];
        M5Cardputer.Display.drawString(t + "                     ", 2, 2);
        last_screen_upd = now;
    }
    KEY('a', 262)
    else KEY('w', 277)
    else KEY('s', 294)
    else KEY('e', 311)
    else KEY('d', 330)
    else KEY('r', 349)
    else KEY('f', 370)
    else KEY('g', 392)
    else KEY('y', 415)
    else KEY('h', 440)
    else KEY('u', 466)
    else KEY('j', 494)
    else KEY('k', 523)
    else if(playing && !M5Cardputer.Keyboard.isPressed()) { tts_stop(); playing = false; }
}