#pragma once
#include <glm/glm.hpp>
#include "cs488-framework/OpenGLImport.hpp"
#include "Shader.hpp"

class Cube {
public:
    Cube( const glm::vec3 &min, const glm::vec3 &max, bool wireframe );
    void init();
    void updateUniform( const glm::mat4 &P, const glm::mat4 &V );
    void render();

public:
    glm::vec3 min;
    glm::vec3 max;

private:
    bool wireframe;

    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    CubeShader *shader;
};
