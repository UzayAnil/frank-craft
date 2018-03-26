#include "Shader.hpp"
#include "Util.hpp"
#include <iostream>
using namespace std;

CubeShader* CubeShader::getInstance() {
    static CubeShader cube_shader(
            getAssetFilePath("CubeVertexShader.vs"),
            getAssetFilePath("CubeFragmentShader.fs")
        );
    return &cube_shader;
}

CubeShader::CubeShader( string vert_path, string frag_path )
        : Shader( vert_path, frag_path ) {
    m_shader.enable();
    P = m_shader.getUniformLocation("P");
    V = m_shader.getUniformLocation("V");
    posAttrib = m_shader.getAttribLocation("pos");
    m_shader.disable();
}

ChunkShader* ChunkShader::getInstance() {
    static ChunkShader cube_shader(
            getAssetFilePath("ChunkVertexShader.vs"),
            getAssetFilePath("ChunkFragmentShader.fs")
        );
    return &cube_shader;
}

ChunkShader::ChunkShader( string vert_path, string frag_path )
        : Shader( vert_path, frag_path ) {
    m_shader.enable();
    P = m_shader.getUniformLocation("P");
    V = m_shader.getUniformLocation("V");
    M = m_shader.getUniformLocation("M");
    posAttrib = m_shader.getAttribLocation("pos");
    texAttrib = m_shader.getUniformLocation("tex");
    m_shader.disable();
}

PuppetShader* PuppetShader::getInstance() {
    static PuppetShader puppet_shader(
            getAssetFilePath("PuppetVertexShader.vs"),
            getAssetFilePath("PuppetFragmentShader.fs")
        );
    return &puppet_shader;
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

        envMapTexAttrib = m_shader.getUniformLocation("envMapTex");
    }

    m_shader.disable();
}

ParticleShader* ParticleShader::getInstance() {
    static ParticleShader particle_shader(
            getAssetFilePath("ParticleVertexShader.vs"),
            getAssetFilePath("ParticleFragmentShader.fs")
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

SkyboxShader* SkyboxShader::getInstance() {
    static SkyboxShader skybox_shader(
            getAssetFilePath("SkyboxVertexShader.vs"),
            getAssetFilePath("SkyboxFragmentShader.fs")
        );
    return &skybox_shader;
}

SkyboxShader::SkyboxShader( string vert_path, string frag_path )
        : Shader( vert_path, frag_path ) {
    m_shader.enable();
    P = m_shader.getUniformLocation("P");
    V = m_shader.getUniformLocation("V");
    posAttrib = m_shader.getAttribLocation("pos");
    texAttrib = m_shader.getUniformLocation("tex");
    m_shader.disable();
}
