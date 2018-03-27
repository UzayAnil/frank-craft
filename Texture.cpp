#include "Texture.hpp"
#include "Util.hpp"
#include "cs488-framework/GlErrorCheck.hpp"
#include <iostream>
#include <lodepng/lodepng.h>
using namespace std;

unsigned int Texture::counter = 0;

Texture::Texture( string img_file ) {
    {
        std::vector<unsigned char> image;
        unsigned int width, height;
        auto err = lodepng::decode(image, width, height, getAssetFilePath(img_file.c_str()));

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

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &(image[0]));
    }
    type = TEX2D;
    CHECK_GL_ERRORS;
}

Texture::Texture( string img_file[6] ) {
    {
        std::vector<unsigned char> image[6];
        unsigned int width[6];
        unsigned int height[6];

        for ( int i = 0; i < 6; i++ ) {
            auto err = lodepng::decode(image[i], width[i], height[i], getAssetFilePath(img_file[i].c_str()));

            if ( err != 0 ) {
                throw "Texture loading error, texture img file: "+img_file[i];
            }
        }

        glGenTextures(1, &tex);
        texCounter = Texture::counter;
        Texture::counter++;

        glActiveTexture( GL_TEXTURE0+texCounter );
        glBindTexture( GL_TEXTURE_CUBE_MAP, tex );


        for ( int i = 0; i < 6; i++ ) {
            glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, 0, GL_RGBA, width[i], height[i], 0, GL_RGBA, GL_UNSIGNED_BYTE, &(image[i][0]) );
        }

        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
        glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    }
    type = TEXCUBE;
    CHECK_GL_ERRORS;
}
Texture::Texture( int width, int height, Texture::Type type ) {
    // just doing 2d shadow type for now
    if ( type == TEXDEPTH ) {
        glGenTextures(1, &tex);
        texCounter = Texture::counter;
        Texture::counter++;

        glActiveTexture( GL_TEXTURE0+texCounter );
        glBindTexture( GL_TEXTURE_2D, tex );

        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        this->type = TEXDEPTH;
        this->width = width;
        this->height = height;
    } else {
        throw "invalid type";
    }
    CHECK_GL_ERRORS;
}

bool Texture::bind( GLint uniform ) {
    {
        glActiveTexture( GL_TEXTURE0 + texCounter );

        if ( type == TEX2D ) glBindTexture( GL_TEXTURE_2D, tex );
        else if ( type == TEXCUBE ) glBindTexture( GL_TEXTURE_CUBE_MAP, tex );

        glUniform1i( uniform, texCounter );
    }
    CHECK_GL_ERRORS;
}

Texture::~Texture() {
    glDeleteTextures(1, &tex);
}
