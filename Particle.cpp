#include "Particle.hpp"
#include "Player.hpp"
#include "Util.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
using namespace glm;
using namespace std;

int ParticleSystem::findFreeParticle() {
    int min_idx = 0;
    float min_life_left = particles[0].lifespan - particles[0].age;

    for ( int i = 0; i < MAX_NUM_PARTICLES; i++ ) {
        if ( !particles[i].alive() )
            return i;

        float life_left = particles[i].lifespan - particles[i].age;

        if ( life_left < min_life_left ) {
            min_idx = i;
            min_life_left = life_left;
        }
    }
    return min_idx;
}

void ParticleSystem::addParticle( glm::vec3 pos, glm::vec3 v ) {
    int idx = findFreeParticle();
    particles[idx].reset( pos, v );
}

void ParticleSystem::render() {
    shader->enable();
    {
        glBindVertexArray( vao );
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        for ( int i = 0; i < MAX_NUM_PARTICLES; i++ ) {
            if( particles[i].alive() ) {
                mat4 S = scale( mat4(), vec3( .1, .1, .1) );
                mat4 T = translate(mat4(), vec3(particles[i].pos));
                mat4 M = T*S;
                glUniformMatrix4fv( shader->M, 1, GL_FALSE, value_ptr(M));
                glDrawElements( GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
            }
        }
        glBindVertexArray( 0 );
        glBindBuffer( GL_ARRAY_BUFFER, 0 );
        CHECK_GL_ERRORS;
    }
    shader->disable();
}

void ParticleSystem::init() {
    shader = ParticleShader::getInstance();

    {
        float cube_verts[] = {
            -.5f, -.5f, -.5f, // y=0, top left
            .5f, -.5f, -.5f, // y=0, top right
            .5f, -.5f, .5f, // y=0, bot right
            -.5f, -.5f, .5f, // y=0, bot left

            -.5f, .5f, -.5f, // y=1, top left
            .5f, .5f, -.5f, // y=1, top right
            .5f, .5f, .5f, // y=1, bot right
            -.5f, .5f, .5f  // y=1, bot left
        };
        GLuint indices[] = {
            0, 1, 2, //bottom square
            2, 3, 0,

            6, 5, 4, //top square
            4, 7, 6,

            7, 4, 0, //side square 1
            0, 3, 7,

            1, 5, 6, //side square 2
            6, 2, 1,

            5, 1, 0, //side square 3
            0, 4, 5,

            3, 2, 6, //side square 4
            6, 7, 3,
        };
        // Set up new VAO "environment" for cubes
        glGenVertexArrays( 1, &vao );
        glBindVertexArray( vao );

        // Create the cube vertex buffer
        glGenBuffers( 1, &vbo );
        glBindBuffer( GL_ARRAY_BUFFER, vbo );
        glBufferData( GL_ARRAY_BUFFER, sizeof(cube_verts), cube_verts, GL_STATIC_DRAW );

        // Create the cube vertex element buffer
        glGenBuffers( 1, &ebo );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ebo );
        glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW );


        // Specify the means of extracting the position values properly.
        glEnableVertexAttribArray( shader->posAttrib );
        glVertexAttribPointer( shader->posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr );

        glBindVertexArray( 0 );
        glBindBuffer( GL_ARRAY_BUFFER, 0 );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
        CHECK_GL_ERRORS;
    }
}

void ParticleSystem::updateUniform( const glm::mat4 &P, const glm::mat4 &V ) {
    shader->enable();
    glUniformMatrix4fv( shader->P, 1, GL_FALSE, value_ptr(P));
    glUniformMatrix4fv( shader->V, 1, GL_FALSE, value_ptr(V));
    shader->disable();
}

void ParticleSystem::update( float delta_time ) {
    for ( int i = 0; i < MAX_NUM_PARTICLES; i++ ) {
        if( particles[i].alive() ) {
            particles[i].update( delta_time );
        }
    }
}

Particle::Particle() :
    age( lifespan+1 ) {}


void Particle::reset( glm::vec3 pos, glm::vec3 v ) {
    this->pos = pos;
    this->v = v;
    this->age = 0;
}

bool Particle::alive() {
    return age <= lifespan;
}

void Particle::update( float delta_time ) {
    if ( age <= lifespan ) {
        v.y -= GRAVITY * delta_time;
        pos += delta_time * v;
        age += delta_time;
    }
}
