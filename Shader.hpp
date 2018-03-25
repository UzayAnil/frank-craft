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

class PuppetShader : public Shader {
public:

    static PuppetShader* getInstance();

    GLint P;
    GLint MV;
    GLint normMatrixAttrib;

    GLint posAttrib;
    GLint normAttrib;

    GLint lightPosAttrib;
    GLint lightRgbAttrib;
    GLint ambientAttrib;

    GLint kdAttrib;
    GLint ksAttrib;
    GLint shineAttrib;

private:
    PuppetShader( std::string vert_path, std::string frag_path );
};

class CubeShader : public Shader {
public:

    static CubeShader* getInstance();

    GLint P;
    GLint V;
    GLint posAttrib;
    GLint texAttrib;

private:
    CubeShader( std::string vert_path, std::string frag_path );
};

class ParticleShader : public Shader {
public:

    static ParticleShader* getInstance();

    GLint P;
    GLint V;
    GLint M;
    GLint posAttrib;

private:
    ParticleShader( std::string vert_path, std::string frag_path );
};

class SkyboxShader : public Shader {
public:

    static SkyboxShader* getInstance();

    GLint P;
    GLint V;
    GLint posAttrib;
    GLint texAttrib;

private:
    SkyboxShader( std::string vert_path, std::string frag_path );
};
