#pragma once
#include <glm/glm.hpp>
#include "cs488-framework/OpenGLImport.hpp"
#include "Shader.hpp"

class Particle {
public:
    Particle();
    void update( float delta_time );
    void reset( glm::vec3 pos, glm::vec3 v );
    bool alive();

    static constexpr float size = 0.1;
    static constexpr float lifespan = 5;

    float age;
    float rotation;

    glm::vec3 pos;
    glm::vec3 v;

};

class ParticleSystem {
public:
    void init();
    void update( float delta_time );
    void render();
    void addParticle( glm::vec3 pos, glm::vec3 v );
    void updateUniform( const glm::mat4 &P, const glm::mat4 &V );


private:
    int findFreeParticle();

private:
    static const int MAX_NUM_PARTICLES = 4096;
    Particle particles[MAX_NUM_PARTICLES];
    ParticleShader *shader;
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
};
