Import("env")

from os.path import join, exists

PATH_A = join(".pio", "libdeps", "esp32dev", "BackgroundAudio", "src", "BackgroundAudioSpeech.h")
PATH_B = join(".pio", "libdeps", "esp32dev", "BackgroundAudio", "src", "libespeak-ng", "synthesize.h")

def patch():
    if not exists(PATH_A) or not exists(PATH_B):
        return
    f = open(PATH_A, "r")
    d = f.read()
    f.close()
    f = open(PATH_A, "w")
    f.write(d.replace(" _out->setBuffers(5, framelen);", " _out->setBuffers(1, framelen);"))
    f.close()

    f = open(PATH_B, "r")
    d = f.read()
    f.close()
    f = open(PATH_B, "w")
    f.write(d.replace("#define N_PHONEME_LIST 1000", "#define N_PHONEME_LIST 250"))
    f.close()

patch()