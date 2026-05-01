#include "pitch.h"
#include <math.h>

int pitch_calc(float pitch_base, float pitch_target) {
    return log2(pitch_target / pitch_base) * 50 + 50;
}