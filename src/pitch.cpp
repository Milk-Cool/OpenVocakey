#include "pitch.h"

int pitch_calc(float pitch_base, float pitch_target) {
    return pitch_base < pitch_target ? pitch_target / pitch_base * 49.5 : (pitch_target - pitch_base / 2) / (pitch_base / 2) * 50; // i *think* this should work?
}