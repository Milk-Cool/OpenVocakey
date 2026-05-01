Import("env")

from os.path import join, exists

PATH = join(".pio", "libdeps", "esp32dev", "BackgroundAudio", "src", "BackgroundAudioSpeech.h")

def patch():
    if not exists(PATH):
        return
    f = open(PATH, "r")
    d = f.read()
    f.close()
    f = open(PATH, "w")
    f.write(d.replace(" _out->setBuffers(5, framelen);", " _out->setBuffers(1, framelen);"))
    f.close()

patch()