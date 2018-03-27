#pragma once
#include <string>
#include "cs488-framework/OpenGLImport.hpp"

class Texture {
public:
    enum Type { TEX2D, TEXCUBE, TEXDEPTH };
public:
    Texture( std::string img_file );
    Texture( std::string img_file[6] );
    Texture( int width, int height, Texture::Type type );
    ~Texture();
    bool bind( GLint uniform );

public:
    Type type;
    int width;
    int height;

    GLuint tex;
    int texCounter;

private:
    static unsigned int counter;
};
