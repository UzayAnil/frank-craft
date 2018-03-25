#include "Texture.hpp"
#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/GlErrorCheck.hpp"
#include <iostream>
#include <lodepng/lodepng.h>
using namespace std;

unsigned int Texture::counter = 0;

Texture::Texture( string img_file ) {
    {
        std::vector<unsigned char> image;
        unsigned int width, height;
        auto err = lodepng::decode(image, width, height, CS488Window::getAssetFilePath("terrain.png").c_str());

        if ( err != 0 ) {
            throw "Texture loading error, texture img file: "+img_file;
        }

        glGenTextures(1, &tex);
        texCounter = Texture::counter;
        Texture::counter++;

        glActiveTexture( GL_TEXTURE0+texCounter );
        glBindTexture( GL_TEXTURE_2D, tex );

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);
    }
    CHECK_GL_ERRORS;
}

bool Texture::bind( GLint uniform ) {
    {
        glActiveTexture( GL_TEXTURE0 + texCounter );
        glBindTexture( GL_TEXTURE_2D, tex );
        glUniform1i( uniform, texCounter );
    }
    CHECK_GL_ERRORS;
}

Texture::~Texture() {
    glDeleteTextures(1, &tex);
}
