#include "Sound.hpp"
#include <cstring>
#include <iostream>
#include <cassert>
using namespace std;
using namespace irrklang;

// open and load file
Audio::Audio() {
    dev = alcOpenDevice( nullptr );
    if(!dev) throw "Can't open device";

    ctx = alcCreateContext( dev, nullptr );
    if ( !alcMakeContextCurrent( ctx ) ) throw "Can't open ctx";

    ALfloat ori[] = { 0, 0, 1, 0, 1, 0 };
    alListener3f(AL_POSITION, 0, 0, 0);
    alListener3f(AL_VELOCITY, 0, 0, 0);
    alListenerfv(AL_ORIENTATION, ori);

    alGenSources((ALuint)1, &walk_source);
    alGenBuffers((ALuint)1, &walk_buffer);
    alSourcei(walk_source, AL_SOURCE_RELATIVE, AL_TRUE);
    alSourcef(walk_source, AL_PITCH, 1);
    alSourcef(walk_source, AL_GAIN, 0.5f);
    alSource3f(walk_source, AL_POSITION, 0.0f, 0.0f, 0.0f);
    alSource3f(walk_source, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
    alSourcei(walk_source, AL_LOOPING, AL_FALSE);

    loadWav("Assets/walk.wav", walk_source, walk_buffer);

    alGenSources((ALuint)1, &jump_source);
    alGenBuffers((ALuint)1, &jump_buffer);
    alSourcei(jump_source, AL_SOURCE_RELATIVE, AL_TRUE);
    alSourcef(jump_source, AL_PITCH, 1);
    alSourcef(jump_source, AL_GAIN, 0.5f);
    alSource3f(jump_source, AL_POSITION, 0.0f, 0.0f, 0.0f);
    alSource3f(jump_source, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
    alSourcei(jump_source, AL_LOOPING, AL_FALSE);

    loadWav("Assets/jump.wav", jump_source, jump_buffer);

    alGenSources((ALuint)1, &land_source);
    alGenBuffers((ALuint)1, &land_buffer);
    alSourcei(land_source, AL_SOURCE_RELATIVE, AL_TRUE);
    alSourcef(land_source, AL_PITCH, 1);
    alSourcef(land_source, AL_GAIN, 0.5f);
    alSource3f(land_source, AL_POSITION, 0.0f, 0.0f, 0.0f);
    alSource3f(land_source, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
    alSourcei(land_source, AL_LOOPING, AL_FALSE);

    loadWav("Assets/land.wav", land_source, land_buffer);

    alGenSources((ALuint)1, &death_source);
    alGenBuffers((ALuint)1, &death_buffer);
    alSourcei(death_source, AL_SOURCE_RELATIVE, AL_TRUE);
    alSourcef(death_source, AL_PITCH, 1);
    alSourcef(death_source, AL_GAIN, 0.5f);
    alSource3f(death_source, AL_POSITION, 0.0f, 0.0f, 0.0f);
    alSource3f(death_source, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
    alSourcei(death_source, AL_LOOPING, AL_FALSE);

    loadWav("Assets/death.wav", death_source, death_buffer);

    soundEngine = createIrrKlangDevice();
    soundEngine->play2D("Assets/background.wav");

}

void Audio::loadWav(const char *path, ALuint source, ALuint buffer) {
    FILE *fp = NULL;
    fp = fopen(path, "rb");

    char s[5]; s[4] = '\0';
    uint32_t size;
    short fm, channels, bytesPS, bitsPS;
    ALuint sample_rate, avgBPS, buffsize;

    fread(s, sizeof(char), 4, fp);
    assert( !strcmp( s, "RIFF") && string("No RIFF, file" + string(path)).c_str() );

    fread(&size, sizeof(uint32_t), 1, fp);
    fread(s, sizeof(char), 4, fp);
    assert( !strcmp( s, "WAVE") && string("No WAVE, file" + string(path)).c_str() );

    fread(s, sizeof(char), 4, fp);
    assert( !strcmp( s, "fmt ") && string("No 'fmt ', file" + string(path)).c_str() );

    fread(&size,           sizeof(uint32_t), 1, fp);
    fread(&fm,             sizeof(short), 1, fp);
    fread(&channels,       sizeof(short), 1, fp);
    fread(&sample_rate,    sizeof(uint32_t), 1, fp);
    fread(&avgBPS, sizeof(uint32_t), 1, fp);
    fread(&bytesPS, sizeof(short), 1, fp);
    fread(&bitsPS,  sizeof(short), 1, fp);

    fread(&s, sizeof(char), 4, fp);
    assert( !strcmp( s, "data") && string("No data, file" + string(path)).c_str() );

    fread(&buffsize, sizeof(uint32_t), 1, fp);

    unsigned char buf[buffsize];
    fread(buf, sizeof(int8_t), buffsize, fp);

    ALenum format=0;
    if (channels == 1) format = bitsPS == 8 ? AL_FORMAT_MONO8 : AL_FORMAT_MONO16;
    else if (channels == 2) format = bitsPS == 8 ? AL_FORMAT_STEREO8 : AL_FORMAT_STEREO16;

    alBufferData(buffer, format, buf, buffsize, sample_rate);
    alSourcei(source, AL_BUFFER, buffer);
    fclose(fp);
}

void Audio::walk() {
    int state; alGetSourcei( walk_source, AL_SOURCE_STATE, &state);
    if( state != AL_PLAYING ) alSourcePlay( walk_source );
}

void Audio::jump() {
    int state; alGetSourcei( jump_source, AL_SOURCE_STATE, &state);
    alSourcePlay( jump_source );
}

void Audio::land() {
    int state; alGetSourcei( land_source, AL_SOURCE_STATE, &state);
    if( state != AL_PLAYING ) alSourcePlay( land_source );
}

void Audio::death() {
    int state; alGetSourcei( death_source, AL_SOURCE_STATE, &state);
    if( state != AL_PLAYING ) alSourcePlay( death_source );
}
