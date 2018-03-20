#pragma once

#include "cs488-framework/ShaderProgram.hpp"
#include "cs488-framework/CS488Window.hpp"

#include <string>
#include <glm/glm.hpp>
#include <iostream>

class Shader {
public:
    Shader(std::string vert_path, std::string frag_path ) {
        m_shader.generateProgramObject();
        m_shader.attachVertexShader( vert_path.c_str() );
        m_shader.attachFragmentShader( frag_path.c_str() );
        m_shader.link();
    }
    void enable() const { m_shader.enable(); };
    void disable() const { m_shader.disable(); };

protected:
    ShaderProgram m_shader;
};

class CubeShader : public Shader {
public:
    GLint P;
    GLint V;
    GLint texture;

    GLint posAttrib;
    GLint texAttrib;

    CubeShader( std::string vert_path, std::string frag_path )
        : Shader( vert_path, frag_path ) {

        m_shader.enable();
        P = m_shader.getUniformLocation("P");
        V = m_shader.getUniformLocation("V");
        posAttrib = m_shader.getAttribLocation("pos");
        texAttrib = m_shader.getUniformLocation("tex");
        m_shader.disable();
    }
};
