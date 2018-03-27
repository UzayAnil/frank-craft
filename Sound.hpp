#pragma once

#include <AL/al.h>
#include <AL/alc.h>

#include <irrKlang.h>

class Audio {
public:
    Audio();
    void loadWav(const char *soundPath, ALuint source, ALuint buffer);
    void walk();
    void jump();
    void land();
private:
    ALCdevice *dev;
    ALCcontext *ctx;

    ALuint walk_source;
    ALuint walk_buffer;

    ALuint jump_source;
    ALuint jump_buffer;

    ALuint land_source;
    ALuint land_buffer;

    irrklang::ISoundEngine* soundEngine;

};
