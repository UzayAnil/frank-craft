#pragma once
#include <string>
#include "cs488-framework/OpenGLImport.hpp"

class Texture {
public:
    enum Type { a };
public:
    Texture( std::string img_file );
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
