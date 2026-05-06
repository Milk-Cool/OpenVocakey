#include <Arduino.h>
#ifdef CARDPUTER
#include <M5Cardputer.h>
#else
// #include "font.h"
#endif
#include "tts.h"
#include "pitch.h"
#include "cvt.h"
#include <LittleFS.h>
#include <vector>
#include "input.h"
#include "gfx.h"

static unsigned i = 0;
static std::vector<String> g_song = {
    "き", "ら", "き", "ら", "ひ", "か", "る",
    "お", "そ", "ら", "の", "ほ", "し", "よ",
    "ま", "ば", "た", "き", "し", "て", "は",
    "み", "ん", "な", "を", "み", "て", "る",
    "き", "ら", "き", "ら", "ひ", "か", "る",
    "お", "そ", "ら", "の", "ほ", "し", "よ",
};

static uint8_t menu_mode = 0;
static uint8_t sel_option = 0;
static uint8_t config_option = 0;
static uint32_t file_idx = 0;
static int32_t num_val = 0;
#define CFG_SONG 1
#define CFG_VB 2
#define CFG_WPM 3
#define CFG_STRETCH 4
#define CFG_VOLUME 5
#define CFG_TRANSPOSE 6
#define CFG_MAX CFG_TRANSPOSE

void setup() {
    Serial.begin(115200);
#ifdef CARDPUTER
    auto cfg = M5.config();
    M5Cardputer.begin(cfg);
    M5Cardputer.Speaker.begin();
#endif
    gfx_init();
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
static void load_song(String str) {
    String cur = "";
    i = 0;
    g_song.clear();
    for(char c : str) {
        if(c == '\r') continue;
        if(c == '\n') {
            if(cur != "") g_song.push_back(cur);
            cur = "";
        } else cur += c;
    }
    if(cur != "") g_song.push_back(cur);
}
static bool boot_pressed = false;
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
                load_song(song.readString());
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

    if(!digitalRead(0) && !boot_pressed) {
        if(menu_mode == 0) {
            menu_mode = 1;
            sel_option = 0;
        } else menu_mode = 0;
        boot_pressed = true;
    } else if(digitalRead(0)) boot_pressed = false;

    // Update state (STATE 0)
    if(menu_mode == 0) {
        input_upd();
        uint64_t now = millis();
        if(now - last_screen_upd > 100) {
            gfx_start_render();
            String t = "";
            for(int I = i % g_song.size(); I < i % g_song.size() + 7 && I < g_song.size(); I++)
                t += g_song[I];
            gfx_set_jp(true);
#ifdef CARDPUTER
            gfx_print(2, 2, t + "                     ");
#else
            gfx_print(2, 32, t + "                     ");
#endif
            last_screen_upd = now;
            gfx_end_render();
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
    // Update state (STATE 1)
    } else if(menu_mode == 1) {
        input_upd();
        uint64_t now = millis();
        if(now - last_screen_upd > 100) {
            gfx_start_render();
            String opt = sel_option == 0
                ? "restart song"
                : sel_option == 1
                ? "change song"
                : sel_option == 2
                ? "change voice"
                : sel_option == 3
                ? "change speed"
                : sel_option == 4
                ? "change stretch"
                : sel_option == 5
                ? "change volume"
                : "transpose";
            gfx_set_jp(false);
#ifdef CARDPUTER
            gfx_print(2, 2, opt);
#else
            gfx_print(2, 16, opt);
#endif
            last_screen_upd = now;
            gfx_end_render();
        }
        if(pressed != KEY_NONE && !input_pressed(pressed)) {
            pressed = KEY_NONE;
        } else if(pressed == KEY_NONE) {
            if(input_pressed(KEY_C4)) {
                pressed = KEY_C4;
                if(sel_option == 0) sel_option = CFG_MAX;
                else sel_option--;
            } else if(input_pressed(KEY_D4)) {
                pressed = KEY_D4;
                if(sel_option == CFG_MAX) sel_option = 0;
                else sel_option++;
            } else if(input_pressed(KEY_B4)) {
                pressed = KEY_B4;
                menu_mode = 0;
            } else if(input_pressed(KEY_C5)) {
                pressed = KEY_C5;
                if(sel_option == 0) {
                    menu_mode = 0;
                    i = 0;
                } else {
                    menu_mode = 2;
                    config_option = sel_option;
                    file_idx = 0;
                    if(config_option == CFG_WPM) num_val = tts_get_wpm();
                    else if(config_option == CFG_STRETCH) num_val = get_stretch();
                    else if(config_option == CFG_VOLUME) num_val = tts_get_vol() * 100;
                    else if(config_option == CFG_TRANSPOSE) num_val = get_transpose();
                }
            }
        }
    // Update state (STATE 2, FILE SELECT)
    } else if(menu_mode == 2 && (config_option == CFG_SONG || config_option == CFG_VB)) {
        input_upd();
        uint64_t now = millis();
        if(now - last_screen_upd > 100) {
            gfx_start_render();
            String p = config_option == CFG_SONG ? "/songs" : "/vbs";
            File dir = LittleFS.open(p);
            String f = dir.getNextFileName();
            for(uint32_t i = 0; i < file_idx; i++) f = dir.getNextFileName();
            if(f == "") {
                file_idx = 0;
                dir.close();
                dir = LittleFS.open(p);
                f = dir.getNextFileName();
                if(f == "") {
                    menu_mode = 0;
                    dir.close();
                    return;
                }
            }
            gfx_set_jp(false);
#ifdef CARDPUTER
            gfx_print(2, 2, config_option == CFG_SONG ? f.substring(7) : f.substring(5));
#else
            gfx_print(2, 16, config_option == CFG_SONG ? f.substring(7) : f.substring(5));
#endif
            gfx_end_render();
            last_screen_upd = now;
            dir.close();
        }
        if(pressed != KEY_NONE && !input_pressed(pressed)) {
            pressed = KEY_NONE;
        } else if(pressed == KEY_NONE) {
            if(input_pressed(KEY_C4)) {
                pressed = KEY_C4;
                if(file_idx != 0) file_idx--;
            } else if(input_pressed(KEY_D4)) {
                pressed = KEY_D4;
                file_idx++;
            } else if(input_pressed(KEY_B4)) {
                pressed = KEY_B4;
                menu_mode = 1;
            } else if(input_pressed(KEY_C5)) {
                pressed = KEY_C5;
                String dname = config_option == CFG_SONG ? "/songs" : "/vbs";
                menu_mode = 0;
                File dir = LittleFS.open(dname);
                String f = dir.getNextFileName();
                for(uint32_t i = 0; i < file_idx; i++) f = dir.getNextFileName();
                dir.close();
                File f2 = LittleFS.open(f);
                if(config_option == CFG_SONG) load_song(f2.readString());
                else tts_load_voice(f2.readString(), 440);
                f2.close();
                if(config_option == CFG_SONG) i = 0;
            }
        }
    // Update state (STATE 2, NUMERIC SETTINGS)
    } else if(menu_mode == 2 && (config_option == CFG_WPM || config_option == CFG_STRETCH || config_option == CFG_VOLUME || config_option == CFG_TRANSPOSE)) {
        input_upd();
        uint64_t now = millis();
        if(now - last_screen_upd > 100) {
            gfx_start_render();
            gfx_set_jp(false);
#ifdef CARDPUTER
            gfx_print(2, 2, String(num_val));
#else
            gfx_print(2, 16, String(num_val));
#endif
            gfx_end_render();
            last_screen_upd = now;
        }
        if(pressed != KEY_NONE && !input_pressed(pressed)) {
            pressed = KEY_NONE;
        } else if(pressed == KEY_NONE) {
            if(input_pressed(KEY_C4)) {
                pressed = KEY_C4;
                if(config_option == CFG_STRETCH && num_val > 0) num_val -= 5;
                else if(config_option == CFG_WPM && num_val > 10) num_val -= 10;
                else if(config_option == CFG_VOLUME && num_val > 0) num_val -= 10;
                else if(config_option == CFG_TRANSPOSE && num_val > -3) num_val--;
            } else if(input_pressed(KEY_D4)) {
                pressed = KEY_D4;
                if(config_option == CFG_STRETCH && num_val < 200) num_val += 5;
                else if(config_option == CFG_WPM && num_val < 500) num_val += 10;
                else if(config_option == CFG_VOLUME && num_val < 100) num_val += 10;
                else if(config_option == CFG_TRANSPOSE && num_val < 8) num_val++;
            } else if(input_pressed(KEY_B4)) {
                pressed = KEY_B4;
                menu_mode = 1;
            } else if(input_pressed(KEY_C5)) {
                pressed = KEY_C5;
                if(config_option == CFG_STRETCH) set_stretch(num_val);
                else if(config_option == CFG_WPM) tts_set_wpm(num_val);
                else if(config_option == CFG_VOLUME) tts_set_vol((float)num_val / 100);
                else if(config_option == CFG_TRANSPOSE) set_transpose(num_val);
                menu_mode = 0;
            }
        }
    }
}