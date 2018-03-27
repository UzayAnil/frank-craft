#include "FrameBuffer.hpp"
#include "cs488-framework/GlErrorCheck.hpp"

FrameBuffer::FrameBuffer(Texture* tex):
        tex( tex ) {

    {
        glGenFramebuffers( 1, &fbo );
        glBindFramebuffer( GL_FRAMEBUFFER, fbo );

        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, tex->tex, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            throw "creation of frame buffer failed ";

        glBindFramebuffer( GL_FRAMEBUFFER, 0 );
    }
    CHECK_GL_ERRORS;
}

FrameBuffer::~FrameBuffer() {
    glDeleteFramebuffers( 1, &fbo );
    delete tex;
}

void FrameBuffer::bind() {
    glBindFramebuffer( GL_FRAMEBUFFER, fbo );
}

void FrameBuffer::unbind() {
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
    CHECK_GL_ERRORS;
}
