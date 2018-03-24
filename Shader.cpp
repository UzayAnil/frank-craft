#include "cs488-framework/CS488Window.hpp"
#include "Shader.hpp"
#include <iostream>
using namespace std;

CubeShader* CubeShader::getInstance() {
    static CubeShader cube_shader(
            CS488Window::getAssetFilePath("CubeVertexShader.vs"),
            CS488Window::getAssetFilePath("CubeFragmentShader.fs")
        );
    return &cube_shader;
}

PuppetShader* PuppetShader::getInstance() {
    static PuppetShader puppet_shader(
            CS488Window::getAssetFilePath("VertexShader.vs"),
            CS488Window::getAssetFilePath("FragmentShader.fs")
        );
    return &puppet_shader;
}

CubeShader::CubeShader( string vert_path, string frag_path )
        : Shader( vert_path, frag_path ) {
    m_shader.enable();
    P = m_shader.getUniformLocation("P");
    V = m_shader.getUniformLocation("V");
    posAttrib = m_shader.getAttribLocation("pos");
    texAttrib = m_shader.getUniformLocation("tex");
    m_shader.disable();
}

PuppetShader::PuppetShader( string vert_path, string frag_path )
    : Shader( vert_path, frag_path ) {

    m_shader.enable();

    {
        P = m_shader.getUniformLocation("Perspective");
        MV = m_shader.getUniformLocation("ModelView");
        normMatrixAttrib = m_shader.getUniformLocation("NormalMatrix");

        posAttrib = m_shader.getAttribLocation("position");
        normAttrib = m_shader.getAttribLocation("normal");

        lightPosAttrib = m_shader.getUniformLocation("light.position");
        lightRgbAttrib = m_shader.getUniformLocation("light.rgbIntensity");
        ambientAttrib = m_shader.getUniformLocation("ambientIntensity");

        kdAttrib = m_shader.getUniformLocation("material.kd");
        ksAttrib = m_shader.getUniformLocation("material.ks");
        shineAttrib = m_shader.getUniformLocation("material.shininess");
    }

    m_shader.disable();
}

ParticleShader* ParticleShader::getInstance() {
    static ParticleShader particle_shader(
            CS488Window::getAssetFilePath("ParticleVertexShader.vs"),
            CS488Window::getAssetFilePath("ParticleFragmentShader.fs")
        );
    return &particle_shader;
}

ParticleShader::ParticleShader( string vert_path, string frag_path )
        : Shader( vert_path, frag_path ) {
    m_shader.enable();
    P = m_shader.getUniformLocation("P");
    V = m_shader.getUniformLocation("V");
    M = m_shader.getUniformLocation("M");
    posAttrib = m_shader.getAttribLocation("pos");
    m_shader.disable();
}
