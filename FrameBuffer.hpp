#pragma once

#include <glm/glm.hpp>
#include "cs488-framework/OpenGLImport.hpp"
#include "Texture.hpp"

class FrameBuffer {
public:
    FrameBuffer( Texture* tex );
    ~FrameBuffer();

    void bind();
    void unbind();

private:
    Texture *tex;
    GLuint fbo;
};
