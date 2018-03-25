#pragma once
#include "cs488-framework/OpenGLImport.hpp"
#include "Texture.hpp"
#include "Shader.hpp"
#include <glm/glm.hpp>

class Skybox {
public:
    void init();
    void updateUniform( glm::mat4 P, glm::mat4 V );
    void render();

private:
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    SkyboxShader *shader;
    Texture *skyboxTex;
};
